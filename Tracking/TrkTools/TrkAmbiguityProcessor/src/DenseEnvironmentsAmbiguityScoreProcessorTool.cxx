/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "DenseEnvironmentsAmbiguityScoreProcessorTool.h"
#include "TrackScoringTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/TrackInfo.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitProbTool.h"
#include "TrkTrackSummary/TrackSummary.h"

#include <map>
#include <ext/functional>
#include <iterator>
#include "TString.h"

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetIdentifier/PixelID.h"

//==================================================================================================
Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::DenseEnvironmentsAmbiguityScoreProcessorTool(const std::string& t, 
                                const std::string& n,
                                const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_scoringTool("Trk::TrackScoringTool/TrackScoringTool"), 
  m_selectionTool("InDet::InDetDenseEnvAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool"),
  m_splitProbTool("InDet::NnPixelClusterSplitProbTool/NnPixelClusterSplitProbTool"),
  m_etabounds{0.8, 1.6, 2.5,4.0},
  m_stat(m_etabounds)
{

  declareInterface<ITrackAmbiguityScoreProcessorTool>(this);
  declareProperty("ScoringTool"          , m_scoringTool);
  declareProperty("SelectionTool"        , m_selectionTool);
  declareProperty("SplitProbTool"        , m_splitProbTool);
  declareProperty("SplitClusterMap_old"  , m_splitClusterMapKey_last);
  declareProperty("SplitClusterMap_new"  , m_splitClusterMapKey);
  declareProperty("sharedProbCut"        , m_sharedProbCut           = 0.3);
  declareProperty("sharedProbCut2"       , m_sharedProbCut2          = 0.3);
  declareProperty("etaBounds"            , m_etabounds,"eta intervals for internal monitoring");

}
//==================================================================================================

Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::~DenseEnvironmentsAmbiguityScoreProcessorTool()
{
}
//==================================================================================================

StatusCode Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_CHECK( m_scoringTool.retrieve());

  ATH_CHECK( m_assoTool.retrieve()) ;
  ATH_CHECK( m_assoToolNotGanged.retrieve( DisableTool{m_assoToolNotGanged.empty()} )) ;
  ATH_CHECK( m_assoMapName.initialize(!m_assoMapName.key().empty()) );

  ATH_CHECK( m_selectionTool.retrieve());

  ATH_CHECK(m_splitProbTool.retrieve( DisableTool{m_splitProbTool.empty()} ));

  ATH_CHECK( m_splitClusterMapKey_last.initialize(!m_splitClusterMapKey_last.key().empty()) );
  ATH_CHECK( m_splitClusterMapKey.initialize(!m_splitClusterMapKey.key().empty()) );

  if (m_stat.etaBounds().size() != TrackStat::kNStatRegions-1) {
     ATH_MSG_FATAL("There must be exactly " << (TrackStat::kNStatRegions-1) << " eta bounds but "
                   << m_stat.etaBounds().size() << " are set." );
     return StatusCode::FAILURE;
  }

  return sc;
}
//==================================================================================================

StatusCode Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::finalize()
{
  return StatusCode::SUCCESS;
}

void Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::statistics() {
   if (msgLvl(MSG::INFO)) {
      MsgStream &out=msg(MSG::INFO);
      out << " -- statistics " << std::endl;
      std::lock_guard<std::mutex> lock( m_statMutex );
      m_stat.dump(out);
      out << endmsg;
   }
}

void Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::TrackStat::dump(MsgStream &out) const
{
   // @TODO restore ios
   std::streamsize ss = out.precision();
   int iw=9;
   out << "------------------------------------------------------------------------------------" << std::endl;
   out << "  Number of events processed      :   "<< m_globalCounter[kNevents].value() << std::endl;
   if (m_globalCounter[kNInvalidTracks]>0) {
      out << "  Number of invalid tracks        :   "<< m_globalCounter[kNInvalidTracks].value() << std::endl;
   }
   if (m_globalCounter[kNTracksWithoutParam]>0) {
      out << "  Tracks without parameters       :   "<< m_globalCounter[kNTracksWithoutParam].value() << std::endl;
   }
   out << "  statistics by eta range          ------All---Barrel---Trans.-- Endcap-- Forwrd-- " << std::endl;
   out << "------------------------------------------------------------------------------------" << std::endl;
   dumpStatType(out, "  Number of candidates at input   :",    kNcandidates,iw);
   dumpStatType(out, "  - candidates rejected score 0   :",    kNcandScoreZero,iw);
   dumpStatType(out, "  - candidates rejected as double :",    kNcandDouble,iw);
   out << "------------------------------------------------------------------------------------" << std::endl;
   out << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2)
       << "    definition: ( 0.0 < Barrel < " << (*m_etabounds)[iBarrel-1] << " < Transition < " << (*m_etabounds)[iTransi-1]
       << " < Endcap < " << (*m_etabounds)[iEndcap-1] << " < Forward < " << (*m_etabounds)[iForwrd-1] << " )" << std::endl;
   out << "------------------------------------------------------------------------------------" << std::endl;
   out << std::setprecision(ss);
}

