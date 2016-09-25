/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscOverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// iFatras
#include "InDetTrackingGeometryUtils/DiscOverlapDescriptor.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
// Trk
#include "TrkSurfaces/Surface.h"

#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"

InDet::DiscOverlapDescriptor::DiscOverlapDescriptor(bool isPixel,
						    const Trk::BinnedArray<Trk::Surface>* bin_array, 
						    std::vector<Trk::BinUtility*>* singleBinUtils):
  m_bin_array(bin_array),
  m_singleBinUtils(singleBinUtils),
  m_pixelCase(isPixel)
{}

/** get the compatible surfaces */
bool InDet::DiscOverlapDescriptor::reachableSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces, 
						     const Trk::Surface& tsf,
						     const Amg::Vector3D& pos,
						     const Amg::Vector3D&) const
  
{
  // Get Storegate, ID helpers, and so on
  ISvcLocator* svcLocator = Gaudi::svcLocator();
    
  // get DetectorStore service
  StoreGateSvc* detStore;
  StatusCode sc = svcLocator->service("DetectorStore",detStore);
  if (sc.isFailure()) {
    return 0;
  }

  const PixelID* m_pixIdHelper = 0;
  const SCT_ID*  m_sctIdHelper = 0;
  
  if (m_pixelCase) {
    if (detStore->retrieve(m_pixIdHelper, "PixelID").isFailure()) {
      return false;
    }
  } else {
    if (detStore->retrieve(m_sctIdHelper, "SCT_ID").isFailure()) {
      return false;
    }
  }
  
  // get the according detector element
  const InDetDD::SiDetectorElement* pElement  = dynamic_cast<const InDetDD::SiDetectorElement*>(tsf.associatedDetectorElement());
  
  // first add the target surface
  surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(pos, 0., true),&tsf));
  int etaModule = m_pixelCase ? m_pixIdHelper->eta_module(tsf.associatedDetectorElementIdentifier()) : m_sctIdHelper->eta_module(tsf.associatedDetectorElementIdentifier());
  
  // return empty cell vector
  if (pElement) {
    
    addOtherSideDO(pElement,surfaces); 
    
    addNextInPhiDO(pElement,surfaces);
    addPrevInPhiDO(pElement,surfaces);
        
    addNextInPhiDO(pElement->nextInPhi(),surfaces);
    addPrevInPhiDO(pElement->prevInPhi(),surfaces);
    
    // First use the bin utiliy to find the surfaces on the same disc but in different rings
    // This is done as follows:
    // 1 -> Find at which bin corresponds the surface, i.e. at which ring on the disc it corresponds
    // 2 -> If it is not 0 (i.e. the surface doesn't belong to the innermost ring), 
    // look for the surfaces in the ring with smaller eta value (it returns 2 or 3 surfaces)
    // 3 -> If it is smaller than the number of rings (i.e. the surface doen't belong to the outermost ring), 
    // look for the surfaces in the ring with bigger eta value (it returns 2 or 3 surfaces)// 

    if (m_bin_array && m_singleBinUtils->size() != 0){

      const Trk::Surface* samePhi_PrevEta     = NULL;
      const Trk::Surface* samePhi_NextEta     = NULL;
      
      const Trk::Surface* previousPhi_PrevEta = NULL;
      const Trk::Surface* nextPhi_PrevEta     = NULL;
      
      const Trk::Surface* previousPhi_NextEta = NULL;
      const Trk::Surface* nextPhi_NextEta     = NULL;

      std::vector<const Trk::Surface*> surf = m_bin_array->arrayObjects();
      size_t offset = 0;
      for (unsigned int bin = 0; bin < m_singleBinUtils->size(); bin++) {
	int etamod = m_pixelCase ? m_pixIdHelper->eta_module((*(surf.at(offset))).associatedDetectorElementIdentifier()) : m_sctIdHelper->eta_module((*(surf.at(offset))).associatedDetectorElementIdentifier());
	
	if (etamod == etaModule || etamod<(etaModule-1) || etamod>(etaModule+1)) {
	  offset += (m_singleBinUtils->at(bin))->bins();
	  continue;	  
	}
	
	double PrevDeltaPhi = 9999.;
	double NextDeltaPhi = -9999.;
	
	for (unsigned int ss = offset; ss < (offset+(m_singleBinUtils->at(bin))->bins()); ss++ ) {
	  
	  if (etamod == (etaModule-1) ) {
	    if( tsf.center().phi() == (*(surf.at(ss))).center().phi() )
	      samePhi_PrevEta = surf.at(ss);
	    
	    double DeltaPhi = tsf.center().phi() - (*(surf.at(ss))).center().phi();
	    if( DeltaPhi < PrevDeltaPhi && DeltaPhi > 0) {
	      previousPhi_PrevEta = surf.at(ss);
	      PrevDeltaPhi = DeltaPhi;
	    }
	  
	    if( DeltaPhi > NextDeltaPhi && DeltaPhi < 0) {
	      nextPhi_PrevEta = surf.at(ss);
	      NextDeltaPhi = DeltaPhi;
	    }
	  } else if (etamod == (etaModule+1) ) {
	    if( tsf.center().phi() == (*(surf.at(ss))).center().phi() )
	      samePhi_NextEta = surf.at(ss);
	    
	    double DeltaPhi = tsf.center().phi() - (*(surf.at(ss))).center().phi();
	    
	    if( DeltaPhi < PrevDeltaPhi && DeltaPhi > 0) {
	      previousPhi_NextEta = surf.at(ss);
	      PrevDeltaPhi = DeltaPhi;
	    }
	    
	    if( DeltaPhi > NextDeltaPhi && DeltaPhi < 0) {
	      nextPhi_NextEta = surf.at(ss);
	      NextDeltaPhi = DeltaPhi;
	    }    
	  }
	}
	offset += (m_singleBinUtils->at(bin))->bins();
      }
      
      if (samePhi_PrevEta) {
  	const InDetDD::SiDetectorElement* PhiEta_Element = dynamic_cast<const InDetDD::SiDetectorElement*>(samePhi_PrevEta->associatedDetectorElement());
  	if (PhiEta_Element) {
  	  addSurfaceDO(PhiEta_Element,surfaces);
  	  addOtherSideDO(PhiEta_Element, surfaces);
  	}
      }

      if (previousPhi_PrevEta) {
  	const InDetDD::SiDetectorElement* PhiEta_Element = dynamic_cast<const InDetDD::SiDetectorElement*>(previousPhi_PrevEta->associatedDetectorElement());
  	if (PhiEta_Element) {
  	  addSurfaceDO(PhiEta_Element,surfaces);
  	  addOtherSideDO(PhiEta_Element, surfaces);
	  
  	  addPrevInPhiDO(PhiEta_Element,surfaces);
  	  addPrevInPhiDO(PhiEta_Element->prevInPhi(),surfaces);
  	}
      }
      
      if (nextPhi_PrevEta) {
  	const InDetDD::SiDetectorElement* PhiEta_Element = dynamic_cast<const InDetDD::SiDetectorElement*>(nextPhi_PrevEta->associatedDetectorElement());
  	if (PhiEta_Element) {
  	  addSurfaceDO(PhiEta_Element,surfaces);
  	  addOtherSideDO(PhiEta_Element, surfaces);
	  
  	  addNextInPhiDO(PhiEta_Element,surfaces);
  	  addNextInPhiDO(PhiEta_Element->nextInPhi(),surfaces);
  	}
      }
      
      if (samePhi_NextEta) {
  	const InDetDD::SiDetectorElement* PhiEta_Element = dynamic_cast<const InDetDD::SiDetectorElement*>(samePhi_NextEta->associatedDetectorElement());
  	if (PhiEta_Element) {
  	  addSurfaceDO(PhiEta_Element,surfaces);
  	  addOtherSideDO(PhiEta_Element, surfaces);
  	}
      }
      
      if (previousPhi_NextEta) {
  	const InDetDD::SiDetectorElement* PhiEta_Element = dynamic_cast<const InDetDD::SiDetectorElement*>(previousPhi_NextEta->associatedDetectorElement());
  	if (PhiEta_Element) {
  	  addSurfaceDO(PhiEta_Element,surfaces);
  	  addOtherSideDO(PhiEta_Element, surfaces);
	  
  	  addPrevInPhiDO(PhiEta_Element,surfaces);
  	  addPrevInPhiDO(PhiEta_Element->prevInPhi(),surfaces);
  	}
      }
      
      if (nextPhi_NextEta) {
  	const InDetDD::SiDetectorElement* PhiEta_Element = dynamic_cast<const InDetDD::SiDetectorElement*>(nextPhi_NextEta->associatedDetectorElement());
  	if (PhiEta_Element) {
  	  addSurfaceDO(PhiEta_Element,surfaces);
  	  addOtherSideDO(PhiEta_Element, surfaces);
	  
  	  addNextInPhiDO(PhiEta_Element,surfaces);
  	  addNextInPhiDO(PhiEta_Element->nextInPhi(),surfaces);
  	}
      }
    }
  }
  
  //dumpSurfaces(m_pixIdHelper, m_sctIdHelper, surfaces);
  return false;

}

