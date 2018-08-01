/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     TrigTopoEgammaBuilder.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEgammaRec

AUTHORS:  Teresa Fonseca Martin and Cibran Santamarina Rios
          Phillip Urquijo
          Alessandro Tricoli
          Ryan Mackenzie White (xAOD)
	  Dmitriy Maximov, Fernando Monticelli (SuperClusters)

CREATED:  Nov 2017
MODIFIED: Dec 2017

PURPOSE:  Modification of TrigEgammaRec to make use of superclusters at HLT 
          Implementation discussed in https://its.cern.ch/jira/browse/ATR-17176
          The idea behind this is to replace cluster cpying by SuperClusters in the creation
          of electron and photon trigger candidates (Feature extraction step at HLT)


********************************************************************/

// INCLUDE HEADER FILES:

// Trigger specific files
#include "TrigEgammaRec/TrigTopoEgammaBuilder.h"
#include "xAODEgammaCnv/xAODElectronMonFuncs.h"
#include "xAODEgammaCnv/xAODPhotonMonFuncs.h"

#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"

// Offline files:
#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IEMTrackMatchBuilder.h"
#include "egammaInterfaces/IEMConversionBuilder.h"
#include "egammaInterfaces/IEMShowerBuilder.h"
#include "egammaInterfaces/IEMBremCollectionBuilder.h"
#include "egammaInterfaces/IEMFourMomBuilder.h"
#include "ElectronPhotonSelectorTools/IEGammaAmbiguityTool.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/ICaloCellIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"
// xAOD
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEventShape/EventShape.h"
#include "egammaRecEvent/egammaRecContainer.h"

#include "AthContainers/ConstDataVector.h"
#include "xAODCore/ShallowCopy.h"

#include "egammaEvent/egammaParamDefs.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationHelpers.h"

//Supercluster interfaces.
#include "egammaInterfaces/IegammaTopoClusterCopier.h"
#include "egammaInterfaces/IelectronSuperClusterBuilder.h"
#include "egammaInterfaces/IphotonSuperClusterBuilder.h"

// INCLUDE GAUDI HEADER FILES:
 
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include <algorithm> 
#include <math.h>

//  END OF HEADER FILES INCLUDE

// needed for online monitor histograms
class ISvcLocator;


// Template class for monitoring
namespace {
    template <class DEST,class SRC>
        inline DEST** my_pp_cast(SRC** ptr) {
            return (DEST**)(ptr);
        }
}

/**
 * @brief Evaluate an expression and check for errors,
 *        with an explicitly specified context name.
 * @param EXP The expression to evaluate.
 * @param CONTEXT_NAME The name of the current context.
 *
 * This macro will evaluate @c EXP, which should return a @c StatusCode.
 * If the status is not successful, then an HLT::ERROR is returned.
 */
#define TRIG_CHECK_SC(EXP) do {  \
    StatusCode sc__ = (EXP);     \
    if (! sc__.isSuccess())      \
     return HLT::ERROR;          \
  } while (0)

/////////////////////////////////////////////////////////////////

//  CONSTRUCTOR:

TrigTopoEgammaBuilder::TrigTopoEgammaBuilder(const std::string& name,ISvcLocator* pSvcLocator):
    HLT::FexAlgo(name, pSvcLocator),
    m_electronContainerName("egamma_Electrons"),
    m_photonContainerName("egamma_Photons"),
    m_lumiBlockMuTool("LumiBlockMuTool/LumiBlockMuTool"),
    m_extrapolationTool("EMExtrapolationTools/EMExtrapolationTools")

{

    // The following default properties are configured in TrigEgammaRecConfig using Factories
    // Specific configurations are found in TriggerMenu

    // Container names for persistency
    declareProperty("ElectronContainerName", m_electronContainerName="egamma_Electrons");
    declareProperty("PhotonContainerName",   m_photonContainerName="egamma_Photons");
    // Additional property to retrieve slw and topo containers from TE
    declareProperty("SlwCaloClusterContainerName",m_slwClusterContName="TrigEFCaloCalibFex");
    declareProperty("TopoCaloClusterContainerName",m_topoClusterContName="TrigEgammaTopoClusters");
    // ShowerBuilder (trigger specific)
    declareProperty("ShowerBuilderTool",  m_showerBuilder, "Handle to Shower Builder");
    // FourMomBuilder (trigger specific)
    declareProperty("FourMomBuilderTool", m_fourMomBuilder, "Handle to FourMomBuilder");
    // ConversionBuilder 
    declareProperty("ConversionBuilderTool", m_conversionBuilder, "Handle to ConversionBuilder");
    // TrackMatchBuilder
    declareProperty("TrackMatchBuilderTool", m_trackMatchBuilder, "Handle to TrackMatchBuilder");
    // AmbiguityTool
    declareProperty("AmbiguityTool", m_ambiguityTool, "Handle to AmbiguityTool");

    // TopoClusterCopierTool                        
    declareProperty("TopoClusterCopierTool", m_topoclustercopier, "Handle to TopoClusterCopierTool");
    // ElectronSuperClusterBuilderTool    
    declareProperty("ElectronSuperClusterBuilderTool", m_electronSCBuilder, "Handle to ElectronSuperClusterBuilderTool");
    // PhotonSuperClusterBuilderTool
    declareProperty("PhotonSuperClusterBuilderTool",  m_photonSCBuilder, "Handle to PhotonSuperClusterBuilderTool");
    //ElectronPIDBuilder
    declareProperty("ElectronPIDBuilder", m_electronPIDBuilder, "Handle to ElectronPIDBuilder"); 
    //ElectronCaloPIDBuilder
    declareProperty("ElectronCaloPIDBuilder", m_electronCaloPIDBuilder, "Handle to ElectronCaloPIDBuilder"); 
    //PhotonPIDBuilder
    declareProperty("PhotonPIDBuilder", m_photonPIDBuilder, "Handle to PhotonPIDBuilder"); 
    // Isolation
    declareProperty("TrackIsolationTool",       m_trackIsolationTool,   "Handle of the track IsolationTool");  
    declareProperty("CaloCellIsolationTool",    m_caloCellIsolationTool,   "Handle of the caloCell IsolationTool");  
    declareProperty("CaloTopoIsolationTool",           m_topoIsolationTool,    "Handle of the calo topo IsolationTool");
    declareProperty("doTrackIsolation",       m_doTrackIsolation = false,   "Handle of the track IsolationTool");  
    declareProperty("doCaloCellIsolation",    m_doCaloCellIsolation = false,   "Handle of the caloCell IsolationTool");  
    declareProperty("doCaloTopoIsolation",           m_doTopoIsolation = false,    "Handle of the calo topo IsolationTool");

    /** Luminosity tool */
    declareProperty("LuminosityTool", m_lumiBlockMuTool, "Luminosity Tool");

    declareProperty("ExtrapolationTool", m_extrapolationTool, "Extrapolation tool");

    /** @brief Track to track assoc after brem, set to false. If true and no GSF original track in cone */
    declareProperty("useBremAssoc",                    m_useBremAssoc          = false, "use track to track assoc after brem"); 
    /** @brief isolation type */
    declareProperty("IsoTypes", m_egisoInts, "Isolation types: vector of vector of enum type Iso::IsolationType, stored as float");
    // Set flag for track matching
    declareProperty("doTrackMatching",m_doTrackMatching = false, "run TrackMatchBuilder");
    // Set flag for conversions 
    declareProperty("doConversions",m_doConversions = false, "run ConversionBuilder");

    // Monitoring
    typedef const DataVector<xAOD::Electron> xAODElectronDV_type;
    typedef const DataVector<xAOD::Photon> xAODPhotonDV_type;
     // Showershape monitoring accesible from xAOD::Egamma (common to Electron / Photon)  
    declareMonitoredCollection("El_E237",	 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getShowerShape_e237);
    declareMonitoredCollection("El_E277",	 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getShowerShape_e277);
    declareMonitoredCollection("El_EtHad1", *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getShowerShape_ethad1);
    declareMonitoredCollection("El_WEta1",	 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getShowerShape_weta1);
    declareMonitoredCollection("El_WEta2",	 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getShowerShape_weta2);
    declareMonitoredCollection("El_F1",	 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getShowerShape_f1);
    declareMonitoredCollection("El_Emax2",*my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getShowerShape_e2tsts1);
    declareMonitoredCollection("El_Emins1", *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getShowerShape_emins1);
    declareMonitoredCollection("El_Emax", *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getShowerShape_emaxs1);
    declareMonitoredCollection("El_DEmaxs1", *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getDEmaxs1);
    declareMonitoredCollection("El_wtots1", *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getShowerShape_wtots1);
    declareMonitoredCollection("El_Fracs1", *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getShowerShape_fracs1);
    
    declareMonitoredCollection("El_EtCluster37",	*my_pp_cast<xAODElectronDV_type>(&m_electron_container), &getEtCluster37); 
    declareMonitoredCollection("El_EtCone20",	*my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getIsolation_etcone20);
    declareMonitoredCollection("El_PtCone20",	*my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getIsolation_etcone20);
    declareMonitoredCollection("El_Eta",	        *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getCluster_eta);
    declareMonitoredCollection("El_Phi",	        *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getCluster_phi);
    declareMonitoredCollection("El_ClusterEt",     *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getCluster_et);
    declareMonitoredCollection("El_EnergyBE0",     *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getEnergyBE0);
    declareMonitoredCollection("El_EnergyBE1",     *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getEnergyBE1);
    declareMonitoredCollection("El_EnergyBE2",     *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getEnergyBE2);
    declareMonitoredCollection("El_EnergyBE3",     *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getEnergyBE3);
    declareMonitoredCollection("El_Eaccordion",     *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getEaccordion);
    declareMonitoredCollection("El_E0Eaccordion",     *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getE0Eaccordion);
    
    declareMonitoredCollection("Ph_e237",	 *my_pp_cast <xAODPhotonDV_type>(&m_photon_container), &getShowerShape_e237);
    declareMonitoredCollection("Ph_e277",	 *my_pp_cast <xAODPhotonDV_type>(&m_photon_container), &getShowerShape_e277);
    declareMonitoredCollection("Ph_ethad1", *my_pp_cast <xAODPhotonDV_type>(&m_photon_container), &getShowerShape_ethad1);
    declareMonitoredCollection("Ph_weta1",	 *my_pp_cast <xAODPhotonDV_type>(&m_photon_container), &getShowerShape_weta1);
    declareMonitoredCollection("Ph_weta2",	 *my_pp_cast <xAODPhotonDV_type>(&m_photon_container), &getShowerShape_weta2);
    declareMonitoredCollection("Ph_f1",	 *my_pp_cast <xAODPhotonDV_type>(&m_photon_container), &getShowerShape_f1);
    declareMonitoredCollection("Ph_Emax2",*my_pp_cast <xAODPhotonDV_type>(&m_photon_container), &getShowerShape_e2tsts1);
    declareMonitoredCollection("Ph_Emins1", *my_pp_cast <xAODPhotonDV_type>(&m_photon_container), &getShowerShape_emins1);
    declareMonitoredCollection("Ph_Emax", *my_pp_cast <xAODPhotonDV_type>(&m_photon_container), &getShowerShape_emaxs1);
    declareMonitoredCollection("Ph_wtots1", *my_pp_cast <xAODPhotonDV_type>(&m_photon_container), &getShowerShape_wtots1);
    declareMonitoredCollection("Ph_fracs1", *my_pp_cast <xAODPhotonDV_type>(&m_photon_container), &getShowerShape_fracs1);

    declareMonitoredCollection("Ph_EtCluster37",	*my_pp_cast<xAODElectronDV_type>(&m_photon_container), &getEtCluster37); 
    declareMonitoredCollection("Ph_EtCone20",	*my_pp_cast <xAODElectronDV_type>(&m_photon_container), &getIsolation_etcone20);
    declareMonitoredCollection("Ph_Eta",	        *my_pp_cast <xAODElectronDV_type>(&m_photon_container), &getCluster_eta);
    declareMonitoredCollection("Ph_Phi",	        *my_pp_cast <xAODElectronDV_type>(&m_photon_container), &getCluster_phi);
    declareMonitoredCollection("Ph_ClusterEt",     *my_pp_cast <xAODElectronDV_type>(&m_photon_container), &getCluster_et);
    declareMonitoredCollection("Ph_EnergyBE0",     *my_pp_cast <xAODElectronDV_type>(&m_photon_container), &getEnergyBE0);
    declareMonitoredCollection("Ph_EnergyBE1",     *my_pp_cast <xAODElectronDV_type>(&m_photon_container), &getEnergyBE1);
    declareMonitoredCollection("Ph_EnergyBE2",     *my_pp_cast <xAODElectronDV_type>(&m_photon_container), &getEnergyBE2);
    declareMonitoredCollection("Ph_EnergyBE3",     *my_pp_cast <xAODElectronDV_type>(&m_photon_container), &getEnergyBE3);
    declareMonitoredCollection("Ph_Eaccordion",    *my_pp_cast <xAODElectronDV_type>(&m_photon_container), &getEaccordion);
    declareMonitoredCollection("Ph_E0Eaccordion",  *my_pp_cast <xAODElectronDV_type>(&m_photon_container), &getE0Eaccordion);
    
    //Track-related monitoring accesible from xAOD::Electron
    declareMonitoredCollection("nBLayerHits",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfInnermostPixelLayerHits);
    declareMonitoredCollection("expectBLayerHit",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_expectInnermostPixelLayerHit);
    declareMonitoredCollection("nPixelHits",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfPixelHits);
    declareMonitoredCollection("nSCTHits",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfSCTHits);
    declareMonitoredCollection("nBLayerOutliers",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfInnermostPixelLayerOutliers);
    declareMonitoredCollection("nPixelOutliers",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfPixelOutliers);
    declareMonitoredCollection("nSCTOutliers",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfSCTOutliers);
    declareMonitoredCollection("nTRTHits",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfTRTHits);
    declareMonitoredCollection("nTRTHitsHighTh",         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfTRTHighThresholdHits);
    declareMonitoredCollection("nTRTHitsHighThOutliers", *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfTRTHighThresholdOutliers);
    declareMonitoredCollection("nTRTHitsOutliers",       *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrackSummary_numberOfTRTOutliers);
    declareMonitoredCollection("TrackPt",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrack_pt);
    declareMonitoredCollection("d0",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrack_d0);
    declareMonitoredCollection("z0",	         *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getTrack_z0);
    declareMonitoredCollection("dEta",	                 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getCaloTrackMatch_deltaEta2);
    declareMonitoredCollection("dPhi",	                 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getCaloTrackMatch_deltaPhi2);
    declareMonitoredCollection("dPhiRescaled",	                 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getCaloTrackMatch_deltaPhiRescaled2);
    declareMonitoredCollection("rTRT",	                 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &rTRT);
    declareMonitoredCollection("SigmaD0",	                 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getSigmaD0);
    declareMonitoredCollection("D0sig",	                 *my_pp_cast <xAODElectronDV_type>(&m_electron_container), &getD0sig);

    declareMonitoredStdContainer("LHValue", m_lhval);
    declareMonitoredStdContainer("LHCaloValue", m_lhcaloval);

    //Vertex-related monitoring for Photons
    //TBD
    m_timerTotal=nullptr;
    m_timerTool1=nullptr;
    m_timerTool2=nullptr;
    m_timerTool3=nullptr;
    m_timerTool4=nullptr;
    m_timerTool5=nullptr;
    m_timerIsoTool1=nullptr;
    m_timerIsoTool2=nullptr;
    m_timerIsoTool3=nullptr;
    m_timerPIDTool1=nullptr;
    m_timerPIDTool2=nullptr;
    m_timerPIDTool3=nullptr;
    m_eg_container=nullptr;
    m_electron_container=nullptr;
    m_photon_container=nullptr;

}

