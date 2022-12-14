/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSERVMATGEOMODEL_TRT_SERVMATFACTORYDC2_H
#define INDETSERVMATGEOMODEL_TRT_SERVMATFACTORYDC2_H

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/ServiceHandle.h"

class StoreGateSvc;
class GeoPhysVol;
class GeoShape;
class GeoMaterial;
class StoredMaterialManager;
class IRDBAccessSvc;

#include <string>


class TRT_ServMatFactoryDC2 : public AthMessaging  {

 public:
  
  // Constructor:
  TRT_ServMatFactoryDC2(StoreGateSvc  *pDetStore,
			ServiceHandle<IRDBAccessSvc> pRDBAccess);   
  // Destructor:
  ~TRT_ServMatFactoryDC2();
  
  // Creation of geometry:
  void create(GeoPhysVol *mother);

 private:  
  const GeoShape* createShape(int volType, 
			      double rmin1, 
			      double rmax1, 
			      double halflength,
			      double rmin2,
			      double rmax2);
  
  const GeoMaterial* createMaterial(const std::string & name,
				    int volType, 
				    double fractionRL,
				    double rmin1, 
				    double rmax1, 
				    double halflength,
				    double rmin2,
				    double rmax2);
  // Illegal operations:
  const TRT_ServMatFactoryDC2 & operator=(const TRT_ServMatFactoryDC2 &right);
  TRT_ServMatFactoryDC2(const TRT_ServMatFactoryDC2 &right);

  // private data
  StoreGateSvc                   *m_detStore;
  ServiceHandle<IRDBAccessSvc>    m_rdbAccess;
  StoredMaterialManager          *m_materialManager;
};

#endif //  INDETSERVMATGEOMODEL_TRT_SERVMATFACTORYDC2_H
