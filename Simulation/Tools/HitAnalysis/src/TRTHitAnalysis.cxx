/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTHitAnalysis.h"

// Section of includes for TRT tests
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "GeoAdaptors/GeoTRTUncompressedHit.h"

#include "TH1.h"
#include "TTree.h"
#include "TString.h"


#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

TRTHitAnalysis::TRTHitAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator)
   , h_TRT_x(0)
   , h_TRT_y(0)
   , h_TRT_z(0)
   , h_TRT_r(0)
   , h_TRT_xy(0)
   , h_TRT_zr(0)
   , h_TRT_time_photons(0)
   , h_TRT_time_nonphotons(0)
   , h_TRT_edep_photons(0)
   , h_TRT_edep_nonphotons(0)
   , h_TRT_kine_photons(0)
   , h_TRT_kine_nonphotons(0)
   , h_TRT_barcode(0)
   , m_TRT_x(0)
   , m_TRT_y(0)
   , m_TRT_z(0)
   , m_TRT_r(0)
   , m_TRT_time_photons(0)
   , m_TRT_time_nonphotons(0)
   , m_TRT_edep_photons(0)
   , m_TRT_edep_nonphotons(0)
   , m_TRT_kine_photons(0)
   , m_TRT_kine_nonphotons(0)
   , m_TRT_barcode(0)
   , m_tree(0)
   , m_path("/histos/TRTHitAnalysis/")
   , m_ntupleFileName("/ntuples/TRTHitAnalysis/")
   , m_thistSvc("THistSvc", name)
   
{
  declareProperty("HistPath", m_path);
  declareProperty("NtupleFileName", m_ntupleFileName);
}

StatusCode TRTHitAnalysis::initialize() {
  ATH_MSG_DEBUG( "Initializing TRTHitAnalysis" );

  // Grab the Ntuple and histogramming service for the tree
  CHECK(m_thistSvc.retrieve());

  /** Histograms **/

  h_TRT_x = new TH1D("h_TRT_x","hits_x", 100,-1100, 1100);
  h_TRT_x->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_TRT_x->GetName(), h_TRT_x));

  h_TRT_y = new TH1D("h_TRT_y", "hits_y", 100,-1100,1100);
  h_TRT_y->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_TRT_y->GetName(), h_TRT_y));

  h_TRT_z = new TH1D("h_TRT_z", "hits_z", 100,-3000,3000);
  h_TRT_z->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_TRT_z->GetName(), h_TRT_z));

  h_TRT_r = new TH1D("h_TRT_r", "hits_r", 100,500,1100);
  h_TRT_r->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_TRT_r->GetName(), h_TRT_r));

  h_TRT_xy = new TH2D("h_TRT_xy", "xy", 100,-1100.,1100.,100, -1100., 1100.);
  h_TRT_xy->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+h_TRT_xy->GetName(), h_TRT_xy));

  h_TRT_zr = new TH2D("h_TRT_zr", "zr", 100,-3000,3000,100,500.,1100.);
  h_TRT_zr->StatOverflows();
  CHECK(m_thistSvc->regHist( m_path+h_TRT_zr->GetName(), h_TRT_zr));

  h_TRT_time_photons = new TH1D("h_TRT_time_photons", "hits_time_photons", 100,0,500);
  h_TRT_time_photons->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_TRT_time_photons->GetName(), h_TRT_time_photons));

  h_TRT_time_nonphotons = new TH1D("h_TRT_time_nonphotons", "hits_time_nonphotons", 100,0,500);
  h_TRT_time_nonphotons->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_TRT_time_nonphotons->GetName(), h_TRT_time_nonphotons));

  h_TRT_edep_photons = new TH1D("h_TRT_edep_photons", "hits_edep_photons", 100,0,500);
  h_TRT_edep_photons->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_TRT_edep_photons->GetName(), h_TRT_edep_photons));

  h_TRT_edep_nonphotons = new TH1D("m_TRT_edep_nonphotons", "hits_edep_nonphotons", 100,0,500);
  h_TRT_edep_nonphotons->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_TRT_edep_nonphotons->GetName(), h_TRT_edep_nonphotons));

  h_TRT_kine_photons = new TH1D("h_TRT_kine_photons", "hits_kine_photons", 100,0,2);
  h_TRT_kine_photons->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_TRT_kine_photons->GetName(), h_TRT_kine_photons));

  h_TRT_kine_nonphotons = new TH1D("h_TRT_kine_nonphotons", "hits_kine_nonphotons", 100,0,1000);
  h_TRT_kine_nonphotons->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_TRT_kine_nonphotons->GetName(), h_TRT_kine_nonphotons));

  h_TRT_barcode = new TH1D("h_TRT_barcode", "hits_barcode", 100,-500,300000);
  h_TRT_barcode->StatOverflows();
  CHECK(m_thistSvc->regHist(m_path + h_TRT_barcode->GetName(), h_TRT_barcode));


  /*ntuples*/

  m_tree= new TTree("TRTHitNtuple","TRTHitAna");
  std::string fullNtupleName =  "/"+m_ntupleFileName+"/";
  CHECK(m_thistSvc->regTree(fullNtupleName,m_tree));

  if (m_tree){
    m_tree->Branch("x", &m_TRT_x);
    m_tree->Branch("y", &m_TRT_y);
    m_tree->Branch("z", &m_TRT_z);
    m_tree->Branch("r", &m_TRT_r);
    m_tree->Branch("time_photons", &m_TRT_time_photons);
    m_tree->Branch("time_nonphotons", &m_TRT_time_nonphotons);
    m_tree->Branch("EnergyDeposit_photons", &m_TRT_edep_photons);
    m_tree->Branch("EnergyDeposit_nonphotons", &m_TRT_edep_nonphotons);
    m_tree->Branch("KineticEnergy_photons", &m_TRT_kine_photons);
    m_tree->Branch("KineticEnergy_nonphotons", &m_TRT_kine_nonphotons);
    m_tree->Branch("barcode", &m_TRT_barcode);
  }else{
    ATH_MSG_ERROR("No tree found!");
  }
  
  
  return StatusCode::SUCCESS;
}		 

  

