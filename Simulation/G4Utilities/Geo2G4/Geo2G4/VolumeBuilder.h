/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO2G4_VolumeBuilder_H
#define GEO2G4_VolumeBuilder_H

#include "Geo2G4/LogicalVolume.h"
#include "Geo2G4/Geo2G4SvcAccessor.h"

#include "GeoModelUtilities/GeoOpticalPhysVol.h"

#include "G4VPhysicalVolume.hh"

#include <string>
#include <iostream>
#include <map>

typedef std::map< const GeoOpticalPhysVol*, G4VPhysicalVolume*,std::less< const GeoOpticalPhysVol*> > OpticalVolumesMap;

class VolumeBuilder
{
 public:
  VolumeBuilder(std::string k): paramOn(false), key(k)
  {
    Geo2G4SvcAccessor accessor;
    Geo2G4SvcBase *g=accessor.GetGeo2G4Svc();
    g->RegisterVolumeBuilder(this);
  }

  virtual ~VolumeBuilder()
  {
    Geo2G4SvcAccessor accessor;
    Geo2G4SvcBase *g=accessor.GetGeo2G4Svc();
    g->UnregisterVolumeBuilder(this);
  }

  std::string GetKey() const {return key;}

  // flag controlling Parameterization to Parameterization translation
  void SetParam(bool flag){paramOn = flag;}
  bool GetParam(){return paramOn;}

  virtual LogicalVolume* Build(PVConstLink pv, OpticalVolumesMap* optical_volumes = 0) const = 0;

 protected:
  bool paramOn;

 private:
  std::string key;
};
#endif
