/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MergedPixelsTool.cxx
// TheSiSimpleClusteringAlg adapted to implement cluster merging
// input from RDOs. 
//
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SiClusterizationTool/MergedPixelsTool.h"

#include "Identifier/IdentifierHash.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetPrepRawData/PixelClusterParts.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/PixelClusterParts.h"
#include "InDetPrepRawData/PixelClusterSplitProb.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitProbTool.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitter.h"
#include "InDetIdentifier/PixelID.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

using CLHEP::micrometer;


namespace InDet {
  
  // Constructor with parameters:
  MergedPixelsTool::MergedPixelsTool(const std::string &type,
                                     const std::string &name,
                                     const IInterface *parent) :
                                     PixelClusteringToolBase(type,name,parent),
                                     m_IBLParameterSvc("IBLParameterSvc",name)
    {
      declareInterface<IPixelClusteringTool>(this);
    }
  
  StatusCode  MergedPixelsTool::initialize(){
    
    if (m_IBLParameterSvc.retrieve().isFailure()) { 
      ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc"); 
    } else { 
      m_clusterSplitter.setTypeAndName(m_IBLParameterSvc->setStringParameters(m_clusterSplitter.typeAndName(),"clusterSplitter")); 
      m_splitProbTool.setTypeAndName(m_IBLParameterSvc->setStringParameters(m_splitProbTool.typeAndName(),"clusterSplitter")); 
      m_IBLParameterSvc->setBoolParameters(m_IBLAbsent,"IBLAbsent");
    }  
    
    // for the cluster splitting
    ATH_CHECK(m_splitProbTool.retrieve( EnableTool{not m_splitProbTool.empty()}));
    ATH_CHECK(m_clusterSplitter.retrieve( EnableTool{not m_clusterSplitter.empty()} ));

    ATH_CHECK(m_pixelDetEleCollKey.initialize());
    
    return PixelClusteringToolBase::initialize();
  }


  StatusCode MergedPixelsTool::finalize()
  {
   
    ATH_MSG_DEBUG("------------------- Clusterization Statistics ------------------------");
    ATH_MSG_DEBUG("-- # Processed Pixel Clusters     : " << m_processedClusters);
    if(m_processedClusters){
      ATH_MSG_DEBUG("-- # Clusters modified  (%)       : " << m_modifiedOrigClusters << " (" << double(m_modifiedOrigClusters)/double(m_processedClusters) << ")" );
      ATH_MSG_DEBUG("-- # Clusters split into more (%) : " << m_splitOrigClusters << " (" << double(m_splitOrigClusters)/double(m_processedClusters) << ")" );
      ATH_MSG_DEBUG("-- # Split Clusters created       : " << m_splitProdClusters);
      ATH_MSG_DEBUG("-- # Large Pixel Clusters (%)     : " << m_largeClusters << " (" << double(m_largeClusters)/double(m_processedClusters) << ")" );
    }
    return StatusCode::SUCCESS;
  }

  //-----------------------------------------------------------------------
  // Called by the PixelPrepRawDataFormation algorithm once for every pixel 
  // module (with non-empty RDO collection...). 
  // It clusters together the RDOs with a pixell cell side in common.

  PixelClusterCollection* MergedPixelsTool::clusterize( const InDetRawDataCollection<PixelRDORawData> &collection,
                                                        const PixelID& pixelID) const
  {
    // call the fast clusterizetion method if required
    if (m_doFastClustering) return clusterizeFast(collection, pixelID);    
    
    // Get the messaging service, print where you are
    
    // Get Identifier and IdentifierHash for these RDOs
    Identifier elementID = collection.identify();
    IdentifierHash idHash = collection.identifyHash();
    
    // Size of RDO's collection:
    const unsigned int RDO_size = collection.size();
    if ( RDO_size==0) {
        // Empty RDO collection
        ATH_MSG_DEBUG (" areNeighbours - problems ");
        return 0;
    }
    
    int clusterNumber = 0;
    
    // If module is bad, do not create a cluster collection
    if (m_useModuleMap && !(m_summaryTool->isGood(idHash))) 
      return 0;
    
    typedef InDetRawDataCollection<PixelRDORawData> RDO_Collection;
    
    // Get detector info.
    // Find detector element for these RDOs

    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey);
    const InDetDD::SiDetectorElementCollection* pixelDetEle(*pixelDetEleHandle);
    if (not pixelDetEleHandle.isValid() or pixelDetEle==nullptr) {
      ATH_MSG_FATAL(m_pixelDetEleCollKey.fullKey() << " is not available.");
      return 0;
    }
    const InDetDD::SiDetectorElement* element = pixelDetEle->getDetectorElement(idHash);
    
