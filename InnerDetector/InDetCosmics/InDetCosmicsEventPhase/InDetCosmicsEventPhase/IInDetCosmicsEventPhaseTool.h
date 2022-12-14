/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IInDetCosmicsEventPhaseTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IINDETINDETCOSMICSEVENTPHASETOOL_H
#define IINDETINDETCOSMICSEVENTPHASETOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "TrkTrack/Track.h"
#include "TrkSegment/TrackSegment.h"

namespace InDet 
{


  static const InterfaceID IID_IInDetCosmicsEventPhaseTool("InDet::IInDetCosmicsEventPhaseTool", 1, 0);

  class IInDetCosmicsEventPhaseTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    /** finds event phase of a track from the leading edge */
    virtual double findPhase(const Trk::Track *track) const =0;

    /** finds event phase of a segment from the leading edge */
    virtual double findPhase(const Trk::Segment *segment) const =0;

    /** finds event phase of a track from the trailing edge */
    virtual double findPhaseFromTE(const Trk::Track *track) const =0;

  };

  inline const InterfaceID& InDet::IInDetCosmicsEventPhaseTool::interfaceID()
    { 
      return IID_IInDetCosmicsEventPhaseTool; 
    }

} // end of namespace

#endif 
