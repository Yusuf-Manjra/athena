/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// local includes
#include "TrigT1NSWSimTools/MMTriggerTool.h"
#include "TrigT1NSWSimTools/MMT_Finder.h"
#include "TrigT1NSWSimTools/MMT_Fitter.h"
// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

//Muon software includes
#include "MuonDigitContainer/MmDigit.h"

//Event info includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// ROOT includes
#include "TTree.h"


using std::vector;
using std::map;
using std::pair;
using std::string;
using std::ios;
using std::setfill;
using std::setw;


namespace NSWL1 {

  MMTriggerTool::MMTriggerTool( const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type,name,parent),
    m_incidentSvc("IncidentSvc",name),
    m_detManager(0),
    m_MmIdHelper(0),
    m_MmDigitContainer(""),
    m_doNtuple(false),
    m_tree(0)
  {
    declareInterface<NSWL1::IMMTriggerTool>(this);
    declareProperty("MM_DigitContainerName", m_MmDigitContainer = "MM_DIGITS", "the name of the MM digit container");
    declareProperty("DoNtuple", m_doNtuple = true, "input the MMStripTds branches into the analysis ntuple");

  }

  MMTriggerTool::~MMTriggerTool() {

  }

  StatusCode MMTriggerTool::initialize() {

    ATH_MSG_INFO( "initializing -- " << name() );

    ATH_MSG_INFO( name() << " configuration:");
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_MmDigitContainer.name() << m_MmDigitContainer.value());
    ATH_MSG_INFO(" " << setw(32) << setfill('.') << setiosflags(ios::left) << m_doNtuple.name() << ((m_doNtuple)? "[True]":"[False]")
                     << setfill(' ') << setiosflags(ios::right) );


    const IInterface* parent = this->parent();
    const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
    const std::string& algo_name = pnamed->name();
    if ( m_doNtuple && algo_name=="NSWL1Simulation" ) {
      ITHistSvc* tHistSvc;
      ATH_CHECK( service("THistSvc", tHistSvc) );

      char ntuple_name[40];
      memset(ntuple_name,'\0',40*sizeof(char));
      sprintf(ntuple_name,"%sTree",algo_name.c_str());

      m_tree = 0;
      ATH_CHECK( tHistSvc->getTree(ntuple_name,m_tree) );
      ATH_MSG_INFO("Analysis ntuple succesfully retrieved");
      ATH_CHECK( this->book_branches() );

    } else this->clear_ntuple_variables();

