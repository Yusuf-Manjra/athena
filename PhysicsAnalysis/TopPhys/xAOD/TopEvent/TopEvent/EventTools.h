/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EVENTTOOLS_H_
#define EVENTTOOLS_H_

#include <string>
#include <vector>

#include "xAODMissingET/MissingET.h"

/**
 * @file EventTools.h
 *
 * @brief A few functions for doing operations on particles / events.  Currently
 * holds code for dR, dPhi, MWT, HT, invariantMass, isSimulation and sorting
 * containers in descending pT order.  Note that the xAOD currently doesn't know
 * if it's fast sim or full sim.
 */

namespace xAOD {
  class IParticle;
}

namespace top {
  class Event;
  
  ///this enum defines a flag used to understand the partonic origin of a lepton (tipically a soft muon), i.e. if it is produced by the direct decay of a boson, 
  ///or, in the case it is from a HF hadron decay, what is the boson/parton that originated the HF hadron.
  ///This flag will not work for Sherpa samples given the structure of Sherpa's truth record.
  /// note that in case of light-hadrons->muons the muon will always be considered as from "unknown", because often we cannot reconstruct the history when light hadrons are involved...
  enum class LepPartonOriginFlag{
    Unknown=0,  ///< e.g. this can mean the muon is coming from light hadrons and there is no truth history, or anyway it was impossible to reconstruct the history
    FromAntiTopViaHadronicBosonToHF=-3,  ///< tipically means that the muon is coming from tbar->W->...->muon in some way (or any tbar->W/Z/gamma*/H->...->muon), the "..." must involve HF hadrons
    FromAntiTopViaLeptonicBoson=-2,  ///< tipically means that the muon is coming from tbar->W->...->muon in some way (or any tbar->W/Z/gamma*/H->...->muon), the "..." cannot involve hadrons
    FromAntiTopViaQuarkToHF=-1, ///< tipically means that the muon is coming from tbar->bbar->...->muon in some way (or any tbar->qbar->...->muon), the "..." will for sure involve HF hadrons
    FromTopViaQuarkToHF=1, ///< tipically means that the muon is coming from t->b->...->muon in some way (or any t->q->...->muon), the "..." will for sure involve HF hadrons
    FromTopViaLeptonicBoson=2, ///< tipically means that the muon is coming from t->W->...->muon in some way (or any t->W/Z/gamma*/H->...->muon), the "..." cannot involve hadrons
    FromTopViaHadronicBosonToHF=3, ///< tipically means that the muon is coming from t->W->...->muon in some way (or any t->W/Z/gamma*/H->...->muon), the "..." must involve HF hadrons
    FromLeptonicBoson=4, ///< W/Z/gamma*/H->muon or W/Z/gamma*/H->tau->muon
    FromHadronicBosonToHF=5, ///< W/Z/gamma*/H->HF hadrons->muon
    FromGluonToHFHadron=6, ///< g->HF hadron->muon or g->HF hadron->tau->muon
  };
  
  ///this enum defines a flag used to understand the particle origin of a lepton (tipically a soft muon),
  ///i.e. if it is from a boson decay, from a B-hadron, from a B->D->mu decay, etc...
  enum class LepParticleOriginFlag{
    Unknown=0, ///< tipically this happens for muon from light-hadrons
    FromLeptonicBoson=1, ///< from H/W/Z/gamma* with leptonic decay (note: this will not work for Sherpa, will appear as Unknown)
    FromLeptonicBosonToTau=2, ///< from H/W/Z/gamma*->tau->lep (note: this will not work for Sherpa, will appear as Unknown)
    FromB=3, ///< from direct B-hadron decay
    FromBtoTau=4, ///< from B-hadron to tau to mu decay
    FromBtoC=5, ///< from B-hadron to C-hadron to muon decay
    FromBtoCtoTau=6,  ///< from B-hadron to C-hadron to tau to muon decay
    FromC=7, ///< from direct C-hadron (with no B-hadron parent) decay
    FromCtoTau=8, ///< from C-hadron (with no B-hadron parent) to tau to mu decay
    FromTau=9, ///< from Tau leptonic (not coming from W or HF-hadron, so not sure this can really happen)
    FromLightHadron=10, ///< often these muons are Unknown, but in some cases we have the truth record and we can verify they are from light-hadrons
  };

/**
 * @brief Print an error message and terminate if thingToCheck is false.
 *
 * Lots of the xAOD functions need checking to see if they work or not.  I guess
 * we take the approach that if it didn't work, we should not really try to
 * recover but just print a message and quit as soon as we can.  So, this will
 * either return nothing or quit your program.
 *
 * @param thingToCheck The thing to check.  If it's false then print the error
 * message and quit.
 */
  void check(bool thingToCheck, const std::string& usefulFailureMessage);

/**
 * @brief Calculate the delta-r distance between two particles (e.g. an Electron
 * and a Muon).
 *
 * You could do this directly with the xAOD interface, but like invariant mass,
 * I think it's nicer like this.
 *
 * @param p1 The first particle of the two.
 * @param p2 The second particle of the two.
 * @return delta-R metric.
 */
  double deltaR(const xAOD::IParticle& p1, const xAOD::IParticle& p2);

/**
 * @brief Calculate delta-phi between two particles (e.g. an Electron and a
 * Muon)
 *
 * @param p1 The first particle of the two.
 * @param p2 The second particle of the two.
 * @return Signed delta-phi in radians.
 */
  double deltaPhi(const xAOD::IParticle& p1, const xAOD::IParticle& p2);

/**
 * @brief Calculate the transverse mass of the W from the charged lepton and
 * the missing ET.  The maths looks something a bit like this:
 *
 * sqrt(2. * lepton.pt() * met.met() * (1. - cos(lepton.phi() - met.phi())))
 *
 * @return The calculated value, obviously.
 */
  double mwt(const xAOD::IParticle& lepton, const xAOD::MissingET& met);

/**
 * @brief HT calculation.
 *
 * Uses the pT of the electrons, muons and jets.  If they are present.  It loops
 * through those three containers and adds up the pT.
 *
 * @return The HT value.
 */
  double ht(const top::Event& event);

/**
 * @brief Calculate the invariant mass of two particles.
 *
 * This can be done using the xAOD objects directly, but I think this type
 * of interface is prettier.
 *
 * @param p1 The first particle of the two.
 * @param p2 The second particle of the two.
 *
 * @return The invariant mass of the pair.
 */
  double invariantMass(const xAOD::IParticle& p1, const xAOD::IParticle& p2);

/**
 * @brief Is this event MC simulation (True) or data (False)?
 *
 * @return True if the event is MC simulation.
 */
  bool isSimulation(const top::Event& event);

/**
 * @brief Used when sorting the e, mu, jet, tau containers after CP corrections.
 *
 * In case they have changed order. Containers have pointers in, so this should
 * take pointers as arguments.
 *
 * @param p1 The first particle of the two.
 * @param p2 The second particle of the two.
 *
 */
  bool descendingPtSorter(const xAOD::IParticle* p0, const xAOD::IParticle* p1);

/**
 * @brief Function used to create poisson weights (mean of 1) for bootstrapping
 *
 * @param nreplicas The number of replicas to fill (corresponds to size of output vector)
 * @param eventNumber The event number used to seed TRandom3
 * @param mcChannelNumber The DSID of MC event used to seed TRandom3
 */
  std::vector<int> calculateBootstrapWeights(int nreplicas, int eventNumber, int mcChannelNumber);
}

#endif
