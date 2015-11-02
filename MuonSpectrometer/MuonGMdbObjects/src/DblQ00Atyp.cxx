/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: DblQ00Atyp.cxx,v 1.4 2007-02-12 17:33:50 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGMdbObjects/DblQ00Atyp.h"
#include "RDBAccessSvc/IRDBQuery.h"
#include <iostream>
#include <stdio.h>

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

DblQ00Atyp::DblQ00Atyp(IRDBQuery* m_atyp)
 : m_nObj(0)
{
  if(m_atyp) {
    m_atyp->execute();
    m_nObj = m_atyp->size();
    m_d = new ATYP[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Atyp banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(m_atyp->next()) {
        m_d[i].version        = m_atyp->data<int>("ATYP_DATA.VERS");    
        m_d[i].jtyp           = m_atyp->data<int>("ATYP_DATA.JTYP");          
        m_d[i].nsta           = m_atyp->data<int>("ATYP_DATA.NSTA");          
        sprintf(m_d[i].type,"%s",m_atyp->data<std::string>("ATYP_DATA.TYP").c_str());
        //std::cerr<<" Atyp:: version, type, jtyp, nsta "<<m_d[i].version<<" "<<m_d[i].type<<" "<<m_d[i].jtyp<<" "<<m_d[i].nsta  <<std::endl;
        i++;
    }
    m_atyp->finalize();
  }
  else {
    m_d = new ATYP[0];
    std::cerr<<"NO Atyp banks in the MuonDD Database"<<std::endl;
  }
}
    
DblQ00Atyp::~DblQ00Atyp()
{
    delete [] m_d;
}

} // end of namespace MuonGM
