/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_PHYSICSVALIDATIONUSERACTION_H
#define ISF_GEANT4TOOLS_PHYSICSVALIDATIONUSERACTION_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "G4AtlasInterfaces/IUserActionSvc.h"

#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/IParticleHelper.h"
#include "ISF_Interfaces/IGeoIDSvc.h"

//#include "ISF_Geant4Interfaces/IPhysicsValidationUserAction.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// Atlas G4 Helpers
#include "SimHelpers/SecondaryTracksHelper.h"

#ifndef MAXCHILDREN
#define MAXCHILDREN 40
#endif

#include <string>

// ROOT forward declarations
class TTree;

namespace ISF {
  class IParticleHelper;
  class IParticleBroker;
}

#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "G4AtlasInterfaces/IPreTrackingAction.h"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA{
  namespace iGeant4 {
    class PhysicsValidationUserAction: public IBeginEventAction,  public IEndEventAction,  public IBeginRunAction,  public ISteppingAction,  public IPreTrackingAction, public AthMessaging
    {

    public:

      struct Config
      {
        unsigned int verboseLevel=0;
	bool validationOutput = true;
	std::string validationStream="ISFG4SimKernel";
	ServiceHandle<ITHistSvc> thistSvc=ServiceHandle<ITHistSvc>("THistSvc", "PhysicsValidationUserAction");
	ServiceHandle<ISF::IParticleBroker>  particleBroker=
	  ServiceHandle<ISF::IParticleBroker>("ISF::ParticleBroker/ISF_ParticleStackService", "PhysicsValidationUserAction");
	ToolHandle<ISF::IParticleHelper>     particleHelper=
	  ToolHandle<ISF::IParticleHelper>("ISF::ParticleHelper/ISF_ParticleHelper");
	ServiceHandle<ISF::IGeoIDSvc>        geoIDSvc=
	  ServiceHandle<ISF::IGeoIDSvc>("ISF::GeoIDSvc/ISF_GeoIDSvc", "PhysicsValidationUserAction");  
	
	ServiceHandle<G4UA::IUserActionSvc>  UASvc=
	  ServiceHandle<G4UA::IUserActionSvc>("G4UA::UserActionService","PhysicsValidationUserAction");
	double idR=1150.-1.e-5;
	double idZ=3490.;
	double caloRmean=0.5*(40.+4250.);
	double caloZmean=0.5*(3490.+6740.);
	double muonRmean=0.5*(60.+30000.);
	double muonZmean=0.5*(6740.+30000.);
	double cavernRmean=300000.0;
	double cavernZmean=300000.0;
	
      };

      PhysicsValidationUserAction(const Config& config);
      virtual void beginOfEvent(const G4Event*) override final;
      virtual void endOfEvent(const G4Event*) override final;
      virtual void beginOfRun(const G4Run*) override final;
      virtual void processStep(const G4Step*) override final;
      virtual void preTracking(const G4Track*) override final;
    private:

      Config m_config;
      
      SecondaryTracksHelper m_sHelper;
      
      /** access to the central ISF GeoID serice*/
      ISF::IGeoIDSvc                      *m_geoIDSvcQuick; //!< quickaccess avoiding gaudi ovehead
      
      TTree                                                        *m_particles;    //!< ROOT tree containing track info
      int                                                           m_pdg;
      int                                                           m_scIn;
      int                                                           m_scEnd;
      int                                                           m_gen;
      int                                                           m_geoID;
      float                                                         m_theta;
      float                                                         m_pth;
      float                                                         m_pph;
      float                                                         m_p;
      float                                                         m_eloss;
      float                                                         m_radloss;
      float                                                         m_ionloss;
      float                                                         m_wzOaTr;
      float                                                         m_thIn;
      float                                                         m_phIn;
      float                                                         m_dIn;
      float                                                         m_thEnd;
      float                                                         m_phEnd;
      float                                                         m_dEnd;
      float                                                         m_X0;
      float                                                         m_L0;
      float                                                         m_wZ;
      float                                                         m_dt;
      
      TTree                                                        *m_interactions;   //!< ROOT tree containing vertex info
      mutable int                                                   m_process;
      mutable int                                                   m_pdg_mother;
      mutable int                                                   m_gen_mother;
      mutable int                                                   m_nChild;
      mutable  float                                                m_vtx_dist;
      mutable float                                                 m_vtx_theta;
      mutable float                                                 m_vtx_phi;
      mutable float                                                 m_vtx_e_diff;
      mutable float                                                 m_vtx_p_diff;
      mutable float                                                 m_vtx_plong_diff;
      mutable float                                                 m_vtx_pperp_diff;
      mutable float                                                 m_p_mother;
      mutable float                                                 m_radLength;
      mutable int                                                   m_pdg_child[MAXCHILDREN];     // decay product pdg code
      mutable float                                                 m_fp_child[MAXCHILDREN];      // fraction of incoming momentum
      mutable float                                                 m_oa_child[MAXCHILDREN];      // opening angle wrt the mother
      
      
      int m_volumeOffset;
      int m_minHistoryDepth;
      
      mutable int m_currentTrack;
      std::map<int, int> m_trackGenMap;
      
    }; // class PhysicsValidationUserAction

  } // namespace iGeant4

} // namespace G4UA

#endif // ISF_GEANT4TOOLS_PHYSICSVALIDATIONUSERACTION_H
