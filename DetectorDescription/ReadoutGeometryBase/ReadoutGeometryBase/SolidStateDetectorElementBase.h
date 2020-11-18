/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SoldiStateDetectorElementBase.h
 **/

#ifndef READOUTGEOMETRYBASE_SOLIDSTATEDETECTORELEMENTBASE_H
#define READOUTGEOMETRYBASE_SOLIDSTATEDETECTORELEMENTBASE_H

/**
 * Base class.
 */
#include "TrkDetElementBase/TrkDetElementBase.h"

#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "ReadoutGeometryBase/InDetDD_Defs.h"
#include "ReadoutGeometryBase/SiCellId.h"
#include "ReadoutGeometryBase/SiCommonItems.h"
#include "ReadoutGeometryBase/DetectorDesign.h"
#include "ReadoutGeometryBase/SiIntersect.h"
#include "ReadoutGeometryBase/SiLocalPosition.h"
#include "TrkEventPrimitives/ParamDefs.h"

#include "CLHEP/Geometry/Point3D.h"

#include <atomic>
#include <limits>
#include <memory>
#include <mutex>

class AtlasDetectorID;
class GeoAlignmentStore;
class GeoVFullPhysVol;

namespace Trk {
  class SurfaceBounds;
}

namespace InDetDD {

  class SiReadoutCellId;

  /**
   *  
   * @class SolidStateDetectorElementBase
   *
   * Class to hold geometrical description of a solid state detector element. 
   * This base class is shared between the InnerDetetector/ITk/HGTD since there is a lot of commonality.
   * 
   * @par Coordinate Frames.
   *
   * The following coordinate frames are used in these elements.
   *
   * - Global frame:\n
   * Currently global frame in G4/GeoModel. Probably eventually
   * will be global frame most suitable for reconstruction 
   * (eg solenoid axis).
   *
   * - Local hit frame:\n 
   * Local frame for hits. It is the same as local frame in G4 and GeoModel. 
   * I also refer to this as the local simulation frame. 
   * By convention elements are orientated such that:
   * - hitDepth = local x
   * - hitPhi   = local y
   * - hitEta   = local z
   * . 
   * Directions of these correspond to the physical wafer. Consequently hitDepth and hitPhi axes go in 
   * different directions depending on the orientation of the module.
   * The readout side is determined from design()->readoutSide(). 
   *
   * - Local reconstruction frame:\n 
   * - distPhi  = local x
   * - distEta  = local y  
   * - distDepth = local z
   * .
   * The directions of the axes are defined as
   * - distPhi in direction of increasing phi
   * - distEta in direction of increasing z in barrel and increasing r in endcap.
   * - distDepth (normal) choosen to give right-handed coordinate. 
   * =>  away from intersection point for barrel, decreasing z for endcap
   * 
   * @par Overview of Methods 
   *
   * Methods are grouped into the the following categories
   *
   * -  Identification
   * -  Navigation
   * -  Transformation/Orientation
   * -  Module Frame
   * -  Element Extent
   * -  Design methods
   * -  Intersection Tests
   * -  Lorentz Correction
   * -  Readout cell id
   * -  Miscellaneous
   * -  Cache handling.
   *
   *
   * @author Grant Gorfine
   * - modified & maintained: Nick Styles, Andreas Salzburger
   * - modified Nigel Hessey: get directions from the design instead of hard-wiring them   
   *
   * @par Some notes on Thread safety for  AthenaMT
   *
   * The private methods of this class do not have locks.
   *
   * The method updateCache of is of particular interest as
   * it set all cache values and at the end sets the
   * m_cacheValid atomic variable to true.
   *
   * The const methods call the updateCache() under a mutex lock
   * when the need to perform lazy initialization
   * \code{.cpp}
   * if (!m_cacheValid) {
   *   std::lock_guard<std::mutex> lock(m_mutex);
   *   if (!m_cacheValid) updateCache();
   * }
   * \endcode
   *
   * So as concurrent const operations are valid
   * and do not race with each other.
   *
   * The non-const methods do not use a mutex lock. They can set the state
   * of the cache or the cache itself (invalidate/setCache methods etc)
   *
   * Note: Synchronisation of creating SiDetElements for different events
   * and accessing for each events
   * can be done via write/read handles or similar EventContext aware
   * framework machinery.
   */
 

  class SolidStateDetectorElementBase : public Trk::TrkDetElementBase {

  public:
    
    /**
     * Constructor with parameters
     */
    SolidStateDetectorElementBase(const Identifier& id, 
                      const DetectorDesign* design,
                      const GeoVFullPhysVol* geophysvol,
                      const SiCommonItems* commonItems,
                      const GeoAlignmentStore* geoAlignStore=nullptr);
    
