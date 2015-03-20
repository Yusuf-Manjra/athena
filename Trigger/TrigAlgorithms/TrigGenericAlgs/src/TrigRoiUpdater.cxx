/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigRoiUpdater.h"
#include "GaudiKernel/MsgStream.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

namespace PESA
{

  TrigRoiUpdater::TrigRoiUpdater(const std::string &name, 
					   ISvcLocator *pSvcLocator)
    : HLT::FexAlgo (name, pSvcLocator),
      m_etaHalfWidth(0.),
      m_phiHalfWidth(0.),
      m_zHalfWidth(0.)
  {
    declareProperty("EtaHalfWidth",           m_etaHalfWidth);
    declareProperty("PhiHalfWidth",           m_phiHalfWidth);

    m_inpPhiMinus = m_inpPhiPlus = m_inpPhiSize = 0.;
    m_inpEtaMinus = m_inpEtaPlus = m_inpEtaSize = 0.;
    m_PhiMinus = m_PhiPlus = m_PhiSize = 0.;
    m_EtaMinus = m_EtaPlus = m_EtaSize = 0.;

  }

  TrigRoiUpdater::~TrigRoiUpdater()
  {}

  //----------------------------------
  //          beginRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TrigRoiUpdater::hltBeginRun() {
    ATH_MSG_DEBUG("TrigRoiUpdater::beginRun()");

    return HLT::OK;
  }
  //----------------------------------------------------------------------------

  ///////////////////////////////////////////////////////////////////
  // Initialisation
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigRoiUpdater::hltInitialize() {

    //signature specific modifications
    const std::string instanceName = name();
    
    if (instanceName.find("Cosmics") !=std::string::npos){
      m_zHalfWidth = 1000.;
      ATH_MSG_INFO ("Instance " << instanceName << " z range modification: " << m_zHalfWidth);
    }


    return HLT::OK;
  }

  ///////////////////////////////////////////////////////////////////
  // Execute HLT Algorithm
  ///////////////////////////////////////////////////////////////////
  HLT::ErrorCode TrigRoiUpdater::hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE) {
 
    ATH_MSG_DEBUG("execHLTAlgorithm()");

    m_inpPhiMinus = m_inpPhiPlus = m_inpPhiSize = -10.;
    m_inpEtaMinus = m_inpEtaPlus = m_inpEtaSize = -10.;
    m_PhiMinus = m_PhiPlus = m_PhiSize = -10.;
    m_EtaMinus = m_EtaPlus = m_EtaSize = -10.;

    const TrigRoiDescriptor *roi = 0;
    bool forIDfound=false;
    //    bool updateNeeded=false;

    std::vector<std::string> roiNames = {"forID3","forID2","forID1", "forID", ""};
    std::string roiName= "";

    for (size_t k=0; k<roiNames.size(); k++){
      if (HLT::OK != getFeature(outputTE, roi, roiNames[k]) || !roi){
	ATH_MSG_DEBUG("|"  << roiNames[k] << "| not found or zero"); 
      } else {
	roiName = roiNames[k-1>0 ? k-1 : 0];
	ATH_MSG_DEBUG("RoI with label |" << roiNames[k] << "| found and use " << roiName << " for an update"); 
	if (roiName.find("forID1")!=std::string::npos) forIDfound = true;
	break;
      }
    }

    if (roi->composite()){
      ATH_MSG_DEBUG("Not touching a composite RoI");
      return HLT::OK;
    }

    //signature specific modifications
    const std::string instanceName = name();

    if (forIDfound && instanceName.find("IDTrigRoiUpdater_Muon_IDTrig")!=std::string::npos){
      ATH_MSG_DEBUG("don't update roiDecriptor from muFast");
      return HLT::OK;
    }


    m_inpPhiMinus= roi->phiMinus(); m_inpPhiPlus = roi->phiPlus();  m_inpPhiSize= m_inpPhiPlus - m_inpPhiMinus;
    m_inpEtaMinus= roi->etaMinus(); m_inpEtaPlus = roi->etaPlus();  m_inpEtaSize= m_inpEtaPlus - m_inpEtaMinus;



    float eta = roi->eta();
    float phi = roi->phi();

    float oldEtaW = m_inpEtaPlus - m_inpEtaMinus;
    float oldPhiW = m_inpPhiPlus - m_inpPhiMinus;
    float diff_e = oldEtaW/2. - m_etaHalfWidth;
    float diff_p = oldPhiW/2. - m_phiHalfWidth;

    float zedm = roi->zedMinus()-m_zHalfWidth;
    float zedp = roi->zedPlus()+m_zHalfWidth;


    TrigRoiDescriptor *outroi =     
		       new TrigRoiDescriptor(eta, m_inpEtaMinus+diff_e, m_inpEtaPlus-diff_e,
					     phi, m_inpPhiMinus+diff_p, m_inpPhiPlus-diff_p,
					     roi->zed(), zedm, zedp);

    ATH_MSG_DEBUG("Input RoI " << *roi);

    if ( HLT::OK !=  attachFeature(outputTE, outroi, roiName) ) {
      ATH_MSG_ERROR("Could not attach feature to the TE");
      return HLT::NAV_ERROR;
    }
    else {
      ATH_MSG_DEBUG("REGTEST: attached RoI " << roiName << *outroi);
    }

    return HLT::OK;
  }
  ///////////////////////////////////////////////////////////////////
  // Finalize
  ///////////////////////////////////////////////////////////////////
  
  HLT::ErrorCode TrigRoiUpdater::hltFinalize() {

    ATH_MSG_DEBUG("finalize() success");
    return HLT::OK;
  }

  //----------------------------------
  //          endRun method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode TrigRoiUpdater::hltEndRun() {
   
    ATH_MSG_DEBUG("TrigRoiUpdater::endRun()");
   
    return HLT::OK;
  }
  //---------------------------------------------------------------------------
} // end namespace


