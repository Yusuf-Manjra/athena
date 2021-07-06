/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonInputProviderLegacy.h"

#include <math.h>

#include "GaudiKernel/ITHistSvc.h"

#include "L1TopoEvent/TopoInputEvent.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/MuCTPIL1Topo.h"
#include "TrigT1Interfaces/MuCTPIL1TopoCandidate.h"

#include "TrigT1Result/MuCTPIRoI.h"
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"

#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfData/L1Menu.h"

using namespace std;
using namespace LVL1;

MuonInputProviderLegacy::MuonInputProviderLegacy( const std::string& type, const std::string& name, 
                                      const IInterface* parent) :
   base_class(type, name, parent),
   m_histSvc("THistSvc", name)
{
   declareInterface<LVL1::IInputTOBConverter>( this );
}

StatusCode
MuonInputProviderLegacy::initialize() {
   ATH_MSG_DEBUG("Retrieving LVL1ConfigSvc " << m_configSvc);
   CHECK( m_configSvc.retrieve() );

   // Get the RPC and TGC RecRoI tool
   ATH_CHECK( m_recRPCRoiTool.retrieve() );
   ATH_CHECK( m_recTGCRoiTool.retrieve() );
   
   ATH_MSG_INFO("UseNewConfig set to " <<  (m_useNewConfig ? "True" : "False"));
   if(! m_useNewConfig) {
      m_MuonThresholds = m_configSvc->ctpConfig()->menu().thresholdConfig().getThresholdVector(TrigConf::L1DataDef::MUON);
   }
   CHECK(m_histSvc.retrieve());

   ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "MuonInputProviderLegacy");
   CHECK(incidentSvc.retrieve());
   incidentSvc->addListener(this,"BeginRun", 100);
   incidentSvc.release().ignore();

   // get MuctpiTool handle
   ATH_MSG_DEBUG("Retrieving MuctpiToolHandle " << m_MuctpiSimTool);
   CHECK( m_MuctpiSimTool.retrieve() );


   //This is a bit ugly but I've done it so the job options can be used to determine 
   //use of storegate
   CHECK(m_MuCTPItoL1TopoLocation.initialize(!m_MuCTPItoL1TopoLocation.key().empty()));
   
   if(!m_MuCTPItoL1TopoLocationPlusOne.key().empty())
      m_MuCTPItoL1TopoLocationPlusOne = m_MuCTPItoL1TopoLocation.key()+std::to_string(1);
   
   CHECK(m_MuCTPItoL1TopoLocationPlusOne.initialize(!m_MuCTPItoL1TopoLocationPlusOne.key().empty()));

   CHECK(m_muonROILocation.initialize(!m_muonROILocation.key().empty()));
   CHECK(m_roibLocation.initialize(!m_roibLocation.key().empty()));

   return StatusCode::SUCCESS;
}

void
MuonInputProviderLegacy::handle(const Incident& incident) {
   if (incident.type()!="BeginRun") return;
   ATH_MSG_DEBUG( "In BeginRun incident");

   string histPath = "/EXPERT/" + name() + "/";
   replace( histPath.begin(), histPath.end(), '.', '/'); 

   auto hPt = std::make_unique<TH1I>("MuonTOBPt", "Muon TOB Pt", 40, 0, 40);
   hPt->SetXTitle("p_{T}");

   auto hEtaPhi = std::make_unique<TH2I>("MuonTOBPhiEta", "Muon TOB Location", 25, -50, 50, 32, -32, 32);
   hEtaPhi->SetXTitle("#eta");
   hEtaPhi->SetYTitle("#phi");

   if (m_histSvc->regShared( histPath + "TOBPt", std::move(hPt), m_hPt ).isSuccess()){
      ATH_MSG_DEBUG("TOBPt histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register TOBPt histogram for MuonProvider");
   }
   if (m_histSvc->regShared( histPath + "TOBPhiEta", std::move(hEtaPhi), m_hEtaPhi ).isSuccess()){
      ATH_MSG_DEBUG("TOBPhiEta histogram has been registered successfully for MuonProvider.");
   }
   else{
      ATH_MSG_WARNING("Could not register TOBPhiEta histogram for MuonProvider");
   }
}

TCS::MuonTOB
MuonInputProviderLegacy::createMuonTOB(uint32_t roiword, const TrigConf::L1Menu * l1menu) const {

   LVL1::RecMuonRoI roi;
   if(m_useNewConfig) {
      roi.construct( roiword, m_recRPCRoiTool.get(), m_recTGCRoiTool.operator->(), l1menu );
   } else {
      roi.construct( roiword, m_recRPCRoiTool.operator->(), m_recTGCRoiTool.operator->(), &m_MuonThresholds );
   }

   ATH_MSG_DEBUG("Muon ROI: thrvalue = " << roi.getThresholdValue() << " eta = " << roi.eta() << " phi = " << roi.phi() << ", w   = " << MSG::hex << std::setw( 8 ) << roi.roiWord() << MSG::dec);
         
   TCS::MuonTOB muon( roi.getThresholdValue(), 0, int(10*roi.eta()), int(10*roi.phi()), roi.roiWord() );
   muon.setEtaDouble( roi.eta() );
   muon.setPhiDouble( roi.phi() );

   m_hPt->Fill(muon.Et());
   m_hEtaPhi->Fill(muon.eta(),muon.phi());
   
   return muon;
}

