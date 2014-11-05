/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Authors: E. Reinherz-Aronis, A. Kreisel
 This Fex is able to run after both TrigMuGirl and TrigMounEF
 ***************************************************************************/
#include "TrigEFDiMuFex.h"


#include "StoreGate/StoreGateSvc.h"
//for the event info
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
// for ntuple
#include "GaudiKernel/NTuple.h"
// for truth information
#include "GeneratorObjects/McEventCollection.h"
// input object
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
//JW
//#include "CLHEP/Vector/ThreeVector.h"
//#include "CLHEP/Vector/LorentzVector.h"
#include "TLorentzVector.h"
//#include "EventPrimitives/EventPrimitives.h" // EIGEN
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuidEvent/MuidTrackContainer.h"

#include "TrigInterfaces/TECreateAlgo.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"


TrigEFDiMuFex::TrigEFDiMuFex(const std::string& name, ISvcLocator* pSvcLocator) :
HLT::FexAlgo(name, pSvcLocator),
m_pStoreGate        (NULL),
mTrigBphysColl(0),
//mTrigBphysAuxColl(0),
m_pTrigEFDiMuNtuple (NULL),
m_iVKVVertexFitter("Trk::TrkVKalVrtFitter")
//  m_iVKVVertexFitter("Trk::TrkVKalVrtFitter/VertexFitterTool",this);
{
    
    declareProperty("MuMuMassMin", m_MassMin = 2800.);
    declareProperty("MuMuMassMax", m_MassMax = 4000.);
    declareProperty("ApplyOppositeCharge", m_ApplyOppCharge = false);
    declareProperty("ApplyMuMuMassMax", m_ApplyMassMax = true);
    declareProperty("NtupleName",           m_ntupleName    = "/NTUPLES/EFDIMU/EFDIMU");
    declareProperty("NtupleTitle",          m_ntupleTitle   = "Di Muon");
    declareProperty("doNTuple",             m_doNTuple      = false);
    declareProperty("doTruth",              m_doTruth       = false);
    
    declareMonitoredStdContainer("pT",           mon_muonpT  , AutoClear);
    declareMonitoredStdContainer("eta",          mon_muonEta , AutoClear);
    declareMonitoredStdContainer("phi",          mon_muonPhi , AutoClear);
    declareMonitoredStdContainer("vtx_pT",       mon_VtxPt   , AutoClear);
    declareMonitoredStdContainer("vtx_jpsiMass", mon_jpsiMass, AutoClear);
}

TrigEFDiMuFex::~TrigEFDiMuFex()
{}

HLT::ErrorCode TrigEFDiMuFex::hltInitialize()
{
    msg() << MSG::INFO << "Initializing TrigEFDiMuFex" << endreq;
    
    // Initialize NTuple
    if (m_doNTuple) {
        msg()<< MSG::WARNING << "Ntuple booking not supported in TrigEFDiMuFex yet " << m_ntupleName << endreq;
        m_doNTuple = false;
    }
    
    if (m_doNTuple)
    {
        return HLT::BAD_JOB_SETUP; // FIXME, enable the ntuple making again
        //m_pTrigEFDiMuNtuple = new TrigEFDiMuNtuple(this, ntupleSvc());
        StatusCode SCstatus = StatusCode::FAILURE; // m_pTrigEFDiMuNtuple->book(m_ntupleName, m_ntupleTitle);
        if (SCstatus.isFailure() || m_pTrigEFDiMuNtuple == NULL)
        {
            msg()<< MSG::ERROR << "Could not book NTuple " << m_ntupleName << endreq;
            return HLT::BAD_JOB_SETUP;
        }
    }
    
    m_GotFitTool=true;
    // Get the VKalVrt vertex fitting tool from ToolSvc
    if ( m_iVKVVertexFitter.retrieve().isFailure() )
    {
        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Fail to retrieve tool "<< m_iVKVVertexFitter<<endreq;
        m_GotFitTool=false;
    }
    else if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Retrieved tool " << m_iVKVVertexFitter<<endreq;
    
    return HLT::OK;
}

HLT::ErrorCode TrigEFDiMuFex::hltFinalize()
{
    msg() << MSG::INFO << "Finalizing TrigEFDiMuFex" << endreq;
    return HLT::OK;
}

