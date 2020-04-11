/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_STGC_DIGITIZATIONTOOL_H
#define MUONDIGITIZATION_STGC_DIGITIZATIONTOOL_H
/** @class sTgcDigitizationTool

    @section sTGC_DigitizerDetails Class methods and properties


    In the initialize() method...
    In the execute() method...

*/

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"
#include "PileUpTools/PileUpToolBase.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "HitManagement/TimedHitCollection.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"
#include "MuonSimEvent/sTGCSimHit.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"

#include "NSWCalibTools/INSWCalibSmearingTool.h"

#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>

/*******************************************************************************/
namespace MuonGM{
  class MuonDetectorManager;
}
namespace CLHEP{
  class HepRandomEngine;
}
class PileUpMergeSvc;
class sTgcHitIdHelper;
class sTgcDigitMaker;

/*******************************************************************************/
class sTgcDigitizationTool : virtual public IMuonDigitizationTool, public PileUpToolBase {

public:
  sTgcDigitizationTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** Initialize */
  virtual StatusCode initialize();

  // /** When being run from PileUpToolsAlgs, this method is called at the start of
  //       the subevts loop. Not able to access SubEvents */
  StatusCode prepareEvent(const EventContext& ctx, const unsigned int /*nInputEvents*/);
  //
  //   /** When being run from PileUpToolsAlgs, this method is called for each active
  //       bunch-crossing to process current SubEvents bunchXing is in ns */
  StatusCode  processBunchXing(int bunchXing,
                               SubEventIterator bSubEvents,
                               SubEventIterator eSubEvents);

  //   /** When being run from PileUpToolsAlgs, this method is called at the end of
  //       the subevts loop. Not (necessarily) able to access SubEvents */
  StatusCode mergeEvent(const EventContext& ctx);
  /** alternative interface which uses the PileUpMergeSvc to obtain
      all the required SubEvents. */
  virtual StatusCode processAllSubEvents(const EventContext& ctx);

  /** Just calls processAllSubEvents - leaving for back-compatibility
      (IMuonDigitizationTool) */

  /**
     reads GEANT4 hits from StoreGate in each of detector
     components corresponding to sTGC modules which are triplets
     or doublets. A triplet has tree sensitive volumes and a
     double has two. This method calls
     sTgcDigitMaker::executeDigi, which digitizes every hit, for
     every readout element, i.e., a sensitive volume of a
     chamber. (IMuonDigitizationTool)
  */
  StatusCode digitize(const EventContext& ctx);

  /** Finalize */
  StatusCode finalize();

  /** accessors */
  ServiceHandle<IAtRndmGenSvc> getRndmSvc() const { return m_rndmSvc; }    // Random number service
  CLHEP::HepRandomEngine *getRndmEngine() const { return m_rndmEngine; } // Random number engine used

private:
  /** Get next event and extract collection of hit collections */
  StatusCode getNextEvent();
  /** Record sTgcDigitContainer and MuonSimDataCollection */
  StatusCode recordDigitAndSdoContainers(const EventContext& ctx);
  /** Core part of digitization use by mergeEvent (IPileUpTool) and digitize (IMuonDigitizationTool) */
  StatusCode doDigitization(const EventContext& ctx);

protected:
  PileUpMergeSvc *m_mergeSvc; // Pile up service
  CLHEP::HepRandomEngine *m_rndmEngine;    // Random number engine used - not init in SiDigitization
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;      // Random number service
  std::string m_rndmEngineName;// name of random engine

private:

  sTgcHitIdHelper*                         m_hitIdHelper;
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  const MuonGM::MuonDetectorManager*       m_mdManager;
  sTgcDigitMaker*                          m_digitizer;
  TimedHitCollection<sTGCSimHit>*   m_thpcsTGC;
  std::list<sTGCSimHitCollection*>  m_STGCHitCollList;

  ToolHandle<Muon::INSWCalibSmearingTool> m_smearingTool;
  BooleanProperty m_doSmearing;

  std::string m_inputHitCollectionName; // name of the input objects
  SG::WriteHandleKey<sTgcDigitContainer> m_outputDigitCollectionKey{this,"OutputObjectName","sTGC_DIGITS","WriteHandleKey for Output sTgcDigitContainer"}; // name of the output digits
  SG::WriteHandleKey<MuonSimDataCollection> m_outputSDO_CollectionKey{this,"OutputSDOName","sTGC_SDO","WriteHandleKey for Output MuonSimDataCollection"}; // name of the output SDOs
  bool m_needsMcEventCollHelper;

  bool m_doToFCorrection;
  int m_doChannelTypes;
  //double m_noiseFactor;
  float m_readoutThreshold;
  float m_neighborOnThreshold;
  float m_saturation;
  
  bool  m_deadtimeON;
  bool  m_produceDeadDigits;
  float m_deadtimeStrip;
  float m_deadtimePad;
  float m_deadtimeWire;
  float m_readtimeStrip;
  float m_readtimePad;
  float m_readtimeWire;
  float m_timeWindowOffsetPad;
  float m_timeWindowOffsetStrip;
  float m_timeWindowPad;
  float m_timeWindowStrip;
  float m_bunchCrossingTime;
  float m_timeJitterElectronicsStrip;
  float m_timeJitterElectronicsPad;
  float m_hitTimeMergeThreshold;

  // Minimum energy deposit for hit to be digitized
  double m_energyDepositThreshold;

  std::map< Identifier, int > m_hitSourceVec;

  void readDeadtimeConfig();

  uint16_t bcTagging(const float digittime, const int channelType) const;
  int humanBC(uint16_t bctag);

};

#endif // MUONDIGITIZATION_STGC_DIGITIZATIONTOOL_H
