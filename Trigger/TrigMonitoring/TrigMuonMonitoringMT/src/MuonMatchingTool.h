/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONMONITORINGMT_MUONMATCHINGTOOL_H
#define TRIGMUONMONITORINGMT_MUONMATCHINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TRandom1.h"
#include <string>
#include <memory>
#include <tuple>


/**
 * @brief Class that provides functionalities for searching for online muons close to a given offline muon
 * and judging they are matched with the specific criteria.
 */
class MuonMatchingTool : public AthAlgTool {

 public:
  MuonMatchingTool(const std::string& type, const std::string &name, const IInterface* parent);

  virtual StatusCode initialize() override;


  enum L1Items{ L1_MU4=1, L1_MU6, L1_MU10, L1_MU11, L1_MU15, L1_MU20, L1_MU21,ERROR};
  int L1ItemSTI(std::string l1item) const {
    if( "L1_MU4"==l1item) return L1Items::L1_MU4;
    if( "L1_MU6"==l1item) return L1Items::L1_MU6;
    if( "L1_MU10"==l1item) return L1Items::L1_MU10;
    if( "L1_MU11"==l1item) return L1Items::L1_MU11;
    if( "L1_MU15"==l1item) return L1Items::L1_MU15;
    if( "L1_MU20"==l1item) return L1Items::L1_MU20;
    if( "L1_MU21"==l1item) return L1Items::L1_MU21;
    return L1Items::ERROR;
  }

  /**
   * @brief Function that searches for a Level 1 muon candidate and judges if it is matched to a given offline muon.
   * @param mu Offline muon around which Level 1 candidates are searched.
   * @param ctx Reference to the @c EventContext needed for accessing the @c LVL1MuonRoIs container.
   * @param trigger Considered level 1 threshold name, e.g. L1_MU10, etc.
   * @param pass True if a candidate is found.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * @todo Consider returning a smart pointer to prevent users from deleting the returned pointer as it is owned by StoreGate.
   */
  const xAOD::MuonRoI* matchL1(const xAOD::Muon *mu, const EventContext& ctx, std::string trigger, bool &pass) const;

  /**
   * @brief Function that searches for an L2 standalone muon (L2MuonSA) candidate and judges if it is matched to a given offline muon.
   * @param mu Offline muon around which L2MuonSA candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param pass True if the matched candidate passed the hypothesis step.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   * @todo Consider returning a smart pointer to prevent users from deleting the returned pointer as it is owned by StoreGate.
   */
  const xAOD::L2StandAloneMuon* matchSA(const xAOD::Muon *mu, std::string trigger, bool &pass) const;

  /**
   * @brief Function that searches for the L2 standalone muon (L2MuonSA) candidate closest to a given offline muon.
   * @param mu Offline muon around which L2MuonSA candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param dR The dR between the offline muon and the L2MuonSA candidate.
   * @return Pointer to the found candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis and matched,
   * users should check @c pass for the decision and @c pass for knowing if it is really matched.
   * @todo Consider returning a smart pointer to prevent users from deleting the returned pointer as it is owned by StoreGate.
   * @todo Consider improving the argument list.
   */
  const xAOD::L2StandAloneMuon* matchSA(const xAOD::Muon *mu, std::string trigger, float &dR) const;

  /**
   * @brief Function that searches for an L2 combined muon (L2muComb) candidate and judges if it is matched to a given offline muon.
   * @param mu Offline muon around which L2muComb candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param pass True if the matched candidate passed the hypothesis step.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   * @todo Consider returning a smart pointer to prevent users from deleting the returned pointer as it is owned by StoreGate.
   */
  const xAOD::L2CombinedMuon* matchCB(const xAOD::Muon *mu, std::string trigger, bool &pass) const;

  /**
   * @brief Function that searches for an EF standalone muon (EFSA) candidate and judges if it is matched to a given offline muon.
   * @param mu Offline muon around which EFSA candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param pass True if the matched candidate passed the hypothesis step.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   * @todo Consider returning a smart pointer to prevent users from deleting the returned pointer as it is owned by StoreGate.
   */
  const xAOD::Muon* matchEFSA(const xAOD::Muon *mu, std::string trigger, bool &pass) const;

  /**
   * @brief Function that searches for an EF combined muon (EFCB) candidate and judges if it is matched to a given offline muon.
   * @param mu Offline muon around which EFCB candidates are searched.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param pass True if the matched candidate passed the hypothesis step.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   * @todo Consider returning a smart pointer to prevent users from deleting the returned pointer as it is owned by StoreGate.
   */
  const xAOD::Muon* matchEF(const xAOD::Muon *mu, std::string trigger, bool &pass) const;

