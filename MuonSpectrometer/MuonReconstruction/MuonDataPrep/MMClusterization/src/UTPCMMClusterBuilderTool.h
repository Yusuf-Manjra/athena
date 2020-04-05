/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef UTPCMMClusterBuilderTool_h
#define UTPCMMClusterBuilderTool_h

#include "GaudiKernel/ServiceHandle.h"
#include "MMClusterization/IMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <numeric>
#include <cmath>

#include "TH2D.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TLinearFitter.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TMatrixDSym.h"

// 
// Simple clusterization tool for MicroMegas
//
namespace Muon
{
  
  class UTPCMMClusterBuilderTool : virtual public IMMClusterBuilderTool, public AthAlgTool {

  public:
    /** Default constructor */
    UTPCMMClusterBuilderTool(const std::string&, const std::string&, const IInterface*);
     
    /** Default destructor */
    virtual ~UTPCMMClusterBuilderTool() = default;

    /** standard initialize method */
    virtual StatusCode initialize() override;

    virtual
    StatusCode getClusters(std::vector<Muon::MMPrepData>& MMprds, 
			   std::vector<Muon::MMPrepData*>& clustersVec) const override;

  private:

    /// Muon Detector Descriptor
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    // params for the hough trafo
    double m_alphaMin,m_alphaMax,m_alphaResolution,m_selectionCut;
    double m_dMin,m_dMax,m_dResolution,m_driftRange;
    int m_houghMinCounts;

    double m_timeOffset,m_dHalf,m_vDrift,m_transDiff,m_longDiff, m_ionUncertainty, m_scaleXerror, m_scaleYerror;
    double m_outerChargeRatioCut;
    int m_maxStripsCut;

    bool m_digiHasNegativeAngles;
    float m_scaleClusterError;

    StatusCode runHoughTrafo(std::vector<int>& flag,std::vector<double>& xpos, std::vector<double>& time,std::vector<int>& idx_selected)const;
    StatusCode fillHoughTrafo(std::unique_ptr<TH2D>& cummulator,std::vector<int>& flag, std::vector<double>& xpos, std::vector<double>& time)const;
    StatusCode houghInitCummulator(std::unique_ptr<TH2D>& cummulator,double xmax,double xmin)const;

    StatusCode findAlphaMax(std::unique_ptr<TH2D>& h_hough, std::vector<std::tuple<double,double>> &maxPos)const;
    StatusCode selectTrack(std::vector<std::tuple<double,double>> &tracks,std::vector<double>& xpos, std::vector<double>& time,std::vector<int>& flag,std::vector<int>& idx_selected)const;

    StatusCode transformParameters(double alpha, double d, double dRMS, double& slope,double& intercept, double& interceptRMS)const;
    StatusCode applyCrossTalkCut(std::vector<int> &idxSelected,const std::vector<MMPrepData> &MMPrdsOfLayer,std::vector<int> &flag,int &nStripsCut)const;
    StatusCode finalFit(const std::vector<Muon::MMPrepData> &mmPrd, std::vector<double>& time, std::vector<int>& idxSelected,double& x0, double &sigmaX0, double &fitAngle, double &chiSqProb)const;
};


}
#endif
