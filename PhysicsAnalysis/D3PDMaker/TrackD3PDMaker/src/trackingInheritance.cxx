/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/trackingInheritance.cxx
 * @author Serhan Mete, scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Declare inheritance relationships for tracking classes.
 *
 * Eventually, these should be moved to the EDM classes.
 */

#include "TrkParameters/TrackParameters.h"
#include "VxVertex/VxContainer.h"
#include "AthenaKernel/BaseInfo.h"


SG_ADD_BASE (Trk::Perigee, Trk::TrackParameters);
SG_ADD_BASE (VxContainer, DataVector<Trk::VxCandidate>);

