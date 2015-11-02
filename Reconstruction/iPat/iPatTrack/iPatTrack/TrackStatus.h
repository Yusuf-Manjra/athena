/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Distinguish type/status of Track objects
 ----------------------------------------
 Copyright (C) 2000 by ATLAS Collaboration
 ***************************************************************************/

#ifndef IPATTRACK_TRACKSTATUS_H
# define IPATTRACK_TRACKSTATUS_H

//<<<<<< INCLUDES                                                       >>>>>>

#ifdef HAVE_NEW_IOSTREAMS
  #include <iostream>
#else
  #include <iostream.h>
#endif

//<<<<<< PUBLIC DEFINES                                                 >>>>>>

enum  TrackStatus
{
    undefined,
    two_point,
    three_point,
    fast_segment,
    segment,
    long_segment,
    secondary,
    truncated,
    pendingTRT,
    primary,
    muon,
    combined_muon,
    combined_ecal
};

//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>

std::ostream &operator<< (std::ostream &, TrackStatus);

#endif // IPATTRACK_TRACKSTATUS_H
