/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorElement_H
#define DetectorElement_H

#include "CxxUtils/checker_macros.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <iostream>
#include <mutex>
#include <string>
#include <vector>

namespace MuonGM {

  class ATLAS_NOT_THREAD_SAFE DetectorElement {
  public:
    std::string name;
    std::string logVolName;

    DetectorElement(std::string n): name(n)
    {
      logVolName = "";
    }

    void setLogVolName(std::string str)
    {
      logVolName = str;
    }

    virtual void print() {
      MsgStream log(Athena::getMessageSvc(), "MuonGM::DetectorElement");
      log << MSG::INFO << "generic detector element" << endmsg;
    }

    virtual ~DetectorElement() {}

    // Static

    //!
    //! Set Material Manager for all childs of this class
    //! MUST BE CALLED BEFORE ANY THREADS TRY TO CALL @ref getMaterialManager
    //!
    static void setMaterialManager(const AbsMaterialManager & matMan) {
      s_matManager = &matMan;
    }

    static const AbsMaterialManager * getMaterialManager() {
      return s_matManager;
    }

  private:
    //!
    //! Actually, it haves thread-safe access only when @ref getMaterialManager
    //! is called before any @ref getMaterialManager.
    //!
    static const AbsMaterialManager *s_matManager ATLAS_THREAD_SAFE;

  }; // class DetectorElement

} // namespace MuonGM

#endif
