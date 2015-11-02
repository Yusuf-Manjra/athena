/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 some simple geometrical properties for direction vectors
 represented using transverse direction cosines
 --------------------------------------------------------
 Copyright (C) 2000 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: Direction.cxx,v 1.5 2006-10-04 21:33:05 pop Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#ifdef HAVE_NEW_IOSTREAMS
  #include <iomanip>
  #include <iostream>
#else
  #include <iomanip.h>
  #include <iostream.h>
#endif
#include "iPatTrackFinder/Direction.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>
//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

void
Direction::print (void) const
{
    std::cout << std::setiosflags(std::ios::fixed)
	      << "Direction : cos_phi "	<< std::setw(7) << std::setprecision(4) << m_cos_phi
	      << "   sin_phi " 	       	<< std::setw(7) << std::setprecision(4) << m_sin_phi
	      << "   cot_theta "       	<< std::setw(7) << std::setprecision(4) << m_cot_theta
	      << std::resetiosflags(std::ios::fixed) << std::endl;
}

