/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsDetectorElement.h"

// ATHENA
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h"
#include "TRT_ReadoutGeometry/TRT_BarrelElement.h"
#include "ActsInterop/IdentityHelper.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// PACKAGE
#include "ActsGeometry/ActsTrackingGeometrySvc.h"
#include "ActsGeometry/ActsAlignmentStore.h"
#include "ActsGeometry/ActsGeometryContext.h"

// ACTS
#include "Acts/Surfaces/StrawSurface.hpp"
#include "Acts/Surfaces/LineBounds.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/Surfaces/TrapezoidBounds.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Utilities/Units.hpp"

// STL
#include <mutex>

// BOOST
#include <boost/variant.hpp>
#include <boost/variant/get.hpp>


using Acts::Transform3D;
using Acts::Surface;

using namespace Acts::UnitLiterals;

constexpr double length_unit = 1_mm;

ActsDetectorElement::ActsDetectorElement(
    const InDetDD::SiDetectorElement* detElem)
{
  m_detElement = detElem;

  //auto center     = detElem->center();
  auto boundsType = detElem->bounds().type();

  // thickness
  m_thickness = detElem->thickness();

  if (boundsType == Trk::SurfaceBounds::Rectangle) {

    const InDetDD::SiDetectorDesign &design = detElem->design();
    double hlX = design.width()/2. * length_unit;
    double hlY = design.length()/2. * length_unit;

    auto rectangleBounds = std::make_shared<const Acts::RectangleBounds>(
        hlX, hlY);

    m_bounds = rectangleBounds;

    m_surface = Acts::Surface::makeShared<Acts::PlaneSurface>(rectangleBounds, *this);

  } else if (boundsType == Trk::SurfaceBounds::Trapezoid) {

    std::string shapeString = detElem->getMaterialGeom()->getLogVol()->getShape()->type();
    //std::cout << __FUNCTION__ << "tapezoid, GeoLogVol -> shape says: " << shapeString << std::endl;

    const InDetDD::SiDetectorDesign &design = detElem->design();

    double minHlX = design.minWidth()/2. * length_unit;
    double maxHlX = design.maxWidth()/2. * length_unit;
    double hlY    = design.length()/2. * length_unit;

    auto trapezoidBounds = std::make_shared<const Acts::TrapezoidBounds>(
        minHlX, maxHlX, hlY);

    m_bounds = trapezoidBounds;

    m_surface = Acts::Surface::makeShared<Acts::PlaneSurface>(trapezoidBounds, *this);

  } else {
    throw std::domain_error("ActsDetectorElement does not support this surface type");
  }
}

ActsDetectorElement::ActsDetectorElement(
    std::shared_ptr<const Transform3D> trf,
    const InDetDD::TRT_BaseElement* detElem,
    const Identifier& id)
{
  m_detElement = detElem;
  m_defTransform = trf;
  m_explicitIdentifier = id;

  // we know this is a straw
  double length = detElem->strawLength()*0.5 * length_unit;

  // we need to find the radius
  auto ecElem = dynamic_cast<const InDetDD::TRT_EndcapElement*>(detElem);
  auto brlElem = dynamic_cast<const InDetDD::TRT_BarrelElement*>(detElem);
  double innerTubeRadius{0.};
  if (ecElem) {
    innerTubeRadius = ecElem->getDescriptor()->innerTubeRadius() * length_unit;
  }
  else {
    if (brlElem){
      innerTubeRadius = brlElem->getDescriptor()->innerTubeRadius() * length_unit;
    } else {
      throw std::runtime_error("Cannot get tube radius for element in ActsDetectorElement c'tor");
    }
  }

  auto lineBounds = std::make_shared<const Acts::LineBounds>(innerTubeRadius, length);
  m_bounds = lineBounds;

  m_surface = Acts::Surface::makeShared<Acts::StrawSurface>(lineBounds, *this);
}

