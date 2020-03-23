/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "SimpleMMClusterBuilderTool.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonIdHelpers/MmIdHelper.h"

using namespace Muon;

Muon::SimpleMMClusterBuilderTool::SimpleMMClusterBuilderTool(const std::string& t,
							     const std::string& n,
							     const IInterface*  p )
  :  
  AthAlgTool(t,n,p)
{
  declareInterface<IMMClusterBuilderTool>(this);

}

Muon::SimpleMMClusterBuilderTool::~SimpleMMClusterBuilderTool()
{

}


StatusCode Muon::SimpleMMClusterBuilderTool::initialize()
{
  ATH_CHECK( m_idHelperSvc.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode Muon::SimpleMMClusterBuilderTool::finalize()
{

  return StatusCode::SUCCESS;
}

StatusCode Muon::SimpleMMClusterBuilderTool::getClusters(std::vector<Muon::MMPrepData>& MMprds, 
							 std::vector<Muon::MMPrepData*>& clustersVect) const 

{
  ATH_MSG_DEBUG("Size of the input vector: " << MMprds.size()); 
  ATH_MSG_DEBUG("Size of the output vector: " << clustersVect.size()); 
  std::vector<int> MMflag;
  IdentifierHash hash;

  if ( MMprds.size() > 0 ) {
    hash = MMprds.at(0).collectionHash();
  }
  else {
    ATH_MSG_DEBUG("Empty PRD collection: no clusterization" );
    return StatusCode::SUCCESS;
  }

  for (unsigned int i=0; i<MMprds.size(); ++i){
    MMflag.push_back(0);
  }

  for (unsigned int i=0; i<MMprds.size(); ++i){
    // skip the merged prds
    if(MMflag[i]==1) continue;
    
    unsigned int jmerge = -1;
    Identifier id_prd = MMprds[i].identify();
    int strip = m_idHelperSvc->mmIdHelper().channel(id_prd);
    int gasGap  = m_idHelperSvc->mmIdHelper().gasGap(id_prd);
    int layer   = m_idHelperSvc->mmIdHelper().multilayer(id_prd);
    ATH_MSG_VERBOSE("  MMprds " <<  MMprds.size() <<" index "<< i << " strip " << strip 
		    << " gasGap " << gasGap << " layer " << layer << " z " << MMprds[i].globalPosition().z() );
    for (unsigned int j=i+1; j<MMprds.size(); ++j){
      Identifier id_prdN = MMprds[j].identify();
      int stripN = m_idHelperSvc->mmIdHelper().channel(id_prdN);
      int gasGapN  = m_idHelperSvc->mmIdHelper().gasGap(id_prdN);
      int layerN   = m_idHelperSvc->mmIdHelper().multilayer(id_prdN);
      if( gasGapN==gasGap && layerN==layer ) {
	ATH_MSG_VERBOSE(" next MMprds strip same gasGap and layer index " << j << " strip " << stripN << " gasGap " << gasGapN << " layer " << layerN );
	if(abs(strip-stripN)<2) {
	  jmerge = j;
	  break;
	}
      }
    }
 
    unsigned int nmerge = 0;
    std::vector<Identifier> rdoList;
    std::vector<unsigned int> mergeIndices;
    std::vector<uint16_t> mergeStrips;
    std::vector<short int> mergeStripsTime;
    std::vector<int> mergeStripsCharge;

    rdoList.push_back(id_prd);
    MMflag[i] = 1;
    mergeIndices.push_back(i);
    mergeStrips.push_back(strip);
    mergeStripsTime.push_back(MMprds[i].time()-MMprds[i].globalPosition().norm()/299.792);
    mergeStripsCharge.push_back(MMprds[i].charge());

    unsigned int nmergeStrips = 1;
    unsigned int nmergeStripsMax = 50;
    for (unsigned int k=0; k < nmergeStripsMax; ++k) {
      for (unsigned int j=jmerge; j<MMprds.size(); ++j){
	if(MMflag[j] == 1) continue;
	Identifier id_prdN = MMprds[j].identify();
	int stripN = m_idHelperSvc->mmIdHelper().channel(id_prdN);
	if( abs(mergeStrips[k]-stripN) <= 1 ) {
	  int gasGapN  = m_idHelperSvc->mmIdHelper().gasGap(id_prdN);
	  int layerN   = m_idHelperSvc->mmIdHelper().multilayer(id_prdN);
	  if( gasGapN==gasGap && layerN==layer ) {
	    if(mergeStrips[k]==stripN) {
	      MMflag[j] = 1;
	      continue;
	    }
	    nmerge++;
	    rdoList.push_back(id_prdN);
	    MMflag[j] = 1;
	    mergeIndices.push_back(j);
	    mergeStrips.push_back(stripN);
      mergeStripsTime.push_back(MMprds[j].time()-MMprds[j].globalPosition().norm()/299.792);
      mergeStripsCharge.push_back(MMprds[j].charge());
	    nmergeStrips++;
	  }
	}
      }
      if(k>=nmergeStrips) break;
    }
    ATH_MSG_VERBOSE(" add merged MMprds nmerge " << nmerge << " strip " << strip << " gasGap " << gasGap << " layer " << layer );

    // start off from strip in the middle
    int stripSum = 0;
    for (unsigned int k =0; k<mergeStrips.size(); ++k) {
      stripSum += mergeStrips[k];
    }
    stripSum = stripSum/mergeStrips.size();
    
    unsigned int j = jmerge;
    for (unsigned int k =0; k<mergeStrips.size(); ++k) {
      if(mergeStrips[k]==stripSum) j = mergeIndices[k];
      ATH_MSG_VERBOSE(" merged strip nr " << k <<  " strip " << mergeStrips[k] << " index " << mergeIndices[k]);
    }
    ATH_MSG_VERBOSE(" Look for strip nr " << stripSum << " found at index " << j);

    ///
    /// get the local position from the cluster centroid
    ///
    double weightedPosX = 0.0;
    double posY = 0.0;
    double totalCharge = 0.0;
    if ( mergeStrips.size() > 0 ) { 
      /// get the Y local position from the first strip ( it's the same for all strips in the cluster)
      posY = MMprds[mergeIndices[0]].localPosition().y();
      for ( unsigned int k=0 ; k<mergeStrips.size() ; ++k ) {
	double posX = MMprds[mergeIndices[k]].localPosition().x();
	double charge = MMprds[mergeIndices[k]].charge();
	weightedPosX += posX*charge;
	totalCharge += charge;
	ATH_MSG_VERBOSE("Adding a strip to the centroid calculation: charge=" << charge);
      } 
      weightedPosX = weightedPosX/totalCharge;
    }

    
    Amg::Vector2D clusterLocalPosition(weightedPosX,posY);
    
    double covX = MMprds[j].localCovariance()(Trk::locX, Trk::locX);

    ///
    /// memory allocated dynamically for the PrepRawData is managed by Event Store
    ///
    Amg::MatrixX* covN = new Amg::MatrixX(1,1);
    covN->setIdentity();
    (*covN)(0,0) = 6.*(nmerge + 1.)*covX;
    if(nmerge<=1) (*covN)(0,0) = covX;
    ATH_MSG_VERBOSE(" make merged prepData at strip " << m_idHelperSvc->mmIdHelper().channel(MMprds[j].identify()) << " nmerge " << nmerge << " sqrt covX " << sqrt((*covN)(0,0)));
    
    ///
    /// memory allocated dynamically for the PrepRawData is managed by Event Store
    ///
    MMPrepData* prdN = new MMPrepData(MMprds[j].identify(), hash, clusterLocalPosition, 
				      rdoList, covN, MMprds[j].detectorElement(),
                                      (short int)0,int(totalCharge),(float)0.0,
				      mergeStrips,mergeStripsTime,mergeStripsCharge);
    clustersVect.push_back(prdN);
  } // end loop MMprds[i]
  //clear vector and delete elements
  MMflag.clear();
  MMprds.clear();
 

  return StatusCode::SUCCESS;
}
