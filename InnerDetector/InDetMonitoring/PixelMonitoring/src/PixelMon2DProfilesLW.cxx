/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Function to handle 2D profile histograms of modules, one for each region
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/PixelMon2DProfilesLW.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "InDetIdentifier/PixelID.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TH2F_LW.h"
#include "GaudiKernel/StatusCode.h"     
#include <string.h>

PixelMon2DProfilesLW::PixelMon2DProfilesLW(std::string name, std::string title, bool doIBL, bool errorHist, bool copy2DFEval)
{
  std::string setatext = ";shifted eta index of module";
  std::string etatext = ";eta index of module";
  std::string phitext = ";phi index of module";
  std::string disktext = ";disk number";

  IBL3D = TProfile2D_LW::create((name+"_IBL3D").c_str(), (title + ", IBL 3D modules " + etatext + phitext).c_str(),8,-.5,7.5,14,-0.5,13.5);
  IBL2D = TProfile2D_LW::create((name+"_IBL2D").c_str(), (title + ", IBL planar modules " + setatext + phitext).c_str(),12,-6.5,5.5,14,-0.5,13.5);
  IBL   = TProfile2D_LW::create((name+"_IBL").c_str(),   (title + ", IBL " + setatext + phitext).c_str(),32,-16.5,15.5,14,-0.5,13.5);
  B0    = TProfile2D_LW::create((name+"_B0").c_str(),    (title + ", B0 " + etatext + phitext).c_str(),13,-6.5,6.5,22,-0.5,21.5);
  B1    = TProfile2D_LW::create((name+"_B1").c_str(),    (title + ", B1 " + etatext + phitext).c_str(),13,-6.5,6.5,38,-0.5,37.5);
  B2    = TProfile2D_LW::create((name+"_B2").c_str(),    (title + ", B2 " + etatext + phitext).c_str(),13,-6.5,6.5,52,-0.5,51.5);
  A     = TProfile2D_LW::create((name+"_ECA" ).c_str(),  (title + ", ECA " + disktext + phitext).c_str(),3,-0.5,2.5,48,-0.5,47.5);
  C     = TProfile2D_LW::create((name+"_ECC" ).c_str(),  (title + ", ECC " + disktext + phitext).c_str(),3,-0.5,2.5,48,-0.5,47.5);
  //DBMA = TProfile2D_LW::create((name+"_DBMA" ).c_str(),  (title + ", DBMA " + disktext + phitext).c_str(),3,-0.5,2.5,4,-0.5,3.5);
  //DBMC = TProfile2D_LW::create((name+"_DBMC" ).c_str(),  (title + ", DBMC " + disktext + phitext).c_str(),3,-0.5,2.5,4,-0.5,3.5);
  mDoIBL = doIBL;
  mDoErrorHist = errorHist;
  mCopy2DFEval = copy2DFEval;
  formatHist();
}

PixelMon2DProfilesLW::~PixelMon2DProfilesLW()
{
   LWHist::safeDelete(IBL3D);
   LWHist::safeDelete(IBL2D);
   LWHist::safeDelete(IBL);
   LWHist::safeDelete(B0);
   LWHist::safeDelete(B1);
   LWHist::safeDelete(B2);
   LWHist::safeDelete(A);
   LWHist::safeDelete(C);
   //LWHist::safeDelete(DBMA);
   //LWHist::safeDelete(DBMC);
}

void PixelMon2DProfilesLW::Fill(Identifier &id, const PixelID* pixID, float weight)
{
  int bec = pixID->barrel_ec(id);
  int ld  = pixID->layer_disk(id);
  int pm  = pixID->phi_module(id);

  if (bec==2) A->Fill(ld,pm,weight); 
  else if (bec==-2) C->Fill(ld,pm,weight);
  //else if (bec==4) DBMA->Fill(ld, pm, weight);
  //else if (bec==-4) DBMC->Fill(ld, pm, weight);

  else if (bec==0) {
    if (mDoIBL) ld--;
    int em  = pixID->eta_module(id);
    if (ld ==0) { 
      B0->Fill(em,pm,weight);
    }
    else if (ld ==1) { 
      B1->Fill(em,pm,weight);
    }
    else if (ld ==2) { 
      B2->Fill(em,pm,weight);
    }
    else if (ld ==-1 && mDoIBL && !mDoErrorHist) {
      int feid = 0;
      int emf = 0;
      bool copy = false;
      if (em<6 && em>-7) {
	if (pixID->eta_index(id) >= 80) feid = 1;
	emf = 2 * em + feid; 
	IBL2D->Fill(em,pm,weight);
	copy = true;
      }
      else if (em<-6) {
	emf = em - 6;
	IBL3D->Fill(em+10,pm,weight); 
      }
      else {
	emf = em + 6;
	IBL3D->Fill(em-2,pm,weight); 
      }
      IBL->Fill(emf,pm,weight);
      if (mCopy2DFEval && copy) IBL->Fill(emf+1, pm, weight);
    }
  }
}

