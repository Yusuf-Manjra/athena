/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_RadiationMapsMaker_H
#define G4UserActions_RadiationMapsMaker_H

#include <vector>
#include <string>
#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"

class TGraph;

namespace G4UA
{
  
  class RadiationMapsMaker: 
    public G4UserRunAction,
    public G4UserSteppingAction
    {
    
    public:

      struct Config
      {
	/// bin sizes and ranges match the requirements for the
	/// Radiation Estimate Web tool for the default values given here.
	/// They can be configured to other values/ranges for other purposes.

	std::string material = std::string("");

	int nBinsr = 120;
	int nBinsz = 240;
	
	double rMinZoom =    0.; // cm
	double rMinFull =    0.; // cm
	
	double rMaxZoom =  480.; // cm
	double rMaxFull = 1200.; // cm
	
	double zMinZoom =    0.; // cm
	double zMinFull =    0.; // cm
	
	double zMaxZoom =  960.; // cm
	double zMaxFull = 2400.; // cm

	int nBinsr3d   = 30;
	int nBinsz3d   = 60;
	int nBinsphi3d = 32;
	
	double phiMinZoom = -180.; // degrees
	double phiMaxZoom =  180.; // degrees

	// neutron spectra
	int nBinslogEn  =  90;   
	double logEMinn = -11.; // min log10(E_kin/MeV) 
	double logEMaxn =   7.; // max log10(E_kin/MeV) 

	// particle spectra for gamma,e^+/-,mu^+/-,pi^+/-,p,rest
	int nBinslogEo  = 45;   
	double logEMino = -2.; // min log10(E_kin/MeV) 
	double logEMaxo =  7.; // max log10(E_kin/MeV) 
      };


      /// @brief Simple struct for holding the radiation maps
      struct Report
      {
        /// vector of tid seen by thread in zoomed area
	std::vector<double> m_rz_tid;
        /// vector of ionizing energy density seen by thread in zoomed area
	std::vector<double> m_rz_eion;
        /// vector of 1 MeV neutron equivalent flux seen by thread in zoomed area
	std::vector<double> m_rz_niel;
        /// vector of >20 MeV hadron flux seen by thread in zoomed area
	std::vector<double> m_rz_h20;
        /// vector of >100 keV hadron flux seen by thread in zoomed area
	std::vector<double> m_rz_neut;
        /// vector of charged hadron flux seen by thread in zoomed area
	std::vector<double> m_rz_chad;

	///  next two vectors are used only in case maps are needed for a particular material instead of all

        /// vector to measure volume fraction of target material in zoomed area
	std::vector<double> m_rz_vol;
        /// vector to normalize the volume fraction in zoomed area
	std::vector<double> m_rz_norm;

        /// vector of tid seen by thread in full area
	std::vector<double> m_full_rz_tid;
        /// vector of ionizing energy density seen by thread in full area
	std::vector<double> m_full_rz_eion;
        /// vector of 1 MeV neutron equivalent flux seen by thread in full area
	std::vector<double> m_full_rz_niel;
        /// vector of >20 MeV hadron flux seen by thread in full area
	std::vector<double> m_full_rz_h20;
        /// vector of >100 keV hadron flux seen by thread in full area
	std::vector<double> m_full_rz_neut;
        /// vector of charged hadron flux seen by thread in full area
	std::vector<double> m_full_rz_chad;

	///  next two vectors are used only in case maps are needed for a particular material instead of all

        /// vector to measure volume fraction of target material in full area
	std::vector<double> m_full_rz_vol;
        /// vector to normalize the volume fraction in full area
	std::vector<double> m_full_rz_norm;

        /// vector of tid seen by thread in 3d
	std::vector<double> m_3d_tid;
        /// vector of ionizing energy density seen by thread in 3d
	std::vector<double> m_3d_eion;
        /// vector of 1 MeV neutron equivalent flux seen by thread in 3d
	std::vector<double> m_3d_niel;
        /// vector of >20 MeV hadron flux seen by thread in 3d
	std::vector<double> m_3d_h20;
        /// vector of >100 keV hadron flux seen by thread in 3d
	std::vector<double> m_3d_neut;
        /// vector of charged hadron flux seen by thread in 3d
	std::vector<double> m_3d_chad;

	///  next two vectors are used only in case maps are needed for a particular material instead of all

        /// vector to measure volume fraction of target material in 3d
	std::vector<double> m_3d_vol;
        /// vector to normalize the volume fraction in 3d
	std::vector<double> m_3d_norm;

	// particle spectra

	// neutrons
	
        /// vector of neutron spectra in log10(E/MeV) bins and the zoom 2d grid
	std::vector<double> m_rz_neut_spec;
        /// vector of neutron spectra in log10(E/MeV) bins and the full 2d grid
	std::vector<double> m_full_rz_neut_spec;

	// gamma
	
        /// vector of gamma spectra in log10(E/MeV) bins and the zoom 2d grid
	std::vector<double> m_rz_gamm_spec;
        /// vector of e^+/- spectra in log10(E/MeV) bins and the full 2d grid
	std::vector<double> m_full_rz_gamm_spec;

	// e^+/-
	
        /// vector of e^+/- spectra in log10(E/MeV) bins and the zoom 2d grid
	std::vector<double> m_rz_elec_spec;
        /// vector of e^+/- spectra in log10(E/MeV) bins and the full 2d grid
	std::vector<double> m_full_rz_elec_spec;

	// mu^+/-
	
        /// vector of mu^+/- spectra in log10(E/MeV) bins and the zoom 2d grid
	std::vector<double> m_rz_muon_spec;
        /// vector of mu^+/- spectra in log10(E/MeV) bins and the full 2d grid
	std::vector<double> m_full_rz_muon_spec;

	// pi^+/-
	
        /// vector of pi^+/- spectra in log10(E/MeV) bins and the zoom 2d grid
	std::vector<double> m_rz_pion_spec;
        /// vector of pi^+/- spectra in log10(E/MeV) bins and the full 2d grid
	std::vector<double> m_full_rz_pion_spec;

	// proton
	
        /// vector of proton spectra in log10(E/MeV) bins and the zoom 2d grid
	std::vector<double> m_rz_prot_spec;
        /// vector of proton spectra in log10(E/MeV) bins and the full 2d grid
	std::vector<double> m_full_rz_prot_spec;

	// rest
	
        /// vector of other particle spectra in log10(E/MeV) bins and the zoom 2d grid
	std::vector<double> m_rz_rest_spec;
        /// vector of e^+/- spectra in log10(E/MeV) bins and the full 2d grid
	std::vector<double> m_full_rz_rest_spec;
	
	void merge(const Report& maps);
      };

      RadiationMapsMaker(const Config& config);

      // initialize maps to 0
      virtual void BeginOfRunAction(const G4Run*) override final;

      // increment radiation maps
      virtual void UserSteppingAction(const G4Step*) override final;

      /// Retrieve my maps
      const Report& getReport() const
      { return m_maps; }

    private:

      struct Config m_config;

      Report m_maps;

      TGraph * m_tgpSiA = 0;
      TGraph * m_tgpSiB = 0;
      TGraph * m_tgnSiA = 0;
      TGraph * m_tgnSiB = 0;
      TGraph * m_tgpiSi = 0;
      TGraph * m_tgeSi  = 0;
      
    }; // class RadiationMapsMaker
  
  
} // namespace G4UA 

#endif