TCS::MuonTOB
MuonInputProviderLegacy::createMuonTOB(const MuCTPIL1TopoCandidate & roi) const {
   ATH_MSG_DEBUG("Muon ROI (MuCTPiToTopo): thr ID = " << roi.getptThresholdID() << " eta = " << roi.geteta() << " phi = " << roi.getphi()  
                  << ", w   = " << MSG::hex << std::setw( 8 ) << roi.getRoiID() << MSG::dec );
   ATH_MSG_DEBUG("                            Oct = " << roi.getMioctID() << " etacode=" <<  roi.getetacode() << " phicode= " <<  
                  roi.getphicode()<< ", Sector="<< roi.getSectorName() );

   // The L1 topo hardware works with phi in [0,2pi]. The MuCTPi give muons in [0,2pi].
   // However, L1 topo simulation works with [-pi, pi] and otherwise it crashes. Thus we have to convert here
   int etaTopo = roi.getieta();
   int phiTopo = roi.getiphi();
   if( phiTopo >= 32 ) phiTopo -= 64;

   TCS::MuonTOB muon(roi.getRoiID());

   // legacy bit setting
   muon.setBitsEt(8);
   muon.setBitsEta(6);
   muon.setBitsPhi(6);

   muon.setEt(roi.getptValue());
   muon.setIsolation(0);
   muon.setEta(etaTopo);
   muon.setPhi(phiTopo);
   muon.setEtDouble(static_cast<double>(roi.getptValue()));
   muon.setEtaDouble(static_cast<double>(etaTopo/10.));
   muon.setEtaDouble(static_cast<double>(phiTopo/10.));

   m_hPt->Fill(muon.Et());
   m_hEtaPhi->Fill(muon.eta(),muon.phi());

   return muon;
}

TCS::LateMuonTOB
MuonInputProviderLegacy::createLateMuonTOB(const MuCTPIL1TopoCandidate & roi) const {

   float phi = roi.getphi();
   if(phi<-M_PI) phi+=2.0*M_PI;
   if(phi> M_PI) phi-=2.0*M_PI;

   ATH_MSG_DEBUG("Late Muon ROI (MuCTPiToTopo):bcid=1 thr pt = " << roi.getptThresholdID() << " eta = " << roi.geteta() << " phi = " << phi << ", w   = " << MSG::hex << std::setw( 8 ) << roi.getRoiID() << MSG::dec);

   TCS::LateMuonTOB muon( roi.getptValue(), 0, int(10*roi.geteta()), int(10*phi), roi.getRoiID() );

   muon.setEtaDouble( roi.geteta() );
   muon.setPhiDouble( phi );

   m_hPt->Fill(muon.Et());
   m_hEtaPhi->Fill(muon.eta(),muon.phi());

   ATH_MSG_DEBUG("LateMuon created");
   return muon;
}

