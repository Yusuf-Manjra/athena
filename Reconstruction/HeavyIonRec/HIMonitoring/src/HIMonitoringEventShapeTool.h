/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef HIMONITORINGEVENTSHAPETOOL_H
#define HIMONITORINGEVENTSHAPETOOL_H



#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include <xAODHIEvent/HIEventShapeContainer.h>

#include "xAODTrigMinBias/TrigT2ZdcSignalsContainer.h"


class TH1D;
class TH1D_LW;
class TH2D_LW;
class TProfile_LW;

class HIMonitoringEventShapeTool: public ManagedMonitorToolBase
{
public:
  HIMonitoringEventShapeTool(const std::string& type, const std::string& name,
                             const IInterface* parent);

  virtual ~HIMonitoringEventShapeTool();

  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();


  bool m_ZDCmon;
  bool m_ESmon;

  void bookFCalEt_hist();
  void getFCalEt(const xAOD::HIEventShapeContainer* evtShape);
  void fillFCalEt_hist();

  void bookES_hist();
  void getES(const xAOD::HIEventShapeContainer* evtShape);
  void fillES_hist();


  void bookZDC_hist();
  void getZDC(const xAOD::TrigT2ZdcSignalsContainer* TrigZdc_p);
  void fillZDC_hist();

  double calc_qn(double qnx, double qny);
  double calc_psin(int n, double qnx, double qny);
  double calc_psin_diff(int n, double psi1, double psi2);
  int ES_eta_layer2bin(double eta);
  double ES_bin2eta_layer(int bin);
private:
  static const int s_num_of_harm = 7;
  static const int s_num_of_eta_bins = 100;


  const std::string m_side_id[2] = {
    "A", "C"
  };
  const std::string m_sqn_num[s_num_of_harm] = {
    "1", "2", "3", "4", "5", "6", "7"
  };

  double m_FCalEt = 0.0;
  double m_FCalEt_A = 0.0;
  double m_FCalEt_C = 0.0;
  double m_FCalEt_eta[s_num_of_eta_bins]{};

  double m_ZDC_HG = 0.0;
  double m_ZDC_LG = 0.0;

  double m_qnx[s_num_of_harm]{};
  double m_qny[s_num_of_harm]{};
  double m_qn[s_num_of_harm]{};

  double m_qnx_A[s_num_of_harm]{};
  double m_qny_A[s_num_of_harm]{};

  double m_qnx_C[s_num_of_harm]{};
  double m_qny_C[s_num_of_harm]{};

  double m_qnx_eta[s_num_of_harm][s_num_of_eta_bins]{};
  double m_qny_eta[s_num_of_harm][s_num_of_eta_bins]{};

  double m_psin_A[s_num_of_harm]{};
  double m_psin_C[s_num_of_harm]{};

  /// histograms

  TH1D* m_h_FCalEt {};
  TH1D* m_h_FCalEt_sides[2] {};
  TProfile_LW* m_h_FCalEt_vs_eta {};
  TH2D_LW* m_h_FCalEt_A_vs_C {};

  TProfile_LW* m_h_qnx_vs_FCalEt[s_num_of_harm] {};
  TProfile_LW* m_h_qny_vs_FCalEt[s_num_of_harm] {};
  TH2D_LW* m_h_qn_vs_FCalEt[s_num_of_harm] {};

  TProfile_LW* m_h_qnx_A_vs_FCalEt[s_num_of_harm] {};
  TProfile_LW* m_h_qny_A_vs_FCalEt[s_num_of_harm] {};
  TProfile_LW* m_h_qnx_C_vs_FCalEt[s_num_of_harm] {};
  TProfile_LW* m_h_qny_C_vs_FCalEt[s_num_of_harm] {};

  TProfile_LW* m_h_qnx_vs_eta[s_num_of_harm] {};
  TProfile_LW* m_h_qny_vs_eta[s_num_of_harm] {};

  TH2D_LW* m_h_psin_A_vs_FCalEt[s_num_of_harm] {};
  TH2D_LW* m_h_psin_C_vs_FCalEt[s_num_of_harm] {};
  TH2D_LW* m_h_psin_ACdiff_vs_FCalEt[s_num_of_harm] {};
  TProfile_LW* m_h_psin_R_vs_FCalEt[s_num_of_harm] {};

  TH2D_LW* m_h_FCalEt_vs_ZDC_HG {};
  TH2D_LW* m_h_FCalEt_vs_ZDC_LG {};

  /// histograms ranges and bining
  int m_FCalEt_nbins;
  double m_low_FCalEt;
  double m_high_FCalEt;

  int m_nbins_phi;

  int m_nbins_eta;
  double m_eta_range;

  double m_FCalEt_eta_hist_cut;
};

#endif
