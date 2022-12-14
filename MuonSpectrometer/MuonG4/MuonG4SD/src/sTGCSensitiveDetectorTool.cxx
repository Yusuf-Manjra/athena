/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGCSensitiveDetectorTool.h"
#include "sTGCSensitiveDetector.h"

sTGCSensitiveDetectorTool::sTGCSensitiveDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
}

G4VSensitiveDetector* sTGCSensitiveDetectorTool::makeSD() const
{
  ATH_MSG_DEBUG( "Initializing SD" );
  return new sTGCSensitiveDetector(name(), m_outputCollectionNames[0]);
}
