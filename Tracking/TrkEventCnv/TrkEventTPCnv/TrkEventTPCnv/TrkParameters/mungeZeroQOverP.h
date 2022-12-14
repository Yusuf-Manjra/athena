// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file mungeZeroQOverP.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief tpcnv compatibility helper
 *
 * Small helper to be (bug-) compatibile with previous version
 * of tp converters.
 */


#ifndef TRKEVENTTPCNV_MUNGEZEROQOVERP_H
#define TRKEVENTTPCNV_MUNGEZEROQOVERP_H

#include "CxxUtils/no_sanitize_undefined.h"

namespace TrkEventTPCnv {

template<int DIM, class T, class S>
void mungeZeroQOverP
NO_SANITIZE_UNDEFINED(Trk::ParametersT<DIM, T, S>& p)
{
  AmgVector(DIM) newParam = p.parameters();
  newParam[Trk::qOverP] = 0;
  p.setParameters(newParam);
}
}


#endif // not TRKEVENTTPCNV_MUNGEZEROQOVERP_H
