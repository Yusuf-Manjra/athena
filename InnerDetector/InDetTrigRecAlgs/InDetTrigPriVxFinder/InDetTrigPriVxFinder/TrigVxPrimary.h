/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************************
/**
// filename: TrigVxPrimary.h
//
// author: Adapted for the Event Filter: Iwona Grabowska-Bold, Nov 2005
//         Iwona.Grabowska@cern.ch
//
// Description:  Trigger version of the InDetPriVxFinder
//               (see original package documentation).
//
// -------------------------------
// ATLAS Collaboration
*/
//***************************************************************************

#ifndef INDETTRIGPRIVXFINDER_VXPRIMARY_H
#define INDETTRIGPRIVXFINDER_VXPRIMARY_H

#include "GaudiKernel/ToolHandle.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"

#include <vector>

/** Primary Vertex Finder.  InDetTrigPriVxFinder uses the
  InDetPrimaryVertexFinderTool in the package
  InnerDetector/InDetRecTools/InDetPriVxFinderTool. It only gives the
  trackcollection from storegate to it and records the returned
  VxContainer.
 */

/* Forward declarations */

namespace MagField {
  class IMagFieldSvc;
}

namespace InDet
{
  class IVertexFinder;


  class TrigVxPrimary : public HLT::FexAlgo
  {
  public:
    TrigVxPrimary(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrigVxPrimary();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);    
    HLT::ErrorCode hltFinalize();
    
  private:
    int m_nTracks;
    int m_nVertices;
    std::vector<float>   m_zOfPriVtx;
    std::vector<float>   m_zOfPileUp;
    std::vector<float>   m_zOfNoVtx;
    bool                 m_runWithoutField;
    
    ToolHandle< IVertexFinder > m_VertexFinderTool;
    ServiceHandle< MagField::IMagFieldSvc> m_fieldSvc;


  };
}
#endif
