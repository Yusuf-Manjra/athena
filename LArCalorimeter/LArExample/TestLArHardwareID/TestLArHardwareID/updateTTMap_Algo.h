/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UPDATETTMAP_ALGO_H
#define UPDATETTMAP_ALGO_H

/* author : F. Ledroit */
/* date of creation : 09/07/2007 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CxxUtils/checker_macros.h"

#include <string>

/** 
  * Algorithm meant to update LAr TT<->cell maping db obj
  *
  */
class ATLAS_NOT_THREAD_SAFE updateTTMap_Algo : public AthAlgorithm {

private :
  bool m_dumpMap;
  
public :
  
  updateTTMap_Algo(const std::string &name ,  ISvcLocator* pSvcLocator) ;
  virtual ~updateTTMap_Algo();
  virtual  StatusCode initialize();
  virtual  StatusCode execute();
  virtual  StatusCode finalize();
  
};

#endif
