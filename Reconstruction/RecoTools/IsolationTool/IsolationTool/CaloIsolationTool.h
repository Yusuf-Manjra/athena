/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONTOOL_CALOISOLATIONTOOL_H
#define ISOLATIONTOOL_CALOISOLATIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#ifndef XAOD_ANALYSIS
// #include "GaudiKernel/ToolHandle.h"
#include "ParticlesInConeTools/ICaloClustersInConeTool.h"
#include "ParticlesInConeTools/IPFlowObjectsInConeTool.h"
#include "RecoToolInterfaces/ICaloIsolationDecoratorTool.h"
#include "RecoToolInterfaces/IParticleCaloCellAssociationTool.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "CaloRec/CaloClusterProcessor.h"
#endif // XAOD_ANALYSIS

#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "RecoToolInterfaces/ICaloCellIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"
#include "RecoToolInterfaces/INeutralEFlowIsolationTool.h"
#include "RecoToolInterfaces/IsolationCommon.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODBase/IParticle.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODEgamma/Egamma.h"
#include "xAODMuon/Muon.h"
#include "xAODEventShape/EventShape.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"

#include <vector>

namespace xAOD {
  
  class CaloIsolationTool: 
    virtual public ICaloTopoClusterIsolationTool,
    virtual public ICaloCellIsolationTool,
    virtual public INeutralEFlowIsolationTool,
#ifndef XAOD_ANALYSIS
    virtual public ICaloIsolationDecoratorTool,
#endif // XAOD_ANALYSIS
    public asg::AsgTool {
    ASG_TOOL_CLASS3( CaloIsolationTool, ICaloTopoClusterIsolationTool, ICaloCellIsolationTool, INeutralEFlowIsolationTool )
  public:
      CaloIsolationTool(const std::string& name);
      ~CaloIsolationTool(void); // destructor
      
      virtual StatusCode initialize() override;
      StatusCode finalize()
#ifndef XAOD_ANALYSIS
        override
#endif
        ;
      
      /**ICaloTopoClusterIsolationTool interface: */    
      bool caloTopoClusterIsolation( CaloIsolation& result, 
				     const IParticle& tp, 
				     const std::vector<Iso::IsolationType>& cones, 
				     CaloCorrection corrections, 
				     const CaloClusterContainer* container = 0) override; 
      using ICaloTopoClusterIsolationTool::caloTopoClusterIsolation;

      ///// *ICaloCellIsolationTool interface: */    
      bool caloCellIsolation(CaloIsolation& result, const IParticle& particle, 
            const std::vector<Iso::IsolationType>& cones, 
            CaloCorrection corrections, 
            const CaloCellContainer* container = 0) override;
      using ICaloCellIsolationTool::caloCellIsolation;
   
      
      /**INeutralEFlowIsolationTool interface: */    
      bool neutralEflowIsolation( CaloIsolation& result, 
				  const IParticle& tp, 
				  const std::vector<Iso::IsolationType>& cones, 
				  CaloCorrection corrections) override;
      using INeutralEFlowIsolationTool::neutralEflowIsolation;

#ifndef XAOD_ANALYSIS
      bool decorateParticle( IParticle& tp,
                             const std::vector<Iso::IsolationType>& cones,
                             CaloCorrection corrections,
                             const CaloCellContainer* Cells = 0,
                             const CaloClusterContainer* TopClusters = 0) override final; 
#endif
      bool decorateParticle_caloCellIso( IParticle& tp,
                             const std::vector<Iso::IsolationType>& cones,
                             CaloCorrection corrections,
                             const CaloCellContainer* Cells) override final;

      bool decorateParticle_topoClusterIso( IParticle& tp,
                             const std::vector<Iso::IsolationType>& cones,
                             CaloCorrection corrections,
                             const CaloClusterContainer* TopClusters) override final;

      bool decorateParticle_eflowIso( IParticle& tp,
                             const std::vector<Iso::IsolationType>& cones,
                             CaloCorrection corrections) override final;

      /**ICaloCellIsolationTool interface: cast for TrackParticle (etcone muon) */    
      bool caloCellIsolation( CaloIsolation& result, 
#ifndef XAOD_ANALYSIS
      const TrackParticle& tp, 
#endif
      const std::vector<Iso::IsolationType>& cones, CaloCorrection corrections
#ifndef XAOD_ANALYSIS
      , const CaloCellContainer* container = 0 
#endif
      );

      /**ICaloIsolationTool interface: cast for egamma (etcone egamma)*/    
      bool caloCellIsolation( CaloIsolation& result, const Egamma& tp, const std::vector<Iso::IsolationType>& cones, CaloCorrection corrections
#ifndef XAOD_ANALYSIS
      , const CaloCellContainer* container = 0
#endif
      );

      /**ICaloIsolationTool interface: cast for TrackParticle (topoetcone muon)*/    
      bool caloTopoClusterIsolation( CaloIsolation& result,  const TrackParticle& tp, const std::vector<Iso::IsolationType>& cones, CaloCorrection corrections, const CaloClusterContainer* container = 0 );

      /**ICaloIsolationTool interface: cast for egamma (topoetcone egamma)*/    
      bool caloTopoClusterIsolation( CaloIsolation& result, const Egamma& tp, const std::vector<Iso::IsolationType>& cones, CaloCorrection corrections, const CaloClusterContainer* container = 0 );

      /**ICaloIsolationTool interface: cast for egamma (pflowetcone egamma)*/       
      bool neutralEflowIsolation(CaloIsolation& result, const Egamma& eg, const std::vector<Iso::IsolationType>& cones, CaloCorrection corrections);

      /**ICaloIsolationTool interface: cast for egamma (pflowetcone egamma)*/       
      bool neutralEflowIsolation(CaloIsolation& result, const TrackParticle& tp, const std::vector<Iso::IsolationType>& cones, CaloCorrection corrections);

  private:
      // final dressing function
      bool decorateParticle(CaloIsolation& result,
                            IParticle& tp,
                            const std::vector<Iso::IsolationType>& cones,
                            CaloCorrection corrections);
      // etcone computation for TrackParticle
#ifndef XAOD_ANALYSIS
      bool etConeIsolation( CaloIsolation& result, const TrackParticle& tp, 
			    const std::vector<Iso::IsolationType>& isoTypes, 
			    const CaloCellContainer* container );
#endif
      
      // etcone computation for Egamma
#ifndef XAOD_ANALYSIS
      bool etConeIsolation( CaloIsolation& result, const Egamma& eg, 
			    const std::vector<Iso::IsolationType>& isoTypes, 
			    const CaloCellContainer* container );
#endif
			    
			    
      // topoetcone computation (common for TrackParticle and Egamma)
      bool topoConeIsolation(CaloIsolation& result, float eta, float phi, 
			     std::vector<float>& m_coneSizes,
                             bool coreEMonly,
			     const CaloClusterContainer* container = 0);
            
      // sum of topo cluster in a cone
      bool topoClustCones (CaloIsolation& result, float eta, float phi, 
			   std::vector<float>& m_coneSizes,
			   const std::vector<const CaloCluster*>& clusts); 

      /// Correct the topo cluster isolation using sum of topo cluster in core region.
      bool correctIsolationEnergy_TopoCore (CaloIsolation& result, float eta, float phi, 
					  float dEtaMax_core, float dPhiMax_core, float dR2Max_core,
					  const std::vector<const CaloCluster*>& clusts, bool onlyEM=false); 

      // pflow etcone computation (common for TrackParticle and Egamma)
      bool pflowConeIsolation(CaloIsolation& result, float eta, float phi, 
			      std::vector<float>& m_coneSizes,
                              bool coreEMonly,
			      const PFOContainer* container = 0);

      // sum of pt of pflow objects in a cone
      bool pflowObjCones (CaloIsolation& result, float eta, float phi, 
			  std::vector<float>& m_coneSizes,
			  const std::vector<const PFO*>& clusts); 

      /// Correct the pflow isolation using sum of pflow objects in core region.
      bool correctIsolationEnergy_pflowCore (CaloIsolation& result, float eta, float phi, 
					     float detaMax, float dphiMax, float dR2Max,
					     const std::vector<const PFO*>& clusts, bool onlyEM = false); 

      // core eg 5x7 egamma subtraction
      bool correctIsolationEnergy_Eeg57 (CaloIsolation& result, 
					 const std::vector<Iso::IsolationType>& isoTypes, 
					 const Egamma* eg);
      // core for muon subtraction
      bool correctIsolationEnergy_MuonCore(CaloIsolation& result, const TrackParticle& tp);

      // helper to get eta,phi of muon extrap
      bool GetExtrapEtaPhi(const TrackParticle* tp, float& eta, float& phi);

      // pt correction (egamma)
      bool PtCorrection (CaloIsolation& result, 
			 const Egamma& eg, 
			 const std::vector<Iso::IsolationType>& isoTypes);

      // Correction for the underlying event
      bool EDCorrection(CaloIsolation& result, 
			const std::vector<Iso::IsolationType>& isoTypes,
			float eta,
			std::string type = "topo");

      // init result structure
      void initresult(CaloIsolation& result, CaloCorrection corrlist, unsigned int typesize);

      /** get reference particle */
      const IParticle* getReferenceParticle(const IParticle& particle) const;

      // add the calo decoration
      void decorateTrackCaloPosition(const IParticle& particle, float eta, float phi) const;

      //JB
      /** to know what type of electrons */
      const CaloCluster *m_fwdClus;
      const Egamma *m_egObj; 

#ifndef XAOD_ANALYSIS
      ToolHandle<Rec::IParticleCaloCellAssociationTool>        m_assoTool;
      ToolHandle<Trk::IParticleCaloExtensionTool>              m_caloExtTool;
      Trk::TrackParametersIdHelper  m_parsIdHelper;

      // clusters in cone tool
      ToolHandle<ICaloClustersInConeTool> m_clustersInConeTool; 
      
      // pflow objects in cone tool
      ToolHandle<IPFlowObjectsInConeTool> m_pflowObjectsInConeTool; 
      
      /** @brief  Property: calo cluster filling tool */
      ToolHandle<CaloClusterProcessor> m_caloFillRectangularTool;

      /** Property: Use cached caloExtension if avaliable. */
      bool m_useCaloExtensionCaching;
#endif // XAOD_ANALYSIS

      /** @brief Tool for pt-corrected isolation calculation (new)*/
      ToolHandle<CP::IIsolationCorrectionTool> m_IsoLeakCorrectionTool;

      /** @brief vector of calo-id to treat*/
      std::vector<int> m_EMCaloNums;

        /** @brief vector of calo-id to treat*/
      std::vector<int> m_HadCaloNums;

      /** Topo Calo cluster location in event store */
      std::string m_CaloCalTopoCluster; 
      
      /** Property: Use TopoClusters at the EM scale. */
      bool m_useEMScale;

      /** Property: do the ED corrections to topoisolation */
      bool m_doEnergyDensityCorrection;

      /** Property: save only requested corrections (trigger usage mainly) */
      bool m_saveOnlyRequestedCorrections;

      /** Property: exclude tile scintillator*/
      bool m_ExcludeTG3;

      /** Property: Name of the central topocluster energy-density container. */ 
      std::string m_tpEDCentral;

      /** Property: Name of the forward topocluster energy-density container. */ 
      std::string m_tpEDForward;

      //JB
      /** Property: Name of the forward topocluster energy-density container. */ 
      std::string m_tpEDveryForward;

      /** Property: Name of the central neutral energy flow energy-density container. */ 
      std::string m_efEDCentral;

      /** Property: Name of the forward neutral energy flow energy-density container. */ 
      std::string m_efEDForward;

      /** Property: The size of the coneCore core energy calculation. */
      double m_coneCoreSize;
      double m_coneCoreSizeEg;
      double m_coneCoreSizeMu;

      /** map to the orignal particle */
      std::map<const IParticle*, const IParticle*> m_derefMap;

      /** Property: Turn on/off the calo extension decoration. */
      bool m_addCaloDeco;

#ifdef XAOD_ANALYSIS // particlesInCone tool will not be avaible. Write our own...
      bool particlesInCone( float eta, float phi, float dr, std::vector<const CaloCluster*>& clusts );
      bool particlesInCone( float eta, float phi, float dr, std::vector<const PFO*>& clusts );
#endif // XAOD_ANALYSIS
      float Phi_mpi_pi(float x){ 
        while (x >= M_PI) x -= 2.*M_PI;
        while (x < -M_PI) x += 2.*M_PI;
        return x;
      }
    };
  
}	// end of namespace

#endif


