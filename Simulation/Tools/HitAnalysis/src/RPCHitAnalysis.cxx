/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCHitAnalysis.h"

// Section of includes for the RPC of the Muon Spectrometer tests
#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/RPCSimHit.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "TH1.h"
#include "TTree.h"
#include "TString.h"


#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>
#include <stdio.h>

RPCHitAnalysis::RPCHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   , h_hits_x(0)
   , h_hits_y(0)
   , h_hits_z(0)
   , h_hits_r(0)
   , h_xy(0)
   , h_zr(0)
   , h_hits_eta(0)
   , h_hits_phi(0)
   , h_hits_lx(0)
   , h_hits_ly(0)
   , h_hits_lz(0)
   , h_hits_time(0)
   , h_hits_edep(0)
   , h_hits_kine(0)
   , h_hits_step(0)
   , m_hits_x(0)
   , m_hits_y(0)
   , m_hits_z(0)
   , m_hits_r(0)
   , m_hits_eta(0)
   , m_hits_phi(0)
   , m_hits_lx(0)
   , m_hits_ly(0)
   , m_hits_lz(0)
   , m_hits_time(0)
   , m_hits_edep(0)
   , m_hits_kine(0)
   , m_hits_step(0)
   , m_tree(0)
   , m_ntupleFileName("/RPCHitAnalysis/ntuple/")
   , m_path("/RPCHitAnalysis/histos/")
   , m_thistSvc("THistSvc", name)
{
  declareProperty("NtupleFileName", m_ntupleFileName); 
  declareProperty("HistPath", m_path); 

}

StatusCode RPCHitAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing RPCHitAnalysis" );

  // Grab the Ntuple and histogramming service for the tree
  CHECK(m_thistSvc.retrieve());

  /** Histograms**/
  h_hits_x = new TH1D("h_hits_rpc_x","hits_x", 100,-11000, 11000);
  h_hits_x->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_x->GetName(), h_hits_x));

  h_hits_y = new TH1D("h_hits_rpc_y", "hits_y", 100,-11000,11000);
  h_hits_y->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_y->GetName(), h_hits_y));

  h_hits_z = new TH1D("h_hits_rpc_z", "hits_z", 100,-12500, 12500);
  h_hits_z->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_z->GetName(), h_hits_z));

  h_hits_r = new TH1D("h_hits_rpc_r", "hits_r", 100,6000,14000);
  h_hits_r->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_r->GetName(), h_hits_r));

  h_xy = new TH2D("h_rpc_xy", "xy", 100,-11000.,11000.,100, -11000., 11000.);
  h_xy->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+h_xy->GetName(), h_xy));

  h_zr = new TH2D("m_rpc_zr", "zr", 100,-12500.,12500.,100, 6000., 14000.);
  h_zr->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+h_zr->GetName(), h_zr));

  h_hits_eta = new TH1D("h_hits_rpc_eta", "hits_eta", 100,-1.5,1.5);
  h_hits_eta->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_eta->GetName(), h_hits_eta));

  h_hits_phi = new TH1D("h_hits_rpc_phi", "hits_phi", 100,-3.2,3.2);
  h_hits_phi->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_phi->GetName(), h_hits_phi));

  h_hits_lx = new TH1D("h_hits_rpc_lx","hits_lx", 100,-10, 10);
  h_hits_lx->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_lx->GetName(), h_hits_lx));

  h_hits_ly = new TH1D("h_hits_rpc_ly", "hits_ly", 100,-1500,1500);
  h_hits_ly->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_ly->GetName(), h_hits_ly));

  h_hits_lz = new TH1D("h_hits_rpc_lz", "hits_lz", 100,-600,600);
  h_hits_lz->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_lz->GetName(), h_hits_lz));


  h_hits_time = new TH1D("h_hits_rpc_time","hits_time", 100,0, 120);
  h_hits_time->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_time->GetName(), h_hits_time));

  h_hits_edep = new TH1D("h_hits_rpc_edep", "hits_edep", 100,0,0.15);
  h_hits_edep->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_edep->GetName(), h_hits_edep));

  h_hits_kine = new TH1D("h_hits_rpc_kine", "hits_kine", 100,0,500);
  h_hits_kine->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_kine->GetName(), h_hits_kine));

  h_hits_step = new TH1D("h_hits_rpc_step", "hits_step", 100,0,25);
  h_hits_step->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_hits_step->GetName(), h_hits_step));


  m_tree= new TTree("NtupleRPCHitAnalysis","RPCHitAna");
  std::string fullNtupleName =  "/"+m_ntupleFileName+"/";
  CHECK(m_thistSvc->regTree(fullNtupleName,m_tree));
  
    
    
  /** now add branches and leaves to the tree */
  if (m_tree){
    m_tree->Branch("x", &m_hits_x);
    m_tree->Branch("y", &m_hits_y);
    m_tree->Branch("z", &m_hits_z);
    m_tree->Branch("r", &m_hits_r);
    m_tree->Branch("eta", &m_hits_eta);
    m_tree->Branch("phi", &m_hits_phi);
    m_tree->Branch("lx", &m_hits_lx);
    m_tree->Branch("ly", &m_hits_ly);
    m_tree->Branch("lz", &m_hits_lz);
    m_tree->Branch("time", &m_hits_time);
    m_tree->Branch("edep", &m_hits_edep);
    m_tree->Branch("kine", &m_hits_kine);
    m_tree->Branch("step", &m_hits_step);
  }else{
    ATH_MSG_ERROR("No tree found!");
  }
  
  return StatusCode::SUCCESS;
}		 



