/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///============================================================
/// T2VertexBeamSpot.h, (c) ATLAS Detector software
/// Trigger/TrigAlgorithms/TrigT2BeamSpot/T2VertexBeamSpot
///
/// Online beam spot measurement and monitoring using
/// L2 recontructed primary vertices.
///
/// Authors : David W. Miller, Rainer Bartoldus,
///           Su Dong
///============================================================
/**********************************************************************************
 * @project: HLT, PESA algorithms
 * @package: TrigT2BeamSpot
 * @class  : T2VertexBeamSpot
 *
 * @brief  PESA algorithm that measures and monitors beam spot position using vertices
 *
 * @author David W. Miller    <David.W.Miller@cern.ch>     - SLAC, Stanford University
 *
 **********************************************************************************/
#ifndef TRIGT2BEAMSPOT_T2VERTEXBEAMSPOT_H
#define TRIGT2BEAMSPOT_T2VERTEXBEAMSPOT_H

#include "AthenaMonitoring/Monitored.h"
#include "xAODEventInfo/EventInfo.h"
/// trigger EDM
#include "TrigInterfaces/AllTEAlgo.h"
//Interface for the beam spot tool
#include "IT2VertexBeamSpotTool.h"

namespace HLT {
  class TriggerElement;
}

/** Return vector of all trigger elements */
HLT::TEVec getAllTEs(const std::vector<HLT::TEVec>& tes_in);

namespace PESA {
  /**
   *  @class T2VertexBeamSpot
   *  This class uses primary vertex reconstruction to measure
   *  and monitor the LHC beam as seen by the ATLAS detector. 
   *  Fast, online vertexing is implemented using Dmitry 
   *  Emeliyanov's (D.Emeliyanov@rl.ac.uk) TrigPrimaryVertexFitter.
   *   
   *  As an AllTEAlgo, this must appear first in the sequence.
   *
   *  @author Ignacio Aracena     <ignacio.aracena@cern.ch>
   *  @author Rainer Bartoldus    <bartoldu@slac.stanford.edu>
   *  @author Su Dong             <sudong@slac.stanford.edu>
   *  @author David W. Miller     <David.W.Miller@cern.ch>
   *   
   */

  class T2VertexBeamSpot : public HLT::AllTEAlgo
    {
    public:

      T2VertexBeamSpot( const std::string& name, ISvcLocator* pSvcLocator ); //!< std Gaudi Algorithm constructor
      virtual ~T2VertexBeamSpot();

      //=================================
      //       Old Run2 setup
      /** Initialize the beamspot algorithm for Run2 configuration within the HLT, initialize all the handles and retrieve the tools associated with the algorithm */
      HLT::ErrorCode hltInitialize();

      /** Finalize the beamspot algorithm for Run2 configuration within the HLT (nothing really happens here atm) */
      HLT::ErrorCode hltFinalize();

      /**
       * @brief implementation of the abstract hltExecute method in HLT::AllTEAlgo.
       *
       * @param input outer vector describeds the different input TE types,
       *              inner vector provides all TE instances of the given type
       * @param output the output TE type
       */
      HLT::ErrorCode hltExecute( std::vector<std::vector<HLT::TriggerElement*> >& input,
                                 unsigned int output );

      /** Function which attaches vertex collections to the trigger element output */
      HLT::ErrorCode attachFeatureVertex( TrigVertexCollection &myVertexCollection,  HLT::TEVec &allTEs, unsigned int type_out );

      /** Function which attaches splitted vertex collections to the trigger element output */
      HLT::ErrorCode attachFeatureSplitVertex(DataVector< TrigVertexCollection > &mySplitVertexCollections,  HLT::TEVec &allTEs, unsigned int type_out );

      //Only for Run2 settings
      bool m_activateTE; /*If true to be added */
      bool m_activateAllTE;/*If true to be added */
      bool m_activateSI;/*If true to be added */
      bool m_attachVertices;/*If true to be added */
      bool m_attachSplitVertices;/*If true to be added */

     std::string m_vertexCollName; 

      //=================================
      //       New Run3 setup
      /** Loop over events, selecting tracks and reconstructing vertices out of these tracks   */
      virtual StatusCode execute() final;

      /** Initialize the beamspot algorithm for Run3 Athena MT configuration, initialize all the handles and retrieve the tools associated with the algorithm */
      virtual StatusCode initialize() final;

      
      SG::ReadHandleKeyArray<TrackCollection> m_trackCollections;   /*Input list of track collection names which should be used for the algorithms*/

      //The same as in Run2 (m_vertexCollName)
      SG::WriteHandleKey<TrigVertexCollection> m_outputVertexCollectionKey;
      //TODO: to be added SG::WriteHandleKeyArray<TrigVertexCollection> m_outputSplitVertexCollectionKey;   /*Input list of track collection names which should be used for the algorithms*/

    private:

      //Read Handles
      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey  { this, "EventInfo", "EventInfo", "" };

      //Tools
      ToolHandle<IT2VertexBeamSpotTool> m_beamSpotTool {this, "PESA::T2VertexBeamSpotTool/T2VertexBeamSpotTool" };
      ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};


    };

} // end namespace




#endif