// DESTRUCTOR:
  
TrigTopoEgammaBuilder::~TrigTopoEgammaBuilder()
{  }
////////////////////////////////////////////////////////////////////////////
//          beginRun method:


HLT::ErrorCode TrigTopoEgammaBuilder::hltStart()
{
    if(msgLvl() <= MSG::INFO) msg() << MSG::INFO << "in Start()" << endreq;

    return HLT::OK;
}

/////////////////////////////////////////////////////////////////

// INITIALIZE METHOD:

HLT::ErrorCode TrigTopoEgammaBuilder::hltInitialize() {

    msg() << MSG::INFO << "in initialize()" << endreq;

    // Global timers
    if (timerSvc()) m_timerTotal = addTimer("Total");

    // Pointer to Tool Service
    IToolSvc* p_toolSvc = 0;
    if (service("ToolSvc", p_toolSvc).isFailure()) {
        msg() << MSG::FATAL << "REGTEST: Tool Service not found " << endreq;
        return HLT::BAD_JOB_SETUP;
    }

    if(m_doTrackMatching){
        if (m_trackMatchBuilder.empty()) {
            ATH_MSG_ERROR("EMTrackMatchBuilder is empty");
            return HLT::BAD_JOB_SETUP;
        }

        if((m_trackMatchBuilder.retrieve()).isFailure()) {
            ATH_MSG_ERROR("Unable to retrieve "<<m_trackMatchBuilder);
            return HLT::BAD_JOB_SETUP;
        }
        else{
            ATH_MSG_DEBUG("Retrieved Tool "<<m_trackMatchBuilder);
            if (timerSvc()) m_timerTool1 = addTimer("EMTrackMatchBuilder");
        }
    }

    if(m_doTrackMatching && m_doConversions){
        if (m_conversionBuilder.empty()) {
            ATH_MSG_ERROR("EMTrackMatchBuilder is empty");
            return HLT::BAD_JOB_SETUP;
        }

        if((m_conversionBuilder.retrieve()).isFailure()) {
            ATH_MSG_ERROR("Unable to retrieve "<<m_conversionBuilder);
            return HLT::BAD_JOB_SETUP;
        }
        else{
            ATH_MSG_DEBUG("Retrieved Tool "<<m_conversionBuilder);
            if (timerSvc()) m_timerTool2 = addTimer("EMConversionBuilder");
        }
    }
    if (m_ambiguityTool.empty()) {
        ATH_MSG_ERROR("EGammaAmbiguityTool is empty");
        return HLT::BAD_JOB_SETUP;
    }

    if((m_ambiguityTool.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_ambiguityTool);
        return HLT::BAD_JOB_SETUP;
    }
    else{
        ATH_MSG_DEBUG("Retrieved Tool "<<m_ambiguityTool);
        if (timerSvc()) m_timerTool3 = addTimer("EGammaAmbiguityTool");
    }
    
    if (m_fourMomBuilder.empty()) {
        ATH_MSG_ERROR("EMFourMomentumBuilder is empty");
        return HLT::BAD_JOB_SETUP;
    }

    if((m_fourMomBuilder.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_fourMomBuilder);
        return HLT::BAD_JOB_SETUP;
    }
    else{
        ATH_MSG_DEBUG("Retrieved Tool "<<m_fourMomBuilder);
        if (timerSvc()) m_timerTool4 = addTimer("EMFourMomBuilder");
    }

    if (m_showerBuilder.empty()) {
        ATH_MSG_ERROR("EMShowerBuilder is empty");
        return HLT::BAD_JOB_SETUP;
    }

    if((m_showerBuilder.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_showerBuilder);
        return HLT::BAD_JOB_SETUP;
    }
    else {
        ATH_MSG_DEBUG("Retrieved Tool "<<m_showerBuilder);
        if (timerSvc()) m_timerTool5 = addTimer("EMShowerBuilder");
    }
    
    
    // TopoClusterCopierTool
    if (m_topoclustercopier.empty()) {
        ATH_MSG_ERROR("TopoClusterCopierTool is empty");
        return HLT::BAD_JOB_SETUP;
    }

    if((m_topoclustercopier.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_topoclustercopier);
        return HLT::BAD_JOB_SETUP;
    } else {
        ATH_MSG_DEBUG("Retrieved Tool "<<m_topoclustercopier);
    }

    // ElectronSuperClusterBuilderTool
    if (m_electronSCBuilder.empty()) {
        ATH_MSG_ERROR("ElectronSuperClusterBuilderTool is empty");
        return HLT::BAD_JOB_SETUP;
    }

    if((m_electronSCBuilder.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_electronSCBuilder);
        return HLT::BAD_JOB_SETUP;
    } else {
        ATH_MSG_DEBUG("Retrieved Tool "<<m_electronSCBuilder);
    }

    // PhotonSuperClusterBuilderTool
    if (m_photonSCBuilder.empty()) {
        ATH_MSG_ERROR("PhotonSuperClusterBuilderTool is empty");
        return HLT::BAD_JOB_SETUP;
    }

    if((m_photonSCBuilder.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_photonSCBuilder);
        return HLT::BAD_JOB_SETUP;
    } else {
        ATH_MSG_DEBUG("Retrieved Tool "<<m_photonSCBuilder);
    }


    /** @brief Retrieve EMPIDBuilders */ 
    if (m_electronPIDBuilder.empty()) {
        ATH_MSG_ERROR("ElectronPIDBuilder is empty");
        return HLT::BAD_JOB_SETUP;
    }
    if((m_electronPIDBuilder.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_electronPIDBuilder);
        return HLT::BAD_JOB_SETUP;
    }
    else {
        ATH_MSG_DEBUG("Retrieved Tool "<<m_electronPIDBuilder);
        if (timerSvc()) m_timerPIDTool1 = addTimer("ElectronPIDBuilder");
    }
    
    if (m_electronCaloPIDBuilder.empty()) {
        ATH_MSG_ERROR("ElectronCaloPIDBuilder is empty");
        return HLT::BAD_JOB_SETUP;
    }
    if((m_electronCaloPIDBuilder.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_electronCaloPIDBuilder);
        return HLT::BAD_JOB_SETUP;
    }
    else {
        ATH_MSG_DEBUG("Retrieved Tool "<<m_electronCaloPIDBuilder);
        if (timerSvc()) m_timerPIDTool2 = addTimer("ElectronCaloPIDBuilder");
    }
    
    if (m_photonPIDBuilder.empty()) {
        ATH_MSG_ERROR("PhotonPIDBuilder is empty");
        return HLT::BAD_JOB_SETUP;
    }
    if((m_photonPIDBuilder.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve "<<m_photonPIDBuilder);
        return HLT::BAD_JOB_SETUP;
    }
    else {
        ATH_MSG_DEBUG("Retrieved Tool "<<m_photonPIDBuilder);
        if (timerSvc()) m_timerPIDTool3 = addTimer("PhotonPIDBuilder");
    }

    // For now, we don't try to retrieve the lumi tool
    if (m_lumiBlockMuTool.retrieve().isFailure()) {                                     
        ATH_MSG_FATAL("Unable to retrieve Luminosity Tool"); 
        return HLT::ERROR;                                                         
    } else {                                                                     
        ATH_MSG_DEBUG("Successfully retrieved Luminosity Tool");
    }      

    /* the extrapolation tool*/
    if(m_extrapolationTool.retrieve().isFailure()){
        ATH_MSG_ERROR("Unable to retrieve extrapolationTool " << m_extrapolationTool);
        return HLT::BAD_JOB_SETUP;
    }

    /** @brief Isolation Configuration */ 
    // Build helpers and decorators for all required isolations
    std::set<std::string> runIsoType;
    unsigned int nI = m_egisoInts.size();
    if (nI > 0 && m_egisoInts[0].size() > 0)
        ATH_MSG_INFO("Will built egamma isolation types : ");
    else
        nI = 0;
    for (unsigned int ii = 0; ii < nI; ii++) {
        std::vector<std::string> isoNames;
        std::vector<xAOD::Iso::IsolationType> isoTypes;
        std::vector<SG::AuxElement::Decorator<float>*> Deco;
        std::string flavName;
        for (unsigned int jj = 0; jj < m_egisoInts[ii].size(); jj++) {
            int egIso = int(m_egisoInts[ii][jj]+0.5);
            std::pair<std::string,std::string> apair = decodeEnum(egIso);
            flavName = apair.first;
            std::string decName  = apair.second;
            isoNames.push_back(decName);
            xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(egIso);
            isoTypes.push_back(isoType);
            Deco.push_back(new SG::AuxElement::Decorator<float>(decName));
            ATH_MSG_INFO(decName);
            if (flavName == "ptcone") {
                decName = "ptvarcone";
                int is = 100*xAOD::Iso::coneSize(isoType);
                decName += std::to_string(is);
                Deco.push_back(new SG::AuxElement::Decorator<float>(decName));
                ATH_MSG_INFO(decName);
            }
        }
        IsoHelp isoH;
        isoH.isoNames = isoNames;
        isoH.isoTypes = isoTypes;
        isoH.isoDeco  = Deco;
        if (flavName == "etcone" || flavName == "topoetcone" ){ 
            CaloIsoHelp cisoH;
            cisoH.help = isoH;
            xAOD::CaloCorrection corrlist;
            // Subtract central 5x7 cells from cone
            corrlist.calobitset.set(static_cast<unsigned int>(xAOD::Iso::core57cells));
            // Correct for out of cone leakage as function of pt
            corrlist.calobitset.set(static_cast<unsigned int>(xAOD::Iso::ptCorrection));
            //ED correction
            if( flavName == "topoetcone"){
                corrlist.calobitset.set(static_cast<unsigned int>(xAOD::Iso::pileupCorrection));
            } 
            cisoH.CorrList = corrlist;
            m_egCaloIso.insert(make_pair(flavName,cisoH));
        } else if (flavName == "ptcone") {
            TrackIsoHelp tisoH;
            tisoH.help = isoH;
            xAOD::TrackCorrection corrlist;
            // Remove electron track from cone
            corrlist.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
            tisoH.CorrList = corrlist;
            m_egTrackIso.insert(make_pair(flavName,tisoH));
        } else
            ATH_MSG_WARNING("Isolation flavour " << flavName << " does not exist ! Check your inputs");
        if (runIsoType.find(flavName) == runIsoType.end()) runIsoType.insert(flavName);
    }
    
    /** @brief Retrieve IsolationTools based on IsoTypes configured */
    if(m_doTrackIsolation){
        if (!m_trackIsolationTool.empty() && runIsoType.find("ptcone") != runIsoType.end()){ 
            ATH_MSG_DEBUG("Retrieve TrackIsolationTool");

            if(m_trackIsolationTool.retrieve().isFailure()){
                ATH_MSG_ERROR("Unable to retrieve " << m_trackIsolationTool);
                return HLT::BAD_JOB_SETUP;
            }
            else {
                ATH_MSG_DEBUG("Retrieved Tool "<<m_trackIsolationTool);
                if (timerSvc()) m_timerIsoTool1 = addTimer("TrackIsolationTool");
            }
        }
    }
    if(m_doCaloCellIsolation){
        if (!m_caloCellIsolationTool.empty() && runIsoType.find("etcone") != runIsoType.end()) {
            ATH_MSG_DEBUG("Retrieve CaloIsolationTool is empty");
            if(m_caloCellIsolationTool.retrieve().isFailure()){
                ATH_MSG_ERROR("Unable to retrieve " << m_caloCellIsolationTool);
                return HLT::BAD_JOB_SETUP;
            }
            else {
                ATH_MSG_DEBUG("Retrieved Tool "<<m_caloCellIsolationTool);
                if (timerSvc()) m_timerIsoTool2 = addTimer("CaloCellIsolationTool");
            }
        }
    }
    if(m_doTopoIsolation){
        if (!m_topoIsolationTool.empty() && runIsoType.find("topoetcone") != runIsoType.end()) {
            ATH_MSG_DEBUG("Retrieve TopoIsolationTool is empty");
            if(m_topoIsolationTool.retrieve().isFailure()){
                ATH_MSG_ERROR("Unable to retrieve " << m_topoIsolationTool);
                return HLT::BAD_JOB_SETUP;
            }
            else {
                ATH_MSG_DEBUG("Retrieved Tool "<<m_topoIsolationTool);
                if (timerSvc()) m_timerIsoTool3 = addTimer("topoIsolationTool");
            }
        }
    }
    //print summary info
    ATH_MSG_INFO("REGTEST: xAOD Reconstruction for Run2" );
    ATH_MSG_INFO("REGTEST: Initialization completed successfully, tools initialized:  " );
    ATH_MSG_INFO("REGTEST: TrackMatch tool: "   <<      m_trackMatchBuilder );
    ATH_MSG_INFO("REGTEST: Conversions tool: "  <<      m_conversionBuilder );
    ATH_MSG_INFO("REGTEST: Shower  tool: "      <<      m_showerBuilder );
    ATH_MSG_INFO("REGTEST: Four Momentum tool: "<<      m_fourMomBuilder );
    ATH_MSG_INFO("REGTEST: Ambiguity tool: "    <<      m_ambiguityTool );
    ATH_MSG_INFO("REGTEST: TrackIsolationTool " <<      m_trackIsolationTool );
    ATH_MSG_INFO("REGTEST: CaloCellIsolationTool " <<   m_caloCellIsolationTool );
    ATH_MSG_INFO("REGTEST: TopoIsolationTool "  <<      m_topoIsolationTool );
    ATH_MSG_INFO("REGTEST: ElectronPID tool: "  <<      m_electronPIDBuilder); 
    ATH_MSG_INFO("REGTEST: ElectronCaloPID tool: " <<   m_electronCaloPIDBuilder); 
    ATH_MSG_INFO("REGTEST: PhotonPID tool: "    <<      m_photonPIDBuilder);
    ATH_MSG_INFO("REGTEST: LumiTool "           <<      m_lumiBlockMuTool);
    ATH_MSG_INFO("REGTEST: ExtrapolationTool: " <<      m_extrapolationTool);


    ATH_MSG_DEBUG("REGTEST: Check properties");
    ATH_MSG_DEBUG("REGTEST: TrackIsolation "        << m_doTrackIsolation);
    ATH_MSG_DEBUG("REGTEST: Do Conversions: "       << m_doConversions);
    ATH_MSG_DEBUG("REGTEST: Do TrackMatching: "     << m_doTrackMatching);
    ATH_MSG_DEBUG("REGTEST: Do Track Isolation: "   << m_doTrackIsolation);
    ATH_MSG_DEBUG("REGTEST: Do CaloCell Isolation: "<< m_doCaloCellIsolation);
    ATH_MSG_DEBUG("REGTEST: Do TopoIsolation: "     << m_doTopoIsolation);
    ATH_MSG_DEBUG("REGTEST: Use BremAssoc: "        << m_useBremAssoc);

    ATH_MSG_INFO("REGTEST: ElectronContainerName: " << m_electronContainerName );
    ATH_MSG_INFO("REGTEST: PhotonContainerName: " << m_photonContainerName );
    return HLT::OK;
}

/////////////////////////////////////////////////
// FINALIZE METHOD:

HLT::ErrorCode TrigTopoEgammaBuilder::hltFinalize() {
   
    // Delete pointers to decorators
    std::map<std::string,CaloIsoHelp>::iterator itc = m_egCaloIso.begin(), itcE = m_egCaloIso.end();
    for (; itc != itcE; itc++) {
        std::vector<SG::AuxElement::Decorator<float>*> vec = itc->second.help.isoDeco;
        for (unsigned int ii = 0; ii < vec.size(); ii++)
            delete vec[ii];
    }
    std::map<std::string,TrackIsoHelp>::iterator itt = m_egTrackIso.begin(), ittE = m_egTrackIso.end();
    for (; itt != ittE; itt++) {
        std::vector<SG::AuxElement::Decorator<float>*> vec = itt->second.help.isoDeco;
        for (unsigned int ii = 0; ii < vec.size(); ii++)
            delete vec[ii];
    }
    ATH_MSG_INFO("Finalization of TrigTopoEgammaBuilder completed successfully");

    return HLT::OK;
}
/////////////////////////////////////////////////////////////////
//          endRun method:

HLT::ErrorCode TrigTopoEgammaBuilder::hltEndRun()
{
    if(msgLvl() <= MSG::INFO) msg() << MSG::INFO << "in endRun()" << endreq;
    return HLT::OK;
}


//////////////////////////////////////////////////

HLT::ErrorCode TrigTopoEgammaBuilder::hltExecute( const HLT::TriggerElement* inputTE,
        HLT::TriggerElement* outputTE )
{
    if ( msgLvl() <= MSG::DEBUG ) {
        msg() << MSG::DEBUG << "Executing HLT alg. TrigTopoEgammaBuilder p03" << endreq;
        msg() << MSG::DEBUG << "inputTE->getId(): " << inputTE->getId() << endreq;
    } 
    // Time total TrigTopoEgammaBuilder execution time.
    if (timerSvc()) m_timerTotal->start();

    m_eg_container = 0;
    m_electron_container = 0;
    m_photon_container = 0;

    //**********************************************************************
    // Retrieve cluster container
    // get pointer to CaloClusterContainer from the trigger element
    std::vector<const xAOD::CaloClusterContainer*> vectorClusterContainer;
    HLT::ErrorCode stat; 
    if(m_doTopoIsolation) stat = getFeatures(inputTE, vectorClusterContainer,m_slwClusterContName);
    else  stat = getFeatures(inputTE, vectorClusterContainer);
    
    if ( stat!= HLT::OK ) {
        msg() << MSG::ERROR << " REGTEST: No CaloClusterContainers retrieved for the trigger element" << endreq;
        return HLT::OK;
    }
    //debug message
    if ( msgLvl() <= MSG::VERBOSE)
        msg() << MSG::VERBOSE << " REGTEST: Got " << vectorClusterContainer.size()
            << " CaloClusterContainers associated to the TE " << endreq;

    // Check that there is only one ClusterContainer in the RoI
    if (vectorClusterContainer.size() < 1){
        if ( msgLvl() <= MSG::ERROR )
            msg() << MSG::ERROR
                << "REGTEST: Size of vectorClusterContainer is not 1, it is: "
                << vectorClusterContainer.size()
                << endreq;
        //return HLT::BAD_JOB_SETUP;
        return HLT::OK;
    }

    // Get the last ClusterContainer
    const xAOD::CaloClusterContainer* clusContainer = vectorClusterContainer.back();
    if(!clusContainer){
        if ( msgLvl() <= MSG::ERROR )
            msg() << MSG::ERROR
                << " REGTEST: Retrieval of CaloClusterContainer from vector failed"
                << endreq;
        //return HLT::BAD_JOB_SETUP;
        return HLT::OK;
    }
    

    bool topoClusTrue = false; 
    std::vector<const xAOD::CaloClusterContainer*> vectorClusterContainerTopo;
    if(m_doTopoIsolation){
        stat = getFeatures(inputTE, vectorClusterContainerTopo,m_topoClusterContName);
    
        if ( stat!= HLT::OK ) {
            ATH_MSG_ERROR(" REGTEST: No CaloTopoClusterContainers retrieved for the trigger element");
            //return HLT::OK; // If you did not get it, it is not a problem, continue!
        }  
             
        //debug message
        if ( msgLvl() <= MSG::VERBOSE){
        msg() << MSG::VERBOSE << " REGTEST: Got " << vectorClusterContainerTopo.size()
             << " CaloCTopoclusterContainers associated to the TE " << endreq;
        }
        // Get the last ClusterContainer
        if ( !vectorClusterContainerTopo.empty() ) {
            const xAOD::CaloClusterContainer* clusContainerTopo = vectorClusterContainerTopo.back();
            if (clusContainerTopo->size() > 0) topoClusTrue = true;
            ATH_MSG_DEBUG("REGTEST: Number of topo containers : " << clusContainerTopo->size());
        } // vector of Cluster Container empty?!
    }


    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG 
        << clusContainer->size() << " calo clusters in container" << endreq;

    if(clusContainer->size() < 1){
        return HLT::OK;
    }

    //**********************************************************************
    // Create an EgammaRec container  
    m_eg_container = new EgammaRecContainer();
    std::string electronContSGKey="";
    std::string electronKey="";
    HLT::ErrorCode sc = getUniqueKey( m_electron_container, electronContSGKey, electronKey);
    if (sc != HLT::OK) {
        msg() << MSG::DEBUG << "Could not retrieve the electron container key" << endreq;
        return sc;
    }

    electronContSGKey+="egammaRec";
    //    // just store the thing now!
    if ( store()-> record(m_eg_container,electronContSGKey).isFailure() ) {
        msg() << MSG::ERROR << "REGTEST: Could not register the EgammaRecContainer" << endreq;
        return HLT::ERROR;
    }
    // Create collections used in the navigation
    // Electrons
    m_electron_container = new xAOD::ElectronContainer();
    xAOD::ElectronTrigAuxContainer electronAux;
    m_electron_container->setStore(&electronAux);

    // Photons
    m_photon_container = new xAOD::PhotonContainer();
    xAOD::PhotonTrigAuxContainer photonAux;
    m_photon_container->setStore(&photonAux);
    
    //***************************************************************************************************************
    // Retrieve from TE containers needed for tool execution, set corresponding flag to be used in execution
    //***************************************************************************************************************

    // Shower Shape & CaloCellContainer
    const CaloCellContainer* pCaloCellContainer = 0;
    const xAOD::CaloClusterContainer* pTopoClusterContainer = 0;
    
    // Get vector of pointers to all CaloCellContainers from TE
    std::string clusCollKey="";
    std::vector<const CaloCellContainer*> vectorCellContainer;
    
    if(m_doTopoIsolation) stat = getFeatures(inputTE, vectorCellContainer,"TrigCaloCellMaker");
    else stat = getFeatures(inputTE, vectorCellContainer);

    if ( stat != HLT::OK ) {
        //m_showerBuilderToExec = false;
        msg() << MSG::ERROR << "REGTEST: No CaloCellContainers retrieved for the trigger element, shower builder to be executed: " << m_showerBuilder << endreq;
        //Should this be ERROR? Maybe just set to run showerbuilder to false?
        return HLT::ERROR;
        //May need to add ERROR codes for online debugging
    } else{
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " REGTEST: Got " << vectorCellContainer.size()
            << " CaloCellContainers associated to the TE " << endreq;
        
        if ( getStoreGateKey( clusContainer, clusCollKey) != HLT::OK) {
            ATH_MSG_ERROR("Failed to get key for ClusterContainer");
        }
        else ATH_MSG_DEBUG("Cluster Collection key in SG: " << clusCollKey);
             
        // Check that there is only one CellContainer in the RoI
        if (vectorCellContainer.size() != 1){
            //m_showerBuilderToExec = false;
            msg() << MSG::ERROR
                << "REGTEST: Size of calo cell container vector is not 1 but " << vectorCellContainer.size()
                << " shower builder to be executed: " << m_showerBuilder
                << endreq;
            return HLT::ERROR;
            //May need to add ERROR codes for online debugging
        } else{

            // Get the last CellContainer if ShoweShapeTool is going to be run
            pCaloCellContainer = vectorCellContainer.back();

            if(!pCaloCellContainer){
                msg() << MSG::ERROR
                    << "Retrieval of CaloCellContainer from vector failed, m_showerBuilderToExec: " << m_showerBuilder
                    << endreq;
                return HLT::ERROR;
                //May need to add ERROR codes for online debugging
            } else{
                if ( msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "Running m_showerBuilder: " << m_showerBuilder << endreq;
            } //pCaloCellContainer
        }
        if(topoClusTrue) pTopoClusterContainer = vectorClusterContainerTopo.back();
    }

    //**********************************************************************
    // If TrackMatchBuilder tool is going to be run 
    // get pointer to TrackParticleContainer from the trigger element and set flag for execution

    const xAOD::TrackParticleContainer* pTrackParticleContainer = 0;
    xAOD::Vertex leadTrkVtx;
    leadTrkVtx.makePrivateStore();
    std::string TrkCollKey="";
    if (m_doTrackMatching){
        std::vector<const xAOD::TrackParticleContainer*> vectorTrackParticleContainer;
        stat = getFeatures(inputTE, vectorTrackParticleContainer);
        // in case a TrackParticleContainer is retrieved from the TE
        if (stat != HLT::OK) {
            m_doTrackMatching=false;
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG 
                << " REGTEST: no TrackParticleContainer from TE, m_trackMatchBuilder: " << m_trackMatchBuilder << endreq;

        } else {
            // check that it is not empty
            if (vectorTrackParticleContainer.size() < 1) {
                if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
                    << " REGTEST: empty TrackParticleContainer from TE, m_trackMatchBuilder: " << m_trackMatchBuilder << endreq;

            } else {
                // Get the pointer to last TrackParticleContainer 
                pTrackParticleContainer = vectorTrackParticleContainer.back();
                m_doTrackMatching = true;
                if(!pTrackParticleContainer){
                    m_doTrackMatching = false;
                    msg() << MSG::ERROR
                        << " REGTEST: Retrieval of TrackParticleContainer from vector failed"
                        << endreq;
                }
                ATH_MSG_DEBUG("m_doTrackMatching: " << m_doTrackMatching);
                if ( getStoreGateKey( pTrackParticleContainer, TrkCollKey) != HLT::OK) {
                    ATH_MSG_ERROR("Failed to get key for TrackContainer");
                }
                else ATH_MSG_DEBUG("Track Collection key in SG: " << TrkCollKey);
            }
        }

        // Create vertex from leading track in RoI
        float leadTrkpt=0.0;
        const xAOD::TrackParticle *leadTrk=NULL;
        for (const xAOD::TrackParticle *trk:*pTrackParticleContainer) {
            if(trk->pt() > leadTrkpt) {
                leadTrkpt = trk->pt();
                leadTrk=trk;
            }
        }
        // set z vertex position to leading track and shift by vz
        if(leadTrk!=NULL) leadTrkVtx.setZ(leadTrk->z0()+leadTrk->vz());
        else ATH_MSG_DEBUG("REGTEST: Lead track pointer null");

    }//m_trackMatching

    //**********************************************************************
    // If Conversion Builder or EMFourMomBuilder are going to be run
    const xAOD::VertexContainer* pVxContainer= 0;
    std::vector<const xAOD::VertexContainer*> vectorVxContainer;
    stat = getFeatures(inputTE,vectorVxContainer);
    // in case a VxContainer is retrieved from the TE
    if (stat != HLT::OK) {
        m_doConversions = false;
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
            << " REGTEST: no VxContainer from TE, m_doConversions: " << m_doConversions << endreq;
    } else {
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " REGTEST: Got " << vectorVxContainer.size()
            << " VertexContainers associated to the TE " << endreq;
        // check that it is not empty
        if (vectorVxContainer.size() < 1) {
            m_doConversions= false;
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
                << " REGTEST: no VxContainer from TE, m_doConversions: " << m_doConversions << endreq;
        } else {
            // Get the pointer to last VxContainer 
            pVxContainer = vectorVxContainer.back();
            if(!pVxContainer){
                m_doConversions = false;
                msg() << MSG::ERROR
                    << " REGTEST: Retrieval of VxContainer from vector failed"
                    << endreq;
            }
            if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "m_doConversions " << m_doConversions << endreq;
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Following offline code

    //--------------------------------
    //Create a shallow copy, the elements of this can be modified,
    //but no need to recreate the cluster

    ATH_MSG_DEBUG( "clusContainer: ownPolicy = " << clusContainer->ownPolicy() );
    ATH_MSG_DEBUG( "clusContainer: getStore = " << clusContainer->getStore() << ", getConstStore = " << clusContainer->getConstStore() );
    ATH_MSG_DEBUG( "clusContainer: hasStore = " << clusContainer->hasStore() << ", hasNonConstStore = " << clusContainer->hasNonConstStore() );

    xAOD::CaloClusterContainer clusContainer_tmp;
    xAOD::CaloClusterAuxContainer clusContainer_tmpAux;
    clusContainer_tmp.setStore(&clusContainer_tmpAux);

    static SG::AuxElement::Accessor < ElementLink < xAOD::CaloClusterContainer > > sisterCluster("SisterCluster");
    static const SG::AuxElement::Accessor < ElementLink < xAOD::CaloClusterContainer > > const_sisterCluster("SisterCluster");

    ATH_MSG_DEBUG( "before copy " << clusContainer->size() << " clusters");

    xAOD::CaloClusterContainer::const_iterator cciter = clusContainer->begin();
    xAOD::CaloClusterContainer::const_iterator ccend  = clusContainer->end();
    for (; cciter != ccend; ++cciter) {
        ATH_MSG_DEBUG("->CHECKING Cluster " << (cciter-clusContainer->begin()) <<
                       " at eta, phi, et: " << (*cciter)->eta() << ", " <<
                       (*cciter)->phi() << ", " << (*cciter)->et() << 
                       ", NCells=" << (*cciter)->size() <<
                       ", cell_begin==cell_end " << ( (*cciter)->cell_begin() == (*cciter)->cell_end() ) );
        ATH_MSG_DEBUG("           getCellLinks=" << (*cciter)->getCellLinks() );

        if(msgLvl() <= MSG::DEBUG) {
            double emfrac(-11111);
            if(!(*cciter)->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emfrac)){
                ATH_MSG_WARNING("No EM fraction momement stored in original cluster");
            }
            ATH_MSG_DEBUG(" >CHECKING Cluster: em_fraction=" << emfrac );
        }
        ATH_MSG_DEBUG(" >CHECKING Cluster: sisterCluster.isAvailable=" << sisterCluster.isAvailable( **cciter ) );

        xAOD::CaloCluster * _tmp_clus = new xAOD::CaloCluster( **cciter );

        // make a link from copy to original
        const ElementLink< xAOD::CaloClusterContainer > clusterLink( *clusContainer, (cciter-clusContainer->begin()) );
        sisterCluster(*_tmp_clus) = clusterLink;

        clusContainer_tmp.push_back( _tmp_clus );

        ATH_MSG_DEBUG("->CHECKING _tmp_clus=" << _tmp_clus  );
        ATH_MSG_DEBUG("           at eta, phi, et: " << _tmp_clus->eta() << ", " << _tmp_clus->phi() << ", " << _tmp_clus->et() );
        ATH_MSG_DEBUG("           NCells=" << _tmp_clus->size() );
        ATH_MSG_DEBUG("           getCellLinks=" << _tmp_clus->getCellLinks() );
        ATH_MSG_DEBUG(" >CHECKING copy Cluster: sisterCluster.isAvailable=" << const_sisterCluster.isAvailable( *_tmp_clus ) );

    }

    ATH_MSG_DEBUG( "before xAOD::shallowCopyContainer, clusContainer_tmp.size=" << clusContainer_tmp.size() );

    std::pair<xAOD::CaloClusterContainer*, xAOD::ShallowAuxContainer* > inputShallowcopy = xAOD::shallowCopyContainer( clusContainer_tmp );

    ATH_MSG_DEBUG( "inputShallowcopy.first->size=" << inputShallowcopy.first->size() );


    for( unsigned int i = 0; i<inputShallowcopy.first->size(); i++) {
        ATH_MSG_DEBUG("Before accessing clusters_copy->at("<<i<<")");
        xAOD::CaloCluster *clus = inputShallowcopy.first->at(i);
        ATH_MSG_DEBUG("->CHECKING inputShallowcopy.first Cluster i=" << i << ", clus=" << clus );
        ATH_MSG_DEBUG("           at eta, phi, et: " << clus->eta() << ", " << clus->phi() << ", " << clus->et() );
        ATH_MSG_DEBUG("           getCellLinks=" << clus->getCellLinks() );
        ATH_MSG_DEBUG(" >CHECKING inputShallowcopy.first Cluster: sisterCluster.isAvailable=" << const_sisterCluster.isAvailable( *clus ) );

        const xAOD::CaloCluster * other = clusContainer_tmp.at(i);
        const CaloClusterCellLink* links=other->getCellLinks();
        if (links) {
            clus->addCellLink(new CaloClusterCellLink(*links));
        } else {
            ATH_MSG_WARNING("No CellLinks in source cluster "<< i << "of xAOD::shallowCopyContainer");
        }
    } // End attaching clusters to egammaRec

    //Here it just needs to be a view copy ,
    //i.e the collection we create does not really
    //own its elements
    ConstDataVector<xAOD::CaloClusterContainer>* viewCopy =  new ConstDataVector <xAOD::CaloClusterContainer> (SG::VIEW_ELEMENTS );

    //First run the egamma Topo Copier that will select copy over cluster of interest to egammaTopoCluster
    TRIG_CHECK_SC(m_topoclustercopier->hltExecute(inputShallowcopy, viewCopy));
    ATH_MSG_DEBUG( "viewCopy->size=" << viewCopy->size() );

    //Then retrieve them
    const xAOD::CaloClusterContainer *clusters_copy = viewCopy->asDataVector();

    ATH_MSG_DEBUG( "after topoclustercopier, clusters_copy->size=" << clusters_copy->size() );
    //--------------------------------

    // loop over clusters.
    // Add egammaRec into container
    // then do next steps
    for( unsigned int i = 0; i<clusters_copy->size(); i++){
        ATH_MSG_DEBUG("Before accessing clusters_copy->at("<<i<<")");
        const xAOD::CaloCluster *clus = clusters_copy->at(i);
        ATH_MSG_DEBUG("->CHECKING copy Cluster i=" << i << ", clus=" << clus );
        ATH_MSG_DEBUG("           at eta, phi, et: " << clus->eta() << ", " << clus->phi() << ", " << clus->et() );
        ATH_MSG_DEBUG("           getCellLinks=" << clus->getCellLinks() );
        ATH_MSG_DEBUG(" >CHECKING copy Cluster: sisterCluster.isAvailable=" << const_sisterCluster.isAvailable( *clus ) );

        // create a new egamma object + corresponding egDetail container
        const ElementLink< xAOD::CaloClusterContainer > clusterLink( *clusters_copy, i );
        const std::vector< ElementLink<xAOD::CaloClusterContainer> > elClusters {clusterLink};
        egammaRec* egRec = new egammaRec(); 
        egRec->setCaloClusters( elClusters );
        m_eg_container->push_back( egRec );
    } // End attaching clusters to egammaRec

    ATH_MSG_DEBUG("m_eg_container->size: "<< m_eg_container->size() );

    //
    // Conversions
    //
    // Need method which takes egRec and VxContainer
    if(m_doConversions){
        ATH_MSG_DEBUG("doConversions=true");
        ATH_MSG_DEBUG("pVxContainer->size: "<< pVxContainer->size() );
        ATH_MSG_DEBUG("REGTEST:: Run Conversion Builder for egContainer");
        if (timerSvc()) m_timerTool2->start(); //timer
        for(auto egRec : *m_eg_container) {
            ATH_MSG_DEBUG( "REGTEST:: Running ConversionBuilder");
            if(m_conversionBuilder->hltExecute(egRec,pVxContainer).isFailure())
                ATH_MSG_DEBUG("REGTEST: no Conversion");
        }
        if (timerSvc()) m_timerTool2->stop(); //timer
    } //m_doConversions/

    // Check for Track Match.

    if(m_doTrackMatching){
        ATH_MSG_DEBUG("doTrackMatching=true");
        ATH_MSG_DEBUG("pTrackParticleContainer->size: "<< pTrackParticleContainer->size() );
        for(auto egRec : *m_eg_container) {
            if (timerSvc()) m_timerTool1->start(); //timer

            ATH_MSG_DEBUG( "REGTEST:: Running TrackMatchBuilder" );

            if(m_trackMatchBuilder->trackExecute(egRec,pTrackParticleContainer).isFailure()) {
                ATH_MSG_DEBUG( "REGTEST: no Track matched to this cluster" );
            }

            if (timerSvc()) m_timerTool1->stop(); //timer
        } // End track match building
        // calculate track to calorimeter extrapolation
        // need for electron supercluster builder
        //Save extrapolated perigee to calo (eta,phi)
        static const SG::AuxElement::Decorator<float> pgExtrapEta ("perigeeExtrapEta");
        static const SG::AuxElement::Decorator<float> pgExtrapPhi ("perigeeExtrapPhi");

        for(auto egRec : *m_eg_container) {
            const xAOD::TrackParticle* trP = egRec->trackParticle();
            if (trP) {
                float extrapEta(-999.), extrapPhi(-999.);
                bool hitEM2 = m_extrapolationTool->getEtaPhiAtCalo( &(trP->perigeeParameters()), &extrapEta, &extrapPhi);
                if (hitEM2) {
                    ATH_MSG_DEBUG("Extrapolated eta="<< extrapEta << ", phi=" << extrapPhi);
                } else {
                    ATH_MSG_WARNING("Extrapolation to EM2 failed!");
                }
                pgExtrapEta(*trP) = extrapEta;
                pgExtrapPhi(*trP) = extrapPhi;
            }
        } // End track extrapolation

    } //m_trackMatchBuilderToExec

    ATH_MSG_DEBUG("Check egammaRecs before call to superclusters builders" );
    if(msgLvl() <= MSG::DEBUG) {
        for (std::size_t i = 0 ; i < m_eg_container->size();++i) {
            auto egRec=m_eg_container->at(i);
            double emFrac(-12345.);
            const xAOD::CaloCluster *eg_clus = egRec->caloCluster();
            ATH_MSG_DEBUG("  > i="<< i <<", eg_clus=" << eg_clus );
            if (!eg_clus->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,emFrac)){
                ATH_MSG_WARNING("NO ENG_FRAC_EM moment available in eg_clus" );
            } else {
                ATH_MSG_DEBUG("ENG_FRAC_EM="<< emFrac );
            }
            ATH_MSG_DEBUG("Number of associated tracks " << egRec->getNumberOfTrackParticles() );
        }
    }
    ATH_MSG_DEBUG("Check egammaRecs completed" );

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create SuperClusters
  ////////////////////////////////////////////////


  //Now all info should be added  the initial topoClusters build SuperClusters
  //Electron superclusters Builder

  // prepare output objects for SuperCluster Builder
    //Create new EgammaRecContainer
    ATH_MSG_DEBUG("Prepare to create electron superclusters" );
    EgammaRecContainer *electronSuperRecs = new EgammaRecContainer();
    ATH_MSG_DEBUG("electronSuper EgammaRecContainer created" );

    // Electrons
    xAOD::CaloClusterContainer* electron_CSCContainer = new xAOD::CaloClusterContainer();
    xAOD::CaloClusterAuxContainer electron_CSCAux;
    electron_CSCContainer->setStore(&electron_CSCAux);

    ATH_MSG_DEBUG("Before call of electron superclusters builder" << m_electronSCBuilder);
    TRIG_CHECK_SC(m_electronSCBuilder->hltExecute(m_eg_container,
                                                            electronSuperRecs,
                                                            electron_CSCContainer));


  //Track Match the final electron SuperClusters
    ATH_MSG_DEBUG("Size of electronSuperClusterRecContainer: " << electronSuperRecs->size());

    if (m_doTrackMatching){
      for (auto egRec : *electronSuperRecs) {
        if (m_trackMatchBuilder->trackExecute(egRec, pTrackParticleContainer).isFailure()){
          ATH_MSG_ERROR("Problem executing TrackMatchBuilder");
          return HLT::ERROR;
        }
      }
    }


  //Photon superclusters Builder
  // prepare output objects for SuperCluster Builder
    //Create new EgammaRecContainer
    ATH_MSG_DEBUG("Prepare to create photon superclusters" );
    EgammaRecContainer *photonSuperRecs = new EgammaRecContainer();
    ATH_MSG_DEBUG("photonSuper EgammaRecContainer created" );

    // Photons
    xAOD::CaloClusterContainer* photon_CSCContainer = new xAOD::CaloClusterContainer();
    xAOD::CaloClusterAuxContainer photon_CSCAux;
    photon_CSCContainer->setStore(&photon_CSCAux);


    ATH_MSG_DEBUG("Before call of photon superclusters builder" << m_photonSCBuilder);
    TRIG_CHECK_SC(m_photonSCBuilder->hltExecute(m_eg_container,
                                                          photonSuperRecs,
                                                          photon_CSCContainer));


    ATH_MSG_DEBUG("Size of photonSuperClusterRecContainer: " << photonSuperRecs->size());

    //Redo conversion matching given the super cluster
    if (m_doConversions) {
      for (auto egRec : *photonSuperRecs) {
        if (m_conversionBuilder->hltExecute(egRec, pVxContainer).isFailure()){
          ATH_MSG_ERROR("Problem executing conversioBuilder on photonSuperRecs");
          return HLT::ERROR;
        }
      }
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  ATH_MSG_DEBUG("Build  "<< electronSuperRecs->size() << " electron Super Clusters");
  ATH_MSG_DEBUG("Build  "<< photonSuperRecs->size() << " photon Super Clusters");

  //
  //For now naive double loops bases on the seed (constituent at position 0)
  //For ambiguity. Probably we could mark in one pass , for now naive solution
  //These should be the CaloCalTopo links for the clustes. the 0 should be the seed (always there)
  static const SG::AuxElement::Accessor < std::vector< ElementLink< xAOD::CaloClusterContainer > > > caloClusterLinks("constituentClusterLinks");
  ATH_MSG_DEBUG("constituentClusterLinks AuxId=" << caloClusterLinks.auxid() );

  //
  //Look at the constituents , for ambiguity resolution, for now based on the seed only
  //We could add secondaries cluster in this logic.
  //Also probably we could factor some common code.
  //-----------------------------------------------------------------
  ATH_MSG_DEBUG("Build xAOD::Electron objects");

  //Build xAOD::Electron objects
  for (const auto& electronRec : *electronSuperRecs) {

    unsigned int author = xAOD::EgammaParameters::AuthorElectron;
    xAOD::AmbiguityTool::AmbiguityType type= xAOD::AmbiguityTool::electron;

    ATH_MSG_DEBUG("build el: Aux for electronRec isAvailable=" << caloClusterLinks.isAvailable( *(electronRec->caloCluster()) ) );

    const std::vector< ElementLink< xAOD::CaloClusterContainer > > &elLinks = caloClusterLinks( *(electronRec->caloCluster()) );
    ATH_MSG_DEBUG( "build el: elLinks.size()=" << elLinks.size() );

    for (const auto& photonRec : *photonSuperRecs) {

      //See if the same seed (0 element in the constituents) seed also a photon
      ATH_MSG_DEBUG("build el: Aux for photonRec isAvailable=" << caloClusterLinks.isAvailable( *(photonRec->caloCluster()) ) );

      const std::vector< ElementLink< xAOD::CaloClusterContainer > > &phLinks = caloClusterLinks( *(photonRec->caloCluster()) );
      ATH_MSG_DEBUG( "build el: phLinks.size()=" << phLinks.size() );

      ATH_MSG_DEBUG("See if the same seed (0 element in the constituents) seed also a photon");
      if( elLinks.at(0) == phLinks.at(0) ) {
	ATH_MSG_DEBUG("Running AmbiguityTool for electron");

        ATH_MSG_DEBUG("REGTEST:: Running AmbiguityTool");
        if (timerSvc()) m_timerTool3->start(); //timer
	author = m_ambiguityTool->ambiguityResolve(electronRec->caloCluster(),
						   photonRec->vertex(),
						   electronRec->trackParticle(),
						   type);

        if (timerSvc()) m_timerTool3->stop(); //timer
        ATH_MSG_DEBUG("REGTEST:: AmbiguityTool Author " << author);

	break;
      }
    }
    //Fill each electron
    if (author == xAOD::EgammaParameters::AuthorElectron ||
	author == xAOD::EgammaParameters::AuthorAmbiguous){
      ATH_MSG_DEBUG("getElectron");
      if ( !getElectron(electronRec, m_electron_container, author,type) ){
	return HLT::ERROR;
      }
      ATH_MSG_DEBUG("getElectron success");
    }
  }

  //-----------------------------------------------------------------

  ATH_MSG_DEBUG("Build xAOD::Photon objects");
  //Build xAOD::Photon objects.
  for (const auto& photonRec : *photonSuperRecs) {
    unsigned int author = xAOD::EgammaParameters::AuthorPhoton;
    xAOD::AmbiguityTool::AmbiguityType type= xAOD::AmbiguityTool::photon;

      ATH_MSG_DEBUG("build ph: Aux for photonRec isAvailable=" << caloClusterLinks.isAvailable( *(photonRec->caloCluster()) ) );

      const std::vector< ElementLink< xAOD::CaloClusterContainer > > &phLinks = caloClusterLinks( *(photonRec->caloCluster()) );
      ATH_MSG_DEBUG( "build ph: phLinks.size()=" << phLinks.size() );

    //See if the same seed (0 element in the constituents) seed also an electron
    for (const auto& electronRec : *electronSuperRecs) {

      ATH_MSG_DEBUG("build ph: Aux for electronRec isAvailable=" << caloClusterLinks.isAvailable( *(electronRec->caloCluster()) ) );

      const std::vector< ElementLink< xAOD::CaloClusterContainer > > &elLinks = caloClusterLinks( *(electronRec->caloCluster()) );
      ATH_MSG_DEBUG( "build ph: elLinks.size()=" << elLinks.size() );

      ATH_MSG_DEBUG("See if the same seed (0 element in the constituents) seed also an electron");
      if( phLinks.at(0) == elLinks.at(0) ) {
        ATH_MSG_DEBUG("Running AmbiguityTool for photon");
        ATH_MSG_DEBUG("REGTEST:: Running AmbiguityTool");
        if (timerSvc()) m_timerTool3->start(); //timer

      author = m_ambiguityTool->ambiguityResolve(electronRec->caloCluster(),
						 photonRec->vertex(),
						 electronRec->trackParticle(),
						 type);
        if (timerSvc()) m_timerTool3->stop(); //timer
        ATH_MSG_DEBUG("REGTEST:: AmbiguityTool Author " << author);
      break;
    }
    }
    //Fill each photon
    if (author == xAOD::EgammaParameters::AuthorPhoton ||
	author == xAOD::EgammaParameters::AuthorAmbiguous){
      ATH_MSG_DEBUG("getPhoton");
      if ( !getPhoton(photonRec, m_photon_container, author,type) ){
	return HLT::ERROR;
      }
      ATH_MSG_DEBUG("getPhoton success");
    }
  }

// -----------------------------------------------------------------------------------------------
    //Dress the Electron objects
    ATH_MSG_DEBUG("Dress the Electron objects");
    for (const auto& eg : *m_electron_container){
        // EMFourMomentum
        if (timerSvc()) m_timerTool4->start(); //timer
        ATH_MSG_DEBUG("about to run EMFourMomBuilder::hltExecute(eg)");
        if(m_fourMomBuilder->hltExecute(eg)); // 
        else if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: problem with fourmom tool" << endreq; 
        if (timerSvc()) m_timerTool4->stop(); //timer

        // Shower Shape
        // Track Isolation tool no longer runs for trigger in showershape, but we can do ourselves
        if (timerSvc()) m_timerTool5->start(); //timer
        ATH_MSG_DEBUG("about to run EMShowerBuilder::recoExecute(eg,pCaloCellContainer)");
        if( m_showerBuilder->recoExecute(eg,pCaloCellContainer) );
        else if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: no shower built for this cluster" << endreq;
        if (timerSvc()) m_timerTool5->stop(); //timer
        
        // Isolation
        //
        // Calo Isolation types
        
        if(m_doCaloCellIsolation || m_doTopoIsolation){       
            if (timerSvc()) m_timerIsoTool2->start(); //timer
            std::map<std::string,CaloIsoHelp>::iterator itc = m_egCaloIso.begin(), itcE = m_egCaloIso.end();
            for (; itc != itcE; itc++) {
                CaloIsoHelp isoH = itc->second;
                std::string flav = itc->first;
                bool bsc = false;
                bool tbsc = false;
                if (flav == "etcone" && pCaloCellContainer){
                    bsc = m_caloCellIsolationTool->decorateParticle_caloCellIso(*eg, isoH.help.isoTypes, isoH.CorrList, pCaloCellContainer);
                }
                else if (flav == "topoetcone" && topoClusTrue){
                    // Add check for topoclusters (when available);
                    tbsc = m_topoIsolationTool->decorateParticle_topoClusterIso(*eg, isoH.help.isoTypes, isoH.CorrList, pTopoClusterContainer);
                }
                if (!bsc && m_doCaloCellIsolation && flav=="etcone") {
                    ATH_MSG_WARNING("Call to CaloIsolationTool failed for flavour " << flav);
                }
                if (!tbsc && m_doTopoIsolation && flav =="topoetcone") {
                    ATH_MSG_WARNING("Call to CaloTopoIsolationTool failed for flavour " << flav);
                }
            }
            ATH_MSG_DEBUG(" REGTEST: etcone20   =  " << getIsolation_etcone20(eg));
            ATH_MSG_DEBUG(" REGTEST: etcone30   =  " << getIsolation_etcone30(eg));
            ATH_MSG_DEBUG(" REGTEST: etcone40   =  " << getIsolation_etcone40(eg));
            ATH_MSG_DEBUG(" REGTEST: topoetcone20   =  " << getIsolation_topoetcone20(eg));
            ATH_MSG_DEBUG(" REGTEST: topoetcone30   =  " << getIsolation_topoetcone30(eg));
            ATH_MSG_DEBUG(" REGTEST: topoetcone40   =  " << getIsolation_topoetcone40(eg));
            if (timerSvc()) m_timerIsoTool2->stop(); //timer
        }
        if(m_doTrackIsolation){
            ATH_MSG_DEBUG("Running TrackIsolationTool for Electrons");

            if (timerSvc()) m_timerIsoTool1->start(); //timer
            if(m_egTrackIso.size() != 0) {
                // Track Isolation types
                std::map<std::string,TrackIsoHelp>::iterator itt = m_egTrackIso.begin(), ittE = m_egTrackIso.end();
                for (; itt != ittE; itt++) {
                    TrackIsoHelp isoH = itt->second;
                    std::string flav  = itt->first;
                    const std::set<const xAOD::TrackParticle*> tracksToExclude = xAOD::EgammaHelpers::getTrackParticles(eg, m_useBremAssoc); // For GSF this may need to be property
                    // Need the decorate methods from IsolationTool
                    bool bsc = m_trackIsolationTool->decorateParticle(*eg, isoH.help.isoTypes, isoH.CorrList, &leadTrkVtx, &tracksToExclude,pTrackParticleContainer);
                    if (!bsc) 
                        ATH_MSG_WARNING("Call to TrackIsolationTool failed for flavour " << flav);
                }
                ATH_MSG_DEBUG(" REGTEST: ptcone20   =  " << getIsolation_ptcone20(eg));
                ATH_MSG_DEBUG(" REGTEST: ptcone30   =  " << getIsolation_ptcone30(eg));
                ATH_MSG_DEBUG(" REGTEST: ptcone40   =  " << getIsolation_ptcone40(eg));

            }
            if (timerSvc()) m_timerIsoTool1->stop(); //timer       
        }
     
        // PID
        ATH_MSG_DEBUG("about to run execute(eg) for PID");
        if (timerSvc()) m_timerPIDTool1->start(); //timer
        if( m_electronPIDBuilder->execute(eg)){
            ATH_MSG_DEBUG("Computed PID and dressed");
            m_lhval.push_back(eg->likelihoodValue("LHValue"));
        }
        else ATH_MSG_DEBUG("Problem in electron PID");
        if (timerSvc()) m_timerPIDTool1->stop(); //timer
        // PID
        ATH_MSG_DEBUG("about to run execute(eg) for Calo LH PID");
        if (timerSvc()) m_timerPIDTool2->start(); //timer
        if( m_electronCaloPIDBuilder->execute(eg)){
            ATH_MSG_DEBUG("Computed PID and dressed");
            m_lhcaloval.push_back(eg->likelihoodValue("LHCaloValue"));
        }
        else ATH_MSG_DEBUG("Problem in electron PID");
        if (timerSvc()) m_timerPIDTool2->stop(); //timer
        ATH_MSG_DEBUG(" REGTEST: xAOD Reconstruction Electron variables: ");
        if(msgLvl() <= MSG::DEBUG) PrintElectron(eg);
    }

    //Dress the Photon objects
    ATH_MSG_DEBUG("Dress the Photon objects");
    for (const auto& eg : *m_photon_container){
        // EMFourMomentum
        if (timerSvc()) m_timerTool4->start(); //timer
        ATH_MSG_DEBUG("about to run EMFourMomBuilder::hltExecute(eg)");
        if(m_fourMomBuilder->hltExecute(eg)); // 
        else if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: problem with fourmom tool" << endreq; 
        if (timerSvc()) m_timerTool4->stop(); //timer

        // Shower Shape
        if (timerSvc()) m_timerTool5->start(); //timer
        ATH_MSG_DEBUG("about to run EMShowerBuilderrecoExecute(eg,pCaloCellContainer)");
        if( m_showerBuilder->recoExecute(eg,pCaloCellContainer) );
        else if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: no shower built for this cluster" << endreq;
        if (timerSvc()) m_timerTool5->stop(); //timer
        
        // Isolation
        
        if(m_doCaloCellIsolation || m_doTopoIsolation){       
            if (timerSvc()) m_timerIsoTool2->start(); //timer
            std::map<std::string,CaloIsoHelp>::iterator itc = m_egCaloIso.begin(), itcE = m_egCaloIso.end();
            for (; itc != itcE; itc++) {
                CaloIsoHelp isoH = itc->second;
                std::string flav = itc->first;
                bool bsc = false;
                bool tbsc = false;
                if (flav == "etcone" && pCaloCellContainer)
                    bsc = m_caloCellIsolationTool->decorateParticle_caloCellIso(*eg, isoH.help.isoTypes, isoH.CorrList, pCaloCellContainer);
                else if (flav == "topoetcone" && topoClusTrue)
                    // Add check for topoclusters (when available);
                    tbsc = m_topoIsolationTool->decorateParticle_topoClusterIso(*eg, isoH.help.isoTypes, isoH.CorrList, pTopoClusterContainer);
                if (!bsc && m_doCaloCellIsolation && flav=="etcone") 
                    ATH_MSG_WARNING("Call to CaloIsolationTool failed for flavour " << flav);
                if (!tbsc && m_doTopoIsolation && flav =="topoetcone") 
                    ATH_MSG_WARNING("Call to CaloTopoIsolationTool failed for flavour " << flav);
            }
            ATH_MSG_DEBUG(" REGTEST: etcone20   =  " << getIsolation_etcone20(eg));
            ATH_MSG_DEBUG(" REGTEST: etcone30   =  " << getIsolation_etcone30(eg));
            ATH_MSG_DEBUG(" REGTEST: etcone40   =  " << getIsolation_etcone40(eg));
            ATH_MSG_DEBUG(" REGTEST: topoetcone20   =  " << getIsolation_topoetcone20(eg));
            ATH_MSG_DEBUG(" REGTEST: topoetcone30   =  " << getIsolation_topoetcone30(eg));
            ATH_MSG_DEBUG(" REGTEST: topoetcone40   =  " << getIsolation_topoetcone40(eg));

            if (timerSvc()) m_timerIsoTool2->stop(); //timer
        }
    
        // Particle ID
        if (timerSvc()) m_timerPIDTool3->start(); //timer
        if( m_photonPIDBuilder->execute(eg)){
            ATH_MSG_DEBUG("Computed PID and dressed");
        }
        else ATH_MSG_DEBUG("Problem in photon PID");
        if (timerSvc()) m_timerPIDTool3->stop(); //timer
        ATH_MSG_DEBUG(" REGTEST: xAOD Reconstruction Photon variables: ");
        if(msgLvl() <= MSG::DEBUG) PrintPhoton(eg);
    }

    // attach electron container to the TE
    if (HLT::OK != attachFeature( outputTE, m_electron_container, m_electronContainerName) ){
        ATH_MSG_ERROR("REGTEST: trigger xAOD::ElectronContainer attach to TE and record into StoreGate failed");
        return HLT::NAV_ERROR;
    } 
    else{
        ATH_MSG_DEBUG("REGTEST: xAOD::ElectronContainer created and attached to TE: " << m_electronContainerName); 
    }

    // attach photon container to the TE
    if (HLT::OK != attachFeature( outputTE, m_photon_container, m_photonContainerName) ){
        ATH_MSG_ERROR("REGTEST: trigger xAOD::PhotonContainer attach to TE and record into StoreGate failed");
        return HLT::NAV_ERROR;
    } 
    else{
        ATH_MSG_DEBUG("REGTEST: xAOD::PhotonContainer created and attached to TE: " << m_photonContainerName); 
    }

    ATH_MSG_DEBUG("HLTAlgo Execution of xAOD TrigTopoEgammaBuilder completed successfully");
    // Time total TrigTopoEgammaBuilder execution time.
    if (timerSvc()) m_timerTotal->stop();

    return HLT::OK;
}


// =====================================================
bool TrigTopoEgammaBuilder::getElectron(const egammaRec* egRec,
				    xAOD::ElectronContainer *electronContainer,
				    const unsigned int author,
				    const uint8_t type) const {

    if (!egRec || !electronContainer) return false;

    xAOD::Electron *electron = new xAOD::Electron();
    electronContainer->push_back(electron);
    electron->setAuthor(author);
    static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
    acc(*electron)=type;

    std::vector< ElementLink< xAOD::CaloClusterContainer > > clusterLinks;
    for (size_t i = 0 ; i < egRec->getNumberOfClusters(); ++i){
        clusterLinks.push_back( egRec->caloClusterElementLink(i) );
    }
    electron->setCaloClusterLinks( clusterLinks );

    std::vector< ElementLink< xAOD::TrackParticleContainer > > trackLinks;
    for (size_t i = 0 ; i < egRec->getNumberOfTrackParticles(); ++i){
        trackLinks.push_back( egRec->trackParticleElementLink(i) );
    }
    electron->setTrackParticleLinks( trackLinks );

    electron->setCharge(electron->trackParticle()->charge());
    //Set DeltaEta, DeltaPhi , DeltaPhiRescaled
    float deltaEta = static_cast<float>(egRec->deltaEta(0));
    float deltaPhi = static_cast<float>(egRec->deltaPhi(0));
    float deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(0));
    electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta0 );
    electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi0 );
    electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled0 );

    deltaEta = static_cast<float>(egRec->deltaEta(1));
    deltaPhi = static_cast<float>(egRec->deltaPhi(1));
    deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(1));
    electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta1 );
    electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi1 );
    electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled1);

    deltaEta = static_cast<float>(egRec->deltaEta(2));
    deltaPhi = static_cast<float>(egRec->deltaPhi(2));
    deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(2));
    electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta2 );
    electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi2 );
    electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled2);

    deltaEta = static_cast<float>(egRec->deltaEta(3));
    deltaPhi = static_cast<float>(egRec->deltaPhi(3));
    deltaPhiRescaled = static_cast<float>(egRec->deltaPhiRescaled(3));
    electron->setTrackCaloMatchValue(deltaEta,xAOD::EgammaParameters::deltaEta3 );
    electron->setTrackCaloMatchValue(deltaPhi,xAOD::EgammaParameters::deltaPhi3 );
    electron->setTrackCaloMatchValue(deltaPhiRescaled,xAOD::EgammaParameters::deltaPhiRescaled3);

    float deltaPhiLast = static_cast<float>(egRec->deltaPhiLast ());
    electron->setTrackCaloMatchValue(deltaPhiLast,xAOD::EgammaParameters::deltaPhiFromLastMeasurement );

    return true;
}

