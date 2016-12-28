/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4SD_MINIFCALSDTOOL_H
#define LARG4SD_MINIFCALSDTOOL_H

// System includes
#include <string>
#include <vector>

// Project includes
#include "LArG4Code/SimpleSDTool.h"

namespace LArG4
{

  /// @class MiniFCALSDTool
  /// @brief SD tool which manages the Mini FCAL sensitive detectors.
  ///
  /// NOTE: this design is in flux.
  ///
  class MiniFCALSDTool : public SimpleSDTool
  {

    public:

      // Constructor
      MiniFCALSDTool(const std::string& type, const std::string& name,
                     const IInterface* parent);

    private:

      /// Create the SD wrapper for current worker thread
      G4VSensitiveDetector* makeSD() override final;

      /// Hit collection name
      std::string m_hitCollName;

      /// The list of volumes
      std::vector<std::string> m_miniVolumes;

  }; // class MiniFCALSDTool

} // namespace LArG4

#endif
