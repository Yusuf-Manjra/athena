/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/TgcDataPreparator.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "Identifier/IdentifierHash.h"

#include "TrigL2MuonSA/TgcData.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"

#include "StoreGate/ActiveStoreSvc.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

using namespace MuonGM;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

static const InterfaceID IID_TgcDataPreparator("IID_TgcDataPreparator", 1, 0);

const InterfaceID& TrigL2MuonSA::TgcDataPreparator::interfaceID() { return IID_TgcDataPreparator; }

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcDataPreparator::TgcDataPreparator(const std::string& type, 
						   const std::string& name,
						   const IInterface*  parent): 
  AthAlgTool(type,name,parent),
   m_activeStore( "ActiveStoreSvc", name ), 
   m_rawDataProviderTool("Muon::TGC_RawDataProviderTool/TGC_RawDataProviderTool"),
   m_tgcPrepDataProvider("Muon::TgcRdoToPrepDataTool/TgcPrepDataProviderTool"),
   m_regionSelector( "RegSelSvc", name ), 
   m_robDataProvider( "ROBDataProviderSvc", name ),
   m_options(), m_recMuonRoIUtils()
{
   declareInterface<TrigL2MuonSA::TgcDataPreparator>(this);
   declareProperty("TgcRawDataProvider", m_rawDataProviderTool);
   declareProperty("TgcPrepDataProvider", m_tgcPrepDataProvider);
}


// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::TgcDataPreparator::~TgcDataPreparator() 
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::initialize()
{
   // Get a message stream instance
  ATH_MSG_DEBUG("Initializing TgcDataPreparator - package version " << PACKAGE_VERSION );
   
   StatusCode sc;
   sc = AthAlgTool::initialize();
   if (!sc.isSuccess()) {
     ATH_MSG_ERROR("Could not initialize the AthAlgTool base class.");
     return sc;
   }

   // Locate RegionSelector
   ATH_CHECK( m_regionSelector.retrieve() );
   ATH_MSG_DEBUG("Retrieved service RegionSelector");

   ATH_CHECK( m_muonIdHelperTool.retrieve() );

   ATH_CHECK( m_activeStore.retrieve() ); 
   ATH_MSG_DEBUG("Retrieved ActiveStoreSvc." );

   // Retreive TGC raw data provider tool
   ATH_MSG_DEBUG(m_decodeBS);
   ATH_MSG_DEBUG(m_doDecoding);
   // disable TGC Raw data provider if we either don't decode BS or don't decode TGCs
   if (m_rawDataProviderTool.retrieve(DisableTool{ !m_decodeBS || !m_doDecoding}).isFailure()) {
     msg (MSG::FATAL) << "Failed to retrieve " << m_rawDataProviderTool << endmsg;
     return StatusCode::FAILURE;
   } else
     msg (MSG::INFO) << "Retrieved Tool " << m_rawDataProviderTool << endmsg;

   // Disable PRD converter if we don't do the data decoding
   ATH_CHECK( m_tgcPrepDataProvider.retrieve(DisableTool{!m_doDecoding}) );
   ATH_MSG_DEBUG("Retrieved tool " << m_tgcPrepDataProvider );

   // Locate ROBDataProvider
   ATH_CHECK( m_robDataProvider.retrieve() );
   ATH_MSG_DEBUG("Retrieved service " << m_robDataProvider.name() );

   ATH_CHECK(m_tgcContainerKey.initialize());
      
   // 
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

void TrigL2MuonSA::TgcDataPreparator::setRoIBasedDataAccess(bool use_RoIBasedDataAccess)
{
  m_use_RoIBasedDataAccess = use_RoIBasedDataAccess;
  return;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::prepareData(const LVL1::RecMuonRoI*  p_roi,
							TrigL2MuonSA::TgcHits&  tgcHits)
{
   float roi_eta = p_roi->eta();
   float roi_phi = p_roi->phi();
   if (roi_phi < 0) roi_phi += 2.0 * CLHEP::pi;
   
   double etaMin = p_roi->eta() - 0.2;
   double etaMax = p_roi->eta() + 0.2;
   double phiMin = p_roi->phi() - 0.1;
   double phiMax = p_roi->phi() + 0.1;
   if( phiMin < 0 ) phiMin += 2*CLHEP::pi;
   if( phiMax < 0 ) phiMax += 2*CLHEP::pi;
   if( phiMin > 2*CLHEP::pi ) phiMin -= 2*CLHEP::pi;
   if( phiMax > 2*CLHEP::pi ) phiMax -= 2*CLHEP::pi;

   TrigRoiDescriptor* roi = new TrigRoiDescriptor( p_roi->eta(), etaMin, etaMax, p_roi->phi(), phiMin, phiMax ); 
   const IRoiDescriptor* iroi = (IRoiDescriptor*) roi;
   
   //const Muon::TgcPrepDataContainer* tgcPrepContainer = 0;
   const Muon::TgcPrepDataContainer* tgcPrepContainer;
   int gasGap;
   int channel;
   
   bool isLowPt = m_recMuonRoIUtils.isLowPt(p_roi);

   // Select the eta cut based on ROI Pt.
   double mid_eta_test = (isLowPt) ? m_options.roadParameters().deltaEtaAtMiddleForLowPt()
     : m_options.roadParameters().deltaEtaAtMiddleForHighPt();
   double inn_eta_test = (isLowPt) ? m_options.roadParameters().deltaEtaAtInnerForLowPt()
     : m_options.roadParameters().deltaEtaAtInnerForHighPt();
   double mid_phi_test = m_options.roadParameters().deltaPhiAtMiddle();
   double inn_phi_test = m_options.roadParameters().deltaPhiAtInner();
   
   if(m_doDecoding) {
     std::vector<IdentifierHash> tgcHashList;
     if (iroi) m_regionSelector->DetHashIDList(TGC, *iroi, tgcHashList);
     else m_regionSelector->DetHashIDList(TGC, tgcHashList);
     if(roi) delete roi;

     // Decode BS
     if (m_decodeBS){
       if ( m_rawDataProviderTool->convert(tgcHashList).isFailure()) {
         ATH_MSG_WARNING("Conversion of BS for decoding of TGCs failed");
       }
     }

     // now convert from RDO to PRD
     std::vector<IdentifierHash> outhash;
     
     if( m_tgcPrepDataProvider->decode(tgcHashList, outhash).isFailure() ){
       ATH_MSG_ERROR("Failed to convert from RDO to PRD");
       return StatusCode::FAILURE;
     }
   }//doDecoding
   
   if ( m_activeStore ) {
     auto tgcContainerHandle = SG::makeHandle(m_tgcContainerKey);
     tgcPrepContainer = tgcContainerHandle.cptr();
     if (!tgcContainerHandle.isValid()) { 
       ATH_MSG_ERROR("Could not retrieve PrepDataContainer key:" << m_tgcContainerKey.key());
       return StatusCode::FAILURE;
     } else {
       ATH_MSG_DEBUG("Retrieved PrepDataContainer: " << tgcPrepContainer->numberOfCollections());
     }
   } else {
     ATH_MSG_ERROR("Null pointer to ActiveStore");
     return StatusCode::FAILURE;
   }  
 
   //Find closest wires in Middle
   Muon::TgcPrepDataContainer::const_iterator wi = tgcPrepContainer->begin();
   Muon::TgcPrepDataContainer::const_iterator wi_end = tgcPrepContainer->end();
   float min_dphi_wire=1000.;
   float second_dphi_wire=1000.;
   std::vector<float> ov_dphi;
   ov_dphi.clear();
   for( ; wi!=wi_end; ++wi ) { // loop over collections
     const Muon::TgcPrepDataCollection* colwi = *wi;
     if( !colwi ) continue;
     Muon::TgcPrepDataCollection::const_iterator cwi = colwi->begin();
     Muon::TgcPrepDataCollection::const_iterator cwi_end = colwi->end();
     for( ;cwi!=cwi_end;++cwi ){ // loop over data in the collection
       if( !*cwi ) continue;
       const Muon::TgcPrepData& prepDataWi = **cwi;
       if (!m_muonIdHelperTool->tgcIdHelper().isStrip(prepDataWi.identify())) {//wire
         int stationNumWi = m_muonIdHelperTool->tgcIdHelper().stationRegion(prepDataWi.identify())-1;
         if (stationNumWi==-1) stationNumWi=3;
         if (stationNumWi<3 && fabs(prepDataWi.globalPosition().eta() - roi_eta) < mid_eta_test ) {
           float dphi = acos(cos(prepDataWi.globalPosition().phi()-roi_phi));
           bool overlap=false;
           for (unsigned int ov=0;ov<ov_dphi.size();ov++)
             if (fabs(dphi-ov_dphi[ov])<1e-5) overlap=true;
           if (overlap) continue;
           ov_dphi.push_back(dphi);
           if (dphi<second_dphi_wire){
             second_dphi_wire=dphi;
           }
           if (dphi<min_dphi_wire) {
             second_dphi_wire=min_dphi_wire;
             min_dphi_wire=dphi;
           }
         }
       }
     }
   }

   //Check if there are enough number of hits
   Muon::TgcPrepDataContainer::const_iterator hit = tgcPrepContainer->begin();
   Muon::TgcPrepDataContainer::const_iterator hit_end = tgcPrepContainer->end();
   int num_min_hits=0;
   int num_second_hits=0;
   for( ; hit!=hit_end; ++hit ) { // loop over collections
     const Muon::TgcPrepDataCollection* colhit = *hit;
     if( !colhit ) continue;
     Muon::TgcPrepDataCollection::const_iterator chit = colhit->begin();
     Muon::TgcPrepDataCollection::const_iterator chit_end = colhit->end();
     for( ;chit!=chit_end;++chit ){ // loop over data in the collection
       if( !*chit ) continue;
       const Muon::TgcPrepData& prepDataHit = **chit;
       if (!m_muonIdHelperTool->tgcIdHelper().isStrip(prepDataHit.identify())) {//strip
         int stationNumHit = m_muonIdHelperTool->tgcIdHelper().stationRegion(prepDataHit.identify())-1;
         if (stationNumHit==-1) stationNumHit=3;
         if (stationNumHit<3 && fabs(prepDataHit.globalPosition().eta() - roi_eta) < mid_eta_test ) {
           float dphi = acos(cos(prepDataHit.globalPosition().phi()-roi_phi));
           if (fabs(dphi-min_dphi_wire)<1e-5) num_min_hits++;
           if (fabs(dphi-second_dphi_wire)<1e-5) num_second_hits++;
         }
       }
     }
   }

   float dphi_wire=min_dphi_wire;
   bool useDefault=false;
   if (num_min_hits<5) {
     if (num_second_hits>5) dphi_wire=second_dphi_wire;
     else useDefault=true;
   }

   Muon::TgcPrepDataContainer::const_iterator it = tgcPrepContainer->begin();
   Muon::TgcPrepDataContainer::const_iterator it_end = tgcPrepContainer->end();
   for( ; it!=it_end; ++it ) { // loop over collections
     const Muon::TgcPrepDataCollection* col = *it;
     if( !col ) continue;
     Muon::TgcPrepDataCollection::const_iterator cit = col->begin();
     Muon::TgcPrepDataCollection::const_iterator cit_end = col->end();
     for( ;cit!=cit_end;++cit ){ // loop over data in the collection
       if( !*cit ) continue;
       
       const Muon::TgcPrepData& prepData = **cit;
       
       bool isInRoad = false;
       int stationNum = m_muonIdHelperTool->tgcIdHelper().stationRegion(prepData.identify())-1;
       if (stationNum==-1) stationNum=3;
       if (m_muonIdHelperTool->tgcIdHelper().isStrip(prepData.identify())) {
	 double dphi = fabs(prepData.globalPosition().phi() - roi_phi);
	 if( dphi > CLHEP::pi*2 ) dphi = dphi - CLHEP::pi*2;
	 if( dphi > CLHEP::pi ) dphi = CLHEP::pi*2 - dphi;
	 // For strips, apply phi cut
	 if     ( stationNum < 3  && dphi < mid_phi_test ) { isInRoad = true; }
	 else if( stationNum == 3 && dphi < inn_phi_test ) { isInRoad = true; }
       }
       else {
	 // For wires, apply eta cut.
         float dphi = acos(cos(prepData.globalPosition().phi()-roi_phi));
	 if     ( stationNum < 3  && fabs(prepData.globalPosition().eta() - roi_eta) < mid_eta_test ) {
           if (useDefault) isInRoad = true;//default
           else if (fabs(dphi-dphi_wire)<1e-5) isInRoad = true;//for close-by muon 
         }
	 else if( stationNum == 3 && fabs(prepData.globalPosition().eta() - roi_eta) < inn_eta_test ) { isInRoad = true; }
       }
       if( ! isInRoad ) continue;
       
       m_tgcReadout = prepData.detectorElement();
       gasGap = m_muonIdHelperTool->tgcIdHelper().gasGap(prepData.identify());
       channel = m_muonIdHelperTool->tgcIdHelper().channel(prepData.identify());
       
       TrigL2MuonSA::TgcHitData lutDigit;
       
       lutDigit.eta = prepData.globalPosition().eta();
       lutDigit.phi = prepData.globalPosition().phi();
       lutDigit.r = prepData.globalPosition().perp();
       lutDigit.z = prepData.globalPosition().z();
       lutDigit.sta = stationNum;
       lutDigit.isStrip = m_muonIdHelperTool->tgcIdHelper().isStrip(prepData.identify());
       if(m_muonIdHelperTool->tgcIdHelper().isStrip(prepData.identify())){
	 lutDigit.width = m_tgcReadout->stripWidth(gasGap, channel);
       }
       else{
	 lutDigit.width = m_tgcReadout->gangLength(gasGap, channel);
       }
       lutDigit.bcTag = 2;
       lutDigit.inRoad = false;
       

       tgcHits.push_back(lutDigit);
       
     }
   }
   
   return StatusCode::SUCCESS; 
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::TgcDataPreparator::finalize()
{
  ATH_MSG_DEBUG("Finalizing TgcDataPreparator - package version " << PACKAGE_VERSION);
   
   StatusCode sc = AthAlgTool::finalize(); 
   return sc;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
