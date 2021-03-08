/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CscDataPreparator.h"

#include "StoreGate/ActiveStoreSvc.h"
#include "xAODTrigMuon/TrigMuonDefs.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool IsUnspoiled ( Muon::CscClusterStatus status );

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::CscDataPreparator::CscDataPreparator(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
   AthAlgTool(type,name,parent),
   m_regionSelector("RegSelTool/RegSelTool_CSC",this)
{
  declareProperty("RegSel_CSC", m_regionSelector);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::CscDataPreparator::initialize()
{


   ATH_MSG_DEBUG("Initializing CscDataPreparator - package version " << PACKAGE_VERSION);   
   ATH_MSG_DEBUG(m_decodeBS);
   ATH_MSG_DEBUG(m_doDecoding);

   // consistency check for decoding flag settings
   if(m_decodeBS && !m_doDecoding) {
     ATH_MSG_FATAL("Inconsistent setup, you tried to enable BS decoding but disable all decoding. Please fix the configuration");
     return StatusCode::FAILURE;
   }

   // Retreive raw data provider tool, but only if we need to decode the BS
   ATH_CHECK( m_rawDataProviderTool.retrieve(DisableTool{ !m_decodeBS || !m_doDecoding}) );
   
   ATH_MSG_INFO("Retrieved Tool " << m_rawDataProviderTool);
   
   // Retrieve PRD and cluster tools if we are doing the decoding
   ATH_CHECK( m_cscPrepDataProvider.retrieve(DisableTool{!m_doDecoding}) );
   ATH_MSG_INFO("Retrieved " << m_cscPrepDataProvider);

   ATH_CHECK( m_cscClusterProvider.retrieve(DisableTool{!m_doDecoding}) );
   ATH_MSG_INFO("Retrieved " << m_cscClusterProvider);

   ATH_CHECK(m_idHelperSvc.retrieve());

   // Locate RegionSelector
   ATH_CHECK( m_regionSelector.retrieve() );
   

   ATH_CHECK(m_cscPrepContainerKey.initialize(!m_cscPrepContainerKey.empty() && !m_doDecoding));
   //Write Handle for CSC clusters (only if we run decoding)
   ATH_CHECK(m_cscClustersKey.initialize(m_doDecoding));
   //
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::CscDataPreparator::prepareData(const TrigRoiDescriptor* p_roids,
							TrigL2MuonSA::MuonRoad& muonRoad,
							TrigL2MuonSA::CscHits&  cscHits)
{
  const IRoiDescriptor* iroi = (IRoiDescriptor*) p_roids;

  const bool to_full_decode=( std::abs(p_roids->etaMinus())>1.7 || std::abs(p_roids->etaPlus())>1.7 ) && !m_use_RoIBasedDataAccess;

  std::vector<IdentifierHash> cscHashIDs;

  // Decode
  if(m_doDecoding) {
    // Select RoI hits
    if (m_use_RoIBasedDataAccess) {
      ATH_MSG_DEBUG("Use Csc RoI based data access");
      m_regionSelector->HashIDList( *iroi, cscHashIDs );
    } else {
      ATH_MSG_DEBUG("Use full data access");
      //    m_regionSelector->DetHashIDList( CSC, cscHashIDs ); full decoding is executed with an empty vector
    }
    ATH_MSG_DEBUG("cscHashIDs.size()=" << cscHashIDs.size());

    if(m_decodeBS) {
      if ( m_rawDataProviderTool->convert(cscHashIDs).isFailure()) {
        ATH_MSG_WARNING("Conversion of BS for decoding of CSCs failed");
      }
    }
    std::vector<IdentifierHash> cscHashIDs_decode;
    cscHashIDs_decode.clear();
    if( !cscHashIDs.empty() || to_full_decode ){
      if( m_cscPrepDataProvider->decode( cscHashIDs, cscHashIDs_decode ).isFailure() ){
        ATH_MSG_WARNING("Problems when preparing CSC PrepData");
      }
      cscHashIDs.clear();
    }
    ATH_MSG_DEBUG("cscHashIDs_decode.size()=" << cscHashIDs_decode.size());

    // Clustering
    std::vector<IdentifierHash> cscHashIDs_cluster;
    cscHashIDs_cluster.clear();
    if(to_full_decode) cscHashIDs_decode.clear();
    if( !cscHashIDs_decode.empty() || to_full_decode ){
      SG::WriteHandle<Muon::CscPrepDataContainer> wh_clusters(m_cscClustersKey);
      if (!wh_clusters.isPresent()) {
	Muon::CscPrepDataContainer* object = new Muon::CscPrepDataContainer(m_idHelperSvc->cscIdHelper().module_hash_max());
      /// record the container in storeGate
	ATH_CHECK(wh_clusters.record(std::unique_ptr<Muon::CscPrepDataContainer>(object)));
	if( m_cscClusterProvider->getClusters( cscHashIDs_decode, cscHashIDs_cluster, object ).isFailure() ){
	  ATH_MSG_WARNING("Problems when preparing CSC Clusters");
	}
      } else {
	//need to retrieve from evt store for Run 2 trigger (only used in non MT processing)
	const Muon::CscPrepDataContainer* outputCollection_c = nullptr;
	ATH_CHECK(evtStore()->retrieve(outputCollection_c, m_cscClustersKey.key()));
	Muon::CscPrepDataContainer* object ATLAS_THREAD_SAFE = const_cast<Muon::CscPrepDataContainer*> (outputCollection_c);
	if( m_cscClusterProvider->getClusters( cscHashIDs_decode, cscHashIDs_cluster, object ).isFailure() ){
	  ATH_MSG_WARNING("Problems when preparing CSC Clusters");
	}
      }
      cscHashIDs_decode.clear();
    }

    // Debug info
    ATH_MSG_DEBUG("CSC cluster #hash = " << cscHashIDs_cluster.size());
    cscHashIDs = cscHashIDs_cluster;// use the cscHashIDs vector later to know if there is any CSC data to process
  }//doDecoding

  // Clear the output
  cscHits.clear();

  // Get CSC container
  if(!m_cscPrepContainerKey.empty() &&(!m_doDecoding || to_full_decode || !cscHashIDs.empty() )){
    auto cscPrepContainerHandle = SG::makeHandle(m_cscPrepContainerKey);
    const Muon::CscPrepDataContainer* cscPrepContainer = cscPrepContainerHandle.cptr();
    if (!cscPrepContainerHandle.isValid()) {
      ATH_MSG_ERROR("Cannot retrieve CSC PRD Container key: " << m_cscPrepContainerKey.key());
      return StatusCode::FAILURE;
    }    

    // Loop over collections
    for( const Muon::CscPrepDataCollection* cscCol : *cscPrepContainer ){
      if( cscCol==nullptr ) continue;
      cscHits.reserve( cscHits.size() + cscCol->size() );
      // Loop over data in the collection
      for( const Muon::CscPrepData* prepData : *cscCol ) {
    	if( prepData==nullptr ) continue;

	// Road info
	int chamber = xAOD::L2MuonParameters::Chamber::CSC;
	double aw = muonRoad.aw[chamber][0];
	double bw = muonRoad.bw[chamber][0];
	//double rWidth = muonRoad.rWidth[chamber][0];
	double phiw = muonRoad.phi[4][0];//roi_descriptor->phi(); //muonRoad.phi[chamber][0];

	//cluster status
	bool isunspoiled = IsUnspoiled (prepData->status());


	// Create new digit
	TrigL2MuonSA::CscHitData cscHit;
	cscHit.StationName  = m_idHelperSvc->cscIdHelper().stationName( prepData->identify() );
	cscHit.StationEta   = m_idHelperSvc->cscIdHelper().stationEta( prepData->identify() );
	cscHit.StationPhi   = m_idHelperSvc->cscIdHelper().stationPhi( prepData->identify() );
	cscHit.ChamberLayer = (true==isunspoiled) ? 1 : 0;
	cscHit.WireLayer    = m_idHelperSvc->cscIdHelper().wireLayer( prepData->identify() );
	cscHit.MeasuresPhi  = m_idHelperSvc->cscIdHelper().measuresPhi( prepData->identify() );
	cscHit.Strip        = m_idHelperSvc->cscIdHelper().strip( prepData->identify() );
	cscHit.Chamber      = chamber;
	cscHit.StripId = (cscHit.StationName << 18)
	  | ((cscHit.StationEta + 2) << 16) | (cscHit.StationPhi << 12)
	  | (cscHit.WireLayer << 9) | (cscHit.MeasuresPhi << 8) | (cscHit.Strip);
	cscHit.eta = prepData->globalPosition().eta();
	cscHit.phi = prepData->globalPosition().phi();
	cscHit.r   = prepData->globalPosition().perp();
	cscHit.z   = prepData->globalPosition().z();
	cscHit.charge = prepData->charge();
	cscHit.time   = prepData->time();
	cscHit.resolution = std::sqrt( prepData->localCovariance()(0,0) );
	cscHit.Residual =  ( cscHit.MeasuresPhi==0 ) ? calc_residual( aw, bw, cscHit.z, cscHit.r ) : calc_residual_phi( aw,bw,phiw, cscHit.phi, cscHit.z);
	cscHit.isOutlier = 0;
	/*if( std::abs(cscHit.Residual) > rWidth ) {
	  cscHit.isOutlier = 2;
	  }*/
	double width = (cscHit.MeasuresPhi) ? 250. : 100.;
	if( std::abs(cscHit.Residual)>width ){
	  cscHit.isOutlier=1;
	  if( std::abs(cscHit.Residual)>3.*width ){
	    cscHit.isOutlier=2;
	  }
	}
	
	cscHits.push_back( cscHit );
	
	// Debug print
       	ATH_MSG_DEBUG("CSC Hits: "
		      << "SN="  << cscHit.StationName << ","
		      << "SE="  << cscHit.StationEta << ","
		      << "SP="  << cscHit.StationPhi << ","
		      << "CL="  << cscHit.ChamberLayer << ","
		      << "WL="  << cscHit.WireLayer << ","
		      << "MP="  << cscHit.MeasuresPhi << ","
		      << "St="  << cscHit.Strip << ","
		      << "ID="  << cscHit.StripId << ","
		      << "eta=" << cscHit.eta << ","
		      << "phi=" << cscHit.phi << ","
		      << "r="   << cscHit.r << ","
		      << "z="   << cscHit.z << ","
		      << "q="   << cscHit.charge << ","
		      << "t="   << cscHit.time << ","
		      << "Rs="  << cscHit.Residual << ","
		      << "OL="  << cscHit.isOutlier);
      }
    }
  }

  //
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::CscDataPreparator::calc_residual(double aw,double bw,double x,double y) const
{
  const double ZERO_LIMIT = 1e-4;
  if( std::abs(aw) < ZERO_LIMIT ) return y-bw;
  double ia  = 1/aw;
  double iaq = ia*ia;
  double dz  = x - (y-bw)*ia;
  return dz/std::sqrt(1.+iaq);

}


double TrigL2MuonSA::CscDataPreparator::calc_residual_phi( double aw, double bw, double phiw, double hitphi, double hitz) const 
{

  double roadr = hitz*aw + bw;

  return roadr*( std::cos(phiw)*std::sin(hitphi)-std::sin(phiw)*std::cos(hitphi) );

}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------


bool IsUnspoiled ( Muon::CscClusterStatus status ) {
  if (status == Muon::CscStatusUnspoiled || status == Muon::CscStatusSplitUnspoiled )
    return true;

  return false;
}