StatusCode TRTHitAnalysis::execute() {
  ATH_MSG_DEBUG( "In TRTHitAnalysis::execute()" );

  m_TRT_x->clear();
  m_TRT_y->clear();
  m_TRT_z->clear();
  m_TRT_r->clear();
  m_TRT_time_photons->clear();
  m_TRT_time_nonphotons->clear();
  m_TRT_edep_photons->clear();
  m_TRT_edep_nonphotons->clear();
  m_TRT_kine_photons->clear();
  m_TRT_kine_nonphotons->clear();
  m_TRT_barcode->clear();
  
  const DataHandle<TRTUncompressedHitCollection> p_collection;
    if (evtStore()->retrieve(p_collection, "TRTUncompressedHits" )==StatusCode::SUCCESS) {
      for(TRTUncompressedHitConstIter i_hit = p_collection->begin(); 
	            i_hit != p_collection->end();++i_hit){
	GeoTRTUncompressedHit ghit(*i_hit);
	HepGeom::Point3D<double> p = ghit.getGlobalPosition();

        h_TRT_x->Fill(p.x());
        h_TRT_y->Fill(p.y());
        h_TRT_z->Fill(p.z());
        h_TRT_r->Fill(p.perp());
        h_TRT_xy->Fill(p.x(), p.y());
        h_TRT_zr->Fill(p.z(),sqrt(pow(p.x(),2)+pow(p.y(),2)));
        h_TRT_barcode->Fill(i_hit->particleLink().barcode());

	m_TRT_x->push_back(p.x());
	m_TRT_y->push_back(p.y());
	m_TRT_z->push_back(p.z());
	m_TRT_r->push_back(p.perp());
	m_TRT_barcode->push_back(i_hit->particleLink().barcode());
        int particleId(i_hit->GetParticleEncoding());
        if(particleId == 22 || static_cast<int>(abs(particleId)/100000)==41 || static_cast<int>(abs(particleId)/10000000) == 1){
          h_TRT_time_photons->Fill(i_hit->GetGlobalTime());
          h_TRT_edep_photons->Fill(i_hit->GetEnergyDeposit());
          h_TRT_kine_photons->Fill(i_hit->GetKineticEnergy());
	  m_TRT_time_photons->push_back(i_hit->GetGlobalTime());
	  m_TRT_edep_photons->push_back(i_hit->GetEnergyDeposit());
	  m_TRT_kine_photons->push_back(i_hit->GetKineticEnergy());
        }else{
          h_TRT_time_nonphotons->Fill(i_hit->GetGlobalTime());
          h_TRT_edep_nonphotons->Fill(i_hit->GetEnergyDeposit());
          h_TRT_kine_nonphotons->Fill(i_hit->GetKineticEnergy());
	  
	  m_TRT_time_nonphotons->push_back(i_hit->GetGlobalTime());
	  m_TRT_edep_nonphotons->push_back(i_hit->GetEnergyDeposit());
	  m_TRT_kine_nonphotons->push_back(i_hit->GetKineticEnergy());
        }
      } // End while hits                                                                                                                                                  
    } 
    if (m_tree) m_tree->Fill();

      

  return StatusCode::SUCCESS;
}