    const Trk::RectangleBounds *mybounds=dynamic_cast<const Trk::RectangleBounds *>(&element->surface().bounds());
    if (not mybounds) {
      ATH_MSG_ERROR("Dynamic cast failed at "<<__LINE__<<" of MergedPixelsTool.cxx.");
      return nullptr;
    }
    // RDOs will be accumulated as a vector of Identifier, which will
    // be added to a vector of groups.
    
    RDO_GroupVector rdoGroups;
    rdoGroups.reserve(RDO_size);
    
    // A similar structure is created for TOT information
    TOT_GroupVector totGroups;
    totGroups.reserve(RDO_size);
    TOT_GroupVector lvl1Groups;
    lvl1Groups.reserve(RDO_size);
    
   // loop on all RDOs
    for(RDO_Collection::const_iterator nextRDO=collection.begin() ; 
        nextRDO!=collection.end() ; ++nextRDO) {
      Identifier rdoID= (*nextRDO)->identify();
      // if a pixel is not skip it in clusterization
      if ( m_usePixelMap && !(m_summaryTool->isGood(idHash,rdoID)) ) continue;
      const int tot = (*nextRDO)->getToT();
      const int lvl1= (*nextRDO)->getLVL1A();
      // check if this is a ganged pixel    
      Identifier gangedID;
      const bool ganged= isGanged(rdoID, element, gangedID);  
      if(ganged) {
        ATH_MSG_VERBOSE("Ganged Pixel, row = " << pixelID.phi_index(rdoID) 
        << "Ganged row = " << pixelID.phi_index(gangedID));   
      }
      else
        ATH_MSG_VERBOSE("Not ganged Pixel, row = " << pixelID.phi_index(rdoID)); 
      
      // loop on all existing RDO groups, until you find that the RDO 
      // is a neighbour of the group.
      bool found= false;
      
      RDO_GroupVector::iterator firstGroup = rdoGroups.begin();
      RDO_GroupVector::iterator lastGroup  = rdoGroups.end();
      TOT_GroupVector::iterator totGroup   = totGroups.begin();    
      TOT_GroupVector::iterator lvl1Group  = lvl1Groups.begin();
      
      while( !found && firstGroup!= lastGroup)
        {
          // Check if RDO is neighbour of the cluster
          if (areNeighbours(**firstGroup, rdoID, element, pixelID)) {
              // if RDO is a duplicate of one in the cluster, do not add it. 
            // Instead the method isDuplicated check wether the new
            // one has a larger LVL1 - if so it does replace the old 
            // lvl1 with the new one.  
            if(!m_checkDuplicatedRDO ||
               (m_checkDuplicatedRDO && !isDuplicated(**firstGroup, **lvl1Group, rdoID, lvl1, pixelID))) {
              (*firstGroup)->push_back(rdoID);
              (*totGroup)->push_back(tot);
              (*lvl1Group)->push_back(lvl1);
              // see if it is a neighbour to  any other groups
              checkForMerge(rdoID, firstGroup, lastGroup, 
                            totGroup, lvl1Group, element, pixelID); 
            } else {
              ATH_MSG_VERBOSE("duplicate found");
            }
            found = true; 
          }
          ++firstGroup;
          ++totGroup;
          ++lvl1Group;
        }
        
      // if RDO is isolated, create new cluster. 
      if(!found) {
        RDO_Vector* newGroup= new RDO_Vector;
        rdoGroups.push_back(newGroup);
        newGroup->push_back(rdoID);
        TOT_Vector* newtotGroup = new TOT_Vector;
        totGroups.push_back(newtotGroup);
        newtotGroup->push_back(tot);
        TOT_Vector* newlvl1Group = new TOT_Vector;
        lvl1Groups.push_back(newlvl1Group);
        newlvl1Group->push_back(lvl1);
      }
      
      // Repeat for ganged pixel if necessary
      if (! ganged) continue;
      
      ATH_MSG_VERBOSE("Ganged pixel, row = " << pixelID.phi_index(gangedID));
      found= false;
      
      firstGroup = rdoGroups.begin();
      lastGroup  = rdoGroups.end();
      totGroup   = totGroups.begin();
      lvl1Group  = lvl1Groups.begin();
      
      while( !found && firstGroup!= lastGroup) {
        // if  neighbour of the cluster, add it to the cluster
        if ( areNeighbours(**firstGroup, gangedID, element, pixelID) ) {
          ATH_MSG_VERBOSE("Ganged pixel is neighbour of a cluster");

          // if RDO is a duplicate of one in the cluster, do not add it. 
          // Instead the method isDuplicated check wether the new
          // one has a larger LVL1 - if so it does replace the old 
          // lvl1 with the new one.  
          
          if(!m_checkDuplicatedRDO ||
             (m_checkDuplicatedRDO && !isDuplicated(**firstGroup, **lvl1Group, gangedID, lvl1, pixelID))) {
            (*firstGroup)->push_back(gangedID);
            (*totGroup)->push_back(tot);
            (*lvl1Group)->push_back(lvl1);
            checkForMerge(gangedID, firstGroup, lastGroup, totGroup, lvl1Group, element, pixelID);
          } else {
            ATH_MSG_VERBOSE("duplicate found");
          }
          found = true;
        }
        ++firstGroup;
        ++totGroup;
        ++lvl1Group;
      }
      
      // if  isolated, create new cluster.       
      if(!found) {
        ATH_MSG_VERBOSE("New cluster with ganged pixel");	
        RDO_Vector* newGroup= new RDO_Vector;
        rdoGroups.push_back(newGroup);
        newGroup->push_back(gangedID);
        TOT_Vector* newtotGroup = new TOT_Vector;
        totGroups.push_back(newtotGroup);
        newtotGroup->push_back(tot);
        TOT_Vector* newlvl1Group = new TOT_Vector;
        lvl1Groups.push_back(newlvl1Group);
        newlvl1Group->push_back(lvl1);
      }
    }
      
