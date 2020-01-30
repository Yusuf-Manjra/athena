/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Lhood_MM_tools_h
#define Lhood_MM_tools_h

#include "Rtypes.h"
#include "TMatrixT.h"
#include "FakeBkgTools/BaseFakeBkgTool.h"
#include "FakeBkgTools/LhoodMMEvent.h"

#include <vector>

class TH1;
class TMinuit_LHMM;
class TDirectory;

namespace CP
{

class LhoodMM_tools : public BaseFakeBkgTool
#ifndef FAKEBKGTOOLS_ATLAS_ENVIRONMENT
  , public FakeBkgTools::ExtraPropertyManager<LhoodMM_tools, BaseFakeBkgTool>
#endif
{

  ASG_TOOL_CLASS(LhoodMM_tools, IFakeBkgTool)

 protected:

  virtual void reset();

  // implement base class methods
  virtual StatusCode addEventCustom() override;

 public:

  LhoodMM_tools(const std::string& name); 
  ~LhoodMM_tools();
  
  virtual StatusCode initialize() override;

  // implement base class methods

  virtual StatusCode getTotalYield(float& yield, float& statErrUp, float& statErrDown) override final;
  virtual StatusCode register1DHistogram(TH1* h1, const float *val) override;
  virtual StatusCode register2DHistogram(TH2* h2, const float *xval, const float *yval) override;

  double nfakes( const std::vector<LhoodMMEvent>& mmevts, Double_t *poserr, Double_t *negerr); 
 
  double nfakes_std(double *error);
  double nfakes_std_perEventWeight(double *error);

  StatusCode setFitType(std::string ft);

  Int_t getFitStatus() {return m_fitStatus;}
  
  void setPrintLevel(Int_t printLevel) { m_printLevel = printLevel;}

  void set_do_std_perEventWeight(bool val) {m_do_std_perEventWeight = val;}
  
  virtual StatusCode saveProgress(TDirectory* dir) override;

  StatusCode mergeSubJobs();

 protected:
  /// This indicates which type of efficiencies/fake factor need to be filled
  virtual FakeBkgTools::Client clientForDB() override final;

 private:

  // pointer to instance of LhoodMM_tools to use in a given fit
  static LhoodMM_tools* m_current_lhoodMM_tool;

 #ifndef __CLING__
  std::vector<std::unique_ptr<FakeBkgTools::FinalState> > m_fsvec;
 #else
  std::vector<int> m_fsvec;
 #endif
  int m_curr_nlep;
  int m_minnlep, m_maxnlep, m_maxnlep_loose;
  int m_theta_tot_start_index;
  std::vector<std::vector<int>> m_real_indices, m_fake_indices;
  bool m_requireSS, m_requireOS;

  bool m_doFakeFactor;

  bool m_fixNormalization;

  float m_maxWeight;

  bool m_alreadyMerged;

  static const int nLepMax = 6;
  static const int maxRank =  64; // i.e. 2^nLepMax
  std::vector<std::vector<double> > m_event_cat;
  std::vector<std::vector<double> > m_event_sumw2;

  std::vector < std::vector < std::vector <double> >> m_coeffs;

  std::vector<LhoodMMEvent> m_mmevts_total; // filled by addEventCustom()
  std::vector< std::vector<LhoodMMEvent> > m_mmevts_internal;  // separated out by lepton 
                                                    // multiplicity in setup()

  std::map<TH1*, std::vector<LhoodMMEvent> > m_mmevts_1dhisto_map;
  std::map<TH2*, std::vector<LhoodMMEvent> > m_mmevts_2dhisto_map;

  double m_ssfrac, m_dilep_SSfrac;
  std::vector< std::vector < double > > m_OSfrac;

  double m_nfakes_std, m_nfakes_std_err;

  double m_nfakes_std_perEventWeight, m_nfakes_std_err_perEventWeight;

  bool m_do_std_perEventWeight;

  Int_t m_fitStatus;

  Int_t m_printLevel;

  std::vector<std::shared_ptr<TMatrixT<double>> > m_nrf_mat_vec;
  std::vector<std::shared_ptr<TMatrixT<double>> > m_MMmatrix_vec;
  std::vector<std::shared_ptr<TMatrixT<double>>> m_ntlpred_vec;

  unsigned m_lastSaveIndex;

  //mathematical constants
  const double _piover2 = 1.57079632679489661923;
  const double _piover4 = 0.785398163397448309616;

  StatusCode setup(const std::vector<LhoodMMEvent>& mmevts);

  static double logPoisson(double obs, double pred);

  StatusCode addEventCustom(const std::vector<bool>& isTight_vals,
       const std::vector<FakeBkgTools::Efficiency>& realEff_vals,
       const std::vector<FakeBkgTools::Efficiency>& fakeEff_vals,
       const std::vector<int>& charges,
       float weight);

  static void fcn_nlep(Int_t &npar , Double_t *gin , Double_t &f, Double_t *par, Int_t iflag );
  static void fcn_minnlep_maxnlep(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);

  void get_init_pars(std::vector<double> &init_pars, int nlep);
  void get_analytic(std::vector<double>& nrf, const int nlep);

  double fixPosErr(double n_fake_fit, TMinuit_LHMM* lhoodFit);
  double fixNegErr(double n_fake_fit, TMinuit_LHMM* lhoodFit);
  void mapLhood(TMinuit_LHMM* lhoodFit, int nlep, double min, double max);

  StatusCode fillHistograms();
  StatusCode fillHisto_internal(const std::vector<LhoodMMEvent>& mmevts, TH1* h);
};

}

#endif
