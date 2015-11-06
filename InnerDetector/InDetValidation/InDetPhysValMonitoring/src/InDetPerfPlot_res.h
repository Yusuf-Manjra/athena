/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_res
#define INDETPHYSVALMONITORING_InDetPerfPlot_res
/**
 * @file InDetPerfPlot_res.h
 * @author shaun roe
**/


//std includes
#include <string>
#include <vector>
#include "TProfile.h"

//New stuff I've added in an attempt to get things to work
#include "TH1D.h"
#include "TH2D.h"
//local includes

#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TF1.h"

#include "InDetPlotBase.h"

//could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

//fwd declaration
class IToolSvc;
class IExtrapolator;

///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_res:public InDetPlotBase {
public:
  enum Param{D0, Z0, PHI, THETA, Z0SIN_THETA, QOVERP, NPARAMS};
  InDetPerfPlot_res(InDetPlotBase * pParent, const std::string & dirName);

  void fill(const xAOD::TrackParticle& trkprt, const xAOD::TruthParticle& truthprt);
  virtual ~InDetPerfPlot_res(){/** nop **/}

  void SetEtaBinning(int trackEtaBins, float etaMin, float etaMax) { m_trackEtaBins = trackEtaBins; m_etaMin = etaMin; m_etaMax = etaMax; };
	
private:
  unsigned int m_trackEtaBins;
  float m_etaMin, m_etaMax;
  float m_truthEtaCut;
  int nBins, m_yBins;
  unsigned int m_PtBins;
  float m_PtMin, m_PtMax;
  double m_Pt_logmin, m_Pt_logmax;
  std::string log_mode;

  std::vector<TH2*> m_meanbasePlots;
  std::vector<TProfile*> m_meanPlots;

  std::vector<TH2*> m_mean_vs_ptbasePlots;
  std::vector<TProfile*> m_mean_vs_ptPlots;

  std::vector<TProfile*> m_resoPlots;
  std::vector<TProfile*> m_resptPlots;

  std::vector<TH1*> m_pullPlots;
  std::vector<TH2*> m_pullbasePlots;
  //std::vector<TProfile*> m_pullbasePlots;
  std::vector<TProfile*> m_pullwidthPlots;

  std::vector<std::pair<std::string, float> > m_paramNames;

  void initializePlots();
  void finalizePlots();

  std::string formName(const unsigned int p, std::string type) const;
  std::string formTitle(const unsigned int parameter, std::string type) const;
};


#endif
