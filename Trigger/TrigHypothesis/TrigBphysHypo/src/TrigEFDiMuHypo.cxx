/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    Authors: E. Reinherz-Aronis, A. Kreisel
             This hypo is set to work after TrigEFDiMuFex FexAlgo
***************************************************************************/
#include "TrigEFDiMuHypo.h"

//for the event info
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigSteeringEvent/TrigPassBits.h"
#include "TrigNavigation/Navigation.h"

// additions of xAOD objects
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
//#include "xAODTrigBphys/TrigBphysAuxContainer.h"

TrigEFDiMuHypo::TrigEFDiMuHypo(const std::string& name, ISvcLocator* pSvcLocator) :
  HLT::HypoAlgo(name, pSvcLocator),
  m_pStoreGate(NULL)
{
  declareProperty("MuMuMassMin", m_MassMin = 2800.);
  declareProperty("MuMuMassMax", m_MassMax = 4000.);
  declareProperty("ApplyMuMuMassMax", m_ApplyMassMax = true);
  declareProperty("ApplyOppositeCharge", m_ApplyOppCharge = true);
  declareProperty("AcceptAll",   m_acceptAll = false);
  declareProperty("UseFittedMass", m_useFitMass=true);

  declareMonitoredStdContainer("Mass",    mon_jpsiMass      , AutoClear);
  declareMonitoredStdContainer("FitMass", mon_jpsiFitMass   , AutoClear);
  declareMonitoredStdContainer("MassCut", mon_jpsiMassCut   , AutoClear);
    //declareMonitoredStdContainer("FitMass", mon_jpsiEta       , AutoClear);
    //declareMonitoredStdContainer("MassCut", mon_jpsiPhi       , AutoClear);
}

TrigEFDiMuHypo::~TrigEFDiMuHypo()
{}

HLT::ErrorCode TrigEFDiMuHypo::hltInitialize()
{
  msg()<<MSG::INFO<<"Initializing TrigEFDiMuHypo"<< endreq;
  return HLT::OK;
}

HLT::ErrorCode TrigEFDiMuHypo::hltFinalize()
{
  msg() << MSG::INFO << "Finalizing TrigEFDiMuHypo" << endreq;
  return HLT::OK;
}

HLT::ErrorCode TrigEFDiMuHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
  if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<< "Executing TrigEFDiMuHypo <--*-->"<<endreq;

