/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWOBJECTCREATORTOOL_H
#define EFLOWOBJECTCREATORTOOL_H

/********************************************************************

NAME:     eflowObjectCreatorTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson (based on eflowObjectBuilder class written by D.Tovey)
CREATED:  15th August, 2005

********************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "eflowRec/eflowBaseAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthLinks/ElementLink.h"
#include "xAODCaloEvent/CaloCluster.h"

#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"

class eflowRecTrack;
class eflowRecCluster;
class eflowCaloObject;
class eflowCaloObjectContainer;

static const InterfaceID IID_eflowObjectCreatorTool("eflowObjectCreatorTool", 1, 0);

class eflowObjectCreatorTool : virtual public eflowBaseAlgTool, public AthAlgTool {

  public:
  
  eflowObjectCreatorTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowObjectCreatorTool() {};

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  StatusCode execute(eflowCaloObject* energyFlowCaloObject);
  void execute(eflowCaloObjectContainer* theEflowCaloObjectContainer);
  StatusCode finalize();

  private:
  StatusCode setupPFOContainers();
  void createChargedEflowObjects(eflowCaloObject* energyFlowCaloObject, bool addClusters = false);
  void createChargedEflowObject(eflowRecTrack* efRecTrack, bool addClusters = false);
  void createNeutralEflowObjects(eflowCaloObject* energyFlowCaloObject);
  /** Function to add cluster moments onto PFO */
  void addMoment(xAOD::CaloCluster::MomentType momentType, xAOD::PFODetails::PFOAttributes pfoAttribute, const xAOD::CaloCluster* theCluster, xAOD::PFO* thePFO);

  /* Name of PFO container to write out: */
  std::string m_PFOOutputName;

  xAOD::PFOContainer* m_chargedPFOContainer;
  xAOD::PFOContainer* m_neutralPFOContainer;
  xAOD::PFOContainer* m_neutralPFOContainer_nonModified;

  bool m_eOverPMode;

  /* String flag to configure for running in golden e/p match mode */
  std::string m_goldenModeString;

  /* Toggle debug output */
  int m_debug;

  /* Bool to toggle which jetetmiss configuration we are in - EM cluster input or LC cluster input */
  bool m_LCMode;

  /** Flag to toggle usage of updated 2015 charged shower subtraction - this disabled charged shower subtraction for tracks found to be in dense environments */
  bool m_useUpdated2015ChargedShowerSubtraction;

};

inline const InterfaceID& eflowObjectCreatorTool::interfaceID() { return IID_eflowObjectCreatorTool; }

#endif
