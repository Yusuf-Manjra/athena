/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Asmp.cxx,v 1.5 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Asmp.h"
#include "RDBAccessSvc/IRDBQuery.h"
#include <iostream>
//#include <stdio>

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

DblQ00Asmp::DblQ00Asmp(IRDBQuery* m_asmp)
 : m_nObj(0)
{
  if(m_asmp) {
    m_asmp->execute();
    m_nObj = m_asmp->size();
    m_d = new ASMP[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Asmp banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(m_asmp->next()) {
        m_d[i].version     = m_asmp->data<int>("ASMP_DATA.VERS");    
        m_d[i].indx        = m_asmp->data<int>("ASMP_DATA.INDX");
        m_d[i].n           = m_asmp->data<int>("ASMP_DATA.N");          
        m_d[i].jtyp        = m_asmp->data<int>("ASMP_DATA.JTYP");       
        i++;
    }
    m_asmp->finalize();    
  }
  else {
    m_d = new ASMP[0];
    std::cerr<<"NO Asmp banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Asmp::~DblQ00Asmp()
{
    delete [] m_d;
}

} // end of namespace MuonGM