    if(totGroups.size() != rdoGroups.size())
      ATH_MSG_ERROR("Mismatch between RDO identifier and TOT info!");
    // We now have groups of contiguous RDOs. Make clusters
    // Make a new empty cluster collection
    PixelClusterCollection  *clusterCollection =  new PixelClusterCollection(idHash);
    clusterCollection->setIdentifier(elementID);
    clusterCollection->reserve(rdoGroups.size());

    std::vector<TOT_Vector *>::iterator totgroup = totGroups.begin();
    std::vector<TOT_Vector *>::iterator lvl1group= lvl1Groups.begin();
        
    // LOOP over the RDO-groups to be clustered ----------------------------------------------------------------
    // Logics in splitting is: the splitter is only activated clusters within min/max split size,
    //  single pixel clusters are not split into more clusters, but can be modified if included by split size req.
    //  additionally:
    // (1) split size > m_minSplitSize : just follow output of the splitter
    // (2) split size <= m_minSplitSize && no split done by the splitter : 
    //      take position update of splitter if available (profit from NN), flag as split 
    // (3) split size <= m_minSplitSize && split done : fallback to pseudo-emulation mode 
    //      (i.e. do not allow splitting, but write split probabilities, do not flag as split)
    // (E) if configured to run in emulation mode, flag as split, save split information, but never split
    //
    // (MAIN CLUSTERIZATION LOOP after connected component finding)
    
    for(std::vector<RDO_Vector *>::iterator group = rdoGroups.begin() ; group!= rdoGroups.end() ; ++group) {

      // the size of the inition rdo group
      const size_t groupSize = (**group).size();
      
      // If cluster is empty, i.e. it has been merged with another, 
      // do not attempt to make cluster.
      if ( groupSize > 0) {
        ++m_processedClusters;
        // create the original cluster - split & split probs are by default false and 0
        PixelCluster* cluster = makeCluster(**group,
                                            **totgroup,
                                            **lvl1group,
                                            element,
                                            pixelID, 
                                            ++clusterNumber);
        
        // no merging has been done;
        if (cluster) { 
          // statistics output
          if ((**group).size() >= m_maxSplitSize ) ++m_largeClusters;
          /** new: store hash id and index in the cluster:
           * hash can be obtained from collection
           * index is just the size of the coll before the push back
           * this is needed for later to make the EL to IDC valid in the 
           * RIO_OnTrack objects set method might be temporary, this tool 
           * (MergedPixelsTool could be friend of the cluster objects)
           */
          cluster->setHashAndIndex(clusterCollection->identifyHash(), clusterCollection->size());
          /** end new stuff */
          clusterCollection->push_back(cluster);
        }
        (**group).clear();
      }
      delete *group;     // now copied into cluster
      *group = 0;
      delete *totgroup;  // won't be used any more (cluster class has not 
      // TOT info at the moment).	
      *totgroup=0;
      totgroup++;
      delete *lvl1group; // now used into cluster
      *lvl1group=0;
      lvl1group++;
    }

