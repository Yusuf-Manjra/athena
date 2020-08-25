/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TileDetectorTool_H
#define TileDetectorTool_H

#include "GeoModelUtilities/GeoModelTool.h"

#include <string>

class MsgStream;
class StoreGateSvc;
class TileDetDescrManager;

class TileDetectorTool final : public GeoModelTool 
{
 public:

  TileDetectorTool(const std::string& type, 
		   const std::string& name, 
		   const IInterface* parent);

  virtual ~TileDetectorTool() override final;
  
  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

  bool isTestBeam() const { return m_testBeam; }
  bool isAddPlates() const { return m_addPlates; }
  int uShape() const { return m_uShape; }
  int csTube() const { return m_csTube; }
  int glue() const { return m_glue; }

 private:

  StatusCode initIds();
  StatusCode createElements();


  bool m_testBeam;
  bool m_addPlates;
  int  m_uShape;

  /** 0: glue layer is removed and replaced by iron,
      1: simulation with glue,
      2: glue is replaced by iron + width of iron is modified in order to get the same sampling fraction      */
  int m_glue;
  int m_csTube;

  bool m_not_locked;
  bool m_useNewFactory;
  std::string m_geometryConfig; // FULL, SIMU, RECO
  TileDetDescrManager* m_manager;
};

#endif 