  /**
   * @brief Function that searches for an offline muon within @c DR_cut from the given eta-phi (of a online muon)
   * @param ctx Reference to the @c EventContext needed for accessing the @c Muons container.
   * @param trigEta Eta of the given online muon
   * @param trigPhi Phi of the given online muon
   * @return Pointer to the matched offline muon. This is @c nullptr when there is no muon found.
   * @todo Consider returning a smart pointer to prevent users from deleting the returned pointer as it is owned by StoreGate.
   */
  const xAOD::Muon* matchOff( const EventContext& ctx, float trigEta, float trigPhi, float DR_cut) const;

  /**
   * @brief Function to extrapolate a Inner Detector track to the pivot plane i.e. the middle layers of the Muon Spectrometer where the level 1 RoI is defined.
   * This functionality is not available yet.
   * @param track Inner Detector track that is extrapolated.
   * @return Pointer to the extrapolated track parameters.
   * @see @c reqdRL1byPt
   * @todo Return std::unique_ptr when this is available.
   */
  const Trk::TrackParameters* extTrackToPivot(const xAOD::TrackParticle *track) const;

  /**
   * @brief Function compute dR used for matching offline muons and level 1 RoIs at the pivot plane.
   * This is needed that the accuracy of the extrapolation by @c extTrackToPivot is dependent on the pt of the offline muon.
   * @param mupt Offline muon pt
   * @return Required dR between the offline muon and Level 1 muons
   */
  static double reqdRL1byPt(double mupt);


 private:
  // private methods
  /**
   * @brief Function that searches for an online muon candidate of type T and judges if it is matched to a given offline muon.
   * @param offl Position of the offline muon used for computing dR.
   * @param trigger Considered chain name, e.g. HLT_mu26_ivarmedium_L1MU20, etc.
   * @param reqdR Requirement of dR used for the matching. Note that reqdR is updated with the dR of the found candidate.
   * @param pass True if the matched candidate passed the hypothesis step.
   * @param trigPosForMatchFunc Function pointer that implements cuts for the online muon candidates.
   * @return Pointer to the matched candidate. This is @c nullptr when there is no candidate found.
   * Important: a valid pointer doesn't mean that it passed the hypothesis, users should check @c pass for the decision.
   * @see MuonMatchingTool.icc for the implementation and MuonMatchingTool.cxx for the instantiation.
   * @todo Consider returning a smart pointer to prevent users from deleting the returned pointer as it is owned by StoreGate.
   * @todo Consider improving the argument list.
   */
  template<class T, class OFFL> const T* match(const OFFL *offl, std::string trigger, float &reqdR, bool &pass,
				   std::tuple<bool,double,double> (*trigPosForMatchFunc)(const T*) = &MuonMatchingTool::trigPosForMatch<T>) const;
  const Amg::Vector3D offlineMuonAtPivot(const xAOD::Muon *mu) const;
  double FermiFunction(double x, double x0, double w) const;
  const Trk::TrackParameters* extTrackToTGC(const xAOD::TrackParticle *track) const;
  const Trk::TrackParameters* extTrackToRPC(const xAOD::TrackParticle *track) const;

  // static methods
  // Template methods that perform different matching schemes for T=xAOD::L2StandAloneMuon, xAOD::L2CombinedMuon and xAOD::Muon (EF).
  template<class T> static inline std::tuple<bool,double,double> trigPosForMatch(const T *trig);
  static inline std::tuple<bool,double,double> trigPosForMatchEFSA(const xAOD::Muon *trig);

  SG::ReadHandleKey<xAOD::MuonRoIContainer> m_MuonRoIContainerKey {this, "MuonRoIContainerName", "LVL1MuonRoIs", "Level 1 muon container"};
  SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey {this, "MuonContainerName", "Muons", "Offline muon container"};

  // properties
  Gaudi::Property<bool> m_use_extrapolator {this, "UseExtrapolator", false, "Flag to enable the extrapolator for matching offline and trigger muons"};

  // tools
  PublicToolHandle<Trig::TrigDecisionTool> m_trigDec {this, "TrigDecisionTool", "Trig::TrigDecisionTool/TrigDecisionTool", "TrigDecisionTool"};
  PublicToolHandle<Trk::IExtrapolator> m_extrapolator {"Trk::Extrapolator/AtlasExtrapolator"};
  //The extrapolator is currently not available. Once it gets available, initialize it with the following, which attempts to retrieve:
  //{this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator", "Track extrapolator"}; 
};

#include "MuonMatchingTool.icc"

#endif //TRIGMUONMONITORINGMT_MUONMATCHINGTOOL_H
