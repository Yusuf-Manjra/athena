/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/// Name    : JetFitterNNTool.h
/// Package : BTagTools
/// Author  : Dan Guest
/// Created : March 2016
///
/// DESCRIPTION:
/// Class to dump a ROOT file with tagger info
///
/////////////////////////////////////////////////////////////////////////////

#include "JetTagTools/TagNtupleDumper.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TTree.h"

#include <utility>
#include <set>

namespace Analysis {

  TagNtupleDumper::TagNtupleDumper(const std::string& name, const std::string& n, const IInterface* p):
    base_class(name, n,p),
    m_hist_svc("THistSvc", name)
  {
    // setup stream
    declareProperty("Stream", m_stream = "FTAG");
  }

  TagNtupleDumper::~TagNtupleDumper() {
    for (auto& pair: m_features) {
      delete pair.second;
    }
    // no idea how the hist service handles trees, do I have to delete them?
    // for (auto& tree: m_trees) {
    //   delete tree.second;
    // }
  }

  StatusCode TagNtupleDumper::initialize() {
    CHECK( m_hist_svc.retrieve() );
    return StatusCode::SUCCESS;
  }

  StatusCode TagNtupleDumper::finalize() { // all taken care of in destructor
    return StatusCode::SUCCESS;
  }
  typedef std::map<std::string,double> var_map;
  void TagNtupleDumper::assignProbability(xAOD::BTagging*,
                                          const var_map &inputs,
                                          const std::string &jetauthor) const
  {
    std::lock_guard<std::mutex> lock (m_mutex);

    TTree* & tree = m_trees[jetauthor];
    if (tree == nullptr) {
      tree = new TTree(jetauthor.c_str(), "who cares");
      if (m_hist_svc->regTree("/" + m_stream + "/" + jetauthor,
                              tree).isFailure()) {
        ATH_MSG_ERROR("Cannot register tree " + m_stream + "/" + jetauthor);
        return;
      }
    }
    for (const auto& in: inputs) {
      const auto idx = std::make_pair(in.first, jetauthor);
      // add this branch if it doesn't exist
      float* & feature = m_features[idx];
      if (feature == nullptr) {
        feature = new float;
        m_trees.at(jetauthor)->Branch(in.first.c_str(), feature);
        ATH_MSG_INFO("Added Branch " + jetauthor + ": " + in.first );
      }
      *feature = in.second;
    }
    // fill tree
    m_trees.at(jetauthor)->Fill();
  } //end assign_probability
}

