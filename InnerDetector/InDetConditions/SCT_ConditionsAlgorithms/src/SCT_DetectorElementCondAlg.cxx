/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_DetectorElementCondAlg.h"

#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/Surface.h"
#include "AthenaKernel/IOVInfiniteRange.h"

#include <map>

SCT_DetectorElementCondAlg::SCT_DetectorElementCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator)
  , m_readKey{"SCTAlignmentStore", "SCTAlignmentStore"}
  , m_detManagerName("SCT")
{
  declareProperty("ReadKey", m_readKey);
  declareProperty("DetManagerName", m_detManagerName);
}

StatusCode SCT_DetectorElementCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // Read Handle
  ATH_CHECK(m_readKey.initialize());

  // Write Handle
  ATH_CHECK(m_writeKey.initialize());
  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());
  // Register write handle
  ATH_CHECK(m_condSvc->regHandle(this, m_writeKey));
  ATH_CHECK(detStore()->retrieve(m_detManager, m_detManagerName));

  // used only if they exist
  ATH_CHECK(m_trtDetElContKey.initialize());
  ATH_CHECK(m_muonManagerKey.initialize());
  ATH_CHECK(m_pixelReadKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode SCT_DetectorElementCondAlg::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handle and check its validity ____________
  SG::WriteCondHandle<InDetDD::SiDetectorElementCollection> writeHandle{m_writeKey, ctx};

  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  const InDetDD::SiDetectorElementCollection* oldColl{m_detManager->getDetectorElementCollection()};
  if (oldColl==nullptr) {
    ATH_MSG_FATAL("Null pointer is returned by getDetectorElementCollection()");
    return StatusCode::FAILURE;
  }

  // ____________ Construct new Write Cond Object ____________
  std::unique_ptr<InDetDD::SiDetectorElementCollection> writeCdo{std::make_unique<InDetDD::SiDetectorElementCollection>()};

  // ____________ Get Read Cond Object ____________
  SG::ReadCondHandle<GeoAlignmentStore> readHandle{m_readKey, ctx};
  const GeoAlignmentStore* readCdo{*readHandle};
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object of " << m_readKey.key());
    return StatusCode::FAILURE;
  }

  // Make sure we make a mixed IOV.
  writeHandle.addDependency (IOVInfiniteRange::infiniteMixed());
  
  // Add dependency
  writeHandle.addDependency(readHandle);
  // Additional dependencies for IOV range to limit lifetime to TrackingGeometry lifetime
  for (const SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> &key :m_muonManagerKey ) {
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> muonDependency{key, ctx};
    if (*muonDependency != nullptr){
      writeHandle.addDependency(muonDependency);
    } else {
    ATH_MSG_WARNING("MuonManager not found, ignoring Muons for SCT_DetElement lifetime");
    }
  }
  for (const SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> &key :m_trtDetElContKey ) {
    SG::ReadCondHandle<InDetDD::TRT_DetElementContainer> trtDependency{key, ctx};
    if (*trtDependency != nullptr){
      writeHandle.addDependency(trtDependency);
    } else {
      ATH_MSG_WARNING("TRT DetEls not found, ignoring TRT for SCT_DetElement lifetime");
    }
  }
  for (const SG::ReadCondHandleKey<GeoAlignmentStore> &key :m_pixelReadKey ) {
    SG::ReadCondHandle<GeoAlignmentStore> pixelDependency{key, ctx};
    if (*pixelDependency != nullptr){
      writeHandle.addDependency(pixelDependency);
    } else {
      ATH_MSG_WARNING("Pixel AlignmentStore not found, ignoring Pixels for SCT_DetElement lifetime");
    }
  }

  // ____________ Update writeCdo using readCdo ____________
  std::map<const InDetDD::SiDetectorElement*, const InDetDD::SiDetectorElement*> oldToNewMap;
  oldToNewMap[nullptr] = nullptr;
  writeCdo->resize(oldColl->size(), nullptr);
  InDetDD::SiDetectorElementCollection::iterator newEl{writeCdo->begin()};
  for (const InDetDD::SiDetectorElement* oldEl: *oldColl) {
    *newEl = new InDetDD::SiDetectorElement(oldEl->identify(),
                                            &(oldEl->design()),
                                            oldEl->GeoVDetectorElement::getMaterialGeom(),
                                            oldEl->getCommonItems(),
                                            readCdo);
    oldToNewMap[oldEl] = *newEl;
    ++newEl;
  }

  // Set neighbours and other side
  // Set layer to surface
  InDetDD::SiDetectorElementCollection::const_iterator oldIt{oldColl->begin()};
  for (InDetDD::SiDetectorElement* newEl: *writeCdo) {
    if (oldToNewMap[(*oldIt)]!=newEl) {
      ATH_MSG_ERROR("Old and new elements are not synchronized!");
    }
    newEl->setNextInEta(oldToNewMap[(*oldIt)->nextInEta()]);
    newEl->setPrevInEta(oldToNewMap[(*oldIt)->prevInEta()]);
    newEl->setNextInPhi(oldToNewMap[(*oldIt)->nextInPhi()]);
    newEl->setPrevInPhi(oldToNewMap[(*oldIt)->prevInPhi()]);
    newEl->setOtherSide(oldToNewMap[(*oldIt)->otherSide()]);
    // Layer of old element is set by InDet::SiLayerBuilder::registerSurfacesToLayer.
    const Trk::Layer* layer{(*oldIt)->surface().associatedLayer()};
    if (layer) {
      newEl->surface().associateLayer(*layer);
    }
    ++oldIt;
  }

  // Apply alignment using readCdo passed to SiDetectorElement
  for (InDetDD::SiDetectorElement* newEl: *writeCdo) {
    newEl->setCache();
  }

  // Record WriteCondHandle
  const std::size_t size{writeCdo->size()};
  if (writeHandle.record(std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record " << writeHandle.key() 
                  << " with EventRange " << writeHandle.getRange()
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << writeHandle.getRange() << " with size of " << size << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_DetectorElementCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());

  return StatusCode::SUCCESS;
}
