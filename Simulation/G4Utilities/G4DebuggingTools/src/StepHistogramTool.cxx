/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"

#include "GaudiKernel/ITHistSvc.h"

#include "StepHistogramTool.h"

namespace G4UA{ 


  StepHistogramTool::StepHistogramTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent):
    ActionToolBaseReport<StepHistogram>(type, name, parent),
    m_config(),
    m_histSvc("THistSvc", name){

    declareProperty("do2DHistograms", m_config.do2DHistograms);
    declareProperty("doGeneralHistograms", m_config.doGeneralHistograms);
  }

  std::unique_ptr<StepHistogram>  StepHistogramTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<StepHistogram>(m_config);
    return std::move(action);
  }

  StatusCode StepHistogramTool::initialize(){
    ATH_CHECK(m_histSvc.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode StepHistogramTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    if(riid == IG4EventActionTool::interfaceID()) {
      *ppvIf = (IG4EventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IG4SteppingActionTool::interfaceID()) {
      *ppvIf = (IG4SteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    if(riid == IG4RunActionTool::interfaceID()) {
      *ppvIf = (IG4RunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<StepHistogram>::queryInterface(riid, ppvIf);
  }

  void StepHistogramTool::BookHistograms(StepHistogram::HistoMapMap_t &hMap, const char* suffix, const char* subfolder)
  {
    for (auto const& x : hMap)
    {
      ATH_MSG_INFO("Currently in volume:\t" << x.first << " got histoMap " << x.second);
      for (auto const& hm : x.second)
      {
        ATH_MSG_INFO("Registering histogram:\t" << hm.first);
        std::ostringstream stringStream;
        stringStream << "/stepHisto/" << subfolder << x.first << "/" << suffix << hm.first;
        if ( m_histSvc->regHist(stringStream.str().c_str(), hm.second).isFailure() ) {
          ATH_MSG_ERROR("Could not register histogram!");
        }
      }
    }    
  }

  StatusCode StepHistogramTool::finalize() {
    ATH_MSG_INFO("Preparing histograms...");

    mergeReports();

    if (m_histSvc) {
      BookHistograms(m_report.histoMapMap_vol_stepSize, "stepLength/", "volumes/");
      BookHistograms(m_report.histoMapMap_vol_stepKineticEnergy, "stepKineticEnergy/", "volumes/");
      BookHistograms(m_report.histoMapMap_vol_postStepKineticEnergy, "postStepKineticEnergy/", "volumes/");
      BookHistograms(m_report.histoMapMap_vol_stepPseudorapidity, "stepPseudorapidity/", "volumes/");
      BookHistograms(m_report.histoMapMap_vol_stepEnergyDeposit, "stepEnergyDeposit/", "volumes/");
      BookHistograms(m_report.histoMapMap_vol_stepEnergyNonIonDeposit, "stepEnergyNonIonDeposit/", "volumes/");
      BookHistograms(m_report.histoMapMap_vol_stepSecondaryKinetic, "stepSecondaryKinetic/", "volumes/");
      BookHistograms(m_report.histoMapMap_vol_trackLengthPerInitialE, "trackLengthPerInitialE/", "volumes/");
      BookHistograms(m_report.histoMapMap_vol_InitialE, "InitialE/", "volumes/");
      BookHistograms(m_report.histoMapMap_vol_numberOfSteps, "numberOfSteps/", "volumes/");
      BookHistograms(m_report.histoMapMap_vol_numberOfStepsPerInitialE, "numberOfStepsPerInitialE/", "volumes/");

      BookHistograms(m_report.histoMapMap_mat_stepSize, "stepLength/", "materials/");
      BookHistograms(m_report.histoMapMap_mat_stepKineticEnergy, "stepKineticEnergy/", "materials/");
      BookHistograms(m_report.histoMapMap_mat_postStepKineticEnergy, "postStepKineticEnergy/", "materials/");
      BookHistograms(m_report.histoMapMap_mat_stepPseudorapidity, "stepPseudorapidity/", "materials/");
      BookHistograms(m_report.histoMapMap_mat_stepEnergyDeposit, "stepEnergyDeposit/", "materials/");
      BookHistograms(m_report.histoMapMap_mat_stepEnergyNonIonDeposit, "stepEnergyNonIonDeposit/", "materials/");
      BookHistograms(m_report.histoMapMap_mat_stepSecondaryKinetic, "stepSecondaryKinetic/", "materials/");
      BookHistograms(m_report.histoMapMap_mat_trackLengthPerInitialE, "trackLengthPerInitialE/", "materials/");
      BookHistograms(m_report.histoMapMap_mat_InitialE, "InitialE/", "materials/");
      BookHistograms(m_report.histoMapMap_mat_numberOfSteps, "numberOfSteps/", "materials/");
      BookHistograms(m_report.histoMapMap_mat_numberOfStepsPerInitialE, "numberOfStepsPerInitialE/", "materials/");

      BookHistograms(m_report.histoMapMap_prc_stepSize, "stepLength/", "processes/");
      BookHistograms(m_report.histoMapMap_prc_stepKineticEnergy, "stepKineticEnergy/", "processes/");
      BookHistograms(m_report.histoMapMap_prc_postStepKineticEnergy, "postStepKineticEnergy/", "processes/");
      BookHistograms(m_report.histoMapMap_prc_stepPseudorapidity, "stepPseudorapidity/", "processes/");
      BookHistograms(m_report.histoMapMap_prc_stepEnergyDeposit, "stepEnergyDeposit/", "processes/");
      BookHistograms(m_report.histoMapMap_prc_stepEnergyNonIonDeposit, "stepEnergyNonIonDeposit/", "processes/");
      BookHistograms(m_report.histoMapMap_prc_stepSecondaryKinetic, "stepSecondaryKinetic/", "processes/");
      BookHistograms(m_report.histoMapMap_prc_trackLengthPerInitialE, "trackLengthPerInitialE/", "processes/");
      BookHistograms(m_report.histoMapMap_prc_InitialE, "InitialE/", "processes/");
      BookHistograms(m_report.histoMapMap_prc_numberOfSteps, "numberOfSteps/", "processes/");
      BookHistograms(m_report.histoMapMap_prc_numberOfStepsPerInitialE, "numberOfStepsPerInitialE/", "processes/");

      if (m_config.doGeneralHistograms) {
        BookHistograms(m_report.histoMapMap_numberOfSteps, "numberOfSteps/", "nSteps/");
        BookHistograms(m_report.histoMapMap_numberOfStepsPerInitialE, "numberOfStepsPerInitialE/", "nSteps/");
        BookHistograms(m_report.histoMapMap_trackLengthPerInitialE, "trackLengthPerInitialE/", "nSteps/");
        BookHistograms(m_report.histoMapMap_InitialE, "InitialE/", "nSteps/");
        BookHistograms(m_report.histoMapMap_stepKinetic, "stepKineticEnergy/", "nSteps/");
        BookHistograms(m_report.histoMapMap_postStepKinetic, "postStepKineticEnergy/", "nSteps/");
      }

      if (m_config.do2DHistograms) {
        BookHistograms(m_report.histoMapMap2D_vol_RZ, "2DMaps/", "volumes/");
        BookHistograms(m_report.histoMapMap2D_mat_RZ, "2DMaps/", "materials/");
        BookHistograms(m_report.histoMapMap2D_prc_RZ, "2DMaps/", "processes/");
        BookHistograms(m_report.histoMapMap2D_vol_RZ_E, "2DMaps_E/", "volumes/");
        BookHistograms(m_report.histoMapMap2D_mat_RZ_E, "2DMaps_E/", "materials/");
      }
    }
    else {
      ATH_MSG_WARNING("HistSvc not initialized...");      
    }

    return StatusCode::SUCCESS;
  }

  
} // namespace G4UA 
