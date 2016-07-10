/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowFirstIntParameters.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson, R Duxfield (based on R.Duxfields Root package)
CREATED:  18th Aug, 2005

********************************************************************/

//Athena Headers
#include "eflowRec/eflowCaloRegions.h"
#include "eflowRec/eflowBinnedParameters.h"
#include "eflowRec/eflowFirstIntParameters.h"
#include <iostream>

eflowFirstIntParameters::eflowFirstIntParameters() :
  m_p(eflowCalo::nRegions) {

  const int np = eflowBinnedParameters::nShapeParams();

  for (int i = 0; i < eflowCalo::nRegions; i++) {
    m_p[i].resize(np);
    for (int j = 0; j < np; j++) m_p[i][j] = 0.0;
  }
}

void eflowFirstIntParameters::printM_P() {
  const int np = eflowBinnedParameters::nShapeParams();

  for (int i = 0; i < eflowCalo::nRegions; i++)
    for (int j = 0; j < np; j++)
      std::cout << "m_p[" << i << "][" << j << "] is " << m_p[i][j] << std::endl;
}
