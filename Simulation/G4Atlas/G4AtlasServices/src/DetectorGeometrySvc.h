/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_DETECTORGEOMETRYSVC_H
#define G4ATLASSERVICES_DETECTORGEOMETRYSVC_H

// Base classes
#include "AthenaBaseComps/AthService.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

// Athena headers
#include "G4AtlasInterfaces/IDetectorConstructionTool.h"
#include "G4AtlasInterfaces/IRegionCreator.h"
#include "G4AtlasInterfaces/IParallelWorldTool.h"
#include "G4AtlasInterfaces/IDetectorGeometryTool.h"

// Gaudi headers
#include "GaudiKernel/ToolHandle.h" // For tool handle array

class G4VUserDetectorConstruction;

class DetectorGeometrySvc : public AthService , public virtual IDetectorGeometrySvc {
public:
  // Standard constructor and destructor
  DetectorGeometrySvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~DetectorGeometrySvc();

  // Gaudi methods
  StatusCode initialize() override final;
  StatusCode finalize() override final;
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID& interfaceID() { return IDetectorGeometrySvc::interfaceID(); }

  G4VUserDetectorConstruction* GetDetectorConstruction() final;

  bool ParallelWorldsActivated() {return m_activateParallelWorlds;}

  void ActivateParallelWorlds() {m_activateParallelWorlds=true;}

  std::vector<std::string>& GetParallelWorldNames();

protected:
  void BuildExtraMaterials();

private:
  ToolHandle<IDetectorGeometryTool> m_detTool;
  ToolHandle<IDetectorConstructionTool> m_detConstruction;
  ToolHandleArray<IRegionCreator> m_regionCreators;
  ToolHandleArray<IParallelWorldTool> m_parallelWorlds;

  bool m_activateParallelWorlds;
  std::vector<std::string> m_parallelWorldNames;
};

#endif
