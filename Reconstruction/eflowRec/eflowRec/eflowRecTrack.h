/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowRecTrack.h
 *
 *  Created on: 30.09.2013
 *      Author: tlodd
 */

#ifndef EFLOWRECTRACK_H_
#define EFLOWRECTRACK_H_

#include <iostream>
#include <cassert>
#include "GaudiKernel/ToolHandle.h"

#include "CxxUtils/fpcompare.h"

#include "AthLinks/ElementLink.h"

#include "Particle/TrackParticleContainer.h"

#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowCellSubtractionManager.h"
#include "eflowRec/PFMatchInterfaces.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

class eflowTrackClusterLink;

class eflowTrackExtrapolatorBaseAlgTool;

class eflowRecTrack {
public:
  eflowRecTrack(const ElementLink<xAOD::TrackParticleContainer>& trackElemLink,
                const ToolHandle<eflowTrackExtrapolatorBaseAlgTool>& theTrackExtrapolatorTool);
  virtual ~eflowRecTrack();

  const xAOD::TrackParticle* getTrack() const { return m_track; }

  const eflowTrackCaloPoints& getTrackCaloPoints() const { return *m_trackCaloPoints; }

  ElementLink<xAOD::TrackParticleContainer> getTrackElemLink() const { return m_trackElemLink; }
  void addClusterMatch(eflowTrackClusterLink* clusterMatch) { m_clusterMatches.push_back(clusterMatch); }

  eflowCellSubtractionManager& getCellSubtractionManager() { return m_cellSubtractionManager; }

  int getType() const { return m_type; }

  const std::vector<eflowTrackClusterLink*>& getClusterMatches() const { return m_clusterMatches; }

  bool hasBin() const { return m_hasBin; }
  void setHasBin(bool hasBin) { m_hasBin = hasBin; }

  void setEExpect(double eExpect, double varEExpect){ m_eExpect = eExpect; m_varEExpect = varEExpect; }
  double getEExpect() const { return m_eExpect; }
  double getVarEExpect() const { return m_varEExpect; }

  void setCaloDepthArray(const double* depthArray);
  const std::vector<double>& getCaloDepthArray() const { return m_caloDepthArray; }

  bool isSubtracted() const { return m_isSubtracted; }

  void setSubtracted() {
    if (isSubtracted()){
      std::cout << "eflowRecTrack\tWarning:\tInvoke setSubtracted() on track that is subtracted already!" << std::endl;
      return;
    }
    m_isSubtracted = true;
  }

  bool isInDenseEnvironment() const { return m_isInDenseEnvironment;}
  void setIsInDenseEnvironment() { m_isInDenseEnvironment = true; }

  void setpull15(double pull15){ m_pull15 = pull15; }
  double getpull15() const { return m_pull15; }

private:
  ElementLink<xAOD::TrackParticleContainer> m_trackElemLink;
  const xAOD::TrackParticle* m_track;
  int m_type;
  double m_pull15;

  bool m_isInDenseEnvironment;
  bool m_isSubtracted;
  bool m_hasBin;
  double m_eExpect;
  double m_varEExpect;

  std::vector<double> m_caloDepthArray;

  eflowTrackCaloPoints* m_trackCaloPoints;
  eflowCellSubtractionManager m_cellSubtractionManager;
  std::vector<eflowTrackClusterLink*> m_clusterMatches;

public:
  class SortDescendingPt {
  public:
    bool operator()(const eflowRecTrack* a, const eflowRecTrack* b) {
      return CxxUtils::fpcompare::greater(a->getTrack()->pt(),
                                          b->getTrack()->pt());
    }
  };
};
class eflowRecMatchTrack: public PFMatch::ITrack {
public:
  eflowRecMatchTrack(const eflowRecTrack* efRecTrack): m_efRecTrack(efRecTrack) { assert(m_efRecTrack); }
  virtual ~eflowRecMatchTrack() { }

  virtual eflowEtaPhiPosition etaPhiInLayer(PFMatch::LayerType layer) const {
    return m_efRecTrack->getTrackCaloPoints().getEtaPhiPos(layer);
  }

private:
  const eflowRecTrack* m_efRecTrack;
};

#endif /* EFLOWRECTRACK_H_ */
