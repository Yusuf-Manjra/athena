#include "tauRecTools/TauJetRNNEvaluator.h"

#include <algorithm>

#include "PathResolver/PathResolver.h"
#include "tauRecTools/TauJetRNN.h"


TauJetRNNEvaluator::TauJetRNNEvaluator(const std::string &name)
    : TauRecToolBase(name), m_net_1p(nullptr), m_net_3p(nullptr) {
    // Network weight files for 1- and 3-prong taus
    // If the filename is an empty string a default value is decorated
    declareProperty("NetworkFile1P", m_weightfile_1p = "");
    declareProperty("NetworkFile3P", m_weightfile_3p = "");
    declareProperty("OutputVarname", m_output_varname = "RNNJetScore");
    declareProperty("MaxTracks", m_max_tracks = 10);
    declareProperty("MaxClusters", m_max_clusters = 6);

    // Naming conventions for the network weight files:
    declareProperty("InputLayerScalar", m_input_layer_scalar = "scalar");
    declareProperty("InputLayerTracks", m_input_layer_tracks = "tracks");
    declareProperty("InputLayerClusters", m_input_layer_clusters = "clusters");
    declareProperty("OutputLayer", m_output_layer = "rnnid_output");
    declareProperty("OutputNode", m_output_node = "sig_prob");
}

TauJetRNNEvaluator::~TauJetRNNEvaluator() {}

StatusCode TauJetRNNEvaluator::initialize() {
    ATH_MSG_INFO("Initializing TauJetRNNEvaluator");

    // Use PathResolver to search for the weight files
    if (!m_weightfile_1p.empty()) {
        auto weightfile_1p = find_file(m_weightfile_1p);
        if (weightfile_1p.empty()) {
            ATH_MSG_ERROR("Could not find network weights: "
                          << m_weightfile_1p);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Using network config [1-prong]: " << weightfile_1p);
        }
        m_weightfile_1p = weightfile_1p;
    }

    if (!m_weightfile_3p.empty()) {
        auto weightfile_3p = find_file(m_weightfile_3p);
        if (weightfile_3p.empty()) {
            ATH_MSG_ERROR("Could not find network weights: "
                          << m_weightfile_3p);
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_INFO("Using network config [3-prong]: " << weightfile_3p);
        }
        m_weightfile_3p = weightfile_3p;
    }

    // Set the layer and node names in the weight file
    TauJetRNN::Config config;
    config.input_layer_scalar = m_input_layer_scalar;
    config.input_layer_tracks = m_input_layer_tracks;
    config.input_layer_clusters = m_input_layer_clusters;
    config.output_layer = m_output_layer;
    config.output_node = m_output_node;

    // Load the weights and create the network
    m_net_1p = std::make_unique<TauJetRNN>(m_weightfile_1p, config);
    if (!m_net_1p) {
        ATH_MSG_WARNING("No network configured for 1-prong taus. "
                        "Decorating defaults...");
    }

    m_net_3p = std::make_unique<TauJetRNN>(m_weightfile_3p, config);
    if (!m_net_1p) {
        ATH_MSG_WARNING("No network configured for multi-prong taus. "
                        "Decorating defaults...");
    }

    return StatusCode::SUCCESS;
}

StatusCode TauJetRNNEvaluator::execute(xAOD::TauJet &tau) {
    // Output variable accessor
    const SG::AuxElement::Accessor<float> output(m_output_varname);

    // Set default score and overwrite later
    output(tau) = -1111.0f;

    // Only apply RNN to 1- and multi-prong taus
    const auto nTracksCharged = tau.nTracksCharged();
    if (nTracksCharged == 0) {
        return StatusCode::SUCCESS;
    }

    // Get input objects
    std::vector<const xAOD::TauTrack *> tracks;
    ATH_CHECK(get_tracks(tau, tracks));
    std::vector<const xAOD::CaloCluster *> clusters;
    ATH_CHECK(get_clusters(tau, clusters));

    // Evaluate networks
    if (nTracksCharged == 1 && m_net_1p) {
        output(tau) = m_net_1p->compute(tau, tracks, clusters);
    }

    if (nTracksCharged > 1 && m_net_3p) {
        output(tau) = m_net_3p->compute(tau, tracks, clusters);
    }

    return StatusCode::SUCCESS;
}

TauJetRNN *TauJetRNNEvaluator::get_rnn_1p() {
    return m_net_1p.get();
}

TauJetRNN *TauJetRNNEvaluator::get_rnn_3p() {
    return m_net_3p.get();
}

StatusCode TauJetRNNEvaluator::get_tracks(
    const xAOD::TauJet &tau, std::vector<const xAOD::TauTrack *> &out) {
    auto tracks = tau.allTracks();

    // Sort by descending pt
    auto cmp_pt = [](const xAOD::TauTrack *lhs, const xAOD::TauTrack *rhs) {
        return lhs->pt() > rhs->pt();
    };
    std::sort(tracks.begin(), tracks.end(), cmp_pt);

    // Truncate tracks
    if (tracks.size() > m_max_tracks) {
        tracks.resize(m_max_tracks);
    }
    out = std::move(tracks);

    return StatusCode::SUCCESS;
}

StatusCode TauJetRNNEvaluator::get_clusters(
    const xAOD::TauJet &tau, std::vector<const xAOD::CaloCluster *> &out) {
    std::vector<const xAOD::CaloCluster *> clusters;

    const xAOD::Jet *jet_seed = *tau.jetLink();
    if (!jet_seed) {
        ATH_MSG_ERROR("Tau jet link is invalid.");
        return StatusCode::FAILURE;
    }

    for (const auto jc : jet_seed->getConstituents()) {
        auto cl = dynamic_cast<const xAOD::CaloCluster *>(jc->rawConstituent());
        if (!cl) {
            ATH_MSG_ERROR("Calorimeter cluster is invalid.");
            return StatusCode::FAILURE;
        }
        clusters.push_back(cl);
    }

    // Sort by descending et
    auto et_cmp = [](const xAOD::CaloCluster *lhs,
                     const xAOD::CaloCluster *rhs) {
        return lhs->et() > rhs->et();
    };
    std::sort(clusters.begin(), clusters.end(), et_cmp);

    // Truncate clusters
    if (clusters.size() > m_max_clusters) {
        clusters.resize(m_max_clusters);
    }
    out = std::move(clusters);

    return StatusCode::SUCCESS;
}