    // retrieve the Incident Service
    if( m_incidentSvc.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve the Incident Service");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Incident Service successfully rertieved");
    }
    m_incidentSvc->addListener(this,IncidentType::BeginEvent);

    //  retrieve the MuonDetectormanager
    if( detStore()->retrieve( m_detManager ).isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve the MuonDetectorManager");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("MuonDetectorManager successfully retrieved");
    }

    //  retrieve the Mm offline Id helper
    if( detStore()->retrieve( m_MmIdHelper ).isFailure() ){
      ATH_MSG_FATAL("Failed to retrieve MmIdHelper");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("MmIdHelper successfully retrieved");
    }

    //Calculate and retrieve wedge geometry, defined in MMT_struct

    const par_par standard=par_par(0.0009,4,4,0.0035,"xxuvxxuv",true);
    const par_par xxuvuvxx=par_par(0.0009,4,4,0.007,"xxuvuvxx",true,true); //.0035 for uv_tol before...
    const par_par xxuvuvxx_uvroads=par_par(0.0009,4,4,0.0035,"xxuvuvxx",true,true); //.0035 for uv_tol before...

    // par_par pars=dlm;
    m_par_large = new MMT_Parameters(xxuvuvxx,'L', m_detManager); // Need to figure out where to delete this!! It's needed once per run
    m_par_small = new MMT_Parameters(xxuvuvxx,'S', m_detManager); // Need to figure out where to delete this!! It's needed once per run

    return StatusCode::SUCCESS;
  }


  StatusCode MMTriggerTool::runTrigger() {

      //Retrieve the current run number and event number
      const EventInfo* pevt = 0;
      ATH_CHECK( evtStore()->retrieve(pevt) );
      int event = pevt->event_ID()->event_number();

      //////////////////////////////////////////////////////////////
      //                                                          //
      // Load Variables From Containers into our Data Structures  //
      //                                                          //
      //////////////////////////////////////////////////////////////

      map<hitData_key,hitData_entry> Hits_Data_Set_Time;
      map<int,evInf_entry> Event_Info;

      const MmDigitContainer *nsw_MmDigitContainer = nullptr;
      ATH_CHECK( evtStore()->retrieve(nsw_MmDigitContainer,"MM_DIGITS") );

      std::string wedgeType = getWedgeType(nsw_MmDigitContainer);
      if(wedgeType=="Large") m_par = m_par_large;
      if(wedgeType=="Small") m_par = m_par_small;
      if(wedgeType=="Neither") {
        ATH_MSG_INFO( "SMALL AND LARGE!! Event did (NOT) pass " );
        return StatusCode::SUCCESS;
      }

      MMLoadVariables load = MMLoadVariables(&(*(evtStore())), m_detManager, m_MmIdHelper, m_par);

      std::vector<digitWrapper> entries;

      load.getMMDigitsInfo(entries, Hits_Data_Set_Time, Event_Info);
      this->fillNtuple(load);

      //Originally boom, this is the saved "particle_info" (originally primer)
      evInf_entry truth_info(Event_Info.find(pevt->event_ID()->event_number())->second);

      bool pass_cuts = truth_info.pass_cut;
      double trueta = truth_info.eta_ip;
      double trupt = truth_info.pt;


      double tent=truth_info.theta_ent;
      double tpos=truth_info.theta_pos;
      double ppos=truth_info.phi_pos;
      double dt=truth_info.dtheta;
      m_trigger_trueThe->push_back(tent);
      m_trigger_truePhi->push_back(ppos);
      m_trigger_trueDth->push_back(dt);

      //from MMT_Loader >>>> If entry matches find(event) adds element to vector
      std::vector<hitData_entry> hitDatas(event_hitDatas(event,Hits_Data_Set_Time));
      //Only consider fits if they satisfy CT and fall in wedge
      if(pass_cuts){
      //Make sure hit info is not empy
      if(!hitDatas.empty()){

        //////////////////////////////////////////////////////////////
        //                                                          //
        //                Finder Applied Here                       //
        //                                                          //
        //////////////////////////////////////////////////////////////

        //Initialization of the finder: defines all the roads
        MMT_Finder find = MMT_Finder(m_par, 1);

        ATH_MSG_DEBUG(  "Number of Roads Configured " <<  find.get_roads()  );

        //Convert hits to slopes and fill the buffer
        map<pair<int,int>,finder_entry> hitBuffer;
        for(int ihds=0; ihds<(int)hitDatas.size(); ihds++){
          find.fillHitBuffer( hitBuffer,                       // Map (road,plane) -> Finder entry
                                hitDatas[ihds].entry_hit(m_par) );  // Hit object

          hitData_info hitInfo = hitDatas[ihds].entry_hit(m_par).info;

          m_trigger_VMM->push_back(hitDatas[ihds].VMM_chip);
          m_trigger_plane->push_back(hitDatas[ihds].plane);
          m_trigger_station->push_back(hitDatas[ihds].station_eta);
          m_trigger_strip->push_back(hitDatas[ihds].strip);
          m_trigger_slope->push_back(hitInfo.slope.getFixed());

        }
        if(hitDatas.size()==8){
          m_trigger_trueEtaRange->push_back(trueta);
          m_trigger_truePtRange->push_back(trupt);
          if(wedgeType=="Large") {
            m_trigger_large_trueEtaRange->push_back(trueta);
            m_trigger_large_truePtRange->push_back(trupt);
          }
          if(wedgeType=="Small") {
            m_trigger_small_trueEtaRange->push_back(trueta);
            m_trigger_small_truePtRange->push_back(trupt);
          }

        }

        //////////////////////////////////////////////////////////////
        //                                                          //
        //                 Fitter Applied Here                      //
        //                                                          //
        //////////////////////////////////////////////////////////////

        MMT_Fitter fit = MMT_Fitter(m_par);

        //First loop over the roads and planes and apply the fitter
        int fits_occupied=0;
        const int nfit_max=1;  //MOVE THIS EVENTUALLY
        int nRoads = find.get_roads();

        vector<evFit_entry> road_fits = vector<evFit_entry>(nRoads,evFit_entry());

        //Variables saved for Alex T. for hardware validation
        double mxl;
        double fillmxl=-999;
        double muGlobal;
        double mvGlobal;
        vector<pair<double,double> > mxmy;

        for(int iRoad=0; iRoad<nRoads; iRoad++){

          vector<bool> plane_is_hit;
          vector<Hit> track;

          //Check if there are hits in the buffer
          find.checkBufferForHits(  plane_is_hit, // Empty, To be filled by function.
                                      track, // Empty, To be filled by function.
                                      iRoad, // roadID
                                      hitBuffer // All hits. Map ( (road,plane) -> finder_entry  )
                                    );

          //Look for coincidences
          int road_num=find.Coincidence_Gate(plane_is_hit);

          if(road_num>0){

            if(fits_occupied>=nfit_max) break;

            //Perform the fit -> calculate local, global X, UV slopes -> calculate ROI and TriggerTool signal (theta, phi, deltaTheta)
            evFit_entry candidate=fit.fit_event(event,track,hitDatas,fits_occupied,mxmy,mxl,mvGlobal,muGlobal);

            ATH_MSG_DEBUG( "THETA " << candidate.fit_theta.getFixed() << " PHI " << candidate.fit_phi.getFixed() << " DTH " << candidate.fit_dtheta.getFixed() );
            road_fits[iRoad]=candidate;
            fillmxl = mxl;
            fits_occupied++;

          }

          road_fits[iRoad].hcode=road_num;

        } //end roads

        //////////////////////////////////////////////////////////////
        //                                                          //
        //              Pass the ROI as Signal                      //
        //                                                          //
        //////////////////////////////////////////////////////////////


        if(road_fits.size()==0 and hitDatas.size()==8 ) {
          ATH_MSG_DEBUG( "TruthRF0 " << tpos     << " " << ppos   << " " << dt << " " << trueta );
        }
        for(unsigned int i=0; i<road_fits.size(); i++){
          if(road_fits[i].fit_roi==0 and hitDatas.size()==8) {
            ATH_MSG_DEBUG( "TruthROI0 " << tpos     << " " << ppos   << " " << dt << " " << trueta );
          }
          if(road_fits[i].fit_roi>0){
            //For the future: how do we want these to pass on as the signal?  Some new data structure?
            double fitTheta      = road_fits[i].fit_theta.getFixed();
            double fitPhi        = road_fits[i].fit_phi.getFixed();
            double fitDeltaTheta = road_fits[i].fit_dtheta.getFixed();


            ATH_MSG_DEBUG( "Truth " << tpos     << " " << ppos   << " " << dt );
            ATH_MSG_DEBUG( "FIT!! " << fitTheta << " " << fitPhi << " " << fitDeltaTheta );
            m_trigger_fitThe->push_back(fitTheta);
            m_trigger_fitPhi->push_back(fitPhi);
            m_trigger_fitDth->push_back(fitDeltaTheta);

            m_trigger_resThe->push_back(fitTheta-tpos);
            m_trigger_resPhi->push_back(fitPhi-ppos);
            m_trigger_resDth->push_back(fitDeltaTheta-dt);

            m_trigger_mx->push_back(mxmy.front().first);
            m_trigger_my->push_back(mxmy.front().second);
            m_trigger_mxl->push_back(fillmxl);

            m_trigger_mu->push_back(muGlobal);
            m_trigger_mv->push_back(mvGlobal);

            m_trigger_fitEtaRange->push_back(trueta);
            m_trigger_fitPtRange->push_back(trupt);
          if(wedgeType=="Large") {
            m_trigger_large_fitEtaRange->push_back(trueta);
            m_trigger_large_fitPtRange->push_back(trupt);
          }
          if(wedgeType=="Small") {
            m_trigger_small_fitEtaRange->push_back(trueta);
            m_trigger_small_fitPtRange->push_back(trupt);
          }

          }//fit roi > 0
        } // end road_fits
      }//end if hitDataS EMPTY
    }//end if PASS_CUTS

    //clear pointers, filled hit info

    Event_Info.erase(Event_Info.find(event));
    vector<hitData_key> kill_keys(event_hitData_keys(event,Hits_Data_Set_Time));
    return StatusCode::SUCCESS;
  }

  //Function that find the hits information and hits keys that get stored throughout the run.
  //The data structures are defined in MMT_struct

  vector<hitData_key> MMTriggerTool::event_hitData_keys(int find_event, map<hitData_key,hitData_entry>& Hits_Data_Set_Time) const{
    vector<hitData_key> ravel;
    int fnd_entries=0;
    for(map<hitData_key,hitData_entry>::const_iterator entry=Hits_Data_Set_Time.begin(); entry!=Hits_Data_Set_Time.end(); ++entry){
      if(entry->second.event==find_event){
        ravel.push_back(entry->first);
        fnd_entries++;
      }
      else if(fnd_entries>0) break;//we use the fact that maps store things according to the strict weak ordering of the key's comparison operator
    }
    return ravel;
  }

  vector<hitData_entry> MMTriggerTool::event_hitDatas(int find_event, map<hitData_key,hitData_entry>& Hits_Data_Set_Time) const{
    vector<hitData_entry> bolero;
    int fnd_entries=0;
    for(map<hitData_key,hitData_entry>::const_iterator entry=Hits_Data_Set_Time.begin(); entry!=Hits_Data_Set_Time.end(); ++entry){
      if(entry->second.event==find_event){
        bolero.push_back(entry->second);
        fnd_entries++;
      }
      else if(fnd_entries>0) break;//we use the fact that maps store things according to the strict weak ordering of the key's comparison operator
    }
    return bolero;
  }

  std::string MMTriggerTool::getWedgeType(const MmDigitContainer *nsw_MmDigitContainer){

    std::vector<bool> isLargeWedge;
    //Digit loop to match to truth
    for(auto dit : *nsw_MmDigitContainer) {

      const MmDigitCollection* coll = dit;
      for (unsigned int item=0; item<coll->size(); item++) {

        const MmDigit* digit = coll->at(item);
        Identifier Id = digit->identify();

          std::string stName   = m_MmIdHelper->stationNameString(m_MmIdHelper->stationName(Id));
          const string& sname(stName);
          if (sname.compare("MML")==0)isLargeWedge.push_back(true);
          else isLargeWedge.push_back(false);
      }
    }

    bool allLarge = true;
    bool allSmall = true;
    for(unsigned int i=0; i<isLargeWedge.size(); i++){
      if (isLargeWedge.at(i)) allSmall = false;
      else allLarge = false;
    }
    std::string wedgeType = "Neither";
    if (allLarge) wedgeType = "Large";
    if (allSmall) wedgeType = "Small";
    return wedgeType;
  }
}//end namespace

