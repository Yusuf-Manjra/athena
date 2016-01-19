/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Book and fill offline histograms showing status of modules
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMainMon.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "TH1F.h"   
#include "TH1I.h"   
#include "TH2F.h"
#include "TH2I.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "TProfile2D.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"
#include "InDetIdentifier/PixelID.h"
#include <sstream>
#include "PixelMonitoring/PixelMon2DMaps.h"
#include "PixelMonitoring/PixelMonModules.h"
#include "PixelMonitoring/PixelMonProfiles.h"

///////////////////////////////////////////////////////////////////////////////
//////////////////////booking methods//////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

StatusCode PixelMainMon::BookStatusMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Status" << endreq;  

   std::string path = "Pixel/Status";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/StatusOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/StatusOnPixelTrack");
   MonGroup statusHistos( this, path.c_str(), run, ATTRIB_MANAGED ); //declare a group of histograms

   // use the following definitions for modules states:
   // GOOD = active + good
   // DISABLED "BAD"  = active + bad
   // DISABLED "INACTIVE" = inactive + good/bad (inactive implicitely means bad though)

   m_status = new PixelMonProfiles("Map_Of_Modules_Status", ("Modules Status (0=Active+Good, 1=Active+Bad, 2=Inactive)" + m_histTitleExt).c_str());
   sc = m_status->regHist(statusHistos);
   m_status->SetMaxValue( 2.0 );

   if(m_doModules)
   {
     m_Status_modules = new PixelMonModules1D("Status_of_Module", ("Module Status (0=Active+Good, 1=Active+Bad, 2=Inactive)" + m_histTitleExt + ";Status").c_str(),2,0,2,m_doIBL);
     sc = m_Status_modules->regHist(this, (path+"/Modules_Status").c_str(),run,m_doIBL);
      
      m_Status_modules->SetBinLabel( "Status",2 ); 
      m_Status_modules->formatHist("status",m_doIBL);
   }
   if(m_doOffline)
   {
      m_dqStatus = new PixelMon2DMaps("Ok_modules", ("module problems, empty bin means dead module not listed in status database"+ m_histTitleExt).c_str());
      sc = m_dqStatus->regHist(statusHistos);
   }
   std::string tmp;
   std::string tmp2;

   int nbins_LB = 2500; double min_LB = -0.5; double max_LB = min_LB + (1.0*nbins_LB);
   std::string atext_LB = ";lumi block";
   std::string atext_nmod = ";# modules/event";

   tmp = "BadModules_per_lumi"; tmp2 = "Number of bad modules (bad+active) per event per LB";
   sc = statusHistos.regHist(m_badModules_per_lumi     = TProfile_LW::create(tmp.c_str(),           (tmp2+        m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_badModules_per_lumi_ECA = TProfile_LW::create((tmp + "_ECA").c_str(),(tmp2+", ECA"+m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_badModules_per_lumi_ECC = TProfile_LW::create((tmp + "_ECC").c_str(),(tmp2+", ECC"+m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_badModules_per_lumi_B0  = TProfile_LW::create((tmp + "_B0").c_str(), (tmp2+", B0" +m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_badModules_per_lumi_B1  = TProfile_LW::create((tmp + "_B1").c_str(), (tmp2+", B1" +m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_badModules_per_lumi_B2  = TProfile_LW::create((tmp + "_B2").c_str(), (tmp2+", B2" +m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   if(m_doIBL){
   sc = statusHistos.regHist(m_badModules_per_lumi_IBL = TProfile_LW::create((tmp + "_IBL").c_str(),(tmp2+", IBL"+m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   }

   //tmp = "DisabledModules_per_lumi"; tmp2 = "Number of disabled modules (inactive or bad) per event per LB";
   tmp = "DisabledModules_per_lumi"; tmp2 = "Number of disabled modules per event per LB";
   sc = statusHistos.regHist(m_disabledModules_per_lumi     = TProfile_LW::create(tmp.c_str(),            (tmp2+        m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_disabledModules_per_lumi_PIX = TProfile_LW::create((tmp + "_PIX").c_str(), (tmp2+", PIX"+m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_disabledModules_per_lumi_ECA = TProfile_LW::create((tmp + "_ECA").c_str(), (tmp2+", ECA"+m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_disabledModules_per_lumi_ECC = TProfile_LW::create((tmp + "_ECC").c_str(), (tmp2+", ECC"+m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_disabledModules_per_lumi_B0  = TProfile_LW::create((tmp + "_B0").c_str(),  (tmp2+", B0" +m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_disabledModules_per_lumi_B1  = TProfile_LW::create((tmp + "_B1").c_str(),  (tmp2+", B1" +m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_disabledModules_per_lumi_B2  = TProfile_LW::create((tmp + "_B2").c_str(),  (tmp2+", B2" +m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   if(m_doIBL){
   sc = statusHistos.regHist(m_disabledModules_per_lumi_IBL = TProfile_LW::create((tmp + "_IBL").c_str(), (tmp2+", IBL"+m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   }

   tmp = "BadDisabledModules_per_lumi"; tmp2 = "Number of disabled & bad modules per event per LB";
   sc = statusHistos.regHist(m_baddisabledModules_per_lumi_ECA = TProfile_LW::create((tmp + "_ECA").c_str(), (tmp2+", ECA"+m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_baddisabledModules_per_lumi_ECC = TProfile_LW::create((tmp + "_ECC").c_str(), (tmp2+", ECC"+m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_baddisabledModules_per_lumi_B0  = TProfile_LW::create((tmp + "_B0").c_str(),  (tmp2+", B0" +m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_baddisabledModules_per_lumi_B1  = TProfile_LW::create((tmp + "_B1").c_str(),  (tmp2+", B1" +m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   sc = statusHistos.regHist(m_baddisabledModules_per_lumi_B2  = TProfile_LW::create((tmp + "_B2").c_str(),  (tmp2+", B2" +m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   if(m_doIBL){
   sc = statusHistos.regHist(m_baddisabledModules_per_lumi_IBL = TProfile_LW::create((tmp + "_IBL").c_str(), (tmp2+", IBL"+m_histTitleExt+atext_LB+atext_nmod).c_str(), nbins_LB, min_LB, max_LB));
   }

   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;         
   return StatusCode::SUCCESS;
}  

StatusCode PixelMainMon::BookStatusLumiBlockMon(void)
{
   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "starting Book Status for lowStat" << endreq;  

   std::string path = "Pixel/LumiBlock";
   if(m_doOnTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnTrack");
   if(m_doOnPixelTrack) path.replace(path.begin(), path.end(), "Pixel/LumiBlockOnPixelTrack");
   MonGroup lumiBlockHist(   this, path.c_str(), lowStat, ATTRIB_MANAGED); //declare a group of histograms

   m_status_LB = new PixelMonProfiles("Map_Of_Modules_Status_LB", ("Module Status (0=Active+Good, 1=Active+Bad, 2=Inactive)"+ m_histTitleExt).c_str());
   sc = m_status_LB->regHist(lumiBlockHist);
     
   if(sc.isFailure())if(msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "histograms not booked" << endreq;         
   return StatusCode::SUCCESS;
}  

StatusCode PixelMainMon::FillStatusMon(void)
{
   int Index = -1;
   PixelID::const_id_iterator idIt    = m_pixelid->wafer_begin();
   PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();

   int nBad=0;
   int nBad_ECA=0;
   int nBad_ECC=0;
   int nBad_IBL=0;
   int nBad_B0=0;
   int nBad_B1=0;
   int nBad_B2=0;

   int nDisabled=0;
   int nDisabled_ECA=0;
   int nDisabled_ECC=0;
   int nDisabled_IBL=0;
   int nDisabled_B0=0;
   int nDisabled_B1=0;
   int nDisabled_B2=0;


   if(m_isNewLumiBlock && m_Status_modules) m_Status_modules->Reset();

   for (; idIt != idItEnd; ++idIt)
   {
      Identifier WaferID = *idIt;
      IdentifierHash id_hash = m_pixelid->wafer_hash(WaferID); 

      // check in order of occurrence to reduce number of calls to conditions service
      if      (m_pixelCondSummarySvc->isActive(id_hash) == true && m_pixelCondSummarySvc->isGood(id_hash) == true ) {Index=0;}
      else if (m_pixelCondSummarySvc->isActive(id_hash) == false) {Index=2;}
      else {Index=1;}

      if(m_status) m_status->Fill(WaferID,m_pixelid,Index,m_doIBL);

      if(m_doLumiBlock){
	      if(m_status_LB) m_status_LB->Fill(WaferID,m_pixelid,Index,m_doIBL);
      }

      if(Index > 0) // bad but active modules  
      {
	      if (Index == 1) {
	         nBad++;
	         if(m_pixelid->barrel_ec(WaferID)==2)  nBad_ECA++;
	         if(m_pixelid->barrel_ec(WaferID)==-2) nBad_ECC++;
	         if(m_pixelid->barrel_ec(WaferID)==0) {
	            if(m_pixelid->layer_disk(WaferID)==0 && m_doIBL) nBad_IBL++;
	            if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nBad_B0++;
	            if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nBad_B1++;
	            if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nBad_B2++;
            }
	      }
	      // inactive or bad modules
	      // should maybe use only inactive modules for these, however, since tracking etc use "disabled module" as !(active+good)
	      // continue monitoring that quantity for now
         if(Index == 2)
	      {
            nDisabled++;
	         if(m_pixelid->barrel_ec(WaferID)==2)  nDisabled_ECA++;
	         if(m_pixelid->barrel_ec(WaferID)==-2) nDisabled_ECC++;
	         if (m_pixelid->barrel_ec(WaferID)==0) {
	            if(m_pixelid->layer_disk(WaferID)==0 && m_doIBL) nDisabled_IBL++;
	            if(m_pixelid->layer_disk(WaferID)==0+m_doIBL) nDisabled_B0++;
	            if(m_pixelid->layer_disk(WaferID)==1+m_doIBL) nDisabled_B1++;
	            if(m_pixelid->layer_disk(WaferID)==2+m_doIBL) nDisabled_B2++;
	         }
         }

	      if (m_Status_modules)
         {
            int diffToFill=0;
            double content = floor(m_Status_modules->GetBinContent(1.5,WaferID,m_pixelid));   // 1.5 refers to the bin [1,2] 
            // If we have module in state 1 (active+bad), and get 2 later (inactive), want to add 1 it to put module in state 2 
            if(content==2)
            {
               diffToFill=0;
            }
            else if(content==1 && (Index==2))
            {
               diffToFill=1;
            }
            else if(content==0)
            {
               diffToFill=Index;
            }
            for(int i=0; i<diffToFill; i++) m_Status_modules->Fill(1.5,WaferID,m_pixelid,m_doIBL);  //fill to the required value
         }
      }
   } // End of loop 

   static float nmod[PixLayer::COUNT] = {144., 144., 286., 494., 676., 280.};

   if(m_disabledModules_per_lumi)     m_disabledModules_per_lumi->Fill(m_manager->lumiBlockNumber(),nDisabled); 
   if(m_disabledModules_per_lumi_PIX) m_disabledModules_per_lumi_PIX->Fill(m_manager->lumiBlockNumber(),nDisabled-nDisabled_IBL);
   if(m_disabledModules_per_lumi_ECA) m_disabledModules_per_lumi_ECA->Fill(m_manager->lumiBlockNumber(),nDisabled_ECA); 
   if(m_disabledModules_per_lumi_ECC) m_disabledModules_per_lumi_ECC->Fill(m_manager->lumiBlockNumber(),nDisabled_ECC); 
   if(m_disabledModules_per_lumi_IBL) m_disabledModules_per_lumi_IBL->Fill(m_manager->lumiBlockNumber(),nDisabled_IBL);
   if(m_disabledModules_per_lumi_B0)  m_disabledModules_per_lumi_B0->Fill(m_manager->lumiBlockNumber(),nDisabled_B0); 
   if(m_disabledModules_per_lumi_B1)  m_disabledModules_per_lumi_B1->Fill(m_manager->lumiBlockNumber(),nDisabled_B1); 
   if(m_disabledModules_per_lumi_B2)  m_disabledModules_per_lumi_B2->Fill(m_manager->lumiBlockNumber(),nDisabled_B2); 

   if(m_badModules_per_lumi)     m_badModules_per_lumi->Fill(m_manager->lumiBlockNumber(),nBad); 
   if(m_badModules_per_lumi_ECA) m_badModules_per_lumi_ECA->Fill(m_manager->lumiBlockNumber(),nBad_ECA/nmod[PixLayer::kECA]); 
   if(m_badModules_per_lumi_ECC) m_badModules_per_lumi_ECC->Fill(m_manager->lumiBlockNumber(),nBad_ECC/nmod[PixLayer::kECC]); 
   if(m_badModules_per_lumi_IBL) m_badModules_per_lumi_IBL->Fill(m_manager->lumiBlockNumber(),nBad_IBL/nmod[PixLayer::kIBL]);
   if(m_badModules_per_lumi_B0)  m_badModules_per_lumi_B0->Fill(m_manager->lumiBlockNumber(),nBad_B0/nmod[PixLayer::kB0]); 
   if(m_badModules_per_lumi_B1)  m_badModules_per_lumi_B1->Fill(m_manager->lumiBlockNumber(),nBad_B1/nmod[PixLayer::kB1]); 
   if(m_badModules_per_lumi_B2)  m_badModules_per_lumi_B2->Fill(m_manager->lumiBlockNumber(),nBad_B2/nmod[PixLayer::kB2]); 

   if(m_baddisabledModules_per_lumi_ECA) m_baddisabledModules_per_lumi_ECA->Fill(m_manager->lumiBlockNumber(),nDisabled_ECA+nBad_ECA); 
   if(m_baddisabledModules_per_lumi_ECC) m_baddisabledModules_per_lumi_ECC->Fill(m_manager->lumiBlockNumber(),nDisabled_ECC+nBad_ECC); 
   if(m_baddisabledModules_per_lumi_IBL) m_baddisabledModules_per_lumi_IBL->Fill(m_manager->lumiBlockNumber(),nDisabled_IBL+nBad_IBL);
   if(m_baddisabledModules_per_lumi_B0)  m_baddisabledModules_per_lumi_B0->Fill(m_manager->lumiBlockNumber(),nDisabled_B0+nBad_B0); 
   if(m_baddisabledModules_per_lumi_B1)  m_baddisabledModules_per_lumi_B1->Fill(m_manager->lumiBlockNumber(),nDisabled_B1+nBad_B1); 
   if(m_baddisabledModules_per_lumi_B2)  m_baddisabledModules_per_lumi_B2->Fill(m_manager->lumiBlockNumber(),nDisabled_B2+nBad_B2); 

   if (nDisabled > (1744+280*m_doIBL)*0.50) {
     m_majorityDisabled = true;
   }
   else {
     m_majorityDisabled = false;
   }

   return StatusCode::SUCCESS;

}   

StatusCode PixelMainMon::ProcStatusMon(void)
{
   if(m_status && m_dqStatus && m_occupancy)
   {
      if(m_doIBL){
	      for(int i=1;i<=12;i++){
	         for(int j=1;j<=14;j++){
	            m_dqStatus->IBL2D->SetBinContent(i,j, m_occupancy->IBL2D->GetBinContent(i,j) + m_status->IBL2D->GetBinContent(i,j) );
	         }	 
	      }
	      for(int i=1;i<=8;i++){
	         for(int j=1;j<=14;j++){
	            m_dqStatus->IBL3D->SetBinContent(i,j, m_occupancy->IBL3D->GetBinContent(i,j) + m_status->IBL3D->GetBinContent(i,j) );
	         }	 
	      }
      }
      for(int i=1;i<=13;i++){
	      for(int j=1;j<=22;j++)
	         m_dqStatus->B0->SetBinContent(i,j, m_occupancy->B0->GetBinContent(i,j) + m_status->B0->GetBinContent(i,j) );
	      for(int j=1;j<=38;j++)                                                                                   
	         m_dqStatus->B1->SetBinContent(i,j, m_occupancy->B1->GetBinContent(i,j) + m_status->B1->GetBinContent(i,j) );
	      for(int j=1;j<=52;j++)                                                                                   
	         m_dqStatus->B2->SetBinContent(i,j, m_occupancy->B2->GetBinContent(i,j) + m_status->B2->GetBinContent(i,j) );
	   }
      for(int i=1;i<=48;i++){
	      for(int j=1;j<=3;j++){
	         m_dqStatus->A->SetBinContent(j,i, m_occupancy->A->GetBinContent(j,i) + m_status->A->GetBinContent(j,i) );
	         m_dqStatus->C->SetBinContent(j,i, m_occupancy->C->GetBinContent(j,i) + m_status->C->GetBinContent(j,i) );
	      }
	   }
   }
   return StatusCode::SUCCESS;                  
}                                               

