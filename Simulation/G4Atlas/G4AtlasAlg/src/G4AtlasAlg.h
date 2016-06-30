/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_G4AtlasAlg_H
#define G4ATLASALG_G4AtlasAlg_H

// Base class header
#include "AthenaBaseComps/AthAlgorithm.h"

// Athena headers
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasInterfaces/IPhysicsListTool.h"

// Gaudi headers
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// STL headers
#include <map>
#include <string>

/// @class G4AtlasAlg
/// @brief Primary Athena algorithm for ATLAS simulation.
///
/// During initialization, this class sets up several things, including:
/// - the ATLAS (master) run manager
/// - physics list assignment to G4
/// - detector construction (currently FADS::FadsDetectorConstruction)
///
/// During the event loop, it handles processing of the event by
/// invoking the (worker) run manager.
///
class G4AtlasAlg : public AthAlgorithm
{

public:

  /// Standard algorithm constructor
  G4AtlasAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Virtual destructor
  virtual ~G4AtlasAlg() {  };

  /// @brief Initialize the algorithm.
  ///
  /// Here we setup several things for simulation, including:
  /// - force intialization of the UserActionSvc
  /// - apply custom G4 UI commands (like custom physics list)
  /// - configure the particle generator and random generator svc
  StatusCode initialize() override;

  /// Finalize the algorithm and invoke G4 run termination.
  StatusCode finalize() override;

  /// @brief Simulate one Athena event.
  StatusCode execute() override;

  /// Poorly named possibly unused method which sets some verbosities.
  void initializeG4();

  /// G4 initialization called only by the first alg instance.
  /// This is done (for now) because we get multiple alg instances in hive.
  void initializeOnce();

  /// G4 finalization called only by the first alg instance.
  /// This is done (for now) because we get multiple alg instances in hive.
  void finalizeOnce();

private:

  /// Properties for the jobOptions
  std::string libList;
  std::string physList;
  std::string generator;
  std::string fieldMap;
  std::string rndmGen;
  bool m_releaseGeoModel;
  bool m_recordFlux;
  bool m_IncludeParentsInG4Event;
  bool m_killAbortedEvents;
  bool m_flagAbortedEvents;

  /// Verbosity settings for Geant4
  std::map<std::string,std::string> m_verbosities;

  /// Commands to send to the G4 UI
  std::vector<std::string> m_g4commands;

  /// Activate multi-threading configuration
  bool m_useMT;

  /// Random number service
  ServiceHandle<IAtRndmGenSvc> m_rndmGenSvc;
  /// First user action service implementation
  ServiceHandle<IUserActionSvc> m_UASvc;
  /// New user action service implementation
  ServiceHandle<G4UA::IUserActionSvc> m_userActionSvc;
  /// Physics List Tool
  ToolHandle<IPhysicsListTool> m_physListTool;

};

#endif// G4ATLASALG_G4AtlasAlg_H
