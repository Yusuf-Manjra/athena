/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// CaloIsolationTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "CaloIsolationTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloEvent/CaloCell.h"
#include "TrackToCalo/CaloCellSelectorLayerdR.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkCaloExtension/CaloExtension.h"

#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "xAODPrimitives/IsolationCorrection.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include "xAODPrimitives/IsolationCorrectionHelper.h"
#include "xAODPrimitives/tools/getIsolationAccessor.h"
#include "xAODPrimitives/tools/getIsolationCorrectionAccessor.h"

#include "xAODEgamma/Egamma.h"
#include "xAODMuon/Muon.h"
#include "FourMomUtils/P4Helpers.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "CaloUtils/CaloCellList.h"
#include "CaloClusterCorrection/CaloFillRectangularCluster.h"

#include "IsolationCorrections/IIsolationCorrectionTool.h"

#include "boost/foreach.hpp"
#include "boost/format.hpp"
#include <cmath> 
#include <map> 


namespace xAOD {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  CaloIsolationTool::CaloIsolationTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),

        m_assoTool("Rec::ParticleCaloCellAssociationTool/ParticleCaloCellAssociationTool"),
        m_caloExtTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
	m_clustersInConeTool("xAOD::CaloClustersInConeTool/CaloClustersInConeTool"),
        m_pflowObjectsInConeTool(""),
        m_caloFillRectangularTool("")
  {
    declareInterface<ICaloCellIsolationTool>(this);
    declareInterface<ICaloTopoClusterIsolationTool>(this);
    declareInterface<INeutralEFlowIsolationTool>(this);
    declareProperty("ParticleCaloCellAssociationTool", m_assoTool);
    declareProperty("ParticleCaloExtensionTool",       m_caloExtTool);      

    // Topo Isolation parameters
    declareProperty("ClustersInConeTool",             m_clustersInConeTool);
    declareProperty("PFlowObjectsInConeTool",         m_pflowObjectsInConeTool);
    declareProperty("UseEMScale",                     m_useEMScale = true, "Use TopoClusters at the EM scale.");
    declareProperty("CaloFillRectangularClusterTool", m_caloFillRectangularTool, "Handle of the CaloFillRectangularClusterTool");
    // Handle of the calorimetric isolation tool
    declareProperty("IsoLeakCorrectionTool",          m_IsoLeakCorrectionTool,         "Handle on the leakage correction tool");
    declareProperty("doEnergyDensityCorrection",      m_doEnergyDensityCorrection    = true, "Correct isolation variables based on energy density estimations");
    declareProperty("saveOnlyRequestedCorrections",   m_saveOnlyRequestedCorrections = false, "save only requested corrections (trigger usage mainly)");
    // list of calo to treat
    declareProperty("EMCaloNums",  m_EMCaloNums,  "list of EM calo to treat");
    declareProperty("HadCaloNums", m_HadCaloNums, "list of Had calo to treat");

    // Choose whether TileGap3 cells are excluded 
    declareProperty("ExcludeTG3", m_ExcludeTG3 = true, "Exclude the TileGap3 cells");
  }

  CaloIsolationTool::~CaloIsolationTool() { }
  
  StatusCode CaloIsolationTool::initialize() {
    
    if (!m_assoTool.empty())
      ATH_CHECK(m_assoTool.retrieve());

    if (!m_caloExtTool.empty())
      ATH_CHECK(m_caloExtTool.retrieve());
   
    if (!m_clustersInConeTool.empty())
      ATH_CHECK(m_clustersInConeTool.retrieve());

    if (!m_pflowObjectsInConeTool.empty())
      ATH_CHECK(m_pflowObjectsInConeTool.retrieve());

    if (!m_caloFillRectangularTool.empty()) {
      ATH_CHECK( m_caloFillRectangularTool.retrieve());
      m_caloFillRectangularToolPtr=dynamic_cast<CaloFillRectangularCluster*>(&(*m_caloFillRectangularTool));
      if (!m_caloFillRectangularToolPtr) {
	ATH_MSG_WARNING("Could not retrieve CaloFillRectangularCluster");      
      }
    }

    if (!m_IsoLeakCorrectionTool.empty())
      ATH_CHECK(m_IsoLeakCorrectionTool.retrieve());

    //check calo number specified for EM Calos
    unsigned int nSubCalo=static_cast<int>(CaloCell_ID::NSUBCALO);
    if (m_EMCaloNums.size()>nSubCalo || m_HadCaloNums.size()>nSubCalo) {
      ATH_MSG_ERROR(" More than " << nSubCalo << " calo specified. Required for EM =" << m_EMCaloNums.size()<< ", and for HCAL = " << m_HadCaloNums.size() << ". Must be wrong. Stop.");
      return StatusCode::FAILURE;
    }
    
    for (unsigned int index=0; index < m_EMCaloNums.size() ; ++index)  {
      if (m_EMCaloNums[index]<0 || m_EMCaloNums[index]>=(int)nSubCalo ) {
        ATH_MSG_ERROR("Invalid calo specification:" << m_EMCaloNums[index] << "Stop.");
        return StatusCode::FAILURE;
      } else 
      	ATH_MSG_DEBUG("Select calorimeter " << m_EMCaloNums[index]);
    }
        
    for (unsigned int index=0; index < m_HadCaloNums.size() ; ++index) {
      if (m_HadCaloNums[index]<0 || m_HadCaloNums[index]>=(int)nSubCalo) {
        ATH_MSG_ERROR("Invalid calo specification:" << m_HadCaloNums[index] << "Stop.");
        return StatusCode::FAILURE;
      } else 
      	ATH_MSG_DEBUG("Select calorimeter " << m_HadCaloNums[index]);
    }
    
    // Exit function
    return StatusCode::SUCCESS;
  }


  StatusCode CaloIsolationTool::finalize() {
    return StatusCode::SUCCESS;
  }


  // IParticle interface for cell-based isolation (etcone)
  bool CaloIsolationTool::caloCellIsolation(CaloIsolation& result, const IParticle& particle, 
					    const std::vector<Iso::IsolationType>& cones, 
					    CaloCorrection corrlist, 
					    const CaloCellContainer* container ) {

    /// get track particle
    const IParticle* ip = getReferenceParticle(particle);
    if( !ip ){
      ATH_MSG_WARNING("Failed to obtain reference particle");
      return false;
    }
    m_derefMap.clear();
    m_derefMap[ip] = &particle;

    // muon etcone isolation
    const TrackParticle* trkp = dynamic_cast<const TrackParticle*>(ip);
    if( trkp ) return caloCellIsolation(result,*trkp,cones,corrlist,container);
    
    // egamma etcone isolation
    const Egamma* egam = dynamic_cast<const Egamma*>(ip);
    if( egam ) return caloCellIsolation(result,*egam,cones,corrlist,container);
    
    ATH_MSG_WARNING("CaloCellIsolation only supported for TrackParticles and Egamma");
    
    return true;
  }
  
  // IParticle interface for cluster-based isolation (topoetcone)
  bool CaloIsolationTool::caloTopoClusterIsolation(CaloIsolation& result, const IParticle& particle, 
						   const std::vector<Iso::IsolationType>& cones, 
						   CaloCorrection corrlist, 
						   const CaloClusterContainer* container  ) {
    
    if ( cones.size() == 0 ) {
      ATH_MSG_DEBUG("No isolation required");
      return false;
    }

    /// get track particle
    const IParticle* ip = getReferenceParticle(particle);
    if( !ip ){
      ATH_MSG_WARNING("Failed to obtain reference particle");
      return false;
    }
    m_derefMap.clear();
    m_derefMap[ip] = &particle;

    // muon topoetcone isolation
    const TrackParticle* trkp = dynamic_cast<const TrackParticle*>(ip);
    if( trkp ) return caloTopoClusterIsolation(result,*trkp,cones,corrlist,container);
    
    // egamma topoetcone isolation
    const Egamma* egam = dynamic_cast<const Egamma*>(ip);
    if( egam ) return caloTopoClusterIsolation(result,*egam,cones,corrlist,container);
    
    ATH_MSG_WARNING("CaloTopoClusterIsolation only supported for TrackParticles and Egamma");
    
    return true;
  }
  

  // interface for pflow based isolation
  bool CaloIsolationTool::neutralEflowIsolation(CaloIsolation& result, const IParticle& particle, 
						const std::vector<Iso::IsolationType>& cones, 
						CaloCorrection corrlist) {
    
    /// get track particle
    const IParticle* ip = getReferenceParticle(particle);
    if( !ip ){
      ATH_MSG_WARNING("Failed to obtain reference particle");
      return false;
    }
    m_derefMap.clear();
    m_derefMap[ip] = &particle;

    // muon pflowetcone isolation
    const TrackParticle* trkp = dynamic_cast<const TrackParticle*>(ip);
    if ( trkp ) return neutralEflowIsolation(result,*trkp,cones,corrlist);

    // egamma pflowetcone isolation
    const Egamma* egam = dynamic_cast<const Egamma*>(ip);
    if ( egam ) return neutralEflowIsolation(result,*egam,cones,corrlist);
    
    ATH_MSG_WARNING("PFlowObjectIsolation only supported for Egamma and TrackParticle");

    return true;
  }
  
  
  // casted interface for TrackParticle cell-based isolation (etcone)
  bool CaloIsolationTool::caloCellIsolation( CaloIsolation& result, const TrackParticle& tp, 
					     const std::vector<Iso::IsolationType>& isoTypes, 
					     CaloCorrection corrlist, 
					     const CaloCellContainer* container ) {
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }

    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);
    
    // get cones
    Iso::IsolationFlavour theFlavour = Iso::isolationFlavour(isoTypes.front());
    
    if (theFlavour == Iso::etcone)     
      return etConeIsolation(result, tp, isoTypes, container);
    
    ATH_MSG_WARNING("Unsupported isolation flavour passed, cannot calculate isolation " << 
		    static_cast<int>(theFlavour));
    
    return false;
  }
  
  // casted interface for EGamma cell-based isolation (etcone)
  bool CaloIsolationTool::caloCellIsolation( CaloIsolation& result, const Egamma& eg, 
					     const std::vector<Iso::IsolationType>& isoTypes, 
					     CaloCorrection corrlist, 
					     const CaloCellContainer* container ) {
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }

    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);
    
    std::vector<float> coneSizes;
    coneSizes.resize(isoTypes.size());
    for (unsigned int is = 0; is < isoTypes.size(); is++)
      coneSizes[is] = Iso::coneSize(isoTypes[is]);

    // get cones
    Iso::IsolationFlavour theFlavour = Iso::isolationFlavour(isoTypes.front());
    
    if (theFlavour == Iso::etcone){
      etConeIsolation(result, eg, isoTypes, container);
    }else{
      ATH_MSG_WARNING("Unsupported isolation flavour passed, cannot calculate isolation " << static_cast<int>(theFlavour));
      return false;
    }

    // Apply corrections
    // core energy subtraction
    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::core57cells))) {
      if (!correctIsolationEnergy_Eeg57(result,isoTypes,&eg))
	ATH_MSG_WARNING("Could not compute core cell energy for egamma in etcone");
    }

    // leakage correction
    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::ptCorrection))) {
      if (!m_IsoLeakCorrectionTool.empty()) {
	if (!PtCorrection(result, eg, isoTypes)) 
	  ATH_MSG_WARNING("Could not apply pt correction to etcone isolation");
      }
    }
    
    return true;
  }
  
  // casted interface for EGamma cluster-based isolation (topoetcone)
  bool CaloIsolationTool::caloTopoClusterIsolation(CaloIsolation& result, 
						   const Egamma& eg, 
						   const std::vector<Iso::IsolationType>& isoTypes, 
						   CaloCorrection corrlist, 
						   const CaloClusterContainer* container)
  {
    
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }
    
    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);
    
    std::vector<float> coneSizes;
    for( auto isoType : isoTypes ){
      coneSizes.push_back(Iso::coneSize(isoType));
    }
    
    float phi = eg.caloCluster()->phi();
    float eta = eg.caloCluster()->eta();
    
    if (!topoConeIsolation(result, eta, phi, coneSizes, true, container)) {
      ATH_MSG_WARNING("topoConeIsolation failed for egamma");
      return false;
    }
    
    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::core57cells))) {
      // Apply core energy subtraction
      if (!correctIsolationEnergy_Eeg57(result,isoTypes,&eg)) 
	ATH_MSG_WARNING("Could not compute core cell energy for egamma in topoetcone");
    }
    
    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::ptCorrection))) {
      // do pt correction
      if (!m_IsoLeakCorrectionTool.empty()) {
	if (!PtCorrection(result, eg, isoTypes))
	  ATH_MSG_WARNING("Could not apply pt correction to topoetcone isolation");
      }
    }
    
    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::pileupCorrection))) {
      // do pile-up correction
      if (m_doEnergyDensityCorrection && !EDCorrection(result,isoTypes,eta))
	ATH_MSG_WARNING("Could not apply ED correction to topo isolation");
    }
    
    return true;
    
  }
  
  // casted interface for EGamma pflow-based isolation 
  bool CaloIsolationTool::neutralEflowIsolation(CaloIsolation& result, 
						const Egamma& eg, 
						const std::vector<Iso::IsolationType>& isoTypes, 
						CaloCorrection corrlist)
  {
    
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }
    
    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);
    
    std::vector<float> coneSizes;
    for( auto isoType : isoTypes ){
      coneSizes.push_back(Iso::coneSize(isoType));
    }
    
    float phi = eg.caloCluster()->phi();
    float eta = eg.caloCluster()->eta();
    
    if (!pflowConeIsolation(result, eta, phi, coneSizes, true)) {
      ATH_MSG_WARNING("pflowConeIsolation failed");
      return false;
    }

    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::core57cells))) {
      // Apply core energy subtraction
      if (!correctIsolationEnergy_Eeg57(result,isoTypes,&eg)) 
	ATH_MSG_WARNING("Could not compute core cell energy for egamma in neflowisol");
    }

    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::ptCorrection))) {
      // do pt correction
      if (!m_IsoLeakCorrectionTool.empty()) {
	if (!PtCorrection(result, eg, isoTypes))
	  ATH_MSG_WARNING("Could not apply pt correction to isolation");
      }
    }

    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::pileupCorrection))) {
      // do pile-up correction
      std::string type = "PFlow";
      if (m_doEnergyDensityCorrection && !EDCorrection(result,isoTypes,eta,type))
	ATH_MSG_WARNING("Could not apply ED correction to topo isolation");
    }
    
    return true;
  }
  
  // casted interface for TP pflow-based isolation 
  bool CaloIsolationTool::neutralEflowIsolation(CaloIsolation& result, 
						const TrackParticle& tp, 
						const std::vector<Iso::IsolationType>& isoTypes, 
						CaloCorrection corrlist)
  {
    
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }

    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);

    std::vector<float> coneSizes;
    for( auto isoType : isoTypes ){
      coneSizes.push_back(Iso::coneSize(isoType));
    }

    float phi = tp.phi();
    float eta = tp.eta();
    GetExtrapEtaPhi(&tp,eta,phi);
    ATH_MSG_DEBUG("TrackParticle eta = " << tp.eta() << ", phi = " << tp.phi() << ", extrap eta = " << eta << ", phi = " << phi);  

    // The core subtraction with pflow removal is done in the method below
    if (!pflowConeIsolation(result, eta, phi, coneSizes, false)) {
      ATH_MSG_WARNING("pflowConeIsolation failed for muon");
      return false;
    }

    // core energy subtraction
    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreMuon))) {
      if (!correctIsolationEnergy_MuonCore(result, tp)) 
	ATH_MSG_WARNING("Could not compute muon core energy (cells) from neflowisol");
    }

    // do pile-up correction
    std::string type = "PFlow";
    if (m_doEnergyDensityCorrection && !EDCorrection(result,isoTypes,eta,type)) 
      ATH_MSG_WARNING("Could not apply ED correction to eflow isolation for muon");
    
    return true;
  }
  
  bool CaloIsolationTool::GetExtrapEtaPhi(const TrackParticle* tp, float& eta, float& phi)
  {
    const Trk::CaloExtension* caloExtension = 0;
    if(!m_caloExtTool->caloExtension(*tp,caloExtension)){
      ATH_MSG_WARNING("Can not get caloExtension.");
      return false;
    };

    const std::vector<const Trk::CurvilinearParameters*>& intersections = caloExtension->caloLayerIntersections();
    if (intersections.size()>0) {
      Amg::Vector3D avePoint(0,0,0);
      for (unsigned int i = 0; i < intersections.size(); ++i){
        const Amg::Vector3D& point = intersections[i]->position();
        ATH_MSG_DEBUG("Intersection: " << i << " ID: " << parsIdHelper.caloSample(intersections[i]->cIdentifier()) 
		      <<  " eta-phi (" << point.eta() << ", " << point.phi() << ")");
        avePoint += point;
      }
      avePoint = (1./intersections.size())*avePoint;
      eta = avePoint.eta();
      phi = avePoint.phi();
    }
    return true;
  }

  // casted interface for TrackParticle cluster-based isolation (topoetcone)
  bool CaloIsolationTool::caloTopoClusterIsolation(CaloIsolation& result,  
						   const TrackParticle& tp,
						   const std::vector<Iso::IsolationType>& isoTypes, 
						   CaloCorrection corrlist, 
						   const CaloClusterContainer* container){
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }

    unsigned int typesize = isoTypes.size();
    initresult(result, corrlist, typesize);

    std::vector<float> coneSizes;
    for( auto isoType : isoTypes ){
      coneSizes.push_back(Iso::coneSize(isoType));
    }

    float phi = tp.phi();
    float eta = tp.eta();
    GetExtrapEtaPhi(&tp,eta,phi);
    ATH_MSG_DEBUG("TrackParticle eta = " << tp.eta() << ", phi = " << tp.phi() << ", extrap eta = " << eta << ", phi = " << phi);  
    
    // get energy in cones
    if (!topoConeIsolation(result, eta, phi, coneSizes, false, container)) {
      ATH_MSG_WARNING("Could not compute topo isolation for muons");
      return false;
    }
    
    // core energy subtraction
    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreMuon))) {
      if (!correctIsolationEnergy_MuonCore(result, tp)) 
	ATH_MSG_WARNING("Could not compute muon core energy (cells) from topoetcone");
    }

    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::pileupCorrection))) {
      if (m_doEnergyDensityCorrection && !EDCorrection(result,isoTypes,eta))
	ATH_MSG_WARNING("Could not apply ED correction to topo isolation");
    }

    return true;
  }

  // etcone implementation for TrackParticle
  bool CaloIsolationTool::etConeIsolation( CaloIsolation& result, const TrackParticle& tp,
					   const std::vector<Iso::IsolationType>& isoTypes, 
					   const CaloCellContainer* container ) {

    // for now always use track, should move to extrapolation to calo entrance 
    const Trk::Track* track = *tp.trackLink();
    if( !track  ) {
      ATH_MSG_WARNING("Failed to access track");
      return false;
    }

    std::vector<double> conesf;
    double maxConeSize = -1;
    for( auto isoType : isoTypes ){
        if( Iso::isolationFlavour(isoType) != Iso::etcone ) {
        ATH_MSG_WARNING("Unsupported isolation type passed, cannot calculate isolation " << static_cast<int>(isoType));
        return false;
      }
      double cone = Iso::coneSize(isoType);
      conesf.push_back(cone);
      if(cone>maxConeSize) maxConeSize = cone;
    }

    /// add coreCone if asked -- make sure it's the last one! or a better
    const double coreConeDR = 0.1;
    bool doCoreCone = (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone)));
    if(doCoreCone && maxConeSize<coreConeDR) maxConeSize = coreConeDR;

    /// if noEM option is on, start from HEC0
