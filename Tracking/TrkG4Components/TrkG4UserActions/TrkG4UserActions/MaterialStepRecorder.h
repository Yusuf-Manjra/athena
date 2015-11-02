/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialStepRecorder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MaterialStepRecorder_H
#define MaterialStepRecorder_H

#include "FadsActions/UserAction.h"
#include <string>
#include <vector>

#include "TrkGeometry/MaterialStepCollection.h"
#include "TrkGeometry/ElementTable.h"
#include "TrkGeometry/Material.h"


/** @class MaterialStepRecorder

    @author Andreas.Salzburger@cern.ch
    @author Wolfgang.Lukas@cern.ch
*/

class StoreGateSvc;

namespace Trk {
    class IPositionMomentumWriter;
}

class MaterialStepRecorder: public FADS::UserAction {

  public:
    /** Standard FADS UsesAction */
    MaterialStepRecorder(std::string s);

    /** All G4 interface methods */
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void SteppingAction(const G4Step*);
    void ParseProperties();

  private:
    StoreGateSvc*                   m_storeGate;
    Trk::MaterialStepCollection*    m_matStepCollection;
    std::string                     m_matStepCollectionName;
	bool                            m_recordComposition;

    double                          m_totalNbOfAtoms;
    size_t                          m_totalSteps;
    size_t                          m_eventID;

    int                             m_verboseLevel;
    
    Trk::ElementTable*              m_elementTable; 
    std::string                     m_elementTableName;
    
    Trk::ElementTable*              m_runElementTable;
    
};

#endif

