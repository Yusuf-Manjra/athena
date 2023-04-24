/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELGEOMODEL_GEOPIXELSICRYSTAL_H
#define PIXELGEOMODEL_GEOPIXELSICRYSTAL_H

#include "Identifier/Identifier.h"
#include "GeoVPixelFactory.h"

#include <memory>

class GeoLogVol;

namespace InDetDD {
  class SiDetectorDesign;
  class PixelDiodeMatrix;
}

class GeoPixelSiCrystal : public GeoVPixelFactory {
 public:
  GeoPixelSiCrystal(InDetDD::PixelDetectorManager* ddmgr,
                    PixelGeometryManager* mgr,
		    GeoModelIO::ReadGeoModel* sqliteReader,
                    std::shared_ptr<std::map<std::string, GeoFullPhysVol*>> mapFPV,
                    std::shared_ptr<std::map<std::string, GeoAlignableTransform*>> mapAX,
                    bool isBLayer, bool isModule3D=false);
  virtual GeoVPhysVol* Build() override;
  inline Identifier getID() {return m_id;}

  bool GetModule3DFlag() { return m_isModule3D; };

 private:
  std::shared_ptr<const InDetDD::PixelDiodeMatrix> makeMatrix(double phiPitch, double etaPitch, double etaPitchLong, double etaPitchLongEnd,
					 int circuitsPhi, int circuitsEta, int diodeRowPerCirc, int diodeColPerCirc);
  Identifier m_id;
  const InDetDD::SiDetectorDesign* m_design{nullptr};
  bool m_isBLayer = false;
  bool m_isModule3D = false;
};

#endif
