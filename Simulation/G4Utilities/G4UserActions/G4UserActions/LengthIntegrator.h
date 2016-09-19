/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_LengthIntegrator_H
#define G4UserActions_LengthIntegrator_H

#include "G4AtlasTools/UserActionBase.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <map>

class TProfile;
class TProfile2D;


// User action to evaluate the thickness (in %r.l. or i.l.) of all detectors
// traversed by outgoing particles

class LengthIntegrator final: public UserActionBase {

 public:
  LengthIntegrator(const std::string& type, const std::string& name, const IInterface* parent);

  virtual void BeginOfEvent(const G4Event*) override;
  virtual void EndOfEvent(const G4Event*) override;
  virtual void Step(const G4Step*) override;

  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

  virtual StatusCode initialize() override;

 private:

  void regAndFillHist(const std::string&,const std::pair<double,double>&);

  double m_etaPrimary ;
  double m_phiPrimary ;
  std::map<std::string,std::pair<double,double>,std::less<std::string> > m_detThick;

  // profiles for rad length
  TProfile2D* m_rzProfRL;
  std::map<std::string,TProfile*,std::less<std::string> > m_etaMapRL;
  std::map<std::string,TProfile*,std::less<std::string> > m_phiMapRL;

  // profiles for int length
  TProfile2D* m_rzProfIL;
  std::map<std::string,TProfile*,std::less<std::string> > m_etaMapIL;
  std::map<std::string,TProfile*,std::less<std::string> > m_phiMapIL;

  ServiceHandle<ITHistSvc> m_hSvc;
};


//=============================================================================
// New design below for multithreading
//=============================================================================

#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"

namespace G4UA
{

  /// @class LengthIntegrator
  /// @brief A user action used to evaluate thickness of all detectors
  ///        traversed by outgoing particles.
  ///
  /// This user action is currently used only in special runs with geantinos.
  /// Thickness is recorded in terms of both rad length and int length.
  ///
  /// NOTE: the current design is safe for multi-threading, but _not_
  /// performant due to sharing of the histograms and excessive locking. If
  /// this action needs to be used in multi-threaded jobs, we can rewrite it so
  /// that each instance has its own copy of the histograms which get merged in
  /// finalization of the LengthIntegratorTool.
  ///
  class LengthIntegrator final : public IBeginEventAction,
                                 public IEndEventAction,
                                 public ISteppingAction
  {

    public:

      /// Constructor takes the name of the histogram service as argument.
      LengthIntegrator(const std::string& histSvcName);

      /// Called at beginning of G4 event to cache some details about the
      /// current primary vertex and particle. Also resets some measurements.
      virtual void beginOfEvent(const G4Event*) override;

      /// Called at end of G4 event to finalize measurements and fill hists
      virtual void endOfEvent(const G4Event*) override;

      /// Called at every particle step to accumulate thickness.
      virtual void processStep(const G4Step*) override;

    private:

      /// Setup one set of measurement hists for a detector name.
      void regAndFillHist(const std::string&, const std::pair<double, double>&);

      /// Handle to the histogram service
      ServiceHandle<ITHistSvc> m_hSvc;

      /// Cached eta of the current primary
      double m_etaPrimary;
      /// Cached phi of the current primary
      double m_phiPrimary;

      /// Map of detector thickness measurements for current event
      std::map<std::string, std::pair<double, double> > m_detThickMap;

      /// Rad-length profile hist in R-Z
      TProfile2D* m_rzProfRL;
      /// Rad-length profile hist in eta
      std::map<std::string, TProfile*> m_etaMapRL;
      /// Rad-length profile hist in phi
      std::map<std::string, TProfile*> m_phiMapRL;

      /// Int-length profile hist in R-Z
      TProfile2D* m_rzProfIL;
      /// Int-length profile hist in eta
      std::map<std::string, TProfile*> m_etaMapIL;
      /// Int-length profile hist in phi
      std::map<std::string, TProfile*> m_phiMapIL;

  }; // class LengthIntegrator

} // namespace G4UA

#endif
