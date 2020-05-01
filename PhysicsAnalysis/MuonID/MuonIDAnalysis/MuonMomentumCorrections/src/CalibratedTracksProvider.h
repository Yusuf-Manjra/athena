/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALIBRATEDTRACKSPROVIDER
#define CALIBRATEDTRACKSPROVIDER

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include <AsgAnalysisInterfaces/IPileupReweightingTool.h>

namespace CP {

   /// decorates a track collection with efficiency and scale factor

   class CalibratedTracksProvider : public AthAlgorithm {

   public:
      /// Regular Algorithm constructor
       CalibratedTracksProvider( const std::string& name, ISvcLocator* svcLoc );

      /// Function initialising the algorithm
      virtual StatusCode initialize();
      /// Function executing the algorithm
      virtual StatusCode execute();

   private:
      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this, "EventInfoContName", "EventInfo", "event info key"};
      /// track container
      std::string m_inputKey,m_outputKey;

      ///Calibration tool handle
      ToolHandle<IMuonCalibrationAndSmearingTool > m_tool;
      ToolHandle<IPileupReweightingTool> m_prwTool;

      /// detector type of track (MS or ID)
      int m_detType;
      bool m_useRndNumber;

   }; // class 

} // namespace CP

#endif //
