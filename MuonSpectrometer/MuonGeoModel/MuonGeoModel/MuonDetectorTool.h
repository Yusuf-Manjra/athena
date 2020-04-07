/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONGEOMODEL_MUONDETECTORTOOL_H
#define MUONGEOMODEL_MUONDETECTORTOOL_H

#include "GeoModelUtilities/GeoModelTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "MuonGeoModel/MuonDetectorFactory001.h"

namespace MuonGM
{
    class MuonDetectorManager;
}


class MuonDetectorTool : public GeoModelTool, virtual public IIncidentListener {

public:

  // Standard Constructor
  MuonDetectorTool( const std::string& type, const std::string& name, const IInterface* parent );

  // Standard Destructor
  virtual ~MuonDetectorTool() override final;

  //initialize - needed to retrieve the alignment Tool 
  virtual StatusCode initialize() override;

  // build the geometry 
  virtual StatusCode create() override final;
  StatusCode createFactory(MuonGM::MuonDetectorFactory001& theFactory);

  // Dereference tree tops and drop readout objects
  virtual StatusCode clear() override final;

  // incident svc handle
  virtual void handle(const Incident&) override;


private:
  std::string m_layout;
  int m_accessCondDb;
  int m_asciiCondData;
  int m_nova;  
  int m_includeCutouts;
  int m_includeCutoutsBog;
  int m_includeCtbBis;
  int m_fillCache_initTime;
  bool m_dumpMemoryBreakDown;  
  int m_enableFineClashFixing;
  bool m_hasCSC;
  bool m_hasSTgc;
  bool m_hasMM;
  int m_stationSelection;
  std::vector<std::string> m_selectedStations;
  std::vector<int> m_selectedStEta;
  std::vector<int> m_selectedStPhi;
  int m_controlAlines;
  bool m_dumpAlines;
  std::string m_altAsztFile;
  int m_minimalGeoFlag;
  bool m_useCscIntAlines;
  int m_controlCscIntAlines;
  bool m_dumpCscIntAlines;
  bool m_useCscIntAlinesFromGM;
  std::string m_altCscIntAlinesFile;
  Gaudi::Property<int> m_cachingFlag { this, "CachingFlag", 1, "Turn on/off caching of ReadoutElement surfaces etc. (i.e. for MDTs)" };
  int m_enableMdtDeformations;
  int m_enableMdtAsBuiltParameters;    
  std::string m_altMdtAsBuiltFile;

  int m_switchOnOff_BUILDINERTMATERIALS;
  int m_switchOnOff_MINIMALGEO;
  int m_switchOnOff_BUILDENDCAP;
  int m_switchOnOff_BUILDCALOSADDLE;
  int m_switchOnOff_BUILDBARRELTOROID;
  int m_switchOnOff_BUILDENDCAPTOROID;
  int m_switchOnOff_BUILDFEET;
  int m_switchOnOff_BUILDDISKSHIELD;
  int m_switchOnOff_BUILDTOROIDSHIELD;
  int m_switchOnOff_BUILDFORWARDSHIELD;
  
  
  MuonGM::MuonDetectorManager*  m_manager;
};

#endif // GEOMODELEXAMPLES_TOYDETECTORTOOL_H