    /**
     * Destructor
     */
    virtual ~SolidStateDetectorElementBase();
    
    /**
     * Don't allow no-argument constructor
     */
    SolidStateDetectorElementBase() = delete;

    /**
     * Don't allow copy constructor
     */
    SolidStateDetectorElementBase(const SolidStateDetectorElementBase&) = delete;

    /**
     * Don't allow assignment operator
     */ 
    SolidStateDetectorElementBase& operator=(const SolidStateDetectorElementBase&) = delete;

    /**
     * Don't allow move constructor
     */
    SolidStateDetectorElementBase(SolidStateDetectorElementBase&&) = delete;

    /**
     * Don't allow move assignment operator
     */
    SolidStateDetectorElementBase& operator=(SolidStateDetectorElementBase&&) = delete;

    /**
     * @name Cache handling
     *
     * Methods to handle invalidating and updating caches. The cached values include values that are affected by alignment
     */
    //@{
    /**
     * Signal that cached values are no longer valid.
     * Invalidate general cache (inline)
     */
    void invalidate();

    /**
     * Set/calculate cache values (inline)
     */
    void setCache();
    //@}

    /**
     * @name Common items
     * Inline method
     */
    //@{
    const SiCommonItems* getCommonItems() const;
    //@}

    /**
     * @name Identification
     * Methods to identify the element and identifier manipulation.
     */ 
    //@{
    
    /**
     * identifier of this detector element (inline)
     */
    //do these need to be virtual? - they are in 21.9 but I don't see why
    Identifier identify() const;
    
    /**
     * identifier hash (inline)
     */
    //do these need to be virtual? - as above
    IdentifierHash identifyHash() const;
    
    /**
     * Returns the id helper (inline)
     */
    const AtlasDetectorID* getIdHelper() const;
    
    /**
     * Identifier <-> SiCellId (ie strip number or pixel eta_index,phi_index)
     * Identifier from SiCellId (ie strip number or pixel eta_index,phi_index)
     */
    virtual Identifier identifierFromCellId(const SiCellId &cellId) const = 0;
    /**
     * SiCellId from Identifier
     */
    virtual SiCellId cellIdFromIdentifier(const Identifier& identifier) const = 0;
      
    //@}

    /**
     * @name Surface
     */
    //@{
    /**
     * Element Surface
     */
    virtual Trk::Surface& surface();
    virtual const Trk::Surface& surface() const;
    /**
     * TrkDetElementBase interface (inline)
     */
    virtual const Trk::Surface& surface(const Identifier&) const;
    /**
     * @name Transformation
     */
    //@{
    /**
     * Local (simulation/hit frame) to global transform
     */
    virtual const GeoTrf::Transform3D& transformHit() const;

    /**
     * Local (reconstruction frame) to global transform
     */
    const HepGeom::Transform3D& transformCLHEP() const;
    const Amg::Transform3D& transform() const;
    /**
     * TrkDetElementBase interface (inline)
     */
    virtual const Amg::Transform3D& transform(const Identifier&) const;

    /**
     * Default Local (reconstruction frame) to global transform
     * ie with no misalignment.
     */
    const HepGeom::Transform3D defTransformCLHEP() const;
    const Amg::Transform3D defTransform() const;

    /**
     * Transform to go from local reconstruction frame to local hit frame.
     */
    const HepGeom::Transform3D recoToHitTransform() const;
    //@}

    /**
     * @name Orientation
     */
    //{@
    
    /**
     * Directions of hit depth,phi,eta axes relative to reconstruction local
     * position axes (LocalPosition). Returns +/-1. inline
     */
    double hitDepthDirection() const;
    /**
     * See previous method. inline
     */
    double hitPhiDirection() const;
    /**
     * See previous method. inline
     */
    double hitEtaDirection() const;
    
    /**
     * To determine if readout direction between online and offline needs
     * swapping, see methods swapPhiReadoutDirection() and
     * swapEtaReadoutDirection() below in "Readout Cell id" section
     *
     * Orientation. 
     * Directions.
     *  - phiAxis in same direction as increasing phi and identifier phi_index/strip. 
     *            NB. This requires some flipping of axes with repsect to the hits.
     *  - etaAxis in direction of increasing z in the barrel and increasing r in the endcap.
     *  - normal  choosen to give right-handed coordinate frame (x=normal,y=phiAxis,z=etaAxis)
     *            NB. This requires some flipping of axes with repsect to the hits.
     *
     * Get reconstruction local phi axes in global frame. 
     * In same direction as increasing phi and identifier phi_index/strip.
     */
    const HepGeom::Vector3D<double>& phiAxisCLHEP() const;
    const Amg::Vector3D& phiAxis() const;
    /**
     * Get reconstruction local eta axes in global frame. 
     * In direction of increasing z in the barrel and increasing r in the endcap.
     */
    const HepGeom::Vector3D<double>& etaAxisCLHEP() const;
    const Amg::Vector3D& etaAxis() const;

