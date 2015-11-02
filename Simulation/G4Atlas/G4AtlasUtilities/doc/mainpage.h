/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage  G4AtlasUtilities
@author Manuel Gallas (gallasm@mail.cern.ch)

@section G4AtlasUtilities Introduction

The G4AtlasUtilities package includes utilities for the modification of events as they are transferred between the generators and Geant4.  The modifiers are all set up in the G4AtlasApps package.

@section G4AtlasUtilities Class Overview

The following filters are available:

 - EtaPhiFilters : This filter removes particles from the primary event (from the generator) based on their angle pointing towards the detector.  Most frequently it is used to remove those particles pointing down the beamline (eta>6).
 - PrimaryEventRotations : This filter applies a rotation to the primary event.
 - VertexPositioners : This filter changes the initial position of the primary event.  It can be used both for a misplaced beamspot and for smearing the beamspot.
 - VertexRangeChecker : This filter ensures that the primary particles are being created in a sensible place (e.g. not outside of the ATLAS detector)

@ref used_G4AtlasUtilities

@ref requirements_G4AtlasUtilities

*/

/**
@page used_G4AtlasUtilities Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_G4AtlasUtilities Requirements
@include requirements
*/

