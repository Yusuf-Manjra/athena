/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETVARTOOL_H
#define JETVARTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"
//#include "JetUtils/JetCaloHelper.h"
#include "JetUtils/JetCaloQualityUtils.h"

#include "Particle/TrackParticleContainer.h"

static const InterfaceID IID_JetVarTool("JetVarTool", 1, 0);

class JetVarTool : public AlgTool {
public:
  JetVarTool(const std::string& type, const std::string& name,
                   const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_JetVarTool; };

  JetVarTool();
  ~JetVarTool();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();

  void setJetTruthCollection(const JetCollection *j) {_jetTruthColl = j;}
  void setJetCollection(const JetCollection *j) {_jetColl = j;}
  void setTrackParticleCollection(const Rec::TrackParticleContainer *t) {_trackParticles = t;}

  const JetCollection *jetTruthCollection() {return _jetTruthColl;}
  const JetCollection *jetCollection() {return _jetColl;}

  const Rec::TrackParticleContainer *trackParticleCollection() {return _trackParticles;}

  StatusCode retrieveContainers();
  StatusCode retrieveJetContainer(std::string jetContainerKey);
  StatusCode retrieveTrackContainer(std::string trackContainerKey);
  StatusCode retrieveTruthJetContainer(std::string truthJetContainerKey);

  float JetPtWeightedSize(const JetCollection *jetcoll);
  float JetPtWeightedSize();

  float JetCaloSamplingPS(const Jet *jet);
  float JetCaloSamplingEM(const Jet *jet);
  float JetCaloSamplingFCAL(const Jet *jet);
  float JetCaloSamplingHEC(const Jet *jet);
  float JetCaloSamplingHEC3(const Jet *jet);
  float JetCaloSamplingTile(const Jet *jet);
  float JetCaloSamplingTile2(const Jet *jet);
  float JetCaloSamplingTile10(const Jet *jet);
  float JetCaloSamplingGap(const Jet *jet);
  float JetCaloSamplingTotal(const Jet *jet);
  float JetCaloSamplingCryo(const Jet* jet);

  float SamplingFracPS(const Jet* jet){return JetCaloSamplingPS(jet) / JetCaloSamplingTotal(jet);}
  float SamplingFracEM(const Jet* jet){return JetCaloSamplingEM(jet) / JetCaloSamplingTotal(jet);}
  float SamplingFracFCAL(const Jet* jet){return JetCaloSamplingFCAL(jet) / JetCaloSamplingTotal(jet);}
  float SamplingFracHEC(const Jet* jet){return JetCaloSamplingHEC(jet) / JetCaloSamplingTotal(jet);}
  float SamplingFracHEC3(const Jet* jet){return JetCaloSamplingHEC3(jet) / JetCaloSamplingTotal(jet);}
  float SamplingFracTile(const Jet* jet){return JetCaloSamplingTile(jet) / JetCaloSamplingTotal(jet);}
  float SamplingFracTile2(const Jet* jet){return JetCaloSamplingTile2(jet) / JetCaloSamplingTotal(jet);}
  float SamplingFracTile10(const Jet* jet){return JetCaloSamplingTile10(jet) / JetCaloSamplingTotal(jet);}
  float SamplingFracGap(const Jet* jet){return JetCaloSamplingGap(jet) / JetCaloSamplingTotal(jet);}
  float SamplingFracCryo(const Jet* jet){return JetCaloSamplingCryo(jet) / JetCaloSamplingTotal(jet);}

  float JetPtWeightedEventEMfraction(const JetCollection *jetcoll);
  float JetPtWeightedEventEMfraction();

  float JetChargeFraction(const Jet *jet);
  float JetChargeFraction(const Jet *jet, const Rec::TrackParticleContainer *trackParticles);

  int JetNumAssociatedTracks(const Jet *jet, const Rec::TrackParticleContainer *trackParticles);
  int JetNumAssociatedTracks(const Jet *jet);
  float JetPtWeightedNumAssociatedTracks(const JetCollection *jetcoll, const Rec::TrackParticleContainer *trackParticles);
  float JetPtWeightedNumAssociatedTracks();

  bool leadingJet() const;
  bool leadingJet(const Jet * & leadJet) const;
  bool leadingJet(const Jet * & leadJet ,const JetCollection *jetcoll) const;
  
  Jet GetLeadingJet() const;

  float leadingJetEt(const JetCollection * jetcoll);
  float leadingJetEt();

  float leadingJetEta(const JetCollection *jetcoll);
  float leadingJetEta();

  float leadingJetPhi(const JetCollection *jetcoll);
  float leadingJetPhi();

  bool secondJet() const;
  bool secondJet(const Jet * & secJet ) const;
  bool secondJet(const Jet * & secJet ,const JetCollection *jetcoll) const;

  float secondJetEt(const JetCollection *jetcoll);
  float secondJetEt();

  float secondJetEta(const JetCollection *jetcoll);
  float secondJetEta();

  float secondJetPhi(const JetCollection *jetcoll);
  float secondJetPhi();

  // for now add non-jet related stuff here as well, until we have a proper place for it somewhere outside MET performance
  float EventEMFraction();

  float leastJetEMfraction();
  float leastJetEMfraction(const JetCollection *jetcoll);

  int lowestN90CellsJet();
  int lowestN90CellsJet(const JetCollection *jetcoll);

  double jetWTileQuality(const Jet* jet);
  double jetWLArQuality(const Jet* jet);

 private:
  /** a handle on Store Gate for access to the Event Store */
  StoreGateSvc* m_storeGate;
  MsgStream *mLog;

  //storegate key names
  std::string _JetCollectionKey;
  std::string _JetTruthCollectionKey;
  std::string _TrackParticleKey;

  //pointers to jet collections
  const JetCollection *_dummyJetColl;
  const JetCollection *_jetColl;
  const JetCollection *_jetTruthColl;

  //pointers to tracks
  const Rec::TrackParticleContainer *_dummyTracks;
  const Rec::TrackParticleContainer *_trackParticles;
};

#endif // JETVARTOOL_H
