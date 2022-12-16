/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKINGGEOMETRY_MUONSTATIONBUILDERIMPL_H
#define MUONTRACKINGGEOMETRY_MUONSTATIONBUILDERIMPL_H

// Amg
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/GeoPrimitives.h"
//
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonTrackingGeometry/MuonStationTypeBuilder.h"
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"
#include "TrkDetDescrGeoModelCnv/GeoShapeConverter.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeHelper.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/TrackingVolume.h"

namespace Trk {
class MaterialProperties;
}

namespace Muon {

/** @class MuonStationBuilderImpl

    The Muon::MuonStationBuilderImpl retrieves muon stations from Muon Geometry
    Tree prototypes built with help of Muon::MuonStationTypeBuilder
    by Sarka.Todorova@cern.ch
  */

class MuonStationBuilderImpl : public AthAlgTool {
 public:
  virtual ~MuonStationBuilderImpl() = default;
  virtual StatusCode initialize() override;

  std::unique_ptr<std::vector<std::unique_ptr<Trk::DetachedTrackingVolume>>>
  buildDetachedTrackingVolumesImpl(const MuonGM::MuonDetectorManager* muonMgr,
                                   bool blend = false) const;

 protected:
  MuonStationBuilderImpl(const std::string&, const std::string&,
                         const IInterface*);

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
      this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  std::vector<std::unique_ptr<Trk::DetachedTrackingVolume>>
  buildDetachedTrackingVolumeTypes(
      bool blend, const MuonGM::MuonDetectorManager* muonMgr) const;

  void glueComponents(Trk::DetachedTrackingVolume*) const;
  void encloseLayers(const Trk::DetachedTrackingVolume*) const;
  void identifyLayers(Trk::DetachedTrackingVolume*, int, int,
                      const MuonGM::MuonDetectorManager*) const;

  void identifyPrototype(Trk::TrackingVolume*, int, int,
                         const Amg::Transform3D&,
                         const MuonGM::MuonDetectorManager*) const;

  void getNSWStationsForTranslation(
      const GeoVPhysVol* pv, const std::string& name, const Amg::Transform3D&,
      std::vector<
          std::pair<std::pair<const GeoLogVol*, Trk::MaterialProperties*>,
                    std::vector<Amg::Transform3D>>>& vols,
      std::vector<std::string>& volNames) const;

  ToolHandle<Muon::MuonStationTypeBuilder> m_muonStationTypeBuilder{
      this, "StationTypeBuilder",
      "Muon::MuonStationTypeBuilder/"
      "MuonStationTypeBuilder"};  //!< Helper Tool
                                  //!< to create
                                  //!< TrackingVolume
                                  //!< Arrays
  ToolHandle<Trk::ITrackingVolumeHelper> m_trackingVolumeHelper{
      this, "TrackingVolumeHelper",
      "Trk::TrackingVolumeHelper/TrackingVolumeHelper"};  //!< Helper Tool to
                                                          //!< create
                                                          //!< TrackingVolumes

  Trk::Material m_muonMaterial;  //!< the material
  //!< shape converter
  std::unique_ptr<Trk::GeoShapeConverter> m_geoShapeConverter;
  //!< material converter
  std::unique_ptr<Trk::GeoMaterialConverter> m_materialConverter;
  Gaudi::Property<bool> m_buildBarrel{this, "BuildBarrelStations", true};
  Gaudi::Property<bool> m_buildEndcap{this, "BuildEndcapStations", true};
  Gaudi::Property<bool> m_buildCsc{this, "BuildCSCStations", true};
  Gaudi::Property<bool> m_buildTgc{this, "BuildTGCStations", true};
};

}  // namespace Muon

#endif  // MUONTRACKINGGEOMETRY_MUONSTATIONBUILDERIMPL_H
