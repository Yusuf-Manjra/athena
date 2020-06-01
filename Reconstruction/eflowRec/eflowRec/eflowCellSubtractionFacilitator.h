/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

  #ifndef EFLOWCELLSUBTRACTIONFACILITATOR_H
#define EFLOWCELLSUBTRACTIONFACILITATOR_H

/********************************************************************

NAME:     eflowCellSubtractionFacilitator.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  3rd February 2009

********************************************************************/

#include "eflowRec/eflowRingSubtractionManager.h"
#include "eflowRec/eflowCellList.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/MsgStream.h"

#include <vector>

/**
 This class deals with the removal of calorimeter cells from calorimeter clusters. The main entry points are the subtractCells methods which find out how much energy they should subtract from the eflowRingSubtractionManager. The eflowRingSubtractionManager also contains a list of calorimeter cell rings (a ring is a list of calorimeter cells, in a given calorimeter layer, between two radii r1 and r2). The cell information is stored in the eflowCellList. Rings are removed one by one in the order they are stored until the summed cell ring energy is >= to the expected energy deposit from the track in the calorimeter.
*/
class eflowCellSubtractionFacilitator: public AthMessaging{

 public:

  eflowCellSubtractionFacilitator();

  double subtractCells(eflowRingSubtractionManager& ringSubtractionManager, double trackEnergy, xAOD::CaloCluster* tracksClus, eflowCellList& orderedCells);
  double subtractCells(eflowRingSubtractionManager& ringSubtractionManager, double trackEnergy, std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClus, eflowCellList& orderedCells);
  bool annFlag() {return m_annFlag;}
  void setAnnFlag() { m_annFlag = true; }

 private:
  bool m_annFlag;
  CaloClusterCellLink::iterator getCellIterator(const xAOD::CaloCluster* thisCluster, const CaloCell* thisCell);
  void updateClusterKinematics(std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters);
  void updateClusterKinematics(xAOD::CaloCluster*);

  double getTotalEnergy(std::vector<std::pair<xAOD::CaloCluster*, bool> > tracksClusters);

  double getRingsEnergy(const std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters, CellIt beginRing,
                        CellIt endRing);
  void subtractPartialRings(std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters, CellIt beginRing,
                            CellIt endRing, double targetRingEnergy, double eRing);
  void subtractFullRings(std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters, CellIt beginRing,
                         CellIt endRing);
  bool subtractRings(eflowRingSubtractionManager& ringSubtractionManager,
                     const std::pair<eflowCaloENUM, short>& ring, double & eSubtracted,
                     const double eExpect, eflowCellList& orderedCells,
                     std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters);

  bool subtractCaloCell(double & eSubtracted, const double eExpect, xAOD::CaloCluster* cluster, CaloCell* cell);
  void annihilateClusters(std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters);
  bool subtractReorderedCells(std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters,
                            double eSubtracted, const double eExpect, eflowCellList& orderedCells);
};
#endif