void PixelMon2DProfilesLW::formatHist()
{
   const int ndisk = 3;
   const int nphi  = 48;
   const int nphi_dbm  = 4;
   const char *disk[ndisk] = { "Disk 1", "Disk 2", "Disk 3" };
   //const char *phi_dbm[nphi_dbm] = { "M1","M2","M3","M4"};
   const int nmod = 13;
   const int nmodIBL2D = 12;
   const int nmodIBL3D = 8;
   const int nmodIBL = 32;
   const char *mod[nmod] = { "M6C", "M5C", "M4C", "M3C", "M2C", "M1C", "M0","M1A", "M2A", "M3A", "M4A", "M5A", "M6A" } ;
   const char *modIBL2D[nmodIBL2D] = { "M3_C6", "M3_C5", "M2_C4", "M1_C3", "M1_C2", "M1_C1", "M1_A1", "M1_A2", "M2_A3", "M2_A4", "M3_A5", "M3_A6" } ;
   const char *modIBL3D[nmodIBL3D] = {"M4_C8_2","M4_C8_1","M4_C7_2","M4_C7_1","M4_A7_1","M4_A7_2","M4_A8_1","M4_A8_2"};
   const char *modIBL[nmodIBL] = {"C8","","C7","",
				  "C6","","C5","",
				  "C4","","C3","",
				  "C2","","C1","",
				  "A1","","A2","",
				  "A3","","A4","",
				  "A5","","A6","",
				  "A7","","A8",""};
   const int nstaveb = 14;
   const char *staveb[nstaveb] = {
     "S01", "S02", "S03", "S04", "S05", "S06","S07",
     "S08", "S09", "S10", "S11", "S12", "S13","S14"};
   const int nstave0 = 22;
   const char *stave0[nstave0] = {                      "B11_S2", 
      "B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2",
      "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2",
      "B07_S1", "B07_S2", "B08_S1", "B08_S2", "B09_S1", "B09_S2",
      "B10_S1", "B10_S2", "B11_S1" };
   const int nstave1 = 38;
   const char *stave1[nstave1] = {
      "B01_S1", "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2",
      "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2",
      "B07_S1", "B07_S2", "B08_S1", "B08_S2", "B09_S1", "B09_S2",
      "B10_S1", "B10_S2", "B11_S1", "B11_S2", "B12_S1", "B12_S2",
      "B13_S1", "B13_S2", "B14_S1", "B14_S2", "B15_S1", "B15_S2",
      "B16_S1", "B16_S2", "B17_S1", "B17_S2", "B18_S1", "B18_S2",
      "B19_S1", "B19_S2" };
   const int nstave2 = 52;
   const char *stave2[nstave2] = {          
                "B01_S2", "B02_S1", "B02_S2", "B03_S1", "B03_S2",
      "B04_S1", "B04_S2", "B05_S1", "B05_S2", "B06_S1", "B06_S2",
      "B07_S1", "B07_S2", "B08_S1", "B08_S2", "B09_S1", "B09_S2",
      "B10_S1", "B10_S2", "B11_S1", "B11_S2", "B12_S1", "B12_S2",
      "B13_S1", "B13_S2", "B14_S1", "B14_S2", "B15_S1", "B15_S2",
      "B16_S1", "B16_S2", "B17_S1", "B17_S2", "B18_S1", "B18_S2",
      "B19_S1", "B19_S2", "B20_S1", "B20_S2", "B21_S1", "B21_S2",
      "B22_S1", "B22_S2", "B23_S1", "B23_S2", "B24_S1", "B24_S2",
      "B25_S1", "B25_S2", "B26_S1", "B26_S2", "B01_S1" };
   const char *nstaveA[nphi] = {
      "B01_S2_M1", "B01_S2_M6", "B01_S2_M2", "B01_S2_M5", "B01_S2_M3", "B01_S2_M4", 
      "B02_S1_M1", "B02_S1_M6", "B02_S1_M2", "B02_S1_M5", "B02_S1_M3", "B02_S1_M4", 
      "B02_S2_M1", "B02_S2_M6", "B02_S2_M2", "B02_S2_M5", "B02_S2_M3", "B02_S2_M4", 
      "B03_S1_M1", "B03_S1_M6", "B03_S1_M2", "B03_S1_M5", "B03_S1_M3", "B03_S1_M4", 
      "B03_S2_M1", "B03_S2_M6", "B03_S2_M2", "B03_S2_M5", "B03_S2_M3", "B03_S2_M4", 
      "B04_S1_M1", "B04_S1_M6", "B04_S1_M2", "B04_S1_M5", "B04_S1_M3", "B04_S1_M4", 
      "B04_S2_M1", "B04_S2_M6", "B04_S2_M2", "B04_S2_M5", "B04_S2_M3", "B04_S2_M4", 
      "B01_S1_M1", "B01_S1_M6", "B01_S1_M2", "B01_S1_M5", "B01_S1_M3", "B01_S1_M4"};
   const char *nstaveC[nphi] = {
      "B01_S2_M4", "B01_S2_M3", "B01_S2_M5", "B01_S2_M2", "B01_S2_M6", "B01_S2_M1", 
      "B02_S1_M4", "B02_S1_M3", "B02_S1_M5", "B02_S1_M2", "B02_S1_M6", "B02_S1_M1", 
      "B02_S2_M4", "B02_S2_M3", "B02_S2_M5", "B02_S2_M2", "B02_S2_M6", "B02_S2_M1", 
      "B03_S1_M4", "B03_S1_M3", "B03_S1_M5", "B03_S1_M2", "B03_S1_M6", "B03_S1_M1", 
      "B03_S2_M4", "B03_S2_M3", "B03_S2_M5", "B03_S2_M2", "B03_S2_M6", "B03_S2_M1", 
      "B04_S1_M4", "B04_S1_M3", "B04_S1_M5", "B04_S1_M2", "B04_S1_M6", "B04_S1_M1", 
      "B04_S2_M4", "B04_S2_M3", "B04_S2_M5", "B04_S2_M2", "B04_S2_M6", "B04_S2_M1", 
      "B01_S1_M4", "B01_S1_M3", "B01_S1_M5", "B01_S1_M2", "B01_S1_M6", "B01_S1_M1"};

   for (int i=0; i<nphi; i++) {
      A->GetYaxis()->SetBinLabel( i+1, nstaveA[i] );
      C->GetYaxis()->SetBinLabel( i+1, nstaveC[i] );
   }

   for (int i=0; i<nphi_dbm; i++) {
      //DBMA->GetYaxis()->SetBinLabel( i+1, phi_dbm[i] );
      //DBMC->GetYaxis()->SetBinLabel( i+1, phi_dbm[i] );
   }

   for (int i=0; i<ndisk; i++) {
      A->GetXaxis()->SetBinLabel( i+1, disk[i] );
      C->GetXaxis()->SetBinLabel( i+1, disk[i] );
      //DBMA->GetXaxis()->SetBinLabel( i+1, disk[i] );
      //DBMC->GetXaxis()->SetBinLabel( i+1, disk[i] );
   }

   for (int i=0; i<nmod; i++) {
     B0->GetXaxis()->SetBinLabel( i+1, mod[i] );  // bin 0 is underflow
     B1->GetXaxis()->SetBinLabel( i+1, mod[i] );
     B2->GetXaxis()->SetBinLabel( i+1, mod[i] );
   }

   if (mDoIBL && !mDoErrorHist) {
     for (int i=0; i<nmodIBL; i++)   IBL->GetXaxis()->SetBinLabel( i+1, modIBL[i] );
     for (int i=0; i<nstaveb; i++)   IBL->GetYaxis()->SetBinLabel( i+1, staveb[i] );
     for (int i=0; i<nmodIBL2D; i++) IBL2D->GetXaxis()->SetBinLabel( i+1, modIBL2D[i] );
     for (int i=0; i<nstaveb; i++)   IBL2D->GetYaxis()->SetBinLabel( i+1, staveb[i] );
     for (int i=0; i<nmodIBL3D; i++) IBL3D->GetXaxis()->SetBinLabel( i+1, modIBL3D[i] );
     for (int i=0; i<nstaveb; i++)   IBL3D->GetYaxis()->SetBinLabel( i+1, staveb[i] );
   }

   for (int i=0; i<nstave0; i++) B0->GetYaxis()->SetBinLabel( i+1, stave0[i] ); 
   for (int i=0; i<nstave1; i++) B1->GetYaxis()->SetBinLabel( i+1, stave1[i] ); 
   for (int i=0; i<nstave2; i++) B2->GetYaxis()->SetBinLabel( i+1, stave2[i] ); 
  

   if (mDoIBL && !mDoErrorHist){
     IBL->GetYaxis()->SetLabelSize(0.03);
     IBL2D->GetYaxis()->SetLabelSize(0.03);
     IBL3D->GetYaxis()->SetLabelSize(0.03);
     IBL->SetOption("colz");
     IBL2D->SetOption("colz");
     IBL3D->SetOption("colz");
     IBL->SetMinimum(0.);
     IBL2D->SetMinimum(0.);
     IBL3D->SetMinimum(0.);
   }

   //Make the text smaller
   B0->GetYaxis()->SetLabelSize(0.03);
   B1->GetYaxis()->SetLabelSize(0.03);
   B2->GetYaxis()->SetLabelSize(0.03);
   A->GetYaxis()->SetLabelSize(0.02);
   C->GetYaxis()->SetLabelSize(0.02);
   //DBMA->GetYaxis()->SetLabelSize(0.02);
   //DBMC->GetYaxis()->SetLabelSize(0.02);
   //Move the lable so you can read it
   // IBL2D->GetYaxis()->SetTitleOffset(1.35);
   // IBL3D->GetYaxis()->SetTitleOffset(1.35);
   // B0->GetYaxis()->SetTitleOffset(1.35);
   // B1->GetYaxis()->SetTitleOffset(1.35);
   // B2->GetYaxis()->SetTitleOffset(1.35);
   // A->GetYaxis()->SetTitleOffset(1.35);
   // C->GetYaxis()->SetTitleOffset(1.35);
   //put histograms in the easier to read colz format
   B0->SetOption("colz");
   B1->SetOption("colz");
   B2->SetOption("colz");
   A->SetOption("colz");
   C->SetOption("colz");
   //DBMA->SetOption("colz");
   //DBMC->SetOption("colz");
   //force the minimum to be 0 so you can spot empty blocks easily
   B0->SetMinimum(0.);
   B1->SetMinimum(0.);
   B2->SetMinimum(0.);
   A->SetMinimum(0.);
   C->SetMinimum(0.);
   //DBMA->SetMinimum(0.);
   //DBMC->SetMinimum(0.);
   //Remvoe the stats box because it's in the way
   // IBL2D->SetStats(0.);
   // IBL3D->SetStats(0.);
   // B0->SetStats(0.);
   // B1->SetStats(0.);
   // B2->SetStats(0.);
   // A->SetStats(0.);
   // C->SetStats(0.);

}

