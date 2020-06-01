/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/MMLoadVariables.h"

#include "TrigT1NSWSimTools/MMT_Finder.h"
#include "TrigT1NSWSimTools/MMT_Fitter.h"
#include "MuonDigitContainer/MmDigitContainer.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"
#include "MuonSimEvent/MM_SimIdToOfflineId.h"
#include "AtlasHepMC/GenEvent.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonIdHelpers/MmIdHelper.h"

#include "TVector3.h"
#include <cmath>

using std::map;
using std::vector;
using std::string;

MMLoadVariables::MMLoadVariables(StoreGateSvc* evtStore, const MuonGM::MuonDetectorManager* detManager, const MmIdHelper* idhelper, MMT_Parameters *par):
      m_msg("MMLoadVariables"){
      m_par = par;
      m_evtStore = evtStore;
      m_detManager = detManager;
      m_MmIdHelper = idhelper;
}

    void MMLoadVariables::getMMDigitsInfo(vector<digitWrapper>& entries, map<hitData_key,hitData_entry>& Hits_Data_Set_Time, map<int,evInf_entry>& Event_Info){
      //*******Following MuonPRD code to access all the variables**********

      histogramVariables fillVars;

      //Get truth variables, vertex
      const McEventCollection *truthContainer = nullptr;
      StatusCode sc1 = m_evtStore->retrieve(truthContainer,"TruthEvent");

      //Get MuEntry variables TBD if still necessary
      const TrackRecordCollection* trackRecordCollection = nullptr;
      StatusCode sc2 = m_evtStore->retrieve(trackRecordCollection,"MuonEntryLayer") ;

      //Get truth information container of digitization
      const MuonSimDataCollection* nsw_MmSdoContainer = nullptr;
      StatusCode sc3 = m_evtStore->retrieve(nsw_MmSdoContainer,"MM_SDO");

      // get digit container (a container corresponds to a multilayer of a module)
      const MmDigitContainer *nsw_MmDigitContainer = nullptr;
      StatusCode sc4 = m_evtStore->retrieve(nsw_MmDigitContainer,"MM_DIGITS");

      std::string wedgeType = getWedgeType(nsw_MmDigitContainer);

      float phiEntry = 0;
      float phiPosition = 0;
      float etaEntry = 0;
      float etaPosition = 0;
 
      TLorentzVector thePart, theInfo;
      TVector3 vertex;
      int pdg=0;
      auto MuEntry_Particle_n = trackRecordCollection->size();
      int j=0; //# iteration of particle entries

      for(const auto it : *truthContainer) {  //first loop in MMT_loader::load_event
        const HepMC::GenEvent *subEvent = it;
        HepMC::ConstGenEventParticleRange particle_range = subEvent->particle_range();
        for(const auto pit : particle_range) {
          const HepMC::GenParticle *particle = pit;
          const HepMC::FourVector momentum = particle->momentum();
          int k=trackRecordCollection->size(); //number of mu entries
          if(particle->barcode() == 10001 && std::abs(particle->pdg_id())==13){
            thePart.SetPtEtaPhiE(momentum.perp(),momentum.eta(),momentum.phi(),momentum.e());
            for(const auto & mit : *trackRecordCollection ) {
              if(k>0&&j<k){
                const CLHEP::Hep3Vector mumomentum = mit.GetMomentum();
                const CLHEP::Hep3Vector muposition = mit.GetPosition();
                pdg=particle->barcode();
                phiEntry = mumomentum.getPhi();
                etaEntry = mumomentum.getEta();
                phiPosition = muposition.getPhi();
                etaPosition = muposition.getEta();
              }
            }//muentry loop
              int l=0;
              HepMC::ConstGenEventVertexRange vertex_range = subEvent->vertex_range();
              for(const auto vit : vertex_range) {
                if(l!=0){break;}//get first vertex of iteration, may want to change this
                const HepMC::GenVertex *vertex1 = vit;
                const HepMC::FourVector position = vertex1->position();
                vertex=TVector3(position.x(),position.y(),position.z());
                l++;
              }//end vertex loop
            }
            j++;
        } //end particle loop
      } //end container loop (should be only 1 container per event)
      const EventInfo* pevt = 0;
      StatusCode sc = m_evtStore->retrieve(pevt);

      int event = pevt->event_ID()->event_number();
      int TruthParticle_n = j;
      evFit_entry fit;fit.athena_event=event;//-1;  **May not be necessary

      //Truth information for theta, phi
      double theta_pos=std::atan(std::exp(-etaPosition))*2;
      double theta_ent=std::atan(std::exp(-etaEntry))*2;
      double phi_pos=phiPosition;

      //get hits container
      const MMSimHitCollection *nswContainer = nullptr;
      StatusCode sc5 = m_evtStore->retrieve(nswContainer,"MicromegasSensitiveDetector");

      for(auto digitCollectionIter : *nsw_MmDigitContainer) {
        // a digit collection is instanciated for each container, i.e. holds all digits of a multilayer
        const MmDigitCollection* digitCollection = digitCollectionIter;
        // loop on all digits inside a collection, i.e. multilayer
        int digit_count =0;

        for (const auto & item:*digitCollection) {
            // get specific digit and identify it
            const MmDigit* digit = item;
            Identifier id = digit->identify();

            Amg::Vector3D hit_gpos(0., 0., 0.);

            std::string stName   = m_MmIdHelper->stationNameString(m_MmIdHelper->stationName(id));
            int stationName      = m_MmIdHelper->stationName(id);
            int stationEta       = m_MmIdHelper->stationEta(id);
            int stationPhi       = m_MmIdHelper->stationPhi(id);
            int multiplet        = m_MmIdHelper->multilayer(id);
            int gas_gap          = m_MmIdHelper->gasGap(id);
            int channel          = m_MmIdHelper->channel(id);

            int isSmall = (stName[2] == 'S');
            const MuonGM::MMReadoutElement* rdoEl = m_detManager->getMMRElement_fromIdFields(isSmall, stationEta, stationPhi, multiplet );

            std::vector<float>  time          = digit->stripTimeForTrigger();
            std::vector<float>  charge        = digit->stripChargeForTrigger();
            std::vector<int>    stripPosition = digit->stripPositionForTrigger();

            std::vector<int>    MMFE_VMM = digit->MMFE_VMM_idForTrigger();
            std::vector<int>    VMM = digit->VMM_idForTrigger();

            bool isValid;

            fillVars.NSWMM_dig_stationEta.push_back(stationEta);
            fillVars.NSWMM_dig_stationPhi.push_back(stationPhi);
            fillVars.NSWMM_dig_multiplet.push_back(multiplet);
            fillVars.NSWMM_dig_gas_gap.push_back(gas_gap);
            fillVars.NSWMM_dig_channel.push_back(channel);

            //match to truth particle
            TLorentzVector truthPart;
            for(auto it1 : *truthContainer) {  //Must be a more elegant way... should work for now though
              const HepMC::GenEvent *subEvent1 = it1;
              HepMC::ConstGenEventParticleRange particle_range1 = subEvent1->particle_range();
              for(auto pit1 : particle_range1) {
                const HepMC::GenParticle *particle1 = pit1;
                const HepMC::FourVector momentum1 = particle1->momentum();
                truthPart.SetPtEtaPhiE(momentum1.perp(),momentum1.eta(),momentum1.phi(),momentum1.e());
              }//end particle loop
            }//end truth container loop (1 iteration) for matching

            std::vector<double> localPosX;
            std::vector<double> localPosY;
            std::vector<double> globalPosX;
            std::vector<double> globalPosY;
            std::vector<double> globalPosZ;

            for (const auto &i: stripPosition) {
              // take strip index form chip information
              int cr_strip = i;
              localPosX.push_back (0.);
              localPosY.push_back (0.);
              globalPosX.push_back(0.);
              globalPosY.push_back(0.);
              globalPosZ.push_back(0.);

              Identifier cr_id = m_MmIdHelper->channelID(stationName, stationEta, stationPhi, multiplet, gas_gap, cr_strip, true, &isValid);
              if (!isValid) {
                ATH_MSG_WARNING("MicroMegas digitization: failed to create a valid ID for (chip response) strip n. " << cr_strip
                               << "; associated positions will be set to 0.0.");
              } else {
                  // asking the detector element to get local position of strip
                  Amg::Vector2D cr_strip_pos(0., 0.);
                  if ( !rdoEl->stripPosition(cr_id,cr_strip_pos) ) {
                    ATH_MSG_WARNING("MicroMegas digitization: failed to associate a valid local position for (chip response) strip n. " << cr_strip
                                   << "; associated positions will be set to 0.0.");
                  } else {
                    localPosX.at(i) = cr_strip_pos.x();
                    localPosY.at(i) = cr_strip_pos.y();
                  }

                  // asking the detector element to transform this local to the global position
                  Amg::Vector3D cr_strip_gpos(0., 0., 0.);
                  rdoEl->surface(cr_id).localToGlobal(cr_strip_pos, Amg::Vector3D(0., 0., 0.), cr_strip_gpos);
                  globalPosX.at(i) = cr_strip_gpos[0];
                  globalPosY.at(i) = cr_strip_gpos[1];
                  globalPosZ.at(i) = cr_strip_gpos[2];

                }

            }//end of strip position loop


            //NTUPLE FILL DIGITS
            fillVars.NSWMM_dig_time.push_back(time);
            fillVars.NSWMM_dig_charge.push_back(charge);
            fillVars.NSWMM_dig_stripPosition.push_back(stripPosition);
            fillVars.NSWMM_dig_stripLposX.push_back(localPosX);
            fillVars.NSWMM_dig_stripLposY.push_back(localPosY);
            fillVars.NSWMM_dig_stripGposX.push_back(globalPosX);
            fillVars.NSWMM_dig_stripGposY.push_back(globalPosY);
            fillVars.NSWMM_dig_stripGposZ.push_back(globalPosZ);
            if(globalPosY.size() == 0) continue;

            int indexOfFastestSignal = -1;

            if( time.size() ) indexOfFastestSignal = 0;

            if(indexOfFastestSignal == -1) continue;
            int hit_count=0;


            MicromegasHitIdHelper* hitHelper = MicromegasHitIdHelper::GetHelper();
            MM_SimIdToOfflineId simToOffline(m_MmIdHelper);
            for( auto it2 : *nswContainer ) { //get hit variables
              const MMSimHit hit = it2;
              fillVars.NSWMM_globalTime.push_back(hit.globalTime());

              const Amg::Vector3D globalPosition = hit.globalPosition();
              if(digit_count==0){
                fillVars.NSWMM_hitGlobalPositionX.push_back(globalPosition.x());
                fillVars.NSWMM_hitGlobalPositionY.push_back(globalPosition.y());
                fillVars.NSWMM_hitGlobalPositionZ.push_back(globalPosition.z());
                fillVars.NSWMM_hitGlobalPositionR.push_back(globalPosition.perp());
                fillVars.NSWMM_hitGlobalPositionP.push_back(globalPosition.phi());
                const Amg::Vector3D globalDirection = hit.globalDirection();
                fillVars.NSWMM_hitGlobalDirectionX.push_back(globalDirection.x());
                fillVars.NSWMM_hitGlobalDirectionY.push_back(globalDirection.y());
                fillVars.NSWMM_hitGlobalDirectionZ.push_back(globalDirection.z());

                fillVars.NSWMM_particleEncoding.push_back(hit.particleEncoding());
                fillVars.NSWMM_kineticEnergy.push_back(hit.kineticEnergy());
                fillVars.NSWMM_depositEnergy.push_back(hit.depositEnergy());
              }

              int simId = hit.MMId();
              std::string sim_stationName = hitHelper->GetStationName(simId);
              int sim_stationEta  = hitHelper->GetZSector(simId);
              int sim_stationPhi  = hitHelper->GetPhiSector(simId);
              int sim_multilayer  = hitHelper->GetMultiLayer(simId);
              int sim_layer       = hitHelper->GetLayer(simId);
              int sim_side        = hitHelper->GetSide(simId);

              // Fill Ntuple with SimId data
              //fillVars.NSWMM_sim_stationName .push_back(sim_stationName);
              if(digit_count){
                fillVars.NSWMM_sim_stationEta  .push_back(sim_stationEta);
                fillVars.NSWMM_sim_stationPhi  .push_back(sim_stationPhi);
                fillVars.NSWMM_sim_multilayer  .push_back(sim_multilayer);
                fillVars.NSWMM_sim_layer       .push_back(sim_layer);
                fillVars.NSWMM_sim_side        .push_back(sim_side);
              }

              if(hit.depositEnergy()==0.) continue; // SimHits without energy loss are not recorded.
              if(digit_count==hit_count) {
                entries.push_back(
                  digitWrapper(digit,
                               hit.globalTime(),
                               TVector3(-999, -99, -999),//Digits_MM_truth_localPosZ->at(i)),
                               TVector3(localPosX.at(indexOfFastestSignal),
                                        localPosY.at(indexOfFastestSignal),
                                        -999),  //Digits_MM_stripLposZ->at(i).at(indexOfFastestSignal)),
                               TVector3(globalPosX.at(indexOfFastestSignal),
                                        globalPosY.at(indexOfFastestSignal),
                                        globalPosZ.at(indexOfFastestSignal) )
                               )
                  );
              }
              hit_count++;
            }//end of hit cotainer loop */
         // } //end if sdo
        digit_count++;
        } //end iterator digit loop
      } // end digit container loop (1 for event?)

      vector<digitWrapper> dummy;
      vector<int> indices;
      //Truth info for particle (originally primer)
      int phiSt = 0;
      if(entries.size() > 0) phiSt = m_MmIdHelper->stationPhi( entries.at(0).id() );
      evInf_entry particle_info(event,pdg,thePart.E(),thePart.Pt(),thePart.Eta(),etaPosition,etaEntry,phi_shift(thePart.Phi(),wedgeType,phiSt),phi_shift(phi_pos,wedgeType,phiSt),phi_shift(phiEntry,wedgeType,phiSt),thePart.Theta(),theta_pos,theta_ent,theta_ent-theta_pos,TruthParticle_n,MuEntry_Particle_n,vertex);
      if(wedgeType == "Neither") particle_info.bad_wedge=true;
      else particle_info.bad_wedge=false;

      vector<digitWrapper> origEntries = entries;

      for(unsigned int i=0; i<entries.size(); i++){
        if(entries.size() < 8) continue;
        Identifier tmpID = entries.at(i).id();
        if     ( m_MmIdHelper->multilayer( tmpID )==1 and m_MmIdHelper->gasGap(tmpID)==1) entries.at(i).gTime = origEntries.at(0).gTime;
        else if( m_MmIdHelper->multilayer( tmpID )==1 and m_MmIdHelper->gasGap(tmpID)==2) entries.at(i).gTime = origEntries.at(1).gTime;
        else if( m_MmIdHelper->multilayer( tmpID )==1 and m_MmIdHelper->gasGap(tmpID)==3) entries.at(i).gTime = origEntries.at(2).gTime;
        else if( m_MmIdHelper->multilayer( tmpID )==1 and m_MmIdHelper->gasGap(tmpID)==4) entries.at(i).gTime = origEntries.at(3).gTime;
        else if( m_MmIdHelper->multilayer( tmpID )==2 and m_MmIdHelper->gasGap(tmpID)==1) entries.at(i).gTime = origEntries.at(4).gTime;
        else if( m_MmIdHelper->multilayer( tmpID )==2 and m_MmIdHelper->gasGap(tmpID)==2) entries.at(i).gTime = origEntries.at(5).gTime;
        else if( m_MmIdHelper->multilayer( tmpID )==2 and m_MmIdHelper->gasGap(tmpID)==3) entries.at(i).gTime = origEntries.at(6).gTime;
        else if( m_MmIdHelper->multilayer( tmpID )==2 and m_MmIdHelper->gasGap(tmpID)==4) entries.at(i).gTime = origEntries.at(7).gTime;
      }
      for(unsigned int i=0; i<entries.size(); i++){
        float min = 100000;
        int minIndex=-999;
        for(unsigned int j=0; j<entries.size(); j++){
          bool notmindex = true;
          for (unsigned int k=0; k<indices.size(); k++){
            if(j==(unsigned int) indices[k]) notmindex=false;
          }
          if(notmindex){
            if(min > entries.at(j).gTime ){
              minIndex = j;
              min = entries.at(minIndex).gTime ;
            }
          }
        }
        if(minIndex < 0) { minIndex = i; }
        dummy.push_back(entries.at(minIndex));
        indices.push_back(minIndex);
      }

      entries = dummy;
      int min_hits = 1,max_hits = 10000,nent=entries.size();

      m_uvxxmod=(m_par->setup.compare("xxuvuvxx")==0);
      //Number of hits cut
      if(!particle_info.bad_wedge)     particle_info.pass_cut=true; //default is false
      if(nent<min_hits||nent>max_hits) particle_info.pass_cut=false;

      double tru_phi = -999;
      if (entries.size() >0) tru_phi=phi_shift(thePart.Phi(),wedgeType,phiSt );
      double tru_theta=thePart.Theta();

      //Hit information in Stephen's code... Starts getting a little weird.
      map<hitData_key,hitData_entry> hit_info; //Originally "targaryen"
      vector<hitData_key> keys;
      //Loop over entries, which has digitization info for each event
      for(unsigned int ient=0; ient<entries.size(); ient++){
        digitWrapper thisSignal=entries.at(ient);
        Identifier tmpID      = thisSignal.id();
        int thisMultiplet     = m_MmIdHelper->multilayer( tmpID );
        int thisGasGap        = m_MmIdHelper->gasGap( tmpID );
        int thisTime          = thisSignal.digit->stripTimeForTrigger().at(0);
        int thisCharge        = 2; //thisSignal.digit->stripChargeForTrigger().at(0);
        int thisStripPosition = thisSignal.digit->stripPositionForTrigger().at(0);
        int thisStationEta    = m_MmIdHelper->stationEta( tmpID );
        //DLM_NEW plane assignments
        //stated [3,2,1,0;7,6,5,4]
        int thisPlane = (thisMultiplet-1)*4+thisGasGap-1;

        int strip = strip_number(thisStationEta,
                                 thisPlane,
                                 thisStripPosition);
        int thisVMM = Get_VMM_chip(strip);
        int BC_id = std::ceil( thisTime / 25. );
        TVector3 mazin_check(
          thisSignal.strip_gpos.X(),
          thisSignal.strip_gpos.Y(),
          thisSignal.strip_gpos.Z()
          );

        TVector3 athena_tru(
          thisSignal.strip_gpos.X(),
          thisSignal.strip_gpos.Y()-thisSignal.truth_lpos.Y(),
          thisSignal.strip_gpos.Z());

        if(m_par->dlm_new){
          athena_tru.SetX(thisSignal.strip_gpos.X()-thisSignal.strip_lpos.X());
        }

        TVector3 athena_rec(thisSignal.strip_gpos);
        //now store some variables BLC initializes; we might trim this down for efficiency later
        //the following line should be rather easy to one-to-one replace

        TVector3 truth(athena_tru.Y(),-athena_tru.X(),athena_tru.Z()), recon(athena_rec.Y(),-athena_rec.X(),athena_rec.Z());

        if(m_uvxxmod){
          xxuv_to_uvxx(truth,thisPlane);xxuv_to_uvxx(recon,thisPlane);
        }

        //we're doing everything by the variable known as "athena_event" to reflect C++ vs MATLAB indexing
        int btime=(event+1)*10+(BC_id-1);
        particle_info.NUV_bg_preVMM = 0;   //thisSignal.gtime;
        int special_time = thisTime + (event+1)*100;

        hitData_entry hit_entry(event,
                             thisSignal.gTime,
                             thisCharge,
                             thisVMM,
                             thisPlane,
                             thisStripPosition,
                             thisStationEta,
                             tru_theta,
                             tru_phi,
                             true,
                             btime,
                             special_time,
                             mazin_check,
                             mazin_check);

        hit_info[hit_entry.entry_key()]=hit_entry;
        ATH_MSG_DEBUG("Filling hit_info slot: ");
        if (msgLvl(MSG::DEBUG)){
          hit_entry.entry_key().print();
        }
        keys.push_back(hit_entry.entry_key()); //may be only used when "incoherent background" is generated (not included for now)

      }//end entries loop

      particle_info.N_hits_preVMM=hit_info.size();
      particle_info.N_hits_postVMM=0;
      // unsigned int ir=0;

      //might want to move these somewhere smarter in future
      m_VMM_Deadtime = 100;
      m_numVMM_PerPlane = 1000;
      m_VMM_ChipStatus=vector<vector<bool> >(m_numVMM_PerPlane,vector<bool>(8,true));
      m_VMM_ChipLastHitTime=vector<vector<int> >(m_numVMM_PerPlane,vector<int>(8,0));

      //*** FIGURE OUT WHAT TO DO WITH THE TIES--IS MIMIC VMM BUSTED? DO WE PLACE THE HIT REQUIREMENTS HERE? (PROBABLY)
      int xhit=0,uvhit=0,strip_X_tot=0,strip_UV_tot=0;
      vector<bool>plane_hit(m_par->setup.size(),false);

      for(map<hitData_key,hitData_entry>::iterator it=hit_info.begin(); it!=hit_info.end(); ++it){
        int plane=it->second.plane;
        plane_hit[plane]=true;
        particle_info.N_hits_postVMM++;
        Hits_Data_Set_Time[it->first]=it->second;
        if(m_par->setup.substr(plane,1).compare("x")==0){
          ATH_MSG_DEBUG("ADD X STRIP VALUE "<<it->second.strip);
          strip_X_tot+=it->second.strip;
        }
        else{
          strip_UV_tot+=it->second.strip;
        }
      }//end map iterator loop

      for(unsigned int ipl=0;ipl<plane_hit.size();ipl++){
        if(plane_hit[ipl]){
          if(m_par->setup.substr(ipl,1)=="x") xhit++;
          else if(m_par->setup.substr(ipl,1)=="u"||m_par->setup.substr(ipl,1)=="v") uvhit++;
        }
      }
      
      particle_info.N_X_hits=xhit;
      particle_info.N_UV_hits=uvhit;
      //X and UV hits minumum cut
      if(xhit<m_par->CT_x) particle_info.pass_cut=false;//return;
      if(uvhit<m_par->CT_uv) particle_info.pass_cut=false;//return;

      //Moved the removing of some entries to the end of ~Trigger
      Event_Info[event]=particle_info;
      histVars = fillVars;

      }

  double MMLoadVariables::phi_shift(double athena_phi,std::string wedgeType, int stationPhi) const{
    float n = 2*(stationPhi-1);
    if(wedgeType=="Small") n+=1;
    float sectorPi = n*M_PI/8.;
    if(n>8) sectorPi = (16.-n)*M_PI/8.;

    if(n<8)       return (athena_phi-sectorPi);
    else if(n==8) return (athena_phi + (athena_phi >= 0? -1:1)*sectorPi);
    else if(n>8)  return (athena_phi+sectorPi);
    else return athena_phi;

  }
  void MMLoadVariables::xxuv_to_uvxx(TVector3& hit,const int plane) const{
    if(plane<4)return;
    else if(plane==4)hit_rot_stereo_bck(hit);//x to u
    else if(plane==5)hit_rot_stereo_fwd(hit);//x to v
    else if(plane==6)hit_rot_stereo_fwd(hit);//u to x
    else if(plane==7)hit_rot_stereo_bck(hit);//v to x
  }

  void MMLoadVariables::hit_rot_stereo_fwd(TVector3& hit)const{
    double degree=TMath::DegToRad()*(m_par->stereo_degree.getFixed());
    if(m_striphack) hit.SetY(hit.Y()*cos(degree));
    else{
      double xnew=hit.X()*std::cos(degree)+hit.Y()*std::sin(degree),ynew=-hit.X()*std::sin(degree)+hit.Y()*std::cos(degree);
      hit.SetX(xnew);hit.SetY(ynew);
    }
  }

  void MMLoadVariables::hit_rot_stereo_bck(TVector3& hit)const{
    double degree=-TMath::DegToRad()*(m_par->stereo_degree.getFixed());
    if(m_striphack) hit.SetY(hit.Y()*std::cos(degree));
    else{
      double xnew=hit.X()*std::cos(degree)+hit.Y()*std::sin(degree),ynew=-hit.X()*std::sin(degree)+hit.Y()*std::cos(degree);
      hit.SetX(xnew);hit.SetY(ynew);
    }
  }


  int 
  MMLoadVariables::Get_Strip_ID(double X,double Y,int plane) const{  //athena_strip_id,module_y_center,plane)
    if(Y==-9999) return -1;
    string setup(m_par->setup);
    double strip_width=m_par->strip_width.getFixed(), degree=TMath::DegToRad()*(m_par->stereo_degree.getFixed());//,vertical_strip_width_UV = strip_width/cos(degree);
    double y_hit=Y;
    int setl=setup.length();
    if(plane>=setl||plane<0){
      ATH_MSG_FATAL("Pick a plane in [0,"<<setup.length()<<"] not "<<plane); 
      exit(1);
    }
    string xuv=setup.substr(plane,1);
    if(xuv=="u"){//||xuv=="v"){
      if(m_striphack)return ceil(Y*cos(degree)/strip_width);
      y_hit = X*sin(degree)+Y*cos(degree);
    }
    else if(xuv=="v"){
      if(m_striphack)return ceil(Y*cos(degree)/strip_width);
      y_hit = -X*sin(degree)+Y*cos(degree);
    }
    else if(xuv!="x"){
      ATH_MSG_FATAL("Invalid plane option " << xuv ); 
      exit(2);
    }
    double strip_hit = ceil(y_hit*1./strip_width);
    return strip_hit;
  }

  int 
  MMLoadVariables::Get_VMM_chip(int strip) const{  //Not Finished... Rough
    int strips_per_VMM = 64;
    return ceil(1.*strip/strips_per_VMM);
  }

  int 
  MMLoadVariables::strip_number(int station,int plane,int spos)const{
    if (station<=0||station>m_par->n_stations_eta) {
      int base_strip = 0;
      return base_strip;
    }
    if (plane<0||plane>(int)m_par->setup.size()) {
      int base_strip = 0;

      return base_strip;
    }
   
    int base_strip=spos;
    return base_strip;
  }
  
  std::string 
  MMLoadVariables::getWedgeType(const MmDigitContainer *nsw_MmDigitContainer){
    std::vector<bool> isLargeWedge;
    //Digit loop to match to truth
    for(auto digitCollectionIter : *nsw_MmDigitContainer) {

      const MmDigitCollection* digitCollection = digitCollectionIter;
      for (unsigned int item=0; item<digitCollection->size(); item++) {

        const MmDigit* digit = digitCollection->at(item);
        Identifier id = digit->identify();

          std::string stName   = m_MmIdHelper->stationNameString(m_MmIdHelper->stationName(id));
          string sname(stName);
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
