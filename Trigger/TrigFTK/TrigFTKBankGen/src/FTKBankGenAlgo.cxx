/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKBankGen/FTKBankGenAlgo.h"
#include "TrigFTKBankGen/FTKPattKDTree.h"
#include "TrigFTKSim/FTKSetup.h"
#include "TrigFTKSim/FTK_SGHitInput.h"
#include "TrigFTKSim/atlClustering.h"

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TROOT.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdio.h>
#include <math.h>

// SET THIS TO 1 TO ENABLE VERBOSE DEBUGGING OF REGION ASSIGNMENT
#define RMAP_DEBUG_PRINT 0

#define NEWTOWERASSIGN

using namespace std;
/////////////////////////////////////////////////////////////////////////////
FTKBankGenAlgo::FTKBankGenAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_hitInputTool("FTK_SGHitInput/FTK_SGHitInput"),
  m_nevent(0),m_nbanks(0), m_nsubregions(1),  
  m_verbosity(0),
  m_BarrelOnly(false),
  m_rawMode(1),
  m_IBLMode(1), // flag to change the IBLMode
  m_fixEndcapL0(false), 
  m_ITkMode(false), // flag to use ITk geometry
  m_pmap(0x0),
  m_rmap(0x0),
  m_pmap_path(),
  m_rmap_path(),
  m_Clustering(true),
  m_SaveClusterContent(false),
  m_DiagClustering(true),
  m_SctClustering(false),
  m_PixelClusteringMode(1),
  m_Ibl3DRealistic(false),
  m_DuplicateGanged(true),
  m_GangedPatternRecognition(false),
  m_UseIdentifierHash(false),
  m_sector_dir_path(),
  m_gcon_dir_path(),
  m_par_c_min(0),
  m_par_c_slices(0),
  m_par_phi_max(0),
  m_par_phi_min(0),
  m_par_phi_slices(0),
  m_par_d0_max(0),
  m_par_d0_min(0),
  m_par_d0_slices(0),
  m_par_z0_max(0),
  m_par_z0_min(0),
  m_par_z0_slices(0),
  m_par_eta_max(0),
  m_par_eta_min(0),
  m_par_eta_slices(0),
  m_PT_THRESHOLD(0),
  m_TRAIN_PDG(0),
  m_constant(0),
  m_const_test_mode(false),
  m_compsecbitmask()
{
  declareProperty("NEvent",m_nevent);
  // number of banks
  declareProperty("NBanks",m_nbanks);
  declareProperty("NSubRegions",m_nsubregions);
  declareProperty("BarrelOnly",m_BarrelOnly);
  declareProperty("rawMode",m_rawMode);
  declareProperty("pmap_path",m_pmap_path);
  declareProperty("rmap_path",m_rmap_path);
  declareProperty("const_test_mode",m_const_test_mode);
  declareProperty("sector_dir_path",m_sector_dir_path);
  declareProperty("gcon_dir_path",m_gcon_dir_path);
  // slice option
  declareProperty("par_c_max",m_par_c_max);
  declareProperty("par_c_min",m_par_c_min);
  declareProperty("par_c_slices",m_par_c_slices);
  declareProperty("par_phi_max",m_par_phi_max);
  declareProperty("par_phi_min",m_par_phi_min);
  declareProperty("par_phi_slices",m_par_phi_slices);
  declareProperty("par_d0_max",m_par_d0_max);
  declareProperty("par_d0_min",m_par_d0_min);
  declareProperty("par_d0_slices",m_par_d0_slices);
  declareProperty("par_z0_max",m_par_z0_max);
  declareProperty("par_z0_min",m_par_z0_min);
  declareProperty("par_z0_slices",m_par_z0_slices);
  declareProperty("par_eta_max",m_par_eta_max);
  declareProperty("par_eta_min",m_par_eta_min);
  declareProperty("par_eta_slices",m_par_eta_slices);
  // slice option
  declareProperty("PT_THRESHOLD",m_PT_THRESHOLD);
  declareProperty("TRAIN_PDG",m_TRAIN_PDG);

  //clustering option 20140219
  declareProperty("Clustering",m_Clustering);
  declareProperty("SaveClusterContent",m_SaveClusterContent);
  declareProperty("DiagClustering",m_DiagClustering);
  declareProperty("SctClustering",m_SctClustering);
  declareProperty("PixelClusteringMode",m_PixelClusteringMode);
  declareProperty("Ibl3DRealistic",m_Ibl3DRealistic);
  declareProperty("DuplicateGanged",m_DuplicateGanged);
  declareProperty("GangedPatternRecognition",m_GangedPatternRecognition);

  declareProperty("UseIdentifierHash",m_UseIdentifierHash);

  declareProperty("IBLMode",m_IBLMode);
  declareProperty("FixEndcapL0",m_fixEndcapL0);
  declareProperty("ITkMode",m_ITkMode);
}
FTKBankGenAlgo::~FTKBankGenAlgo()
{
  if (m_pmap) delete m_pmap;
  if (m_rmap) delete m_rmap;

  delete [] m_constant;

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKBankGenAlgo::initialize(){
  // get the reference to the central standalone instance
  FTKSetup &ftkset = FTKSetup::getFTKSetup();

  // Set Option (temporaly way for checking standalone ver. -> should change to better way) //
  ftkset.setBarrelOnly(m_BarrelOnly);
  ftkset.setRawMode(m_rawMode);
  ftkset.setVerbosity(m_verbosity);
  ftkset.setIBLMode(m_IBLMode);
  ftkset.setfixEndcapL0(m_fixEndcapL0);
  ftkset.setITkMode(m_ITkMode);

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTKBankGenAlgo::initialize()" << endmsg;

  StatusCode sc;
  sc = service("THistSvc", m_thistSvc);
  if(sc.isFailure()) {
    log << MSG::FATAL <<("Unable to retrieve THistSvc")<< endmsg;
    return StatusCode::FAILURE;
  }
  
  sc = m_hitInputTool.retrieve();
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not retrieve FTK_SGHitInput tool" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::INFO << "Setting FTK_SGHitInput tool" << endmsg;
  }

  log << MSG::INFO << "Read the logical layer definitions" << endmsg;
  // Look for the main plane-map
  if (m_pmap_path.empty()) {
    log << MSG::FATAL << "Main plane map definition missing" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    m_pmap = new FTKPlaneMap(m_pmap_path.c_str());
    if (!(*m_pmap)) {
      log << MSG::FATAL << "Error using plane map: " << m_pmap_path << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // set the pmap address to FTKDataInput to use in processEvent
  FTKDataInput *hitinput = m_hitInputTool->reference();
  hitinput->setPlaneMaps(m_pmap,0x0);
 
  // Create the region map objects
  if (m_rmap_path.empty()) {
    log << MSG::FATAL << "Region map file needed" << endmsg;
    return StatusCode::FAILURE;
  }
  else {
    log << MSG::INFO << "Creating region map" << endmsg;
    m_rmap = new FTKRegionMap(m_pmap, m_rmap_path.c_str());
    if (!(*m_rmap)) {
      log << MSG::FATAL << "Error creating region map from: " << m_rmap_path.c_str() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  /* initialize the clustering global variables, decalred in TrigFTKSim/atlClusteringLNF.h */
  //hitinput->setClustering(m_Clustering);
  hitinput->setPixelClusteringMode(m_PixelClusteringMode);
  hitinput->setIbl3DRealistic(m_Ibl3DRealistic);
  hitinput->setSctClustering(m_SctClustering);
  /*
  SAVE_CLUSTER_CONTENT = m_SaveClusterContent;
  DIAG_CLUSTERING = m_DiagClustering;
  //  SCT_CLUSTERING = m_SctClustering;
  PIXEL_CLUSTERING_MODE = m_PixelClusteringMode;
  DUPLICATE_GANGED = m_DuplicateGanged;
  GANGED_PATTERN_RECOGNITION = m_GangedPatternRecognition;
*/
  hitinput->init();

  // get detector configurations
  m_nplanes=m_pmap->getNPlanes();
  m_nregions = m_rmap->getNumRegions();  
  m_TotalDim=m_pmap->getTotalDim();
  initPattTree(m_nbanks, m_nplanes+1,m_TotalDim);
  
  log << MSG::INFO << "get dim" << m_TotalDim <<endmsg;
    
  m_TotalDim2=m_TotalDim*m_TotalDim;
  
  m_sectorID =(int *) calloc(m_nplanes,sizeof(int));
  m_hashID =(int *) calloc(m_nplanes,sizeof(int));
  m_tmpVec = (double *) calloc(m_TotalDim,sizeof(double));    
  m_tmpxC = (double *) calloc(m_TotalDim,sizeof(double));
  m_tmpxD = (double *) calloc(m_TotalDim,sizeof(double));
  m_tmpxPhi  = (double *) calloc(m_TotalDim,sizeof(double));
  m_tmpxCoto  =  (double *) calloc(m_TotalDim,sizeof(double));
  m_tmpxZ  =(double *) calloc(m_TotalDim,sizeof(double));
  m_tmpcovx = (double *) calloc(m_TotalDim*m_TotalDim,sizeof(double));

  m_npatterns =(int *) calloc(m_nbanks,sizeof(int));
  m_ntracks =(int *) calloc(m_nbanks,sizeof(int));
    
  m_p_ss = (int *) calloc(m_nplanes+1,sizeof(int));
  m_p_hashss = (int *) calloc(m_nplanes+1,sizeof(int));
  m_tmphitc = (double *) calloc(m_TotalDim,sizeof(double));    
  m_tmpxC2 = (double *) calloc(m_TotalDim,sizeof(double));
  m_tmpxD2 = (double *) calloc(m_TotalDim,sizeof(double));
  m_tmpxPhi2  = (double *) calloc(m_TotalDim,sizeof(double));
  m_tmpxCoto2  =  (double *) calloc(m_TotalDim,sizeof(double));
  m_tmpxZ2  =(double *) calloc(m_TotalDim,sizeof(double));
  m_tmpcovx2 = (double *) calloc(m_TotalDim*m_TotalDim,sizeof(double));
  for(int i=0;i<m_nbanks;i++){
    m_ntracks[i]=0;
    m_npatterns[i]=0;
  }

  if(m_const_test_mode){

    m_count_pass_filter = 0;
    m_count_match = 0;
    m_coverage = 0;

    strcpy(m_c_sector_dir_path, m_sector_dir_path.c_str());
    strcpy(m_c_gcon_dir_path, m_gcon_dir_path.c_str());

    //sector file
    //TODO tmp_SSID.resize(m_nplanes); to check
    m_tmp_ssID = (int *) calloc(m_nplanes+1,sizeof(int));
    m_tmp_hashID = (int *) calloc(m_nplanes+1,sizeof(int));

    for(int ibank=0;ibank<m_nbanks;ibank++){
      sprintf(m_str_sector_file_name,"%s/sectors_raw_%dL_reg%d.patt",m_c_sector_dir_path,m_nplanes,ibank);
      
      ifstream sector_file(m_str_sector_file_name);
      if(!sector_file){
    	log << MSG::FATAL << m_str_sector_file_name << " not found!!" << endmsg;
	return StatusCode::FAILURE;
      }

      //read header
      for(int i=0;i<2;i++)sector_file >> m_header[i];
    
      while(!sector_file.eof()){

	m_addPattReturnCode=0;      

    	//read sector
    	sector_file >> m_tmp_sectorID;
      
    	for(int i=0;i<m_nplanes;i++) sector_file >> m_tmp_ssID[i];
	m_tmp_ssID[m_nplanes]=m_tmp_sectorID;
    	sector_file >> m_dummy;
    	sector_file >> m_dummy;
	
	m_int_c.clear();
	m_int_phi.clear();
	m_int_d0.clear();
	m_int_z0.clear();
	m_int_eta.clear();

	m_int_c.resize(1);
	m_int_phi.resize(1);
	m_int_d0.resize(1);
	m_int_z0.resize(1);
	m_int_eta.resize(1);
  
	m_Mtmp.nhit=1;
	m_addPattReturnCode=addKDPattern(ibank, m_const_test_mode, m_tmp_ssID, m_tmp_hashID, 1,m_Mtmp,m_tmphitc,m_tmpxC2,m_tmpxD2,m_tmpxPhi2,m_tmpxCoto2,m_tmpxZ2,m_tmpcovx2,
				       m_int_c,m_int_phi,m_int_d0,m_int_z0,m_int_eta);

      }//sector_file.eof
      log << MSG::INFO << "sector:region " << ibank << " loaded." << endmsg;
      
    }

    m_constant = new FTKConstantBank*[m_nbanks];
    for (int i=0;i<m_nbanks;++i)m_constant[i]= 0;

    //setBank function
    for(int ibank=0;ibank<m_nbanks;ibank++){
      sprintf(m_str_gcon_file_name,"%s/corrgen_raw_%dL_reg%d.gcon",m_c_gcon_dir_path,m_nplanes,ibank);
      ifstream gcon_file(m_str_gcon_file_name);
      if(!gcon_file){
      	log << MSG::FATAL << m_str_gcon_file_name << " not found!!" << endmsg;
	return StatusCode::FAILURE;
      }

      m_gcon_path = m_str_gcon_file_name;
      log << MSG::INFO << "gcon file =  " << m_gcon_path << endmsg;
      m_constant[ibank] = new FTKConstantBank(m_TotalDim,m_gcon_path.c_str());
      log << MSG::INFO << "constant:region " << ibank << " loaded." << endmsg;
    }

    m_base_trk.setNCoords(m_TotalDim);
    m_base_trk.setNPlanes(m_nplanes);
    m_base_trk.setNMissing(0);

    char filename_coverage[30];
    sprintf(filename_coverage,"coverage.txt");
    m_file_coverage.open(filename_coverage);

    char filename_truthpar[30];
    sprintf(filename_truthpar,"truthpar.txt");
    m_file_truthpar.open(filename_truthpar);

    char filename_recpar[30];
    sprintf(filename_recpar,"recpar.txt");
    m_file_recpar.open(filename_recpar);

    char filename_resolution[30];
    sprintf(filename_resolution,"resolution.txt");
    m_file_resolution.open(filename_resolution);

    m_file_resolution << "dC dD dPhi dZ0 dCoto Chi2" << endl;

  }else{//const test mode

    char filename[30];
  
    sprintf(filename,"matrix_%dL_%dDim.root",m_nplanes,m_TotalDim);
  
    m_file = new TFile(filename,"recreate");
    gROOT->cd();
  
    char name[5];
    char title[20];

    for(int i = 0; i < m_nregions; ++i){
    
      sprintf(name,"am%d",i);
      sprintf(title,"Ambank %d para",i); 

      m_tree[i] = new TTree(name, title);
    }

    m_montree = new TTree("montree", "monitor tree");
    m_montree->Branch("monval", m_monval, "m_monval[100]/l");
    for(int i=0;i<100;i++){
      m_monval[i]=0;
    }// for i                                                                    

    m_slicetree=new TTree("slice", "slice para");
  
    for(int i = 0; i < m_nregions; ++i){
      m_tree[i]->Branch("ndim",&m_TotalDim,"ndim/I");
      m_tree[i]->Branch("ndim2",&m_TotalDim2,"ndim2/I");
      m_tree[i]->Branch("Vec", m_tmpVec,"Vec[ndim]/D");
      m_tree[i]->Branch("nplanes",&m_nplanes,"nplanes/I");
      m_tree[i]->Branch("sectorID", m_sectorID,"sectorID[nplanes]/I");
      m_tree[i]->Branch("hashID", m_hashID,"hashID[nplanes]/I");
      m_tree[i]->Branch("nhit", &m_nhit,"nhit/F");
      m_tree[i]->Branch("tmpC", &m_tmpC,"tmpC/D");
      m_tree[i]->Branch("tmpD", &m_tmpD,"tmpD/D");
      m_tree[i]->Branch("tmpPhi", &m_tmpPhi,"tmpPhi/D");
      m_tree[i]->Branch("tmpCoto", &m_tmpCoto,"tmpCoto/D");
      m_tree[i]->Branch("tmpZ", &m_tmpZ,"tmpZ/D");
      m_tree[i]->Branch("tmpxC2", m_tmpxC,"tmpxC2[ndim]/D");
      m_tree[i]->Branch("tmpxD2", m_tmpxD,"tmpxD2[ndim]/D");
      m_tree[i]->Branch("tmpxPhi2", m_tmpxPhi,"tmpxPhi2[ndim]/D");
      m_tree[i]->Branch("tmpxCoto2", m_tmpxCoto,"tmpxCoto2[ndim]/D");
      m_tree[i]->Branch("tmpxZ2", m_tmpxZ,"tmpxZ2[ndim]/D");
      m_tree[i]->Branch("tmpcovx2", m_tmpcovx,"tmpcovx2[ndim2]/D");
    
      m_intc= new std::vector<short>;
      m_intphi= new std::vector<short>;
      m_intd0= new std::vector<short>;
      m_intz0= new std::vector<short>;
      m_inteta= new std::vector<short>;

      m_tree[i]->Branch("tmpintc", &m_intc);
      m_tree[i]->Branch("tmpintphi", &m_intphi);
      m_tree[i]->Branch("tmpintd0", &m_intd0);
      m_tree[i]->Branch("tmpintz0", &m_intz0);
      m_tree[i]->Branch("tmpinteta", &m_inteta);
          
      m_tree[i]->Print();
    }
    m_slicetree->Branch("c_max",&m_par_c_max);
    m_slicetree->Branch("phi_max",&m_par_phi_max);
    m_slicetree->Branch("d0_max",&m_par_d0_max);
    m_slicetree->Branch("z0_max",&m_par_z0_max);
    m_slicetree->Branch("eta_max",&m_par_eta_max);
  
    m_slicetree->Branch("c_min",&m_par_c_min);
    m_slicetree->Branch("phi_min",&m_par_phi_min);
    m_slicetree->Branch("d0_min",&m_par_d0_min);
    m_slicetree->Branch("z0_min",&m_par_z0_min);
    m_slicetree->Branch("eta_min",&m_par_eta_min);
  
    m_slicetree->Branch("c_slices",&m_par_c_slices);
    m_slicetree->Branch("phi_slices",&m_par_phi_slices);
    m_slicetree->Branch("d0_slices",&m_par_d0_slices);
    m_slicetree->Branch("z0_slices",&m_par_z0_slices); 
    m_slicetree->Branch("eta_slices",&m_par_eta_slices);

  }//const test mode

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKBankGenAlgo::execute() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "FTKBankGenAlgo::execute() start" << endmsg;
  
  // clear array of variables
  m_hitInputTool->reference()->clearRawHits(); 
  m_hitInputTool->reference()->clearTruthTrack();  
  m_hitslist.clear();
  m_trainingtracks.clear();

  m_int_c.clear();
  m_int_phi.clear();
  m_int_d0.clear();
  m_int_z0.clear();
  m_int_eta.clear();
  
  // event difinitions
  m_doPattgen = true;
  m_GoodTrack = true;
  
  /////////////////////////////////////////////////////
  // track validation part (copied from th_rd.c)    //
  ////////////////////////////////////////////////////
  //hits & truth track is get from SGHitInput
  m_hitInputTool->reference()->readData();  
  m_truth_track = m_hitInputTool->reference()->getTruthTrack();

  // clustering hits
  m_hitInputTool->reference()->processEvent(false);
  m_nhits =  m_hitInputTool->reference()->getNHits(); 
  m_hitslist = m_hitInputTool->reference()->getHitsList();

  m_nmuon = 0;
  m_eta = 0;
  m_pt =0;
  m_truth_phi = 0;

  // reset the hits of maps
  m_maphits.clear();

  for(unsigned int i=0;i<m_truth_track.size();++i){
    m_c = m_truth_track[i].getQ()/(sqrt(m_truth_track[i].getPX()*m_truth_track[i].getPX()+m_truth_track[i].getPY()*m_truth_track[i].getPY()));
    m_c=(m_c)/2.;
    m_pt = TMath::Sqrt(m_truth_track[i].getPX()*m_truth_track[i].getPX()+m_truth_track[i].getPY()*m_truth_track[i].getPY())/1000;
    m_eta = TMath::ASinH(m_truth_track[i].getPZ()/TMath::Sqrt(m_truth_track[i].getPX()*m_truth_track[i].getPX()+m_truth_track[i].getPY()*m_truth_track[i].getPY()));
    m_truth_phi = TMath::ATan2(m_truth_track[i].getPY(),m_truth_track[i].getPX());
    log << MSG::DEBUG << "i pt = "<<i<< " " <<m_pt<<endmsg;
    log << MSG::DEBUG << "i c = "<<i<< " " <<m_c<<endmsg;
    log << MSG::DEBUG << "i eta = "<<i<<" "<<m_eta<<endmsg;
    log << MSG::DEBUG << "i phi = "<<i<<" "<<m_truth_phi<<endmsg;
    log << MSG::DEBUG << "i pdgcode = "<<i<<" "<<m_truth_track[i].getPDGCode()<<endmsg;

    m_monval[0]++;
    if(m_truth_track[i].getBarcode() >= 1000000 || (abs(m_truth_track[i].getPDGCode())!=m_TRAIN_PDG))continue;

    m_monval[1]++;

    //    if(m_truth_track[i].getBarcode() >= 100000)continue;
    if(m_pt>m_PT_THRESHOLD && abs(m_truth_track[i].getPDGCode())==m_TRAIN_PDG){//m_GoodTrack!!
      // add the track to the list of good tracks
      m_trainingtracks.push_back(m_truth_track[i]);
      // prepare the entry in the map of hits
      m_maphits[m_truth_track[i].getBarcode()] = vector<FTKHit>();

      m_monval[2]++;
    }
  }

  // get the number of tracks
  m_nmuon = m_trainingtracks.size();

  for(unsigned int j=0;j<m_hitslist.size();j++){ // loop over the hits
    // get the predominant barcode for the current hit
    int hitbarcode = m_hitslist[j].getTruth().best_barcode();
    // check if the barcode is associated to a good muon
    map<int, vector<FTKHit> >::iterator ilist = m_maphits.find(hitbarcode);

    // skip the hits if not associated to a good truth tracks
    if(ilist==m_maphits.end()) continue;

    // add hit to the list
    (*ilist).second.push_back(m_hitslist[j]);
  } // end loop over the hits

  if(0==m_nmuon) m_GoodTrack=false;

  ///////////////////////////////////////////////////////
  //////   make sector                             //////
  ///////////////////////////////////////////////////////
  
  if(!m_GoodTrack){log << MSG::DEBUG << " bad track event " << endmsg;}
  else{ // there are good tracks

    //****************************************************************//
    //****   pattgen copied and modfied from  TrigFTKLib      ********// 
    //***************************************************************//


    for(int nTruth=0;nTruth<m_nmuon;nTruth++){ // loop over the tracks
      m_monval[3]++;
      m_doPattgen = true;

      int sector=0,sector_HW=0,cur_plane=0,cur_sector=0,cur_sector_HW=0;
      int phi_mod=0,eta_mod=0,section=0;
      int cur_phi=0,cur_eta=0,cur_section=0;
      int sdiff=0,ediff=0,pdiff=0;
      int rmax=0;

      std::vector<filterSecHit> sechit(m_nplanes);//initialize truth by truth
      for (int ilayer=0; ilayer!=m_nplanes; ++ilayer) {
        // enforce the initialization of the sechit elements
        sechit[ilayer].count = 0; // it is enough to just set the layer counter to 0
      }
      // this block works on the hit related to this tracks
      FTKTruthTrack &curtrack = m_trainingtracks[nTruth];

      // list of hits associated to the current truth track
      vector<FTKHit> &curhitlist = m_maphits[curtrack.getBarcode()]; // it has to exist, at least has empty list
      for(unsigned int i=0;i<curhitlist.size();++i){ // loop over the hits

        // point to the current
        FTKHit &curhit = curhitlist[i];
#define FHIT sechit[curhit.getPlane()] //filtered hit for the current logical layer

        if(m_doPattgen && m_GoodTrack){//don't make pattern
           //int plane= curhit.getPlane();
           sector = curhit.getSector();
           sector_HW = curhit.getIdentifierHash();

          phi_mod = curhit.getPhiModule();

          if(curhit.getIsBarrel()){//section and m_eta definition
            eta_mod = curhit.getEtaModule();
            section =0;
          }else{
            eta_mod = curhit.getEtaModule();
            section = curhit.getSection();
          }

          log << MSG::DEBUG << "sector, phi_mod, eta_mod, section = " <<sector << " " << phi_mod << " " << eta_mod << " " << section << endmsg;

          if(0==FHIT.count){ // initialize
            FHIT.count++;
            cur_plane = curhit.getPlane();
            cur_sector = curhit.getSector();
            cur_sector_HW = curhit.getIdentifierHash();

            // copy the original hit, this should be the only required information
            FHIT.originalhit =  curhit;

            FHIT.plane = cur_plane;
            FHIT.sector = cur_sector;
            FHIT.sector_HW = cur_sector_HW;
            FHIT.ndim = curhit.getDim();
            FHIT.CSide = curhit.getCSide();
            FHIT.ASide = curhit.getASide();

            if(2==FHIT.ndim){
              FHIT.dimx = curhit.getCoord()[0];
              FHIT.dimy = curhit.getCoord()[1];
            }else if(1==FHIT.ndim){
              FHIT.dimx = curhit.getCoord()[0];
              FHIT.dimy = 0;
            }
          }else if(1==FHIT.count){ // the position was already filled with an hit
            cur_plane = FHIT.originalhit.getPlane();
            cur_sector = FHIT.originalhit.getSector();
            cur_phi = FHIT.originalhit.getPhiModule();;
            cur_eta = FHIT.originalhit.getEtaModule();;
            cur_section = FHIT.originalhit.getSection();

            // check for overlap
            FHIT.count++;

            if(sector == cur_sector){
              //	    if(sector_ref==FHIT.sector_ref){
              log << MSG::DEBUG << "Two hits on same module, exiting filterHitsSec " <<endmsg;
              m_doPattgen = false;
	      m_monval[4]++;
            }else {//not same sector
              sdiff = cur_section-section;
              pdiff = cur_phi-phi_mod;
              ediff = cur_eta-eta_mod;

              if(ediff==0 && sdiff==0){
                section=cur_section;
                //TODO why is this not a global declaration?
                if(8==m_nregions){
                  rmax=m_rmap->getRegionMapItem(m_nregions-1,curhit.getPlane(),section).getMax();
                }else if(64==m_nregions || 32==m_nregions){
                  rmax=m_rmap->getRegionMapItem(m_nregions-1,curhit.getPlane(),section).getTotMod();
                }

                if(pdiff == 1 || pdiff== -rmax ){
                  // Do noting ,we got the right one
                }else if(pdiff == -1 ||pdiff == rmax ){
                  //swap hits, the new one is the right one
                  FHIT.originalhit =  curhit;
                  FHIT.sector =sector;
                  FHIT.sector_HW =sector_HW;
                  FHIT.ndim = curhit.getDim();
                  if(2==FHIT.ndim){
                    FHIT.dimx = curhit.getCoord()[0];
                    FHIT.dimy = curhit.getCoord()[1];
                  }else if(1==FHIT.ndim){
                    FHIT.dimx = curhit.getCoord()[0];
                    FHIT.dimy = 0;
                  }
                }else{
                  //Not overlap, free and exit
                  log << MSG::DEBUG << "Hits are too far away in phi, exiting filterHitsSec " <<endmsg;
                  m_doPattgen =false;
		  m_monval[5]++;
                }

                // sector overlap is a perfectly reasonable situation with forward disks
                // m_eta and phi will differ in general in this case
                // Take the lower-z hit preferentially (right thing to do? d0/pT tradeoff)
                // But something fishy is going on if we've got two hits on the same disk.
              }
              else if (sdiff!=0){//ediff sdiff orig
                // If one is barrel and one endcap, it's definitely OK, take the barrel hit
                if(m_pmap->getPlane(curhit.getPlane(),cur_section).getIsEndcap()
                    && !m_pmap->getPlane(curhit.getPlane(),section).getIsEndcap()){
                  // Swap hits, the new one is the right one
                  FHIT.originalhit = curhit;
                  FHIT.sector=sector;
                  FHIT.sector_HW=sector_HW;
                  FHIT.ndim=curhit.getDim();
                  if(2==FHIT.ndim){
                    FHIT.dimx=curhit.getCoord()[0];
                    FHIT.dimy=curhit.getCoord()[1];
                  }else if(1==FHIT.ndim){
                    FHIT.dimx=curhit.getCoord()[0];
                    FHIT.dimy=0;
                  }
                  log << MSG::DEBUG << " Swapping to use barrel hit " <<endmsg;
                }
                else if (!m_pmap->getPlane(curhit.getPlane(),cur_section).getIsEndcap()
                    && m_pmap->getPlane(curhit.getPlane(),cur_section).getIsEndcap()){

                  log << MSG::DEBUG << " Fine, we've got the barrel hit " <<endmsg;
                  // Two endcap hits : same disk: discard
                }
                else if (m_pmap->getPlane(curhit.getPlane(),cur_section).getPDisk()
                    ==m_pmap->getPlane(curhit.getPlane(),section).getPDisk() ){
                  log << MSG::DEBUG << "Two modules hit in same physical disk" <<m_pmap->getPlane(curhit.getPlane(),cur_section).getPDisk()<< "exiting filterHitsSec " <<endmsg;
                  m_doPattgen=false;
		  m_monval[6]++;
                  //Two endcap hits on opposite sides makes no sense: discard
                }
                else if((FHIT.CSide && curhit.getASide())
                    || (FHIT.ASide && curhit.getCSide())){

                  log << MSG::DEBUG << " Endcap disks on opposite sides hit, exiting filterHitsSec " <<endmsg;

                  m_doPattgen=false;
		  m_monval[7]++;
                  //Two endcap hits on same side: different disks: take the lower-z
                }
                else {

                  log << MSG::DEBUG << " Keeping the lower-z of the two disks hit " <<endmsg;
                  //If CURHIT disk is a lower number, we've got the right one

                  if(m_pmap->getPlane(curhit.getPlane(),cur_section).getPDisk()
                      >m_pmap->getPlane(curhit.getPlane(),section).getPDisk()){
                    // Swap hits, the new one is the right one
                    FHIT.originalhit = curhit;
                    FHIT.sector=sector;
                    FHIT.sector_HW=sector_HW;
                    FHIT.ndim=curhit.getDim();
                    if(2==FHIT.ndim){
                      FHIT.dimx=curhit.getCoord()[0];
                      FHIT.dimy=curhit.getCoord()[1];
                    }else if(1==FHIT.ndim){
                      FHIT.dimx=curhit.getCoord()[0];
                      FHIT.dimy=0;
                    }
                  }
                  log << MSG::DEBUG << "\t\t\t...Kept " <<endmsg;
                }


              }
              else{//ediff != 0: Different m_eta idx is no good, free and exit
                log << MSG::DEBUG << "Hits are in different eta, exiting filterHitsSec " <<endmsg;
                m_doPattgen=false;
		m_monval[8]++;
              }
            }//not same sector
          }
          else if(FHIT.count>1) {

            log << MSG::DEBUG << "Too many hits on a plane, exiting filterHitsSec " <<endmsg;
            m_doPattgen = false;
	    m_monval[9]++;
          } // How many do hits exist in same plane.
          log << MSG::DEBUG << " gethittype "<<FHIT.hittype <<endmsg;
          log << MSG::DEBUG << " getdim "<<FHIT.ndim <<endmsg;
        }//do pattgen
      } // end loop over the hits

      curhitlist.clear();

      if(m_doPattgen && m_GoodTrack){
        for(int i=0;i<m_nplanes;++i){
          log << MSG::DEBUG << "getsector pre " <<sechit[i].sector<< endmsg;
          if(0==sechit[i].count){
            m_doPattgen = false;
	    m_monval[10]++;
            log << MSG::DEBUG << "Plane"<< i <<" has no hits, exiting filterHitsSec "<< endmsg;

          }
        }
      }

      /////////////////////////////////////////////////////////
      /////            which regios?(old format)        /////
      ///////////////////////////////////////////////////////

      int nregion=-999;
      if(!m_doPattgen){
        log << MSG::INFO << "don't generate sector table " <<endmsg;
        nregion=-999;
      } else {

        // use sectorID as Identifier Hash
	/* tkaji
        if (m_UseIdentifierHash){
          for(int i=0;i<m_nplanes;++i) {
            log << MSG::DEBUG << "getsector " <<sechit[i].sector_HW<< endmsg;
            m_p_ss[i]=sechit[i].sector_HW;
          }
        }
        else {
          for(int i=0;i<m_nplanes;++i) {
            log << MSG::DEBUG << "getsector " <<sechit[i].sector<< endmsg;
            m_p_ss[i]=sechit[i].sector;
          }
        }
	*/

	for(int i=0;i<m_nplanes;i++){
	  m_p_hashss[i] = sechit[i].sector_HW;
	  m_p_ss[i] = sechit[i].sector;
	}

        m_p_hashss[m_nplanes]=0;//for sectors, we set it to zero.
        m_p_ss[m_nplanes]=0;//for sectors, we set it to zero.
	
#ifdef NEWTOWERASSIGN
        // the sector is set by the first hit, the following hits can only confirm it
        m_compsecbitmask.set();// all towers are possible

        for(int i=0;i<m_nplanes;++i) { // loop over the layer
          for (int itwr=0;itwr!=m_nbanks;++itwr) { // loop over hte towers
            if (!m_rmap->isHitInRegion(sechit[i].originalhit,itwr))
              m_compsecbitmask.set(itwr,false); // switch off an impossible sector
          } // end loop over the towers
        } // end loop over the layers

        /* At the end of the previous loop the compsecbitmask object is 1 for
         * every tower that *is* compatible with the given sector, is 0 for
         * every tower that *isn't* compatible with the given sector
         */
        nregion = -999;
        for (int itwr=0;itwr!=m_nbanks;++itwr) { // loop over the towers
          if (m_compsecbitmask.test(itwr)) { // good tower block
            // when the previous test is true the current tower is compatible with the sector
	    if (m_ITkMode) {
	      // for now just give preference to lowest region index for simplicity
	      nregion = itwr;
	      break;
	    }
            else if (m_nbanks==64) { // 64 towers block case
              if (nregion==-999) {
                // initialize the tower selection variable
                nregion = itwr;
              }
              else {
                // assumes 64 towers in 4 barrels, with 16 phi slices each
                const int curphi(itwr%16);
                const int refphi(nregion%16);
                const int cureta(itwr/16);
                const int refeta(nregion/16);
                if (cureta!=refeta){
                  // central towers are preferred
                  if (refeta==0&&cureta==1) nregion = itwr;
                  else if (refeta==3&&cureta==2) nregion = itwr;
                  // in case both towers are central, the last layer decides
                  else if (refeta==1&&cureta==2&&sechit[m_nplanes-1].originalhit.getEtaCode()>=6) nregion = itwr;
                }
                else {
                  /* If the the current and reference towers are in the same m_eta
                   * region, the greater index is preferred. The exception is to
                   * consider refphi=0 preferred to curphi=15, to restore the simmetry
                   */
                  if (refphi==0 && curphi==15) continue; // nothing to do
                  else nregion = itwr; // normal case
                }
              }
            }
            else if (m_nbanks==32) { // 32 towers block case
              if (nregion==-999) {
                // initialize the tower selection variable
                nregion = itwr;
              }
              else {
                // assumes 64 towers in 4 barrels, with 16 phi slices each
                const int curphi(itwr%8);
                const int refphi(nregion%8);
                const int cureta(itwr/8);
                const int refeta(nregion/8);
                if (cureta!=refeta){
                  // central towers are preferred
                  if (refeta==0&&cureta==1) nregion = itwr;
                  else if (refeta==3&&cureta==2) nregion = itwr;
                  // in case both towers are central, the last layer decides
                  else if (refeta==1&&cureta==2&&sechit[m_nplanes-1].originalhit.getEtaCode()>=6) nregion = itwr;
                }
                else {
                  /* If the the current and reference towers are in the same m_eta
                   * region, the greater index is preferred. The exception is to
                   * consider refphi=0 preferred to curphi=7, to restore the simmetry
                   */
                  if (refphi==0 && curphi==7) continue; // nothing to do
                  else nregion = itwr; // normal case
                }
              }
            }
            else if (m_nbanks==8) {
              nregion = itwr; // the bank with the lowest index wins
              break;
            }
            else {
              cerr << "Error" << endl;
              return StatusCode::FAILURE;
            }
          } // end good tower block
        }
#else // OLD TOWER ASSIGNEMENT CODE, DEPRECATED
#include "TrigFTKBankGen/decodeEtacode.h"
        if(m_nbanks==8){

          std::vector<int>result(m_nplanes);

          for(int i=0;i<m_nplanes;++i) {
            //	    log << MSG::DEBUG << "getsector " <<sechit[i].sector<< endmsg;
            log << MSG::DEBUG << "getsector " <<sechit[i].sector<< endmsg;
            log << MSG::DEBUG << "getplane " <<sechit[i].plane<< endmsg;

            m_p_ss[m_nplanes]=0;//for sectors, we set it to zero.

            //	    int phi = sechit[i].sector/1000;
            int phi = sechit[i].sector/1000;
            result[i]=0;

            if(sechit[i].sector%1000<20){//section and m_eta definition
              //	    if(sechit[i].sector_ref%1000<20){//section and m_eta definition
              section=0;
            }else{
              section=sechit[i].sector%10;
              //	      section=sechit[i].sector_ref%10;
            }

            //	 Region 0 is special: some negative phi indices
            if((phi <= m_rmap->getRegionMapItem(0,i,section).getMax() ||
                phi >= m_rmap->getRegionMapItem(m_nregions-1,i,section).getTotMod() +
                m_rmap->getRegionMapItem(0,i,section).getMin()+1 )) {
              result[i] += 1;
            }

            /* Check other regions */
            for(int k = 1; k < m_nregions; ++k) {
              if((phi <= m_rmap->getRegionMapItem(k,i,section).getMax()) &&
                  (phi >= m_rmap->getRegionMapItem(k,i,section).getMin())) {
                result[i] += 1 << k;
              }
            }
          } // loop over planes

          int region = result[0];

          for(int k=1; k<m_nplanes; ++k)
            region &= result[k];

          if(!region){
            nregion =-999;
          }else if(region == (1 << (m_nbanks-1))+1){// 7 and 0, return 0
            nregion=0;
          }else{
            nregion=0;
            while((region >> nregion) != 1)
              ++nregion;
          }
          result.clear();
        }else if(m_nbanks==64){

          ////////////////////////////////////////////
          /////////  region for 64-tower  ////////////
          ////////////////////////////////////////////

          for(int i=0;i<m_nplanes;++i) {
            log << MSG::DEBUG << "getsector " <<sechit[i].sector<< endmsg;
            m_p_ss[i]=sechit[i].sector;
            // log << MSG::DEBUG << "getsector " <<sechit[i].sector_ref<< endmsg;
            // m_p_ss[i]=sechit[i].sector_ref;
          }
          m_p_ss[m_nplanes]=0;//for sectors, we set it to zero.

          ////////////////////////////////////////////////////////
          //   Eta (analogues to whichRegionEta of rmap_rd.m_c)   //
          ////////////////////////////////////////////////////////

          static int pM_eta[]={2,3,5,7};
          static int *result_eta;
          static int allocated_eta = 0;
          int region_eta = -1;
          // total tower id's where we switch tower-m_eta
          const int t0 = 0;
          const int t1 = m_rmap->getNumRegionsPhi()*1;
          const int t2 = m_rmap->getNumRegionsPhi()*2;
          const int t3 = m_rmap->getNumRegionsPhi()*3;
          int i_eta,itower_eta;
          int aside,cside,etamod;
          int found_eta;
          int tower1_OK;

          if(!allocated_eta) {
            allocated_eta = 1;
            if((result_eta = (int *)calloc(m_nplanes,sizeof(int))) == NULL)
              log << MSG::ERROR << "whichRegionAlt: could not allocate result space" << endmsg;
          }
          for(i_eta=0;i_eta<m_nplanes;++i_eta)
            result_eta[i_eta] = 1;

          for(i_eta=0;i_eta<m_nplanes;++i_eta) {
            cside = CSIDE_INT(m_p_ss[i_eta]);
            aside = ASIDE_INT(m_p_ss[i_eta]);
            // endcap C-side
            if(cside==1) {
              result_eta[i_eta] *= pM_eta[0];
              log << MSG::DEBUG << " ->eta: "<< sechit[i_eta].sector <<" - - -" << endmsg;
              //	      log << MSG::DEBUG << " ->eta: "<< sechit[i_eta].sector_ref <<" - - -" << endmsg;
            }
            // endcap A-side
            else if(aside==1) {
              result_eta[i_eta] *= pM_eta[3];
              log << MSG::DEBUG << " ->eta: - - - " <<sechit[i_eta].sector<< endmsg;
              //	      log << MSG::DEBUG << " ->eta: - - - " <<sechit[i_eta].sector_ref<< endmsg;
            }
            // barrel
            else {
              etamod = ETA_MOD_INT(m_p_ss[i_eta]);
              if(etamod >= m_rmap->getRegionMapItem(t0,i_eta,0).getEtaMin() && etamod<=m_rmap->getRegionMapItem(t0,i_eta,0).getEtaMax() ) {
                result_eta[i_eta] *= pM_eta[0];
              }
              if(etamod >= m_rmap->getRegionMapItem(t1,i_eta,0).getEtaMin() && etamod<=m_rmap->getRegionMapItem(t1,i_eta,0).getEtaMax() ) {
                result_eta[i_eta] *= pM_eta[1];
              }
              if(etamod >= m_rmap->getRegionMapItem(t2,i_eta,0).getEtaMin() && etamod<=m_rmap->getRegionMapItem(t2,i_eta,0).getEtaMax() ) {
                result_eta[i_eta] *= pM_eta[2];
              }
              if(etamod >= m_rmap->getRegionMapItem(t3,i_eta,0).getEtaMin() && etamod<=m_rmap->getRegionMapItem(t3,i_eta,0).getEtaMax() ) {
                result_eta[i_eta] *= pM_eta[3];
              }
            }
          }

          // now find intersection of result_eta[i_eta] arrays for all i
          tower1_OK = 0;
          for(itower_eta=0;itower_eta<4;itower_eta++) {
            found_eta=1;
            for(i_eta=0;i_eta<m_nplanes;++i_eta) {
              if((result_eta[i_eta]%pM_eta[itower_eta])!=0) {found_eta = 0; break; }
            }
            if(found_eta==1) {
              if(itower_eta==1) {
                tower1_OK = 1;
                continue;
              }
              else {
                if(itower_eta==2 && tower1_OK==1) {
                  if( ETA_MOD_INT(m_p_ss[m_nplanes-1]) <= 5 ) region_eta=1; else region_eta=2;
                  break;
                }
                region_eta = itower_eta;
                break;
              }
            }
            else if(itower_eta==2 && tower1_OK==1){
              region_eta=1;
              break;
            }
          }

          /////////////
          //   Phi   //
          /////////////

          static int pM_phi[]={2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53};
          static int *result_phi;
          static int allocated_phi = 0;
          const int debug = RMAP_DEBUG_PRINT;
          int region_phi = -1;
          int i_phi,itow,itower_phi;
          int phi,section_phi;
          int found_phi;
          int tow0_OK;
          int j;

          if(-1 != region_eta){//m_eta is defined
            if(!allocated_phi) {
              allocated_phi = 1;
              if((result_phi = (int *)calloc(m_nplanes,sizeof(int))) == NULL)
                log << MSG::ERROR << "whichRegionAlt: could not allocate result space" << endmsg;
            }
            for(i_phi=0;i_phi<m_nplanes;++i_phi)
              result_phi[i_phi] = 1;

            for(i_phi=0;i_phi<m_nplanes;++i_phi) {
              phi = PHI_MOD_INT(m_p_ss[i_phi]);
              section_phi = SECTION_INT(m_p_ss[i_phi]);
              for(itow=0;itow<m_rmap->getNumRegionsPhi();itow++) {
                itower_phi = m_rmap->getNumRegionsPhi()*region_eta + itow;
                if( m_rmap->getRegionMapItem(itower_phi,i_phi,section_phi).getPhiMin() <= m_rmap->getRegionMapItem(itower_phi,i_phi,section_phi).getPhiMax() ) {
                  if(phi >= m_rmap->getRegionMapItem(itower_phi,i_phi,section_phi).getPhiMin() && phi <= m_rmap->getRegionMapItem(itower_phi,i_phi,section_phi).getPhiMax() ) {
                    result_phi[i_phi] *= pM_phi[itow];
                  }
                } else {
                  if(phi >= m_rmap->getRegionMapItem(itower_phi,i_phi,section_phi).getPhiMin() || phi <= m_rmap->getRegionMapItem(itower_phi,i_phi,section_phi).getPhiMax() ) {
                    result_phi[i_phi] *= pM_phi[itow];
                  }
                }

              }
              if(debug) {
                printf(" ->phi: ");
                for(j=0;j<m_rmap->getNumRegionsPhi();j++) printf("%s ", ((result_phi[i_phi]%pM_phi[j]==0)?"x":"-"));
                printf("  phi=%d section=%d",phi,section_phi);
                printf("\n");
              }
            }

            // now find intersection of result_phi[i_phi] arrays for all i
            tow0_OK = 1;
            for(i_phi=0;i_phi<m_nplanes;++i_phi) {
              if((result_phi[i_phi]%pM_phi[0])!=0) { tow0_OK = 0; break; }
            }

            for(itow=m_rmap->getNumRegionsPhi()-1;itow>=0;itow--) {
              found_phi=1;
              for(i_phi=0;i_phi<m_nplanes;++i_phi) {
                if((result_phi[i_phi]%pM_phi[itow])!=0) {found_phi = 0; break; }
              }
              if(found_phi==1) {
                if(itow==m_rmap->getNumRegionsPhi()-1 && tow0_OK==1) {
                  region_phi = 0;
                  break;
                }
                else {
                  region_phi = itow;
                  break;
                }
              }
            }

          }//m_eta is defined

          /////////////////////
          //  which region?? //
          /////////////////////

          if(region_eta==-1) {
            nregion = -999;
            log << MSG::DEBUG <<" bad eta!!!" <<endmsg;
          }else if(region_phi==-1) {
            nregion = -999;
            log << MSG::DEBUG <<" bad phi!!!" <<endmsg;
          } else {
            nregion = region_eta * m_rmap->getNumRegionsPhi() + region_phi;
          }
        }//nbanks == 64
#endif // END OLD TOWER ASSIGNEMENT CODE BLOCK
      }//do pattgen

      //cout << "REGION " << nregion << endl;

      //ConstGenTest: check m_coverage
      if(m_const_test_mode) {
        m_addPattReturnCode=-1;
        if(-999!=nregion){

          m_count_pass_filter++;

          m_int_c.resize(1);
          m_int_phi.resize(1);
          m_int_d0.resize(1);
          m_int_z0.resize(1);
          m_int_eta.resize(1);

          m_Mtmp.nhit=5;
          m_p_ss[m_nplanes]=std::numeric_limits<int>::max();
          m_p_hashss[m_nplanes]=std::numeric_limits<int>::max();

          m_addPattReturnCode=addKDPattern(nregion, m_const_test_mode, m_p_ss,m_p_hashss, 1,m_Mtmp,m_tmphitc,m_tmpxC2,m_tmpxD2,m_tmpxPhi2,m_tmpxCoto2,m_tmpxZ2,m_tmpcovx2,
                                         m_int_c,m_int_phi,m_int_d0,m_int_z0,m_int_eta);

          if(-1!=m_addPattReturnCode){//find sector
            m_count_match++;
            m_the_sectorID = m_addPattReturnCode;
          }
        }

        m_coverage = double(m_count_match)/double(m_count_pass_filter);
        log << MSG::INFO <<" count_pass_filter count_match= "<< m_count_pass_filter << " " << m_count_match <<endmsg;
        log << MSG::INFO <<" coverage= "<< m_coverage <<endmsg;
      }//const test mode


      ///////////////////////////////////////////////////////////////////////////////
      /////////////                   corrgen part                  ////////////////
      //////////////////////////////////////////////////////////////////////////////
      if(m_doPattgen==true){
	m_monval[11]++;

        int npixel=0,nsct=0;

        ///////geopar.m_c
        // This updated by JAA for new code to calculate d0
        m_d = curtrack.getD0();
        m_z0 =curtrack.getZ();

        m_x0 = curtrack.getX() -(curtrack.getQ())*(curtrack.getPY()) ;
        m_y0 = curtrack.getY() +(curtrack.getQ())*(curtrack.getPX()) ;
        m_c = curtrack.getQ()/(sqrt(curtrack.getPX()*curtrack.getPX()+curtrack.getPY()*curtrack.getPY()));
        m_c=(m_c)/2.;
        m_phi = atan2(-m_x0*fabs(curtrack.getQ())/(curtrack.getQ()),m_y0*fabs(curtrack.getQ())/(curtrack.getQ()));
        m_coto = curtrack.getPZ()/sqrt(curtrack.getPX()*curtrack.getPX()+curtrack.getPY()*curtrack.getPY());

        // // //m_d = - 1/((m_c)*2.) + sqrt(m_x0*m_x0+m_y0*m_y0)*fabs(m_c)/(m_c);

        // // //d0 is the distance of beam spot
        // // double a,b;
        // // //for NOMINAL
        // // double beam_x=0;
        // // double beam_y=0;
        // //for data
        // //     double beam_x=-0.324;
        // //     double beam_y=0.628;
        // double tmp_x = curtrack.getX() - beam_x;
        // double tmp_y = curtrack.getY() - beam_y;

        // a=curtrack.getPY()/curtrack.getPX();
        // b=curtrack.getY()-a*curtrack.getX();
        // m_d=fabs(-1*a*beam_x + beam_y - b )/sqrt(a*a + 1);

        // if(tmp_x*curtrack.getPY() - tmp_y*curtrack.getPX() >= 0  ){
        //   m_d = -1*m_d;
        // }

        ////////////////////////////////////////////////////////
        ///////  slice m_file part           ////////////////////
        ////////////////////////////////////////////////////////
        m_int_c.resize(1);
        m_int_phi.resize(1);
        m_int_d0.resize(1);
        m_int_z0.resize(1);
        m_int_eta.resize(1);

        m_int_c[0]=  (m_c-m_par_c_min)*m_par_c_slices/(m_par_c_max-m_par_c_min);
        m_int_phi[0]=(m_phi-m_par_phi_min*M_PI)*m_par_phi_slices/((m_par_phi_max-m_par_phi_min)*M_PI);
        m_int_d0[0]=(m_d-m_par_d0_min)*m_par_d0_slices/(m_par_d0_max-m_par_d0_min);
        m_int_z0[0]=(m_z0-m_par_z0_min)*m_par_z0_slices/(m_par_z0_max-m_par_z0_min);
        m_int_eta[0]=(m_coto-sinh(m_par_eta_min))*m_par_eta_slices/(sinh(m_par_eta_max)-sinh(m_par_eta_min));

        log << MSG::DEBUG << "check 38 int region ntruth c d0 phi z0 coto = " << nregion << " " << nTruth << " " << m_int_c[0] << " " << m_int_d0[0] << " " << m_int_phi[0] << " " << m_int_z0[0] << " " << m_int_eta[0] << endmsg;

        ////////////////////////////////// slice m_file part end

        m_Mtmp.C=m_c;
        m_Mtmp.D=m_d;
        m_Mtmp.Coto=m_coto;
        m_Mtmp.Z=m_z0;

        // region = 3,4,5 (default). Testing: add in region 6!
        if(!m_const_test_mode){
          if(8==m_nbanks){
            if (nregion>=3 && nregion <= 5) {
              while(m_phi<0) m_phi += 2*M_PI;
            }
          }else if(64==m_nbanks){
            if( nregion==9 || nregion==10 || nregion==25 || nregion==26 || nregion==41 || nregion==42 || nregion==57 || nregion==58 ) {
              while(m_phi<0) m_phi += 2*M_PI;
            }
          }else if(32==m_nbanks){
	    if( nregion%8==4 || nregion%8==5 ){
	      if(m_phi<0) m_phi += 2*M_PI;
	    }
	  }
	  if(m_ITkMode){
	    // Play it safe for now since region map is not yet solidified
	    while( m_phi < 0 ) m_phi += 2*M_PI;
	  }
        }

        m_Mtmp.Phi=m_phi;

        log << MSG::DEBUG << "check 35 region ntruth_c d0 phi z0 coto = " << nregion << " " << nTruth << " " << m_c << " " << m_d << " " << m_phi << " " << m_z0 << " " << m_coto << endmsg;
        
        for(int i=0;i<m_nplanes;++i){
          if(2==sechit[i].ndim){
            npixel++;
          }else if(1==sechit[i].ndim){
            nsct++;
          }
        }
        
        log << MSG::DEBUG <<" npixel= "<<npixel<<"nsct= "<<nsct<<endmsg;

        //check constant
        if(m_const_test_mode){
          if(-1!=m_addPattReturnCode){//if find same sector
            m_base_trk.setSectorID(m_the_sectorID);

            int ix,iy;

            for(int j=0;j<npixel;j++){
              ix = 2*j;
              iy = 2*j+1;
              m_base_trk.setCoord(ix,sechit[j].dimx);
              m_base_trk.setCoord(iy,sechit[j].dimy);
            }

            for(int j=0;j<nsct;j++){
              ix = 2*npixel+j;
              m_base_trk.setCoord(ix,sechit[npixel+j].dimx);
            }

            FTKConstantBank *current_bank = m_constant[nregion];

            if (!current_bank) { // no valid constants
              log << MSG::FATAL << "*** Constants for region " << nregion << " - " << 1 << " don't exists" << endmsg;
              return StatusCode::FAILURE;
            }
            /* Do the actual fit - see code in FTKConstantBank::linfit  */
            current_bank->linfit(m_the_sectorID,m_base_trk);

            log << MSG::INFO << "\t\t\tReconstructed parameters\n\t\t\t"	\
                << 2.*m_base_trk.getHalfInvPt() << ' ' << m_base_trk.getIP()		\
                << ' ' << m_base_trk.getPhi() << ' ' << m_base_trk.getZ0()	\
                << ' ' << m_base_trk.getCotTheta() << endmsg;
            log << MSG::INFO << "\t\t\tChi2 " << m_base_trk.getChi2() << endmsg;

            m_file_resolution << m_Mtmp.C - 2.*m_base_trk.getHalfInvPt() << " " << m_Mtmp.D - m_base_trk.getIP() << " " << m_Mtmp.Phi - m_base_trk.getPhi() << " " << m_Mtmp.Z - m_base_trk.getZ0() << " " << m_Mtmp.Coto - m_base_trk.getCotTheta() << " " << m_base_trk.getChi2() << endl;
            m_file_truthpar << nregion << " " << m_Mtmp.C << " " << m_Mtmp.D << " " << m_Mtmp.Phi << " " << m_Mtmp.Z << " " << m_Mtmp.Coto << endl;
            m_file_recpar << nregion << " " << 2.*m_base_trk.getHalfInvPt() << " " << m_base_trk.getIP() << " " << m_base_trk.getPhi() << " " << m_base_trk.getZ0() << " " << m_base_trk.getCotTheta() << endl;

          }

        }else{
          // add a sector to the map of sectors

          std::vector<double> vec(m_TotalDim);

          for(int j=0;j<npixel;j++){
            vec[2*(j)]=sechit[j].dimx;
            vec[2*(j)+1]=sechit[j].dimy;
          }

          for(int j=0;j<nsct;j++){
            vec[2*npixel+j]=sechit[npixel+j].dimx;
          }
          log << MSG::DEBUG <<" x0= "<<m_x0<<"y0= "<<m_y0<<endmsg;
          //	  log << MSG::DEBUG <<" c= "<<m_c<<"d= "<<m_d<<"phi= "<<m_phi<<"z0= "<<m_z0<<"coto= " <<m_coto<<endmsg;

          for(int i=0;i<m_TotalDim;i++){
            //      log << MSG::DEBUG <<" vec= "<<vec[i]<<endmsg;
            m_tmphitc[i]=vec[i];

            m_tmpxC2[i]=m_Mtmp.C*vec[i];
            m_tmpxD2[i]=m_Mtmp.D*vec[i];
            m_tmpxPhi2[i]=m_Mtmp.Phi*vec[i];
            m_tmpxCoto2[i]=m_Mtmp.Coto*vec[i];
            m_tmpxZ2[i]=m_Mtmp.Z*vec[i];

            for( int j = i; j < m_TotalDim; j++ ) {
              m_tmpcovx2[i*m_TotalDim+j]=vec[i]*vec[j];
            }
          }

          if(-999!=nregion){
	    m_monval[12]++;
            m_Mtmp.nhit=1;

            m_addPattReturnCode=addKDPattern(nregion, m_const_test_mode, m_p_ss, m_p_hashss, 1,
                                           m_Mtmp,m_tmphitc,m_tmpxC2,m_tmpxD2,m_tmpxPhi2,m_tmpxCoto2,m_tmpxZ2,m_tmpcovx2,
                                           m_int_c,m_int_phi,m_int_d0,m_int_z0,m_int_eta);

            if(1==m_addPattReturnCode){
              ++m_npatterns[nregion];
            }else if(2==m_addPattReturnCode){
              ++m_ntracks[nregion];
            }
            log << MSG::DEBUG <<"region is "<<nregion<<" #pattern= " <<m_npatterns[nregion]<<" #ntracks= "<<m_ntracks[nregion]<<endmsg;
          }

        }//const test mode
        
        //	sechit.clear();
      }//do pattgen

      sechit.clear();
    }//nTruth
  }//good track

  log << MSG::INFO << "FTKBankGenAlgo::execute() end" << endmsg;
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTKBankGenAlgo::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endmsg;

  ///////////////////////////////////////////
  //sort function and tree                 //
  //////////////////////////////////////////
  if(m_const_test_mode){

    m_file_coverage << "denominator numerator coverage" << endl;
    m_file_coverage << m_count_pass_filter << " " << m_count_match << " " << m_coverage << endl;
      
    m_file_coverage.close();
    m_file_resolution.close();

  }else{//const test mode
      
    for(int b = 0; b<m_nbanks;b++){
      log << MSG::DEBUG <<"bank= "<<b <<endmsg;
      sortPatterns(b);
      for(int i = 0; i<getNPatterns(b);i++){
	m_intc->clear();
	m_intphi->clear();
	m_intd0->clear();
	m_intz0->clear();
	m_inteta->clear();	
	
	log << MSG::DEBUG <<"sector " <<i <<"  ";
	for(int k=0;k<m_nplanes;k++){
	  m_sectorID[k]=getPatternSS(b,i,k);
	  m_hashID[k]=getPatthashID(b,i,k);
	  log <<getPatternSS(b,i,k)<<" ";
	}
	log <<" "<<getPattNHits(b,i) <<endmsg;
	m_nhit=getPattNHits(b,i);
	m_tmpC=getPattC(b,i);
	m_tmpD=getPattD(b,i);
	m_tmpPhi=getPattPhi(b,i);
	m_tmpCoto=getPattCoto(b,i);
	m_tmpZ=getPattZ(b,i);
	
	for(int k=0;k<m_TotalDim;k++){
	  m_tmpVec[k]=getPatthitc(b,i,k);
	  m_tmpxC[k]=getPattxC(b,i,k);
	  m_tmpxD[k]=getPattxD(b,i,k);
	  m_tmpxPhi[k]=getPattxPhi(b,i,k);
	  m_tmpxCoto[k]=getPattxCoto(b,i,k);
	  m_tmpxZ[k]=getPattxZ(b,i,k);
	  
	  for(int l=0;l<m_TotalDim;l++){
	    m_tmpcovx[k*m_TotalDim+l]=getPattcovx(b,i,k*m_TotalDim+l);
	  }
	  log <<getPatthitc(b,i,k)<<" ";
	}
	log <<"  "<<endmsg;
			
	log << MSG::DEBUG <<"b= "<< b <<" i= "<< i << endmsg;
	log << MSG::DEBUG <<"C= "<< getPattC(b,i)   <<"D= "<< getPattD(b,i) <<"Phi= "<< getPattPhi(b,i)
	    <<"Coto= "<< getPattCoto(b,i) <<"Z= "<< getPattZ(b,i)<<endmsg;
	m_nsector=m_npatterns[b];
	for(int m=0;m<m_nhit;m++){
	  m_intc->push_back(getPattintc(b,i,m));
	  m_intphi->push_back(getPattintphi(b,i,m));
	  m_intd0->push_back(getPattintd0(b,i,m));
	  m_intz0->push_back(getPattintz0(b,i,m));
	  m_inteta->push_back(getPattinteta(b,i,m));
	}
	m_file->cd();
	m_tree[b]->Fill();
	gROOT->cd();
      }

      m_file->cd();
      m_tree[b]->Write();   
      gROOT->cd();
      delete m_tree[b];
      m_tree[b]=NULL;
    }
    
    free(m_p_ss);
    free(m_p_hashss);
    free(m_tmphitc);
    free(m_tmpxC2);
    free(m_tmpxD2);
    free(m_tmpxPhi2);
    free(m_tmpxCoto2);
    free(m_tmpxZ2);
    free(m_tmpcovx2);

    m_file->cd();
    m_slicetree->Fill();
    m_slicetree->Write();
    m_montree->Fill();
    m_montree->Write();
    gROOT->cd();
        
    m_file->Close();
    delete m_file;
    m_file=NULL;
    //  }
  }//const test mode  
  void finishPattTree();
  // detach data input and output modules from ConfGen to avoid its deletion
  return StatusCode::SUCCESS;
}
