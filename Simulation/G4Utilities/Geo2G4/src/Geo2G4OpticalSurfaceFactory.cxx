/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Geo2G4/Geo2G4OpticalSurfaceFactory.h"
#include "Geo2G4/Geo2G4MatPropTableFactory.h"

#include "G4OpticalSurface.hh"
#include "GeoModelUtilities/GeoOpticalSurface.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

Geo2G4OpticalSurfaceFactory::Geo2G4OpticalSurfaceFactory()
{
}

G4OpticalSurface* Geo2G4OpticalSurfaceFactory::Build(const GeoOpticalSurface* geoOpticalSurface)
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IMessageSvc* msgSvc;
  StatusCode sc = svcLocator->service("MessageSvc", msgSvc);
  if(sc.isFailure())
  {
    std::cerr << "Geo2G4OpticalSurfaceFactory unable to get Message Service\n";
    return 0;
  }

  MsgStream log(msgSvc,"Geo2G4OpticalSurfaceFactory");

  //
  // Check if this surface has already been defined.
  //
  if(definedOptSurfaces.find(geoOpticalSurface) != definedOptSurfaces.end())  {
    return definedOptSurfaces[geoOpticalSurface];
  }

  // map enums
  G4OpticalSurfaceModel g4Model;
  G4OpticalSurfaceFinish g4Finish;
  G4SurfaceType g4Type;

  switch(geoOpticalSurface->GetModel())
  {
  case GeoOpticalSurface::glisur:
    {
      g4Model = glisur;
      break;
    }
  case GeoOpticalSurface::unified:
    {
      g4Model = unified;
      break;
    }
  default:
    g4Model = glisur;
  }

  switch(geoOpticalSurface->GetFinish())
  {
  case GeoOpticalSurface::polished:
    {
      g4Finish = polished;
      break;
    }
  case GeoOpticalSurface::polishedfrontpainted:
    {
      g4Finish = polishedfrontpainted;
      break;
    }
  case GeoOpticalSurface::polishedbackpainted:
    {
      g4Finish = polishedbackpainted;
      break;
    }
  case GeoOpticalSurface::ground:
    {
      g4Finish = ground;
      break;
    }
  case GeoOpticalSurface::groundfrontpainted:
    {
      g4Finish = groundfrontpainted;
      break;
    }
  case GeoOpticalSurface::groundbackpainted:
    {
      g4Finish = groundbackpainted;
      break;
    }
  default:
    g4Finish = polished;
  }

  switch(geoOpticalSurface->GetType())
  {
  case GeoOpticalSurface::dielectric_metal:
    {
      g4Type = dielectric_metal;
      break;
    }
  case GeoOpticalSurface::dielectric_dielectric:
    {
      g4Type = dielectric_dielectric;
      break;
    }
  case GeoOpticalSurface::firsov:
    {
      g4Type = firsov;
      break;
    }
  case GeoOpticalSurface::x_ray:
    {
      g4Type = x_ray;
      break;
    }
  default:
    g4Type = dielectric_dielectric;
  }



  G4OpticalSurface* newG4Surface = new G4OpticalSurface(geoOpticalSurface->GetName(),
							g4Model,
							g4Finish,
							g4Type,
							geoOpticalSurface->GetParameter());

  // Create material properties table 	 
  Geo2G4MatPropTableFactory* tFactory = Geo2G4MatPropTableFactory::instance(); 	 
  GeoMaterialPropertiesTable* geoPropTable = geoOpticalSurface->GetMaterialPropertiesTable(); 	 
	  	 
  if(geoPropTable){ 	 
    G4MaterialPropertiesTable* g4PropTable = tFactory->Build(geoPropTable); 	 
    if(g4PropTable) 	 
      newG4Surface->SetMaterialPropertiesTable(g4PropTable); 	 
  }

  return newG4Surface;
}
