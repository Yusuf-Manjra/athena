/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REINITTTMAP_ALGO_H
#define REINITTTMAP_ALGO_H

/* author : F. Ledroit */
/* date of creation : 11/03/2009 */

#include "GaudiKernel/Algorithm.h"
#include <string>
#include "RegistrationServices/IIOVRegistrationSvc.h"

/** 
  * Algorithm meant to initialise LAr TT<->cell maping
  *
  */
class LArFCAL_ID;
class StoreGateSvc;
class IAthenaOutputStreamTool;

class reinitTTMap_Algo : public Algorithm {

  private :
  

  bool m_dumpMap;
  bool m_testOnly;
  unsigned int m_begRun;
  unsigned int m_endRun;
  unsigned int m_begLB;
  unsigned int m_endLB;

  StoreGateSvc*           m_detStore ;
  const LArFCAL_ID*       m_fcalHelper;
  IIOVRegistrationSvc*    m_regSvc;
  
  virtual  StatusCode initMap();
  virtual  StatusCode testStruct();

  StringProperty            m_streamName;
  IAthenaOutputStreamTool*  m_streamer;

  public :
  
  reinitTTMap_Algo(const std::string &name ,  ISvcLocator* pSvcLocator) ;
  virtual ~reinitTTMap_Algo();
  virtual  StatusCode initialize();
  virtual  StatusCode execute();
  virtual  StatusCode finalize();
  
};

#endif
