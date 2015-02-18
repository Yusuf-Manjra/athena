/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __IASGELECTRONMULTILEPTONSELECTOR__
#define __IASGELECTRONMULTILEPTONSELECTOR__

/**
   @class IAsgElectronMultiLeptonSelector
   @brief Interface to tool to select electrons

   12-MAR-2014, convert to ASG tool

*/


// CONSIDER MERGING THIS WITH IASGELECTRONLIKELIHOODTOOL, SINCE THE INTERFACE
// IS EXACTLY THE SAME. LEAVING THEM SEPERATE FOR NOW IN CASE THEY DIVERGE

// Include the interfaces
#include "PATCore/IAsgSelectionTool.h"

// Include the return object and the underlying ROOT tool
#include "PATCore/TAccept.h"

// Forward declarations
#include "xAODEgamma/ElectronFwd.h"

class IAsgElectronMultiLeptonSelector : virtual public IAsgSelectionTool
{

  ASG_TOOL_INTERFACE(IAsgElectronMultiLeptonSelector)

public:

  /**Virtual Destructor*/
  virtual ~IAsgElectronMultiLeptonSelector() {};

  /** The main accept method: using the generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle* part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron* part ) const = 0;
  /** The main accept method: using the generic interface */
  virtual const Root::TAccept& accept( const xAOD::IParticle& part ) const = 0;

  /** The main accept method: the actual cuts are applied here */
  virtual const Root::TAccept& accept( const xAOD::Electron& part ) const = 0;

  /** Method to get the operating point */
  virtual std::string getOperatingPointName( ) const=0;

}; // End: class definition

#endif