    /**
     * Get reconstruction local normal axes in global frame. Choosen to give right-handed coordinate frame.
     */
    const Amg::Vector3D& normal() const;
    /**
     * TrkDetElementBase interface (inline)
     */
    virtual const Amg::Vector3D& normal(const Identifier&) const;

    //@}

    /**
     * @name Position
     */
    //@{
    /**
     * Center in global coordinates
     */
    const Amg::Vector3D& center() const;
    /**
     * TrkDetElementBase interface (inline)
     */
    virtual const Amg::Vector3D& center(const Identifier&) const;

    /**
     * transform a hit local position into a global position (inline):
     */
    HepGeom::Point3D<double> globalPositionHit(const HepGeom::Point3D<double>& simulationLocalPos) const;
    Amg::Vector3D globalPositionHit(const Amg::Vector3D& simulationLocalPos) const;
      
    /**
     * transform a reconstruction local position into a global position (inline):
     */
    HepGeom::Point3D<double> globalPosition(const HepGeom::Point3D<double>& localPos) const;
    Amg::Vector3D globalPosition(const Amg::Vector3D& localPos) const;

    /**
     * as in previous method but for 2D local position (inline)
     */
    HepGeom::Point3D<double> globalPositionCLHEP(const Amg::Vector2D& localPos) const;
    Amg::Vector3D globalPosition(const Amg::Vector2D& localPos) const;
    
    /**
     * Simulation/Hit local frame to reconstruction local frame. 2D.
     *  TODO: Will change order of parameters at some point.
     */
    Amg::Vector2D hitLocalToLocal(double xEta, double xPhi) const;
    /**
     * Same as previuos method but 3D.
     */
    HepGeom::Point3D<double> hitLocalToLocal3D(const HepGeom::Point3D<double>& hitPosition) const;

    /**
     * transform a global position into a 2D local position (reconstruction frame) (inline)
     */
    Amg::Vector2D localPosition(const HepGeom::Point3D<double>& globalPosition) const;
    Amg::Vector2D localPosition(const Amg::Vector3D& globalPosition) const;
    
    //@}


    /**
     * @name Element Extent
     * Inline methods to get extent of element in r,phi and z.  
     */
    //@{
    double rMin() const;
    double rMax() const;
    double zMin() const;
    double zMax() const;
    double phiMin() const;
    double phiMax() const;
    
    /**
     * Method for building up region of interest table.
     * Get eta/phi extent for the element. Returns min/max eta and phi
     * and r (for barrel) or z (for endcap) Takes as input the vertex
     * spread in z (-deltaZ to +deltaZ)
     */
    void getEtaPhiRegion(double deltaZ, 
                         double& etaMin, double& etaMax,
                         double& phiMin, double& phiMax,
                         double& rz) const;
    //@}
    virtual double get_rz() const = 0;
    
    /**
     * @name Design methods
     */
    //@{
    
    /**
     * access to the local description (inline):
     */
    
    virtual const DetectorDesign& design() const;
    
    virtual const Trk::SurfaceBounds& bounds() const;
    /**
     * TrkDetElementBase interface (inline)
     */
    virtual const Trk::SurfaceBounds& bounds(const Identifier&) const;

    /**
     * Methods from design (inline)
     *
     * Width in phi direction. For the SCT endcap it returns the average width.
     */
    double width() const;
    /**
     * Min width. Needed for the SCT endcap.
     */
    double minWidth() const;
    /**
     * Max width. Needed for the SCT endcap.
     */
    double maxWidth() const;
    /**
     * Length in eta direction (z - barrel, r - endcap)
     */
    double length() const;
    double thickness() const;

    /**
     * Pitch (inline methods)
     *
     * NOTE: phiPitch is ambiguous for the Forward SCT where it varies along the strip.\n
     *       etaPitch is ambiguous for the pixel which has long pixels between FE chips.\n
     *
     * For these cases:
     *
     * phiPitch:  For SCT Forward returns pitch at center.\n
     * etaPitch:  For pixel returns average pitch. (Active_length/number_of_cells)\n
     *
     * All return pitch in distance units. 
     */
    double etaPitch() const;
    double phiPitch() const;