void PixelMon2DProfilesLW::Fill2DMon(PixelMon2DProfilesLW* oldmap)
{
   //for(int x=1; x<=DBMA->GetNbinsX(); x++){
   //   for(int y=1; y<=DBMA->GetNbinsY(); y++){
   //      float content = oldmap->DBMA->GetBinContent(x, y);
   //      DBMA->SetBinContent(x, y, content);
   //      oldmap->DBMA->SetBinContent(x, y, 0);
   //   }
   //}
   //for(int x=1; x<=DBMC->GetNbinsX(); x++){
   //   for(int y=1; y<=DBMC->GetNbinsY(); y++){
   //      float content = oldmap->DBMC->GetBinContent(x, y);
   //      DBMC->SetBinContent(x, y, content);
   //      oldmap->DBMC->SetBinContent(x, y, 0);
   //   }
   //}
   for(unsigned int x=1; x<=A->GetNbinsX(); x++){
      for(unsigned int y=1; y<=A->GetNbinsY(); y++){
         float content = oldmap->A->GetBinContent(x, y);
         A->SetBinContent(x, y, content);
         oldmap->A->SetBinContent(x, y, 0);
      }
   }
   for(unsigned int x=1; x<=C->GetNbinsX(); x++){
      for(unsigned int y=1; y<=C->GetNbinsY(); y++){
         float content = oldmap->C->GetBinContent(x, y);
         C->SetBinContent(x, y, content);
         oldmap->C->SetBinContent(x, y, 0);
      }
   }
   for(unsigned int x=1; x<=B0->GetNbinsX(); x++){
      for(unsigned int y=1; y<=B0->GetNbinsY(); y++){
         float content = oldmap->B0->GetBinContent(x, y);
         B0->SetBinContent(x, y, content);
         oldmap->B0->SetBinContent(x, y, 0);
      }
   }
   for(unsigned int x=1; x<=B1->GetNbinsX(); x++){
      for(unsigned int y=1; y<=B1->GetNbinsY(); y++){
         float content = oldmap->B1->GetBinContent(x, y);
         B1->SetBinContent(x, y, content);
         oldmap->B1->SetBinContent(x, y, 0);
      }
   }
   for(unsigned int x=1; x<=B2->GetNbinsX(); x++){
      for(unsigned int y=1; y<=B2->GetNbinsY(); y++){
         float content = oldmap->B2->GetBinContent(x, y);
         B2->SetBinContent(x, y, content );
         oldmap->B2->SetBinContent(x, y, 0);
      }
   }
   for(unsigned int x=1; x<=IBL->GetNbinsX(); x++){
      for(unsigned int y=1; y<=IBL->GetNbinsY(); y++){
         float content = oldmap->IBL->GetBinContent(x, y);
         IBL->SetBinContent(x, y, content);
         oldmap->IBL->SetBinContent(x, y, 0);
      }
   }
   for(unsigned int x=1; x<=IBL2D->GetNbinsX(); x++){
      for(unsigned int y=1; y<=IBL2D->GetNbinsY(); y++){
         float content = oldmap->IBL2D->GetBinContent(x, y);
         IBL2D->SetBinContent(x, y, content);
         oldmap->IBL2D->SetBinContent(x, y, 0);
      }
   }
   for(unsigned int x=1; x<=IBL3D->GetNbinsX(); x++){
      for(unsigned int y=1; y<=IBL3D->GetNbinsY(); y++){
         float content = oldmap->IBL3D->GetBinContent(x, y);
         IBL3D->SetBinContent(x, y, content);
         oldmap->IBL3D->SetBinContent(x, y, 0);
      }
   }
}