// =====================================================
bool TrigTopoEgammaBuilder::getPhoton(const egammaRec* egRec,
				  xAOD::PhotonContainer *photonContainer,
				  const unsigned int author,
				  const uint8_t type) const {
    if (!egRec || !photonContainer) return false;

    xAOD::Photon *photon = new xAOD::Photon();
    photonContainer->push_back(photon);
    photon->setAuthor(author);

    static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
    acc(*photon)=type;

    // Transfer the links to the clusters
    std::vector< ElementLink< xAOD::CaloClusterContainer > > clusterLinks;
    for (size_t i = 0 ; i < egRec->getNumberOfClusters(); ++i){
        clusterLinks.push_back( egRec->caloClusterElementLink(i) );
    }
    photon->setCaloClusterLinks( clusterLinks );

    // Transfer the links to the vertices
    std::vector< ElementLink< xAOD::VertexContainer > > vertexLinks;
    for (size_t i = 0 ; i < egRec->getNumberOfVertices(); ++i){
        vertexLinks.push_back( egRec->vertexElementLink(i) );
    }
    photon->setVertexLinks( vertexLinks );

    // Transfer deltaEta/Phi info
    float deltaEta = egRec->deltaEtaVtx(), deltaPhi = egRec->deltaPhiVtx();
    if (!photon->setVertexCaloMatchValue( deltaEta, xAOD::EgammaParameters::convMatchDeltaEta1) )
    {
        ATH_MSG_WARNING("Could not transfer deltaEta to photon");
        return true;
    }
    if (!photon->setVertexCaloMatchValue( deltaPhi,xAOD::EgammaParameters::convMatchDeltaPhi1) )
    {
        ATH_MSG_WARNING("Could not transfer deltaEta to photon");
        return true;
    }

    return true;
}