    /**
     * carrier type for readout. ie holes for SCT and electrons for pixels. inline
     */
    InDetDD::CarrierType carrierType() const;

    /**
     * Determine if readout direction between online and offline needs swapping.
     * Returns true if online and offline numbers run in opposite directions.
     * For strip/phi_index (inline)
     */
    bool swapPhiReadoutDirection() const;
    /**
     * For eta_index (only relevant for pixel) (inline)
     */
    bool swapEtaReadoutDirection() const;

    //@}
    
    /**
     * @name Intersection Tests
     */
    //@{
    
    /**
     * Test that it is in the active region
     *
     * Intersect has 3 states
     * bool SiIntersect::in() const // definitely in\n
     * bool SiIntersect::out() const // definitely out\n
     * bool SiIntersect::nearBoundary() const // near a boundary within the tolerances\n
     * bool SiIntersect::mayIntersect() const // in() OR nearBoundary()
     */
    SiIntersect inDetector(const Amg::Vector2D& localPosition, double phiTol, double etaTol) const;
    SiIntersect inDetector(const HepGeom::Point3D<double>& globalPosition, double phiTol, double etaTol) const;
    //@}
    
    /**
     * @name Readout cell id
     * Cell id's are the strip number in SCT and phi_index,eta_index in the pixel
     * as defined in the offline identifier. Their direction runs in the distPhi, distEta
     * direction in the Reconstruction local frame. 
     *
     * For methods taking a SiCellId (basically phi,eta index for pixel or strip for SCT) you
     * can do the following fro example:
     *   - For pixel\n 
     *     localPositionOfCell(SiCellId(phi_index,eta_index));\n
     *   - For SCT\n
     *     localPositionOfCell(SiCellId(strip));\n
     */
    //@{
    
    /**
     * Full identifier of the cell for a given position:
     * assumes a raw local position (no Lorentz shift)
     */
    Identifier identifierOfPosition(const Amg::Vector2D& localPos) const;
    /**
     * As in previous method but returns SiCellId
     */
    SiCellId cellIdOfPosition(const Amg::Vector2D& localPos) const;
    
    /**
     * Returns position (center) of cell.
     * These are the raw positions _NOT_ corrected for the Lorentz shift
     */
    Amg::Vector2D rawLocalPositionOfCell(const SiCellId& cellId) const;
    /**
     * As above
     */
    Amg::Vector2D rawLocalPositionOfCell(const Identifier& id) const;
    
    /**
     * Test if readout cell has more than one diode associated with it.
     * Number of cells sharing the same readout as this cell.
     * ie generally 1 except for ganged pixels which will be 2.
     */
    int numberOfConnectedCells(const SiCellId cellId) const;
    /**
     * Get the cell ids sharing the readout for this cell.
     * number = 0 will return the primary readout cell id.
     */
    SiCellId connectedCell(const SiCellId cellId, int number) const;

    /**
     * Recalculate cached values.
     */
    virtual void updateCache() const;
    
  private:
    /**
     * @name Private Methods
     *
     * Do not use locks.
     */
    //@{
    /**
     * Common code for constructors.
     */
    void commonConstructor();
    
    /**
     * Calculate extent in r,z and phi. The values are cached and there
     * are rMin(), rMax etc methods.
     * It is only used from updateCache
     */
    void getExtent(double& rMin, double& rMax,
                   double& zMin, double& zMax,
                   double& phiMin, double& phiMax) const;
    
    /**
     * Return the four corners of an element in local coordinates.
     * Pass it an array of length 4.
     * This function is used by getEtaPhiRegion()
     */
    void getCorners(HepGeom::Point3D<double>* corners) const;
    
    /**
     * Get eta and phi coresponding to a point in local coordinates. 
     * Requires as input the vertex spread. Returns etaMin, etaMax, and phi.
     * This function is used by getEtaPhiRegion()
     */
    void getEtaPhiPoint(const HepGeom::Point3D<double>& point, double deltaZ,
                        double& etaMin, double& etaMax, double& phi) const;

    /**
     * Private recoToHitTransform Implementation method with no lock
     */
    const HepGeom::Transform3D recoToHitTransformImpl() const;
 
    /**
     * Declaring the Message method for further use (inline)
     */
    MsgStream& msg(MSG::Level lvl) const;
    
    /**
     * Declaring the Method providing Verbosity Level (inline)
     */
    bool msgLvl(MSG::Level lvl) const;
    //@}
    
    /**
     * Protected data:
     */
  protected:

