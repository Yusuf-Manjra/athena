/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"
#include "AthContainers/ConstDataVector.h"


#include "TopParticleLevel/ParticleLevelRCJetObjectLoader.h"
#include "TopEvent/EventTools.h"
#include "TopParticleLevel/ParticleLevelEvent.h"
#include "TopConfiguration/TopConfig.h"

#include "fastjet/ClusterSequence.hh"
#include <fastjet/contrib/Nsubjettiness.hh>
#include <fastjet/contrib/EnergyCorrelator.hh>
#include "JetSubStructureUtils/Qw.h"
#include "JetSubStructureUtils/KtSplittingScale.h"
#include "JetSubStructureUtils/EnergyCorrelatorGeneralized.h"
#include "JetSubStructureUtils/EnergyCorrelator.h"

ParticleLevelRCJetObjectLoader::ParticleLevelRCJetObjectLoader(const std::shared_ptr<top::TopConfig>& cfg) : asg::
   AsgTool("ParticleLevelRCJetObjectLoader"), m_config(cfg) {
  m_ptcut = 0.;
  m_etamax = 0.;
  m_inputJetPtMin = 0.;
  m_inputJetEtaMax = 999.;
  m_trim = 0.;
  m_radius = 0.;
  m_treeName = "particleLevel";
  m_InJetContainerBase = "AntiKt4TruthWZJets_RC";
  m_OutJetContainerBase = "AntiKt10RCTrim";
  m_InputJetContainer = "AntiKt4TruthWZJets_RC";
  m_OutputJetContainer = "AntiKt10RCTrim";
  m_name = "";
  m_useJSS = false;
  m_jet_def_rebuild = nullptr;
  m_nSub1_beta1 = nullptr;
  m_nSub2_beta1 = nullptr;
  m_nSub3_beta1 = nullptr;
  m_ECF1 = nullptr;
  m_ECF2 = nullptr;
  m_ECF3 = nullptr;
  m_split12 = nullptr;
  m_split23 = nullptr;
  m_qw = nullptr;
  m_gECF332 = nullptr;
  m_gECF461 = nullptr;
  m_gECF322 = nullptr;
  m_gECF331 = nullptr;
  m_gECF422 = nullptr;
  m_gECF441 = nullptr;
  m_gECF212 = nullptr;
  m_gECF321 = nullptr;
  m_gECF311 = nullptr;


  declareProperty("VarRCjets", m_VarRCjets = false);
  declareProperty("VarRCjets_rho", m_VarRCjets_rho = "");
  declareProperty("VarRCjets_mass_scale", m_VarRCjets_mass_scale = "");
}

ParticleLevelRCJetObjectLoader::~ParticleLevelRCJetObjectLoader() {}