//     bool noEM = false;
//     const CaloSampling::CaloSample fistTile = CaloSampling::HEC0;

    /// start the calculation 
    ATH_MSG_DEBUG("calculating etcone for # " << conesf.size() << " cones");
    const Rec::ParticleCellAssociation* association = 0;
    if( !m_assoTool->particleCellAssociation(tp,association,maxConeSize,container) ){
      ATH_MSG_DEBUG("failed to obtain the ParticleCellAssociation");
      return false;
    }

    Trk::CaloCellSelectorLayerdR selector(maxConeSize);
    selector.preSelectAction(association->caloExtension());
    ATH_MSG_DEBUG("looping over cells " << association->data().size());
    for(unsigned int i=0; i<conesf.size(); i++){
      double totE = 0.;
      selector.setConeSize(conesf[i]);
      for (auto aCell : association->data()){
        if( !selector.select(*aCell) ) continue;
        if (m_ExcludeTG3 && CaloCell_ID::TileGap3 == aCell->caloDDE()->getSampling()) continue;
//         if(noEM && aCell->caloDDE()->getSampling()<fistTile) continue;
        totE += aCell->energy();
      }
      result.etcones[i] = totE;
      ATH_MSG_DEBUG("etcone raw: coneSize = " << conesf[i] << "; etcone = " << result.etcones[i]);
    }
    ATH_MSG_DEBUG("done looping over cells ");

    /// do coreCone
    if(doCoreCone){
      ATH_MSG_DEBUG("starting etcone, coreCone");
      double totE = 0.;
      selector.setConeSize(coreConeDR);
      for (auto aCell : association->data()){
        if( !selector.select(*aCell) ) continue;
        if (m_ExcludeTG3 && CaloCell_ID::TileGap3 == aCell->caloDDE()->getSampling()) continue;
//         if(noEM && aCell->caloDDE()->getSampling()<fistTile) continue;
        totE += aCell->energy();
      }
      std::map<Iso::IsolationCorrectionParameter,float> corecorr;
      corecorr[Iso::coreEnergy] = totE;
      corecorr[Iso::coreArea]   = coreConeDR*coreConeDR*M_PI;
      result.coreCorrections[Iso::coreCone] = corecorr;
      ATH_MSG_DEBUG("done etcone, coreCone");

      /// apply the correction if required.
      if(result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone))){
        double ecore = totE;
        ATH_MSG_DEBUG("Applying coreCone correction for trackParticle etcone isolation.");
        for( unsigned int i=0;i<result.etcones.size();++i ) {
          result.etcones[i] -= ecore;
          ATH_MSG_DEBUG("i: " << i << " cone [before] " << result.etcones[i]+ecore << " cone [after] " << result.etcones[i]);
        }
      }
    }

    // calculate etcore 
    if(!m_saveOnlyRequestedCorrections ||
       result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreMuon))){
      correctIsolationEnergy_MuonCore(result, tp);
    }
    
    return true;
  }

  // etcone implementation for Egamma
  bool CaloIsolationTool::etConeIsolation( CaloIsolation& result, const Egamma& eg,
					   const std::vector<Iso::IsolationType>& isoTypes, 
					   const CaloCellContainer* container ) {
    if( isoTypes.empty() ) {
      ATH_MSG_WARNING("Empty list passed, failing calculation");
      return false;
    }
    
    std::vector<float> coneSizes;        coneSizes.resize(3);
    std::vector<float> coneSizesSquared; coneSizesSquared.resize(3);
    for (unsigned int i = 0; i < isoTypes.size(); i++) {
      float dR = Iso::coneSize(isoTypes.at(i));
      coneSizes[i] = dR;
      coneSizesSquared[i] = dR*dR;
    }

    float phi = eg.caloCluster()->phi();
    float eta = eg.caloCluster()->eta();
        
    // Define a new Calo Cell list corresponding to EM Calo
    //CaloCell_ID::SUBCALO EMCaloCellID = CaloCell_ID::SUBCALO::LAREM;
    std::vector<CaloCell_ID::SUBCALO> Vec_EMCaloEnums;
    for (unsigned int n=0; n < m_EMCaloNums.size(); ++n)
      Vec_EMCaloEnums.push_back(static_cast<CaloCell_ID::SUBCALO>( m_EMCaloNums[n] ));
    CaloCellList* EMccl = new CaloCellList(container, Vec_EMCaloEnums);
   
    std::vector<CaloCell_ID::SUBCALO> Vec_HadCaloEnums;
    for (unsigned int n=0; n < m_HadCaloNums.size(); ++n) 
      Vec_HadCaloEnums.push_back(static_cast<CaloCell_ID::SUBCALO>( m_HadCaloNums[n] ));
    CaloCellList* HADccl = new CaloCellList(container, Vec_HadCaloEnums);
    
    // Let's determine some values based on the input specs
    // Search for largest radius
    double Rmax = 0.0;
    for (unsigned int n=0; n< coneSizes.size(); n++)
      if (coneSizes[n] > Rmax) Rmax = coneSizes[n];
        
    if (EMccl) {
      // get the cells for the first one; by convention, it must be bigger than all the other cones.
      EMccl->select(eta,phi,Rmax);

      for (CaloCellList::list_iterator it = EMccl->begin(); it != EMccl->end(); ++it) {
	double etacel=(*it)->eta();
	double phicel=(*it)->phi();
        
	double deleta = eta-etacel;
	double delphi = P4Helpers::deltaPhi(phi, phicel);
	double drcel2 = (deleta*deleta) + (delphi*delphi);
        
	for (unsigned int i = 0; i < coneSizes.size(); i++) {
	  if (drcel2 < coneSizesSquared[i])
	    result.etcones[i] += (*it)->et();
	}
      }
    }
    
    if (HADccl) {
      // get the cells for the first one; by convention, it must be bigger than all the other cones.
      HADccl->select(eta, phi, Rmax);

      for (CaloCellList::list_iterator it = HADccl->begin(); it != HADccl->end(); ++it) {
	// Optionally remove TileGap cells
	if (m_ExcludeTG3 && CaloCell_ID::TileGap3 == (*it)->caloDDE()->getSampling())
	{
	  ATH_MSG_DEBUG("Excluding cell with Et = " << (*it)->et());
	  continue;
	}
	
	// if no TileGap cells excluded, log energy of all cells
	double etacel = (*it)->eta();
	double phicel = (*it)->phi();
        
	double deleta = eta-etacel;
	double delphi = P4Helpers::deltaPhi(phi, phicel);
	double drcel2 = (deleta*deleta) + (delphi*delphi);

	for (unsigned int i = 0; i < coneSizes.size(); i++) {
	  if (drcel2 < coneSizesSquared[i]) 
	    result.etcones[i] += (*it)->et();
	}
      }
    }

    delete EMccl;
    delete HADccl;
    return true;
  }
  
  bool CaloIsolationTool::topoConeIsolation(CaloIsolation& result, float eta, float phi, 
					    std::vector<float>& coneSizes, 
                                            bool coreEMonly,
					    const CaloClusterContainer* container)
  {

    // offline topocluster container is large: preselect only those in max cone size
    auto max_cone_iter=std::max_element(coneSizes.begin(), coneSizes.end());
    float max_cone= (*max_cone_iter);
    std::vector<const CaloCluster*> clusts;    
    if(!container){ 
      if(m_clustersInConeTool){
	m_clustersInConeTool->particlesInCone(eta,phi,max_cone,clusts);
      }else{
	ATH_MSG_WARNING("No CaloClustersInConeTool available");      
      }
    }else{ // trigger container is small enough
      auto clItr  = container->begin();
      auto clItrE = container->end();
      for(; clItr != clItrE; ++clItr){
	clusts.push_back (*clItr);
      }
    }

    // Calculate isolation for topo cluster
    if (!topoClustCones (result,eta,phi, coneSizes, clusts)) {
      ATH_MSG_WARNING("Could not compute topo isolation");
      return false;
    }

    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone))) {
      // Subtract core (easier to do that here than outside like other core corrections)
      if (!correctIsolationEnergy_TopoCore(result, eta, phi, -1, -1, 0.01, clusts, coreEMonly)) 
	ATH_MSG_WARNING("Could not compute topo core");
    }

    return true;
  }
  
  bool CaloIsolationTool::pflowConeIsolation(CaloIsolation& result, float eta, float phi, 
					     std::vector<float>& coneSizes,
                                             bool coreEMonly,
					     const PFOContainer* container)
  {

    // container is large: preselect only those in max cone size
    auto max_cone_iter=std::max_element(coneSizes.begin(), coneSizes.end());
    float max_cone = (*max_cone_iter);
    std::vector<const PFO*> clusts;    
    if (!container) { 
      if (m_pflowObjectsInConeTool) {
	m_pflowObjectsInConeTool->particlesInCone(eta,phi,max_cone,clusts);
      } else {
	ATH_MSG_WARNING("No PFlowObjectsInConeTool available");      
      }
    } else { // trigger container is small enough
      auto clItr  = container->begin();
      auto clItrE = container->end();
      for(; clItr != clItrE; ++clItr){
	clusts.push_back (*clItr);
      }
    }
    
    // Calculate isolation for pflow objects
    if (!pflowObjCones (result,eta,phi,coneSizes,clusts)) {
      ATH_MSG_WARNING("Could not compute pflow isolation");
      return false;     
    }

    if (!m_saveOnlyRequestedCorrections || result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone))) {
      // Core subtraction
      //if (!correctIsolationEnergy_pflowCore(result, eta, phi, -1, -1, 0.01, clusts))
      if (!correctIsolationEnergy_pflowCore(result, eta, phi, -1, -1, 0.01, clusts, coreEMonly))  // be careful, require a certain tag of eflowRec, below which eSample are not always filled
	ATH_MSG_WARNING("Could not compure pflow core");
    }

    return true;
  }

  /**
   * @brief Calculate isolation cones in topo clusters around @a eg.
   */
  bool CaloIsolationTool::topoClustCones (CaloIsolation& result, float eta, float phi, 
					  std::vector<float>& coneSizes,
					  const std::vector<const CaloCluster*>& clusts) 
  {
    
    ATH_MSG_DEBUG("In CaloIsolationTool::topoClustCones");
    
    BOOST_FOREACH (const CaloCluster* cl, clusts) {
      float et = (m_useEMScale ? cl->p4(CaloCluster::State::UNCALIBRATED).Et() : cl->pt() );
      if(et <= 0 || fabs(cl->eta()) > 7.0) continue;

      float st = 1./cosh(cl->p4(CaloCluster::State::UNCALIBRATED).Eta());
      float tilegap3_et = cl->eSample(CaloSampling::TileGap3)*st;
      et -= tilegap3_et;
      
      float dPhi = P4Helpers::deltaPhi( cl->phi(), phi);
      float dEta = cl->eta()-eta;
      float dr=sqrt(dPhi*dPhi+ dEta*dEta);
      
      for (unsigned int i = 0; i < coneSizes.size(); i++) {
	if (dr < coneSizes[i]) {
	  result.etcones[i] += et;
	}
      }
    }
    
    return true;
  }

  /*
  // This method is never called !
  bool CaloIsolationTool::topoClustCones (CaloIsolation& result, float eta, float phi, 
					  std::vector<float> coneSizes,
					  const std::vector<const CaloCluster*>& clusts,
                                          float dEtaMin, float dPhiMin, float dR2Min, bool onlyEM) 
  {
    
    ATH_MSG_DEBUG("In topoClustCones::topoClustCones [exclude core]");
    
    std::vector<float> coneSizes2(coneSizes.size());
    for (unsigned int i = 0; i < coneSizes.size(); i++) {coneSizes2[i]=coneSizes[i]*coneSizes[i];}

    float ecore = 0;

    ATH_MSG_DEBUG("In topoClustCones:: clusts size " << clusts.size());
    BOOST_FOREACH (const CaloCluster* cl, clusts) {
      float et = (m_useEMScale ? cl->p4(CaloCluster::State::UNCALIBRATED).Et() : cl->pt() );
      if(et <= 0 || fabs(cl->eta()) > 7.0) continue;

      float st = 1./cosh(cl->p4(CaloCluster::State::UNCALIBRATED).Eta());
      float tilegap3_et = cl->eSample(CaloSampling::TileGap3)*st;
      et -= tilegap3_et;
      ATH_MSG_DEBUG("In topoClustCones:: et = " << et);
      
      float dPhi = P4Helpers::deltaPhi( cl->phi(), phi);
      float dEta = cl->eta()-eta;
      float dr2  = dPhi*dPhi + dEta*dEta;
      if ( ( dPhiMin > 0 && dEtaMin > 0 && fabs(dPhi) < dPhiMin && fabs(dEta) < dEtaMin ) ||
	   ( dR2Min > 0 && dr2 < dR2Min ) ) {
	double emfrac = 1.;
	if(onlyEM) {
	  if (!m_useEMScale) {
	    if (!cl->retrieveMoment(CaloCluster::ENG_FRAC_EM, emfrac)) 
	      ATH_MSG_WARNING("CaloCluster::ENG_FRAC_EM is not retrieved.");
	    else {
	      double eEM = cl->energyBE(0)+cl->energyBE(1)+cl->energyBE(2)+cl->energyBE(3);
	      emfrac = std::min(1., eEM / cl->p4(CaloCluster::State::UNCALIBRATED).E());
	    }
	  }
	}
	ecore += emfrac*et;
	continue;
      }

      for (unsigned int i = 0; i < coneSizes2.size(); i++) {
	if (dr2 < coneSizes2[i]) result.etcones[i] += et;
        ATH_MSG_DEBUG("In topoClustCones:: result.etcones[" << i << "] = " << result.etcones[i]);
      }
    }

    float areacore = -999.;
    if (dEtaMin > 0 && dPhiMin > 0) areacore = 4*dEtaMin*dPhiMin;
    else if (dR2Min > 0)            areacore = M_PI*dR2Min;
    std::map<Iso::IsolationCorrectionParameter,float> corecorr;
    corecorr[Iso::coreEnergy] = ecore;
    corecorr[Iso::coreArea]   = areacore;
    result.coreCorrections[Iso::coreCone] = corecorr;
    
    return true;
  }
  */
  
  /**
   * @brief Calculate isolation cones in pflow objects around @a eg.
   */
  bool CaloIsolationTool::pflowObjCones (CaloIsolation& result, float eta, float phi, 
					 std::vector<float>& coneSizes,
					 const std::vector<const PFO*>& clusts) 
  {
    
    ATH_MSG_DEBUG("In pflowObjCones obj eta = " << eta << " phi = " << phi);
    
    BOOST_FOREACH (const PFO* cl, clusts) {
      float et = cl->pt();
      if (m_useEMScale)
	et = cl->ptEM();
      if (et <= 0 || fabs(cl->eta()) > 7.0) continue;
      
      float dPhi = P4Helpers::deltaPhi(cl->phi(), phi);
      float dEta = cl->eta()-eta;
      float dR   = sqrt(dPhi*dPhi+ dEta*dEta);
      
      for (unsigned int i = 0; i < coneSizes.size(); i++) {
	if (dR < coneSizes[i]) {
	  result.etcones[i] += et;
	}
      }
    }
    
    return true;
  }


  bool CaloIsolationTool::correctIsolationEnergy_Eeg57 (CaloIsolation& result, 
							const std::vector<Iso::IsolationType>& isoTypes, 
							const Egamma* eg) 
  {

    float coreV = 0;
    bool gotIso = eg->isolationCaloCorrection(coreV,Iso::etcone, Iso::core57cells, Iso::coreEnergy);
    if (gotIso) 
      ATH_MSG_DEBUG("core57cells available = " << coreV);
    else
      ATH_MSG_DEBUG("core57cells not available");
    if ((gotIso && fabs(coreV) < 1e-3) || !gotIso) { 
      const CaloCluster *cleg = eg->caloCluster();
      // now correct the isolation energy for the core cluster energy
      float eraw57=0., eta=0.;
      if(cleg && cleg->getCellLinks()){
	CaloCluster* egcCloneFor57 = CaloClusterStoreHelper::makeCluster(cleg->getCellLinks()->getCellContainer(),
									 cleg->eta0(),cleg->phi0(),
									 cleg->clusterSize()
									 );
	if (!m_caloFillRectangularToolPtr) {
	  ATH_MSG_WARNING("CaloFillRectangularCluster is not available");
	  return false;
	}
	m_caloFillRectangularToolPtr->makeCorrection(egcCloneFor57);
	eraw57 = egcCloneFor57->e();
	eta    = cleg->eta(); //FillRectangularCluster doesn't recalculated the overall cluster eta (only per-sampling)
	coreV  = eraw57/cosh(eta);
	ATH_MSG_DEBUG("Number of cells in 5x7 " << egcCloneFor57->size() 
		      << " seed eta,phi " << cleg->eta0() << " " << cleg->phi0()
		      << " eraw = " << eraw57 << " etraw = " << coreV
		      );
	delete egcCloneFor57;
      }
    }

    std::map<Iso::IsolationCorrectionParameter,float> corecorr;
    corecorr[Iso::coreEnergy] = coreV;
    corecorr[Iso::coreArea]   = 5*0.025*7*TMath::Pi()/128;
    result.coreCorrections[Iso::core57cells] = corecorr;
    if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::core57cells))) {
      for (unsigned int i = 0; i < isoTypes.size(); i++){
	result.etcones[i] -= coreV;
      }
    }

    return true;
  }
  

  bool CaloIsolationTool::correctIsolationEnergy_TopoCore(CaloIsolation& result, float eta, float phi, 
					  float dEtaMax_core, float dPhiMax_core, float dR2Max_core,
					  const std::vector<const CaloCluster*>& clusts, bool onlyEM) 
  {
    ATH_MSG_DEBUG("In CaloIsolationTool::correctIsolationEnergy_TopoCore");
    ATH_MSG_DEBUG("particle: eta " << eta << " phi " << phi);

    float topoCore(0.);
    BOOST_FOREACH (const CaloCluster* cl, clusts) {
      ATH_MSG_DEBUG("cl: eta " << cl->eta() << " phi " << cl->phi() 
 		    << " E " << cl->p4(CaloCluster::State::UNCALIBRATED).E() 
 		    << " pt " << cl->p4(CaloCluster::State::UNCALIBRATED).Et() 
 		    << " cal E " << cl->e());
      /// check distance
      float dPhi = P4Helpers::deltaPhi(cl->phi(), phi);
      if(dEtaMax_core>0 && fabs(dPhi) > dPhiMax_core) continue;
      float dEta = cl->eta()-eta;
      if(dPhiMax_core>0 && fabs(dEta) > dEtaMax_core) continue;
      if(dR2Max_core>0 && dPhi*dPhi+dEta*dEta > dR2Max_core) continue;
      ATH_MSG_DEBUG("dist: dPhi " << dPhi << " dEta " << dEta << " dR2 " << dPhi*dPhi+dEta*dEta);
 
      /// get enenrgy
      float et = (m_useEMScale ? cl->p4(CaloCluster::State::UNCALIBRATED).Et() : cl->pt() );
      if(et <= 0 || fabs(cl->eta()) > 7.0) continue;

      /// remove TileGap3
      double ettg3 = cl->eSample(CaloSampling::TileGap3)/cosh(cl->p4(CaloCluster::State::UNCALIBRATED).Eta());
      et -= ettg3;
      if (fabs(ettg3) > 1) 
	ATH_MSG_DEBUG("After TG3 removal, pt = " << et);

      /// if only EM
      double emfrac = 1.;
      if(onlyEM){
	double eEM = cl->energyBE(0)+cl->energyBE(1)+cl->energyBE(2)+cl->energyBE(3);
	emfrac     = std::min(1., eEM / cl->p4(CaloCluster::State::UNCALIBRATED).E());
      }
      et *= emfrac;

      /// add to the core
      topoCore += et;
      ATH_MSG_DEBUG("adding in core et: " << et << " (em frac = " << emfrac << ") total " << topoCore);
    }

    float areacore = -999.;
    if (dEtaMax_core>0 && dPhiMax_core>0) areacore = 4*dEtaMax_core*dPhiMax_core;
    else if (dR2Max_core>0) areacore = M_PI*dR2Max_core;
    
    std::map<Iso::IsolationCorrectionParameter,float> corecorr;
    corecorr[Iso::coreEnergy] = topoCore;
    corecorr[Iso::coreArea]   = areacore;
    result.coreCorrections[Iso::coreCone] = corecorr;
    
    if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone))) {
      for (unsigned int i = 0; i < result.etcones.size(); i++){
	result.etcones[i] -= topoCore;
	ATH_MSG_DEBUG("i: " << i << " cone [before] " << result.etcones[i]+topoCore << " cone [after] " << result.etcones[i]);
      }
    }
    
    return true;
  }

