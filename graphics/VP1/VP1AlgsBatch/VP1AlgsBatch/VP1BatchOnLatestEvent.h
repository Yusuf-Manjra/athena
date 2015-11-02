/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1BatchOnLatestEvent            //
//                                                         //
//  Riccardo-Maria BIANCHI <rbianchi@cern.ch>              //
//  11 Mar 2015                                            //
//                                                         //
//  This is the Athena algorithm to launch VP1 in          //
//  batch-mode, to automatically create 3D event           //
//  displays on-the-fly.                                   //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1ALGS_VP1BatchOnLatestEvent
#define VP1ALGS_VP1BatchOnLatestEvent

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include "PoolSvc/IPoolSvc.h"
#include <string>

class StoreGateSvc;

class VP1BatchOnLatestEvent: public AthAlgorithm,
		    public IIncidentListener
{
 public:
  VP1BatchOnLatestEvent(const std::string& name, ISvcLocator* pSvcLocator);
  ~VP1BatchOnLatestEvent();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void handle(const Incident& inc);

 private:
  // run/event number to be used in the vp1 event file name
//  int m_runNumber;
//  int m_eventNumber;
//  unsigned int m_timeStamp;
//  std::string m_humanTimestamp;

  // properties
  std::string m_inputVP1CfgFile;
  std::string m_destinationDir;
//  int m_maxProducedFiles;
  int m_nEvent; // Internal counter for the number of processed events
  // service handle
  ServiceHandle<IPoolSvc> m_poolSvc;
};

#endif
