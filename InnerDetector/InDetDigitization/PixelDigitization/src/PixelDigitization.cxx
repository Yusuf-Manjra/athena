/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDigitization.h"

// Constructor with parameters:
PixelDigitization::PixelDigitization(const std::string &name,
                                     ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{
}

PixelDigitization::~PixelDigitization() { }

// Initialize method:
StatusCode PixelDigitization::initialize() {

  ATH_MSG_DEBUG("initialize()");
  CHECK(m_pixelDigitizationTool.retrieve());
  ATH_MSG_DEBUG ( "Successfully retreived IPixelDigitizaitonTool." );
  return StatusCode::SUCCESS;
}

// Execute method:
StatusCode PixelDigitization::execute() {
  ATH_MSG_DEBUG ( "execute()" );
  return m_pixelDigitizationTool->processAllSubEvents();
}