StatusCode ParticleLevelRCJetObjectLoader::initialize() {
  /* Initialize the re-clustered jets */
  ATH_MSG_INFO(" Initializing particle level Re-clustered jets ");

  m_name = m_VarRCjets_rho + m_VarRCjets_mass_scale;


  if (m_VarRCjets) {
    m_ptcut = m_config->VarRCJetPtcut();        // 100 GeV
    m_etamax = m_config->VarRCJetEtacut();       // 2.5
    m_trim = m_config->VarRCJetTrimcut();      // 0.05 (5% jet pT)
    m_radius = m_config->VarRCJetMaxRadius(); // 1.2  (min=0.4)
    m_minradius = 0.4;                                // 0.4 default (until we have smaller jets!)
    std::string original_rho(m_VarRCjets_rho);
    std::replace(original_rho.begin(), original_rho.end(), '_', '.');
    float rho = std::stof(original_rho);
    float m_scale = mass_scales.at(m_VarRCjets_mass_scale);
    m_massscale = rho * m_scale * 1e-3;

    m_useJSS = m_config->useVarRCJetSubstructure();
    m_substructureVariables = m_config->VarRCJetSubstructureVariables();
  } else {
    m_ptcut = m_config->RCJetPtcut();     // for initialize [GeV] & passSelection
    m_etamax = m_config->RCJetEtacut();    // for passSelection
    m_trim = m_config->RCJetTrimcut();   // for initialize
    m_radius = m_config->RCJetRadius(); // for initialize
    m_minradius = -1.0;
    m_massscale = -1.0;

    m_useJSS = m_config->useRCJetSubstructure();
    m_substructureVariables = m_config->rcJetSubstructureVariables();
  }

  m_inputJetPtMin = m_config->RCInputJetPtMin();
  m_inputJetEtaMax = m_config->RCInputJetEtaMax();



  m_InputJetContainer = m_InJetContainerBase;
  m_OutputJetContainer = m_OutJetContainerBase + m_name;

  // build a jet re-clustering tool for each case
  m_jetReclusteringTool = std::make_shared<JetReclusteringTool>(m_treeName + m_name);
  top::check(m_jetReclusteringTool->setProperty("InputJetContainer",
                                                m_InputJetContainer),
             "Failed inputjetcontainer initialize reclustering tool");
  top::check(m_jetReclusteringTool->setProperty("OutputJetContainer",
                                                m_OutputJetContainer),
             "Failed outputjetcontainer initialize reclustering tool");
  top::check(m_jetReclusteringTool->setProperty("ReclusterRadius",
                                                m_radius), "Failed re-clustering radius initialize reclustering tool");
  top::check(m_jetReclusteringTool->setProperty("RCJetPtMin",
                                                m_ptcut * 1e-3), "Failed ptmin [GeV] initialize reclustering tool");
  top::check(m_jetReclusteringTool->setProperty("InputJetPtMin", m_inputJetPtMin * 1e-3), "Failed InputJetPtMin [GeV] initialize reclustering tool");
  top::check(m_jetReclusteringTool->setProperty("TrimPtFrac",
                                                m_trim), "Failed pT fraction initialize reclustering tool");
  top::check(m_jetReclusteringTool->setProperty("VariableRMinRadius",
                                                m_minradius), "Failed VarRC min radius initialize reclustering tool");
  top::check(m_jetReclusteringTool->setProperty("VariableRMassScale",
                                                m_massscale), "Failed VarRC mass scale initialize reclustering tool");
  top::check(m_jetReclusteringTool->initialize(), "Failed to initialize reclustering tool");

  if (m_useJSS) {
    ATH_MSG_INFO("Calculating RCJet Substructure");

    // Setup a bunch of FastJet stuff
    //define the type of jets you will build (http://fastjet.fr/repo/doxygen-3.0.3/classfastjet_1_1JetDefinition.html)
    m_jet_def_rebuild = std::make_unique<fastjet::JetDefinition>(fastjet::antikt_algorithm, 10.0, fastjet::E_scheme,
                                                                 fastjet::Best);
    //Substructure tool definitions
    m_nSub1_beta1 = std::make_unique<fastjet::contrib::Nsubjettiness>(1,
                                                                      fastjet::contrib::OnePass_WTA_KT_Axes(),
                                                                      fastjet::contrib::UnnormalizedMeasure(1.0));
    m_nSub2_beta1 = std::make_unique<fastjet::contrib::Nsubjettiness>(2,
                                                                      fastjet::contrib::OnePass_WTA_KT_Axes(),
                                                                      fastjet::contrib::UnnormalizedMeasure(1.0));
    m_nSub3_beta1 = std::make_unique<fastjet::contrib::Nsubjettiness>(3,
                                                                      fastjet::contrib::OnePass_WTA_KT_Axes(),
                                                                      fastjet::contrib::UnnormalizedMeasure(1.0));


    m_split12 = std::make_unique<JetSubStructureUtils::KtSplittingScale>(1);
    m_split23 = std::make_unique<JetSubStructureUtils::KtSplittingScale>(2);

    m_qw = std::make_unique<JetSubStructureUtils::Qw>();
    
    m_ECF1 = std::make_unique<fastjet::contrib::EnergyCorrelator>(1, 1.0, fastjet::contrib::EnergyCorrelator::pt_R);
    m_ECF2 = std::make_unique<fastjet::contrib::EnergyCorrelator>(2, 1.0, fastjet::contrib::EnergyCorrelator::pt_R);
    m_ECF3 = std::make_unique<fastjet::contrib::EnergyCorrelator>(3, 1.0, fastjet::contrib::EnergyCorrelator::pt_R);

    m_gECF332 = std::make_unique<JetSubStructureUtils::EnergyCorrelatorGeneralized>(3, 3, 2,
                                                                                    JetSubStructureUtils::EnergyCorrelator::pt_R);
    m_gECF461 = std::make_unique<JetSubStructureUtils::EnergyCorrelatorGeneralized>(6, 4, 1,
                                                                                    JetSubStructureUtils::EnergyCorrelator::pt_R);
    m_gECF322 = std::make_unique<JetSubStructureUtils::EnergyCorrelatorGeneralized>(2, 3, 2,
                                                                                    JetSubStructureUtils::EnergyCorrelator::pt_R);
    m_gECF331 = std::make_unique<JetSubStructureUtils::EnergyCorrelatorGeneralized>(3, 3, 1,
                                                                                    JetSubStructureUtils::EnergyCorrelator::pt_R);
    m_gECF422 = std::make_unique<JetSubStructureUtils::EnergyCorrelatorGeneralized>(2, 4, 2,
                                                                                    JetSubStructureUtils::EnergyCorrelator::pt_R);
    m_gECF441 = std::make_unique<JetSubStructureUtils::EnergyCorrelatorGeneralized>(4, 4, 1,
                                                                                    JetSubStructureUtils::EnergyCorrelator::pt_R);
    m_gECF212 = std::make_unique<JetSubStructureUtils::EnergyCorrelatorGeneralized>(1, 2, 2,
                                                                                    JetSubStructureUtils::EnergyCorrelator::pt_R);
    m_gECF321 = std::make_unique<JetSubStructureUtils::EnergyCorrelatorGeneralized>(2, 3, 1,
                                                                                    JetSubStructureUtils::EnergyCorrelator::pt_R);
    m_gECF311 = std::make_unique<JetSubStructureUtils::EnergyCorrelatorGeneralized>(1, 3, 1,
                                                                                    JetSubStructureUtils::EnergyCorrelator::pt_R);
  }
  ATH_MSG_INFO(" Re-clustered jets initialized ");

  return StatusCode::SUCCESS;
} // end initialize()