bool CaloIsolationTool::correctIsolationEnergy_pflowCore(CaloIsolation& result, float eta, float phi, 
							 float detaMax, float dphiMax, float dR2Max,
							 const std::vector<const PFO*>& clusts, bool onlyEM) 
  {
    
    float pflowCore(0.);
    BOOST_FOREACH (const PFO* cl, clusts) {
      ATH_MSG_DEBUG("pflo: eta " << cl->eta() << " phi " << cl->phi() << " pt " << cl->pt() << " ptEM = " << cl->ptEM()
		    << " charge " << cl->charge());
      float dphi = P4Helpers::deltaPhi(cl->phi(), phi);
      if (detaMax > 0 && fabs(dphi) > dphiMax) continue;
      float deta = cl->eta()-eta;
      if (dphiMax > 0 && fabs(deta) > detaMax) continue;
      if (dR2Max > 0 && dphi*dphi+deta*deta > dR2Max) continue;
      
      /// get energy
      float et = cl->pt();
      if (m_useEMScale) 
	et = cl->ptEM();
      if (et <= 0 || fabs(cl->eta()) > 7.0) continue;

      double emfrac = 1.;
      if (onlyEM) {
	const xAOD::CaloCluster *ocl = cl->cluster(0);
	double eEM = ocl->energyBE(0)+ocl->energyBE(1)+ocl->energyBE(2)+ocl->energyBE(3);
	emfrac     = std::min(1.,eEM / cl->eEM());
      }
      et *= emfrac;
      
      /// add to the core
      pflowCore += et;
      ATH_MSG_DEBUG("pflow to be added to core: et = " << et << " (em frac = " << emfrac << "), total = " << pflowCore);
    }
    
    /// set results
    float areacore = -999.;
    if      (detaMax > 0 && dphiMax > 0) areacore = 4*detaMax*dphiMax;
    else if (dR2Max > 0)                 areacore = M_PI*dR2Max;
    
    std::map<Iso::IsolationCorrectionParameter,float> corecorr;
    corecorr[Iso::coreEnergy] = pflowCore;
    corecorr[Iso::coreArea]   = areacore;
    result.coreCorrections[Iso::coreCone] = corecorr;
    
    if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone))) {
      for (unsigned int i = 0; i < result.etcones.size(); i++) {
	ATH_MSG_DEBUG("pflow, cone type " << i << ", in cone " << result.etcones[i] << ", subtracted " << result.etcones[i]-pflowCore);
	result.etcones[i] -= pflowCore;
      }
    }
    
    return true;
  }



  // etcone implementation for TrackParticle
  bool CaloIsolationTool::correctIsolationEnergy_MuonCore( CaloIsolation& result, const TrackParticle& tp) {

    ATH_MSG_DEBUG("in  CaloIsolationTool::correctIsolationEnergy_MuonCore ");

    // initialize varialbes
    double ecore = 0.;
    auto muI = m_derefMap.find(&tp);
    if(muI!=m_derefMap.end() && muI->second->isAvailable<float>("ET_Core")){
      ecore = muI->second->auxdataConst<float>("ET_Core");
    }else{
     ATH_MSG_WARNING("ET_Core of muon not found! coreMuon isolation correction will not be applied!!!");
     return false;
    }

// //     bool noEM = false; // should be an argument of the function
// //     const CaloSampling::CaloSample firstTile = CaloSampling::HEC0;
// 
//     /// get intersections
//     const Trk::CaloExtension* caloExtension = 0;
//     if(!m_caloExtTool->caloExtension(tp, caloExtension)){
//       ATH_MSG_WARNING("Can not get caloExtension.");
//       return false;
//     }
//     const std::vector<const Trk::CurvilinearParameters*>& intersections = caloExtension->caloLayerIntersections();
// 
//     /// start calculation
//     Amg::Vector3D enterMom(0.,0.,0.), currentMom(0.,0.,0.);
//     bool entered = false;
//     for (unsigned int i=0;i<intersections.size();++i){
//       int layerID = parsIdHelper.caloSample(intersections[i]->cIdentifier());
//       if(layerID>=24) continue;
// //       if(noEM && layerID<=firstTile) continue;
// 
//       bool entering = parsIdHelper.isEntryToVolume(intersections[i]->cIdentifier());
//       currentMom = intersections[i]->momentum();
// 
//       /// if already entered and it's not entering another
//       if(entered && !entering) ecore += enterMom.mag() - currentMom.mag();
//       ATH_MSG_DEBUG("layerID=" <<  layerID <<" entering=" << entering << " entered=" << entered << " enterMom=" << enterMom.mag() << " currentMom=" << currentMom.mag() << " ecore=" << ecore);
// 
//       /// if just entering after leaving the previous one
//       if(entering && !entered) enterMom = currentMom;
// 
//       /// save preious status
//       entered = entering;
//     }
   
    std::map<Iso::IsolationCorrectionParameter,float> corecorr;
    corecorr[Iso::coreEnergy] = ecore;
    corecorr[Iso::coreArea]   = 0;
    result.coreCorrections[Iso::coreMuon] = corecorr;
    
    if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreMuon))) {
      for( unsigned int i=0;i<result.etcones.size();++i ) {
	result.etcones[i] -= ecore;
	ATH_MSG_DEBUG("i: " << i << " cone [before] " << result.etcones[i]+ecore << " cone [after] " << result.etcones[i]);
      }
    }
    
    return true;
  }
  
  bool CaloIsolationTool::PtCorrection (CaloIsolation& result, 
					const Egamma& eg, 
					const std::vector<Iso::IsolationType>& isoTypes)
  {

    std::vector<float> corrvec; 
    corrvec.resize(isoTypes.size(),0.);
    for (unsigned int i = 0; i < isoTypes.size(); i++) {
      
      corrvec[i] = m_IsoLeakCorrectionTool->GetPtCorrection(eg, isoTypes[i]);
      if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::ptCorrection))) {
	result.etcones[i] -= corrvec[i];
	ATH_MSG_DEBUG("eta = " << eg.eta() << ", phi = " << eg.phi() << ", pt = " << eg.pt() << ", isoType = " << Iso::toString(isoTypes[i]) 
		      << ", ptcorr = " << corrvec[i] << ", isol pt corrected = " << result.etcones[i] );    
      }
    }
    result.noncoreCorrections[Iso::ptCorrection] = corrvec;

    return true;
  }

  bool CaloIsolationTool::EDCorrection(CaloIsolation& result, 
				       const std::vector<Iso::IsolationType>& isoTypes,
				       float eta, // In principle, could be (eta,phi)
				       std::string type)

  {
    // assume two densities for the time being
    const EventShape* edShape;
    std::string esName = (fabs(eta) < 1.5) ? "TopoClusterIsoCentralEventShape" : "TopoClusterIsoForwardEventShape";
    if (type == "PFlow") {
      if (fabs(eta) < 1.5) 
	esName = "NeutralParticleFlowIsoCentralEventShape";
      else
	esName = "NeutralParticleFlowIsoForwardEventShape";
    }
    if (evtStore()->retrieve(edShape,esName).isFailure()) {
      ATH_MSG_WARNING("Cannot retrieve density container " << esName << " for isolation correction. No ED correction");
      return false;
    }	

    double rho = 0;
    bool gotDensity = edShape->getDensity(EventShape::Density,rho);
    if (!gotDensity) {
      ATH_MSG_WARNING("Cannot retrieve density " << esName << " for isolation correction. No ED correction");
      return false;
    }

    // Get the core size
    float areacore = 0;
    std::map<xAOD::Iso::IsolationCorrectionParameter,float> ecore;
    if(result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreMuon))){
      ecore = result.coreCorrections.find(Iso::coreMuon)->second;
    }else if(result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::core57cells))){
      ecore = result.coreCorrections.find(Iso::core57cells)->second;
    }else if(result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::coreCone))){
      ecore = result.coreCorrections.find(Iso::coreCone)->second;
    }
    
    auto iter = ecore.find(xAOD::Iso::coreArea);
    if (iter != ecore.end())
      areacore = ecore.find(xAOD::Iso::coreArea)->second;
    
    std::vector<float> corrvec;
    corrvec.resize(isoTypes.size(),0.);
    for (unsigned int i = 0; i < isoTypes.size(); i++) {
      float dR    = Iso::coneSize(isoTypes.at(i));
      float toSub = rho*(dR*dR*M_PI - areacore);
      corrvec[i] = toSub;
      if (result.corrlist.calobitset.test(static_cast<unsigned int>(Iso::pileupCorrection))){
	result.etcones[i] -= toSub;
      }
      ATH_MSG_DEBUG("ED correction for size " << dR << " = " << toSub);
    }

    result.noncoreCorrections[Iso::pileupCorrection] = corrvec;
    return true;
  }

  void CaloIsolationTool::initresult(CaloIsolation& result, 
				     CaloCorrection corrlist, 
				     unsigned int typesize){

    result.corrlist = corrlist;
    result.coreCorrections.clear();
    result.noncoreCorrections.clear();
    result.etcones.resize(typesize,0.);

  }

  const IParticle* CaloIsolationTool::getReferenceParticle(const IParticle& particle) const {
    const TrackParticle* tp = dynamic_cast<const TrackParticle*>(&particle);
    if( tp ) return tp;
    const Muon* muon = dynamic_cast<const Muon*>(&particle);
    if( muon ) {
      const xAOD::TrackParticle* tp = 0;
      if(muon->inDetTrackParticleLink().isValid()) tp = *muon->inDetTrackParticleLink();
      if( !tp ) tp = muon->primaryTrackParticle();
      if( !tp ) {
        ATH_MSG_WARNING(" No TrackParticle found for muon " );
        return 0;
      }
      return tp;
    }
    return &particle;
  }

  bool CaloIsolationTool::decorateParticle(CaloIsolation& result,
					   IParticle& tp,
					   const std::vector<Iso::IsolationType>& cones,
					   CaloCorrection corrections){
    // get the applied corrections
    std::vector<Iso::IsolationCaloCorrection> correctionTypes;
    Iso::IsolationCaloCorrectionBitsetHelper::decode(corrections.calobitset,correctionTypes);
    ATH_MSG_DEBUG("Decoded correction types: " << correctionTypes.size());

    // decorate the particle
    SG::AuxElement::Accessor< uint32_t >* bitsetAcc = getIsolationCorrectionBitsetAccessor(Iso::isolationFlavour(cones[0]));

    if( bitsetAcc )
      (*bitsetAcc)(tp) = corrections.calobitset.to_ulong();
    else
      ATH_MSG_WARNING("Cannot find bitset accessor for flavour " << toString(Iso::isolationFlavour(cones[0])));
    
    // Fill all computed corrections
    // core correction type (e.g. coreMuon, core57cells)
    for (auto coretype : result.coreCorrections) {
      Iso::IsolationCaloCorrection ctype = coretype.first;
      // core energy and area
      for (auto par : coretype.second) {
	if (par.first == Iso::coreArea) continue; // do not store area, as they are constant ! (pi R**2 or 5*0.025 * 7*pi/128)
	SG::AuxElement::Accessor< float >* isoCorAcc = getIsolationCorrectionAccessor( Iso::isolationFlavour(cones[0]), ctype, par.first );
	if (isoCorAcc) { 
	  ATH_MSG_DEBUG("Storing core correction " << Iso::toString(ctype) << " var " << Iso::toString(par.first) << " = " << par.second);
	  (*isoCorAcc)(tp) = par.second;
	} else {
	  ATH_MSG_WARNING("Accessor not found for core correction " << Iso::toString(ctype) << ", var " << Iso::toString(par.first));
	}
      }
    }
    // noncore correction type (e.g. pileup)
    for (auto corrtype : result.noncoreCorrections) {
      Iso::IsolationCaloCorrection ctype = corrtype.first;
      if (ctype == Iso::pileupCorrection) continue; // do not store pileup corrections as they are rho * pi * (R**2 - areaCore) and rho is stored...
      std::vector<float> corrvec         = corrtype.second;
      if (corrvec.size() != cones.size()) {
	ATH_MSG_WARNING("Only cone size-based corrections are supported. Will do nothing to" << Iso::toString(ctype) );
	continue;
      }
      for (unsigned int i = 0; i < corrvec.size();i++) {
	SG::AuxElement::Accessor< float >* isoCorAcc = getIsolationCorrectionAccessor(cones[i],ctype);
	if (isoCorAcc) {
	  ATH_MSG_DEBUG("Storing non core correction " << Iso::toString(ctype) << " of iso type " << Iso::toString(cones[i]) << " = " << corrvec[i]);
	  (*isoCorAcc)(tp) = corrvec[i];
	} else
	  ATH_MSG_WARNING("Accessor not found for non core correction " << Iso::toString(ctype) << " of iso type " << Iso::toString(cones[i]));
      }
    }

    // fill main isolation
    // loop over cones
    if (result.etcones.size() == cones.size()) {
      for( unsigned int i=0;i<cones.size();++i ){
	
	Iso::IsolationType type = cones[i];
        SG::AuxElement::Accessor< float >* isoTypeAcc = getIsolationAccessor(type);
        if ( isoTypeAcc ) {
	  ATH_MSG_DEBUG("Filling " << Iso::toString(type) << " = " << result.etcones[i]);
          (*isoTypeAcc)(tp) = result.etcones[i];
        } else 
	  ATH_MSG_WARNING("Cannot find accessor for " << Iso::toString(type));
      }
    } else if( !result.etcones.empty() )
      ATH_MSG_WARNING("Inconsistent etcones vector size: results : " << result.etcones.size() << ", number of wanted cones : " << cones.size() );
    
    return true;
  }

  bool CaloIsolationTool::decorateParticle_caloCellIso( IParticle& tp,
                                          const std::vector<Iso::IsolationType>& cones,
                                          CaloCorrection corrections,
                                          const CaloCellContainer* Cells){
    // calculate the isolation
    CaloIsolation result;
    if( !caloCellIsolation(result,tp,cones,corrections,Cells) ) {
      ATH_MSG_DEBUG("Calculation of caloCellIsolation failed");
      return false;
    }
    if( !decorateParticle(result, tp, cones, corrections) ){
      ATH_MSG_DEBUG("Decoration of caloCellIsolation failed");
      return false;
    }

    return true;
  }


  bool CaloIsolationTool::decorateParticle_topoClusterIso( IParticle& tp,
                                          const std::vector<Iso::IsolationType>& cones,
                                          CaloCorrection corrections,
                                          const CaloClusterContainer* TopClusters){
    // calculate the isolation
    CaloIsolation result;
    if( !caloTopoClusterIsolation(result,tp,cones,corrections,TopClusters) ) {
      ATH_MSG_DEBUG("Calculation of caloTopoClusterIsolation failed");
      return false;
    }
    if( !decorateParticle(result, tp, cones, corrections) ){
      ATH_MSG_DEBUG("Decoration of caloTopoClusterIsolation failed");
      return false;
    }

    return true;
  }

  bool CaloIsolationTool::decorateParticle_eflowIso( IParticle& tp,
                                          const std::vector<Iso::IsolationType>& cones,
                                          CaloCorrection corrections){
    // calculate the isolation
    CaloIsolation result;
    if( !neutralEflowIsolation(result,tp,cones,corrections) ) {
      ATH_MSG_DEBUG("Calculation of neutralEflowIsolation failed");
      return false;
    }
    if( !decorateParticle(result, tp, cones, corrections) ){
      ATH_MSG_DEBUG("Decoration of neutralEflowIsolation failed");
      return false;
    }
    
    return true;
  }

  bool CaloIsolationTool::decorateParticle( IParticle& tp,
                                          const std::vector<Iso::IsolationType>& cones,
                                          CaloCorrection corrections,
                                          const CaloCellContainer* Cells,
                                          const CaloClusterContainer* TopClusters){
    ATH_MSG_DEBUG("just a test ");
    /// check Isolation flavour
    Iso::IsolationFlavour flavour = Iso::numIsolationFlavours;
    for( unsigned int i=0;i<cones.size();++i ){
      if(i==0) {flavour = isolationFlavour( cones[i] ); continue;}
      if(flavour != isolationFlavour( cones[i] )){
	ATH_MSG_FATAL("decorateParticle does not support mixture of the isolation flavour!!!");
	return false;
      }
    }
    
    bool suc(false);
    switch(flavour) {
    case Iso::etcone:
      suc = decorateParticle_caloCellIso(tp, cones, corrections, Cells); break;
    case Iso::topoetcone:
      suc = decorateParticle_topoClusterIso(tp, cones, corrections, TopClusters); break;
    case Iso::neflowisol:
      suc = decorateParticle_eflowIso(tp, cones, corrections); break;
    default:
      ATH_MSG_FATAL("Unsupported isolation flavour!!!"); return false;
    }
    
    return suc;    
  }
  
}	// end of namespace