//==================================================================================================

/** Do actual processing of event. Takes a track container, 
    and then returns the tracks which have been selected*/

void Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::process(std::vector<const Track*>* tracks,
                                                                Trk::TracksScores* trackScoreTrackMap) const
{
  InDet::PixelGangedClusterAmbiguities *splitClusterMap = nullptr;
  if(!m_splitClusterMapKey.key().empty()){
    if(!m_splitClusterMapKey_last.key().empty()){
      SG::ReadHandle<InDet::PixelGangedClusterAmbiguities> splitClusterMapHandle_last(m_splitClusterMapKey_last);
      if ( !splitClusterMapHandle_last.isValid() ){
        ATH_MSG_ERROR("Could not read last splitClusterMap.");
      }
      splitClusterMap =  new InDet::PixelGangedClusterAmbiguities(*splitClusterMapHandle_last);
    }
    else{
      splitClusterMap =  new InDet::PixelGangedClusterAmbiguities();
    }
  }

  addNewTracks(tracks, trackScoreTrackMap);
  std::unique_ptr<Trk::PRDtoTrackMap> prd_to_track_map( m_assoToolNotGanged.isEnabled()
                                                        ? m_assoToolNotGanged->createPRDtoTrackMap()
                                                        : m_assoTool->createPRDtoTrackMap() );
  overlappingTracks(trackScoreTrackMap, splitClusterMap, *prd_to_track_map);
  if (!m_assoMapName.key().empty()) {
     if (SG::WriteHandle<Trk::PRDtoTrackMap>(m_assoMapName).record(
                (m_assoToolNotGanged.isEnabled()
                 ? m_assoToolNotGanged->reduceToStorableMap(std::move(prd_to_track_map))
                 : m_assoTool->reduceToStorableMap(std::move(prd_to_track_map)) )).isFailure()) {
        ATH_MSG_FATAL("Failed to add PRD to track association map " << m_assoMapName.key() << ".");
     }
  }


  if(!m_splitClusterMapKey.key().empty()){
    SG::WriteHandle<InDet::PixelGangedClusterAmbiguities> splitClusterMapHandle(m_splitClusterMapKey);
    splitClusterMapHandle = std::unique_ptr<InDet::PixelGangedClusterAmbiguities>(splitClusterMap);
    if( !splitClusterMapHandle.isValid() ){
      ATH_MSG_ERROR("Could not record splitClusterMap.");
    }
  }

  }


//==================================================================================================
void Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::addNewTracks(std::vector<const Track*>* tracks,
                                                                     Trk::TracksScores* trackScoreTrackMap) const
{
  TrackStat stat(m_stat.etaBounds());
  stat.newEvent();

  std::unique_ptr<Trk::PRDtoTrackMap> prd_to_track_map( m_assoTool->createPRDtoTrackMap() );
  PrdSignatureSet prdSigSet;  

  ATH_MSG_DEBUG ("Number of tracks at Input: "<<tracks->size());
 
  for(const Track* a_track : *tracks) {
    ATH_MSG_DEBUG ("Processing track candidate "<<a_track);
    stat.increment_by_eta(TrackStat::kNcandidates,a_track); // @TODO should go to the score processor

    // only fitted tracks get hole search, input is not fitted
    float score = m_scoringTool->score( *a_track, true);
    ATH_MSG_DEBUG ("Track Score is "<< score);
    // veto tracks with score 0
    bool reject = score==0;      
    
    // double track rejection
    if (!reject) {
      stat.increment_by_eta(TrackStat::kNcandScoreZero,a_track);
      std::vector<const Trk::PrepRawData*> prds = m_assoTool->getPrdsOnTrack(*prd_to_track_map, *a_track);
      // convert to set
      PrdSignature prdSig( prds.begin(),prds.end() );
      // we try to insert it into the set, if we fail (pair.second), it then exits already
      if ( !(prdSigSet.insert(prdSig)).second ) {
        ATH_MSG_DEBUG ("Double track, reject it !");
        stat.increment_by_eta(TrackStat::kNcandDouble,a_track);
        reject = true;
      } else {
        ATH_MSG_DEBUG ("Insert new track in PrdSignatureSet");
      }
    }

    if (!reject) {
      // add track to map, map is sorted small to big ! set if fitted

      ATH_MSG_VERBOSE ("Track ("<< a_track <<" --> "<< *a_track << ") has score "<<score);
      trackScoreTrackMap->push_back( std::make_pair(a_track, -score));
    }
  }

  ATH_MSG_DEBUG ("Number of tracks in map:"<<trackScoreTrackMap->size());
  {
     std::lock_guard<std::mutex> lock(m_statMutex);
     m_stat += stat;
  }

  }

