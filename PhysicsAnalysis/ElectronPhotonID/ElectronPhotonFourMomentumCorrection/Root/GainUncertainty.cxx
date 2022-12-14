/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <ElectronPhotonFourMomentumCorrection/GainUncertainty.h>
#include <TH1.h>
#include <TFile.h>

namespace egGain {

//--------------------------------------

  GainUncertainty::GainUncertainty(const std::string& filename) : asg::AsgMessaging("GainUncertainty") {

    ATH_MSG_INFO("opening file " << filename);
    std::unique_ptr<TFile> gainFile(TFile::Open( filename.c_str(), "READ"));

    if (not (m_alpha_specialGainRun = (TH1*)(gainFile->Get("alpha_specialGainRun")))) ATH_MSG_FATAL("cannot open alpha_specialGainRun");
    m_alpha_specialGainRun->SetDirectory(nullptr);
    if (not (m_gain_impact_Zee = (TH1*)(gainFile->Get("gain_impact_Zee")))) ATH_MSG_FATAL("cannot open gain_impact_Zee");
    m_gain_impact_Zee->SetDirectory(nullptr);
    for (int i=0;i<s_nEtaBins;i++) {
     char name[60];
     sprintf(name,"gain_Impact_elec_%d",i);
     if (not (m_gain_Impact_elec[i] = (TH1*)(gainFile->Get(name)))) ATH_MSG_FATAL("cannot open " << name);
     m_gain_Impact_elec[i]->SetDirectory(nullptr);
     sprintf(name,"gain_Impact_conv_%d",i);
     if (not (m_gain_Impact_conv[i] = (TH1*)(gainFile->Get(name)))) ATH_MSG_FATAL("cannot open " << name);
     m_gain_Impact_conv[i]->SetDirectory(nullptr);
     sprintf(name,"gain_Impact_unco_%d",i);
     if (not (m_gain_Impact_unco[i] = (TH1*)(gainFile->Get(name)))) ATH_MSG_FATAL("cannot open " << name);
     m_gain_Impact_unco[i]->SetDirectory(nullptr);
    }

  }

//----------------------------------------------

  GainUncertainty::~GainUncertainty() {
      delete m_alpha_specialGainRun;
      delete m_gain_impact_Zee;
      for (int i=0;i<s_nEtaBins;i++) {
        delete m_gain_Impact_elec[i];
        delete m_gain_Impact_conv[i];
        delete m_gain_Impact_unco[i];
      }
  }

//----------------------------------------------

    // returns relative uncertainty on energy

  double GainUncertainty::getUncertainty(double etaCalo_input, double et_input,  PATCore::ParticleType::Type ptype) const {
       double aeta=std::fabs(etaCalo_input);
       int ibin=-1;
       if (aeta<0.8) ibin=0;
       else if (aeta<1.37) ibin=1;
       else if (aeta<1.52) ibin=2;
       else if (aeta<1.80) ibin=3;
       else if (aeta<2.50) ibin=4;
       if (ibin<0) return 0.;

       //Protection needed as the histograms stops at 1 TeV
       if(et_input>999999.) et_input = 999999.;

//       std::cout << " --- in  GainUncertainty::getUncertainty " << etaCalo_input << " " << et_input << " " << ptype << " ibin " << ibin << std::endl;

       double impact=0.;
       if (ptype==PATCore::ParticleType::Electron) impact = m_gain_Impact_elec[ibin]->GetBinContent(m_gain_Impact_elec[ibin]->FindFixBin(0.001*et_input));
       if (ptype==PATCore::ParticleType::ConvertedPhoton) impact = m_gain_Impact_conv[ibin]->GetBinContent(m_gain_Impact_conv[ibin]->FindFixBin(0.001*et_input));
       if (ptype==PATCore::ParticleType::UnconvertedPhoton) impact = m_gain_Impact_unco[ibin]->GetBinContent(m_gain_Impact_unco[ibin]->FindFixBin(0.001*et_input));

//       std::cout << " impact " << impact << std::endl;
       double_t sigmaE = m_alpha_specialGainRun->GetBinContent(m_alpha_specialGainRun->FindFixBin(aeta))
                        /m_gain_impact_Zee->GetBinContent(m_gain_impact_Zee->FindFixBin(aeta))
                        *impact ;

//       std::cout << " m_alpha_specialGainRun, m_gain_impact_Zee, impact , sigmaE " << m_alpha_specialGainRun->GetBinContent(m_alpha_specialGainRun->FindBin(aeta))
//  << " " << m_gain_impact_Zee->GetBinContent(m_gain_impact_Zee->FindBin(aeta)) << " " << impact << " " << sigmaE << std::endl;

       return sigmaE;
  }


}