/** @brief Decoration debug method for electrons */
void TrigTopoEgammaBuilder::PrintElectron(xAOD::Electron *eg) const {
    // This will return exception if string not correct
    // Safe method to pass value to fill
    unsigned int isEMbit=0;
    ATH_MSG_DEBUG("isEMVLoose " << eg->selectionisEM(isEMbit,"isEMVLoose"));
    ATH_MSG_DEBUG("isEMVLoose bit " << std::hex << isEMbit); 
    ATH_MSG_DEBUG("isEMLoose " << eg->selectionisEM(isEMbit,"isEMLoose"));
    ATH_MSG_DEBUG("isEMLoose bit " << std::hex << isEMbit); 
    ATH_MSG_DEBUG("isEMMedium " << eg->selectionisEM(isEMbit,"isEMMedium"));
    ATH_MSG_DEBUG("isEMMedium bit " << std::hex << isEMbit); 
    ATH_MSG_DEBUG("isEMTight " << eg->selectionisEM(isEMbit,"isEMTight"));
    ATH_MSG_DEBUG("isEMTight bit " << std::hex << isEMbit); 

    ATH_MSG_DEBUG("LHValue " << eg->likelihoodValue("LHValue"));
    ATH_MSG_DEBUG("LHCaloValue " << eg->likelihoodValue("LHCaloValue"));

    ATH_MSG_DEBUG("LHVLoose " << eg->passSelection("LHVLoose"));
    ATH_MSG_DEBUG("LHLoose " << eg->passSelection("LHLoose"));
    ATH_MSG_DEBUG("LHMedium " << eg->passSelection("LHMedium"));
    ATH_MSG_DEBUG("LHTight " << eg->passSelection("LHTight"));

    ATH_MSG_DEBUG("LHCaloVLoose " << eg->passSelection("LHCaloVLoose"));
    ATH_MSG_DEBUG("LHCaloLoose " << eg->passSelection("LHCaloLoose"));
    ATH_MSG_DEBUG("LHCaloMedium " << eg->passSelection("LHCaloMedium"));
    ATH_MSG_DEBUG("LHCaloTight " << eg->passSelection("LHCaloTight"));
    
    ATH_MSG_DEBUG("isEMLHVLoose " << eg->selectionisEM(isEMbit,"isEMLHVLoose"));
    ATH_MSG_DEBUG("isEMLHVLoose bit " << std::hex << isEMbit); 
    ATH_MSG_DEBUG("isEMLHLoose " << eg->selectionisEM(isEMbit,"isEMLHLoose"));
    ATH_MSG_DEBUG("isEMLHLoose bit " << std::hex << isEMbit); 
    ATH_MSG_DEBUG("isEMLHMedium " << eg->selectionisEM(isEMbit,"isEMLHMedium"));
    ATH_MSG_DEBUG("isEMLHMedium bit " << std::hex << isEMbit); 
    ATH_MSG_DEBUG("isEMLHTight " << eg->selectionisEM(isEMbit,"isEMLHTight"));
    ATH_MSG_DEBUG("isEMLHTight bit " << std::hex << isEMbit); 
    float val_float=-99;
    //DEBUG output for Egamma container
    
    //Cluster and ShowerShape info
    //REGTEST printout
    if (eg) {
        msg() << MSG::DEBUG << " REGTEST: electron energy: " << eg->e() << endreq;
        msg() << MSG::DEBUG << " REGTEST: electron eta: " << eg->eta() << endreq;
        msg() << MSG::DEBUG << " REGTEST: electron phi: " << eg->phi() << endreq;
        ATH_MSG_DEBUG(" REGTEST: electron charge " << eg->charge());
    } else{
        msg() << MSG::DEBUG << " REGTEST: problems with electron pointer" << endreq;
    }

    ATH_MSG_DEBUG(" REGTEST: cluster variables");
    if (eg->caloCluster()) {
        msg() << MSG::DEBUG << " REGTEST: electron cluster transverse energy: " << eg->caloCluster()->et() << endreq;
        msg() << MSG::DEBUG << " REGTEST: electron cluster eta: " << eg->caloCluster()->eta() << endreq;
        msg() << MSG::DEBUG << " REGTEST: electron cluster phi: " << eg->caloCluster()->phi() << endreq;
        double tmpeta = -999.;
        double tmpphi = -999.;
        eg->caloCluster()->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta);
        eg->caloCluster()->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,tmpphi); 
        ATH_MSG_DEBUG(" REGTEST: electron Calo-frame coords. etaCalo = " << tmpeta); 
        ATH_MSG_DEBUG(" REGTEST: electron Calo-frame coords. phiCalo = " << tmpphi);
    } else{
        msg() << MSG::DEBUG << " REGTEST: problems with electron cluster pointer" << endreq;
    }

    ATH_MSG_DEBUG(" REGTEST: EMShower variables");
    ATH_MSG_DEBUG(" REGTEST: ethad    =  " << getShowerShape_ethad(eg));
    ATH_MSG_DEBUG(" REGTEST: e011     =  " << getShowerShape_e011(eg));
    ATH_MSG_DEBUG(" REGTEST: e132     =  " << getShowerShape_e132(eg)); 
    ATH_MSG_DEBUG(" REGTEST: etcone20   =  " << getIsolation_etcone20(eg));
    ATH_MSG_DEBUG(" REGTEST: etcone30   =  " << getIsolation_etcone30(eg));
    ATH_MSG_DEBUG(" REGTEST: etcone40   =  " << getIsolation_etcone40(eg));
    ATH_MSG_DEBUG(" REGTEST: ptcone20   =  " << getIsolation_ptcone20(eg));
    ATH_MSG_DEBUG(" REGTEST: ptcone30   =  " << getIsolation_ptcone30(eg));
    ATH_MSG_DEBUG(" REGTEST: ptcone40   =  " << getIsolation_ptcone40(eg));
    eg->showerShapeValue(val_float,xAOD::EgammaParameters::e237); 
    ATH_MSG_DEBUG(" REGTEST: e237     =  " << val_float);
    eg->showerShapeValue(val_float,xAOD::EgammaParameters::e335); 
    ATH_MSG_DEBUG(" REGTEST: e335     =  " << val_float);
    eg->showerShapeValue(val_float,xAOD::EgammaParameters::e2ts1); 
    ATH_MSG_DEBUG(" REGTEST: e2ts1    =  " << val_float);
    eg->showerShapeValue(val_float,xAOD::EgammaParameters::e2tsts1); 
    ATH_MSG_DEBUG(" REGTEST: e2tsts1  =  " << val_float);

    //DEBUG info for Electrons which by definition have a track match
    ATH_MSG_DEBUG(" REGTEST: trackmatch variables");

    if(eg->trackParticle()){
        msg() << MSG::DEBUG << " REGTEST: pt=  " << eg->trackParticle()->pt() << endreq;
        msg() << MSG::DEBUG << " REGTEST: charge=  " << eg->trackParticle()->charge() << endreq;
        msg() << MSG::DEBUG << " REGTEST: E/p=  " << eg->caloCluster()->et() / eg->trackParticle()->pt() << endreq;
        eg->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaEta1);
        msg() << MSG::DEBUG << " REGTEST: Delta eta 1st sampling=  " << val_float << endreq;
        eg->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaPhi2);
        msg() << MSG::DEBUG << " REGTEST: Delta phi 2nd sampling=  " << val_float << endreq;
    } else{
        msg() << MSG::DEBUG << " REGTEST: no electron eg->trackParticle() pointer" << endreq; 
    }
}