IdentityHelper
ActsDetectorElement::identityHelper() const
{
  size_t which = m_detElement.which();
  if (which == 0) {
    return IdentityHelper(boost::get<const InDetDD::SiDetectorElement*>(m_detElement));
  } else {
    throw std::domain_error("Cannot get IdentityHelper for TRT element");
  }
}

const Acts::Transform3D&
ActsDetectorElement::transform(const Acts::GeometryContext& anygctx) const
{
  // any cast to known context type
  const ActsGeometryContext* gctx = std::any_cast<const ActsGeometryContext*>(anygctx);

  // This is needed for initial geometry construction. At that point, we don't have a
  // consistent view of the geometry yet, and thus we can't populate an alignment store
  // at that time.
  if (gctx->construction) {
    // this should only happen at initialize (1 thread, but mutex anyway)
    return getDefaultTransformMutexed();
  }

  // unpack the alignment store from the context
  const ActsAlignmentStore* alignmentStore = gctx->alignmentStore;

  // no GAS, is this initialization?
  assert(alignmentStore != nullptr);

  // get the correct cached transform
  // units should be fine here since we converted at construction
  const Transform3D* cachedTrf = alignmentStore->getTransform(this);

  assert(cachedTrf != nullptr);

  return *cachedTrf;
}

void
ActsDetectorElement::storeTransform(ActsAlignmentStore* gas) const
{
  struct get_transform : public boost::static_visitor<Transform3D>
  {
    get_transform(ActsAlignmentStore* gas2, const Transform3D* trtTrf)
      : m_store(gas2), m_trtTrf(trtTrf) {}

    Transform3D operator()(const InDetDD::SiDetectorElement* detElem) const
    {
      Amg::Transform3D g2l
        = detElem->getMaterialGeom()->getAbsoluteTransform(m_store)
        * Amg::CLHEPTransformToEigen(detElem->recoToHitTransform());

      // need to make sure translation has correct units
      g2l.translation() *= length_unit;

      return g2l;
    }

    Transform3D operator()(const InDetDD::TRT_BaseElement*) const
    {
      return *m_trtTrf;
    }

    ActsAlignmentStore* m_store;
    const Transform3D* m_trtTrf;
  };

  Transform3D trf
    = boost::apply_visitor(get_transform(gas, m_defTransform.get()), m_detElement);

  gas->setTransform(this, trf);
  if (gas->getTransform(this) == nullptr) {
    throw std::runtime_error("Detector element was unable to store transform in GAS");
  }

}

const Acts::Transform3D&
ActsDetectorElement::getDefaultTransformMutexed() const
{
  struct get_default_transform : public boost::static_visitor<Transform3D>
  {
    get_default_transform(const Transform3D* trtTrf) : m_trtTrf(trtTrf) {}

    Transform3D operator()(const InDetDD::SiDetectorElement* detElem) const
    {
      Amg::Transform3D g2l
        = detElem->getMaterialGeom()->getDefAbsoluteTransform()
        * Amg::CLHEPTransformToEigen(detElem->recoToHitTransform());

      // need to make sure translation has correct units
      g2l.translation() *= length_unit;

      return g2l;
    }

    Transform3D operator()(const InDetDD::TRT_BaseElement*) const
    {
      return *m_trtTrf;
    }

    const Transform3D* m_trtTrf;
  };

  std::lock_guard<std::mutex> guard(m_cacheMutex);
  if (m_defTransform) {
    return *m_defTransform;
  }
  // transform not yet set
  m_defTransform
    = std::make_shared<const Transform3D>(
        boost::apply_visitor(get_default_transform(m_defTransform.get()), m_detElement));

  return *m_defTransform;
}

const Acts::Surface&
ActsDetectorElement::surface() const
{
  return (*m_surface);
}

double
ActsDetectorElement::thickness() const
{
  return m_thickness;
}

Identifier
ActsDetectorElement::identify() const
{
  return boost::apply_visitor(IdVisitor(m_explicitIdentifier), m_detElement);
}
