/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TileTBFactory.h
 *
 * @brief Definition of TileTBFactory class
 *
 * @author Gia Khoriauli <Gia.Khoriauli@cern.ch>
 *
 * $Id: TileTBFactory.h,v 1.6 2007-11-05 15:45:18 baranov Exp $
 */

#ifndef TileTBFactory_h
#define TileTBFactory_h 1

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileGeoModel/TileSwitches.h"

class MsgStream;
class StoreGateSvc;


class TileTBFactory : public GeoVDetectorFactory
{
public:
  
  /** Constructor */
  TileTBFactory(StoreGateSvc *pDetStore, TileDetDescrManager *manager,
                const TileSwitches & switches, MsgStream *log);
  
  /** Destructor */
  ~TileTBFactory();
  
  /** Creation of Test Beam Tile geometry */
  virtual void create(GeoPhysVol *world);
  
  /** Access function to TileDetDescr geometry data */
  virtual const TileDetDescrManager * getDetectorManager() const { return m_detectorManager; }

private:  
  
  /** Detector pointer to Store Gate service */
  StoreGateSvc              *m_detectorStore;

  /** Detector pointer to TileDetDescrManager */  
  TileDetDescrManager       *m_detectorManager;  

  /** Get message SVC */
  MsgStream                 *m_log;

  /** all switches */
  TileSwitches m_switches;

  /** Flag for activation verbose level for debugging */
  bool                       m_verbose;
};

#endif

