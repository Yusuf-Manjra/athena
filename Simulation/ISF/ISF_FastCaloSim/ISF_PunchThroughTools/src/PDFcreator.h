/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PDFcreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PUNCHTHROUGHTOOLS_SRC_PDFCREATOR_H
#define ISF_PUNCHTHROUGHTOOLS_SRC_PDFCREATOR_H

// Athena Base
#include "AthenaKernel/IAtRndmGenSvc.h"

// ROOT includes
#include "TFile.h"
#include "TF1.h"
#include "TObject.h"
#include "TH2F.h"


namespace ISF
{
  /** @class PDFcreator

      Creates random numbers with a distribution given as ROOT TF1.
      The TF1 function parameters will be retrieved from a histogram given by addPar.

      @author  Elmar Ritsch <Elmar.Ritsch@cern.ch>
  */

  class PDFcreator
  {

  public:
    /** construct the class with a given TF1 and a random engine */
    PDFcreator(CLHEP::HepRandomEngine *engine):m_randomEngine(engine) {} ;

    ~PDFcreator() { };

    /** all following is used to set up the class */
    void setName( std::string PDFname ){ m_name = PDFname; }; //get the pdf's name
    void addToEnergyEtaRangeHist1DMap(double energy, std::vector<double> etaMinEtaMax, TH1 *hist) { std::map<std::vector<double>, TH1*> inner; inner.insert(std::make_pair(etaMinEtaMax, hist)); m_energy_etaRange_hists1D.insert(std::make_pair(energy, inner)); }; //add entry to map linking energy, eta window and histogram
    void addToEnergyEtaRangeHist2DMap(double energy, std::vector<double> etaMinEtaMax, TH2 *hist) { std::map<std::vector<double>, TH2*> inner; inner.insert(std::make_pair(etaMinEtaMax, hist)); m_energy_etaRange_hists2D.insert(std::make_pair(energy, inner)); }; //add entry to map linking energy, eta window and histogram

    /** get the random value with this methode, by providing the input parameters */
    double getRand( std::vector<double> inputPar, double outEnergy = 0., double randMin = 0., double randMax = 0.);
    std::string getName(){return m_name;};
    static bool compareEnergy1D(std::pair< double , std::map< std::vector<double>, TH1*> > map, double value){ return map.first < value; };
    static bool compareEnergy2D(std::pair< double , std::map< std::vector<double>, TH2*> > map, double value){ return map.first < value; };
    static bool compareEtaMax1D(std::pair< std::vector<double>, TH1*> map, double value){ return map.first.at(1) < value; };
    static bool compareEtaMax2D(std::pair< std::vector<double>, TH2*> map, double value){ return map.first.at(1) < value; };

  private:
    CLHEP::HepRandomEngine             *m_randomEngine;       //!< Random Engine
    std::string                         m_name;               //!< Give pdf a name for debug purposes 
    std::map< double , std::map< std::vector<double>, TH1*> > m_energy_etaRange_hists1D; //!< map of energies to map of eta ranges to 1D histograms
    std::map< double , std::map< std::vector<double>, TH2*> > m_energy_etaRange_hists2D; //!< map of energies to map of eta ranges to 2D histograms
    std::map< double , std::map< std::vector<double>, TObject*> > m_energy_etaRange_tobject; //!< map of energies to map of eta ranges to histograms


  };
}

#endif
