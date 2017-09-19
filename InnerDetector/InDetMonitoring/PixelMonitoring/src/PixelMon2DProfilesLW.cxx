/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Function to handle 2D profile histograms of modules, one for each region
///////////////////////////////////////////////////////////////////////////////

#include "PixelMonitoring/Components.h"
#include "PixelMonitoring/PixelMon2DProfilesLW.h"
#include "PixelMonitoring/PixelMon2DMapsLW.h"
#include "InDetIdentifier/PixelID.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "GaudiKernel/StatusCode.h"     
#include <string.h>

PixelMon2DProfilesLW::PixelMon2DProfilesLW(std::string name, std::string title, const PixMon::HistConf& config, bool copy2DFEval)
    : IBL3D(nullptr),
      IBL2D(nullptr),
      IBL(nullptr),
      B0(nullptr),
      B1(nullptr),
      B2(nullptr),
      A(nullptr),
      C(nullptr),
      DBMA(nullptr),
      DBMC(nullptr),
      m_config(config),
      m_copy2DFEval(copy2DFEval)
{
   std::string setatext = ";shifted eta index of module";
   std::string etatext = ";eta index of module";
   std::string phitext = ";phi index of module";
   std::string disktext = ";disk number";
   std::string layertext = ";layer number";

   if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kIBL3D)) {
      IBL3D = TProfile2D_LW::create((name+"_IBL3D").c_str(), (title + ", IBL 3D modules " + etatext + phitext).c_str(),
                                    PixMon::kNumModulesIBL3D, -.5, -.5 + PixMon::kNumModulesIBL3D,
                                    PixMon::kNumStavesIBL, -0.5, -0.5 + PixMon::kNumStavesIBL);
   }
   if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kIBL2D) ){
      IBL2D = TProfile2D_LW::create((name+"_IBL2D").c_str(), (title + ", IBL planar modules " + setatext + phitext).c_str(),
                                    PixMon::kNumModulesIBL2D, -6.5, -6.5 + PixMon::kNumModulesIBL2D,
                                    PixMon::kNumStavesIBL, -0.5, -0.5 + PixMon::kNumStavesIBL);
   }
   if (m_doIBL && PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kIBL)) {
      IBL = TProfile2D_LW::create((name+"_IBL").c_str(), (title + ", IBL " + setatext + phitext).c_str(),
                                  PixMon::kNumModulesIBL, -16.5, -16.5 + PixMon::kNumModulesIBL,
                                  PixMon::kNumStavesIBL, -0.5, -0.5 + PixMon::kNumStavesIBL);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kB0)) {
      B0 = TProfile2D_LW::create((name+"_B0").c_str(), (title + ", B0 " + etatext + phitext).c_str(),
                                 PixMon::kNumModulesBarrel, -6.5, -6.5 + PixMon::kNumModulesBarrel,
                                 PixMon::kNumStavesL0, -0.5, -0.5 + PixMon::kNumStavesL0);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kB1)) {
      B1 = TProfile2D_LW::create((name+"_B1").c_str(), (title + ", B1 " + etatext + phitext).c_str(),
                                 PixMon::kNumModulesBarrel, -6.5, -6.5 + PixMon::kNumModulesBarrel,
                                 PixMon::kNumStavesL1, -0.5, -0.5 + PixMon::kNumStavesL1);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kB2)) {
      B2 = TProfile2D_LW::create((name+"_B2").c_str(), (title + ", B2 " + etatext + phitext).c_str(),
                                 PixMon::kNumModulesBarrel, -6.5, -6.5 + PixMon::kNumModulesBarrel,
                                 PixMon::kNumStavesL2, -0.5, -0.5 + PixMon::kNumStavesL2);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kECA)) {
      A = TProfile2D_LW::create((name+"_ECA" ).c_str(), (title + ", ECA " + disktext + phitext).c_str(),
                                PixMon::kNumLayersDisk, -0.5, -0.5 + PixMon::kNumLayersDisk,
                                PixMon::kNumModulesDisk, -0.5, -0.5 + PixMon::kNumModulesDisk);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kECC)) {
      C = TProfile2D_LW::create((name+"_ECC" ).c_str(), (title + ", ECC " + disktext + phitext).c_str(),
                                PixMon::kNumLayersDisk, -0.5, -0.5 + PixMon::kNumLayersDisk,
                                PixMon::kNumModulesDisk, -0.5, -0.5 + PixMon::kNumModulesDisk);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kDBMA)) {
      DBMA = TProfile2D_LW::create((name+"_DBMA").c_str(), (title + ", DBMA " + layertext + phitext).c_str(),
                                   PixMon::kNumLayersDBM, -0.5, -0.5 + PixMon::kNumLayersDBM,
                                   PixMon::kNumModulesDBM, -0.5, -0.5 + PixMon::kNumModulesDBM);
   }
   if (PixMon::HasComponent(m_config, PixMon::LayerIBL2D3DDBM::kDBMC)) {
      DBMC = TProfile2D_LW::create((name+"_DBMC").c_str(), (title + ", DBMC " + layertext + phitext).c_str(),
                                   PixMon::kNumLayersDBM, -0.5, -0.5 + PixMon::kNumLayersDBM,
                                   PixMon::kNumModulesDBM, -0.5, -0.5 + PixMon::kNumModulesDBM);
   }

   m_histograms = {IBL, IBL2D, IBL3D, B0, B1, B2, A, C, DBMA, DBMC};

   formatHist();
}

