/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Copy McEventCollection in overlaying jobs.
//
// Andrei Gaponenko <agaponenko@lbl.gov>, 2008

#ifndef COPYMCEVENTCOLLECTION_H
#define COPYMCEVENTCOLLECTION_H

#include <string>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"

#include "OverlayAlgBase/OverlayAlgBase.h"

class CopyMcEventCollection : public OverlayAlgBase  {
public:
  
  CopyMcEventCollection(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

protected:
  std::string m_infoType;
  bool m_realdata, m_checkeventnumbers;
  ServiceHandle<StoreGateSvc> m_storeGateData2;

};

#endif/*COPYMCEVENTCOLLECTION_H*/
