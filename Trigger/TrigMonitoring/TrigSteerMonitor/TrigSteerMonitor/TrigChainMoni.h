/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: ATLAS HLT Steering Monitoring
 * @Package: Trigger/TrigMonitoring/TrigChainMonitor
 * @Class  : TrigChainMoni
 * @Created: Nov, 2006
 *
 * @brief number of events vs chain for raw accepted and events after passthrough and 
 *        prescale
 *
 * @author Gordon Fischer        Gordon.Fischer@cern.ch
 *
 * File and Version Information:
 * $Id: TrigChainMoni.h,v 1.13 2009-02-17 08:05:45 tbold Exp $
 **********************************************************************************/


#ifndef TRIGCHAINMONI_H
#define TRIGCHAINMONI_H

#include <vector>
#include <string>
#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigMonitorBase/TrigLBNHist.h"

namespace HLT {
   class TrigSteer;
}

class TH1I;
class StoreGateSvc;

class TrigChainMoni: public TrigMonitorToolBase {

public:
   TrigChainMoni(const std::string & type, const std::string & name,
                 const IInterface* parent);

   virtual ~TrigChainMoni();

   virtual StatusCode initialize();
   virtual StatusCode finalize();
   virtual StatusCode endRun();
   virtual StatusCode bookHists();
   virtual StatusCode bookHistograms( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun );
   virtual StatusCode fillHists();

   // does noting
   virtual StatusCode finalHists();
  
private:
   StoreGateSvc* m_storeGate; //!< storegate
   MsgStream* m_log; 
   unsigned int m_logLvl; //!< MsgStram level
   std::string m_histoPathshift;
   //  unsigned int m_reserveLumiHistos;  // this is availabel in base class so not needed here
   const HLT::TrigSteer* m_parentAlg; // should give us pointer to TrigSteer topAlgorithm!!!

   // 1D histograms

   //Lumi Block Histos

   TH1I *m_chainAcceptanceHist;
   TH1I *m_chainAcceptancePSHist;
   TH1I *m_chainAcceptancePTHist;
   TH1I *m_activeChainsHist;
   TH1I *m_runChainsHist;
   TH1I *m_rerunChainsHist;

   unsigned int binnr; //not used anymore
   std::map< unsigned int, unsigned int > m_binmap;
   std::string m_trigLvl;
   bool m_useLBHistos;
};


#endif