PixelMon2DProfilesLW::~PixelMon2DProfilesLW()
{
   for (auto& hist : m_histograms) {
      if (hist) LWHist::safeDelete(hist);
   }
}

void PixelMon2DProfilesLW::SetMaxValue(float max)
{
   for (auto& hist : m_histograms) {
      if (hist) hist->SetMaximum(max);
   }
}

void PixelMon2DProfilesLW::Reset()
{
   for (auto& hist : m_histograms) {
      if (hist) hist->Reset();
   }
}

void PixelMon2DProfilesLW::Fill(Identifier &id, const PixelID* pixID, float weight)
{
   const int bec = pixID->barrel_ec(id);
   const int pm  = pixID->phi_module(id);
   int ld = pixID->layer_disk(id);

   if (bec == 2 && A) A->Fill(ld, pm, weight);
   else if (bec == -2 && C) C->Fill(ld, pm, weight);
   else if (bec == 4 && DBMA) DBMA->Fill(ld, pm, weight);
   else if (bec == -4 && DBMC) DBMC->Fill(ld, pm, weight);
   else if (bec == 0) {
      if (m_doIBL) ld--;
      const int em = pixID->eta_module(id);
      if (ld == 0 && B0) {
         B0->Fill(em, pm, weight);
      } else if (ld == 1 && B1) {
         B1->Fill(em, pm, weight);
      } else if (ld == 2 && B2) {
         B2->Fill(em, pm, weight);
      } else if (ld == -1 && IBL) {
         int feid = 0;
         int emf = 0;
         bool copy = false;
         if (em < 6 && em > -7) {
            if (pixID->eta_index(id) >= 80) feid = 1;
            emf = 2 * em + feid; 
            if (IBL2D) IBL2D->Fill(em, pm, weight);
            copy = true;
         } else if (em < -6) {
            emf = em - 6;
            if (IBL3D) IBL3D->Fill(em + 10, pm, weight);
         } else {
            emf = em + 6;
            if (IBL3D) IBL3D->Fill(em - 2, pm, weight);
         }
         IBL->Fill(emf, pm, weight);
         if (m_copy2DFEval && copy) IBL->Fill(emf + 1, pm, weight);
      }
   }
}

