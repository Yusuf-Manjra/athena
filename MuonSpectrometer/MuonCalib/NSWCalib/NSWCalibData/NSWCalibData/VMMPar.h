/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONVMMPAR_H
#define MUONVMMPAR_H

#include "Identifier/Identifier.h"
namespace Muon{
class VMMPar{

public:
  VMMPar();
  ~VMMPar();

  Identifier identify(){return m_identifier;}
  void setIdentifier(Identifier identifier){m_identifier=identifier;}


protected:
  Identifier m_identifier; 


};


#endif
}
