/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeomACTS/PrintSiElements.h"

#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "TrkSurfaces/TrapezoidBounds.h"

#include <Eigen/Dense>

#include "ACTS/Detector/TrackingGeometry.hpp"
//#include "ACTS/Plugins/GeoModelPlugins/GeoModelDetectorElement.hpp"
#include "ACTS/Plugins/GeoModelPlugins/GeoModelLayerBuilder.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Tools/LayerCreator.hpp"
#include "ACTS/Tools/SurfaceArrayCreator.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Tools/LayerCreator.hpp"
#include "ACTS/Tools/PassiveLayerBuilder.hpp"
#include "ACTS/Tools/TrackingGeometryBuilder.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"
#include "ACTS/Tools/ITrackingVolumeBuilder.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
namespace {
const Amg::Vector3D origin(0., 0., 0.);
}

#define PRV(v)                           \
  #v << "(x="                            \
     << v.x() << ", y="                  \
              << v.y() << ", z="         \
                       << v.z() << " r=" \
                                << v.perp() << ", phi=" << v.phi() << ")"

#define DUMP(v) #v << "=" << v

/////////////////////////////////////////////////////////////////////////////
//
// Prints out SiDetectorElement transform

/////////////////////////////////////////////////////////////////////////////

PrintSiElements::PrintSiElements(const std::string& name,
                                 ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
      m_firstEvent(true),
      m_geoModelSvc("GeoModelSvc", name) {
  // Get parameter values from jobOptions file
  declareProperty("ModulesOnly", m_modulesOnly = true,
                  "Print transforms of modules");
  declareProperty("ExpandId", m_expandId = true, "Print fields of identifier");
  declareProperty("NominalPosition", m_nominal = true,
                  "Print out nominal geometry");
  declareProperty("AlignedPosition", m_aligned = false,
                  "Print out aligned geometry");
  declareProperty("FullRotationMatrix", m_fullRotationMatrix = false,
                  "If true prints the 9 elements of the rotation matrix");
  declareProperty("OutputFile", m_outputFileName = "geometry.dat",
                  "Output file name");
  declareProperty("GeoModelSvc", m_geoModelSvc);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PrintSiElements::initialize() {
  // Get GeoModelSvc
  const IGeoModelSvc* geoModel;
  ATH_CHECK(service("GeoModelSvc", geoModel));
  m_fileout.open(m_outputFileName.c_str());
  ATH_MSG_DEBUG("THIS IS THE GOOD SHIT RIGHT HERE");
  ATH_MSG_DEBUG("Opening output file " << m_outputFileName);
  if (!m_fileout) {
    ATH_MSG_ERROR("Could not open file " << m_outputFileName);
    return StatusCode::FAILURE;
  }
  // Print version infomration
  m_fileout << "# ATLAS tag: " << geoModel->atlasVersion() << std::endl;
  m_fileout << "# InDet tag: " << geoModel->inDetVersionOverride() << std::endl;
  m_fileout << "# Pixel tag: " << geoModel->pixelVersionOverride() << std::endl;
  m_fileout << "# SCT   tag: " << geoModel->SCT_VersionOverride() << std::endl;
  m_fileout << "# TRT   tag: " << geoModel->TRT_VersionOverride() << std::endl;
  return StatusCode::SUCCESS;
}

StatusCode PrintSiElements::printElements(const std::string& managerName) {
  const InDetDD::SiDetectorManager* manager;
  ATH_CHECK(detStore()->retrieve(manager, managerName));

  Eigen::Vector3d ctrAvg(0, 0, 0);
  size_t nElem = 0;

  using GMLB = Acts::GeoModelLayerBuilder;
  GMLB::Config cfg;

  auto elementStore = std::make_shared<GMLB::ElementVector>();

  cfg.mng = manager;
  cfg.elementStore = elementStore;

  auto matcher = [](Acts::BinningValue bValue, const Acts::Surface* aS,
                    const Acts::Surface* bS) -> bool {

    auto a = dynamic_cast<const Acts::GeoModelDetectorElement*>(
        aS->associatedDetectorElement());
    auto b = dynamic_cast<const Acts::GeoModelDetectorElement*>(
        bS->associatedDetectorElement());

    // check if same bec
    // can't be same if not
    if(a->bec() != b->bec()) return false;

    if (bValue == Acts::binPhi) {
      return a->phi_module() == b->phi_module();
    }

    if (bValue == Acts::binZ) {
      return (a->eta_module() == b->eta_module())
             && (a->layer_disk() == b->layer_disk())
             && (a->bec() == b->bec());
    }

    if (bValue == Acts::binR) {
      //std::cout << "eta " << a->eta_module() << " <-> " << b->eta_module() << std::endl;
      //std::cout << "bec " << a->bec() << " <-> " << b->bec() << std::endl;
      //std::cout << "ld " << a->layer_disk() << " <-> " << b->layer_disk() << std::endl;
      return (a->eta_module() == b->eta_module()) 
             && (a->layer_disk() == b->layer_disk()) 
             && (a->bec() == b->bec());
    }

    return false;
  };

  Acts::SurfaceArrayCreator::Config sacCfg;
  sacCfg.surfaceMatcher = matcher;

  auto surfaceArrayCreator = std::make_shared<Acts::SurfaceArrayCreator>(
      sacCfg,
      Acts::getDefaultLogger("SurfaceArrayCreator", Acts::Logging::VERBOSE));
  Acts::LayerCreator::Config lcCfg;
  lcCfg.surfaceArrayCreator = surfaceArrayCreator;
  auto layerCreator = std::make_shared<Acts::LayerCreator>(
      lcCfg, Acts::getDefaultLogger("LayerCreator", Acts::Logging::VERBOSE));

  cfg.layerCreator = layerCreator;


  Acts::Logging::Level loggingLevel = Acts::Logging::VERBOSE;

  // auto gmLayerBuilder = GMLB(cfg);
  //GMLB gmLayerBuilder(cfg);
  auto gmLayerBuilder = std::make_shared<const GMLB>(cfg);



  auto layerArrayCreator = std::make_shared<const Acts::LayerArrayCreator>(
      Acts::getDefaultLogger("LayArrayCreator", loggingLevel));

  auto trackingVolumeArrayCreator
      = std::make_shared<const Acts::TrackingVolumeArrayCreator>(
          Acts::getDefaultLogger("TrkVolArrayCreator", loggingLevel));

  Acts::CylinderVolumeHelper::Config cvhConfig;
  cvhConfig.layerArrayCreator          = layerArrayCreator;
  cvhConfig.trackingVolumeArrayCreator = trackingVolumeArrayCreator;

  auto cylinderVolumeHelper
    = std::make_shared<const Acts::CylinderVolumeHelper>(
        cvhConfig, Acts::getDefaultLogger("CylVolHelper", loggingLevel));


  Acts::CylinderVolumeBuilder::Config cvbConfig;
  cvbConfig.layerEnvelopeR = {2, 2};
  cvbConfig.layerEnvelopeZ       = 2;
  cvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
  cvbConfig.volumeSignature      = 0;
  cvbConfig.volumeName           = "Pixel";
  //cvbConfig.volumeMaterial       = volumeMaterial;
  cvbConfig.layerBuilder         = gmLayerBuilder;

  auto cylinderVolumeBuilder
    = std::make_shared<const Acts::CylinderVolumeBuilder>(
        cvbConfig,
        Acts::getDefaultLogger("CylinderVolumeBuilder", loggingLevel));

  std::list<std::shared_ptr<const Acts::ITrackingVolumeBuilder>> volumeBuilders;
  volumeBuilders.push_back(cylinderVolumeBuilder);


  Acts::TrackingGeometryBuilder::Config tgbConfig;
  tgbConfig.trackingVolumeHelper   = cylinderVolumeHelper;
  tgbConfig.trackingVolumeBuilders = volumeBuilders;
  auto trackingGeometryBuilder
      = std::make_shared<const Acts::TrackingGeometryBuilder>(tgbConfig);
  
  
  std::unique_ptr<const Acts::TrackingGeometry> trackingGeometry = trackingGeometryBuilder->trackingGeometry();



  //gmLayerBuilder.centralLayers();
  //gmLayerBuilder.negativeLayers();
  //gmLayerBuilder.positiveLayers();
  





  return StatusCode::SUCCESS;

  InDetDD::SiDetectorElementCollection::const_iterator iter;
  for (iter = manager->getDetectorElementBegin();
       iter != manager->getDetectorElementEnd(); ++iter) {
    const InDetDD::SiDetectorElement* element = *iter;
    if (element) {
      Amg::Transform3D trans;
      Amg::Transform3D defTrans;

      trans = element->transform();
      defTrans = element->defTransform();

      auto center = element->center();

      Identifier id = element->identify();
      int det = 0;
      int bec = 0;
      int layer_disk = 0;
      int phi_module = 0;
      int eta_module = 0;
      int side = 0;
      int layer_disk_max = 0;

      //// Get identifier fields.
      if (element->isPixel()) {
        const PixelID* pixIdHelper =
            dynamic_cast<const PixelID*>(element->getIdHelper());
        if (pixIdHelper) {
          det = 1;
          bec = pixIdHelper->barrel_ec(id);
          layer_disk = pixIdHelper->layer_disk(id);
          phi_module = pixIdHelper->phi_module(id);
          eta_module = pixIdHelper->eta_module(id);
          layer_disk_max = pixIdHelper->layer_disk_max(id);
          side = 0;
        }
      } else {  // SCT
        const SCT_ID* sctIdHelper =
            dynamic_cast<const SCT_ID*>(element->getIdHelper());
        if (sctIdHelper) {
          det = 2;
          bec = sctIdHelper->barrel_ec(id);
          layer_disk = sctIdHelper->layer_disk(id);
          phi_module = sctIdHelper->phi_module(id);
          eta_module = sctIdHelper->eta_module(id);
          layer_disk_max = sctIdHelper->layer_disk_max(id);
          side = sctIdHelper->side(id);
        }
      }

      InDetDD::DetectorShape shape = element->design().shape();

      // Eigen::Vector3d ctr;
      // ctr << center.x(), center.y(), center.z();
      // std::cout << ctr << std::endl;

      if (bec == 0) {
        // ctrAvg += ctr;
        // nlem += 1;
        std::cout << DUMP(det) << " " << DUMP(bec) << " " << DUMP(layer_disk)
                  << " " << DUMP(phi_module) << " ";
        std::cout << DUMP(eta_module) << " " << DUMP(side) << " " << PRV(center)
                  << std::endl;
        std::cout << DUMP(layer_disk_max) << std::endl;
      }

      // if(shape == InDetDD::Trapezoid) {
      ////const Trk::SurfaceBounds *bounds = &element->bounds();
      // auto trap = dynamic_cast<const
      // Trk::TrapezoidBounds*>(&element->bounds());
      // double minHLX = trap->minHalflengthX();
      // double maxHLX = trap->maxHalflengthX();
      // double hlY = trap->halflengthY();
      // std::cout << minHLX << ", " << maxHLX << ", " << hlY << std::endl;

      //}
    }
  }

  // ctrAvg /= nElem;

  // std::cout << DUMP(nElem) << std::endl;
  // std::cout << DUMP(ctrAvg) << std::endl;

  return StatusCode::SUCCESS;
}

StatusCode PrintSiElements::execute() {
  if (m_firstEvent) {
    m_firstEvent = false;
    ATH_CHECK(printElements("Pixel"));
    // ATH_CHECK( printElements("SCT") );
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode PrintSiElements::finalize() {
  m_fileout.close();
  return StatusCode::SUCCESS;
}

std::string PrintSiElements::printTransform(
    const Amg::Transform3D& trans) const {
  Amg::Vector3D xyz = trans * origin;
  std::ostringstream ostr;
  ostr << xyz.x() << " " << xyz.y() << " " << xyz.z() << " ";
  if (m_fullRotationMatrix) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        ostr << trans(i, j) << " ";
      }
    }
  } else {
    double alpha = 0, beta = 0, gamma = 0;
    extractAlphaBetaGamma(trans, alpha, beta, gamma);
    ostr << alpha << " " << beta << " " << gamma;
  }
  // CLHEP::HepRotation rot = trans.getRotation();
  // ostr << std::endl << rot;
  return ostr.str();
}

void PrintSiElements::extractAlphaBetaGamma(const Amg::Transform3D& trans,
                                            double& alpha, double& beta,
                                            double& gamma) const {
  double siny = trans(0, 2);
  beta = asin(siny);
  // Check if cosy = 0. This requires special treatment.
  // can check either element (1,2),(2,2) both equal zero
  // or (0,1) and (0,0)
  if ((trans(1, 2) == 0) && (trans(2, 2) == 0)) {
    // alpha and gamma are degenerate. We arbitrarily choose
    // gamma = 0.
    gamma = 0;
    alpha = atan2(trans(1, 1), trans(2, 1));
  } else {
    alpha = atan2(-trans(1, 2), trans(2, 2));
    gamma = atan2(-trans(0, 1), trans(0, 0));
    if (alpha == 0) alpha = 0;  // convert -0 to 0
    if (gamma == 0) gamma = 0;  // convert -0 to 0
  }
}