/** @brief Decoration debug method for photons */
void TrigTopoEgammaBuilder::PrintPhoton(xAOD::Photon *eg) const {
    // This will return exception if string not correct
    // Safe method to pass value to fill
    unsigned int isEMbit=0;
    //ATH_MSG_DEBUG("isEMLoose " << eg->selectionisEM(isEMbit,"isEMLoose"));
    //ATH_MSG_DEBUG("isEMLoose " << std::hex << isEMbit); 
    //ATH_MSG_DEBUG("isEMMedium " << eg->selectionisEM(isEMbit,"isEMMedium"));
    //ATH_MSG_DEBUG("isEMMedium " << std::hex << isEMbit); 
    ATH_MSG_DEBUG("isEMTight " << eg->selectionisEM(isEMbit,"isEMTight"));
    ATH_MSG_DEBUG("isEMTight bit " << std::hex << isEMbit); 
    float val_float=-99;
    //DEBUG output for Egamma container
    msg() << MSG::DEBUG << " REGTEST: xAOD Reconstruction Photon variables: " << endreq; 
    //Cluster and ShowerShape info
    //REGTEST printout
    if (eg) {
        msg() << MSG::DEBUG << " REGTEST: photon energy: " << eg->e() << endreq;
        msg() << MSG::DEBUG << " REGTEST: photon eta: " << eg->eta() << endreq;
        msg() << MSG::DEBUG << " REGTEST: photon phi: " << eg->phi() << endreq;
    } else{
        msg() << MSG::DEBUG << " REGTEST: problems with photon pointer" << endreq;
    }

    ATH_MSG_DEBUG(" REGTEST: cluster variables");
    if (eg->caloCluster()) {
        msg() << MSG::DEBUG << " REGTEST: photon cluster transverse energy: " << eg->caloCluster()->et() << endreq;
        msg() << MSG::DEBUG << " REGTEST: photon cluster eta: " << eg->caloCluster()->eta() << endreq;
        msg() << MSG::DEBUG << " REGTEST: photon cluster phi: " << eg->caloCluster()->phi() << endreq;
        double tmpeta = -999.;
        double tmpphi = -999.;
        eg->caloCluster()->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta);
        eg->caloCluster()->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,tmpphi); 
        ATH_MSG_DEBUG(" REGTEST: photon Calo-frame coords. etaCalo = " << tmpeta); 
        ATH_MSG_DEBUG(" REGTEST: photon Calo-frame coords. phiCalo = " << tmpphi); 
    } else{
        msg() << MSG::DEBUG << " REGTEST: problems with photon cluster pointer" << endreq;
    }

    ATH_MSG_DEBUG(" REGTEST: EMShower variables");
    ATH_MSG_DEBUG(" REGTEST: ethad    =  " << getShowerShape_ethad(eg));
    ATH_MSG_DEBUG(" REGTEST: e011     =  " << getShowerShape_e011(eg));
    ATH_MSG_DEBUG(" REGTEST: e132     =  " << getShowerShape_e132(eg)); 
    ATH_MSG_DEBUG(" REGTEST: etcone20   =  " << getIsolation_etcone20(eg));
    ATH_MSG_DEBUG(" REGTEST: etcone30   =  " << getIsolation_etcone30(eg));
    ATH_MSG_DEBUG(" REGTEST: etcone40   =  " << getIsolation_etcone40(eg));
    ATH_MSG_DEBUG(" REGTEST: ptcone20   =  " << getIsolation_ptcone20(eg));
    ATH_MSG_DEBUG(" REGTEST: ptcone30   =  " << getIsolation_ptcone30(eg));
    ATH_MSG_DEBUG(" REGTEST: ptcone40   =  " << getIsolation_ptcone40(eg));
    eg->showerShapeValue(val_float,xAOD::EgammaParameters::e237); 
    ATH_MSG_DEBUG(" REGTEST: e237     =  " << val_float);
    eg->showerShapeValue(val_float,xAOD::EgammaParameters::e335); 
    ATH_MSG_DEBUG(" REGTEST: e335     =  " << val_float);
    eg->showerShapeValue(val_float,xAOD::EgammaParameters::e2ts1); 
    ATH_MSG_DEBUG(" REGTEST: e2ts1    =  " << val_float);
    eg->showerShapeValue(val_float,xAOD::EgammaParameters::e2tsts1); 
    ATH_MSG_DEBUG(" REGTEST: e2tsts1  =  " << val_float);

    //DEBUG info for Electrons which by definition have a track match
    ATH_MSG_DEBUG(" REGTEST: trackmatch variables");

}

