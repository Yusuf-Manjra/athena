/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill histograms showing pixel hit level information
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "InDetIdentifier/PixelID.h"
#include "TH1F.h"   
#include "TH1I.h"   
#include "TH2F.h"
#include "TH3F.h"
#include "TH2I.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "TProfile.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"
#include <sstream>

#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h" 
#include "PixelMonitoring/PixelMon2DMaps.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "PixelMonitoring/PixelMon2DProfilesLW.h"
#include "PixelMonitoring/DBMMon2DMaps.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "PixelMonitoring/PixelMon2DLumiProfiles.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"
#include "PixelCabling/IPixelCablingSvc.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::BookHitsMon(void)
{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Hits" << endmsg;  
 
   std::string path = "Pixel/Hits";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/HitsOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/HitsOnPixelTrack");
   MonGroup rdoShift(   this, path.c_str(),  run, ATTRIB_MANAGED ); //declare a group of histograms
   MonGroup rdoExpert ( this, path.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms

   std::string pathT = "Pixel/Timing";
   if(m_doOnTrack) pathT.replace(pathT.begin(), pathT.end(), "Pixel/TimingOnTrack");
   if(m_doOnPixelTrack) pathT.replace(pathT.begin(), pathT.end(), "Pixel/TimingOnPixelTrack");
   MonGroup timeShift(  this, pathT.c_str(),  run, ATTRIB_MANAGED ); //declare a group of histograms
   MonGroup timeExpert( this, pathT.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms
   
   
   std::string tmp;
   std::string tmp2;
   std::string atitles; 
   std::string hname;
   std::string htitles;
   
   int nbins_LB = m_lbRange;     double min_LB  = -0.5;   double max_LB = min_LB + (1.0*nbins_LB);        
   int nbins_BCID = m_bcidRange; double min_BCID  = -0.5; double max_BCID = min_BCID + (1.0*nbins_BCID);        
   int nbins_tot3 = 300; double min_tot3 = -0.5; double max_tot3 = min_tot3 + (1.0*nbins_tot3);
   int nbins_tot4 = 20;  double min_tot4 = -0.5; double max_tot4 = min_tot4 + (1.0*nbins_tot4);

   static constexpr int nmod_phi[PixLayer::COUNT] = {48, 48, 22, 38, 52, 14};
   static constexpr int nmod_eta[PixLayer::COUNT] = {3, 3, 13, 13, 13, 20};

   std::string atext_LB = ";lumi block"; 
   std::string atext_BCID = ";BCID"; 
   std::string atext_nevt = ";# events"; 
   std::string atext_nhit = ";# hits"; 
   std::string atext_hit = ";# hits/event"; 
   std::string atext_occ = ";# hits/pixel/event"; 
   std::string atext_tot = ";ToT [BC]"; 
   std::string atext_lv1 = ";Level 1 Accept"; 

   StatusCode sc;

   hname = makeHistname("Hits_per_lumi", false);
   htitles = makeHisttitle("Average number of pixel hits per event", (atext_LB+atext_hit), false);
   sc = rdoShift.regHist(m_hits_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   hname = makeHistname("AvgOcc_per_lumi", false);
   htitles = makeHisttitle("Average pixel occupancy per event over all layers", (atext_LB+atext_occ), false);
   sc = rdoShift.regHist(m_avgocc_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   hname = makeHistname("AvgOcc_RatioIBLB0_per_lumi", false);
   htitles = makeHisttitle("Average pixel occupancy ratio of IBL/B0 per event per LB", (atext_LB+";ratio"), false);
   sc = rdoShift.regHist(m_avgocc_ratioIBLB0_per_lumi = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));


   for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){ // not include IBL2D and IBL3D
      hname = makeHistname(("Hits_per_lumi_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Average number of pixel hits per event, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_LB+atext_hit), false);
      sc = rdoExpert.regHist(m_hits_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      hname = makeHistname(("nHits_per_module_per_event_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Number of hits in a module in an event, "+m_modLabel_PixLayerIBL2D3D[i]), ";#hits in a module in an event;#events #times #modules", false);
      sc = rdoShift.regHist(m_nhits_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 1000, -0.5, -0.5+1000.0));

      hname = makeHistname(("Hit_ToTMean_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Hit ToT Mean, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_LB+";Average Hit ToT"), false);
      sc = rdoExpert.regHist( m_hit_ToTMean_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB) );
   }

   for(int i=0; i<PixLayerIBL2D3D::COUNT; i++){
      if(!m_doIBL && i >= PixLayerIBL2D3D::kIBL ) continue;

      hname = makeHistname(("AvgOcc_per_lumi_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Average pixel occupancy per event, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_LB+atext_occ), false);
      sc = rdoExpert.regHist(m_avgocc_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      hname = makeHistname(("AvgOcc_per_BCID_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Average pixel occupancy per BCID, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_BCID+atext_occ), false);
      sc = rdoExpert.regHist(m_avgocc_per_bcid_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

      hname = makeHistname(("AvgOcc_active_per_lumi_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Average pixel occupancy for active per event, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_LB+atext_occ), false);
      sc = rdoExpert.regHist(m_avgocc_active_per_lumi_mod[i] = TProfile_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

      hname = makeHistname(("MaxOcc_per_lumi_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Max. pixel occupancy per event, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_LB+atext_occ), false);
      sc = rdoShift.regHist(m_maxocc_per_lumi_mod[i] = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 500, 0.0001, 0.01));

      hname = makeHistname(("MaxOcc_per_BCID_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Max. pixel occupancy per BCID, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_BCID+atext_occ), false);
      sc = rdoShift.regHist(m_maxocc_per_bcid_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

      hname = makeHistname(("ToatlHits_per_BCID_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Total Number of hits per BCID, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_BCID+";#hits"), false);
      sc = rdoExpert.regHist(m_totalhits_per_bcid_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_BCID, min_BCID, max_BCID));

      hname = makeHistname(("nLargeEvent_per_lumi_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Number of large events (hitocc > 0.7#times 10^{-3}), "+m_modLabel_PixLayerIBL2D3D[i]), (atext_LB+atext_nevt), false);
      sc = rdoShift.regHist(m_nlargeevt_per_lumi_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB));

   }

   for(int i=0; i<PixLayerIBL2D3DDBM::COUNT; i++){
      hname = makeHistname(("Hit_ToT_"+m_modLabel_PixLayerIBL2D3DDBM[i]), false);
      htitles = makeHisttitle(("Hit ToT, "+m_modLabel_PixLayerIBL2D3DDBM[i]), (atext_tot+atext_nhit), false);
      if(i<PixLayerIBL2D3DDBM::kDBMA){
         sc = rdoExpert.regHist(m_hit_ToT[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot3, min_tot3, max_tot3));
      }else if(m_doIBL){
         sc = rdoExpert.regHist(m_hit_ToT[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot4, min_tot4, max_tot4));
      }
   }
   
   m_hitmap_tmp = new PixelMon2DMaps("HitMap_tmp", ("Hit map for monitoring" + m_histTitleExt).c_str()); sc = m_hitmap_tmp->regHist(rdoShift);

   for( int i=0; i<PixLayer::COUNT; i++){
     hname   = makeHistname(("nFEswithHits_"+m_modLabel_PixLayerIBL2D3D[i]), false);
     htitles = makeHisttitle(("Number of FEs with hits, "+m_modLabel_PixLayerIBL2D3D[i]), ";lumi block;eta index of module;# FEs with hits in a module in an event;# event #times # modules", false);
     sc = rdoExpert.regHist(m_nFEswithHits_mod[i] = new TH3F(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, nmod_eta[i], -0.5, -0.5 + nmod_eta[i], 18, -0.5, 17.5));
   }
     
   if (m_doOnline)
     {
       sc = rdoShift.regHist(m_occupancy_time1= new TProfile("occupancy_time_10min", ("Module hit occupancy as function of time over 10 minutes. 6 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99,0.,1.,"i"));
       sc = rdoShift.regHist(m_occupancy_time2= new TProfile("occupancy_time_1hr",   ("Module hit occupancy as function of time over 1 hour.  36 sec/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99,0.,1.,"i"));
       sc = rdoShift.regHist(m_occupancy_time3= new TProfile("occupancy_time_6hr",   ("Module hit occupancy as function of time over 6 hours.  3.6 min/bin" + m_histTitleExt + ";time;module occupancy").c_str(), 99,0.,1.,"i"));
       
       m_hitmap_mon = new PixelMon2DMaps("HitMap_Mon", ("Hit map for monitoring" + m_histTitleExt).c_str()); sc = m_hitmap_mon->regHist(rdoShift);
       
       for (int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         hname = makeHistname(("Hit_ToT_Mon_"+m_modLabel_PixLayerIBL2D3D[i]), false);
         htitles = makeHisttitle(("Hit ToT Monitoring, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_tot+atext_nhit), false);
         if ( i<PixLayer::kIBL ){
	   sc = rdoExpert.regHist(m_hit_ToT_Mon_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot3, min_tot3, max_tot3));
         } else {
	   sc = rdoExpert.regHist(m_hit_ToT_Mon_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot4, min_tot4, max_tot4));
         }

         hname = makeHistname(("Hit_ToT_tmp_"+m_modLabel_PixLayerIBL2D3D[i]), false);
         htitles = makeHisttitle(("Hit ToT tmp, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_tot+atext_nhit), false);
         if ( i<PixLayer::kIBL ){
	   sc = rdoExpert.regHist(m_hit_ToT_tmp_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot3, min_tot3, max_tot3));
         } else{
	   sc = rdoExpert.regHist(m_hit_ToT_tmp_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_tot4, min_tot4, max_tot4));
         }
       }
     }
   
   if(m_doModules && !m_doOnline){
      for( int i=0; i<PixLayer::COUNT; i++){
         tmp = "ToT_etaphi"; tmp2 = "Average Hit ToT Map"; atitles = ";Module eta;Module phi;Avg. ToT";
         float xmin = -0.5; float xmax = xmin + (1.0 * nmod_eta[i]);
         float ymin = -0.5; float ymax = ymin + (1.0 * nmod_phi[i]);
         sc= rdoExpert.regHist(m_ToT_etaphi_mod[i] = TProfile2D_LW::create((tmp + "_" + m_modLabel_PixLayerIBL2D3D[i]).c_str(), (tmp2 + ", " + m_modLabel_PixLayerIBL2D3D[i] + m_histTitleExt + atitles).c_str(), nmod_eta[i], xmin, xmax, nmod_phi[i], ymin, ymax));
      }
   }
      
   if (m_doTiming)
     {
       sc = timeExpert.regHist(m_Lvl1ID_PIX     = TH1I_LW::create("LvlID_PIX",  ("Level 1 ID (PIX)" + m_histTitleExt + ";level 1 ID" + atext_nhit).c_str(), 20,-0.5,19.5));
       sc = timeExpert.regHist(m_Lvl1ID_IBL     = TH1I_LW::create("LvlID_IBL",  ("Level 1 ID (IBL)" + m_histTitleExt + ";level 1 ID" + atext_nhit).c_str(), 40,-0.5,39.5));
       sc = timeShift.regHist(m_BCID            = TH1I_LW::create("Pixel_BCID", ("BCID" + m_histTitleExt + ";Pixel BCID" + atext_nhit).c_str(), 300,-0.5,299.5));
       sc = timeShift.regHist(m_Atlas_BCID      = TH1F_LW::create("Atlas_BCID", ("BCID" + m_histTitleExt + ";ATLAS BCID" + atext_nhit).c_str(), 3500,-0.5,3499.5));
       sc = timeShift.regHist(m_Atlas_BCID_hits = TH2F_LW::create("Atlas_BCID_Hits",  ("BCID" + m_histTitleExt + ";BCID" + atext_nhit).c_str(), 3500,-0.5,3499.5,100,0,25000));
       sc = timeExpert.regHist(m_BCID_Profile   = TProfile_LW::create("Atlas_BCID_Profile",  ("BCID_Profile" + m_histTitleExt + ";BCID" + atext_nhit).c_str(), 3500,-0.5,3499.5));  
       sc = timeShift.regHist(m_Lvl1A = TH1F_LW::create("Lvl1A", ("Hit Level 1 Accept" + m_histTitleExt + ";Level 1 Accept" + atext_nhit).c_str(), 14, -1.5, 12.5));
       
       for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         tmp = "ROD_Module_BCID_" + m_modLabel_PixLayerIBL2D3D[i];
         tmp2 = "Difference between BCID of RODs and Modules, " + m_modLabel_PixLayerIBL2D3D[i] + m_histTitleExt + ";BCID:ROD-Module (#bc) ;Number of Pixels";
         sc = timeExpert.regHist(m_diff_ROD_vs_Module_BCID_mod[i] = TH1I_LW::create(tmp.c_str(), tmp2.c_str(),101,-50.5,50.5));
	 
         tmp = "Lvl1ID_diff_ATLAS_mod_" + m_modLabel_PixLayerIBL2D3D[i];
         tmp2 = "ATLAS_{Level 1 ID} - Module_{Level 1 ID}, " + m_modLabel_PixLayerIBL2D3D[i] + m_histTitleExt + ";#Delta Level 1 ID; # hits";
         sc = timeExpert.regHist(m_Lvl1ID_diff_mod_ATLAS_mod[i] = TH1I_LW::create(tmp.c_str(), tmp2.c_str(), 201, -200.5, 200.5));
       }

       for(int i=0; i<PixLayerDBM::COUNT-1+(int)(m_doIBL); i++){
         hname = makeHistname(("Lvl1A_"+m_modLabel_PixLayerIBL2D3DDBM[i]), false);
         htitles = makeHisttitle(("Hit Level 1 Accept, "+m_modLabel_PixLayerIBL2D3DDBM[i]), (atext_lv1+atext_nhit), false);
         sc = timeShift.regHist(m_Lvl1A_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 14, -1.5, 12.5));
       }
     }

   if (m_do2DMaps)
     {
       m_occupancy = new PixelMon2DMaps("Occupancy", ("hit map"+ m_histTitleExt).c_str());
       sc = m_occupancy->regHist(rdoShift);
       
       m_average_pixocc = new PixelMon2DMapsLW("Occupancy_per_pixel", ("#hits / pixel" + m_histTitleExt).c_str(), m_doIBL, false);
       sc = m_average_pixocc->regHist(rdoShift, m_doIBL, false);
       
       m_occupancy_pix_evt = new PixelMon2DProfilesLW("Occupancy_per_pixel_event", ("#hits / pixel / event" + m_histTitleExt).c_str(), m_doIBL, false, false);
       sc = m_occupancy_pix_evt->regHist(rdoShift);
             
       m_Lvl1ID_diff_mod_ATLAS_per_LB = new PixelMon2DLumiProfiles("Lvl1ID_diff_ATLAS_mod_per_LB", ("ATLAS_{Level 1 ID} - Module_{Level 1 ID} per LB" + m_histTitleExt).c_str(),"#Delta Level 1 ID",m_doIBL,false);
       sc = m_Lvl1ID_diff_mod_ATLAS_per_LB->regHist(timeExpert,m_doIBL,false);
       
       m_Lvl1ID_absdiff_mod_ATLAS_per_LB = new PixelMon2DLumiProfiles("Lvl1ID_absdiff_ATLAS_mod_per_LB", ("ATLAS_{Level 1 ID} - Module_{Level 1 ID} per LB" + m_histTitleExt).c_str(),"#Delta Level 1 ID",m_doIBL,false);
       sc = m_Lvl1ID_absdiff_mod_ATLAS_per_LB->regHist(timeExpert,m_doIBL,false);
     }
   
   if (m_doModules)
     {
       m_hit_num_mod = new PixelMonModules1D("Hit_num", ("Number of hits in a module in an event" + m_histTitleExt).c_str(), 15,-0.5,149.5,m_doIBL);
       sc = m_hit_num_mod->regHist(this,(path+"/Modules_NumberOfHits").c_str(),run, m_doIBL);
       m_hiteff_mod = new PixelMonModulesProf("Hit_track_eff", ("Proportion of hits on track" + m_histTitleExt).c_str(), 2500,-0.5,2499.5,m_doIBL);
       sc = m_hiteff_mod->regHist(this,(path+"/Modules_HitEff").c_str(),run, m_doIBL);
     }
   if (m_doFEChipSummary)
     {
       m_FE_chip_hit_summary = new PixelMonModules1D("FE_Chip_Summary", ("FE Chip Summary" + m_histTitleExt).c_str(), 16,-0.5,15.5,m_doIBL);
       sc = m_FE_chip_hit_summary->regHist(this,(path+"/Modules_FEChipSummary").c_str(),run, m_doIBL);
     }
   if (m_doLowOccupancy || m_doHighOccupancy)
     {
       int nbins_hits = 2000;  float max_hits = 80000.0;
       int nbins_avhits = 100; float max_avhits = 100.0;
       
       if(m_doLowOccupancy){
         nbins_hits = 200;  max_hits = 200.0;
         nbins_avhits = 50; max_avhits = 2.0;
       }
       
       if(m_doHeavyIonMon){ max_hits = 350000; max_avhits = 2500; }
       
       sc = rdoShift.regHist(m_num_hits= TH1I_LW::create("num_hits",  ("Number of pixel hits in an event" + m_histTitleExt + ";# pixel hits/event;# events").c_str(), nbins_hits, 0., max_hits));
       
       for(int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++){
         hname = makeHistname(("Occupancy_Summary_"+m_modLabel_PixLayerIBL2D3D[i]), false);
         htitles = makeHisttitle(("Average hit occupancy, "+m_modLabel_PixLayerIBL2D3D[i]), ";average # hits in a module in an event;# modules", false);
         sc = rdoExpert.regHist(m_occupancy_summary_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), nbins_avhits, 0., max_avhits));
       }
     }
   if (m_doPixelOccupancy)
     {
       m_pixel_occupancy = new PixelMonModules2D("Pixel_Occupancy", ("Pixel Occupancy" + m_histTitleExt).c_str(), 160, -0.,160.,336,0.,336.,m_doIBL);
       sc = m_pixel_occupancy->regHist(this,(path+"/PixelOccupancy").c_str(),run, m_doIBL);
     }
   if (m_doRodSim)
     {
       sc = rdoExpert.regHist(m_RodSim_BCID_minus_ToT= TH1F_LW::create("RodSim_BCID_minus_ToT",  ("BCID - ToT" + m_histTitleExt).c_str(), 300,-0.5,299.5));
       sc = rdoExpert.regHist(m_RodSim_FrontEnd_minus_Lvl1ID= TH1F_LW::create("RodSim_FrontEnd_minus_Lvl1ID",  ("Front End Chip - Lvl1ID" + m_histTitleExt).c_str(), 33,-16.5,16.5));
     }

   if (m_doDetails)
     /// 4 special modules
     {
       sc = rdoExpert.regHist(m_Details_mod1_num_hits  = TH1F_LW::create(("Details_num_hits_"+m_DetailsMod1).c_str(),  ("number of pixel hits per event for mod1" + m_histTitleExt).c_str(), 100,-0.,100)); 
       sc = rdoExpert.regHist(m_Details_mod2_num_hits  = TH1F_LW::create(("Details_num_hits_"+m_DetailsMod2).c_str(),  ("number of pixel hits per event for mod2" + m_histTitleExt).c_str(), 100,-0.,100));  
       sc = rdoExpert.regHist(m_Details_mod3_num_hits  = TH1F_LW::create(("Details_num_hits_"+m_DetailsMod3).c_str(),  ("number of pixel hits per event for mod3" + m_histTitleExt).c_str(), 100,-0.,100));  
       sc = rdoExpert.regHist(m_Details_mod4_num_hits  = TH1F_LW::create(("Details_num_hits_"+m_DetailsMod4).c_str(),  ("number of pixel hits per event for mod4" + m_histTitleExt).c_str(), 100,-0.,100));
       //New ranges for 2 FEI4Bs:
       sc = rdoExpert.regHist(m_Details_mod1_occupancy = TH2F_LW::create(("Details_occupancy_"+m_DetailsMod1).c_str(),   ("column vs row hitmap for mod1" + m_histTitleExt).c_str(), 160,-0.5,155.5,336,-0.5,335.5));  
       sc = rdoExpert.regHist(m_Details_mod2_occupancy = TH2F_LW::create(("Details_occupancy_"+m_DetailsMod2).c_str(),   ("column vs row hitmap for mod2" + m_histTitleExt).c_str(), 160,-0.5,155.5,336,-0.5,335.5));   
       sc = rdoExpert.regHist(m_Details_mod3_occupancy = TH2F_LW::create(("Details_occupancy_"+m_DetailsMod3).c_str(),   ("column vs row hitmap for mod3" + m_histTitleExt).c_str(), 160,-0.5,155.5,336,-0.5,335.5));   
       sc = rdoExpert.regHist(m_Details_mod4_occupancy = TH2F_LW::create(("Details_occupancy_"+m_DetailsMod4).c_str(),   ("column vs row hitmap for mod4" + m_histTitleExt).c_str(), 160,-0.5,155.5,336,-0.5,335.5));   
       sc = rdoExpert.regHist(m_Details_mod1_ToT       = TH1F_LW::create(("Details_ToT_"+m_DetailsMod1).c_str(),  ("ToT mod1" + m_histTitleExt).c_str(), 300,-0.5,299.5));  
       sc = rdoExpert.regHist(m_Details_mod2_ToT       = TH1F_LW::create(("Details_ToT_"+m_DetailsMod2).c_str(),  ("ToT mod2" + m_histTitleExt).c_str(), 300,-0.5,299.5));  
       sc = rdoExpert.regHist(m_Details_mod3_ToT       = TH1F_LW::create(("Details_ToT_"+m_DetailsMod3).c_str(),  ("ToT mod3" + m_histTitleExt).c_str(), 300,-0.5,299.5));  
       sc = rdoExpert.regHist(m_Details_mod4_ToT       = TH1F_LW::create(("Details_ToT_"+m_DetailsMod4).c_str(),  ("ToT mod4" + m_histTitleExt).c_str(), 300,-0.5,299.5));  
     }

   if (m_doOfflineAnalysis) {
     /// Quick Status
     ///
     hname = makeHistname("Hits_per_lumi_L0_B11_S2_C6", false);
     htitles = makeHisttitle("Number of hits, L0_B11_S2_C6", ";lumi block;FE ID (16*(6-eta_mod) + 8*(pix_phi/164) + (eta_pix/18);#hits", false);
     sc = rdoShift.regHist(m_nhits_L0_B11_S2_C6 = TH2F_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 96, -0.5, -0.5+96));
     m_nhits_L0_B11_S2_C6->SetOption("colz");

     hname = makeHistname("Occupancy_per_lumi_L0_B11_S2_C6", false);
     htitles = makeHisttitle("Average pixel occupancy per event, L0_B11_S2_C6", ";lumi block;FE ID (16*(6-eta_mod) + 8*(pix_phi/164) + (eta_pix/18);#hits/pixel/event", false);
     sc = rdoShift.regHist(m_occupancy_L0_B11_S2_C6 = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), nbins_LB, min_LB, max_LB, 96, -0.5, -0.5+96));
     m_occupancy_L0_B11_S2_C6->SetOption("colz");

     for(int i=0; i<PixLayerIBL2D3D::COUNT; i++){
        hname = makeHistname(("AvgOcc_per_BCID_per_lumi_"+m_modLabel_PixLayerIBL2D3D[i]), false);
        htitles = makeHisttitle(("Average pixel occupancy per BCID per lumi, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_LB+atext_BCID+atext_occ), false);
        sc = rdoExpert.regHist(m_avgocc_per_bcid_per_lumi_mod[i] = TProfile2D_LW::create(hname.c_str(), htitles.c_str(), 2000, -0.5, -0.5+2000, nbins_BCID, min_BCID, max_BCID));

        hname = makeHistname(("Hit_ToT_per_lumi_"+m_modLabel_PixLayerIBL2D3D[i]), false);
        htitles = makeHisttitle(("Hit ToT per lumi, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_LB+atext_tot+atext_occ), false);
        sc = rdoExpert.regHist(m_hit_ToT_per_lumi_mod[i] = TH2F_LW::create(hname.c_str(), htitles.c_str(), 2000, -0.5, -0.5+2000, nbins_tot3, min_tot3, max_tot3));

     }
   }

   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endmsg;         
   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::BookHitsLumiBlockMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Hits for lowStat" << endmsg;  
   
   std::string path = "Pixel/LumiBlock";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnPixelTrack");
   MonGroup lumiBlockHist(   this, path.c_str(), lowStat, ATTRIB_MANAGED); //declare a group of histograms

   std::string hname;
   std::string htitles;
   std::string atext_LB = ";lumi block"; 
   std::string atext_nevt = ";# events"; 
   std::string atext_nhit = ";# hits"; 
   std::string atext_hit = ";# hits/event"; 
   std::string atext_occ = ";# hits/pixel/event"; 
   std::string atext_tot = ";ToT [BC]"; 
   std::string atext_lv1 = ";Level 1 Accept"; 

   StatusCode sc;

   if(m_doLowOccupancy || m_doHighOccupancy) {
      int nbins_hits = 100;  double min_hits = -0.5; double max_hits = min_hits + 25000.0;
      if(m_doLowOccupancy){
         nbins_hits = 200;  max_hits = min_hits + 200.0;
      }
      hname = makeHistname("num_hits_LB", false);
      htitles = makeHisttitle("Number of pixel hits in an event", (atext_hit+atext_nevt), false);
      sc = lumiBlockHist.regHist(m_num_hits_LB = TH1I_LW::create(hname.c_str(), htitles.c_str(), nbins_hits, min_hits, max_hits));
   }
   if(m_doModules) {
      hname = makeHistname("num_Hits_mod_LB", false);
      htitles = makeHisttitle("Number of pixel hits in a module in an event", (atext_hit+atext_nevt), false);
      m_hit_num_mod_LB = new PixelMonModules1D(hname.c_str(), htitles.c_str(), 20, -0.5, 19.5, m_doIBL);
      sc = m_hit_num_mod_LB->regHist(this, (path+"/Modules_NumberOfHits").c_str(), lowStat, m_doIBL);
   }
   for( int i=0; i<PixLayer::COUNT; i++){
      hname = makeHistname(("Hit_ToT_LB_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Hit ToT, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_tot+atext_nhit), false);
      if(i != PixLayer::kIBL){
         sc = lumiBlockHist.regHist(m_hit_ToT_LB_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 300, -0.5, 299.5));   
      }else{
         sc = lumiBlockHist.regHist(m_hit_ToT_LB_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 20, -0.5, 19.5));   
      }

      hname = makeHistname(("Lvl1A_10min_"+m_modLabel_PixLayerIBL2D3D[i]), false);
      htitles = makeHisttitle(("Hit Level 1 Accept, "+m_modLabel_PixLayerIBL2D3D[i]), (atext_lv1+atext_nhit), false);
      sc = lumiBlockHist.regHist(m_Lvl1A_10min_mod[i] = TH1F_LW::create(hname.c_str(), htitles.c_str(), 14, -1.5, 12.5));
   }

   m_occupancy_10min = new PixelMon2DMaps("Occupancy_10min", ("hit occupancy" + m_histTitleExt).c_str());
   sc = m_occupancy_10min->regHist(lumiBlockHist);
   
   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endmsg;         
   return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::FillHitsMon(void) //Called once per event
{

  int DetailsMod1 = 0;
  int DetailsMod2 = 0;
  int DetailsMod3 = 0;
  int DetailsMod4 = 0;
  int nhitsM1=0;
  int nhitsM2=0;
  int nhitsM3=0;
  int nhitsM4=0;
  if(m_doDetails)
    {
      DetailsMod1 = ParseDetailsString(m_DetailsMod1); 
      DetailsMod2 = ParseDetailsString(m_DetailsMod2); 
      DetailsMod3 = ParseDetailsString(m_DetailsMod3); 
      DetailsMod4 = ParseDetailsString(m_DetailsMod4); 
    }

  static constexpr int nmod_phi[PixLayer::COUNT] = {48, 48, 22, 38, 52, 14};
  static constexpr int nmod_eta[PixLayer::COUNT] = {3, 3, 13, 13, 13, 20};
  
  int HitPerEventArray_disksA[ nmod_phi[PixLayer::kECA] ][ nmod_eta[PixLayer::kECA] ];
  int HitPerEventArray_disksC[ nmod_phi[PixLayer::kECC] ][ nmod_eta[PixLayer::kECC] ];
  int HitPerEventArray_l0[ nmod_phi[PixLayer::kB0] ][ nmod_eta[PixLayer::kB0] ];
  int HitPerEventArray_l1[ nmod_phi[PixLayer::kB1] ][ nmod_eta[PixLayer::kB1] ];
  int HitPerEventArray_l2[ nmod_phi[PixLayer::kB2] ][ nmod_eta[PixLayer::kB2] ];
  int HitPerEventArray_lI[ nmod_phi[PixLayer::kIBL] ][ nmod_eta[PixLayer::kIBL] ];
  
  for(int i=0; i<PixLayer::COUNT; i++){
    for( int phi=0; phi<nmod_phi[i]; phi++){
      for(int eta=0; eta<nmod_eta[i]; eta++){
	if(i==PixLayer::kECA) HitPerEventArray_disksA[phi][eta]=0;
	if(i==PixLayer::kECC) HitPerEventArray_disksC[phi][eta]=0;
	if(i==PixLayer::kB0)  HitPerEventArray_l0[phi][eta]=0;
	if(i==PixLayer::kB1)  HitPerEventArray_l1[phi][eta]=0;
	if(i==PixLayer::kB2)  HitPerEventArray_l2[phi][eta]=0;
	if(i==PixLayer::kIBL) HitPerEventArray_lI[phi][eta]=0;
      }
    }
  }
  
  double nhits=0;
  double nhits_mod[PixLayerIBL2D3D::COUNT]={0};
  int nhits_L0_B11_S2_C6[96] = {0};
  
  int fewithHits_EA[ nmod_phi[PixLayer::kECA]][ nmod_eta[PixLayer::kECA]][16];
  int fewithHits_EC[ nmod_phi[PixLayer::kECC]][ nmod_eta[PixLayer::kECC]][16];
  int fewithHits_B0[ nmod_phi[PixLayer::kB0] ][ nmod_eta[PixLayer::kB0] ][16];
  int fewithHits_B1[ nmod_phi[PixLayer::kB1] ][ nmod_eta[PixLayer::kB1] ][16];
  int fewithHits_B2[ nmod_phi[PixLayer::kB2] ][ nmod_eta[PixLayer::kB2] ][16];
  for(int i=0; i<PixLayer::COUNT; i++){
    for( int phi=0; phi<nmod_phi[i]; phi++){
      for(int eta=0; eta<nmod_eta[i]; eta++){
	for(int j=0 ; j<16 ; j++){
	  if(i==PixLayer::kECA) fewithHits_EA[phi][eta][j]=0;
	  if(i==PixLayer::kECC) fewithHits_EC[phi][eta][j]=0;
	  if(i==PixLayer::kB0)  fewithHits_B0[phi][eta][j]=0;
	  if(i==PixLayer::kB1)  fewithHits_B1[phi][eta][j]=0;
	  if(i==PixLayer::kB2)  fewithHits_B2[phi][eta][j]=0;
	}
      }
    }
  }
  
  Identifier rdoID;
  
  int nChannels_mod[PixLayerIBL2D3D::COUNT] = {46080, 46080, 46080, 46080, 46080, 26880, 53760, 26880};
  double nActiveChannels_total = 0.;
  double nGoodChannels_layer[PixLayerIBL2D3D::COUNT];
  double nActiveChannels_layer[PixLayerIBL2D3D::COUNT];
  for( int i=0; i<PixLayerIBL2D3D::COUNT; i++){
    nGoodChannels_layer[i]   = 1.0 * nChannels_mod[i] * m_nGood_mod[i];
    nActiveChannels_layer[i] = 1.0 * nChannels_mod[i] * m_nActive_mod[i];
    nActiveChannels_total    =+ nGoodChannels_layer[i];
  }

  StatusCode sc;

  // get Pixel ROD BCID (II - tbc ROD BCID seems to be of the last ROD, why not to take the one from ATLAS in main?)
  // this BCID is used for ATLAS fill, but also for diff ROD-module in m_diff_ROD_vs_Module_BCID [# of Hits!]
  // change to hit BCID versus ATLAS BCID, useful?? (code in use since Run1)
  //
  const InDetTimeCollection *Pixel_BCIDColl = 0;  
  if ( evtStore()->contains<InDetTimeCollection>("PixelBCID") )  sc = evtStore()->retrieve(Pixel_BCIDColl, "PixelBCID"); 
  int pix_rod_bcid      = 0;
  if ( !sc.isFailure() && Pixel_BCIDColl!=0 ) 
    {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Found Pixel BCID collection"<<endmsg;
      for ( InDetTimeCollection::const_iterator ipix_bcid = Pixel_BCIDColl->begin(); ipix_bcid != Pixel_BCIDColl->end(); ++ipix_bcid ) 
	{
	  if (!(*ipix_bcid)) continue;
	  const unsigned int pix_bcid = (*ipix_bcid)->second;
	  pix_rod_bcid = pix_bcid;
	} 
    }
  if (sc.isFailure()) if(msgLvl(MSG::INFO)) {msg(MSG::INFO)  << "Could not find the data object PixelBCID" << " !" << endmsg;}

  // get ATLAS LVL1ID
  //
  int lvl1idATLAS(-1);
  const EventInfo* thisEventInfo;
  sc=evtStore()->retrieve(thisEventInfo);
  if (sc != StatusCode::SUCCESS)
    {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "No EventInfo object found" << endmsg;
    } else 
    {
      lvl1idATLAS = (int)((thisEventInfo->trigger_info()->extendedLevel1ID())&0xf);
    }
  
  // retrieve Pixel RDO container from storegate
  sc=evtStore()->retrieve(m_rdocontainer,m_Pixel_RDOName);
  if (sc.isFailure() || !m_rdocontainer) 
    {
      if (msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Could not find the data object " << m_Pixel_RDOName << " !" << endmsg;
      if (m_storegate_errors) m_storegate_errors->Fill(1.,3.);  //first entry (1). is for RDO, second (2) is for retrieve problem
      return StatusCode::SUCCESS;  //fail gracefully and keep going in the next tool
    } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Data object " << m_Pixel_RDOName << " found" << endmsg;
  }

  PixelRDO_Container::const_iterator colNext   = m_rdocontainer->begin();
  PixelRDO_Container::const_iterator lastCol   = m_rdocontainer->end();
  DataVector<PixelRDORawData>::const_iterator p_rdo;
  
  for (; colNext != lastCol; ++colNext) // Pixel RDO (hits) Loop
    {
      const InDetRawDataCollection<PixelRDORawData>* PixelCollection(*colNext);
      if (!PixelCollection) 
	{
	  if(m_storegate_errors) m_storegate_errors->Fill(1.,5.);  //first entry (1). is for RDO, second (4) is for data problem
	  continue;
       }
      
      for (p_rdo=PixelCollection->begin(); p_rdo!=PixelCollection->end(); ++p_rdo) {
	
	rdoID=(*p_rdo)->identify();
	if (m_doOnTrack || m_doOnPixelTrack) if(!OnTrack(rdoID,false) ) continue; //if we only want hits on track, and the hit is NOT on the track, skip filling

	int pixlayer = GetPixLayerID(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_doIBL);
	int pixlayerdbm = GetPixLayerIDDBM(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_doIBL);
	int pixlayeribl2d3d = pixlayer;
	if ( pixlayeribl2d3d == PixLayer::kIBL ){
	  pixlayeribl2d3d = GetPixLayerIDIBL2D3D(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_pixelid->eta_module(rdoID), m_doIBL);
	}
	int pixlayeribl2d3ddbm = pixlayerdbm;
	if ( pixlayeribl2d3ddbm == PixLayerDBM::kIBL ){
	  pixlayeribl2d3ddbm = GetPixLayerIDIBL2D3DDBM(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_pixelid->eta_module(rdoID), m_doIBL);
	}
	bool isIBL = false;
	if (m_pixelid->barrel_ec(rdoID)==0 && m_doIBL && m_Lvl1A_mod[PixLayerDBM::kIBL] && m_pixelid->layer_disk(rdoID)==0) isIBL = true;
	
	/////////////Start main fill block here///////////////
	//be sure to check each histo exists before filling it
	
	/// Fill Occupancy
	if ( m_occupancy) m_occupancy->Fill(rdoID, m_pixelid, m_doIBL);
	if ( m_occupancy_10min && m_doLumiBlock) m_occupancy_10min->Fill(rdoID, m_pixelid, m_doIBL);
	if ( m_hitmap_tmp ) m_hitmap_tmp->Fill(rdoID, m_pixelid, m_doIBL);
	if ( m_average_pixocc && nChannels_mod[pixlayeribl2d3d] > 0 ) m_average_pixocc->WeightingFill(rdoID, m_pixelid, m_doIBL, 1.0/( 1.0*nChannels_mod[pixlayeribl2d3d]) );

       
	/// Fill Lvl1A
	if (m_Lvl1A) {
	  m_Lvl1A->Fill((*p_rdo)->getLVL1A());
	  if(pixlayerdbm != 99 && m_Lvl1A_mod[pixlayerdbm]) m_Lvl1A_mod[pixlayerdbm]->Fill( (*p_rdo)->getLVL1A());
	}
	if (pixlayer != 99 && m_Lvl1A_10min_mod[pixlayer] && m_doLumiBlock ) m_Lvl1A_10min_mod[pixlayer]->Fill( (*p_rdo)->getLVL1A() );
	if(isIBL && m_Lvl1ID_IBL) m_Lvl1ID_IBL->Fill((*p_rdo)->getLVL1ID()%32);
	if(!isIBL && m_Lvl1ID_PIX) m_Lvl1ID_PIX->Fill((*p_rdo)->getLVL1ID());
	
       
	/// Fill difference of Lvl1
	int lvl1idMOD = (int)(*p_rdo)->getLVL1ID();
	int difflvl1id = lvl1idATLAS - lvl1idMOD%32;	  
	if(pixlayer != 99 && m_Lvl1ID_diff_mod_ATLAS_mod[pixlayer]) m_Lvl1ID_diff_mod_ATLAS_mod[pixlayer]->Fill(difflvl1id);
	
	if(m_Lvl1ID_diff_mod_ATLAS_per_LB) m_Lvl1ID_diff_mod_ATLAS_per_LB->Fill(m_manager->lumiBlockNumber(),rdoID,m_pixelid,difflvl1id,m_doIBL,false);
	if(m_Lvl1ID_absdiff_mod_ATLAS_per_LB) m_Lvl1ID_absdiff_mod_ATLAS_per_LB->Fill(m_manager->lumiBlockNumber(),rdoID,m_pixelid,fabs(difflvl1id),m_doIBL,false);
	
	
	/// Fill BCID
	if (m_BCID) m_BCID->Fill((*p_rdo)->getBCID());
	if (m_Atlas_BCID) m_Atlas_BCID->Fill(pix_rod_bcid); //defined at the start of the method
	if (m_BCID_Profile) m_BCID_Profile->Fill(double(pix_rod_bcid),double(nhits));          
	if (pixlayer != 99 && m_diff_ROD_vs_Module_BCID_mod[pixlayer]) m_diff_ROD_vs_Module_BCID_mod[pixlayer]->Fill( (pix_rod_bcid&0x000000ff)-(*p_rdo)->getBCID() ); 
	if (m_doRodSim) {
	  if (m_RodSim_FrontEnd_minus_Lvl1ID) m_RodSim_FrontEnd_minus_Lvl1ID->Fill(m_pixelCableSvc->getFE(&rdoID,rdoID) -(*p_rdo)->getLVL1ID());
	  if (m_RodSim_BCID_minus_ToT) m_RodSim_BCID_minus_ToT->Fill((*p_rdo)->getBCID() - (*p_rdo)->getToT() );
	}
       
	if (m_FE_chip_hit_summary) m_FE_chip_hit_summary->Fill(m_pixelCableSvc->getFE(&rdoID,rdoID),rdoID,m_pixelid,m_doIBL);
	
	if (m_hiteff_mod) {
	  if (OnTrack(rdoID,false)) {
	    m_hiteff_mod->Fill(m_manager->lumiBlockNumber(),1.,rdoID,m_pixelid,m_doIBL);
	  } else {
	    m_hiteff_mod->Fill(m_manager->lumiBlockNumber(),0.,rdoID,m_pixelid,m_doIBL);
	  }
	}
	    

	/// Fill ToT
	if (pixlayerdbm != 99 && m_hit_ToT[pixlayerdbm]) m_hit_ToT[pixlayerdbm]->Fill((*p_rdo)->getToT());
	if (pixlayer != 99 && m_hit_ToTMean_mod[pixlayer]) m_hit_ToTMean_mod[pixlayer]->Fill(m_manager->lumiBlockNumber(), (*p_rdo)->getToT());
	if (pixlayerdbm == PixLayerDBM::kIBL){
	  int ibl2d3d = GetPixLayerIDIBL2D3DDBM(m_pixelid->barrel_ec(rdoID), m_pixelid->layer_disk(rdoID), m_pixelid->eta_module(rdoID), m_doIBL);
	  if(m_hit_ToT[ibl2d3d]) m_hit_ToT[ibl2d3d]->Fill((*p_rdo)->getToT());
	}
	if (m_doLumiBlock && pixlayer != 99){
	  if(m_hit_ToT_LB_mod[pixlayer]) m_hit_ToT_LB_mod[pixlayer]->Fill((*p_rdo)->getToT());
	}
	if (pixlayer != 99 && m_ToT_etaphi_mod[pixlayer] ) m_ToT_etaphi_mod[pixlayer]->Fill(m_pixelid->eta_module(rdoID), m_pixelid->phi_module(rdoID), (*p_rdo)->getToT());
		
	if (m_doOnline)
	  {
	    if(pixlayer != 99 && m_hit_ToT_tmp_mod[pixlayer]) m_hit_ToT_tmp_mod[pixlayer]->Fill((*p_rdo)->getToT());
	  }
	
	/// Not yet modified for IBL geometry:
	if (m_doDetails) {
	  int currentID = 1000000 + (-m_pixelid->barrel_ec(rdoID) + 2)*100000 + (m_pixelid->layer_disk(rdoID))*10000 + (m_pixelid->phi_module(rdoID))*100 + (m_pixelid->eta_module(rdoID) + 6); 
	  if(m_Details_mod1_occupancy && currentID==DetailsMod1) m_Details_mod1_occupancy->Fill(m_pixelid->eta_index(rdoID),m_pixelid->phi_index(rdoID));
	  if(m_Details_mod2_occupancy && currentID==DetailsMod2) m_Details_mod2_occupancy->Fill(m_pixelid->eta_index(rdoID),m_pixelid->phi_index(rdoID));
	  if(m_Details_mod3_occupancy && currentID==DetailsMod3) m_Details_mod3_occupancy->Fill(m_pixelid->eta_index(rdoID),m_pixelid->phi_index(rdoID));
	  if(m_Details_mod4_occupancy && currentID==DetailsMod4) m_Details_mod4_occupancy->Fill(m_pixelid->eta_index(rdoID),m_pixelid->phi_index(rdoID));
	  if(m_Details_mod1_ToT && currentID==DetailsMod1) m_Details_mod1_ToT->Fill((*p_rdo)->getToT());     
	  if(m_Details_mod2_ToT && currentID==DetailsMod2) m_Details_mod2_ToT->Fill((*p_rdo)->getToT());     
	  if(m_Details_mod3_ToT && currentID==DetailsMod3) m_Details_mod3_ToT->Fill((*p_rdo)->getToT());     
	  if(m_Details_mod4_ToT && currentID==DetailsMod4) m_Details_mod4_ToT->Fill((*p_rdo)->getToT());  
	}
	if (m_pixel_occupancy) m_pixel_occupancy->Fill(m_pixelid->eta_index(rdoID),m_pixelid->phi_index(rdoID),rdoID, m_pixelid,m_doIBL);
	
	if (pixlayer != 99) nhits_mod[pixlayer]++;
	nhits++;
	nhitsM1++;
	nhitsM2++;
	nhitsM3++;
	nhitsM4++;
	
	///if (m_doModuleas)//fill module hit arrays so we can calculate the number of hits/event/module 
	///{
	if(m_pixelid->barrel_ec(rdoID)==2 ) HitPerEventArray_disksA[m_pixelid->phi_module(rdoID)][m_pixelid->layer_disk(rdoID)]++;
	if(m_pixelid->barrel_ec(rdoID)==-2) HitPerEventArray_disksC[m_pixelid->phi_module(rdoID)][m_pixelid->layer_disk(rdoID)]++;
	if(m_pixelid->barrel_ec(rdoID)==0 )
	  {
	    if(m_doIBL && m_pixelid->layer_disk(rdoID)==0) HitPerEventArray_lI[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID)+10]++;
	    if(m_pixelid->layer_disk(rdoID)==0+m_doIBL) HitPerEventArray_l0[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID)+6]++;
	    if(m_pixelid->layer_disk(rdoID)==1+m_doIBL) HitPerEventArray_l1[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID)+6]++;
	    if(m_pixelid->layer_disk(rdoID)==2+m_doIBL) HitPerEventArray_l2[m_pixelid->phi_module(rdoID)][m_pixelid->eta_module(rdoID)+6]++;
	  } 
	///}
	
	/// Quick Status
	int fephi=0;
	int feeta=0;
	if (m_doOfflineAnalysis) {
	  if ( pixlayer == PixLayer::kB0 && GetFEID( pixlayer, m_pixelid->phi_index(rdoID), m_pixelid->eta_index(rdoID), fephi, feeta) ){
	    if (m_pixelid->phi_module(rdoID) == 0 && m_pixelid->eta_module(rdoID) < 0 && m_nhits_L0_B11_S2_C6) {
	      m_nhits_L0_B11_S2_C6->Fill( m_manager->lumiBlockNumber(), (16*fabs(6+m_pixelid->eta_module(rdoID)))+(8.0*fephi)+feeta );
	      nhits_L0_B11_S2_C6[ (int)(16*fabs(6+m_pixelid->eta_module(rdoID)))+(8*fephi)+feeta ]++;
	    }
	  }
	  /// for Pixel Operation TF
	  if (pixlayer != 99 && m_hit_ToT_per_lumi_mod[pixlayer] && nGoodChannels_layer[pixlayer]>0) m_hit_ToT_per_lumi_mod[pixlayer]->Fill(m_manager->lumiBlockNumber(), (*p_rdo)->getToT(), 1.0/nGoodChannels_layer[pixlayer]);
	  if (pixlayer == PixLayer::kIBL && m_hit_ToT_per_lumi_mod[pixlayeribl2d3d] && nGoodChannels_layer[pixlayeribl2d3d]>0) m_hit_ToT_per_lumi_mod[pixlayeribl2d3d]->Fill(m_manager->lumiBlockNumber(), (*p_rdo)->getToT(), 1.0/nGoodChannels_layer[pixlayeribl2d3d]);
	}
	if ( pixlayer == PixLayer::kB0 && GetFEID( pixlayer, m_pixelid->phi_index(rdoID), m_pixelid->eta_index(rdoID), fephi, feeta) ){
	  fewithHits_B0[m_pixelid->phi_module(rdoID)][(int)(fabs(6+m_pixelid->eta_module(rdoID)))][(int)((8*fephi)+feeta)] = 1;
	}
	if ( pixlayer == PixLayer::kB1 && GetFEID( pixlayer, m_pixelid->phi_index(rdoID), m_pixelid->eta_index(rdoID), fephi, feeta) ){
	  fewithHits_B1[m_pixelid->phi_module(rdoID)][(int)(fabs(6+m_pixelid->eta_module(rdoID)))][(int)((8*fephi)+feeta)] = 1;
	}
	if ( pixlayer == PixLayer::kB2 && GetFEID( pixlayer, m_pixelid->phi_index(rdoID), m_pixelid->eta_index(rdoID), fephi, feeta) ){
	  fewithHits_B2[m_pixelid->phi_module(rdoID)][(int)(fabs(6+m_pixelid->eta_module(rdoID)))][(int)((8*fephi)+feeta)] = 1;
	}
	if ( pixlayer == PixLayer::kECA && GetFEID( pixlayer, m_pixelid->phi_index(rdoID), m_pixelid->eta_index(rdoID), fephi, feeta) ){
	  fewithHits_EA[m_pixelid->phi_module(rdoID)][(int)m_pixelid->layer_disk(rdoID)][(int)((8*fephi)+feeta)] = 1;
	}
	if ( pixlayer == PixLayer::kECC && GetFEID( pixlayer, m_pixelid->phi_index(rdoID), m_pixelid->eta_index(rdoID), fephi, feeta) ){
	  fewithHits_EC[m_pixelid->phi_module(rdoID)][(int)m_pixelid->layer_disk(rdoID)][(int)((8*fephi)+feeta)] = 1;
	}
	
	///////////End of main fill block////////////////////
      }
    } //end of RDO (hit) loop
   
   

  ////////////////////Fill after event block////////////////
  if (m_doOnline)//should we fill these mid run or only at the end?
    {
      FillSummaryHistos(m_occupancy, m_occupancy_summary_mod[PixLayer::kECA], m_occupancy_summary_mod[PixLayer::kECC], 
			m_occupancy_summary_mod[PixLayer::kIBL], m_occupancy_summary_mod[PixLayer::kB0], 
			m_occupancy_summary_mod[PixLayer::kB1],  m_occupancy_summary_mod[PixLayer::kB2]);
      if(m_occupancy_time1&&m_occupancy_time2&&m_occupancy_time3) FillTimeHisto(double(nhits/(1744.0+280*m_doIBL)),m_occupancy_time1, m_occupancy_time2, m_occupancy_time3,10.,60.,360. );
      if (m_doRefresh) {
	for(int i=0; i<PixLayer::COUNT; i++)
	  if( m_hit_ToT_Mon_mod[i] && m_hit_ToT_tmp_mod[i]) TH1FFillMonitoring(m_hit_ToT_Mon_mod[i], m_hit_ToT_tmp_mod[i]);
	if ( m_hitmap_tmp && m_hitmap_mon) {
	  if ( m_occupancy_pix_evt ) m_occupancy_pix_evt->FillFromMap(m_hitmap_tmp, false);
	  m_hitmap_mon->Fill2DMon(m_hitmap_tmp);
	}
      }
    } //end of doOnline loop processing                                  
  
  /// Fill number of hits per lLB
  if (m_hits_per_lumi) m_hits_per_lumi->Fill(m_manager->lumiBlockNumber(), nhits);     
  for( int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++) 
    if (m_hits_per_lumi_mod[i]) m_hits_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), nhits_mod[i]);
  
  /// Fill average occupancy
  if ( !m_doOnline && m_occupancy_pix_evt && m_hitmap_tmp) m_occupancy_pix_evt->FillFromMap(m_hitmap_tmp, true);
  
  double avgocc = 0;
  double avgocc_mod[PixLayerIBL2D3D::COUNT] = {0};
  double avgocc_active_mod[PixLayerIBL2D3D::COUNT] = {0};
  if(nActiveChannels_total>0) avgocc = nhits/nActiveChannels_total;
  if(m_avgocc_per_lumi) m_avgocc_per_lumi->Fill(m_manager->lumiBlockNumber(), avgocc);

  if (m_doOfflineAnalysis) {
    for( int i=0; i<PixLayerIBL2D3D::COUNT; i++)
      if(m_avgocc_per_bcid_per_lumi_mod[i]) m_avgocc_per_bcid_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), pix_rod_bcid, avgocc_mod[i]);  
  }

  for( int i=0; i<PixLayerIBL2D3D::COUNT; i++){
    if(nGoodChannels_layer[i] > 0){
      avgocc_mod[i] = nhits_mod[i]/nGoodChannels_layer[i];
    }
    if( nActiveChannels_layer[i] > 0){
      avgocc_active_mod[i] = nhits_mod[i]/nActiveChannels_layer[i];
    }
    ///
    if(m_avgocc_per_lumi_mod[i]) m_avgocc_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(),avgocc_mod[i]);
    if(m_avgocc_per_bcid_mod[i]) m_avgocc_per_bcid_mod[i]->Fill(pix_rod_bcid, avgocc_mod[i]);
    if(m_avgocc_active_per_lumi_mod[i]) m_avgocc_active_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(),avgocc_active_mod[i]);

    if(m_maxocc_per_lumi_mod[i]) m_maxocc_per_lumi_mod[i]->Fill(m_manager->lumiBlockNumber(), avgocc_mod[i]);
    if(m_maxocc_per_bcid_mod[i]){
      int bin = m_maxocc_per_bcid_mod[i]->GetXaxis()->FindBin( 1.0*pix_rod_bcid );
      double content = m_maxocc_per_bcid_mod[i]->GetBinContent( bin );
      if( avgocc_mod[i] > content ) m_maxocc_per_bcid_mod[i]->SetBinContent(bin, avgocc_mod[i]);
    }
    if(m_totalhits_per_bcid_mod[i]) m_totalhits_per_bcid_mod[i]->Fill(1.0*pix_rod_bcid, nhits_mod[i]);
    if(avgocc_mod[i] > 0.0007 && m_nlargeevt_per_lumi_mod[i]) m_nlargeevt_per_lumi_mod[i]->Fill( m_lumiBlockNum );
  }
  
  if(avgocc_mod[PixLayer::kB0] > 0 && m_avgocc_ratioIBLB0_per_lumi) 
    m_avgocc_ratioIBLB0_per_lumi->Fill(m_manager->lumiBlockNumber(), avgocc_mod[PixLayer::kIBL]/avgocc_mod[PixLayer::kB0]);
  
  
  if(m_Atlas_BCID_hits) m_Atlas_BCID_hits->Fill(pix_rod_bcid,nhits);
  
  /// Fill the #hit per module per event
  for(int i=0; i<PixLayer::COUNT; i++){
    for(int phi=0; phi<nmod_phi[i]; phi++){
      for(int eta=0; eta<nmod_eta[i]; eta++){
	if(i == PixLayer::kECA && m_nhits_mod[i]) m_nhits_mod[i]->Fill( HitPerEventArray_disksA[phi][eta] );
	if(i == PixLayer::kECC && m_nhits_mod[i]) m_nhits_mod[i]->Fill( HitPerEventArray_disksC[phi][eta] );
	if(i == PixLayer::kB0  && m_nhits_mod[i]) m_nhits_mod[i]->Fill( HitPerEventArray_l0[phi][eta]);
	if(i == PixLayer::kB1  && m_nhits_mod[i]) m_nhits_mod[i]->Fill( HitPerEventArray_l1[phi][eta]);
	if(i == PixLayer::kB2  && m_nhits_mod[i]) m_nhits_mod[i]->Fill( HitPerEventArray_l2[phi][eta]);
	if(i == PixLayer::kIBL && m_nhits_mod[i]) m_nhits_mod[i]->Fill( HitPerEventArray_lI[phi][eta]);
      }
    }
  }
  
  /// Put the #hits per event for each layer
  if ( m_event == 0) {
    for( int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++) {
      m_hitocc_stock[i].push_back( avgocc_mod[i] );
    }
  } else if ( !newLumiBlockFlag() ) {
    for( int i=0; i<PixLayer::COUNT-1+(int)(m_doIBL); i++) {
      m_hitocc_stock[i].push_back( avgocc_mod[i] );
    }
  }
  /// Fill some histograms only if =< 50% of modules disabled
  if(!m_majorityDisabled) {
    if (m_doDetails) {
      if(m_Details_mod1_num_hits) m_Details_mod1_num_hits->Fill(nhitsM1);
      if(m_Details_mod2_num_hits) m_Details_mod2_num_hits->Fill(nhitsM2);
      if(m_Details_mod3_num_hits) m_Details_mod3_num_hits->Fill(nhitsM3);
      if(m_Details_mod4_num_hits) m_Details_mod4_num_hits->Fill(nhitsM4);
    }
    if(m_num_hits) m_num_hits->Fill(nhits);
    if(m_doLumiBlock){ 
      if(m_num_hits_LB) m_num_hits_LB->Fill(nhits);
    }
    
    if (m_doModules) 
      {
	PixelID::const_id_iterator idIt    = m_pixelid->wafer_begin();
	PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();
	for (; idIt != idItEnd; ++idIt) {
	  Identifier WaferID = *idIt;
	  if(m_pixelid->barrel_ec(WaferID)==2 ){
	    m_hit_num_mod->Fill( HitPerEventArray_disksA[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid ,m_doIBL);
	  }
	  if(m_pixelid->barrel_ec(WaferID)==-2) m_hit_num_mod->Fill( HitPerEventArray_disksC[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid ,m_doIBL);
	  if(m_pixelid->barrel_ec(WaferID)==0 )
	    {
	      if(m_doIBL && m_pixelid->layer_disk(WaferID)==0) {
		m_hit_num_mod->Fill( HitPerEventArray_lI[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+10], WaferID, m_pixelid ,m_doIBL);
	      }
	      if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) {
		 m_hit_num_mod->Fill( HitPerEventArray_l0[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
	      }
	      if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) m_hit_num_mod->Fill( HitPerEventArray_l1[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
	      if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) m_hit_num_mod->Fill( HitPerEventArray_l2[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
	    }  
	  
	  if (m_doLumiBlock) {
	    if(m_pixelid->barrel_ec(WaferID)==2 ) m_hit_num_mod_LB->Fill( HitPerEventArray_disksA[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid ,m_doIBL);
	    if(m_pixelid->barrel_ec(WaferID)==-2) m_hit_num_mod_LB->Fill( HitPerEventArray_disksC[m_pixelid->phi_module(WaferID)][m_pixelid->layer_disk(WaferID)], WaferID, m_pixelid ,m_doIBL);
	    if(m_pixelid->barrel_ec(WaferID)==0 )                                                                                                                                       
	      {
		if(m_doIBL && m_pixelid->layer_disk(WaferID)==0) m_hit_num_mod_LB->Fill( HitPerEventArray_lI[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+10], WaferID, m_pixelid ,m_doIBL);
		if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) m_hit_num_mod_LB->Fill( HitPerEventArray_l0[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
		if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) m_hit_num_mod_LB->Fill( HitPerEventArray_l1[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
		if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) m_hit_num_mod_LB->Fill( HitPerEventArray_l2[m_pixelid->phi_module(WaferID)][m_pixelid->eta_module(WaferID)+6], WaferID, m_pixelid ,m_doIBL);
	      }  
	  }
	}
      }
  }
  
  /// Quick Status
  if (m_doOfflineAnalysis) {
    if( m_occupancy_L0_B11_S2_C6 ) {
      for(int i=0 ; i<96 ; i++)
	m_occupancy_L0_B11_S2_C6->Fill(m_manager->lumiBlockNumber(), i, (1.0*nhits_L0_B11_S2_C6[i]/(18.0*164.0)));
    }
  }
  
  for(int i=0; i<PixLayer::COUNT; i++){
    for( int phi=0; phi<nmod_phi[i]; phi++){
      for(int eta=0; eta<nmod_eta[i]; eta++){
	int nfes = 0;
	for(int j=0 ; j<16 ; j++){
	  if(i==PixLayer::kECA) nfes += fewithHits_EA[phi][eta][j];
	  if(i==PixLayer::kECC) nfes += fewithHits_EC[phi][eta][j];
	  if(i==PixLayer::kB0)  nfes += fewithHits_B0[phi][eta][j];
	  if(i==PixLayer::kB1)  nfes += fewithHits_B1[phi][eta][j];
	  if(i==PixLayer::kB2)  nfes += fewithHits_B2[phi][eta][j];
	}
	if(m_nFEswithHits_mod[i]) m_nFEswithHits_mod[i]->Fill(m_manager->lumiBlockNumber(), eta, nfes);
      }
    }
  }
  
  ////////////////////End fill after event block///////////
  if(nhits==0 && m_storegate_errors) m_storegate_errors->Fill(1.,4.);//first entry for RDO, second for size = 0
  
  return StatusCode::SUCCESS;
}

StatusCode PixelMainMon::ProcHitsMon(void)
{
   
  double events = m_event;
  if (events==0) return StatusCode::SUCCESS; //if no events, the rest of the test is pointless and would divide by 0
  
  if (m_doOffline)
    {
      FillSummaryHistos(m_occupancy, m_occupancy_summary_mod[PixLayer::kECA], m_occupancy_summary_mod[PixLayer::kECC], 
			m_occupancy_summary_mod[PixLayer::kIBL], m_occupancy_summary_mod[PixLayer::kB0], 
			m_occupancy_summary_mod[PixLayer::kB1],  m_occupancy_summary_mod[PixLayer::kB2]);
    }

  if (m_average_pixocc && m_num_hits->GetEntries() == m_event) {  
    m_event2 = m_num_hits->GetEntries();     //FIXME USED in Clusters!!!
  }

  return StatusCode::SUCCESS;
}

