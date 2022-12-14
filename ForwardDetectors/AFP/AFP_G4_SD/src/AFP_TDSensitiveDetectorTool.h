/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_G4_SD_AFP_TDSensitiveDetectorTool_h
#define AFP_G4_SD_AFP_TDSensitiveDetectorTool_h

// Base class header
#include "G4AtlasTools/SensitiveDetectorBase.h"

// STL headers
#include <string>

class G4VSensitiveDetector;

class AFP_TDSensitiveDetectorTool : public SensitiveDetectorBase
{
public:
  // Constructor
  AFP_TDSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent);

  // Destructor
  ~AFP_TDSensitiveDetectorTool() {}
  /** Beginning of an athena event.  This is where collection initialization should happen.*/
  StatusCode SetupEvent() override final; //FIXME would be good to be able to avoid this.
  /** End of an athena event */
  StatusCode Gather() override final; //FIXME would be good to be able to avoid this.

protected:
  // Make me an SD!
  G4VSensitiveDetector* makeSD() const override final;
};

#endif //AFP_G4_SD_AFP_TDSensitiveDetectorTool_h