// Taken from IsolationBuilder to set the decoration -- should not need to do this ourselves
std::pair<std::string,std::string> TrigTopoEgammaBuilder::decodeEnum(int isoT) {
    std::string baseN     = "";

    xAOD::Iso::IsolationType Type          = static_cast<xAOD::Iso::IsolationType>(isoT);
    xAOD::Iso::IsolationFlavour isoFlav    = xAOD::Iso::isolationFlavour(Type);
    int is = 100*xAOD::Iso::coneSize(Type);
    if (isoFlav == xAOD::Iso::IsolationFlavour::neflowisol)
        baseN = "neflowisol";
    else if (isoFlav == xAOD::Iso::IsolationFlavour::etcone)
        baseN = "etcone";
    else if (isoFlav == xAOD::Iso::IsolationFlavour::topoetcone)
        baseN = "topoetcone";
    else if (isoFlav == xAOD::Iso::IsolationFlavour::ptcone)
        baseN = "ptcone";
    else if (isoFlav == xAOD::Iso::IsolationFlavour::ptvarcone)
        baseN = "ptvarcone";
    else
        ATH_MSG_WARNING("This isolation flavour is not dealt with this algorithm ! Check your inputs");
    std::string aN = baseN; aN += std::to_string(is);

    return make_pair(baseN,aN);
}
