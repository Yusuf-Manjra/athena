/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TruthPixelClusterSplitter.cxx
/// Package : SiClusterizationTool 
/// Author  : Roland Jansky & Felix Cormier
/// Created : April 2016
///
/// DESCRIPTION: Split cluster given the truth information
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SiClusterizationTool/TruthPixelClusterSplitter.h"
#include "VxVertex/RecVertex.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "SiClusterizationTool/TruthClusterizationFactory.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

InDet::TruthPixelClusterSplitter::TruthPixelClusterSplitter(const std::string &type,
        const std::string &name,
        const IInterface *parent) :
        AthAlgTool(type,name,parent)
 {
   declareInterface<IPixelClusterSplitter>(this);
}

StatusCode InDet::TruthPixelClusterSplitter::initialize() {
    
  if (m_truthClusterizationFactory.retrieve().isFailure())
  {
    ATH_MSG_ERROR(" Unable to retrieve "<< m_truthClusterizationFactory );
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO(" Cluster splitter initialized successfully "<< m_truthClusterizationFactory );
  return StatusCode::SUCCESS;
}

StatusCode InDet::TruthPixelClusterSplitter::finalize() {
    return StatusCode::SUCCESS;
}

/* default method which simply splits cluster into 2 */
std::vector<InDet::PixelClusterParts> InDet::TruthPixelClusterSplitter::splitCluster(const InDet::PixelCluster& origCluster ) const
{

  //add treatment for b-layer only HERE

  const std::vector<Identifier>& rdos  = origCluster.rdoList();  
  const std::vector<int>&  totList     = origCluster.totList();

  //fill lvl1group all with the same value... (not best way but ...)
  std::vector<int> lvl1group;
  lvl1group.reserve(rdos.size());
  std::vector<Identifier>::const_iterator rdoBegin=rdos.begin();
  std::vector<Identifier>::const_iterator rdoEnd=rdos.end();
  for (std::vector<Identifier>::const_iterator rdoIter=rdoBegin;rdoIter!=rdoEnd;rdoIter++)
  {
    lvl1group.push_back(origCluster.LVL1A());
  }
  

  std::vector<Amg::Vector2D>     allLocalPositions;
  std::vector<Amg::MatrixX>       allErrorMatrix;

  
  std::vector<Amg::MatrixX> errorMatrix;

  std::vector<Amg::Vector2D> localPosition=m_truthClusterizationFactory->estimatePositions(origCluster);



  if (errorMatrix.size()!=2 || localPosition.size()!=2)
  {
    ATH_MSG_WARNING("Error matrix or local position vector size is not 2, it is:" << errorMatrix.size() << " or " << localPosition.size() << ".");
  }

  std::vector<InDet::PixelClusterParts> allMultiPClusters;
  
  
  allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[0],errorMatrix[0]);
  allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[1],errorMatrix[1]);
 
  
  return allMultiPClusters;

}