HLT::ErrorCode TrigEFDiMuFex::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE)
{
    if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Executing TrigEFDiMuFex <--*-->"<<endreq;
    
    //    const EventInfo* pEventInfo;
    eRunNumber=0;
    eEventNumber=0;
    
    m_pt.clear();
    m_cotTh.clear();
    m_eta.clear();
    m_phi.clear();
    m_m.clear();
    m_charge.clear();
    m_type.clear();
    
    m_pStoreGate = store();
    
    // Retrieve event info
    int IdRun   = 0;
    int IdEvent = 0;
    
    // JW - Try to get the xAOD event info
    const EventInfo* pEventInfo(0);
    const xAOD::EventInfo *evtInfo(0);
    if ( store()->retrieve(evtInfo).isFailure() ) {
        if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get xAOD::EventInfo " << endreq;
        // now try the old event ifo
        if ( store()->retrieve(pEventInfo).isFailure() ) {
            if ( msgLvl() <= MSG::DEBUG ) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
            //mon_Errors.push_back( ERROR_No_EventInfo );
        } else {
            IdRun   = pEventInfo->event_ID()->run_number();
            IdEvent = pEventInfo->event_ID()->event_number();
            if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << IdRun << " Event " << IdEvent <<  endreq;
        }// found old event info
    }else { // found the xAOD event info
        if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << " Run " << evtInfo->runNumber()
            << " Event " << evtInfo->eventNumber() << endreq;
        IdRun   = evtInfo->runNumber();
        IdEvent = evtInfo->eventNumber();
    } // get event ifo
    eRunNumber=IdRun;
    eEventNumber=IdEvent;

    
    HLT::ErrorCode status;
    
    if (m_doNTuple)
    {
        StatusCode statusfill = StatusCode::FAILURE; // nm_pTrigEFDiMuNtuple->fillEvent(eEventNumber,eRunNumber);
        if (statusfill.isFailure())
        {
            msg() << MSG::ERROR << " Cannot fill NTuple event # " << endreq;
            return HLT::BAD_JOB_SETUP;
        }
    }
    
    if (m_doTruth)
    {
        if (doMuTruth() != HLT::OK)
            msg() << MSG::ERROR  << "The doMuTruth status is not Ok" << endreq;
    }
    
    //Get vector of pointers to all TrigMuonEFInfo objects linked to the outputTE.
    // Don't let the name confuse you, TrigMuonEFInfo is filled by both TrigMuGirl or TrigMuonEF
    //const TrigMuonEFInfoContainer* trackCont=0;
    //std::vector<const TrigMuonEFInfoContainer*> vectorOfTrigMuonEF;
    // get the MuonEFInfo objects
    
    std::vector<const xAOD::MuonContainer*> muonContainerEF1;
    const xAOD::MuonContainer* xAODTrackCont(0);

    status=getFeatures(outputTE, muonContainerEF1);
    if (status != HLT::OK || muonContainerEF1.size()==0)
    {
        if (msgLvl() <= MSG::DEBUG)msg()<<MSG::DEBUG<<"Could not retieve vector of xAOD::MuonContainer"<<endreq;
        return HLT::MISSING_FEATURE;
    }
    msg()<<MSG::DEBUG<<"size of muonContainerEF1 container "<<muonContainerEF1.size()<<endreq;
    for (unsigned int i=0; i<muonContainerEF1.size(); i++)
    {
        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Element "<<i<<" of vector of xAOD::MuonContainer containers  "<<endreq;
        // Get first (and only) RoI:
        xAODTrackCont = muonContainerEF1[i];
        if(xAODTrackCont==0)
        {
            // JK this should be WARNING
            msg()<<MSG::DEBUG << "Retrieval of xAODTrackCont container from vector failed"<<endreq;
            return  HLT::MISSING_FEATURE;
        }
        else
            if(xAODTrackCont->size()==0)
            {
                msg()<<MSG::DEBUG << "xAODTrackCont container is empty"<<endreq;
                return  HLT::OK;
            }
            else if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"xAODTrackCont container OK with size "<<xAODTrackCont->size()<<endreq;
    }
    
    int nMuons=0;
    // ERA identification of the input muonType 2=MuGirl 1=MuonEF
    //jk not sure why but this doesn't work
    // int muonType=(*( ((*(trackCont->begin()))->TrackContainer())->begin()))->MuonType();
    //jk if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Track container type:"<<muonType<<endreq;
    int muonType=-99;
    
    //    // next loop will prepare the muons data for matching (includes ntuple)
    //    for (TrigMuonEFInfoContainer::const_iterator tr=trackCont->begin();tr != trackCont->end(); tr++)
    //    {
    //        TrigMuonEFInfo* eInfo = (*tr);
    //        TrigMuonEFInfoTrackContainer* trI=eInfo->TrackContainer();
    //
    //        // JK I think this is a problem
    //        //     if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Track container type:"<<(*(trI->begin()))->MuonType()<<endreq;
    //        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Track container size: "<< trI->size() <<endreq;
    //        for (TrigMuonEFInfoTrackContainer::const_iterator  Ir= trI->begin();Ir != trI->end(); Ir++)
    //        {
    //
    //            TrigMuonEFInfoTrack* muonInfo = (*Ir);
    //            // JK put in protection combined track
    //            if (!(muonInfo->hasCombinedTrack())) {
    //                if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"muonInfo has no combined track! " <<endreq;
    //                continue;
    //            }
    //            TrigMuonEFCbTrack* trC = muonInfo->CombinedTrack(); //also TrigMuGirl track
    //
    //            m_type.push_back(muonInfo->MuonType());
    //            if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "muon type, Track pT " << muonInfo->MuonType() << " " << trC->pt() << endreq;
    //            m_pt.push_back(trC->pt());
    //            m_cotTh.push_back(trC->cotTh());
    //            m_eta.push_back(trC->eta());
    //            m_phi.push_back(trC->phi());
    //            m_m.push_back(trC->m());
    //            m_charge.push_back(trC->charge());
    //            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"muon with type, pT, phi "<<muonInfo->MuonType()<<" "<<trC->pt()<<" "<<trC->phi()<<endreq;
    //            // JK set muontype here
    //            muonType = muonInfo->MuonType();
    //            if(m_doNTuple)
    //            {
    //                ++nMuons;
    //                // tan(x/2)=sqrt(1+cot**2)-cot
    //                double eta=-log(sqrt(1+(trC->cotTh())*(trC->cotTh()))-(trC->cotTh()));
    //                StatusCode statusfill = m_pTrigEFDiMuNtuple->fillReco(nMuons,trC->phi(),trC->cotTh(),eta,trC->pt(), (int)trC->charge(), muonInfo->MuonType());
    //
    //                if (statusfill.isFailure())
    //                {
    //                    msg()<<MSG::ERROR<<"Cannot fill NTuple Reco"<<endreq;
    //                    return HLT::BAD_JOB_SETUP;
    //                }
    //            }
    //        } // end loop over track container
    //    } //end loop over MuonEFInfoContainer
 
    // next loop will prepare the muons data for matching (includes ntuple)
    for (xAOD::MuonContainer::const_iterator tr=xAODTrackCont->begin();tr != xAODTrackCont->end(); tr++)
    {
        const xAOD::Muon * muon = *tr;
        if (!muon) {
            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"null pointer muon! " <<endreq;
            continue;
        }
        
        // make sure we have a combined track:
        const xAOD::TrackParticle* tpCombMuon = muon->trackParticle( xAOD::Muon::CombinedTrackParticle);
        if (!tpCombMuon) {
            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"muonInfo has no combined track! " <<endreq;
            continue;
        }
    
        
        
        // TrigMuonEFCbTrack* trC = muonInfo->CombinedTrack(); //also TrigMuGirl track
        
        m_type.push_back(muon->muonType());
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "muon type, Track pT " << muon->muonType() << " " << muon->pt() << endreq;
        m_pt.push_back(muon->pt());
        double cotTh = tan(muon->p4().Theta());
        if (cotTh !=0) cotTh = 1./cotTh;
        m_cotTh.push_back(cotTh); // FIXME muon->cotTh());
        m_eta.push_back(muon->eta());
        m_phi.push_back(muon->phi());
        m_m.push_back(muon->p4().M());
        m_charge.push_back(tpCombMuon->charge()); // FIXME is this the best approach to charge determination
        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"muon with type, pT, phi "<<muon->muonType()<<" "<<muon->pt()<<" "<<muon->phi()<<endreq;
        // JK set muontype here
        muonType = muon->muonType();
        if(m_doNTuple)
        {
            ++nMuons;
            // tan(x/2)=sqrt(1+cot**2)-cot
            StatusCode statusfill = StatusCode::FAILURE; // m_pTrigEFDiMuNtuple->fillReco(nMuons,muon->phi(),cotTh,muon->eta(),muon->pt(), (int)tpCombMuon->charge(), muon->muonType());
            
            if (statusfill.isFailure())
            {
                msg()<<MSG::ERROR<<"Cannot fill NTuple Reco"<<endreq;
                return HLT::BAD_JOB_SETUP;
            }
        }
    } //end loop over MuonEFInfoContainer

    
    // const xAOD::TrackParticle* tpIDtrack = muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle);

    std::vector<const xAOD::TrackParticleContainer*> vecxAODIDTrakCont;

    //parameters for ID track association
    const xAOD::TrackParticleContainer* idtrackxAODCont=0;
    // std::vector<const Rec::TrackParticleContainer*> vecIDTrakCont;
    bool IDCont=true;
    int Mu1=-1;
    int Mu2=-1;
    // additional parameters for MuonEF
    std::vector<const Trk::Track*> IdTracks;
    std::vector<const xAOD::MuonContainer*> xAODMuidTracksEF;
    // prepare the needed containers for track association
    // 2=TrigMuGirl 1=TrigMuonEF
    if (muonType==2 || muonType==3) status = getFeatures(inputTE, vecxAODIDTrakCont, "eGirlID"); // FIXME CHECK MUON TYPE ENUMS
    else
    {
        status = getFeatures(inputTE, xAODMuidTracksEF);
        if (status!= HLT::OK) {
            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Unable to getFeature for xAODMuidTracksEF"<<endreq;
        }
        else {
            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"xAODMuidTracksEF found size()= "<<xAODMuidTracksEF.size()<<endreq;
        }
        const xAOD::MuonContainer* xAODMuIdTracks;
        if (xAODMuidTracksEF.size() !=1) msg()<<MSG::DEBUG<<"xAODMuidTracksEF conatiner size not 1!!"<< endreq;
        else
        {
            xAODMuIdTracks = xAODMuidTracksEF.front();
            for (xAOD::MuonContainer::const_iterator tr = xAODMuIdTracks->begin();tr != xAODMuIdTracks->end(); tr++){
                //	 if (msgLvl() <= MSG::DEBUG)   msg()<<MSG::DEBUG<<"Muon 1 code is < "<<(*tr)->muonCode()<<" >"<<endreq;
                if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"track code is "<</*(*tr)->muonCode()*/ "FIXME - unknown" <<endreq;
                const xAOD::TrackParticle* tpIDtrack = (*tr)->trackParticle( xAOD::Muon::InnerDetectorTrackParticle);
                if (!tpIDtrack) {
                    if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"muon in loop has no idtp " <<endreq;
                    continue;
                } // ok
                const Trk::Perigee* perigee = &tpIDtrack->perigeeParameters();

                // const Trk::Perigee* perigee=(*tr)->indetTrack()->perigeeParameters();
                if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"InDet track muon pointer, perigeeParameters pointer pt/eta/phi "<<tpIDtrack<<" "<<perigee<<" "<<perigee->pT()<<" / "<<perigee->eta()<<" / "<<perigee->parameters()[Trk::phi0]<< endreq;
                IdTracks.push_back(tpIDtrack->track());
            }
        }
        status = getFeatures(outputTE, vecxAODIDTrakCont);
    }
    if (status!= HLT::OK) {
        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Unable to getFeature for vecxAODIDTrakCont"<<endreq;
    }  else {
        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<" vecxAODIDTrakCont found size()= "<<vecxAODIDTrakCont.size()<<endreq;
        if (vecxAODIDTrakCont.size()!=1)
        {
            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"vecxAODIDTrakCont conatiner size not 1!!"<<endreq;
            IDCont=false;
        }
        else idtrackxAODCont  = vecxAODIDTrakCont.front();
    }
    

    
    //    //parameters for ID track association
    //    const Rec::TrackParticleContainer* idtrackCont=0;
    //    std::vector<const Rec::TrackParticleContainer*> vecIDTrakCont;
    //    bool IDCont=true;
    //    int Mu1=-1;
    //    int Mu2=-1;
    //    // additional parameters for MuonEF
    //    std::vector<const Trk::Track*> IdTracks;
    //    std::vector<const MuidTrackContainer*> MuidTracksEF;
    //    // prepare the needed containers for track association
    //    // 2=TrigMuGirl 1=TrigMuonEF
    //    if (muonType==2 || muonType==3) status = getFeatures(inputTE, vecIDTrakCont, "eGirlID");
    //    else
    //    {
    //        status = getFeatures(inputTE, MuidTracksEF);
    //        if (status!= HLT::OK) {
    //            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Unable to getFeature for MuidTracks"<<endreq;
    //        }
    //        else {
    //            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"MuidTracks found size()= "<<MuidTracksEF.size()<<endreq;
    //        }
    //        const MuidTrackContainer* MuIdTracks;
    //        if (MuidTracksEF.size() !=1) msg()<<MSG::DEBUG<<"MuidTrack conatiner size not 1!!"<< endreq;
    //        else
    //        {
    //            MuIdTracks = MuidTracksEF.front();
    //            for (MuidTrackContainer::const_iterator tr = MuIdTracks->begin();tr != MuIdTracks->end(); tr++){
    //                //	 if (msgLvl() <= MSG::DEBUG)   msg()<<MSG::DEBUG<<"Muon 1 code is < "<<(*tr)->muonCode()<<" >"<<endreq;
    //                if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"track code is "<<(*tr)->muonCode()<<endreq;
    //                const Trk::Perigee* perigee=(*tr)->indetTrack()->perigeeParameters();
    //                if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"InDet track muon pointer, perigeeParameters pointer pt/eta/phi "<<(*tr)->indetTrack()<<" "<<perigee<<" "<<perigee->pT()<<" / "<<perigee->eta()<<" / "<<perigee->parameters()[Trk::phi0]<< endreq;
    //                IdTracks.push_back((*tr)->indetTrack());
    //            }
    //        }
    //        status = getFeatures(outputTE, vecIDTrakCont);
    //    }
    //    if (status!= HLT::OK) {
    //        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Unable to getFeature for vecIDTrakCon"<<endreq;
    //    }  else {
    //        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<" vecIDTrakCon found size()= "<<vecIDTrakCont.size()<<endreq;
    //        if (vecIDTrakCont.size()!=1)
    //        {
    //            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"vecIDTrakCon conatiner size not 1!!"<<endreq;
    //            IDCont=false;
    //        }
    //        else idtrackCont  = vecIDTrakCont.front();
    //    }
    
    // initialization of invariat mass and monitored parameters
    jpsiMass=-99;
    jpsiE=-99;
    NumberOfMuons=-99;
    NumberOfMuons=m_pt.size();

    //    pOut=new TrigEFBphysContainer();
    mTrigBphysColl    = new xAOD::TrigBphysContainer();
    //mTrigBphysAuxColl = new xAOD::TrigBphysAuxContainer;
    xAOD::TrigBphysAuxContainer xAODTrigBphysAuxColl;
    mTrigBphysColl->setStore(&xAODTrigBphysAuxColl);

    // loop over all muon candidates
    // looking at all opposite charge muon pairs
    if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Number of muons: "<<NumberOfMuons<<endreq;
    if (NumberOfMuons>1)
        for (int index=0; index< NumberOfMuons-1; index++)
        {
            p1[1]=fabs(m_pt[index])*cos( m_phi[index]);
            p1[2]=fabs(m_pt[index])*sin( m_phi[index]);
            p1[3]=fabs(m_pt[index])*m_cotTh[index];
            p1[0]=sqrt(p1[1]*p1[1]+p1[2]*p1[2]+p1[3]*p1[3]+m_m[index] * m_m[index]);
            if (msgLvl() <= MSG::VERBOSE) msg()<<MSG::VERBOSE<<"mu1 pT, phi, cotTh "<<m_pt[index]<<" "<<m_phi[index]<<" "<<m_cotTh[index]<<endreq;
            //	if (msgLvl() <= MSG::VERBOSE) msg()<<MSG::VERBOSE<<"calculate mass with mu1 px,py,pz,E "<<p1[1]<<" "<<p1[2]<<" "<<p1[3]<<" "<<p1[0]<<endreq;
            
            for (int jndex=index+1; jndex<NumberOfMuons; jndex++)
            {
                if (m_ApplyOppCharge) {
                    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "check for  opposite sign check: " << m_charge[index] << " " << m_charge[jndex] << endreq;
                    if ( m_charge[index] + m_charge[jndex] != 0) {
                        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "failed opp sign check" << endreq;
                        continue;
                    }
                }
                if ( m_type[index] != m_type[jndex]) continue;
                //	if (msgLvl() <= MSG::DEBUG)    msg()<<MSG::DEBUG<<"vec test index= "<<jndex<<" m_pt[index]= "<<m_pt[jndex]<<endreq;
                p2[1]=fabs(m_pt[jndex])*cos( m_phi[jndex]);
                p2[2]=fabs(m_pt[jndex])*sin( m_phi[jndex]);
                p2[3]=fabs(m_pt[jndex])*m_cotTh[jndex];
                p2[0]=sqrt(p2[1]*p2[1]+p2[2]*p2[2]+p2[3]*p2[3]+m_m[jndex]*m_m[jndex]);
                if (msgLvl() <= MSG::VERBOSE) msg()<<MSG::VERBOSE<<" mu2 pT, phi, cotTh "<<m_pt[jndex]<<" "<<m_phi[jndex]<<" "<<m_cotTh[jndex]<<endreq;
                //	   if (msgLvl() <= MSG::VERBOSE) msg()<<MSG::VERBOSE<<" calculate mass with mu2 px,py,pz,E "<<p2[1]<<" "<<p2[2]<<" "<<p2[3]<<" "<<p2[0]<<endreq;
                
                jpsiMass=sqrt((p1[0]+p2[0])*(p1[0]+p2[0])-(p1[1]+p2[1])*(p1[1]+p2[1])-(p1[2]+p2[2])*(p1[2]+p2[2])-(p1[3]+p2[3])*(p1[3]+p2[3]));
                jpsiE=p1[0]+p2[0];
                // pMuPair=new TrigEFBphys(-99, 0, 0, TrigEFBphys::JPSIMUMU, jpsiMass);
                xAOD::TrigBphys* xaodObj = new xAOD::TrigBphys();
                mTrigBphysColl->push_back( xaodObj );
                xaodObj->initialise(-99, 0., 0., xAOD::TrigBphys::JPSIMUMU, jpsiMass, xAOD::TrigBphys::EF );
                
                if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Created TrigBphys with mass "<<jpsiMass<<endreq;
                if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Check IDCont "<<IDCont<< " muonType " << muonType << endreq;
                // match ID tracks
                if (IDCont) // check if there is an ID track container available
                {
                    if (muonType==2 || muonType==3) //TrigMuGirl // FIXME CHECK MUON TYPES
                    {
                        ElementLink<xAOD::TrackParticleContainer> trackEL1(*idtrackxAODCont,index);
                        xaodObj->addTrackParticleLink(trackEL1);
                        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"id track "<<index<<" WAS store in EFBPhys"<<endreq;
                        ElementLink<xAOD::TrackParticleContainer> trackEL2(*idtrackxAODCont,jndex);
                        xaodObj->addTrackParticleLink(trackEL2);
                        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"id track "<<jndex<<" WAS store in EFBPhys"<<endreq;

                        //ERA for vertex fitting
                        Mu1=index;
                        Mu2=jndex;
                    }
                    else {
                        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"look for tracks, IDTracks.size() =  " <<  IdTracks.size()  << " Numberof Muons = " << NumberOfMuons << endreq;
                        
                        //		  if ((int)IdTracks.size() ==  NumberOfMuons)  { // TrigMuonEF
                        // JK Find tracks in TrackParticleContainer to store in EFBphys
                        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"TrackParticle parameters"<<endreq;
                        xAOD::TrackParticleContainer::const_iterator trPart=idtrackxAODCont->begin();
                        bool FoundTrack1=false;
                        bool FoundTrack2=false;
                        for (int itrk=0;trPart!=idtrackxAODCont->end(); ++trPart,++itrk)
                        {
                            // JW EDM const Trk::MeasuredPerigee* trackPerigee=(*trPart)->measuredPerigee();
                            //const Trk::Perigee* trackPerigee=(*trPart)->measuredPerigee();
                            const Trk::Perigee* trackPerigee= &(*trPart)->perigeeParameters();
                            double phi = trackPerigee->parameters()[Trk::phi];
                            double theta = trackPerigee->parameters()[Trk::theta];
                            double px = trackPerigee->momentum()[Trk::px];
                            double py = trackPerigee->momentum()[Trk::py];
                            double pt = sqrt(px*px + py*py);
                            double eta = -std::log(tan(theta/2));
                            if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "track  measuredPerigee pt phi eta " << (*trPart) << " " << trackPerigee << " " << pt << " " <<
                                phi << " " << eta << endreq;
                            const Trk::Track* track = (*trPart)->track();
                            if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "original track pointer " << track << " and perigee " << track->perigeeParameters() <<endreq;
                            if (track == IdTracks[index] && !FoundTrack1)
                            {
                                ElementLink<xAOD::TrackParticleContainer> trackEL(*idtrackxAODCont,itrk);
                                xaodObj->addTrackParticleLink(trackEL);
                                if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG	<< "track matches track 1 , will store in EFBPhys "<< endreq;
                                Mu1=itrk;

                                FoundTrack1=true;
                            }
                            if (track == IdTracks[jndex] && !FoundTrack2)
                            {
                                ElementLink<xAOD::TrackParticleContainer> trackEL(*idtrackxAODCont,itrk);
                                xaodObj->addTrackParticleLink(trackEL);
                                if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG	<< "track matches track 2 , will store in EFBPhys "<< endreq;
                                Mu2=itrk;
                                FoundTrack2=true;
                            }
                        } // for
                        //  } // end if good track info for matching
                    } // end else (muonType==1)
                } // end if IDCont
                // to condition Mu1!=Mu2 was added to protect the vertex fitter
                // should be check in later versions, maybe i can be taken out
                if(m_GotFitTool && (Mu1 !=Mu2)) // check if FitTool is available
                {
                    // ERA vertex fitting
                    // define fit parameters
                    //CLHEP::Hep3Vector appVertex; //JW
                    Amg::Vector3D appVertex(0,0,0);
                    // Trk::Vertex vertex ( appVertex );

                    //std::vector<const Trk::TrackParticleBase*> idmuons;
                    std::vector<const Trk::Track*> idmuons;

                    // fill fit parameter idmuons
                    xAOD::TrackParticleContainer::const_iterator tb=idtrackxAODCont->begin();
                    for (int kndex=0; tb!=idtrackxAODCont->end(); tb++,kndex++)
                    {
                        if (kndex==Mu1) idmuons.push_back( (*tb)->track());
                        if (kndex==Mu2) idmuons.push_back( (*tb)->track());
                    }
                    // to add --> cheack identical tracks
                    // define the fit object
                    m_eVKalVrtFitter=dynamic_cast<Trk::TrkVKalVrtFitter*>(&(*m_iVKVVertexFitter));
                    // initialize the fit object
                    m_eVKalVrtFitter->setDefault();
                    m_eVKalVrtFitter->setRobustness(0);
                    // No total momentum and its covariance matrix
                    m_eVKalVrtFitter->setMomCovCalc(0);
                    //CLHEP::Hep3Vector appVertex_CLHEP;
                    if (!(m_eVKalVrtFitter->VKalVrtFitFast(idmuons,appVertex).isSuccess())){ // FIXME - check this uses the starting point method
                        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Warning from VKaVrt - fast fit failed!"<<MSG::DEBUG<<endreq;
                    }
                    //                    appVertex[0] =appVertex_CLHEP.x(); // JW FIXME -eigen
                    //                    appVertex[1] =appVertex_CLHEP.y(); // JW FIXME
                    //                    appVertex[2] =appVertex_CLHEP.z(); // JW FIXME
                    
                    //Use fast fit result as starting point for full fit
                    m_eVKalVrtFitter->setApproximateVertex(appVertex.x(),appVertex.y(),appVertex.z());
                    // define parameters for full fitt
                    // CLHEP::Hep3Vector finalVertex_CLHEP; // Position of properly fitted vertex
                    // CLHEP::HepLorentzVector momentum_CLHEP;
		    TLorentzVector momentum;
                    Amg::Vector3D finalVertex(0,0,0); // Position of properly fitted vertex

                    
                    long int charge=0;
                    double invariantMass=0, invariantMassError=0, chi2=0;
                    // Fit error matrix and chi2 per track
                    std::vector<double> errorMatrix, chi2PerTrack;
                    std::vector<double> vkPerigee, covPerigee, completeCov;
                    // "True" tracks passing through vertex (theta, phi, 1/R)
                    std::vector< std::vector<double> > trkAtVrt;
                    m_eVKalVrtFitter->setMomCovCalc(1);
                    // do the full fit
                    if(!(m_eVKalVrtFitter->VKalVrtFit(idmuons, finalVertex, momentum, charge, errorMatrix, chi2PerTrack, trkAtVrt, chi2).isSuccess())) {
                        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Vertex fitting - not a goodFit"<<endreq;
                    }
                    else
                    {
                        //                        finalVertex[0] = finalVertex_CLHEP.x(); // JW FIXME
                        //                        finalVertex[1] = finalVertex_CLHEP.y(); // JW FIXME
                        //                        finalVertex[2] = finalVertex_CLHEP.z(); // JW FIXME
                        
                        // monitoring - now only successfuly founded J/Psi
                        // pt of muons [GeV]
                        mon_VtxPt.push_back((m_pt[jndex])*0.001);
                        // mass of jpsi [GeV]
                        mon_jpsiMass.push_back((jpsiMass*0.001));
                        
                        // end of monitoring
                        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Vertex fitting - was ok"<<endreq;
                        std::vector<int> trkIndices;
                        for (int i=0;i<(int)idmuons.size();++i) {trkIndices.push_back(1);}
                        if (!(m_eVKalVrtFitter->VKalGetMassError(trkIndices,invariantMass,invariantMassError).isSuccess())) {
                            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Warning from VKaVrt - cannot calculate uncertainties!"<<endreq;
                        }
                        // if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"mass delta="<<jpsiMass-invariatMass<<endreq;
                        //                        pMuPair->fitmass((float)invariantMass);
                        //                        pMuPair->fitchi2((float)chi2);
                        //                        pMuPair->fitndof(2*(idmuons.size())-3);
                        //                        pMuPair->fitx(finalVertex.x());
                        //                        pMuPair->fity(finalVertex.y());
                        //                        pMuPair->fitz(finalVertex.z());
                        xaodObj->setFitmass     ((float)invariantMass);
                        xaodObj->setFitchi2     ((float)chi2);
                        xaodObj->setFitndof     (2*(idmuons.size())-3);
                        xaodObj->setFitx        (finalVertex.x());
                        xaodObj->setFity        (finalVertex.y());
                        xaodObj->setFitz        (finalVertex.z());

                    }
                    
                }// end vertex fitting
                
                //                pOut->push_back(pMuPair);
                if ((jpsiMass<m_MassMax || (!m_ApplyMassMax)) && jpsiMass>m_MassMin)
                {
                    mon_muonpT .push_back(m_pt[jndex]*0.001);
                    mon_muonEta.push_back(m_eta[jndex]);
                    mon_muonPhi.push_back(m_phi[jndex]);
                }
            } // mu2
        } // mu1
    
    if (m_doNTuple)
    {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Write Ntuple " << endreq;
        StatusCode statusNT = StatusCode::FAILURE; // m_pTrigEFDiMuNtuple->writeRecord();
        if (statusNT.isFailure() || m_pTrigEFDiMuNtuple == NULL)
        {
            msg() << MSG::ERROR << "TrigEFDiMuFex::execute Cannot write NTuple" << endreq;
            return HLT::BAD_JOB_SETUP;
        }
    }
    
    // no need to attached an empty vector  
    if (mTrigBphysColl->size()!=0)
    {
        status= attachFeature(outputTE, mTrigBphysColl, "EFMuPairs");
        if(status != HLT::OK) msg()<<MSG::WARNING<<"Failed to attach xAOD::TrigBphysContainer"<<endreq;
        else {
            if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Successfully attached a xAOD::TrigBphysContainer with size "<<mTrigBphysColl->size()<<endreq;
        }
    }
    else {
        if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"xAOD::TrigBphysContainer is empty so it wasn't attached to TEout"<<endreq;
        delete mTrigBphysColl;
        mTrigBphysColl    =0;
    }
    //  m_pt.clear();
    // m_cotTh.clear();
    //  m_phi.clear();
    //  m_m.clear();
    //  m_charge.clear();
    //  m_type.clear();
    
    return HLT::OK;
}