StatusCode RPCHitAnalysis::execute() {
  ATH_MSG_DEBUG( "In RPCHitAnalysis::execute()" );

  m_hits_x->clear();
  m_hits_y->clear();
  m_hits_z->clear();
  m_hits_r->clear();
  m_hits_eta->clear();
  m_hits_phi->clear();
  m_hits_lx->clear();
  m_hits_ly->clear();
  m_hits_lz->clear();
  m_hits_time->clear();
  m_hits_edep->clear();
  m_hits_kine->clear();
  m_hits_step->clear();
  
  const DataHandle<RPCSimHitCollection> rpc_container;
  if (evtStore()->retrieve(rpc_container, "RPC_Hits" )==StatusCode::SUCCESS) {
    for(RPCSimHitCollection::const_iterator i_hit = rpc_container->begin(); 
	i_hit != rpc_container->end();++i_hit){
      //RPCSimHitCollection::const_iterator i_hit;
      //for(auto i_hit : *rpc_container){
      GeoRPCHit ghit(*i_hit);
      if(!ghit) continue;
      Amg::Vector3D p = ghit.getGlobalPosition();
      h_hits_x->Fill(p.x());
      h_hits_y->Fill(p.y());
      h_hits_z->Fill(p.z());
      h_hits_r->Fill(p.perp());
      h_xy->Fill(p.x(), p.y());
      h_zr->Fill(p.z(),p.perp());
      h_hits_eta->Fill(p.eta());
      h_hits_phi->Fill(p.phi());
      h_hits_lx->Fill( (*i_hit).localPosition().x());
      h_hits_ly->Fill( (*i_hit).localPosition().y());
      h_hits_lz->Fill( (*i_hit).localPosition().z());
      h_hits_edep->Fill((*i_hit).energyDeposit());
      h_hits_time->Fill((*i_hit).globalTime());
      h_hits_step->Fill((*i_hit).stepLength());
      h_hits_kine->Fill((*i_hit).kineticEnergy());
      
      m_hits_x->push_back(p.x());
      m_hits_y->push_back(p.y());
      m_hits_z->push_back(p.z());
      m_hits_r->push_back(p.perp());
      m_hits_eta->push_back(p.eta());
      m_hits_phi->push_back(p.phi());
      m_hits_lx->push_back( (*i_hit).localPosition().x());
      m_hits_ly->push_back( (*i_hit).localPosition().y());
      m_hits_lz->push_back( (*i_hit).localPosition().z());
      m_hits_edep->push_back((*i_hit).energyDeposit());
      m_hits_time->push_back((*i_hit).globalTime());
      m_hits_step->push_back((*i_hit).stepLength());
      m_hits_kine->push_back((*i_hit).kineticEnergy());
    }
  } // End while hits
  
  
  if (m_tree) m_tree->Fill();  
  return StatusCode::SUCCESS;
}
