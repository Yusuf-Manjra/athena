/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ProjectionMMClusterBuilderTool_h
#define ProjectionMMClusterBuilderTool_h

#include "MMClusterization/IMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"


#include <numeric>


class MmIdHelper;
namespace MuonGM
{
  class MuonDetectorManager;
}

//
// fixed angle projection cluster builder tool for MicroMegas
//
namespace Muon
{
  
  class ProjectionMMClusterBuilderTool : virtual public IMMClusterBuilderTool, public AthAlgTool {

  public:
    /** Default constructor */
    ProjectionMMClusterBuilderTool(const std::string&, const std::string&, const IInterface*);
     
    /** Default destructor */
    virtual ~ProjectionMMClusterBuilderTool()=default;

    /** standard initialize method */
    virtual StatusCode initialize() override;
    
    /**Interface fuction to IMMClusterBuilderTool; calling function manages the pointers inside clustersVec  */
    virtual
    StatusCode getClusters(std::vector<Muon::MMPrepData>& MMprds, std::vector<Muon::MMPrepData*>& clustersVec) override;

  private: 

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    double m_vDrift,m_tmin,m_tmax,m_tOffset;
    double m_p0,m_p1,m_p2; //correction factors for charge dependence

    int m_t0;

    uint m_minClusterSize;


    StatusCode calculateCorrection(const std::vector<Muon::MMPrepData> &prdsOfLayer,std::vector<double>& v_posxc,std::vector<double>& v_cor);
    StatusCode doFineScan(std::vector<int>& flag,const std::vector<double>& v_posxc, const std::vector<double>& v_cor, std::vector<int>& idx_selected);   
    StatusCode doPositionCalculation(std::vector<double>& v_posxc, const std::vector<double>& v_cor, const std::vector<int> idx_selected,double& xmean, double& xmeanErr, double &  qtot,const std::vector<Muon::MMPrepData>& prdsOfLayer);
    
    StatusCode writeNewPrd(std::vector<Muon::MMPrepData*>& clustersVect,double xmean, double xerr,double qtot,const std::vector<int>& idx_selected,const std::vector<Muon::MMPrepData>& prdsOfLayer);
    
    double getSigma(double correction);

};


}
#endif
