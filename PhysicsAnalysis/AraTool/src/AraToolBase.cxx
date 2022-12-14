/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AraTool/AraToolBase.h"

AraToolBase::AraToolBase(PropertyMgr *pmgr)
  :m_propertyMgr(pmgr),m_runningARA(false)
{
  if(!m_propertyMgr) {
    m_runningARA=true;
  }
}

AraToolBase::~AraToolBase()
{
}