    return clusterCollection;
  }

  //-----------------------------------------------------------------------
  // Once the lists of RDOs which makes up the clusters have been found by the
  // clusterize() method, this method is called for each of these lists.
  // The method computes the local position of the cluster, and create 
  // a "cluster object" with all the required information (including charge 
  // interpolation variables Omegax and Omegay, and whether the cluster 
  // contains ganged pixels) 
  // This method calls the ClusterMakerTool to compute global position and 
  // position errors.
  // Input parameters are the list of RDOs identifier of the would-be 
  // cluster, the list of TOT values, the module the cluster belongs to,
  // the pixel helper tool and the number of RDOS of the cluster.
  
  PixelCluster* MergedPixelsTool::makeCluster (const std::vector<Identifier>& group,
                                               const std::vector<int>& totgroup,
                                               const std::vector<int>& lvl1group,
                                               const InDetDD::SiDetectorElement* element,
                                               const PixelID& pixelID,
                                               int& clusterNumber,
                                               bool split,
                                               double splitProb1,
                                               double splitProb2) const 
  {
      ATH_MSG_VERBOSE("makeCluster called, number " << clusterNumber);
  
      Identifier gangedID;
      const Identifier elementID = element->identify();
      const InDetDD::PixelModuleDesign* design
          (dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design()));
      if (not design){
        ATH_MSG_ERROR("Dynamic cast failed at "<<__LINE__<<" of MergedPixelsTool.");
        return nullptr;
      }
      int rowMin = int(2*(design->width()/design->phiPitch()))+1;
      int rowMax = 0;
      int colMin = int(2*(design->length()/design->etaPitch()))+1;
      int colMax = 0;
      int qRowMin = 0;  int qRowMax = 0;
      int qColMin = 0;  int qColMax = 0;
      int lvl1min = 15; // lvl1 of a cluster is the minimum of the lvl1 of the hits
  
      InDetDD::SiLocalPosition sumOfPositions(0,0,0);
      std::vector<Identifier> DVid;
      DVid.reserve( group.size() );
      std::vector<Identifier>::const_iterator rdosBegin = group.begin();
      std::vector<Identifier>::const_iterator rdosEnd = group.end();
      std::vector<int>::const_iterator tot = totgroup.begin();    
      std::vector<int>::const_iterator lvl1= lvl1group.begin();    
  
      // Flag to tag clusters with any ganged pixel
      bool hasGanged = false;  
      for (; rdosBegin!= rdosEnd; ++rdosBegin) {
        // compute cluster lvl1
        if ( (*lvl1) < lvl1min ) lvl1min=(*lvl1);
        lvl1++;
        // process identifier
        const Identifier rId =  *rdosBegin;
        const int row = pixelID.phi_index(rId);
        const int col = pixelID.eta_index(rId);
        // flag if cluster contains at least a ganged pixel
        hasGanged = hasGanged || isGanged(rId, element, gangedID);
        DVid.push_back(rId);
        InDetDD::SiLocalPosition siLocalPosition
        (design->positionFromColumnRow(col,row)); 
        sumOfPositions += siLocalPosition;
        
        // check overflow for IBL
        int realtot = *tot;
        
        if (row == rowMin) qRowMin += realtot;
        if (row < rowMin) { 
          rowMin = row; 
          qRowMin = realtot;
        }
        
        if (row == rowMax) qRowMax += realtot;
        if (row > rowMax) {
          rowMax = row;
          qRowMax = realtot;
        }
        
        if (col == colMin) qColMin += realtot;
        if (col < colMin) {
          colMin = col;
          qColMin = realtot;
        }
        
        if (col == colMax) qColMax += realtot;
        if (col > colMax) {
          colMax = col;
          qColMax = realtot;
        }
        tot++;
      }
      
      const int numberOfPixels = group.size();
      InDetDD::SiLocalPosition centroid(sumOfPositions/numberOfPixels);
      const Identifier id = element->identifierOfPosition(centroid);
  
      const int colWidth = colMax-colMin+1;
      const int rowWidth = rowMax-rowMin+1;
  
      // Compute eta for charge interpolation correction (if required)
      // Two pixels may have tot=0 (very rarely, hopefully)
      float etaRow = -1;
      float etaCol = -1;
      if(qRowMin+qRowMax > 0) etaRow = qRowMax/float(qRowMin+qRowMax);
      if(qColMin+qColMax > 0) etaCol = qColMax/float(qColMin+qColMax);
  
      double etaWidth = design->widthFromColumnRange(colMin, colMax);
      double phiWidth = design->widthFromRowRange(rowMin, rowMax);
      SiWidth siWidth(Amg::Vector2D(rowWidth,colWidth), Amg::Vector2D(phiWidth,etaWidth) );
  
      // Charge interpolation. Very rough guess (one can do better with 
      // candidate track information later) TL
      if(m_posStrategy == 1 && !hasGanged && etaRow>0 && etaCol > 0){
        // width of the region of charge sharing
        // For disks assume normal incidence: delta is small, due to diffusion
        // of drifting charges in silicon
        // For barrel, assume 10 deg. incidence in Rphi, in z compute from 
        // pseudorapidity
        // this may be improved with better parameterization, but it is 
        // probably better to use candidate track information later in 
        // reconstruction. TL
        // Values are made dependent on the sensor thickness to accomodate 
        // different sensors layout. AA
        float deltax = 0;
        float deltay = 0;
        const float sensorThickness = element->thickness();
        Amg::Vector3D globalPos = element->globalPosition(centroid);
        InDetDD::SiLocalPosition totCorrection(0,0,0);
        if(pixelID.is_barrel(elementID)) {
          deltax = 30*micrometer*(sensorThickness/(250*micrometer));
          deltay = sensorThickness*fabs(globalPos.z())/globalPos.perp();
          if(deltay > (design->etaPitch()) ) deltay = design->etaPitch();
        } else {
          deltax = 10*micrometer*sqrt(sensorThickness/(250*micrometer));
          deltay = 10*micrometer*sqrt(sensorThickness/(250*micrometer));
        }
        InDetDD::SiLocalPosition pos1 = design->positionFromColumnRow(colMin,rowMin);
        InDetDD::SiLocalPosition pos2 = design->positionFromColumnRow(colMax,rowMin);
        InDetDD::SiLocalPosition pos3 = design->positionFromColumnRow(colMin,rowMax);
        InDetDD::SiLocalPosition pos4 = design->positionFromColumnRow(colMax,rowMax);
        centroid = 0.25*(pos1+pos2+pos3+pos4)+
        InDetDD::SiLocalPosition(deltay*(etaCol-0.5),deltax*(etaRow-0.5),0.);
        ATH_MSG_VERBOSE("Barrel cluster with global position r= " << globalPos.perp() << " and z = " << globalPos.z());
        ATH_MSG_VERBOSE("deltax = " << deltax << " deltay = " << deltay);
  
      }
      if(m_posStrategy == 10 && !hasGanged  && etaRow>0 && etaCol > 0){
        // recFlag == 10 (CTB simulation)
        // use parametrization studied on CTB data by I. Reisinger (Dortmund)
        if (m_printw) {
          ATH_MSG_ERROR("Detected position strategy = 10, this is an obsolete setting for CTB analysis and is not supported anymore since Athena 15.4.0"); 
          ATH_MSG_ERROR("...reverting to default setting: position strategy = 0");
          m_printw=false;
        }
      }
      // Endcap SR1 Cosmics
      if(m_posStrategy == 20 && !hasGanged  && etaRow>0 && etaCol > 0){
        ATH_MSG_VERBOSE("Endcap cosmics simulation");
        const double deltax = 35*micrometer;
        const double deltay = 35*micrometer;
        InDetDD::SiLocalPosition pos1 = 
            design->positionFromColumnRow(colMin,rowMin);
        InDetDD::SiLocalPosition pos2 = 
            design->positionFromColumnRow(colMax,rowMin);
        InDetDD::SiLocalPosition pos3 = 
            design->positionFromColumnRow(colMin,rowMax);
        InDetDD::SiLocalPosition pos4 = 
            design->positionFromColumnRow(colMax,rowMax);
        centroid = 0.25*(pos1+pos2+pos3+pos4)+
            InDetDD::SiLocalPosition(deltay*(etaCol-0.5),
            deltax*(etaRow-0.5),0.);
      }
  
  
      Amg::Vector2D position(centroid.xPhi(),centroid.xEta());
  
      ATH_MSG_VERBOSE("Cluster ID =" << id); 
      ATH_MSG_VERBOSE("Cluster width (eta x phi) = " << colWidth 
          << " x " << rowWidth);
      ATH_MSG_VERBOSE("Cluster width (eta x phi) = " << etaWidth 
          << " x " << phiWidth);
      ATH_MSG_VERBOSE("Cluster local position (eta,phi) = " 
          << (position)[0] << " " 
          << (position)[1]);
  
      if(!m_clusterMaker){
        PixelCluster* cluster = new PixelCluster(id,position,DVid,lvl1min,totgroup,siWidth,element,0);
        return cluster;
      } else {
        ATH_MSG_VERBOSE("Cluster omega old = " << etaRow <<  " " << etaCol);       
        PixelCluster* cluster = m_clusterMaker->pixelCluster(id,
                                                             position,
                                                             DVid,
                                                             lvl1min,
                                                             totgroup,
                                                             siWidth,
                                                             element,
                                                             hasGanged,
                                                             m_errorStrategy,
                                                             pixelID,
                                                             split,
                                                             splitProb1,
                                                             splitProb2);
          return cluster;
      }
  }
  //--------------------------------------------------------------------------
  // Determines if a pixel cell (whose identifier is the first argument) is 
  // a ganged pixel. If this is the case, the last argument assumes the 
  // value of the identifier of the cell it is ganged with. 
  // The second argument is the pixel module the hit belongs to.
  
  
  bool MergedPixelsTool::isGanged(const Identifier& rdoID,
                                  const InDetDD::SiDetectorElement* element,  
                                  Identifier& gangedID) const
  {
    // returns true for ganged pixels. If true returns Identifier of pixel
    InDetDD::SiCellId cellID = element->cellIdFromIdentifier (rdoID);
    if (element->numberOfConnectedCells (cellID) > 1) {
      InDetDD::SiCellId gangedCellID = element->connectedCell (cellID,1);
      gangedID = element->identifierFromCellId (gangedCellID);
      return true;
    } else {
      gangedID = Identifier();
      return false;
    }
  } 
  
  
  
  //-----------------------------------------------------------------------
  // Checks if two RDO lists (would be clusters) whould be merged, 
  // which do happen if there is a pair of pixel cells belonging to the 
  // two different groups which have a side in common
  
  void MergedPixelsTool::checkForMerge(const Identifier& id, 
                                       MergedPixelsTool::RDO_GroupVector::iterator  baseGroup,
                                       MergedPixelsTool::RDO_GroupVector::iterator lastGroup,
                                       MergedPixelsTool::TOT_GroupVector::iterator totGroup,
                                       MergedPixelsTool::TOT_GroupVector::iterator lvl1Group,
                                       const InDetDD::SiDetectorElement* element,
                                       const PixelID& pixelID) const
  {
    // Look at each of groups that haven't already been checked to see if
    // RDO identifier is in any of them. If so, copy vector to base group and 
    // the vector that has been copied.
    typedef RDO_Vector::const_iterator idIterator;
    typedef TOT_Vector::const_iterator totIterator;
    MergedPixelsTool::RDO_GroupVector::const_iterator nextGroup(baseGroup+1);
    MergedPixelsTool::TOT_GroupVector::const_iterator nextTotGroup(totGroup+1);
    MergedPixelsTool::TOT_GroupVector::const_iterator nextLvl1Group(lvl1Group+1);
  
    for (; nextGroup!= lastGroup; ++nextGroup) { 
      if (areNeighbours(**nextGroup, id, element, pixelID)) {
        // merge the RDO identifier groups
        idIterator firstID = (*nextGroup)->begin();
        idIterator lastID = (*nextGroup)->end();
        for (; firstID != lastID; ++firstID) {
          (*baseGroup)->push_back(*firstID);
        }
        (*nextGroup)->clear();
        // merge the tot vectors
        totIterator firstTot = (*nextTotGroup)->begin();
        totIterator lastTot = (*nextTotGroup)->end();
        for (; firstTot != lastTot; ++firstTot) {
          (*totGroup)->push_back(*firstTot);
        }
        (*nextTotGroup)->clear();
        // merge the tot vectors
        totIterator firstLvl1 = (*nextLvl1Group)->begin();
        totIterator lastLvl1 = (*nextLvl1Group)->end();
        for (; firstLvl1 != lastLvl1; ++firstLvl1) {
          (*lvl1Group)->push_back(*firstLvl1);
        }
        (*nextLvl1Group)->clear();
      }
      nextTotGroup++;
      nextLvl1Group++;
    }  
  }
  
  //-----------------------------------------------------------------------
  
  
  
 
  //-----------------------------------------------------------------------
  // Called by the clusterize method if fast clustering is switched on.
  // Runs for every pixel module (with non-empty RDO collection...). 
  // It clusters together the RDOs with a pixell cell side in common
  // using connected component analysis based on four-cell connectivity  
  PixelClusterCollection*  MergedPixelsTool::clusterizeFast(const InDetRawDataCollection<PixelRDORawData> &collection,
                                                            const PixelID& pixelID) const {
    
    // Size of RDO's collection:
    const unsigned int RDO_size = collection.size();
    if ( RDO_size==0) {
        // Empty RDO collection
        ATH_MSG_DEBUG (" areNeighbours - problems ");
        return nullptr;
    }
    
    // Get Identifier and IdentifierHash for these RDOs
    const Identifier elementID = collection.identify();
    const IdentifierHash idHash = collection.identifyHash();
    
    // If module is bad, do not create a cluster collection
    if (m_useModuleMap && !(m_summaryTool->isGood(idHash))) 
      return nullptr;
    
    // Get detector info.
    // Find detector element for these RDOs

    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey);
    const InDetDD::SiDetectorElementCollection* pixelDetEle(*pixelDetEleHandle);
    if (not pixelDetEleHandle.isValid() or pixelDetEle==nullptr) {
      ATH_MSG_FATAL(m_pixelDetEleCollKey.fullKey() << " is not available.");
      return nullptr;
    }
    const InDetDD::SiDetectorElement* element = pixelDetEle->getDetectorElement(idHash);
    
    const Trk::RectangleBounds *mybounds=dynamic_cast<const Trk::RectangleBounds *>(&element->surface().bounds());
    if (not mybounds) {
      ATH_MSG_ERROR("Dynamic cast failed at "<<__LINE__<<" of MergedPixelsTool.cxx.");
      return nullptr;
    }
    
    std::vector<rowcolID> collectionID;

    for(const auto & rdo : collection) {
      
      const Identifier rdoID= rdo->identify();
      if (m_usePixelMap and !(m_summaryTool->isGood(idHash,rdoID))) continue;
          
      const int lvl1= rdo->getLVL1A();      
      if (m_checkDuplicatedRDO and checkDuplication(pixelID, rdoID, lvl1, collectionID)) continue;
      
      const int tot = rdo->getToT();
      
      rowcolID   RCI(-1, pixelID.phi_index(rdoID), pixelID.eta_index(rdoID), tot, lvl1, rdoID); 
      collectionID.push_back(RCI);
      
      // check if this is a ganged pixel    
      Identifier gangedID;
      const bool ganged = isGanged(rdoID, element, gangedID);  
      
      if (not ganged) continue;
          
      // if it is a ganged pixel, add its ganged RDO id to the collections
      rowcolID   RCI_ganged(-1, pixelID.phi_index(gangedID), pixelID.eta_index(gangedID), tot, lvl1, gangedID);
      collectionID.push_back(RCI_ganged);      
    }
    
    // Sort pixels in ascending columns order
    // 
    if(collectionID.empty()) return 0;    
    if(collectionID.size() > 1) std::sort(collectionID.begin(),collectionID.end(),pixel_less);
    
    // initialize the networks
    std::vector<network> connections(collectionID.size());

    // Network production
    //
    int r = 0, re = collectionID.size();

    for(; r!=re-1; ++r) {

      int NB  = 0;
      int row = collectionID.at(r).ROW  ;
      int col = collectionID.at(r).COL+1; 

      for(int rn = r+1; rn!=re; ++rn) {
                
        int dc = collectionID.at(rn).COL - col;
        
        if( dc > 0) break;
        
        if( fabs(collectionID.at(rn).ROW-row)+dc == 0 ) {
          connections.at(r).CON.at(connections.at(r).NC++) = rn;
          connections.at(rn).CON.at(connections.at(rn).NC++) = r ;
          if(++NB==2) break;
        }
      }
    }

    // Pixels clusterization
    //
    int Ncluster = 0;
    for(r=0; r!=re; ++r) {
      if(collectionID.at(r).NCL < 0) {
        collectionID.at(r).NCL = Ncluster;
        addClusterNumber(r,Ncluster,connections,collectionID);
        ++Ncluster;
      }
    }

    // Clusters sort in Ncluster order
    //
    if(--re > 1) {
      for(int i(1); i<re; ++i ) {
        rowcolID U  = collectionID.at(i+1);
        
        int j(i);
        while(collectionID.at(j).NCL > U.NCL) {
          collectionID.at(j+1)=collectionID.at(j); 
          --j;
        }
        collectionID.at(j+1)=U;
      }
    }

    // Make a new pixel cluster collection
    //
    PixelClusterCollection  *clusterCollection = new PixelClusterCollection(idHash);
    clusterCollection->setIdentifier(elementID);
    clusterCollection->reserve(Ncluster);

    std::vector<Identifier> DVid = {collectionID.at(0).ID };
    std::vector<int>        Totg = {collectionID.at(0).TOT};
    std::vector<int>        Lvl1 = {collectionID.at(0).LVL1};
    
    int clusterNumber = 0;
    int NCL0          = 0;
    
    DVid.reserve(collectionID.back().NCL);
    Totg.reserve(collectionID.back().NCL);
    Lvl1.reserve(collectionID.back().NCL);

    ++re;    
    for(int i=1; i<=re; ++i) {

      if(i!=re and collectionID.at(i).NCL==NCL0) {
        DVid.push_back(collectionID.at(i).ID );
        Totg.push_back(collectionID.at(i).TOT);
        Lvl1.push_back(collectionID.at(i).LVL1);
      
      } else {
        
        // Cluster production
        ++m_processedClusters;
        PixelCluster* cluster = makeCluster(DVid,
                                            Totg,
                                            Lvl1,
                                            element,
                                            pixelID, 
                                            ++clusterNumber);
        
        const size_t groupSize = DVid.size();        
        
        
        
        // no merging has been done;
        if (cluster) { 
          // statistics output
          if (groupSize >= m_maxSplitSize ) ++m_largeClusters;
          /** new: store hash id and index in the cluster:
           * hash can be obtained from collection
           * index is just the size of the coll before the push back
           * this is needed for later to make the EL to IDC valid in the 
           * RIO_OnTrack objects set method might be temporary, this tool 
           * (MergedPixelsTool could be friend of the cluster objects)
           */
          cluster->setHashAndIndex(clusterCollection->identifyHash(), clusterCollection->size());
          /** end new stuff */
          clusterCollection->push_back(cluster);
        }      
        
        
        // Preparation for next cluster
        if (i!=re) {
          NCL0   = collectionID.at(i).NCL                     ;
          DVid.clear(); DVid = {collectionID.at(i).ID };
          Totg.clear(); Totg = {collectionID.at(i).TOT};
          Lvl1.clear(); Lvl1 = {collectionID.at(i).LVL1};
        }
      }
    }

    return clusterCollection;
  }

  ///////////////////////////////////////////////////////////////////
  // Clusterization for fast mode 
  ///////////////////////////////////////////////////////////////////
  
  void MergedPixelsTool::addClusterNumber(const int& r, 
                                          const int& Ncluster,
                                          const std::vector<network>& connections,                                         
                                          std::vector<rowcolID>& collectionID) const {
    for(int i=0; i!=connections.at(r).NC; ++i) {      
      const int k = connections.at(r).CON.at(i);
      if(collectionID.at(k).NCL < 0) {
        collectionID.at(k).NCL = Ncluster;
        addClusterNumber(k, Ncluster, connections, collectionID);
      }
    }
  }
  
  
  bool MergedPixelsTool::checkDuplication(const PixelID& pixelID,
                                          const Identifier& rdoID, 
                                          const int& lvl1, 
                                          std::vector<rowcolID>& collectionID) const {
    
    // check if duplicates are found. If that is the case, update the LVL1 
    auto isDuplicate=[pixelID,rdoID](const rowcolID& rc) -> bool {
      return std::make_tuple(pixelID.phi_index(rdoID), pixelID.eta_index(rdoID)) == std::make_tuple(pixelID.phi_index(rc.ID), pixelID.eta_index(rc.ID));
    }; 
    
    const auto pDuplicate = std::find_if(collectionID.begin(), collectionID.end(),isDuplicate); 
    const bool foundDuplicate{pDuplicate != collectionID.end()}; 
    if (foundDuplicate) pDuplicate->LVL1 = std::max(pDuplicate->LVL1, lvl1); 
    return foundDuplicate;
  }                                            
}
//----------------------------------------------------------------------------
