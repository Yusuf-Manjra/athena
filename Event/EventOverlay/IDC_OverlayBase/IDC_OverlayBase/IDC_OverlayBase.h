/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*- 

/**
 * @file
 * 
 * Common base class and generic overlaying code for boolean-like hits.
 * Factored out from InDetOverlay.
 *
 * @author Andrei Gaponenko <agaponenko@lbl.gov>, 2006-2009
 *
 */

#ifndef IDC_OVERLAYBASE_H
#define IDC_OVERLAYBASE_H

#include <string>

#include "OverlayAlgBase/OverlayAlgBase.h"
#include "IDC_OverlayBase/IDC_OverlayCommon.h"

class IDC_OverlayBase;

namespace Overlay {

  /**
   *  Merging of hits on the same channel.  Modifies the first argument by adding data from the second..
   *  Implementations can assume that the arguments have the same Id.  (That is, correspond to the same readout channel.)
   *  
   *  A generic implementation of this template is provided in this package, 
   *  but subdetectors can provide specializations suitable for their RDO type.
   *  A simple example of providing a specialization is in InDetOverlay.cxx (for TRT_RDORawData).
   *  A more elaborate implementation can be found e.g. in MdtOverlay.cxx
   *  
   *  Note that a declaration of a specialized template must occur before it is used
   *  by overlayContainer()/mergeCollections() [typically from the execute() method of your algorithm].
   */
  template<class Datum> void mergeChannelData(Datum& r1, const Datum& r2, IDC_OverlayBase* parent);

  /**
   *  Adds data from the second collection to the first merging where necessary.
   *  After this call the "data" collection contains all information, and the "mc"
   *  collection is empty.
   * 
   *  A generic implementation in the .icc file can be overriden by a
   *  specialization, see above.
   */

  template<class Collection> void mergeCollectionsNew(Collection *data_coll, Collection *mc_coll, IDC_OverlayBase* parent);
}


class IDC_OverlayBase : public OverlayAlgBase  {
public:
  
  IDC_OverlayBase(const std::string &name, ISvcLocator *pSvcLocator)
    : OverlayAlgBase(name, pSvcLocator)
  {}

  /**
   *  Transfers all collection from the second argument the first merging where necessary.
   *  After this call the "data" container contains all information, and the "mc" 
   *  container is empty.
   */
  template<class IDC_Container> void overlayContainer(IDC_Container* data, IDC_Container* mc) {
    Overlay::overlayContainer(data, mc, this);
  }

  template<class IDC_Container> void overlayContainer(const std::auto_ptr<IDC_Container>& data, const std::auto_ptr<IDC_Container>& mc) {
    this->overlayContainer(data.get(), mc.get());
  }

  template<class IDC_Container> void overlayContainerNew(IDC_Container* data, IDC_Container* mc) {
    Overlay::overlayContainerNew(data, mc, this);
  }

  template<class IDC_Container> void overlayContainerNew(const std::auto_ptr<IDC_Container>& data, const std::auto_ptr<IDC_Container>& mc) {
    this->overlayContainerNew(data.get(), mc.get());
  }

  template<class IDC_Container> std::string shortPrint(const IDC_Container *container, unsigned numprint = 25) {
    return Overlay::shortPrint(container, numprint);
  }
  
  template<class IDC_Container> std::string shortPrint(const std::auto_ptr<IDC_Container>& ac, unsigned numprint = 25) {
    return Overlay::shortPrint(ac, numprint);
  }

  /**
   *  Adds data from the second collection to the first merging where necessary.
   *  After this call the "data" collection contains all information, and the "mc"
   *  collection is empty.
   */
  template<class Collection> void mergeCollections(Collection *data_coll, Collection *mc_coll);

};

#include "IDC_OverlayBase/IDC_OverlayBase.icc"

#endif/*IDC_OVERLAYBASE_H*/