HLT::ErrorCode TrigEFDiMuFex::doMuTruth()
{
    int nTruthMu = 0;
    const McEventCollection* pMcEventCollection = NULL;
    StatusCode status = m_pStoreGate->retrieve(pMcEventCollection, "TruthEvent");
    if (status.isFailure() || pMcEventCollection == NULL)
    {
        msg() << MSG::DEBUG << " Cannot retrieve McEventCollection " << endreq;
        return HLT::SG_ERROR;
    }
    
	McEventCollection::const_iterator
    itMc = pMcEventCollection->begin(),
    itMc_e = pMcEventCollection->end();
    for (; itMc != itMc_e; ++itMc)
    {
        const HepMC::GenEvent* pGenEvent = *itMc;
        
		HepMC::GenEvent::particle_const_iterator
        itPart = pGenEvent->particles_begin(),
        itPart_e = pGenEvent->particles_end();
        for (; itPart != itPart_e; ++itPart)
        {
            const HepMC::GenParticle* pPart = *itPart;
            
            if (pPart->pdg_id() == -13 || pPart->pdg_id() == 13)
            {
                int parent_pdg = 0;
                if (pPart->production_vertex() != NULL)
                {
					HepMC::GenVertex::particle_iterator
                    itVxPart = pPart->production_vertex()->particles_begin(HepMC::parents),
                    itVxPart_e = pPart->production_vertex()->particles_end(HepMC::parents);
                    for (; itVxPart != itVxPart_e; ++itVxPart)
                    {
                        parent_pdg = (*itVxPart)->pdg_id();
                    }
                }
                
                ++nTruthMu;
                if (m_doNTuple)
                {
                    double cot=1/tan(2*atan(exp(-pPart->momentum().eta())));
                    
                    status = StatusCode::FAILURE; //m_pTrigEFDiMuNtuple->fillTruth(nTruthMu,
                                                  //          pPart->momentum().phi(),
                                                  //          cot,
                                                  //          pPart->momentum().eta(),
                                                  //          pPart->momentum().perp(),
                                                  //          pPart->pdg_id(),
                                                  //          parent_pdg);
                    if (status.isFailure())
                    {
                        msg() << MSG::ERROR << " Cannot fill NTuple TRUTH " << endreq;
                        return HLT::BAD_JOB_SETUP;
                    }
                }
            }
        }
    }
    //}
    
	return HLT::OK;
}


