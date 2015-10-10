/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscOverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// iFatras
#include "ISF_FatrasDetDescrModel/DiscOverlapDescriptor.h"
#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"
// Trk
#include "TrkSurfaces/Surface.h"
#include "TrkParameters/TrackParameters.h"

iFatras::DiscOverlapDescriptor::DiscOverlapDescriptor(const Trk::BinnedArray<Trk::Surface>* bin_array, 
						      const std::vector<Trk::BinUtility*>* singleBinUtils):
						      m_bin_array(bin_array),
						      m_singleBinUtils(singleBinUtils)
{}

/** get the compatible surfaces */
bool iFatras::DiscOverlapDescriptor::reachableSurfaces(std::vector<Trk::SurfaceIntersection>& surfaces, 
                                                         const Trk::Surface& tsf,
                                                         const Amg::Vector3D& pos,
                                                         const Amg::Vector3D&) const
                                                        
{
  // get the according detector element
  const iFatras::PlanarDetElement* pElement  = dynamic_cast<const iFatras::PlanarDetElement*>(tsf.associatedDetectorElement());

  // first add the target surface
  surfaces.push_back(Trk::SurfaceIntersection(Trk::Intersection(pos, 0., true),&tsf));
  
  // return empty cell vector
  if (pElement) {

    addOtherSide(pElement,surfaces); 

    addNextInPhi(pElement,surfaces);
    addPrevInPhi(pElement,surfaces);
    
    // First use the bin utiliy to find the surfaces on the same disc but in different rings
    // This is done as follows:
    // 1 -> Find at which bin corresponds the surface, i.e. at which ring on the disc it corresponds
    // 2 -> If it is not 0 (i.e. the surface doen't belong to the innermost ring), 
    // look for the surfaces in the ring with smaller eta value (it returns 2 or 3 surfaces)
    // 3 -> If it is smaller than the number of rings (i.e. the surface doen't belong to the outermost ring), 
    // look for the surfaces in the ring with bigger eta value (it returns 2 or 3 surfaces)// 

    const Trk::Surface* samePhi_PrevEta     = NULL;
    const Trk::Surface* previousPhi_PrevEta = NULL;
    const Trk::Surface* nextPhi_PrevEta     = NULL;
    
    const Trk::Surface* samePhi_NextEta     = NULL;
    const Trk::Surface* previousPhi_NextEta = NULL;
    const Trk::Surface* nextPhi_NextEta     = NULL;

    if (m_bin_array && m_singleBinUtils->size() != 0){
      
      const Amg::Vector3D center = tsf.center();
      
      unsigned int Util = 0;
      unsigned int binSurf = 0;
      for (; Util < m_singleBinUtils->size(); Util++) {
	if((m_singleBinUtils->at(Util))->inside(center)) {
	  binSurf = (m_singleBinUtils->at(Util))->bin(center);
	  break;   
	}
      }

      
      int prevUtil = Util - 1;
      int nextUtil = Util + 1;
      
      size_t offset = 0;
      for (unsigned int utils = 0; utils<Util; utils++)
	offset+=(m_singleBinUtils->at(utils))->dimensions();
      
      if (prevUtil >= 0) {

	size_t start_previous = offset - (m_singleBinUtils->at(prevUtil))->bins();//dimensions();
	size_t end_previous = offset;
	size_t currentBinSurf = (end_previous + binSurf -1);

	// look for same/next/previous phi in the ring with smaller eta value.
	// If there is an element with exactly the same phi we can return 3 elements 
	// If there is not an element with exactly the same phi we can return 2 elements (previous/next only)
        
	std::vector<const Trk::Surface*> surf = m_bin_array->arrayObjects();
	
	double PrevDeltaPhi = 9999.;
	double NextDeltaPhi = -9999.;

	for (size_t prev_Surf = start_previous; prev_Surf < end_previous; prev_Surf++ ) {

	  if( (*(surf.at(currentBinSurf))).center().phi() == (*(surf.at(prev_Surf))).center().phi() )
	    samePhi_PrevEta = surf.at(prev_Surf);
	  
	  double DeltaPhi = ((*(surf.at(currentBinSurf))).center().phi() - (*(surf.at(prev_Surf))).center().phi());
	  
	  if( DeltaPhi < PrevDeltaPhi && DeltaPhi > 0) {
	    previousPhi_PrevEta = surf.at(prev_Surf);
	    PrevDeltaPhi = DeltaPhi;
	  }
	  
	  if( DeltaPhi > NextDeltaPhi && DeltaPhi < 0) {
	    nextPhi_PrevEta = surf.at(prev_Surf);
	    NextDeltaPhi = DeltaPhi;
	  }    
	}
      }

      if ((unsigned int)nextUtil < m_singleBinUtils->size()) {

	size_t start_next = offset + (m_singleBinUtils->at(Util))->bins();
	size_t end_next = start_next + (m_singleBinUtils->at(nextUtil))->bins();
	size_t currentBinSurf = offset + binSurf;

	// look for same/next/previous phi in the ring with bigger eta value.
	// If there is an element with exactly the same phi we can return 3 elements 
	// If there is not an element with exactly the same phi we can return 2 elements (previous/next only)
        
	std::vector<const Trk::Surface*> surf = m_bin_array->arrayObjects();
	
	double PrevDeltaPhi = 9999.;
	double NextDeltaPhi = -9999.;

	for (size_t next_Surf = start_next; next_Surf < end_next; next_Surf++ ) {

	  if( (*(surf.at(currentBinSurf))).center().phi() == (*(surf.at(next_Surf))).center().phi() )
	    samePhi_NextEta = surf.at(next_Surf);

	  double DeltaPhi = ((*(surf.at(currentBinSurf))).center().phi() - (*(surf.at(next_Surf))).center().phi());
	  
	  if( DeltaPhi < PrevDeltaPhi && DeltaPhi > 0) {
	    previousPhi_NextEta = surf.at(next_Surf);
	    PrevDeltaPhi = DeltaPhi;
	  }
	  
	  if( DeltaPhi > NextDeltaPhi && DeltaPhi < 0) {
	    nextPhi_NextEta = surf.at(next_Surf);
	    NextDeltaPhi = DeltaPhi;
	  }    
	}
	
      }
    }
    
    if (m_bin_array && m_singleBinUtils->size() != 0){
      
      if (samePhi_PrevEta) {
	const iFatras::PlanarDetElement* PhiEta_Element = dynamic_cast<const iFatras::PlanarDetElement*>(samePhi_PrevEta->associatedDetectorElement());
	if (PhiEta_Element) {
	  addSurface(PhiEta_Element,surfaces);
	  addOtherSide(PhiEta_Element, surfaces);
	}
      }

      if (previousPhi_PrevEta) {
	const iFatras::PlanarDetElement* PhiEta_Element = dynamic_cast<const iFatras::PlanarDetElement*>(previousPhi_PrevEta->associatedDetectorElement());
	if (PhiEta_Element) {
	  addSurface(PhiEta_Element,surfaces);
	  addOtherSide(PhiEta_Element, surfaces);
	}
      }

      if (nextPhi_PrevEta) {
	const iFatras::PlanarDetElement* PhiEta_Element = dynamic_cast<const iFatras::PlanarDetElement*>(nextPhi_PrevEta->associatedDetectorElement());
	if (PhiEta_Element) {
	  addSurface(PhiEta_Element,surfaces);
	  addOtherSide(PhiEta_Element, surfaces);
	}
      }

      if (samePhi_NextEta) {
	const iFatras::PlanarDetElement* PhiEta_Element = dynamic_cast<const iFatras::PlanarDetElement*>(samePhi_NextEta->associatedDetectorElement());
	if (PhiEta_Element) {
	  addSurface(PhiEta_Element,surfaces);
	  addOtherSide(PhiEta_Element, surfaces);
	}
      }
      
      if (previousPhi_NextEta) {
	const iFatras::PlanarDetElement* PhiEta_Element = dynamic_cast<const iFatras::PlanarDetElement*>(previousPhi_NextEta->associatedDetectorElement());
	if (PhiEta_Element) {
	  addSurface(PhiEta_Element,surfaces);
	  addOtherSide(PhiEta_Element, surfaces);
	}
      }

      if (nextPhi_NextEta) {
	const iFatras::PlanarDetElement* PhiEta_Element = dynamic_cast<const iFatras::PlanarDetElement*>(nextPhi_NextEta->associatedDetectorElement());
	if (PhiEta_Element) {
	  addSurface(PhiEta_Element,surfaces);
	  addOtherSide(PhiEta_Element, surfaces);
	}
      }
    }
  }

  return false;

}
