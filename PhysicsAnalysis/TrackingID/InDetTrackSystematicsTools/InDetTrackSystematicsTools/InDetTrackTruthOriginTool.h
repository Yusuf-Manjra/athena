// -*- c++ -*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSYSTEMATICSTOOLS_INDETTRACKTRUTHORIGINTOOL_H
#define INDETTRACKSYSTEMATICSTOOLS_INDETTRACKTRUTHORIGINTOOL_H

#include "IInDetTrackTruthOriginTool.h"
#include "AsgTools/AsgTool.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#include <string>

namespace InDet {

  /**
  @class InDetTrackTruthOriginTool
  This class determines the truth origin of a track
  @author Remi Zaidan (remi.zaidan@cern.ch)
  @author Felix Clark (michael.ryan.clark@cern.ch)
  */

  class InDetTrackTruthOriginTool : public virtual IInDetTrackTruthOriginTool, public asg::AsgTool {

    ASG_TOOL_CLASS2( InDetTrackTruthOriginTool,
		     asg::IAsgTool,
		     InDet::IInDetTrackTruthOriginTool )

  public:

    InDetTrackTruthOriginTool (const std::string& name);
    virtual ~InDetTrackTruthOriginTool();

    virtual StatusCode initialize() override;

    /** Safely access a track's linked truth particle, if available **/
    virtual const xAOD::TruthParticle* getTruth( const xAOD::TrackParticle* track ) const override;

    /** Computes the tracks origin */
    virtual int getTrackOrigin(const xAOD::TrackParticle* track) const override;

    /** Computes the truth origin (from B or D hadron) */
    virtual bool isFrom(const xAOD::TruthParticle* part, int flav) const override;

  private:

    int getParentID(const xAOD::TruthParticle* part) const;

    float m_matchingProbabilityCut;
    float m_barcodeG4;

    std::string m_truthParticleLinkName;
    std::string m_truthMatchProbabilityAuxName;

}; // class InDetTrackTruthOriginTool

} // namespace InDet

#endif