//  const EventInfo* pEventInfo;
//  eRunNumber=0;
//  eEventNumber=0;
//
//  m_pStoreGate = store();
//
//  if ( m_pStoreGate->retrieve(pEventInfo).isFailure() ) {
//    if (msgLvl() <= MSG::DEBUG) msg()  << MSG::DEBUG << "Failed to get EventInfo " << endreq;
//  } else
//  {
//    eRunNumber   = pEventInfo->event_ID()->run_number();
//    eEventNumber = pEventInfo->event_ID()->event_number();
//    if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"Run "<<eRunNumber<<" Event "<<eEventNumber<<endreq;
//  }

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

    
  if (m_acceptAll)
  {
    pass = true;
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Accept property is set: taking all the events" << endreq;
    return HLT::OK;
  }

  if(msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"in execute()"<<endreq;

  HLT::ErrorCode status;

  bool result = false;
    // Some debug output:
  if(msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG<<"outputTE->ID(): "<<outputTE->getId()<<endreq;

    // Get vector of pointers to all TrigMuonEF objects linked to the outputTE
    //   by label "EFMuPair":
    //const TrigEFBphysContainer* vectorOfMuPairs=NULL;
    const xAOD::TrigBphysContainer* xAODVectorOfMuPairs(0);
  status = getFeature(outputTE, xAODVectorOfMuPairs, "EFMuPairs");

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Returned from getFeature" << endreq;
  if(status!=HLT::OK || xAODVectorOfMuPairs==NULL)
  {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "no TrigMuPair Feature found" << endreq;
    return status;
  }
  else {
    if (msgLvl() <= MSG::DEBUG) msg()<<MSG::DEBUG <<"TrigMuPair Feature found with "<<xAODVectorOfMuPairs->size()<<" TrigBphys particles"<<endreq;
  }

  // initialize the variables that are going to be used
  TrigPassBits *bits = HLT::makeTrigPassBits(xAODVectorOfMuPairs);

  for(xAOD::TrigBphysContainer::const_iterator pairIt = xAODVectorOfMuPairs->begin(); pairIt != xAODVectorOfMuPairs->end(); ++pairIt)
  {
    bool passMass=false;
    bool passOppCharge=false;
    msg()<<MSG::DEBUG<<"mass="<<(*pairIt)->mass()<<endreq;
    msg()<<MSG::DEBUG<<"fitmass="<<(*pairIt)->fitmass()<<endreq;
  //  msg()<<MSG::DEBUG<<"fitchi2="<<(*pairIt)->fitchi2()<<endreq;
  //  msg()<<MSG::DEBUG<<"fitx="<<(*pairIt)->fitx()<<endreq;

  // use the mass() as a fallback in the case that the fit didn't work
    if (m_useFitMass && ((*pairIt)->fitmass()!=-99) )
    {
      mon_jpsiMassCut.push_back(((*pairIt)->fitmass())*0.001); // monitoring
      jpsiMassCutAlg = (*pairIt)->fitmass();
    }
    else{ mon_jpsiMassCut.push_back(((*pairIt)->mass())*0.001); // monitoring
      jpsiMassCutAlg = (*pairIt)->mass();
    }

    if ( m_MassMin < jpsiMassCutAlg && (jpsiMassCutAlg < m_MassMax || (!m_ApplyMassMax)))
    {
      passMass=true;
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "pass mass cut with mass "  << jpsiMassCutAlg <<endreq;
    }
    else jpsiMassCutAlg=-99;

  // JK check tracks, for debugging only
      // const ElementLinkVector<xAOD::TrackParticleContainer> trackVector = (*pairIt)->trackParticleLinks();
      const std::vector<ElementLink<xAOD::TrackParticleContainer> > trackVector = (*pairIt)->trackParticleLinks();
      
    if (trackVector.size() != 0) {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " got track vector size: " << trackVector.size() << endreq;

      //check that number of tracks is 2
      if(trackVector.size()!=2) {
        msg() << MSG::WARNING << "Number of trigger tracks in J/psi vertex differs from 2 " << endreq;
        continue;
      }

      // check broken ElementLinks
      if( !trackVector.at(0).isValid() || !trackVector.at(1).isValid() ) {
        msg() << MSG::WARNING << "Broken ElementLink." << endreq;
        return HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::USERDEF_1);
      }

      std::vector<ElementLink<xAOD::TrackParticleContainer> >::const_iterator trkIt=trackVector.begin();
      std::vector<float> qOverP;
      for (int itrk=0 ; trkIt!= trackVector.end(); ++itrk, ++trkIt)
      {
          //JW EDM        const Trk::MeasuredPerigee* trackPerigee=(*(*trkIt))->measuredPerigee();
          //const Trk::Perigee* trackPerigee=(*(*trkIt))->measuredPerigee();
          const Trk::Perigee* trackPerigee= &(*(*trkIt))->perigeeParameters();
          
        if (msgLvl() <= MSG::VERBOSE) msg() << MSG::VERBOSE << "track, iterator, pointer " << itrk << " " << *trkIt << " " << *(*trkIt) << endreq;
        double phi = trackPerigee->parameters()[Trk::phi];
        double theta = trackPerigee->parameters()[Trk::theta];
        double px = trackPerigee->momentum()[Trk::px];
        double py = trackPerigee->momentum()[Trk::py];
        double pt = sqrt(px*px + py*py);
        qOverP.push_back(pt *  trackPerigee->parameters()[Trk::qOverP]);
        double eta = -std::log(tan(theta/2));
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "track " << itrk << " pt phi eta " << pt << " " <<
              phi << " " << eta << endreq;
      }
    // end JK debug printout
      if (m_ApplyOppCharge) {
        if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "apply opp charge cut: qOverP " << qOverP[0] << "  " << qOverP[1] << endreq;
        if ((qOverP[0] * qOverP[1]) < 0.) passOppCharge = true;
      }
    } else {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " no track vector!!! "  << endreq;
    }

    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " passMass, passOppCharge, ApplyOppCharge "  << passMass << passOppCharge << m_ApplyOppCharge << endreq;
    if (passMass ) {
      if (m_ApplyOppCharge) {
        if (passOppCharge) {
          result=true;
          HLT::markPassing(bits, *pairIt, xAODVectorOfMuPairs); // store decision per object
        }
      } else {
        result=true;
        HLT::markPassing(bits, *pairIt, xAODVectorOfMuPairs); // store decision per object
      }
    }

    // for monitoring
    if (!m_ApplyOppCharge || passOppCharge) {
      mon_jpsiMass.push_back(((*pairIt)->mass())*0.001); // monitoring
      mon_jpsiFitMass.push_back(((*pairIt)->fitmass())*0.001); // monitoring
    }
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "end of this Bphys, result is "  << result << endreq;
  } // end loop over EFBphys

  // store result
  if ( attachBits(outputTE, bits) != HLT::OK ) {
    msg() << MSG::ERROR << "Problem attaching TrigPassBits! " << endreq;
  }

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "end of routine, result is "  << result << endreq;
  pass = result;
  return HLT::OK;

}
