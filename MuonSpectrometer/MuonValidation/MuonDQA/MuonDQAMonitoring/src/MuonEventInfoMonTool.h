/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MuonDQAMonitoring
// Author:   N. Benekos(Illinois)
// March 2008
//
// DESCRIPTION:
// Subject: MDT-->Offline Muon Data Quality
///////////////////////////////////////////////////////////////////////////

#ifndef MuonEventInfoMonTool_H
#define MuonEventInfoMonTool_H

 
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include <string>

class StatusCode;
namespace MuonDQA{
  class MuonDQAEvent;
}
class TH1F;
class StoreGateSvc;
 
template <class ConcreteAlgorithm> class AlgFactory;

namespace MuonDQA {
  class MuonDQAEventInfo;
  //}
  /////////////////////////////////////////////////////////////////////////////

  class MuonEventInfoMonTool: public ManagedMonitorToolBase 
    {
    public:

 
      MuonEventInfoMonTool( const std::string & type, const std::string & name, const IInterface* parent ); 
 
      virtual ~MuonEventInfoMonTool()=default;
      StatusCode initialize();
      virtual StatusCode bookHistograms();
      virtual StatusCode fillHistograms();
      virtual StatusCode procHistograms();


    private: 

      /** retrieve event information from storegate and convert to MuonRawDQAEventInfo */
      const MuonDQAEvent* retrieveEvent() const; 
      /** retrieve event information from storegate and convert to MuonCalibEventInfo */
      MuonDQAEventInfo retrieveEventInfo() const;
  
      StoreGateSvc* m_eventStore;
  
      //Declare Properties
 
      int m_lastEvent;
      bool m_TriggerTagAdd;             //!< property toggling if the Trigger type should be added to the EventTag 

      std::string  m_eventTag;          //!< tag of the event, describing reconstruction mechanism 
    
      TH1F* m_hTriggerType; 

    };
}

#endif  
