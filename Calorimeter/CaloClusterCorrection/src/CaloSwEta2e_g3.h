/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOSWETA2E_G3_H
#define CALOCLUSTERCORRECTION_CALOSWETA2E_G3_H
/********************************************************************

NAME:     CaloSwEta2e_g3.h
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 15, 1999

PURPOSE:  S-shape corrections in sampling 2 as a function of eta
	  for endcap EM calorimeter. 
          (Tuned using 100 GeV E photons)  
          base class: CaloClusterCorrection
          Correction tuned on G3 samples.

Atrecon Orig: emreco/qeta2e.age          

Updated:  Feb 11, 2003   (MW)
          new correction for DC1 data

Updated:  May 5, 2004    (Sven Menke)
	  base class changed from algo to tool

Updated:  June, 2004    (sss)
          Use ToolWithConstants to get correction constants.
********************************************************************/

#include "CaloClusterCorrection/CaloClusterCorrection.h"
#include "CaloConditions/Array.h"

class CaloSwEta2e_g3 : public CaloClusterCorrection
{
public:
  /// Inherit constructor.
  using CaloClusterCorrection::CaloClusterCorrection;

  
  // Virtual function in CaloClusterCorrection
  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;

 private:
  CaloSwEta2e_g3() = delete;

  Constant<CxxUtils::Array<2> > m_correction { this, "correction", "" };
  Constant<int>        m_correction_degree { this, "correction_degree", "" };
  Constant<int>        m_region { this, "region", 1, "" };

  static const float s_middle_layer_granularity;
};


#endif