StatusCode ParticleLevelRCJetObjectLoader::execute(const top::ParticleLevelEvent& plEvent) {
  /*
     Make the jet container (if necessary) and execute the re-clustering tool
      https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/TopPhys/xAOD/TopEvent/trunk/Root/TopEventMaker.cxx#L31
   */

  // -- Save the jet container to the TStore (only if it doesn't already exist!)
  // -- Then, we can access it with the re-clustering tool further down
  if (!evtStore()->contains<xAOD::JetContainer>(m_InputJetContainer)) {
    auto rcJetInputs = std::make_unique< ConstDataVector< xAOD::JetContainer > >(SG::VIEW_ELEMENTS);
    for(const xAOD::Jet* jet : *plEvent.m_jets) {
      if(jet->pt() < m_inputJetPtMin || std::abs(jet->eta()) > m_inputJetEtaMax) continue;
      rcJetInputs->push_back(jet);
    }
    top::check(evtStore()->tds()->record(std::move(rcJetInputs), m_InputJetContainer),
	       "Failed to put jets in TStore for re-clustering");
  } // end if jet container exists

  



  // --- EXECUTE --- //
  // only execute if the jet container doesn't exist
  // (do not re-make the 'nominal' jet container over & over again!)
  if (!evtStore()->contains<xAOD::JetContainer>(m_OutputJetContainer)) {
    m_jetReclusteringTool->execute();

    const xAOD::JetContainer* myJets(nullptr);
    top::check(evtStore()->retrieve(myJets, m_OutputJetContainer), "Failed to retrieve particle RC JetContainer");

    for (auto rcjet : *myJets) {
      rcjet->auxdecor<bool>("PassedSelection") = passSelection(*rcjet);
    }

    if (m_useJSS) {
      static const SG::AuxElement::ConstAccessor<bool> passedSelection("PassedSelection");

      for (auto rcjet : *myJets) {
        if (!passedSelection(*rcjet)) continue; // Calculate JSS only if passed object selection


        std::vector<fastjet::PseudoJet> clusters;
        clusters.clear();

        for (auto subjet : rcjet->getConstituents()) {
          const xAOD::Jet* subjet_raw = static_cast<const xAOD::Jet*>(subjet->rawConstituent());


          for (auto clus_itr : subjet_raw->getConstituents()) {
            
            TLorentzVector temp_p4;
            temp_p4.SetPtEtaPhiM(clus_itr->pt(), clus_itr->eta(), clus_itr->phi(), clus_itr->m());


            // Only in case AntiKt4EMPFlowJets are used include in the substructure only the charged component of the
            // substructure
            if (m_config->sgKeyJetsTDS(m_config->nominalHashValue(),
                                       false).find("AntiKt4EMPFlowJets") != std::string::npos) {
              
              const xAOD::TruthParticle* tp = dynamic_cast<const xAOD::TruthParticle*>(clus_itr->rawConstituent());  
            
              if( tp == nullptr) 
                  continue;
            
              // Do not use charged particles
              if (tp->charge() == 0) continue;
              
              //Apply same track selection used at reco-level
              if ((clus_itr->pt() < m_config->ghostTrackspT()) || ( std::abs(clus_itr->eta()) > 2.5 ) )  continue;
            }

            clusters.push_back(fastjet::PseudoJet(temp_p4.Px(), temp_p4.Py(), temp_p4.Pz(), temp_p4.E()));
          }
        }

        if (clusters.size() != 0) {
          fastjet::ClusterSequence clust_seq_rebuild = fastjet::ClusterSequence(clusters, *m_jet_def_rebuild);
          std::vector<fastjet::PseudoJet> my_pjets = fastjet::sorted_by_pt(clust_seq_rebuild.inclusive_jets(0.0));

          fastjet::PseudoJet correctedJet;
          correctedJet = my_pjets[0];
          if (my_pjets.size() > 1)  ATH_MSG_WARNING("WARNING::the particle-level jet is split and the RC-jets substructure makes no sense");
          

          // Now finally we can calculate some substructure!
	  auto it1 = std::begin(m_substructureVariables);
	  auto it2 = std::end(m_substructureVariables);
	  
	  float tau1(-999.),tau2(-999.),tau3(-999.);
	  if(std::find(it1,it2,"Tau1_clstr")!=it2) rcjet->auxdecor<float>("Tau1_clstr") = tau1 = m_nSub1_beta1->result(correctedJet);
	  if(std::find(it1,it2,"Tau2_clstr")!=it2) rcjet->auxdecor<float>("Tau2_clstr") = tau2 = m_nSub2_beta1->result(correctedJet);
	  if(std::find(it1,it2,"Tau3_clstr")!=it2) rcjet->auxdecor<float>("Tau3_clstr") = tau3 = m_nSub3_beta1->result(correctedJet);

	  
	  if(std::find(it1,it2,"Tau21_clstr")!=it2) {
	    if(tau1<0.) tau1 = m_nSub1_beta1->result(correctedJet);
	    if(tau2<0.) tau2 = m_nSub2_beta1->result(correctedJet);
	    rcjet->auxdecor<float>("Tau21_clstr") = std::abs(tau1) > 1e-8 ? tau2/tau1 : -999.;
	  }
	  if(std::find(it1,it2,"Tau32_clstr")!=it2) {
	    if(tau3<0.) tau3 = m_nSub3_beta1->result(correctedJet);
	    if(tau2<0.) tau2 = m_nSub2_beta1->result(correctedJet);
	    rcjet->auxdecor<float>("Tau32_clstr") = std::abs(tau2) > 1e-8 ? tau3/tau2 : -999.;
	  }

	  if(std::find(it1,it2,"d12_clstr")!=it2) rcjet->auxdecor<float>("d12_clstr") = m_split12->result(correctedJet);      
	  if(std::find(it1,it2,"d23_clstr")!=it2) rcjet->auxdecor<float>("d23_clstr") = m_split23->result(correctedJet);      
	  if(std::find(it1,it2,"Qw_clstr")!=it2) rcjet->auxdecor<float>("Qw_clstr") = m_qw->result(correctedJet);      
	  
	  
	  float vECF1(-999.),vECF2(-999.),vECF3(-999.);
	  if(std::find(it1,it2,"ECF1_clstr")!=it2) rcjet->auxdecor<float>("ECF1_clstr") = vECF1 = m_ECF1->result(correctedJet);      
	  if(std::find(it1,it2,"ECF2_clstr")!=it2) rcjet->auxdecor<float>("ECF2_clstr") = vECF2 = m_ECF2->result(correctedJet);      
	  if(std::find(it1,it2,"ECF3_clstr")!=it2) rcjet->auxdecor<float>("ECF3_clstr") = vECF3 = m_ECF3->result(correctedJet);      
	  
	  if(std::find(it1,it2,"D2_clstr")!=it2) {
	    if(vECF1<0.) vECF1 = m_ECF1->result(correctedJet);
	    if(vECF2<0.) vECF2 = m_ECF2->result(correctedJet);
	    if(vECF3<0.) vECF3 = m_ECF3->result(correctedJet);
	    rcjet->auxdecor<float>("D2_clstr") = std::abs(vECF2) > 1e-8 ? vECF3 * vECF1* vECF1* vECF1 / (vECF2 * vECF2 * vECF2) : -999.;
	  }

	  if(std::find(it1,it2,"nconstituent_clstr")!=it2) rcjet->auxdecor<float>("nconstituent_clstr") = clusters.size();
	  
	  // MlB's t/H discriminators
	  // E = (a*n) / (b*m)
	  // for an ECFG_X_Y_Z, a=Y, n=Z -> dimenionless variable
	  float gECF332(-999.), gECF461(-999.), gECF322(-999.), gECF331(-999.), gECF422(-999.), gECF441(-999.), gECF212(-999.), gECF321(-999.), gECF311(-999.);

	  if(std::find(it1,it2,"gECF332_clstr")!=it2) rcjet->auxdecor<float>("gECF332_clstr") = gECF332 = m_gECF332->result(correctedJet);
	  if(std::find(it1,it2,"gECF461_clstr")!=it2) rcjet->auxdecor<float>("gECF461_clstr") = gECF461 = m_gECF461->result(correctedJet);
	  if(std::find(it1,it2,"gECF322_clstr")!=it2) rcjet->auxdecor<float>("gECF322_clstr") = gECF322 = m_gECF322->result(correctedJet);
	  if(std::find(it1,it2,"gECF331_clstr")!=it2) rcjet->auxdecor<float>("gECF331_clstr") = gECF331 = m_gECF331->result(correctedJet);
	  if(std::find(it1,it2,"gECF422_clstr")!=it2) rcjet->auxdecor<float>("gECF422_clstr") = gECF422 = m_gECF422->result(correctedJet);
	  if(std::find(it1,it2,"gECF441_clstr")!=it2) rcjet->auxdecor<float>("gECF441_clstr") = gECF441 = m_gECF441->result(correctedJet);
	  if(std::find(it1,it2,"gECF212_clstr")!=it2) rcjet->auxdecor<float>("gECF212_clstr") = gECF212 = m_gECF212->result(correctedJet);
	  if(std::find(it1,it2,"gECF321_clstr")!=it2) rcjet->auxdecor<float>("gECF321_clstr") = gECF321 = m_gECF321->result(correctedJet);
	  if(std::find(it1,it2,"gECF311_clstr")!=it2) rcjet->auxdecor<float>("gECF311_clstr") = gECF311 = m_gECF311->result(correctedJet);

	  if(std::find(it1,it2,"L1_clstr")!=it2) {
	    if(gECF212<0.) gECF212 = m_gECF212->result(correctedJet);
	    if(gECF321<0.) gECF321 = m_gECF321->result(correctedJet);
	    rcjet->auxdecor<float>("L1_clstr") = std::abs(gECF212) > 1e-12 ? gECF321 / gECF212 : -999.;
	  }
	  if(std::find(it1,it2,"L2_clstr")!=it2) {
	    if(gECF212<0.) gECF212 = m_gECF212->result(correctedJet);
	    if(gECF331<0.) gECF331 = m_gECF331->result(correctedJet);
	    rcjet->auxdecor<float>("L2_clstr") = std::abs(gECF212) > 1e-12 ? gECF331 / sqrt(gECF212*gECF212*gECF212) : -999.;
	  }
	  if(std::find(it1,it2,"L3_clstr")!=it2) {
	    if(gECF331<0.) gECF331 = m_gECF331->result(correctedJet);
	    if(gECF311<0.) gECF311 = m_gECF311->result(correctedJet);
	    rcjet->auxdecor<float>("L3_clstr") = std::abs(gECF331) > 1e-12 ? gECF311 / pow(gECF331,1./3.) : -999.;
	  }
	  if(std::find(it1,it2,"L4_clstr")!=it2) {
	    if(gECF331<0.) gECF331 = m_gECF331->result(correctedJet);
	    if(gECF322<0.) gECF322 = m_gECF322->result(correctedJet);
	    rcjet->auxdecor<float>("L4_clstr") = std::abs(gECF331) > 1e-12 ? gECF322 / pow(gECF331,4./3.) : -999.;
	  }
	  if(std::find(it1,it2,"L5_clstr")!=it2) {
	    if(gECF441<0.) gECF441 = m_gECF441->result(correctedJet);
	    if(gECF422<0.) gECF422 = m_gECF422->result(correctedJet);
	    rcjet->auxdecor<float>("L5_clstr") = std::abs(gECF441) > 1e-12 ? gECF422/gECF441 : -999.;
	  }
	
	  // lets also store the rebuilt jet incase we need it later
	  if(std::find(it1,it2,"rrcjet_pt")!=it2) rcjet->auxdecor<float>("rrcjet_pt") = correctedJet.pt();
	  if(std::find(it1,it2,"rrcjet_eta")!=it2) rcjet->auxdecor<float>("rrcjet_eta") = correctedJet.eta();
	  if(std::find(it1,it2,"rrcjet_phi")!=it2) rcjet->auxdecor<float>("rrcjet_phi") = correctedJet.phi();
	  if(std::find(it1,it2,"rrcjet_e")!=it2) rcjet->auxdecor<float>("rrcjet_e") = correctedJet.e();
	  
        }//end of check on cluster size
      } // end rcjet loop
    } // //m_useJSS
  } //if (!evtStore()->contains<xAOD::JetContainer>(m_OutputJetContainer))


  //   // }

  return StatusCode::SUCCESS;
} // end execute()

StatusCode ParticleLevelRCJetObjectLoader::finalize() {
  return StatusCode::SUCCESS;
}

bool ParticleLevelRCJetObjectLoader::passSelection(const xAOD::Jet& jet) const {
  /*
     Check if the re-clustered jet passes selection.
     Right now, this only does something for |eta| because
     pT is taken care of in the re-clustering tool.  When
     small-r jet mass is available (calibrated+uncertainties),
     we can cut on that.
   */
  // [pT] calibrated to >~ 22 GeV (23 Jan 2016)
  if (jet.pt() < m_ptcut) return false;

  // [|eta|] calibrated < 2.5
  if (std::abs(jet.eta()) > m_etamax) return false;

  // small-r jet mass not calibrated and no uncertainties

  return true;
}
