/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
 // filename: TrigRoiUpdater.h
 //
 // author: Jiri Masik 
 //
 // Description: signature specific RoI manipulation for the ID reconstruction
 //              uniform attachment of the RoI to ease an analysis
 */
////////////////////////////////////////////////////////////////////////////

#ifndef INDETTRIGROIUPDATER_H
#define INDETTRIGROIUPDATER_H

#include "GaudiKernel/ToolHandle.h"


//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelTool.h"


namespace PESA

{

  class TrigRoiUpdater : public HLT::FexAlgo, virtual public IIncidentListener {
  public:
    TrigRoiUpdater(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrigRoiUpdater();
    HLT::ErrorCode hltInitialize();
    virtual void handle(const Incident& inc); 
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();

  private:
    
    HLT::ErrorCode registerROBs(const TrigRoiDescriptor *roi);

    float m_etaHalfWidth;
    float m_phiHalfWidth;
    float m_zHalfWidth;

    //ROB prefetching
    ToolHandle<IRegSelTool>     m_regionSelector_pix { this, "RegSelTool_Pixel", "RegSelTool/RegSelTool_Pixel" };     
    ToolHandle<IRegSelTool>     m_regionSelector_sct { this, "RegSelTool_SCT",   "RegSelTool/RegSelTool_SCT"   };     

    //!< region selector service
    bool  m_requestPIXRobs;
    bool  m_requestSCTRobs;

    //monitoring
    float m_inpPhiMinus,m_inpPhiPlus, m_inpPhiSize;
    float m_inpEtaMinus,m_inpEtaPlus, m_inpEtaSize;
    float m_PhiMinus,m_PhiPlus, m_PhiSize;
    float m_EtaMinus,m_EtaPlus, m_EtaSize;

    //monitoring of multiple execution
    bool m_monitorDuplicateRoIs;
    std::vector<TrigRoiDescriptor>      m_rois;
    uint64_t     m_invocations;
    uint64_t     m_duplicateRoIs;
  };

}

#endif
