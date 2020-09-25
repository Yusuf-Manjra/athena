/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAANALYSISINTERFACES__IASGELECTRONMULTILEPTONSELECTOR__
#define EGAMMAANALYSISINTERFACES__IASGELECTRONMULTILEPTONSELECTOR__

/**
   @class IAsgElectronMultiLeptonSelector
   @brief Interface to tool to select electrons

   12-MAR-2014, convert to ASG tool

*/
// Include the interfaces
#include "PATCore/IAsgSelectionTool.h"
#include "xAODEgamma/ElectronFwd.h"

class IAsgElectronMultiLeptonSelector : virtual public IAsgSelectionTool
{

  ASG_TOOL_INTERFACE(IAsgElectronMultiLeptonSelector)

public:

  /// @name IAsgElectronMultiLeptonSelector methods in addition to the IAsgSelectionTool ones
  ///{@

  /// accept with pointer to  IParticle  so as to not hide the IAsgSelectionTool one
  virtual asg::AcceptData accept( const xAOD::IParticle* part ) const = 0;

  /// accept method with pointer to electron */
  virtual asg::AcceptData accept( const xAOD::Electron* part ) const = 0;

  ///Method to get the operating point */
  virtual std::string getOperatingPointName( ) const=0;

  ///@}

  ///Virtual Destructor*/
  virtual ~IAsgElectronMultiLeptonSelector() {};

}; // End: class definition

#endif

