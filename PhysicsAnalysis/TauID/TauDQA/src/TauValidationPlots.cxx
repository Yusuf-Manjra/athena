/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "../share/TauValidationPlots.h"

TauValidationPlots::TauValidationPlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):PlotBase(pParent, sDir),
                                                          m_oGeneralTauAllProngsPlots(this, "Matched/", sTauJetContainerName),
													      m_oNewCoreMatchedPlots(this,"Matched/All/", sTauJetContainerName),             // all tau reco, newCore variables
													      m_oHad1ProngPlots(this, "Matched/Tau1P/", sTauJetContainerName),             // tau1P plots : variables for tau ID
													      m_oHad3ProngPlots(this, "Matched/Tau3P/", sTauJetContainerName),             // tau3P plots : variables for tau ID
													      m_oMatchedTauAllProngsPlots(this,"Matched/PFOs/", sTauJetContainerName),
													      m_oMigrationPlots(this,"Matched/Migration/", sTauJetContainerName),             // Migration Matrix
// all tau reco, no match to truth
													      m_oElMatchedParamPlots(this, "Electron/", sTauJetContainerName),     // electron veto variables for electrons matching tau candidates	  
													      m_oElMatchedEVetoPlots(this, "Electron/", sTauJetContainerName),     // electron veto variables for electrons matching tau candidates
													      m_oFakeGeneralTauAllProngsPlots(this,"Fake/", sTauJetContainerName),  // general tau all fake prongs plots
													      m_oNewCoreFakePlots(this,"Fake/All/", sTauJetContainerName),             // all tau reco, newCore variables
													      m_oFakeHad1ProngPlots(this,"Fake/Jet1P/", sTauJetContainerName), 		     // tau1P fake plots : variables for tau ID
													      m_oFakeHad3ProngPlots(this,"Fake/Jet3P/", sTauJetContainerName),  	       // tau3P fake plots : variables for tau ID
													      m_oFakeTauAllProngsPlots(this,"Fake/PFOs/", sTauJetContainerName),             // all tau reco, no match to truth

													      m_oRecoGeneralTauAllProngsPlots(this,"RecTau/All/",sTauJetContainerName),// "recTau_General"),  // general tau all fake prongs plots
													      m_oRecoHad1ProngPlots(this,"RecTau/1P/", sTauJetContainerName),//"recTau_1P"), 		     // tau1P fake plots : variables for tau ID
													      m_oRecoHad3ProngPlots(this,"RecTau/3P/", sTauJetContainerName),// "recTau_3P"),  	       // tau3P fake plots : variables for tau ID
													      m_oRecoTauAllProngsPlots(this,"RecTau/PFOs/", sTauJetContainerName),             // all tau reco, no match to truth
													      m_oNewCorePlots(this,"RecTau/All/", sTauJetContainerName)             // all tau reco, newCore variables
													      
													      
{}	

// no fill method implement in order to let filling logic stay in the ManagedMonitoringTool
