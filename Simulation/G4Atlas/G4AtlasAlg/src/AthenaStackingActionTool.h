/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_ATLASSTACKINGACTIONTOOL_H
#define G4ATLASALG_ATLASSTACKINGACTIONTOOL_H

// STL includes
#include <string>

// Local includes
#include "AthenaStackingAction.h"

// Infrastructure includes
#include "G4AtlasInterfaces/IStackingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"


namespace G4UA
{

  /// @class AthenaStackingActionTool
  /// @brief Tool which manages the AthenaStackingAction
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class AthenaStackingActionTool : public ActionToolBase<AthenaStackingAction>,
                                   public IStackingActionTool
  {

    public:

      /// Standard constructor
      AthenaStackingActionTool(const std::string& type, const std::string& name,
                               const IInterface* parent);

      /// Initialize the tool (just for debugging printout)
      virtual StatusCode initialize() override;

      /// Retrieve the stepping action
      virtual IStackingAction* getStackingAction() override final
      { return static_cast<IStackingAction*>( getAction() ); }

      /// Query interface for gaudi
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

    protected:

      /// Create an action for this thread
      virtual std::unique_ptr<AthenaStackingAction> makeAction() override final;

    private:

      /// Configuration parameters
      AthenaStackingAction::Config m_config;

  }; // class AthenaStackingActionTool

} // namespace G4UA

#endif