    /**
     * @name Variables for cache validities
     */
    //@{
    /**
     * For alignment associated quatities.
     */
    //not obvious why we need a separate bool for the base class version...
    mutable std::atomic_bool m_baseCacheValid{false};
    mutable std::atomic_bool m_cacheValid{false};
    /**
     * For alignment independent quantities
     */
    //ditto
    mutable std::atomic_bool m_firstTimeBase{true};
    mutable std::atomic_bool m_firstTime{true};
    
    /**
     * @name Mutex guard to update mutable variables in const methods
     */
    //@{
    mutable std::mutex m_mutex{};
    //@}

    /**
     * @name Variables set by constructor
     */
    //@{

    /**
     * identifier of this detector element
     */
    Identifier m_id{};

    /**
     * local description of this detector element
     */
    const DetectorDesign* m_design{nullptr};

    const SiCommonItems* m_commonItems{nullptr};
    std::unique_ptr<Trk::Surface> m_surface;
    const GeoAlignmentStore* m_geoAlignStore{};

    /**
     * Axes
     */
    DetectorDesign::Axis m_hitEta;
    DetectorDesign::Axis m_hitPhi;
    DetectorDesign::Axis m_hitDepth;
    //@}

    /**
     * @name Variables set by commonConstructor
     */
    //@{
    /**
     * hash id of this detector element
     */
    IdentifierHash m_idHash{};
    //@}

    /**
     * @name Variables set by updateCache with m_firstTime of true
     * Happens only once
     *
     * Directions of axes. These are true if the hit/simulation and reconstruction local
     * frames are in the same direction and false if they are opposite.
     */
    //@{
    /**
     * Direction of depth axis.
     * Also direction of readout implant (n+ for pixel, p+ for SCT).
     */
    mutable bool m_depthDirection ATLAS_THREAD_SAFE {true};
    mutable bool m_phiDirection ATLAS_THREAD_SAFE {true};
    mutable bool m_etaDirection ATLAS_THREAD_SAFE {true};
    //@}

    /**
     * @name Variables set by updateCache
     */
    //@{
    mutable Amg::Transform3D m_transformHit ATLAS_THREAD_SAFE;
    mutable Amg::Transform3D m_transform ATLAS_THREAD_SAFE;
    mutable HepGeom::Transform3D m_transformCLHEP ATLAS_THREAD_SAFE;

    mutable Amg::Vector3D m_normal ATLAS_THREAD_SAFE;
    mutable Amg::Vector3D m_etaAxis ATLAS_THREAD_SAFE;
    mutable HepGeom::Vector3D<double> m_etaAxisCLHEP ATLAS_THREAD_SAFE;
    mutable Amg::Vector3D m_phiAxis ATLAS_THREAD_SAFE;
    mutable HepGeom::Vector3D<double> m_phiAxisCLHEP ATLAS_THREAD_SAFE;
    mutable Amg::Vector3D m_center ATLAS_THREAD_SAFE;
    mutable HepGeom::Vector3D<double> m_centerCLHEP ATLAS_THREAD_SAFE;
    mutable Amg::Vector3D m_origin ATLAS_THREAD_SAFE;

      /**
     * @name Variable set by updateCache
     * Determines if the orientations is "barrel like"
     * (which for ITk pixel can be the case even if 
     * not strictly part of the barrel geometry)
     */
    mutable bool m_barrelLike ATLAS_THREAD_SAFE {false};

    mutable double m_minZ ATLAS_THREAD_SAFE {std::numeric_limits<double>::max()};
    mutable double m_maxZ ATLAS_THREAD_SAFE {std::numeric_limits<double>::lowest()};
    mutable double m_minR ATLAS_THREAD_SAFE {std::numeric_limits<double>::max()};
    mutable double m_maxR ATLAS_THREAD_SAFE {std::numeric_limits<double>::lowest()};
    mutable double m_minPhi ATLAS_THREAD_SAFE {std::numeric_limits<double>::max()};
    mutable double m_maxPhi ATLAS_THREAD_SAFE {std::numeric_limits<double>::lowest()};
    //@}

    /**
     * @name Variable set by determineStereo with m_stereoCacheValid of false
     * Happens only once
     */
    //@{
    mutable bool m_isStereo ATLAS_THREAD_SAFE {false};
    //@}

    /**
     * @name Variable set by surfaces ith m_surfacesValid of false
     * Happens only once
     */
    //@{
    mutable std::vector<const Trk::Surface*> m_surfaces ATLAS_THREAD_SAFE {};
    //@}

  };
    
} // namespace InDetDD

/**
 * Include implementation of inline methods
 */
#include "ReadoutGeometryBase/SolidStateDetectorElementBase.icc"

#endif // INDETREADOUTGEOMETRY_SIDETECTORELEMENTBASE_H
