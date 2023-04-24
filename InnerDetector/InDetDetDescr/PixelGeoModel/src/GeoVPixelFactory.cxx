/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoVPixelFactory.h"

using InDetDD::PixelDetectorManager;

GeoVPixelFactory::GeoVPixelFactory(InDetDD::PixelDetectorManager* ddmgr, 
                                   PixelGeometryManager* mgr,
				   GeoModelIO::ReadGeoModel* sqliteReader,
                                   std::shared_ptr<std::map<std::string, GeoFullPhysVol*>> mapFPV,
                                   std::shared_ptr<std::map<std::string, GeoAlignableTransform*>> mapAX)
  : m_gmt_mgr (mgr)
  , m_mat_mgr (m_gmt_mgr->getMaterialManager())
  , m_DDmgr (ddmgr)
  , m_sqliteReader(sqliteReader)
  , m_mapFPV(mapFPV)
  , m_mapAX(mapAX)
  , m_epsilon(0.0001)
{
}

GeoVPixelFactory::~GeoVPixelFactory()
{
}
