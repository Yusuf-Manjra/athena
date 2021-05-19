/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuTagMatchingTool_H
#define MuTagMatchingTool_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentSelectionTool.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagMatchingTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkGeometry/TrackingGeometry.h"

/**
   @class MuTagMatchingTool

@author Zdenko.van.Kesteren@cern.ch

*/

namespace Trk {
class Surface;
class Track;
}  // namespace Trk

class MuTagMatchingTool : virtual public IMuTagMatchingTool, public AthAlgTool {
  public:
    MuTagMatchingTool(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~MuTagMatchingTool() = default;

    virtual StatusCode initialize();

    std::string segmentStationString(const Muon::MuonSegment* segment) const;

    void testExtrapolation(const Trk::Surface* pSurface, const Trk::Track* pTrack) const;

    bool match(const Trk::TrackParameters* atSurface, const Muon::MuonSegment* segment, std::string surfaceName) const;

    bool surfaceMatch(const Trk::TrackParameters* atSurface, const Muon::MuonSegment* segment,
                      std::string surfaceName) const;

    bool phiMatch(const Trk::TrackParameters* atSurface, const Muon::MuonSegment* segment,
                  std::string surfaceName) const;

    bool thetaMatch(const Trk::TrackParameters* atSurface, const Muon::MuonSegment* segment) const;

    bool rMatch(const Trk::TrackParameters* atSurface, const Muon::MuonSegment* segment) const;

    /** Get extrapolation at MS entrance level*/
    const Trk::TrackParameters* ExtrapolateTrktoMSEntrance(const Trk::Track*  pTrack,
                                                           Trk::PropDirection direction) const;

    /** Get extrapolation at MSSurface level*/
    const Trk::TrackParameters* ExtrapolateTrktoMSSurface(const Trk::Surface*         surface,
                                                          const Trk::TrackParameters* pTrack,
                                                          Trk::PropDirection          direction) const;

    /** Get extrapolation at Segment Plane Surface level*/
    const Trk::AtaPlane* ExtrapolateTrktoSegmentSurface(const Muon::MuonSegment*    segment,
                                                        const Trk::TrackParameters* pTrack,
                                                        Trk::PropDirection          direction) const;
    bool                 hasPhi(const Muon::MuonSegment* seg) const;

    double errorProtection(double exTrk_Err, bool isAngle) const;

    bool matchSegmentPosition(MuonCombined::MuonSegmentInfo* info, bool idHasEtaHits) const;

    bool matchSegmentDirection(MuonCombined::MuonSegmentInfo* info, bool idHasEtaHits) const;


    bool matchPtDependentPull(MuonCombined::MuonSegmentInfo* info, const Trk::Track* trk) const;

    bool matchDistance(MuonCombined::MuonSegmentInfo* info) const;

    bool matchCombinedPull(MuonCombined::MuonSegmentInfo* info) const;

    void nrTriggerHits(const Muon::MuonSegment* seg, int& nRPC, int& nTGC) const;

    const Trk::Perigee* flipDirection(const Trk::Perigee* inputPars) const;

    MuonCombined::MuonSegmentInfo muTagSegmentInfo(const Trk::Track* track, const Muon::MuonSegment* segment,
                                                   const Trk::AtaPlane* exTrack) const;

    void calculateLocalAngleErrors(const Trk::AtaPlane* expPars, double& exTrkErrXZ, double& exTrkErrYZ,
                                   double& covLocYYZ) const;

    void calculateLocalAngleErrors(const Muon::MuonSegment* segment, double& exTrkErrXZ, double& exTrkErrYZ) const;


  private:
    ///////////////////////////////////
    bool         isCscSegment(const Muon::MuonSegment* seg) const;
    unsigned int cscHits(const Muon::MuonSegment* seg) const;

    // exploit correlation between residual in position and angle
    double matchingDistanceCorrection(double resPos, double resAngle);

    ToolHandle<Trk::IExtrapolator> p_IExtrapolator{
        this,
        "IExtrapolator",
        "Trk::Extrapolator/AtlasExtrapolator",
    };  //!< Pointer on IExtrapolator
    ToolHandle<Trk::IPropagator> p_propagator{
        this,
        "Propagator",
        "Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator",
    };  //!< Pointer on propagator for SL propagation
    ToolHandle<Muon::MuonEDMPrinterTool> m_printer{
        this,
        "Printer",
        "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",
    };
    ToolHandle<Muon::IMuonSegmentHitSummaryTool> m_hitSummaryTool{
        this,
        "MuonSegmentHitSummary",
        "Muon::MuonSegmentHitSummaryTool/MuonSegmentHitSummaryTool",
    };
    ToolHandle<Muon::IMuonSegmentSelectionTool> m_selectionTool{
        this,
        "MuonSegmentSelection",
        "Muon::MuonSegmentSelectionTool/MuonSegmentSelectionTool",
    };
    ToolHandle<Trk::IResidualPullCalculator> m_pullCalculator{
        this,
        "ResidualPullCalculator",
        "Trk::ResidualPullCalculator/ResidualPullCalculator",
    };

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };
    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc{
        this,
        "edmHelper",
        "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
        "Handle to the service providing the IMuonEDMHelperSvc interface",
    };


    std::string   m_t0Location;
    StoreGateSvc* p_StoreGateSvc{};  //!< Pointer On StoreGateSvc

    // MuonDetectorManager from the conditions store
    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{
        this,
        "DetectorManagerKey",
        "MuonDetectorManager",
        "Key of input MuonDetectorManager condition data",
    };
    
    ServiceHandle<Trk::ITrackingGeometrySvc> m_trackingGeometrySvc { this, "TrackingGeometrySvc", "TrackingGeometrySvc/AtlasTrackingGeometrySvc" };
    
    SG::ReadCondHandleKey<Trk::TrackingGeometry> m_trackingGeometryReadKey { this, "TrackingGeometryReadKey", "", "Key of input TrackingGeometry" };
        
    bool m_assumeLocalErrors;
    bool m_extrapolatePerigee;

    double m_GLOBAL_THETA_CUT;
    double m_GLOBAL_PHI_CUT;
    double m_GLOBAL_R_CUT;

    double m_MATCH_THETA;
    double m_MATCH_PHI;
    double m_MATCH_THETAANGLE;
    double m_MATCH_PHIANGLE;

    double m_ERROR_EX_POS_CUT;
    double m_ERROR_EX_ANGLE_CUT;

    bool   m_doDistCut;
    double m_DIST_POS;
    double m_DIST_ANGLE;

    double m_SAFE_THETA;
    double m_SAFE_PHI;
    double m_SAFE_THETAANGLE;
    double m_SAFE_PHIANGLE;

    double m_chamberPullCut;
    double m_combinedPullCut;
    
    inline const Trk::TrackingVolume* getVolume(const std::string &vol_name) const {
        /// Good old way of retrieving the volume via the geometry service
        if (m_trackingGeometryReadKey.empty()) {
                return m_trackingGeometrySvc->trackingGeometry()->trackingVolume(vol_name);
        }
        SG::ReadCondHandle < Trk::TrackingGeometry > handle(m_trackingGeometryReadKey, Gaudi::Hive::currentContext());
        if (!handle.isValid()) {
            ATH_MSG_WARNING("Could not retrieve a valid tracking geometry");
                return nullptr;
        }
        return handle.cptr()->trackingVolume(vol_name);
    }


};


#endif  // MuTagMatchingTool_H
