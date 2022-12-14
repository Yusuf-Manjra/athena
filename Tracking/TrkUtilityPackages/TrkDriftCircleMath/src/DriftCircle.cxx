/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/DCOnTrack.h"

MsgStream& operator<<(MsgStream& os, const TrkDriftCircleMath::DriftCircle& dc) {
    os << dc.position() << std::setw(6) << " r " << dc.r() << " dr " << dc.dr() << " id " << dc.id();
    if (dc.driftState() == TrkDriftCircleMath::DriftCircle::EarlyHit)
        os << " EarlyHit";
    else if (dc.driftState() == TrkDriftCircleMath::DriftCircle::InTime)
        os << " InTime";
    else if (dc.driftState() == TrkDriftCircleMath::DriftCircle::LateHit)
        os << " LateHit";
    else if (dc.driftState() == TrkDriftCircleMath::DriftCircle::EmptyTube)
        os << " EmptyTube";
    else if (dc.driftState() == TrkDriftCircleMath::DriftCircle::NotCrossed)
        os << " NotCrossed";
    else
        os << " unknown driftState";
    return os;
}

std::ostream& operator<<(std::ostream& os, const TrkDriftCircleMath::DriftCircle& dc) {
    os << dc.position() << " r " << dc.r() << " dr " << dc.dr() << " id " << dc.id();
    if (dc.driftState() == TrkDriftCircleMath::DriftCircle::EarlyHit)
        os << " EarlyHit";
    else if (dc.driftState() == TrkDriftCircleMath::DriftCircle::InTime)
        os << " InTime";
    else if (dc.driftState() == TrkDriftCircleMath::DriftCircle::LateHit)
        os << " LateHit";
    else if (dc.driftState() == TrkDriftCircleMath::DriftCircle::EmptyTube)
        os << " EmptyTube";
    else if (dc.driftState() == TrkDriftCircleMath::DriftCircle::NotCrossed)
        os << " NotCrossed";
    else
        os << " unknown driftState";
    return os;
}
