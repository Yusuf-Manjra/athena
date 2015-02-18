/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigCaloCluster_p2
 *
 * @brief persistent partner for TrigCaloCluster
 *
 * @author Denis Oliveira Damazio <Denis.Oliveira.Damazio@cern.ch>
 *
 * File and Version Information:
 * $Id: TrigCaloCluster_p3.h 590712 2014-04-01 19:27:58Z ssnyder $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGCALOCLUSTER_P3_H
#define TRIGCALOEVENTTPCNV_TRIGCALOCLUSTER_P3_H

class TrigCaloCluster_p3
{
 public:
  static const int MAXSIZE_P = 25;
  
  TrigCaloCluster_p3() {}
  friend class TrigCaloClusterCnv_p3;
  
 private:

/*
  float        m_rawEnergy;
  float        m_rawEt;
  float        m_rawEnergyS[MAXSIZE];
  float        m_rawEta;
  float        m_rawPhi;
*/
  float m_allFloats[4+MAXSIZE_P];

  long         m_roiWord;
  int          m_numberUsedCells;
  unsigned int m_quality;

};

#endif
