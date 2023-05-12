/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/


#ifndef G4ATLASINTERFACES_G4UA__IUSERACTIONSVC_H
#define G4ATLASINTERFACES_G4UA__IUSERACTIONSVC_H

// Framework includes
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ToolHandle.h"


#include <vector>
#include "G4UserSteppingAction.hh"
#include "G4AtlasInterfaces/IUserActionTool.h"
namespace G4UA
{

  /// @class IUserActionSvc
  /// @brief Abstract interface for the UserActionSvc
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IUserActionSvc : virtual public IService
  {

  public:

    /// @brief Initialize all the user actions for the current thread.
    virtual StatusCode initializeActions() = 0;

    // For ISF, get UserActions that could have stored secondary particles
    virtual StatusCode getSecondaryActions( std::vector< G4UserSteppingAction* >& actions ) = 0;

    /// @brief In very rare cases, the IUserActionSvc needs to call tools of which it cannot be owner during its initialization
    ///        as these tools declare event data dependencies which is a big no go for a service. Let instead the algorithm own the tool
    ///        and pipe it to the service during initialization
    /// @param service_tool 
    /// @return 
    virtual StatusCode addActionTool(const ToolHandle<IUserActionTool>& service_tool) = 0;


    /// Creates the InterfaceID and interfaceID() method
    DeclareInterfaceID(G4UA::IUserActionSvc, 1, 0);

  }; // class IUserActionSvc

} // namespace G4UA

#endif