//==================================================================================================
void Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::updatePixelSplitInformationForCluster(const std::pair<const InDet::PixelCluster* const,
                                                                                                              const Trk::TrackParameters*> & clusterTrkPara,
                                                                                              InDet::PixelGangedClusterAmbiguities *splitClusterMap) const
{

  // Recalculate the split prob with the use of the track parameters
  InDet::PixelClusterSplitProb splitProb = m_splitProbTool->splitProbability( *clusterTrkPara.first, *clusterTrkPara.second );
  // update the split prob information on the cluster --  the use of the split flag is now questionable -- possible it will now indicate if the cluster is shared between multiple tracks
  InDet::PixelCluster* pixelCluster = const_cast<InDet::PixelCluster*> ( clusterTrkPara.first );    
  //TODO: const_cast?

  ATH_MSG_DEBUG (  "---- "<< pixelCluster->globalPosition().perp() 
                             <<" Updating split probs 1:  Old " << pixelCluster->splitProbability1() << "  New " << splitProb.splitProbability(2) 
                             <<" Probs 2:  Old " << pixelCluster->splitProbability2() << "  New " << splitProb.splitProbability(3) 
                             << std::endl
                             << " --- pixelCluster: " <<  *pixelCluster
                             << std::endl
                             << " --- trk params: " << *clusterTrkPara.second  );

  if ( splitProb.splitProbability(2)  < 0 ){
    pixelCluster->packSplitInformation( false, 0.0, 0.0 );    
    pixelCluster->setTooBigToBeSplit( true );    
  }else{  
    pixelCluster->packSplitInformation( false, splitProb.splitProbability(2), splitProb.splitProbability(3) ) ;
    pixelCluster->setTooBigToBeSplit( false );    
  }
  
  if(splitClusterMap){
    if(  pixelCluster->splitProbability2()  >=  m_sharedProbCut2){
      splitClusterMap->insert(std::make_pair( pixelCluster, pixelCluster ) );
      splitClusterMap->insert(std::make_pair( pixelCluster, pixelCluster ) );
    } else if ( pixelCluster->splitProbability1()  >=  m_sharedProbCut ){  
      splitClusterMap->insert(std::make_pair( pixelCluster, pixelCluster ) );
    }
  }

}

