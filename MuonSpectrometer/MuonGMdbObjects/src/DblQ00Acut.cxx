/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Acut.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Acut.h"
#include "RDBAccessSvc/IRDBQuery.h"
#include <iostream>
#include <stdexcept>

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

namespace MuonGM
{

DblQ00Acut::DblQ00Acut(IRDBQuery* m_acut)
 : m_nObj(0)
{
  if(m_acut) {
    m_acut->execute();
    m_nObj = m_acut->size();
    m_d = new ACUT[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Acut banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(m_acut->next()) {
	m_d[i].version = m_acut->data<int>("ACUT_DATA.VERS");
	m_d[i].i       = m_acut->data<int>("ACUT_DATA.I");
	m_d[i].icut    = m_acut->data<int>("ACUT_DATA.ICUT");
	m_d[i].n       = m_acut->data<int>("ACUT_DATA.N");
	i++;
    }
    m_acut->finalize();
  }
  else {
    m_d = new ACUT[0];
    std::cerr<<"NO Acut banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Acut::~DblQ00Acut()
{
    delete [] m_d;
    m_d = 0;
}

} // end of namespace MuonGM
