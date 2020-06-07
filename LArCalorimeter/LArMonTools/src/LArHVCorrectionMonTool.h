/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArHVCorrectionMonTool
 * @author Jessica Leveque <jleveque@in2p3.fr>
 *
 */

#ifndef LARMONTOOLS_LARHVCORRECTIONMONTOOL_H
#define LARMONTOOLS_LARHVCORRECTIONMONTOOL_H

#include "SelectAllLArRawChannels.h"
#include "LArOnlineIDStrHelper.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "EventContainers/SelectAllObject.h" 
#include "LArCabling/LArCablingLegacyService.h"
#include "LArElecCalib/ILArHVScaleCorr.h"

#include <string>
#include <map>

class LArEM_ID;
class LArOnlineID;
class CaloDetDescrManager;
class CaloDetDescrElement;
class ITHistSvc;

#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile_LW.h"
class TProfile_LW;

class LArHVCorrectionMonTool: public ManagedMonitorToolBase
{
 public:
  LArHVCorrectionMonTool(const std::string& type, 
                      const std::string& name,
		      const IInterface* parent);

  /** @brief Default destructor */
  virtual ~LArHVCorrectionMonTool();

  /** @brief Overwrite dummy method from AlgTool */
  StatusCode initialize();

  /** Book general histograms
   *  Implement pure virtual methods of IMonitorToolBase */
  StatusCode bookHistograms();

  /** Called each event */
  StatusCode fillHistograms();

  /** Regularly called to fill noise maps
   *  Overwrite dummy method from MonitorToolBase */
  StatusCode procHistograms();

 protected:

  // services
  const LArOnlineID* m_LArOnlineIDHelper;
  const LArEM_ID*    m_LArEM_IDHelper;
  const LArFCAL_ID*  m_LArFCAL_IDHelper;
  const LArHEC_ID*   m_LArHEC_IDHelper;
  const CaloIdManager*       m_caloIdMgr;

  LArOnlineIDStrHelper* m_strHelper;
  ITHistSvc* m_rootStore;
  /** Handle to LArCablingService */
  ToolHandle<LArCablingLegacyService> m_larCablingService;  

  SG::ReadCondHandleKey<ILArHVScaleCorr> m_scaleCorrKey
  { this, "LArHVScaleCorr", "LArHVScaleCorrRecomputed", "" };
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_onlineScaleCorrKey
  { this, "OnlineLArHVScaleCorr", "LArHVScaleCorr", "" };

 private:

  // To retrieve bad channel DB keywords 
  int DBflag(HWIdentifier onID);

  // To set histos Style
  void SetTH2Style(TH2F_LW* h);
  void SetTH1Style(TH1I_LW* h);

  // Properties
  SG::ReadHandleKey<LArRawChannelContainer> m_channelKey{this, "LArRawChannelKey", "LArRawChannels"};
  float m_threshold;
  float m_delta_eta;
  float m_delta_phi;
  int m_nlumi_blocks;

  // Other things
  int m_eventsCounter;

  // Non-nominal Maps 
  TH2F_LW* m_hLArHVCorrectionEMB[2];
  TH2F_LW* m_hLArHVCorrectionEMEC[2];
  TH2F_LW* m_hLArHVCorrectionHEC[2];
  TH2F_LW* m_hLArHVCorrectionFCAL[2];

  TH1I_LW* m_hNDeviatingChannelsEMB[2];
  TH1I_LW* m_hNDeviatingChannelsEMEC[2];
  TH1I_LW* m_hNDeviatingChannelsHEC[2];
  TH1I_LW* m_hNDeviatingChannelsFCAL[2];

};

#endif