//==================================================================================================
void Trk::DenseEnvironmentsAmbiguityScoreProcessorTool::overlappingTracks(const TracksScores* scoredTracks,
                                                                          InDet::PixelGangedClusterAmbiguities *splitClusterMap,
                                                                          Trk::PRDtoTrackMap &prd_to_track_map) const
{
  const Trk::IPRDtoTrackMapTool *the_asso_tool = (m_assoToolNotGanged.isEnabled() ? &(*m_assoToolNotGanged) : &(*m_assoTool));
  // Function currnetly does nothing useful expect for printout debug information
  ATH_MSG_DEBUG ("Starting to resolve overlapping tracks");

  // Map to add all pixel clusters on track to
  std::map< const InDet::PixelCluster*, const Trk::TrackParameters* > setOfPixelClustersOnTrack;
  std::map< const InDet::PixelCluster*, const Trk::Track* > setOfPixelClustersToTrackAssoc;

  // Fill pixel cluster into the above map
  // Fill all PRD infromation into the association tool
  for( const std::pair<const Track*, float>& scoredTracksItem : *scoredTracks )
  {
    // clean it out to make sure not to many shared hits
    ATH_MSG_VERBOSE ("--- Adding next track "<<scoredTracksItem.first
                     << ":" << (scoredTracksItem.first->trackParameters() ? scoredTracksItem.first->trackParameters()->front()->pT() : -1.)
                     << ", " << scoredTracksItem.first->measurementsOnTrack()->size()
                     <<"\t with score "<<-scoredTracksItem.second << " to PRD map");
    
    //  This should only be done in region defined by Jets 
    //  ..... for now let do the whole detector coudl be slow
    if(the_asso_tool->addPRDs( prd_to_track_map, *scoredTracksItem.first ).isSuccess()){
      ATH_MSG_VERBOSE("--- Added hits to the association tool");
    } else {
      ATH_MSG_VERBOSE("--- Failed to add hits to the association tool");
      continue;    
    }  
  
    // get all prds on 'track'
    const DataVector<const TrackStateOnSurface>* tsosVec = scoredTracksItem.first->trackStateOnSurfaces();  
    if(!tsosVec){
      ATH_MSG_WARNING("TSOS vector does not exist");
      continue;   
    }

    ATH_MSG_VERBOSE("---> Looping over TSOS's to allow  for cluster updates: "<< tsosVec->size() );

    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos = tsosVec->begin();
    for (; tsos != tsosVec->end(); ++tsos) {
      const MeasurementBase* measurement = (*tsos)->measurementOnTrack();   
        
      if(!measurement || ! (*tsos)->trackParameters()){
        ATH_MSG_VERBOSE("---- TSOS has either no measurement or parameters: "<< measurement << "  " << (*tsos)->trackParameters() );
        continue;           
      }
      
      const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*> ( measurement );
      if(!rio){
        continue;
      }
           
      const InDet::PixelCluster* pixel = dynamic_cast<const InDet::PixelCluster*> ( rio->prepRawData() );
      if (pixel) {
        
        //Update the pixel split information if the element is unique (The second element of the pair indiciates if the element was inserted into the map)
        auto ret =  setOfPixelClustersOnTrack.insert(std::make_pair( pixel, (*tsos)->trackParameters() ));
        if (ret.second) {
          updatePixelSplitInformationForCluster( *(ret.first), splitClusterMap);
        }
        
        setOfPixelClustersToTrackAssoc.insert( std::make_pair( pixel, scoredTracksItem.first ) );
        continue;
      }
    
    }  
  }

  // now loop as long as map is not empty
    std::vector< std::pair< const InDet::PixelCluster*, const Trk::Track* > > sorted;
    sorted.reserve( setOfPixelClustersToTrackAssoc.size() );
    for( const std::pair< const InDet::PixelCluster*, const Trk::Track* > &pixelTrackItem : setOfPixelClustersToTrackAssoc ) {
      sorted.push_back( pixelTrackItem );
    }
    std::sort( sorted.begin(), sorted.end(), [](const std::pair< const InDet::PixelCluster*, const Trk::Track* > &a,
                                                const std::pair< const InDet::PixelCluster*, const Trk::Track* > &b) {
                 return a.first->getHashAndIndex().collHash() < b.first->getHashAndIndex().collHash()
                   || ( a.first->getHashAndIndex().collHash() == b.first->getHashAndIndex().collHash()
                        &&  a.first->getHashAndIndex().objIndex() < b.first->getHashAndIndex().objIndex() );
      });
    //  for( auto pixelTrackItem : setOfPixelClustersToTrackAssoc )
    for (const std::pair< const InDet::PixelCluster*, const Trk::Track* >  &pixelTrackItem :  sorted) {
    ATH_MSG_VERBOSE ("---- Checking if track shares pixel hits if other tracks: " << pixelTrackItem.first << " with R " << pixelTrackItem.first->globalPosition().perp() );

    // find out how many tracks use this hit already
    Trk::PRDtoTrackMap::ConstPrepRawDataTrackMapRange range = prd_to_track_map.onTracks( *pixelTrackItem.first );
    int numberOfTracksWithThisPrd = std::distance(range.first,range.second);
    if (msgLvl(MSG::VERBOSE)) {
      TString tracks("---- number of tracks with this shared Prd: ");
      tracks += numberOfTracksWithThisPrd;
      for (Trk::IPRD_AssociationTool::ConstPRD_MapIt it =range.first; it != range.second;++it ){
       tracks += "    ";
       tracks += Form( " %p",(void*)(it->second));
       double pt = (it->second->trackParameters() ? it->second->trackParameters()->front()->pT() : -1);
       tracks += Form(":%.3f", pt);
       tracks += Form(",%i",static_cast<int>(it->second->measurementsOnTrack()->size()));
      }
      ATH_MSG_VERBOSE (tracks);
    }
    
  }  
  }