void PixelMon2DProfilesLW::formatHist()
{
   if (A && C) {
      for (unsigned int i = 0; i < PixMon::kNumModulesDisk; ++i) {
         A->GetYaxis()->SetBinLabel(i + 1, PixMon::ModulesECA.at(i).c_str());
         C->GetYaxis()->SetBinLabel(i + 1, PixMon::ModulesECC.at(i).c_str());
      }
      for (unsigned int i = 0; i < PixMon::kNumLayersDisk; ++i) {
         A->GetXaxis()->SetBinLabel(i + 1, PixMon::LayersDisk.at(i).c_str());
         C->GetXaxis()->SetBinLabel(i + 1, PixMon::LayersDisk.at(i).c_str());
      }
   }
   if (DBMA && DBMC) {
      for (unsigned int i = 0; i < PixMon::kNumModulesDBM; ++i) {
         DBMA->GetYaxis()->SetBinLabel(i + 1, PixMon::ModulesDBM.at(i).c_str());
         DBMC->GetYaxis()->SetBinLabel(i + 1, PixMon::ModulesDBM.at(i).c_str());
      }
      for (unsigned int i = 0; i < PixMon::kNumLayersDBM; ++i) {
         DBMA->GetXaxis()->SetBinLabel(i + 1, PixMon::LayersDBM.at(i).c_str());
         DBMC->GetXaxis()->SetBinLabel(i + 1, PixMon::LayersDBM.at(i).c_str());
      }
   }
   if (B0 && B1 && B2) {
      for (unsigned int i = 0; i < PixMon::kNumModulesBarrel; ++i) {
         B0->GetXaxis()->SetBinLabel(i + 1, PixMon::ModulesBarrel.at(i).c_str());
         B1->GetXaxis()->SetBinLabel(i + 1, PixMon::ModulesBarrel.at(i).c_str());
         B2->GetXaxis()->SetBinLabel(i + 1, PixMon::ModulesBarrel.at(i).c_str());
      }
      for (unsigned int i = 0; i < PixMon::kNumStavesL0; ++i) {
         B0->GetYaxis()->SetBinLabel(i + 1, PixMon::StavesL0.at(i).c_str());
      }
      for (unsigned int i = 0; i < PixMon::kNumStavesL1; ++i) {
         B1->GetYaxis()->SetBinLabel(i + 1, PixMon::StavesL1.at(i).c_str());
      }
      for (unsigned int i = 0; i < PixMon::kNumStavesL2; ++i) {
         B2->GetYaxis()->SetBinLabel(i + 1, PixMon::StavesL2.at(i).c_str());
      }
   }
   if (IBL) {
      for (unsigned int i = 0; i < PixMon::kNumModulesIBL; ++i) {
         IBL->GetXaxis()->SetBinLabel(i + 1, PixMon::ModulesIBL.at(i).c_str());
      }
      for (unsigned int i = 0; i < PixMon::kNumStavesIBL; ++i) {
         IBL->GetYaxis()->SetBinLabel(i + 1, PixMon::StavesIBL.at(i).c_str());
      }
   }
   if (IBL2D && IBL3D) {
      for (unsigned int i = 0; i < PixMon::kNumModulesIBL2D; ++i) {
         IBL2D->GetXaxis()->SetBinLabel(i + 1, PixMon::ModulesIBL2D.at(i).c_str());
      }
      for (unsigned int i = 0; i < PixMon::kNumModulesIBL3D; ++i) {
         IBL3D->GetXaxis()->SetBinLabel(i + 1, PixMon::ModulesIBL3D.at(i).c_str());
      }
      for (unsigned int i = 0; i < PixMon::kNumStavesIBL; ++i) {
         IBL2D->GetYaxis()->SetBinLabel(i + 1, PixMon::StavesIBL.at(i).c_str());
         IBL3D->GetYaxis()->SetBinLabel(i + 1, PixMon::StavesIBL.at(i).c_str());
      }
   }

   for (auto& hist : m_histograms) {
      if (!hist) continue;
      if (hist == A || hist == C) {
         hist->GetYaxis()->SetLabelSize(0.02);
      } else {
         hist->GetYaxis()->SetLabelSize(0.03);
      }
      hist->SetOption("colz");
      hist->SetMinimum(0.);
   }
}

void PixelMon2DProfilesLW::Fill2DMon(PixelMon2DProfilesLW* oldmap)
{
   for (unsigned int index = 0; index < m_histograms.size(); ++index) {
      auto& hist = m_histograms.at(index);
      auto& oldhist = oldmap->m_histograms.at(index);
      if (!hist) continue;
      if (!oldhist) continue;
      for (unsigned int x = 1; x <= hist->GetNbinsX(); ++x) {
         for (unsigned int y = 1; y <= hist->GetNbinsY(); ++y) {
            const auto content = oldhist->GetBinContent(x, y);
            hist->SetBinContent(x, y, content);
            oldhist->SetBinContent(x, y, 0);
         }
      }
   }
}

void PixelMon2DProfilesLW::FillFromMap(PixelMon2DMapsLW* inputmap, bool clear_inputmap)
{
   const float weightIBL   = 1.0 / 26880.0;
   const float weightPixel = 1.0 / 46080.0;

   for (unsigned int index = 0; index < m_histograms.size(); ++index) {
      auto& hist = m_histograms.at(index);
      auto& map = inputmap->m_histograms.at(index);
      if (!hist) continue;
      if (!map) continue;
      for (unsigned int x = 1; x <= hist->GetNbinsX(); x++) {
         for (unsigned int y = 1; y <= hist->GetNbinsY(); y++) {
            auto content = map->GetBinContent(x, y);
            if (hist == IBL || hist == IBL3D || hist == DBMA || hist == DBMC) {
               content *= weightIBL;
            } else if (hist == IBL2D) {
               content *= weightIBL * 0.5;
            } else {
               content *= weightPixel;
            }
            hist->Fill(map->GetXaxis()->GetBinCenter(x), map->GetYaxis()->GetBinCenter(y), content);
         }
      }
      if (clear_inputmap) map->Reset();
   }
}

StatusCode PixelMon2DProfilesLW::regHist(ManagedMonitorToolBase::MonGroup &group)
{
   StatusCode sc = StatusCode::SUCCESS;

   for (auto& hist : m_histograms) {
      if (!hist) continue;
      if (group.regHist(hist).isFailure()) {
         sc = StatusCode::FAILURE;
      }
   }
  
   return sc;
}

const bool PixelMon2DProfilesLW::m_doIBL{true};
