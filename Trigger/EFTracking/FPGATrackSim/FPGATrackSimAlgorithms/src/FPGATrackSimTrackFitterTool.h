/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPGATrackSimTRACKFITTERTOOL_H
#define FPGATrackSimTRACKFITTERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "FPGATrackSimMaps/IFPGATrackSimMappingSvc.h"
#include "FPGATrackSimBanks/IFPGATrackSimBankSvc.h"

#include "TrackFitter.h"
#include "FPGATrackSimBanks/FPGATrackSimSectorBank.h"

#include <string>
#include <vector>
#include <ostream>

class IFPGATrackSimMappingSvc;
class IFPGATrackSimBankSvc;
class FPGATrackSimSectorBank;
class TrackFitter;

/////////////////////////////////////////////////////////////////////////////
class FPGATrackSimTrackFitterTool: public AthAlgTool {
public:

  FPGATrackSimTrackFitterTool (const std::string&, const std::string&, const IInterface*);
  ~FPGATrackSimTrackFitterTool() = default;


  StatusCode initialize() override;

  StatusCode getTracks(std::vector<FPGATrackSimRoad*> const & roads, std::vector<FPGATrackSimTrack> & tracks);
  StatusCode getMissingHitsCheckTracks(std::vector<FPGATrackSimTrack> & tracks_guessed);

  StatusCode getNFits(int & n)            { n = m_tfpobj->getNFits();             return StatusCode::SUCCESS; }
  StatusCode getNFitsMajority(int & n)    { n = m_tfpobj->getNFitsMajority();     return StatusCode::SUCCESS; }
  StatusCode getNFitsMajoritySCI(int & n) { n = m_tfpobj->getNFitsMajoritySCI();  return StatusCode::SUCCESS; }
  StatusCode getNFitsMajorityPix(int & n) { n = m_tfpobj->getNFitsMajorityPix();  return StatusCode::SUCCESS; }
  StatusCode getNFitsRecovery(int & n)    { n = m_tfpobj->getNFitsRecovery();     return StatusCode::SUCCESS; }

private:
  std::unique_ptr<TrackFitter> m_tfpobj; // instance of the TrackFitter object

  ServiceHandle<IFPGATrackSimMappingSvc>   m_FPGATrackSimMapping{this,"FPGATrackSimMappingSvc","FPGATrackSimMappingSvc"};
  ServiceHandle<IFPGATrackSimBankSvc>   m_FPGATrackSimBank{this,"FPGATrackSimBankSvc","FPGATrackSimBankSvc"};

  Gaudi::Property <int> m_chi2dof_recovery_min {this, "chi2DofRecoveryMin", 40, "min chi^2 cut for attempting recovery fits"};
  Gaudi::Property <int> m_chi2dof_recovery_max {this, "chi2DofRecoveryMax", 1e30, "max chi^2 cut for attempting recovery fits"};
  Gaudi::Property <bool> m_do2ndStage {this, "Do2ndStageTrackFit", false, "Do 2nd stage track fit"};
  Gaudi::Property <int> m_doMajority {this, "doMajority", 1, "Do Majority fits"};
  Gaudi::Property <int> m_maxNhitsPerPlane { this, "maxHitsPerPlane", -1, "if >0, max hits per plane to consider"};
  Gaudi::Property <int> m_noRecoveryNHits { this, "nHits_noRecovery", -1, "nHits for no recovery"};
  Gaudi::Property <bool> m_guessHits { this, "GuessHits", true,  "If True then we Guess hits, if False then we use separate banks and don't guess"};
  Gaudi::Property <bool> m_doDeltaGPhis { this, "DoDeltaGPhis", false, "If True will do the fit by the delta global phis method"};
  Gaudi::Property <bool> m_doMissingHitsChecks {this, "DoMissingHitsChecks", false, "If True and we guess hits, when we have 8/8 we also drop hits and guess them to compare to true positions"};
  Gaudi::Property <int> m_idealCoordFitType {this, "IdealCoordFitType", 0, "Fit type for idealized coordinates, 0 if off"};
};

#endif // FPGATrackSimTrackFitterTool_h