void PixelMon2DProfilesLW::FillFromMap(PixelMon2DMapsLW* inputmap)
{
  float weightIBL   = 1.0 / 26880.0;
  float weightPixel = 1.0 / 46080.0;


  //for(int x=1; x<=DBMA->GetNbinsX(); x++){
  //   for(int y=1; y<=DBMA->GetNbinsY(); y++){
  //      float content = inputmap->DBMA->GetBinContent(x, y);
  //      DBMA->Fill(inputmap->DBMA->GetXaxis()->GetBinCenter(x), inputmap->DBMA->GetYaxis()->GetBinCenter(y), content*weightIBL);
  //      inputmap->DBMA->SetBinContent(x, y, 0);
  //   }
  //}
  //for(int x=1; x<=DBMC->GetNbinsX(); x++){
  //   for(int y=1; y<=DBMC->GetNbinsY(); y++){
  //      float content = inputmap->DBMC->GetBinContent(x, y);
  //      DBMC->Fill(inputmap->DBMC->GetXaxis()->GetBinCenter(x), inputmap->DBMC->GetYaxis()->GetBinCenter(y), content*weightIBL);
  //      inputmap->DBMC->SetBinContent(x, y, 0);
  //   }
  //}
  
  for(unsigned int x=1; x<=A->GetNbinsX(); x++){
    for(unsigned int y=1; y<=A->GetNbinsY(); y++){
      float content = inputmap->A->GetBinContent(x, y);
      A->Fill(inputmap->A->GetXaxis()->GetBinCenter(x), inputmap->A->GetYaxis()->GetBinCenter(y), content*weightPixel);
      inputmap->A->SetBinContent(x, y, 0);
    }
  }
  for(unsigned int x=1; x<=C->GetNbinsX(); x++){
    for(unsigned int y=1; y<=C->GetNbinsY(); y++){
      float content = inputmap->C->GetBinContent(x, y);
      C->Fill(inputmap->C->GetXaxis()->GetBinCenter(x), inputmap->C->GetYaxis()->GetBinCenter(y), content*weightPixel);
      inputmap->C->SetBinContent(x, y, 0);
    }
  }
  for(unsigned int x=1; x<=B0->GetNbinsX(); x++){
    for(unsigned int y=1; y<=B0->GetNbinsY(); y++){
      float content = inputmap->B0->GetBinContent(x, y);
      B0->Fill(inputmap->B0->GetXaxis()->GetBinCenter(x), inputmap->B0->GetYaxis()->GetBinCenter(y), content*weightPixel);
      inputmap->B0->SetBinContent(x, y, 0);
    }
  }
  for(unsigned int x=1; x<=B1->GetNbinsX(); x++){
    for(unsigned int y=1; y<=B1->GetNbinsY(); y++){
      float content = inputmap->B1->GetBinContent(x, y);
      B1->Fill(inputmap->B1->GetXaxis()->GetBinCenter(x), inputmap->B1->GetYaxis()->GetBinCenter(y), content*weightPixel);
      inputmap->B1->SetBinContent(x, y, 0);
    }
  }
  for(unsigned int x=1; x<=B2->GetNbinsX(); x++){
    for(unsigned int y=1; y<=B2->GetNbinsY(); y++){
      float content = inputmap->B2->GetBinContent(x, y);
      B2->Fill(inputmap->B2->GetXaxis()->GetBinCenter(x), inputmap->B2->GetYaxis()->GetBinCenter(y), content*weightPixel);
      inputmap->B2->SetBinContent(x, y, 0);
    }
  }
  for(unsigned int x=1; x<=IBL->GetNbinsX(); x++){
    for(unsigned int y=1; y<=IBL->GetNbinsY(); y++){
      float content = inputmap->IBL->GetBinContent(x, y);
      IBL->Fill(inputmap->IBL->GetXaxis()->GetBinCenter(x), inputmap->IBL->GetYaxis()->GetBinCenter(y), content*weightIBL);
      inputmap->IBL->SetBinContent(x, y, 0);
    }
  }
  for(unsigned int x=1; x<=IBL2D->GetNbinsX(); x++){
    for(unsigned int y=1; y<=IBL2D->GetNbinsY(); y++){
      float content = inputmap->IBL2D->GetBinContent(x, y);
      IBL2D->Fill(inputmap->IBL2D->GetXaxis()->GetBinCenter(x), inputmap->IBL2D->GetYaxis()->GetBinCenter(y), content*weightIBL);
      inputmap->IBL2D->SetBinContent(x, y, 0);
    }
  }
  for(unsigned int x=1; x<=IBL3D->GetNbinsX(); x++){
    for(unsigned int y=1; y<=IBL3D->GetNbinsY(); y++){
      float content = inputmap->IBL3D->GetBinContent(x, y);
      IBL3D->Fill(inputmap->IBL3D->GetXaxis()->GetBinCenter(x), inputmap->IBL3D->GetYaxis()->GetBinCenter(y), content*weightIBL);
      inputmap->IBL3D->SetBinContent(x, y, 0);
    }
  }
}


StatusCode PixelMon2DProfilesLW::regHist(ManagedMonitorToolBase::MonGroup &group)
{
  if (mDoIBL && !mDoErrorHist) {
    sc = group.regHist(IBL);
    sc = group.regHist(IBL2D);
    sc = group.regHist(IBL3D);
  }
  sc = group.regHist(B0);
  sc = group.regHist(B1);
  sc = group.regHist(B2);
  sc = group.regHist(A);
  sc = group.regHist(C);
  //sc = group.regHist(DBMA);
  //sc = group.regHist(DBMC);
  
   return sc;
}