std::vector<InDet::PixelClusterParts> InDet::TruthPixelClusterSplitter::splitCluster(const InDet::PixelCluster& origCluster, 
                                                                                  const InDet::PixelClusterSplitProb& splitProb) const
{


  if (m_splitOnlyOnBLayer)
  {
    const InDetDD::SiDetectorElement* element=origCluster.detectorElement();
    if (element==nullptr) {
      ATH_MSG_WARNING("Could not get detector element");
      return std::vector<InDet::PixelClusterParts>();
    }
    const AtlasDetectorID* aid = element->getIdHelper();
    if (aid==nullptr)
    {
      ATH_MSG_WARNING("Could not get ATLASDetectorID");
      return std::vector<InDet::PixelClusterParts>();
    }
    const PixelID* pixelIDp=dynamic_cast<const PixelID*>(aid);
    if (!pixelIDp)
    {
      ATH_MSG_WARNING("Could not get PixelID pointer");
      return std::vector<InDet::PixelClusterParts>();
    } 
    //check if original pixel is on b-layer and if yes continue, otherwise interrupt...
    Identifier pixelId = origCluster.identify();
    if (!pixelIDp->is_blayer(pixelId))
    {
      //return empty object...
      ATH_MSG_VERBOSE(" Cluster not on b-layer. Return empty object-->back to default clustering." );
      
      return std::vector<InDet::PixelClusterParts>();
    }
  }

  //add treatment for b-layer only HERE

  const std::vector<Identifier>& rdos  = origCluster.rdoList();  
  const std::vector<int>&  totList     = origCluster.totList();

  //fill lvl1group all with the same value... (not best way but ...)
  std::vector<int> lvl1group;
  lvl1group.reserve(rdos.size());
  std::vector<Identifier>::const_iterator rdoBegin=rdos.begin();
  std::vector<Identifier>::const_iterator rdoEnd=rdos.end();
  for (std::vector<Identifier>::const_iterator rdoIter=rdoBegin;rdoIter!=rdoEnd;rdoIter++)
  {
    lvl1group.push_back(origCluster.LVL1A());
  }
  

  if (splitProb.getHighestSplitMultiplicityStored()<3) return std::vector<InDet::PixelClusterParts>();

  double splitProb2=splitProb.splitProbability(2);
  double splitProb3rel=splitProb.splitProbability(3);

  double splitProb3=splitProb3rel/(splitProb3rel+splitProb2);

  ATH_MSG_VERBOSE( " SplitProb -->2 " << splitProb2 << " SplitProb -->3 " << splitProb3 );

  int nParticles=1;
  
  if (splitProb2>m_thresholdSplittingIntoTwoClusters)
  {
    if (splitProb3>m_thresholdSplittingIntoThreeClusters)
    {
      nParticles=3;
    }
    else
    {
      nParticles=2;
    }
  }


  ATH_MSG_VERBOSE( " Decided for n. particles: " << nParticles << "." );

  std::vector<Amg::Vector2D>      allLocalPositions;
  std::vector<Amg::MatrixX>       allErrorMatrix;
  
  std::vector<InDet::PixelClusterParts> allMultiPClusters;
  
  if (nParticles==1)
  {
    std::vector<Amg::MatrixX> errorMatrix;
    std::vector<Amg::Vector2D> localPosition=m_truthClusterizationFactory->estimatePositions(origCluster);

    if (errorMatrix.size()!=1 || localPosition.size()!=1)
    {
      ATH_MSG_ERROR("Error matrix or local position vector size is not 1, it is:" << errorMatrix.size() << " or " << localPosition.size() << ".");
    }
    
    allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[0],errorMatrix[0]);
  }
  else if (nParticles==2)
  {

    std::vector<Amg::MatrixX> errorMatrix;
    std::vector<Amg::Vector2D> localPosition=m_truthClusterizationFactory->estimatePositions(origCluster);
    
    if (errorMatrix.size()!=2 || localPosition.size()!=2)
    {
      ATH_MSG_ERROR("Error matrix or local position vector size is not 2, it is:" << errorMatrix.size() << " or " << localPosition.size() << ".");
    }
    
    allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[0],errorMatrix[0]);
    allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[1],errorMatrix[1]);
  }
  else if (nParticles==3)
  {

    std::vector<Amg::MatrixX> errorMatrix;
    std::vector<Amg::Vector2D> localPosition=m_truthClusterizationFactory->estimatePositions(origCluster);
    
    if (errorMatrix.size()!=3 || localPosition.size()!=3)
    {
      ATH_MSG_ERROR("Error matrix or local position vector size is not 2, it is:" << errorMatrix.size() << " or " << localPosition.size() << ".");
    }
    
    
    allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[0],errorMatrix[0]);
    allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[1],errorMatrix[1]);
    allMultiPClusters.emplace_back(rdos,totList,lvl1group,localPosition[2],errorMatrix[2]);
  }
  
  return allMultiPClusters;

}
