/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ISF_FastCaloSimParamAction_H
#define ISF_FastCaloSimParamAction_H

// STL includes
#include <string>
#include <vector>

// athena simulation includes
#include "FadsActions/UserAction.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

// forward declarations in namespaces
namespace ISF_FCS_Parametrization {
  class FCS_StepInfoCollection;
}
namespace HepMC {
  class GenParticle;
}
// forward declarations in global namespace
//class StoreGateSvc;
class LArVCalculator;
class G4VSolid;
class G4AffineTransform;
class LArVHitMerger;
class StoreGateSvc;
class LArIdentifierConverter;
class TileGeoG4SDCalc;

  /**
   *
   *   @short Class for collecting G4 hit information
   *
   *          Collect and store Geant4 hit information, i.e.
   *          position, deposited energy and time, from hits
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *  @author Sasha Glazov, DESY Hamburg, Germany
   *  @author Zdenek Hubacek, CERN
   *  @author Geoffrey Gilles, Clermont-Ferrand, France
   *
   *
   */

class FastCaloSimParamAction: public FADS::UserAction {

 public:

  //! default constructor
  FastCaloSimParamAction(std::string s);

  ~FastCaloSimParamAction();

  //! run code at begin of event
  void BeginOfEventAction(const G4Event*);
  //! run code at end of event
  void EndOfEventAction(const G4Event*);
  //! run code at begin of run
  void BeginOfRunAction(const G4Run*);
  //! run code at end of event
  void EndOfRunAction(const G4Run*);
  //! run code after each step
  void SteppingAction(const G4Step*);


 private:

  /* data members */

  LArVCalculator* m_current_calculator;
  TileGeoG4SDCalc* m_current_calculator_Tile;

  G4VSolid* m_current_solid;
  G4AffineTransform* m_current_transform;

  // calculators 
  LArVCalculator* m_calculator_EMECIW_pos;            //!< pointer to EMEC inner wheel calculator
  LArVCalculator* m_calculator_EMECIW_neg;            //!< pointer to EMEC inner wheel calculator                                           

  LArVCalculator* m_calculator_EMECOW_pos;            //!< pointer to EMEC outer wheel calculator
  LArVCalculator* m_calculator_EMECOW_neg;

  //LArVCalculator* m_calculator_BIB; //!< BackInnerBarrette 
  LArVCalculator* m_calculator_BOB; //!< BackOuterBarrette

  LArVCalculator* m_calculator_EMB;
  LArVCalculator* m_calculator_FCAL1;
  LArVCalculator* m_calculator_FCAL2;
  LArVCalculator* m_calculator_FCAL3;
  LArVCalculator* m_calculator_HEC;
  LArVCalculator* m_calculator_EMBPS;             //!< calculators for presampler
  LArVCalculator* m_calculator_EMEPS;

  //LArVCalculator* m_calculator_HECLocal;
  TileGeoG4SDCalc* m_calculator_Tile;  

  // helper
  LArIdentifierConverter* m_lar_helper;


  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  StoreGateSvc_t m_storeGateSvc;                  //!< pointer to StoreGate service
  StoreGateSvc* m_detStore;
  //! return pointer to StoreGate service
  StoreGateSvc_t storeGateSvc() const;
  StoreGateSvc* detStoreSvc() const;


  ISF_FCS_Parametrization::FCS_StepInfoCollection* m_eventSteps;    //!< collection of StepInfo
  std::map<std::string,int> m_detectormap;
  std::set<std::string> m_unuseddetector;
  int m_ndetectors;

};

#endif