StatusCode
MuonInputProviderLegacy::fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const {

   if( m_MuonEncoding == 0 ) {

      ATH_MSG_DEBUG("Filling the muon input from MuCTPIToRoIBSLink produced by L1Muctpi.cxx.");

      const ROIB::RoIBResult* roibResult {nullptr};

      const L1MUINT::MuCTPIToRoIBSLink* muctpi_slink {nullptr};


      if(m_muonROILocation.key().empty()==false){
         SG::ReadHandle<L1MUINT::MuCTPIToRoIBSLink> MuCTPIHandle(m_muonROILocation);//LVL1MUCTPI::DEFAULT_MuonRoIBLocation)
         if( MuCTPIHandle.isValid() ){
            muctpi_slink = MuCTPIHandle.cptr();
         }
      }

      if(muctpi_slink == nullptr && not m_roibLocation.key().empty()){
         SG::ReadHandle<ROIB::RoIBResult> roib (m_roibLocation);
         if( roib.isValid() ){
            roibResult = roib.cptr(); 
         }
      }

      if(!muctpi_slink && !roibResult) {
         ATH_MSG_WARNING("Neither a MuCTPIToRoIBSLink with SG key " << m_muonROILocation.key() << " nor an RoIBResult were found in the event. No muon input for the L1Topo simulation.");
         return StatusCode::RECOVERABLE;
      }


      if( roibResult ) {

         const TrigConf::L1Menu * l1menu = nullptr;
         if( m_useNewConfig ) {
            ATH_CHECK( detStore()->retrieve(l1menu) );
         }

         const std::vector< ROIB::MuCTPIRoI >& rois = roibResult->muCTPIResult().roIVec();

         ATH_MSG_DEBUG("Filling the input event from RoIBResult. Number of Muon ROIs: " << rois.size() );

         for( const ROIB::MuCTPIRoI & muonRoI : rois ) {

            if( !( muonRoI.roIWord() & LVL1::CandidateVetoMask  ) ) {
               inputEvent.addMuon( MuonInputProviderLegacy::createMuonTOB( muonRoI.roIWord(), l1menu ) );
            } else {
            // overflow implemented only for reduced granularity encoding (see below)
               ATH_MSG_DEBUG(" Ignore Vetoed L1 Mu RoI " <<  muonRoI.roIWord() );
            }
         }

      } else if( muctpi_slink ) {

         const TrigConf::L1Menu * l1menu = nullptr;
         if( m_useNewConfig ) {
            ATH_CHECK( detStore()->retrieve(l1menu) );
         }

         ATH_MSG_DEBUG("Filling the input event. Number of Muon ROIs: " << muctpi_slink->getMuCTPIToRoIBWords().size() - ROIB::Header::wordsPerHeader - ROIB::Trailer::wordsPerTrailer - 1);
      
         unsigned int icnt = 0;
         for ( unsigned int roiword : muctpi_slink->getMuCTPIToRoIBWords() ) {
               
            ++icnt;
            // skip header
            if ( icnt <= ROIB::Header::wordsPerHeader + 1 ) {
               continue;
            }
            // skip trailer
            if ( icnt > ( muctpi_slink->getMuCTPIToRoIBWords().size() - ROIB::Trailer::wordsPerTrailer ) ) {
               continue;
            }
            if( !(roiword & LVL1::CandidateVetoMask) ) {
               inputEvent.addMuon( MuonInputProviderLegacy::createMuonTOB( roiword, l1menu ) );
            } else {
               ATH_MSG_DEBUG(" Ignore Vetoed L1 Mu RoI " << roiword );
            }
         }
      
      }
   } else {  // reduced granularity encoding
      ATH_MSG_DEBUG("Use MuCTPiToTopo granularity Muon ROIs.");

      // first see if L1Muctpi simulation already ran and object is in storegate, if not
      // call tool version of the L1MuctpiSimulation and create it on the fly

      const LVL1::MuCTPIL1Topo* l1topo  {nullptr};

      if(m_MuCTPItoL1TopoLocation.key().empty()==false){
         SG::ReadHandle<LVL1::MuCTPIL1Topo> l1topoh(m_MuCTPItoL1TopoLocation);
         if( l1topoh.isValid() ) l1topo = l1topoh.cptr();
      }

      if( l1topo ) {
         ATH_MSG_DEBUG("Use MuCTPiToTopo granularity Muon ROIs: retrieve from SG");

         const std::vector<MuCTPIL1TopoCandidate> & candList = l1topo->getCandidates();
         for( const MuCTPIL1TopoCandidate & muCand : candList) {
            inputEvent.addMuon( MuonInputProviderLegacy::createMuonTOB( muCand ) );
            if(muCand.moreThan2CandidatesOverflow()){
               inputEvent.setOverflowFromMuonInput(true);
               ATH_MSG_DEBUG("setOverflowFromMuonInput : true (MuCTPIL1TopoCandidate from SG)");
            }
         }
      } else {
         ATH_MSG_DEBUG("Use MuCTPiToTopo granularity Muon ROIs: calculate from ROIs sent to RoIB");
         LVL1::MuCTPIL1Topo l1topo;
         CHECK(m_MuctpiSimTool->fillMuCTPIL1Topo(l1topo));
         for( const MuCTPIL1TopoCandidate & cand : l1topo.getCandidates() ) {
            inputEvent.addMuon( MuonInputProviderLegacy::createMuonTOB( cand ) );
            if(cand.moreThan2CandidatesOverflow()){
               inputEvent.setOverflowFromMuonInput(true);
               ATH_MSG_DEBUG("setOverflowFromMuonInput : true (MuCTPIL1TopoCandidate from MuctpiSimTool)");
            }
         }
      }

      //BC+1 ... this can only come from simulation, in data taking this is collected by the L1Topo at its input
      // so no need to do anything else here
      if(m_MuCTPItoL1TopoLocationPlusOne.key().empty()==false) {
         SG::ReadHandle<LVL1::MuCTPIL1Topo> l1topoBC1(m_MuCTPItoL1TopoLocationPlusOne);
         if( l1topoBC1.isValid() ) {
            ATH_MSG_DEBUG( "Contains L1Topo LateMuons L1Muctpi object from StoreGate!" );
            const std::vector<MuCTPIL1TopoCandidate> & candList = l1topoBC1->getCandidates();
            for( const MuCTPIL1TopoCandidate& muCand : candList)
            {
               ATH_MSG_DEBUG("MuonInputProviderLegacy addLateMuon ");
               inputEvent.addLateMuon( MuonInputProviderLegacy::createLateMuonTOB( muCand ) );	   
            }
         }
      }
   }
   return StatusCode::SUCCESS;
}