void InDet::DiscOverlapDescriptor::dumpSurfaces(const PixelID* m_pixIdHelper, const SCT_ID* m_sctIdHelper, std::vector<Trk::SurfaceIntersection>& surfaces) const {
  std::cout << "Dumping Surfaces for "<< (m_pixIdHelper ? "Pixel " : "SCT ") << "with size = " << surfaces.size() << std::endl;
  for (unsigned int surf = 0; surf < surfaces.size(); surf++) {
    Identifier hitId = ((surfaces.at(surf)).object)->associatedDetectorElementIdentifier(); 
    if (m_pixIdHelper)
      std::cout <<  "barrel_ec " << m_pixIdHelper->barrel_ec(hitId) << ", layer_disk " << m_pixIdHelper->layer_disk(hitId) << ", phi_module " << m_pixIdHelper->phi_module(hitId) << ", eta_module " << m_pixIdHelper->eta_module(hitId) << std::endl;
    else if (m_sctIdHelper)
      std::cout <<  "barrel_ec " << m_sctIdHelper->barrel_ec(hitId) << ", layer_disk " << m_sctIdHelper->layer_disk(hitId) << ", phi_module " << m_sctIdHelper->phi_module(hitId) << ", eta_module " << m_sctIdHelper->eta_module(hitId) << ", side " << m_sctIdHelper->side(hitId) << std::endl;
  }
}


