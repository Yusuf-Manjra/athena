/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
// MuonCreatorTool
//  AlgTool performing MS hit reallocation for a likely spectrometer-indet 
//  match which has given combined fit problems.
//  Extrapolates indet track to MS.
//  Returns a combined track with full track fit.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCreatorTool.h"
#include "SortInDetCandidates.h"
#include "MuonCombinedEvent/StacoTag.h"
#include "MuonCombinedEvent/CombinedFitTag.h"
#include "MuonCombinedEvent/SegmentTag.h"
#include "MuonCombinedEvent/CaloTag.h"
#include "MuonCombinedEvent/MuGirlLowBetaTag.h"

#include "MuonSegment/MuonSegment.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/AlignmentEffectsOnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"

#include "TrkSegment/SegmentCollection.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuonCnv/IMuonSegmentConverterTool.h"

#include "muonEvent/CaloEnergy.h"
#include "FourMomUtils/P4Helpers.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "TrackToCalo/CaloCellCollector.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

#include "MuidEvent/FieldIntegral.h"

#include "StoreGate/ReadCondHandle.h"

namespace MuonCombined {
 
  MuonCreatorTool::MuonCreatorTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
    m_makeMSPreExtrapLink(false),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_muonPrinter("Rec::MuonPrintingTool/MuonPrintingTool"),
    m_caloExtTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool", this),
    m_particleCreator("Trk::TrackParticleCreatorTool/MuonCombinedTrackParticleCreator"),
    m_ambiguityProcessor("Trk::TrackSelectionProcessorTool/MuonAmbiProcessor"),
    m_propagator("Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"),
    m_muonDressingTool("MuonCombined::MuonDressingTool/MuonDressingTool"),
    m_momentumBalanceTool("Rec::MuonMomentumBalanceSignificanceTool/MuonMomentumBalanceSignificanceTool"),
    m_scatteringAngleTool("Rec::MuonScatteringAngleSignificanceTool/MuonScatteringAngleSignificanceTool"),
    m_selectorTool("CP::MuonSelectionTool/MuonSelectionTool"),
    m_muonSegmentConverterTool("Muon::MuonSegmentConverterTool/MuonSegmentConverterTool"),
    m_meanMDTdADCTool("Rec::MuonMeanMDTdADCFillerTool/MuonMeanMDTdADCFillerTool"),
    m_caloMaterialProvider("Trk::TrkMaterialProviderTool/TrkMaterialProviderTool", this),
    m_trackSegmentAssociationTool("Muon::TrackSegmentAssociationTool/TrackSegmentAssociationTool"),
    m_trackQuery("Rec::MuonTrackQuery/MuonTrackQuery")

  {
    declareInterface<IMuonCreatorTool>(this);
    declareProperty("MakeTrackAtMSLink",m_makeMSPreExtrapLink=false);
    declareProperty("Printer",m_printer );
    declareProperty("ParticleCaloExtensionTool", m_caloExtTool);      
    declareProperty("MuonPrinter",m_muonPrinter );
    declareProperty("TrackParticleCreator",m_particleCreator );
    declareProperty("AmbiguityProcessor",m_ambiguityProcessor );
    declareProperty("Propagator",m_propagator );
    declareProperty("MuonDressingTool",m_muonDressingTool );
    declareProperty("MomentumBalanceTool",m_momentumBalanceTool);
    declareProperty("ScatteringAngleTool",m_scatteringAngleTool);
    declareProperty("MuonSelectionTool", m_selectorTool);
    declareProperty("MeanMDTdADCTool",m_meanMDTdADCTool);
    declareProperty("TrackSegmentAssociationTool",m_trackSegmentAssociationTool);
    declareProperty("BuildStauContainer",m_buildStauContainer=false);
    declareProperty("FillEnergyLossFromTrack",m_fillEnergyLossFromTrack=true);
    declareProperty("FillAlignmentEffectsOnTrack",m_fillAlignmentEffectsOnTrack=true);
    declareProperty("FillExtraELossInfo", m_fillExtraELossInfo=true);
    declareProperty("PrintSummary", m_printSummary=false);
    declareProperty("UseUpdatedExtrapolatedTrack", m_useUpdatedExtrapolatedTrack = true );
    //Default data source for the calocells
    declareProperty("SigmaCaloNoiseCut", m_sigmaCaloNoiseCut=3.4);
    declareProperty("CaloMaterialProvider", m_caloMaterialProvider);
    declareProperty("FillTimingInformation", m_fillTimingInformation = true );
    declareProperty("FillTimingInformationOnMuon", m_fillTimingInformationOnMuon = false );
    declareProperty("AssociateSegmentsToLowBetaMuons",m_segLowBeta = false);
    declareProperty("UseCaloCells",m_useCaloCells = true);
    declareProperty("MakeSAMuons", m_doSA=false);
    declareProperty("TrackQuery", m_trackQuery);
     
  }

  StatusCode MuonCreatorTool::initialize() {

    if( m_buildStauContainer ) ATH_MSG_DEBUG(" building Stau container ");

    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_muonPrinter.retrieve());
    ATH_CHECK(m_caloExtTool.retrieve());
    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_CHECK(m_particleCreator.retrieve());
    ATH_CHECK(m_ambiguityProcessor.retrieve());
    ATH_CHECK(m_muonDressingTool.retrieve());
    ATH_CHECK(m_muonSegmentConverterTool.retrieve());
    if(!m_trackSegmentAssociationTool.empty()) ATH_CHECK(m_trackSegmentAssociationTool.retrieve());
    else m_trackSegmentAssociationTool.disable();
    ATH_CHECK(m_trackQuery.retrieve());
    if(!m_momentumBalanceTool.empty()) ATH_CHECK(m_momentumBalanceTool.retrieve());
    else m_momentumBalanceTool.disable();
    if(!m_scatteringAngleTool.empty()) ATH_CHECK(m_scatteringAngleTool.retrieve());   
    else m_scatteringAngleTool.disable();
    if(!m_selectorTool.empty()) ATH_CHECK(m_selectorTool.retrieve());
    else m_selectorTool.disable();
    if(!m_meanMDTdADCTool.empty()) ATH_CHECK(m_meanMDTdADCTool.retrieve());
    else m_meanMDTdADCTool.disable();

    ATH_CHECK( m_caloNoiseKey.initialize (SG::AllowEmpty) );

    ATH_MSG_INFO("ET_Core calculation: doNoiseCut, sigma - " 
                 << !m_caloNoiseKey.empty() << " " << m_sigmaCaloNoiseCut);

    if(!m_doSA){
      ATH_CHECK(m_caloMaterialProvider.retrieve());
      ATH_CHECK(m_propagator.retrieve());
    }
    else{
      m_caloMaterialProvider.disable();
      m_propagator.disable();
    }

    ATH_CHECK(m_cellContainerName.initialize(m_useCaloCells));

    return StatusCode::SUCCESS;
  }

  void MuonCreatorTool::create( const MuonCandidateCollection* muonCandidates,  const InDetCandidateCollection* inDetCandidates, std::vector<const InDetCandidateToTagMap*> tagMaps, 
				OutputData& outputData ) const {

    unsigned int numIdCan = inDetCandidates ? inDetCandidates->size() : 0;
    unsigned int numMuCan = muonCandidates  ? muonCandidates->size() : 0;
    ATH_MSG_DEBUG("Creating xAOD::Muons from: " << numIdCan << " indet candidates and " << numMuCan << " muon candidates ");
    
    // Add RPC timing information to all MS tracks
    if( m_fillTimingInformation && muonCandidates ) 
      for (auto candidate : *muonCandidates) 
	if(candidate->muonSpectrometerTrackLink().isValid())
	  addRpcTiming(**(candidate->muonSpectrometerTrackLink()));    
    
    // Create containers for resolved candidates (always of type VIEW_ELEMENTS)
    InDetCandidateTagsMap resolvedInDetCandidates;
    //std::vector<const MuonCombined::InDetCandidate*> resolvedInDetCandidates;
    std::vector<const MuonCombined::MuonCandidate*> resolvedMuonCandidates;
    
    // Resolve Overlap
    if (!m_buildStauContainer) resolveOverlaps(inDetCandidates, muonCandidates, tagMaps, resolvedInDetCandidates, resolvedMuonCandidates);    
    else selectStaus(inDetCandidates, resolvedInDetCandidates, tagMaps);

    if( inDetCandidates ) ATH_MSG_DEBUG("InDetCandidates : overlap removal " << inDetCandidates->size() << " in, " <<resolvedInDetCandidates.size() <<" out");
    if( !m_buildStauContainer && muonCandidates  ) ATH_MSG_DEBUG("MuonCandidates  : overlap removal " << muonCandidates->size() << " in, " <<resolvedMuonCandidates.size() <<" out");

    // Create a container for resolved candidates (always of type VIEW_ELEMENTS)
    for( InDetCandidateTags can : resolvedInDetCandidates ) {
      ATH_MSG_DEBUG("New InDetCandidate");
      xAOD::Muon* muon = create(can,outputData);
      if( !muon ) { 
        ATH_MSG_DEBUG("no muon found");
      }else {
        ATH_MSG_DEBUG("muon found");
        // checkMuon(*muon);
        if (!muon->primaryTrackParticleLink().isValid()) {
          ATH_MSG_ERROR("This muon has no valid primaryTrackParticleLink! Author="<<muon->author());
        }
      }
      ATH_MSG_DEBUG("Creation of Muon from InDetCandidates done");
    }
    for( auto can : resolvedMuonCandidates )  {
      ATH_MSG_DEBUG("New MuonCandidate");
      create(*can,outputData);
      ATH_MSG_DEBUG("Creation of Muon from MuonCandidates done");
    }

    // Add alignment effects on tracks to ME and CB tracks
    if( m_fillAlignmentEffectsOnTrack ) {
      if(outputData.combinedTrackParticleContainer) addAlignmentEffectsOnTrack(outputData.combinedTrackParticleContainer);
      if(outputData.extrapolatedTrackParticleContainer) addAlignmentEffectsOnTrack(outputData.extrapolatedTrackParticleContainer);
      if(outputData.msOnlyExtrapolatedTrackParticleContainer) addAlignmentEffectsOnTrack(outputData.msOnlyExtrapolatedTrackParticleContainer);
      for(auto mu: *outputData.muonContainer){
	if(mu->primaryTrackParticle()==mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)) continue; //no CT or ST muons
	if(mu->primaryTrackParticle()==mu->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle)) continue; //no SA muons w/o ME tracks
	const xAOD::TrackParticle* ptp=mu->primaryTrackParticle();
	uint8_t prec=0; //all precision layers
	mu->summaryValue(prec,xAOD::numberOfPrecisionLayers);
	int nTotPrec=(int)prec,nGoodPrec=0,nBadPrec=0,nBadBar=0,nBadEnd=0,nBadSmall=0,nBadLarge=0,nGoodBar=0,nGoodEnd=0,nGoodLarge=0,nGoodSmall=0;
	std::vector< std::vector<unsigned int> > chIds=ptp->auxdata<std::vector< std::vector<unsigned int> > >("alignEffectChId");
	std::vector<float> alignEffSDT=ptp->auxdata<std::vector<float> >("alignEffectSigmaDeltaTrans");
	std::map<Muon::MuonStationIndex::ChIndex,int> chamberQual; //1=good, 2=bad; for choosing large/small
	for(unsigned int i=0;i<chIds.size();i++){
	  for(unsigned int j=0;j<chIds[i].size();j++){
	    Muon::MuonStationIndex::ChIndex currInd=(Muon::MuonStationIndex::ChIndex)chIds[i][j];
	    if(alignEffSDT[i]>=0.5){
	      if((chamberQual.count(currInd) && chIds[i].size()>1) || !chamberQual.count(currInd)){
		//either we haven't seen this chamber before, or we have but now we're in a sub-vector that's not just this chamber
		chamberQual[currInd]=2;
	      }
	    }
	    else{
	      if((chamberQual.count(currInd) && chIds[i].size()>1) || !chamberQual.count(currInd)){
                //either we haven't seen this chamber before, or we have but now we're in a sub-vector that's not just this chamber
                chamberQual[currInd]=1;
	      }
	    }
	  }
	}
	for(std::map<Muon::MuonStationIndex::ChIndex,int>::iterator it=chamberQual.begin();it!=chamberQual.end();++it){
	  int chnum=(unsigned int)((*it).first);
	  if((*it).second==2){
	    nBadPrec++;
	    if(chnum<7){
	      nBadBar++;
	      if(chnum%2==0) nBadSmall++;
	      else nBadLarge++;
	    }
	    else{
	      nBadEnd++;
	      if(chnum%2==0) nBadLarge++;
              else nBadSmall++;
	    }
	  }
	  else{
	    if(chnum<7){ 
	      nGoodBar++;
	      if(chnum%2==0) nGoodSmall++;
	      else nGoodLarge++;
	    }
            else{
	      nGoodEnd++;
	      if(chnum%2==0) nGoodLarge++;
              else nGoodSmall++;
            }
          }
	}
	int isSmall=0,isEnd=0;
	bool countHits=false;
	if((nTotPrec-nBadPrec)>nBadPrec){
	  nGoodPrec=nTotPrec-nBadPrec;
	  if(nGoodEnd>nGoodBar) isEnd=1; //arbitrarily setting it to barrel if they're equal for now
	  if(nGoodSmall>nGoodLarge) isSmall=1;
	  if(nGoodSmall==nGoodLarge) countHits=true;
	}
	else{
	  nGoodPrec=nBadPrec;
	  if((nTotPrec-nBadPrec)==nBadPrec){ //if equal, set to whichever has the smaller error, right?
	    if(nGoodEnd>nGoodBar) isEnd=1;
	    if(nGoodSmall>nGoodLarge) isSmall=1;
	    if(nGoodSmall==nGoodLarge) countHits=true;
	  }
	  else{
	    if(nBadEnd>nBadBar) isEnd=1;
	    if(nBadSmall>nBadLarge) isSmall=1;
	    if(nBadSmall==nBadLarge) countHits=true;
	  }
	}
	//as we arbitrarily declare this to be a barrel track if there are equal numbers of good barrel and endcap chambers, we need not worry about that situation
	if(countHits){ //decide large-small by counting hits
	  uint8_t sumval=0;
	  int nSmallHits=0,nLargeHits=0;
	  mu->summaryValue(sumval,xAOD::innerSmallHits);
          nSmallHits+=(int)sumval;
          sumval=0;
	  mu->summaryValue(sumval,xAOD::middleSmallHits);
          nSmallHits+=(int)sumval;
          sumval=0;
	  mu->summaryValue(sumval,xAOD::outerSmallHits);
          nSmallHits+=(int)sumval;
	  sumval=0;
	  mu->summaryValue(sumval,xAOD::extendedSmallHits);
          nSmallHits+=(int)sumval;
	  sumval=0;
	  mu->summaryValue(sumval,xAOD::innerLargeHits);
          nLargeHits+=(int)sumval;
	  sumval=0;
          mu->summaryValue(sumval,xAOD::middleLargeHits);
          nLargeHits+=(int)sumval;
	  sumval=0;
          mu->summaryValue(sumval,xAOD::outerLargeHits);
          nLargeHits+=(int)sumval;
	  sumval=0;
          mu->summaryValue(sumval,xAOD::extendedLargeHits);
          nLargeHits+=(int)sumval;
	  if(nSmallHits>nLargeHits) isSmall=1;
	}
	mu->setSummaryValue(nGoodPrec,xAOD::numberOfGoodPrecisionLayers);
	mu->setSummaryValue(isSmall,xAOD::isSmallGoodSectors);
	mu->setSummaryValue(isEnd,xAOD::isEndcapGoodLayers);
      }
    }

    if( msgLvl(MSG::DEBUG) || m_printSummary ){    
      ATH_MSG_INFO("Printing muon container:");
      ATH_MSG_INFO(m_muonPrinter->print(*outputData.muonContainer));
      ATH_MSG_INFO("Done");
    }
    if( msgLvl(MSG::VERBOSE) && outputData.clusterContainer ){    
      ATH_MSG_VERBOSE("Associated clusters : " << outputData.clusterContainer->size());
    }

  }

  xAOD::Muon* MuonCreatorTool::create( const MuonCandidate& candidate, OutputData& outputData ) const {

    // skip all muons without extrapolated track
    if( !candidate.extrapolatedTrack() ) {
      ATH_MSG_DEBUG("MuonCreatorTool::create(...) No extrapolated track - aborting. Will not create Muon.");
      return 0; // Do we really want to do this?
    }
    
    // Create the xAOD object:
    xAOD::Muon* muon = new xAOD::Muon();
    outputData.muonContainer->push_back( muon );

    muon->setAuthor(xAOD::Muon::MuidSA);
    muon->setMuonType(xAOD::Muon::MuonStandAlone);
    muon->addAllAuthor(xAOD::Muon::MuidSA);

    // create candidate from SA muon only
    addMuonCandidate(candidate,*muon,outputData);

    if(!muon->extrapolatedMuonSpectrometerTrackParticleLink().isValid()){
      ATH_MSG_DEBUG("Creation of track particle for SA muon failed, removing it");
      outputData.muonContainer->pop_back();
      return 0;
    }
    
    if( !dressMuon(*muon) ){
      ATH_MSG_WARNING("Failed to dress muon");
      outputData.muonContainer->pop_back();
      return 0;
    }

    //make sure we can extrapolate the track back through the calo, otherwise it's not a muon
    //shouldn't above requirement that an extrapolated track exist do this, though?
    //difference between different ways of extrapolating probably needs to be investigated
    //the muons that are rejected by this all seem to be useless low-pT SA muons with 2 precision layers that would never make it into analyses, though
    std::unique_ptr<Trk::CaloExtension> caloExtension = m_caloExtTool->caloExtension(**muon->extrapolatedMuonSpectrometerTrackParticleLink());
    if(!caloExtension){
      ATH_MSG_DEBUG("failed to get a calo extension for this SA muon, discard it");
      outputData.muonContainer->pop_back();
      return 0;
    }
    if( caloExtension->caloLayerIntersections().empty()){
      ATH_MSG_DEBUG("failed to retrieve any calo layers for this SA muon, discard it");
      outputData.muonContainer->pop_back();
      return 0;
    }

    // check if there is a cluster container, if yes collect the cells around the muon and fill
    // Etcore variables for muon
    if(m_useCaloCells) collectCells(*muon,outputData.clusterContainer);

    return muon;
  }


  xAOD::Muon* MuonCreatorTool::create( InDetCandidateTags& candidate, OutputData& outputData ) const {

    // no tags, no muon
    if( candidate.second.empty() ) {
      ATH_MSG_DEBUG("MuonCreatorTool::create(...) - InDetCandidate with empty combinedDataTags. Aborting. Will not create Muon.");
      return 0;
    }
    
    if( !candidate.first->indetTrackParticleLink().isValid() ) {
      ATH_MSG_WARNING("MuonCreatorTool::create(...) : Have InDetCandidate with no valid link to an ID trackparticle. Aborting! Will not create Muon.");
      return 0;
    }
    
    std::vector<const TagBase*> tags = candidate.second;
    if(tags.size()==1 && !m_buildStauContainer){
      const MuGirlLowBetaTag* muGirlLowBetaTag = dynamic_cast<const MuGirlLowBetaTag*>(tags[0]);
      if (muGirlLowBetaTag){
        ATH_MSG_DEBUG("Track has only a MuGirlLowBetaTag but Staus are not being built, so will not create muon");
        return 0;
      }
    }
    
    // Create the xAOD object:
    xAOD::Muon* muon = new xAOD::Muon();
    outputData.muonContainer->push_back( muon );
    std::vector< ElementLink< xAOD::MuonSegmentContainer > > segments;
    muon->setMuonSegmentLinks(segments) ;
  
    // now we need to sort the tags to get the best muon
    
    // set the link to the ID track particle
    muon->setTrackParticleLink(xAOD::Muon::InnerDetectorTrackParticle, candidate.first->indetTrackParticleLink() );
    ATH_MSG_DEBUG("Adding InDet Track: pt " << (*candidate.first->indetTrackParticleLink())->pt() 
      << " eta " << (*candidate.first->indetTrackParticleLink())->eta() << " phi " << (*candidate.first->indetTrackParticleLink())->phi() );
    
    ATH_MSG_DEBUG("creating Muon with " << tags.size() << " tags ");
    // loop over the tags
    
    // Set variables to zero by calling the functions with null pointers.
    addCaloTag(*muon,0);
    addCombinedFit(*muon,0, outputData );
    addStatisticalCombination(*muon,*(candidate.first),0, outputData );
    addMuGirl(*muon,0, outputData );  
    addSegmentTag(*muon,0);
    
    bool first = true;
    for( auto tag : tags ){

      ATH_MSG_DEBUG("Handling tag: type " << tag->type() );

      // staus
      if( m_buildStauContainer ){

        const MuGirlLowBetaTag* muGirlLowBetaTag = dynamic_cast<const MuGirlLowBetaTag*>(tag);

        if( muGirlLowBetaTag ) {
          ATH_MSG_DEBUG("MuonCreatorTool MuGirlLowBetaTag ");

          muon->setAuthor(tag->author());
          muon->setMuonType(tag->type());
	  
          if (tag->type() == xAOD::Muon::Combined ){
            ATH_MSG_DEBUG("MuonCreatorTool MuGirlLowBetaTag combined");
    
            // Create the xAOD object:
            if( outputData.slowMuonContainer ) {
              xAOD::SlowMuon* slowMuon = new xAOD::SlowMuon();
              outputData.slowMuonContainer->push_back( slowMuon );

	      addMuGirlLowBeta(*muon,muGirlLowBetaTag,slowMuon, outputData ); // CHECK to see what variables are created here.

	      ATH_MSG_DEBUG("slowMuon muonContainer size "<<outputData.muonContainer->size());
	      ElementLink<xAOD::MuonContainer> muonLink(*outputData.muonContainer,outputData.muonContainer->size()-1);
	      if( slowMuon && muonLink.isValid() ) {

		ATH_MSG_DEBUG("slowMuon muonLink valid");
		slowMuon->setMuonLink(muonLink);
	      }
	    }
          }          
        }
      }else{

        //Don't want staus in muon branch
        const MuGirlLowBetaTag* muGirlLowBetaTag = dynamic_cast<const MuGirlLowBetaTag*>(tag);
        if (muGirlLowBetaTag) continue;
        
        // set author info 
        if( first ) {
          ATH_MSG_DEBUG("MuonCreatorTool first muon tag: author="<<tag->author()<<"  type="<<tag->type());
          muon->setAuthor(tag->author());
          muon->setMuonType(tag->type());
          // Overrride type if InDet track is SiAssociated.
          if (candidate.first->isSiliconAssociated() ) {
            muon->setMuonType(xAOD::Muon::SiliconAssociatedForwardMuon);
          }
          first = false;
        }

        muon->addAllAuthor(tag->author());
        
        // this is not too elegant, maybe rethink implementation
        xAOD::Muon::MuonType type = tag->type();
        if( type == xAOD::Muon::Combined ){
          // work out type of tag
          const CombinedFitTag* cbFitTag = dynamic_cast<const CombinedFitTag*>(tag);
          const StacoTag* stacoTag = dynamic_cast<const StacoTag*>(tag);
          const MuGirlTag* muGirlTag = dynamic_cast<const MuGirlTag*>(tag);
          
          addCombinedFit(*muon,cbFitTag, outputData );
          addStatisticalCombination(*muon,*(candidate.first),stacoTag, outputData );
          addMuGirl(*muon,muGirlTag, outputData );          
          if (! (cbFitTag||stacoTag||muGirlTag) ) {
            ATH_MSG_WARNING("Unknown combined tag ");
          }
          
        } else if( type == xAOD::Muon::SegmentTagged ){
          const SegmentTag* segTag = dynamic_cast<const SegmentTag*>(tag);
          const MuGirlTag* muGirlTag = dynamic_cast<const MuGirlTag*>(tag);
          
          addSegmentTag(*muon,segTag);
          addMuGirl(*muon,muGirlTag, outputData );
          
          if (! (segTag||muGirlTag) ) {
            ATH_MSG_WARNING("Unknown segment-tagged tag ");
          }
        }else if( type == xAOD::Muon::CaloTagged ){
          const CaloTag* caloTag = dynamic_cast<const CaloTag*>(tag);
          addCaloTag(*muon,caloTag);
          if( !caloTag ) {
            ATH_MSG_WARNING("Unknown calo tag type ");
          }
        } else {
          ATH_MSG_WARNING("Unknown tag type. Type= "+std::to_string(type));
        }
      }
    }// m_buildStauContainer

    if( !dressMuon(*muon) ){ 
      ATH_MSG_WARNING("Failed to dress muon");
      outputData.muonContainer->pop_back();
      return 0;
    }

    // If eLoss is not already available then build it
    float eLoss=-1;
    bool haveEloss=muon->parameter(eLoss,xAOD::Muon::EnergyLoss);
    if(!haveEloss || eLoss==0){
      ATH_MSG_DEBUG("Adding Energy Loss to muon");
      addEnergyLossToMuon(*muon);
      //ATH_MSG_DEBUG(m_muonPrinter->print(*muon));
    }
        
    // check if there is a cluster container, if yes collect the cells around the muon and fill
    // Etcore variables for muon
    if(m_useCaloCells) collectCells(*muon,outputData.clusterContainer);

    ATH_MSG_DEBUG("Done creating muon with "<<muon->auxdata<int>("nUnspoiledCscHits")<<" unspoiled csc hits");

    return muon;
  }

  void MuonCreatorTool::addStatisticalCombination( xAOD::Muon& muon, const InDetCandidate& candidate, const StacoTag* tag, OutputData& outputData ) const {
    if (!tag ){
      // init variables if necessary.
      muon.auxdata<float>("d0_staco")=-999;
      muon.auxdata<float>("z0_staco")=-999;
      muon.auxdata<float>("phi0_staco")=-999;
      muon.auxdata<float>("theta_staco")=-999;
      muon.auxdata<float>("qOverP_staco")=-999;
      muon.auxdata<float>("qOverPErr_staco")=-999;
      return;
    }
    
    ATH_MSG_DEBUG("Adding Staco Muon  " << tag->author() << " type " << tag->type());
    
    if( !muon.combinedTrackParticleLink().isValid() ){
      // create primary track particle

      // get summary
      const Trk::Track* idTrack = candidate.indetTrackParticle().track();
      const Trk::Track* msTrack = tag->muonCandidate().extrapolatedTrack() ? tag->muonCandidate().extrapolatedTrack() : &tag->muonCandidate().muonSpectrometerTrack();

      const Trk::TrackSummary* idSummary = idTrack ? idTrack->trackSummary() : 0;
      const Trk::TrackSummary* msSummary = msTrack->trackSummary();

      Trk::TrackSummary summary;
      if( idSummary ) summary += *idSummary;
      if( msSummary ) summary += *msSummary;

      Trk::FitQuality fq(  tag->matchChi2() ,5);
      Trk::TrackInfo info( msTrack->info() );
      // todo update patrec bit set adding ID values

            
      if( outputData.combinedTrackParticleContainer ){
        xAOD::TrackParticle* tp = m_particleCreator->createParticle(&tag->combinedParameters(),&fq,&info,&summary,
                                                                  std::vector<const Trk::TrackParameters*>(),
                                                                  std::vector<xAOD::ParameterPosition>(),
                                                                  xAOD::muon, outputData.combinedTrackParticleContainer);

        if( !tp ){
          ATH_MSG_WARNING("Failed to create track particle");
        }else{
          ElementLink<xAOD::TrackParticleContainer> link(*outputData.combinedTrackParticleContainer,outputData.combinedTrackParticleContainer->size()-1);
          if( link.isValid() ) {
            //link.toPersistent();
            ATH_MSG_DEBUG("Adding statistical combination: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
            muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
          }
          // for the purpose of the truth matching, set the track link to point to the ID track
          //tp->setTrackLink(candidate.indetTrackParticle().trackLink());
	  tp->setTrackLink( ElementLink< TrackCollection >() );
        }
      } //endif outputData.combinedTrackParticleContainer 
    }
    // add muon candidate
    addMuonCandidate(tag->muonCandidate(),muon,outputData);
     
    // Add inner match chi^2
    muon.setParameter(5,   xAOD::Muon::msInnerMatchDOF);
    muon.setParameter(static_cast<float>(tag->matchChi2()),  xAOD::Muon::msInnerMatchChi2);

    //STACO parameters added as auxdata
    muon.auxdata<float>("d0_staco")=tag->combinedParameters().parameters()[Trk::d0];
    muon.auxdata<float>("z0_staco")=tag->combinedParameters().parameters()[Trk::z0];
    muon.auxdata<float>("phi0_staco")=tag->combinedParameters().parameters()[Trk::phi0];
    muon.auxdata<float>("theta_staco")=tag->combinedParameters().parameters()[Trk::theta];
    muon.auxdata<float>("qOverP_staco")=tag->combinedParameters().parameters()[Trk::qOverP];
    muon.auxdata<float>("qOverPErr_staco")=sqrt((*(tag->combinedParameters().covariance()))(Trk::qOverP,Trk::qOverP));
    
    ATH_MSG_DEBUG("Done adding Staco Muon  " << tag->author() << " type " << tag->type());
  }

  void MuonCreatorTool::addCombinedFit( xAOD::Muon& muon, const CombinedFitTag* tag, OutputData& outputData ) const {
    if (!tag){
      // init variables if necessary.
      return;
    }
    
    ATH_MSG_DEBUG("Adding Combined fit Muon  " << tag->author() << " type " << tag->type());
    if( !muon.combinedTrackParticleLink().isValid() ){
      // if the combined track particle is part of a container set the link
      if( outputData.combinedTrackParticleContainer ){

        // create element link from the track
        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( tag->combinedTrackLink(),
                                                                                         *outputData.combinedTrackParticleContainer,
											 outputData.combinedTrackCollection);

        if( link.isValid() ) {
          //link.toPersistent();
          ATH_MSG_DEBUG("Adding combined fit: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
          muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
        }
        else ATH_MSG_WARNING("Creating of Combined TrackParticle Link failed");
      }
    }

    // add muon candidate
    addMuonCandidate(tag->muonCandidate(),muon,outputData,tag->updatedExtrapolatedTrackLink());

    // Add inner match chi^2
    muon.setParameter(static_cast<int>(tag->matchDoF()),     xAOD::Muon::msInnerMatchDOF);
    muon.setParameter(static_cast<float>(tag->matchChi2()),  xAOD::Muon::msInnerMatchChi2);

    ATH_MSG_DEBUG("Done adding Combined Fit Muon  " << tag->author() << " type " << tag->type());

  }

  void MuonCreatorTool::addMuGirlLowBeta( xAOD::Muon& muon, const MuGirlLowBetaTag* tag, xAOD::SlowMuon* slowMuon, OutputData& outputData ) const {
    if (!tag){
      // init variables if necessary.
      
      return;
    }
    
    ATH_MSG_DEBUG("Adding MuGirlLowBeta Muon  " << tag->author() << " type " << tag->type());
    
    //get stauExtras and write to slowMuon
    const MuGirlNS::StauExtras* stauExtras = tag->getStauExtras();
    if( slowMuon && stauExtras ){

      ATH_MSG_DEBUG("StauSummary beta "<<stauExtras->betaAll);
      slowMuon->setBeta(stauExtras->betaAll);
      slowMuon->setBetaT(stauExtras->betaAllt);
      slowMuon->setAnn(stauExtras->ann);
      slowMuon->setNRpcHits(stauExtras->numRpcHitsInSeg);
      slowMuon->setNTileCells(stauExtras->numCaloCells);
      slowMuon->setRpcInfo(stauExtras->rpcBetaAvg,
                           stauExtras->rpcBetaRms,
                           stauExtras->rpcBetaChi2,
                           stauExtras->rpcBetaDof);
      slowMuon->setMdtInfo(stauExtras->mdtBetaAvg,
                           stauExtras->mdtBetaRms,
                           stauExtras->mdtBetaChi2,
                           stauExtras->mdtBetaDof);
      slowMuon->setCaloInfo(stauExtras->caloBetaAvg,
                            stauExtras->caloBetaRms,
                            stauExtras->caloBetaChi2,
                            stauExtras->caloBetaDof);
      std::vector<uint8_t> eTechVec;
      std::vector<unsigned int> idVec;
      std::vector<float> mToFVec;
      std::vector<float> xVec;
      std::vector<float> yVec;
      std::vector<float> zVec;
      std::vector<float> eVec;
      std::vector<float> errorVec;
      std::vector<float> shiftVec;
      std::vector<float> propagationTimeVec;

      for( const auto& hit : stauExtras->hits ){
        eTechVec.push_back( hit.eTech );
        idVec.push_back(hit.id.get_identifier32().get_compact());
        mToFVec.push_back(hit.mToF);
        xVec.push_back(hit.x);
        yVec.push_back(hit.y);
        zVec.push_back(hit.z);
        eVec.push_back(hit.e);
        errorVec.push_back(hit.error);
        shiftVec.push_back(hit.shift);
        propagationTimeVec.push_back(hit.propagationTime);
      }
      slowMuon->auxdata< std::vector<uint8_t> >("hitTechnology") = eTechVec;
      slowMuon->auxdata< std::vector<unsigned int> >("hitIdentifier") = idVec;
      slowMuon->auxdata< std::vector<float> >("hitTOF") = mToFVec;
      slowMuon->auxdata< std::vector<float> >("hitPositionX") = xVec;
      slowMuon->auxdata< std::vector<float> >("hitPositionY") = yVec;
      slowMuon->auxdata< std::vector<float> >("hitPositionZ") = zVec;
      slowMuon->auxdata< std::vector<float> >("hitEnergy") = eVec;
      slowMuon->auxdata< std::vector<float> >("hitError") = errorVec;
      slowMuon->auxdata< std::vector<float> >("hitShift") = shiftVec;
      slowMuon->auxdata< std::vector<float> >("hitPropagationTime") = propagationTimeVec;
    }

    if( !muon.combinedTrackParticleLink().isValid() && tag->combinedTrack()){  
      
      // if the combined track particle is part of a container set the link
      if( outputData.combinedTrackParticleContainer ){
        // create element link 
        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( tag->combinedTrackLink(),
                                                                                         *outputData.combinedTrackParticleContainer,
											 outputData.combinedTrackCollection);                    
        
        if( link.isValid() ) {
          //link.toPersistent();
          ATH_MSG_DEBUG("Adding MuGirlLowBeta: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
          muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
        }
        else ATH_MSG_WARNING("Creating of MuGirlLowBeta TrackParticle Link failed");
      } 
    }
    
    if (outputData.xaodSegmentContainer){
      ATH_MSG_DEBUG( "Adding MuGirlLowBeta muonSegmentColection");
      
      std::vector< ElementLink< xAOD::MuonSegmentContainer > > segments;
      for( const auto& seglink : tag->segments() ){ 
	
        ElementLink<xAOD::MuonSegmentContainer> link = createMuonSegmentElementLink(seglink,
                                                                                    *outputData.xaodSegmentContainer,
										    outputData.muonSegmentCollection);
        if( link.isValid() ) {
          //link.toPersistent();
          segments.push_back(link);
          ATH_MSG_DEBUG("Adding MuGirlLowBeta: xaod::MuonSegment px " << (*link)->px() << " py " << (*link)->py() << " pz " << (*link)->pz() ); 
        }
        else ATH_MSG_WARNING("Creating of MuGirlLowBeta segment Link failed");         	
      }
      muon.setMuonSegmentLinks(segments);  
    }
  }
  
  void MuonCreatorTool::addMuGirl( xAOD::Muon& muon, const MuGirlTag* tag, OutputData& outputData ) const {
    if (!tag){
      // init variables if necessary.
      
      return;
    }
    
    ATH_MSG_DEBUG("Adding MuGirl Muon  " << tag->author() << " type " << tag->type());

    if( !muon.combinedTrackParticleLink().isValid() && tag->combinedTrack()){
      // if the combined track particle is part of a container set the link
      if( outputData.combinedTrackParticleContainer ){
        
        // create element link 
        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( tag->combinedTrackLink(),
                                                                                         *outputData.combinedTrackParticleContainer,
											 outputData.combinedTrackCollection);
        
        if( link.isValid() ) {
          //link.toPersistent();
          ATH_MSG_DEBUG("Adding MuGirl: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
          muon.setTrackParticleLink(xAOD::Muon::CombinedTrackParticle, link);
        }
        else ATH_MSG_WARNING("Creating of MuGirl TrackParticle Link failed");
      }

      if( outputData.extrapolatedTrackParticleContainer &&  tag->updatedExtrapolatedTrack()){
        // create element link 
        ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( tag->updatedExtrapolatedTrackLink(),
                                                                                         *outputData.extrapolatedTrackParticleContainer,
											 outputData.extrapolatedTrackCollection);
                
        if( link.isValid() ) {
          //link.toPersistent();
          ATH_MSG_DEBUG("Adding MuGirl: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
          muon.setTrackParticleLink(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle, link );
        }
        else ATH_MSG_WARNING("Creating of MuGirl TrackParticle Link failed");
      }     
    }

    if (outputData.xaodSegmentContainer){
      ATH_MSG_DEBUG( "Adding MuGirl muonSegmentCollection");

      std::vector< ElementLink< xAOD::MuonSegmentContainer > > segments;
      for( const auto& segLink : tag->segments() ){ 
	
        ElementLink<xAOD::MuonSegmentContainer> link = createMuonSegmentElementLink(segLink,
                                                                                    *outputData.xaodSegmentContainer,
										    outputData.muonSegmentCollection);
        if( link.isValid() ) {
          //link.toPersistent();
          segments.push_back(link);
          ATH_MSG_DEBUG("Adding MuGirl: xaod::MuonSegment px " << (*link)->px() << " py " << (*link)->py() << " pz " << (*link)->pz() ); 
        }
        else ATH_MSG_WARNING("Creating of MuGirl segment Link failed");         
      }
      muon.setMuonSegmentLinks(segments);
    }  
    ATH_MSG_DEBUG("Done Adding MuGirl Muon  " << tag->author() << " type " << tag->type());    
  }

  void MuonCreatorTool::addSegmentTag( xAOD::Muon& muon, const SegmentTag* tag ) const {
    if (!tag){
      // init variables if necessary.
      muon.setParameter(static_cast<float>(-1.0),xAOD::Muon::segmentDeltaEta);
      muon.setParameter(static_cast<float>(-1.0),xAOD::Muon::segmentDeltaPhi);
      muon.setParameter(static_cast<float>(-1.0),xAOD::Muon::segmentChi2OverDoF);
      return;
    }
    
    ATH_MSG_DEBUG("Adding Segment Tag Muon  " << tag->author() << " type " << tag->type());
    
    std::vector< ElementLink< xAOD::MuonSegmentContainer > > segments;
    bool foundseg=false;
    for( const auto& info : tag->segmentsInfo() ){
      if( info.link.isValid()){
        if(muon.author()==xAOD::Muon::MuTagIMO) segments.push_back(info.link); //non-segment-tagged muons get their list of segments in a different way
	if(!foundseg){ //add parameters for the first segment
          muon.setParameter(static_cast<float>(info.dtheta),xAOD::Muon::segmentDeltaEta);
          muon.setParameter(static_cast<float>(info.dphi),xAOD::Muon::segmentDeltaPhi);
          muon.setParameter(static_cast<float>(info.segment->fitQuality()->chiSquared()/info.segment->fitQuality()->numberDoF()),xAOD::Muon::segmentChi2OverDoF);
	  foundseg=true;
	}
	else if(muon.author()!=xAOD::Muon::MuTagIMO) break; //for non-segment-tagged muons, we only need to set the above parameters
      }
    }
    if(muon.author()==xAOD::Muon::MuTagIMO) muon.setMuonSegmentLinks(segments) ; //set the associated segments
  }

  void MuonCreatorTool::addCaloTag( xAOD::Muon& mu, const CaloTag* tag ) const {
    
    if (!tag){
      // init variables if necessary.
      
      mu.setParameter(static_cast<float>( 0.0 ), xAOD::Muon::CaloLRLikelihood);
      mu.setParameter(static_cast<int>( 0xFF ), xAOD::Muon::CaloMuonIDTag); 
      if ( m_fillExtraELossInfo) {
        // Here we can make sure that we store the extra calotag information - just always add it since this is then unambigious for debugging
        mu.auxdata< float >("CT_ET_Core")   = 0.0;
        mu.auxdata< float   >("CT_EL_Type")   = -999.0; // FIXME - should be uint
        mu.auxdata< float >("CT_ET_LRLikelihood")   = -999.0;
        mu.auxdata< float >("CT_ET_FSRCandidateEnergy")   = -999.0;
      }
      return; 
    }
    
    ATH_MSG_DEBUG("Adding Calo Muon  " << tag->author() << " type " << tag->type());
    mu.setParameter(static_cast<float>( tag->caloLRLikelihood() ), xAOD::Muon::CaloLRLikelihood);
    mu.setParameter(static_cast<int>( tag->caloMuonIdTag() ), xAOD::Muon::CaloMuonIDTag); 
    
    if ( m_fillExtraELossInfo) {
      // Here we can make sure that we store the extra calotag information - just always add it since this is then unambigious for debugging
      mu.auxdata< float >("CT_ET_Core")   = tag->etCore();
      mu.auxdata< float >("CT_EL_Type")   = tag->energyLossType(); // FIXME - should be uint
      mu.auxdata< float >("CT_ET_LRLikelihood")   = tag->caloLRLikelihood();
      mu.auxdata< float >("CT_ET_FSRCandidateEnergy")   = tag->fsrCandidateEnergy();
    }
    // FIXME - calo deposits
  }
  
  ElementLink<xAOD::TrackParticleContainer> MuonCreatorTool::createTrackParticleElementLink( ElementLink<TrackCollection> trackLink,
                                                                                             xAOD::TrackParticleContainer& trackParticleContainer,
											     TrackCollection* trackCollection) const {
    ATH_MSG_DEBUG("createTrackParticleElementLink");
    const xAOD::TrackParticle* tp = 0;
    if( trackCollection ){
      trackCollection->push_back(new Trk::Track(**trackLink));
      //want to link the track particle to this track
      ElementLink<TrackCollection> link( *trackCollection,trackCollection->size()-1);
      if(link.isValid()) tp = m_particleCreator->createParticle( link, &trackParticleContainer, 0, xAOD::muon );
      else ATH_MSG_WARNING("new Track Collection link invalid");
    }
    if(!tp){
      // create extrapolated track particle without a link to the track
      tp = m_particleCreator->createParticle( **trackLink, &trackParticleContainer, 0, xAOD::muon );
    }

    if( tp ){
      ElementLink<xAOD::TrackParticleContainer> link( trackParticleContainer,trackParticleContainer.size()-1);
      return link;
    }
    return ElementLink<xAOD::TrackParticleContainer>();
  }

  ElementLink<xAOD::MuonSegmentContainer> MuonCreatorTool::createMuonSegmentElementLink( ElementLink<Trk::SegmentCollection> segLink,
                                                                                         xAOD::MuonSegmentContainer& xaodSegmentContainer,
											 Trk::SegmentCollection* muonSegmentCollection) const {
    const xAOD::MuonSegment* ms = 0;
    if( muonSegmentCollection ) {

      // if a muon segment collection is provided, duplicate the segment and create a link to that
      const Muon::MuonSegment* oldSeg=dynamic_cast<const Muon::MuonSegment*>(*segLink);
      muonSegmentCollection->push_back(new Muon::MuonSegment(*oldSeg));

      ElementLink<Trk::SegmentCollection> link( *muonSegmentCollection,muonSegmentCollection->size()-1);
      if( link.isValid() ){
        link.toPersistent();
        ms = m_muonSegmentConverterTool->convert(link, &xaodSegmentContainer);
      }
      else ATH_MSG_WARNING("new Segment Collection link invalid");
    }
    if( !ms ){
      // create muon segments without a link to the original
      const Muon::MuonSegment* seg=dynamic_cast<const Muon::MuonSegment*>(*segLink);
      ms = m_muonSegmentConverterTool->convert( *seg, &xaodSegmentContainer);
    }
    if( ms ){
      ElementLink<xAOD::MuonSegmentContainer> link( xaodSegmentContainer,xaodSegmentContainer.size()-1);
      link.toPersistent();
      return link;
    }
    return ElementLink<xAOD::MuonSegmentContainer>();
  }

  void MuonCreatorTool::addMuonCandidate( const MuonCandidate& candidate, xAOD::Muon& muon, OutputData& outputData, ElementLink<TrackCollection> meLink ) const {

    // only set once 
    if( muon.muonSpectrometerTrackParticleLink().isValid() ) {
      return;
    }

    // if(m_makeMSPreExtrapLink) {
    //   // copy over element link to the MS track before extrapolation
    //   muon.auxdata< ElementLink<xAOD::TrackParticleContainer> >("msTrackLink") = candidate.muonSpectrometerTrackLink();
    //   ATH_MSG_DEBUG("Added aux data element link, status = " << muon.auxdata< ElementLink<xAOD::TrackParticleContainer> >("msTrackLink").isValid());
    // }

    //case where we have a MuGirl muon that is also reconstructed by STACO: don't want to add this track as it is misleading
    //however, we will still keep the MS-only extrapolated track (see below) for debugging purposes
    if(muon.author()!=xAOD::Muon::MuGirl) muon.setTrackParticleLink(xAOD::Muon::MuonSpectrometerTrackParticle, candidate.muonSpectrometerTrackLink() );
    
    // we need both the container and the extrapolated muon track to add the link
    if( !outputData.extrapolatedTrackParticleContainer || (!candidate.extrapolatedTrack() && !meLink.isValid()) ) {
      return;
    }
    
    const Trk::Track* extrapolatedTrack = candidate.extrapolatedTrack();

    if (! extrapolatedTrack || !extrapolatedTrack->perigeeParameters()) {
      ATH_MSG_DEBUG("Something is wrong with this track.");
      if (!extrapolatedTrack) 
        ATH_MSG_WARNING("Track doesn't have extrapolated track. Skipping");
      if (extrapolatedTrack && !extrapolatedTrack->perigeeParameters()) 
        ATH_MSG_WARNING("Track doesn't have perigee parameters on extrapolated track. Skipping");
    } else {
      //Now we just add the original extrapolated track itself
      //but not for SA muons, for consistency they will still have extrapolatedTrackParticle
      if(muon.muonType()!=xAOD::Muon::MuonStandAlone){
	if(meLink.isValid()){ //add ME track and MS-only extrapolated track
	  if(outputData.msOnlyExtrapolatedTrackParticleContainer){ //add un-refitted extrapolated track as MS-only extrapolated track
	    ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( candidate.extrapolatedTrackLink(),
											     *outputData.msOnlyExtrapolatedTrackParticleContainer,
											     outputData.msOnlyExtrapolatedTrackCollection);
	    
	    if( link.isValid() ) {
	      ATH_MSG_DEBUG("Adding MS-only extrapolated track: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
	      //link.toPersistent();
	      muon.setTrackParticleLink(xAOD::Muon::MSOnlyExtrapolatedMuonSpectrometerTrackParticle, link );
	    }
	    else ATH_MSG_WARNING("failed to create MS-only extrapolated track particle");
	  }
	  //now add refitted track as ME track
	  ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( meLink,
											   *outputData.extrapolatedTrackParticleContainer,
											   outputData.extrapolatedTrackCollection);
	  if( link.isValid() ) {
	    ATH_MSG_DEBUG("Adding standalone fit (refitted): pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
	    muon.setTrackParticleLink(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle, link );
	    float fieldInt=m_trackQuery->fieldIntegral(**meLink).betweenSpectrometerMeasurements();
	    muon.setParameter(fieldInt,xAOD::Muon::spectrometerFieldIntegral);
	    int nunspoiled=(*link)->track()->trackSummary()->get(Trk::numberOfCscUnspoiltEtaHits);
	    muon.auxdata<int>("nUnspoiledCscHits")=nunspoiled;
	  }
	}
	else{ //no refitted track, so add original un-refitted extrapolated track as ME track
	  if(muon.author()==xAOD::Muon::MuGirl && muon.extrapolatedMuonSpectrometerTrackParticleLink().isValid()){
	    //MuGirl case: ME track is already set, but now we have the extrapolated track from the STACO tag
	    //add this as the MS-only extrapolated track instead
	    ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( candidate.extrapolatedTrackLink(),
											     *outputData.msOnlyExtrapolatedTrackParticleContainer,
											     outputData.msOnlyExtrapolatedTrackCollection);

	    if( link.isValid() ) {
	      ATH_MSG_DEBUG("Adding MS-only extrapolated track to MuGirl muon: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
	      //link.toPersistent();                                                                                                                                              
	      muon.setTrackParticleLink(xAOD::Muon::MSOnlyExtrapolatedMuonSpectrometerTrackParticle, link );
	      float fieldInt=m_trackQuery->fieldIntegral(**candidate.extrapolatedTrackLink()).betweenSpectrometerMeasurements();
	      muon.setParameter(fieldInt,xAOD::Muon::spectrometerFieldIntegral);
	    }
	  }
	  else{
	    ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( candidate.extrapolatedTrackLink(),
											     *outputData.extrapolatedTrackParticleContainer,
											     outputData.extrapolatedTrackCollection);
	    
	    if( link.isValid() ) {
	      ATH_MSG_DEBUG("Adding standalone fit (un-refitted): pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
	      //link.toPersistent();
	      muon.setTrackParticleLink(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle, link );
	      float fieldInt=m_trackQuery->fieldIntegral(**candidate.extrapolatedTrackLink()).betweenSpectrometerMeasurements();
	      muon.setParameter(fieldInt,xAOD::Muon::spectrometerFieldIntegral);
	    }
	  }
	}
      }
      else{ //SA tracks only get un-refitted track as ME track
	// create element link from the track
	ElementLink<xAOD::TrackParticleContainer> link = createTrackParticleElementLink( candidate.extrapolatedTrackLink(),
											 *outputData.extrapolatedTrackParticleContainer,
											 outputData.extrapolatedTrackCollection);
	
	if( link.isValid() ) {
	  ATH_MSG_DEBUG("Adding standalone fit: pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi() );
	  //link.toPersistent();
	  muon.setTrackParticleLink(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle, link );
	  float fieldInt=m_trackQuery->fieldIntegral(**candidate.extrapolatedTrackLink()).betweenSpectrometerMeasurements();
	  muon.setParameter(fieldInt,xAOD::Muon::spectrometerFieldIntegral);
	  int nunspoiled=extrapolatedTrack->trackSummary()->get(Trk::numberOfCscUnspoiltEtaHits);
	  muon.auxdata<int>("nUnspoiledCscHits")=nunspoiled;
	}
	else{
	  ATH_MSG_WARNING("failed to create ME track particle for SA muon");
	}
      }
    }
  }

  void MuonCreatorTool::selectStaus( const InDetCandidateCollection* inDetCandidates,
                                     InDetCandidateTagsMap& resolvedInDetCandidates, 
				     std::vector<const InDetCandidateToTagMap*> tagMaps) const {

    if( inDetCandidates ) {

      for( auto candidate : *inDetCandidates ) {
	std::vector<const TagBase*> tags;
	const TagBase* tag=tagMaps[0]->getTag(candidate); //in the staus case, just one tag map is present
	const MuGirlLowBetaTag* muGirlLowBetaTag = dynamic_cast<const MuGirlLowBetaTag*>(tag);
	if( muGirlLowBetaTag) {
	  tags.push_back(tag);
	  resolvedInDetCandidates.push_back(InDetCandidateTags(candidate,tags));
	  ATH_MSG_DEBUG("save ID candidate stau ");
        }
      }
    }
    
    // print-out
    if( msgLvl(MSG::DEBUG) && inDetCandidates ){
      ATH_MSG_DEBUG("ID candidates:  " << inDetCandidates->size() << "  after stau selection " << resolvedInDetCandidates.size() );
      for( auto candidate : resolvedInDetCandidates ){
        msg(MSG::DEBUG) << "ID candidate staus:  " << candidate.first->toString() << endmsg;
      }      
    }
  }

  void MuonCreatorTool::resolveOverlaps( const InDetCandidateCollection* inDetCandidates, const MuonCandidateCollection* muonCandidates, 
					 std::vector<const InDetCandidateToTagMap*> tagMaps,
					 InDetCandidateTagsMap& resolvedInDetCandidates, 
                                         std::vector<const MuonCombined::MuonCandidate*>& resolvedMuonCandidates) const
  {
    
    TrackCollection* resolvedTracks=0;
    std::set<size_t> alreadyIncluded;
    std::hash<const Trk::Track*> trackHash;
    //TrackCollection will hold copies of tracks used for overlap removal
    TrackCollection muonTracks;
    
    // the muons only found by the calo tagger should always be kept so we can filter them out from the start
    InDetCandidateTagsMap caloMuons;

    // Each InDetCandidate corresponds to a different ID track.
    // Resolve overlap among InDetCandidates for cases where different 
    // ID tracks are tagged by the same MS info (track or segment)
    if( inDetCandidates ) {

      // first loop over ID candidates and select all candidates that have a tag
      for( auto candidate : *inDetCandidates ){
	std::vector<const TagBase*> tags;
	for(auto map : tagMaps){
          if (map) {
            const TagBase* tag=map->getTag(candidate);
            if(tag) tags.push_back(tag);
          }
	}
        if( !tags.empty() ) {
          // sort the tags based on quality
          std::stable_sort(tags.begin(),tags.end(),SortTagBasePtr());
          if( tags.size() == 1 && tags.front()->type() == xAOD::Muon::CaloTagged ) caloMuons.push_back(InDetCandidateTags(candidate,tags));
	  else                                                                     resolvedInDetCandidates.push_back(InDetCandidateTags(candidate,tags));
	  
          if( msgLvl(MSG::DEBUG) && tags.size() >= 1 ) {
            msg(MSG::DEBUG) << " InDetCandidate ";
            for( auto tag : tags ) msg(MSG::DEBUG) << " - " << tag->toString() << " " << typeRank(tag->type()) << " " << authorRank(tag->author());
            msg(MSG::DEBUG)<< endmsg;
          }
        }
      }
    
      // now sort the selected ID candidates
      std::stable_sort(resolvedInDetCandidates.begin(),resolvedInDetCandidates.end(),SortInDetCandidates());
      if( msgLvl(MSG::DEBUG) ){
        ATH_MSG_DEBUG("ID candidates:  " << inDetCandidates->size() << "  after selection " << resolvedInDetCandidates.size() );
        for( auto candidate : resolvedInDetCandidates ){
          msg(MSG::DEBUG) << "ID candidate:  " << candidate.first->toString() << endmsg;
        }      
      }
      
      // a dummy track for segment tagged candidates to be used in the overlap check
      std::map<const Trk::Track*, InDetCandidateTags> trackInDetCandLinks;

      for(auto candidate : resolvedInDetCandidates) {
        // retrieve the primary tag
        const TagBase* primaryTag = candidate.second.at(0);
        if(!primaryTag) {
          ATH_MSG_WARNING("Unable to retrieve tag data for InDetCandidate");
          continue;
        }
        // check if a track is available
        if(primaryTag->primaryTrack()) {
          if( alreadyIncluded.count(trackHash(primaryTag->primaryTrack())) ){
            ATH_MSG_DEBUG("Duplicated ID candidate " << candidate.first->toString() );
            continue;
          }
          alreadyIncluded.insert(trackHash(primaryTag->primaryTrack()));
	  //create a copy for ambi processing to avoid const_cast
	  //all we care about in the end is getting the right candidates anyway
	  //muonTracks takes ownership of the memory
	  muonTracks.push_back(new Trk::Track(*primaryTag->primaryTrack()));
          trackInDetCandLinks[ muonTracks.back() ] = candidate;
        }
        // if not, make a dummy track out of segments, muonTracks takes ownership of the memory
        else{
          std::vector< const Muon::MuonSegment*> segments = primaryTag->associatedSegments();
          if( !segments.empty() && candidate.first->indetTrackParticle().trackLink().isValid() ){
	    muonTracks.push_back(createDummyTrack(primaryTag->associatedSegments(), *(candidate.first->indetTrackParticle().track())));
            trackInDetCandLinks[ muonTracks.back() ] = candidate;
          }
        }
      }
      
      // Resolve ambiguity between muon tracks
      resolvedTracks = m_ambiguityProcessor->process(&muonTracks); 
      resolvedInDetCandidates.clear();
      
      // link back to InDet candidates and fill the resolved container  
      for(auto track : *resolvedTracks) {
      
        auto trackCandLink = trackInDetCandLinks.find( track );
        if( trackCandLink==trackInDetCandLinks.end() ) {
          ATH_MSG_WARNING("Unable to find internal link between MS track and ID candidate!");
          continue;	
        }
	
        resolvedInDetCandidates.push_back( trackCandLink->second );
      }
      // add muons only found by calo tagger
      resolvedInDetCandidates.insert( resolvedInDetCandidates.end(), caloMuons.begin(), caloMuons.end() );
    }

    // print-out
    if( msgLvl(MSG::DEBUG) && inDetCandidates ){
      ATH_MSG_DEBUG("ID candidates:  " << inDetCandidates->size() << "  after ambiguity solving " << resolvedInDetCandidates.size() );
      for( auto candidate : resolvedInDetCandidates ){
        msg(MSG::DEBUG) << "ID candidate:  " << candidate.first->toString() << endmsg;
      }      
    }
    
    // MuonCandidateCollection contains all muon tracks (SA extrapolated or not)
    // Resolve overlap with InDetCandidate collection
    if( muonCandidates ) {

      if( msgLvl(MSG::DEBUG) ){
        ATH_MSG_DEBUG("Muon candidates:  " << muonCandidates->size() );
        for( auto candidate : *muonCandidates ){
          msg(MSG::DEBUG) << "Muon candidate:  " << candidate->toString() << endmsg;
        }      
      }

      
      if(!resolvedTracks) {
        resolvedTracks = new TrackCollection(SG::VIEW_ELEMENTS);
      }

      // add MS tracks to resolvedTrack collection and store a link between tracks and muon candidates
      std::map<const Trk::Track*, const MuonCandidate*> trackMuonCandLinks;
      for(auto candidate : *muonCandidates) {
        const Trk::Track* track = candidate->extrapolatedTrack() ? candidate->extrapolatedTrack() : &candidate->muonSpectrometerTrack();
        if( alreadyIncluded.count(trackHash(track)) ){
          ATH_MSG_DEBUG("Duplicate MS track " << m_printer->print(*track));
          continue;
        }
        alreadyIncluded.insert(trackHash(track));
	//again, create a copy of the track to use in ambiguity resolution
	//the important thing is to end up with the right candidates at the end anyway
	Trk::Track* copyTrack=new Trk::Track(*track);
	//this is a little awkward, but the ambi processing returns a VIEW_ELEMENTS collection
        resolvedTracks->push_back(copyTrack); //VIEW_ELEMENTS, pointer only
	muonTracks.push_back(copyTrack); //OWN_ELEMENTS, takes ownership
        trackMuonCandLinks[ copyTrack ] = candidate;
      }
      
      // solve ambiguity
      TrackCollection* resolvedAllTracks = m_ambiguityProcessor->process(resolvedTracks); 
      
      // loop over resolved tracks and fill resolved muon candidates
      for(auto track : *resolvedAllTracks) {
        auto trackCandLink = trackMuonCandLinks.find( track );
        if( trackCandLink != trackMuonCandLinks.end() ) 
          resolvedMuonCandidates.push_back( trackCandLink->second );
      }    
      if(resolvedTracks) delete resolvedTracks;
      if(resolvedAllTracks) delete resolvedAllTracks;

      // print-out
      if( msgLvl(MSG::DEBUG) ){
        ATH_MSG_DEBUG("Muon candidates:  " << muonCandidates->size() << "  after ambiguity solving " << resolvedMuonCandidates.size() );
        for( auto candidate : resolvedMuonCandidates ){
          msg(MSG::DEBUG) << "Muon candidate:  " << candidate->toString() << endmsg;
        }      
      }
    }
  }

 
  Trk::Track* MuonCreatorTool::createDummyTrack( std::vector<const Muon::MuonSegment*> segments, const Trk::Track& indetTrack ) const {

    ATH_MSG_VERBOSE("Creating dummy tracks from segments...");

    DataVector<const Trk::TrackStateOnSurface>*  trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

    for(auto seg : segments) {
      // create pars for muon and loop over hits
      double momentum = 1e8;
      double charge   = 0.;
      const Trk::TrackParameters* pars = m_edmHelperSvc->createTrackParameters( *seg, momentum, charge );
      std::vector<const Trk::MeasurementBase*>::const_iterator mit = seg->containedMeasurements().begin();
      std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = seg->containedMeasurements().end();
      for( ;mit!=mit_end;++mit ){
        const Trk::MeasurementBase& meas = **mit;  
        std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
        typePattern.set(Trk::TrackStateOnSurface::Measurement);
        const Trk::TrackParameters* exPars = m_propagator->propagateParameters(*pars,meas.associatedSurface(),
        Trk::anyDirection, false, Trk::MagneticFieldProperties(Trk::NoField));
        if(!exPars){
          ATH_MSG_VERBOSE("Could not propagate Track to segment surface");
        }
        const Trk::TrackStateOnSurface* trackState = new Trk::TrackStateOnSurface( meas.clone(), exPars, 0, 0, typePattern );
        trackStateOnSurfaces->push_back( trackState ); 
      } // end segment loop
      delete pars;
    }
    
    Trk::TrackInfo info(Trk::TrackInfo::Unknown,Trk::muon);
    Trk::TrackInfo::TrackPatternRecoInfo author = Trk::TrackInfo::MuTag;
    info.setPatternRecognitionInfo(author);
    Trk::Track* newtrack = new Trk::Track(info, trackStateOnSurfaces, (indetTrack.fitQuality())->clone());

    return newtrack;
  }

  bool MuonCreatorTool::dressMuon(  xAOD::Muon& muon  ) const {

    if( muon.primaryTrackParticleLink().isValid() ){
      const xAOD::TrackParticle* primary = muon.primaryTrackParticle();

      // update parameters with primary track particle
      setP4(muon,*primary);
      float qOverP = primary->qOverP();
      if (qOverP!=0.0){
        muon.setCharge(qOverP/std::fabs(qOverP));
        // try/catch didn't work...
      } else {
        ATH_MSG_WARNING("MuonCreatorTool::dressMuon - trying to set qOverP, but value from muon.primaryTrackParticle ["<<
          muon.primaryTrackParticleLink().dataID()<<"] is zero. Setting charge=0.0. The eta/phi of the muon is: "<<muon.eta()<<"/"<<muon.phi());
        muon.setCharge(0.0);
      } 
      /*
      if (m_fillMuonTruthLinks){
        // Add truth information
        // Check if primary has a truth link (if not, we need to do something different.)
        if (primary->isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" )) {
          // ATH_MSG_INFO("HAVE truth link on primary track particle ...");
        
          ElementLink<xAOD::TruthParticleContainer>& theLink =  muon.auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" );
          theLink =  primary->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" );
          muon.auxdata<int>("truthType") = primary->auxdata<int>("truthType");
          muon.auxdata<int>("truthOrigin") = primary->auxdata<int>("truthOrigin");
        } else {
          ATH_MSG_WARNING("No truth link on primary track particle of type="<<muon.muonType()<<" and author="<<muon.author()<<". Adding dummy link.");
          muon.auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" ) =  ElementLink<xAOD::TruthParticleContainer>();
          muon.auxdata<int>("truthType") = -99999;
          muon.auxdata<int>("truthOrigin") = -99999;
        }
      }
      */
    }else{
      ATH_MSG_DEBUG("No primary track particle set, deleting muon");
      return false;
    }

    // add hit summary
    m_muonDressingTool->addMuonHitSummary(muon);
    
    // calculate scattering significance and momentum balance significance
    if( !m_scatteringAngleTool.empty() ){
      Rec::ScatteringAngleSignificance scatSign = m_scatteringAngleTool->scatteringAngleSignificance(muon);
      float curvatureSignificance = scatSign.curvatureSignificance();
      muon.setParameter(curvatureSignificance,xAOD::Muon::scatteringCurvatureSignificance);
      float neighbourSignificance = scatSign.neighbourSignificance();
      muon.setParameter(neighbourSignificance,xAOD::Muon::scatteringNeighbourSignificance);
      ATH_MSG_VERBOSE("Got curvatureSignificance " << curvatureSignificance << "  and neighbourSignificance " << neighbourSignificance );
    }

    if( !m_momentumBalanceTool.empty() ){
      float momentumBalanceSignificance = m_momentumBalanceTool->momentumBalanceSignificance(muon);
      muon.setParameter(momentumBalanceSignificance,xAOD::Muon::momentumBalanceSignificance);
      ATH_MSG_VERBOSE("Got momentumBalanceSignificance " << momentumBalanceSignificance);
    }
 
    if( !m_meanMDTdADCTool.empty() ){
      float meanDeltaADC = float(m_meanMDTdADCTool->meanMDTdADCFiller(muon));
      muon.setParameter(meanDeltaADC,xAOD::Muon::meanDeltaADCCountsMDT);
      ATH_MSG_VERBOSE("Got meanDeltaADCCountsMDT " << meanDeltaADC);
    }
 
    if( !m_selectorTool.empty() ){
      muon.auxdata<float>("MuonSpectrometerPt")=muon.pt();
      muon.auxdata<float>("InnerDetectorPt")=muon.pt();
      // set id cuts
      m_selectorTool->setPassesIDCuts(muon);
      ATH_MSG_VERBOSE("Setting passesIDCuts " << muon.passesIDCuts() );
      
      // set quality
      m_selectorTool->setQuality(muon);
      ATH_MSG_VERBOSE("Setting Quality " << muon.quality() );
      
    }
    
    if (m_fillEnergyLossFromTrack){
      const Trk::Track* trk =0;
      if (muon.combinedTrackParticleLink ().isValid()){
         trk = (*(muon.combinedTrackParticleLink() ) )->track();
      }
      if (!trk && muon.extrapolatedMuonSpectrometerTrackParticleLink().isValid()) {
        trk = (*(muon.extrapolatedMuonSpectrometerTrackParticleLink() ) )->track();
      }
      if (trk) {
        fillEnergyLossFromTrack(muon, &(trk->trackStateOnSurfaces()->stdcont()));
      } else {
        fillEnergyLossFromTrack(muon, 0); // Just fill empty variables.
        ATH_MSG_VERBOSE("Couldn't find matching track which might have energy loss." );
      }
    }

    if( m_fillTimingInformationOnMuon  ) addRpcTiming(muon);
    
    if( !m_trackSegmentAssociationTool.empty() && muon.author()!=xAOD::Muon::MuTagIMO && muon.author()!=xAOD::Muon::MuGirl && 
	(muon.author()!=xAOD::Muon::MuGirlLowBeta || m_segLowBeta)) addSegmentsOnTrack(muon);

    addMSIDScatteringAngles(muon);
    if(muon.combinedTrackParticleLink().isValid()) addMSIDScatteringAngles(**(muon.combinedTrackParticleLink()));
    if(muon.extrapolatedMuonSpectrometerTrackParticleLink().isValid()) addMSIDScatteringAngles(**(muon.extrapolatedMuonSpectrometerTrackParticleLink()));
    if(muon.msOnlyExtrapolatedMuonSpectrometerTrackParticleLink().isValid()) addMSIDScatteringAngles(**(muon.msOnlyExtrapolatedMuonSpectrometerTrackParticleLink()));

    if(!muon.isAvailable<int>("nUnspoiledCscHits")){
      muon.auxdata<int>("nUnspoiledCscHits")=-999;
    }

    return true;
  }

  void MuonCreatorTool::getRpcTiming(const xAOD::TrackParticle& tp,
				     std::vector<unsigned int>& rpcHitIdentifier,
				     std::vector<float>& rpcHitPositionX,
				     std::vector<float>& rpcHitPositionY,
				     std::vector<float>& rpcHitPositionZ,
				     std::vector<float>& rpcHitTime) const
  {
    // loop over measurements
    const DataVector<const Trk::TrackStateOnSurface>& tsos = *(tp.track()->trackStateOnSurfaces());
    auto end = tsos.end();
    for( auto it = tsos.begin(); it!=end;++it ){
      
      // require measurement and track parameters 
      const Trk::MeasurementBase* meas = (*it)->measurementOnTrack();
      const Trk::TrackParameters* pars = (*it)->trackParameters();
      if( !meas || !pars ) continue;
      
      // only consider RPC hits
      Identifier mid = m_edmHelperSvc->getIdentifier(*meas);
      if( !m_idHelperSvc->isMuon(mid) || !m_idHelperSvc->isRpc(mid) ) continue;
      
      // lambda to add a hit
      auto addHit = [&]( const Trk::MeasurementBase& meas) { 
	const Muon::RpcClusterOnTrack* rot = dynamic_cast<const Muon::RpcClusterOnTrack*>(&meas);
	if( !rot ) return;
	
	rpcHitIdentifier.push_back(rot->identify().get_identifier32().get_compact());
	rpcHitPositionX.push_back(pars->position().x());
	rpcHitPositionY.push_back(pars->position().y());
	rpcHitPositionZ.push_back(pars->position().z());
	rpcHitTime.push_back(rot->time()); 
      };
      
      // in case competing rots loop over the contained rots
      const Muon::CompetingMuonClustersOnTrack* crot = dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(meas);
      if( crot ){
	for( unsigned int i=0;i<crot->numberOfContainedROTs();++i){
	  addHit(crot->rioOnTrack(i));
	}
      }else{
	addHit(*meas);
      }
    }    
  }		     
  
  void MuonCreatorTool::addRpcTiming( xAOD::Muon& muon ) const {

    // vectors to be filled
    std::vector<unsigned int> rpcHitIdentifier;
    std::vector<float> rpcHitPositionX;
    std::vector<float> rpcHitPositionY;
    std::vector<float> rpcHitPositionZ;
    std::vector<float> rpcHitTime;
    
    // only fill if the primary track particle is not equal to the ID track particle and 
    // it has an associated track with track states 
    const xAOD::TrackParticle* tp = muon.primaryTrackParticle();        
    if( tp && tp->track() && tp->track()->trackStateOnSurfaces() && tp != muon.trackParticle( xAOD::Muon::InnerDetectorTrackParticle) ){
      getRpcTiming(*tp,
		   rpcHitIdentifier,
		   rpcHitPositionX,
		   rpcHitPositionY,
		   rpcHitPositionZ,
		   rpcHitTime);
    }
    // always add vectors 
    muon.auxdata< std::vector<unsigned int> >("rpcHitIdentifier") = rpcHitIdentifier;
    muon.auxdata< std::vector<float> >("rpcHitPositionX")         = rpcHitPositionX;
    muon.auxdata< std::vector<float> >("rpcHitPositionY")         = rpcHitPositionY;
    muon.auxdata< std::vector<float> >("rpcHitPositionZ")         = rpcHitPositionZ;
    muon.auxdata< std::vector<float> >("rpcHitTime")              = rpcHitTime;
  }

  void MuonCreatorTool::addRpcTiming( const xAOD::TrackParticle& tp ) const {

    // vectors to be filled
    std::vector<unsigned int> rpcHitIdentifier;
    std::vector<float> rpcHitPositionX;
    std::vector<float> rpcHitPositionY;
    std::vector<float> rpcHitPositionZ;
    std::vector<float> rpcHitTime;
    
    // only fill if the  track particle has an associated track with track states 
    if( tp.track() && tp.track()->trackStateOnSurfaces() ){
      getRpcTiming(tp,
		   rpcHitIdentifier,
		   rpcHitPositionX,
		   rpcHitPositionY,
		   rpcHitPositionZ,
		   rpcHitTime);
    }
    // always add vectors 
    tp.auxdecor< std::vector<unsigned int> >("rpcHitIdentifier") = rpcHitIdentifier;
    tp.auxdecor< std::vector<float> >("rpcHitPositionX")         = rpcHitPositionX;
    tp.auxdecor< std::vector<float> >("rpcHitPositionY")         = rpcHitPositionY;
    tp.auxdecor< std::vector<float> >("rpcHitPositionZ")         = rpcHitPositionZ;
    tp.auxdecor< std::vector<float> >("rpcHitTime")              = rpcHitTime;
  }
  
  void MuonCreatorTool::addSegmentsOnTrack( xAOD::Muon& muon ) const {
    
    std::vector< ElementLink<xAOD::MuonSegmentContainer> > associatedSegments;
    if( !m_trackSegmentAssociationTool->associatedSegments(muon,associatedSegments) ){
      ATH_MSG_DEBUG("Failed to find associated segments ");
    }
    muon.setMuonSegmentLinks(associatedSegments) ;
    
  }

  void MuonCreatorTool::addEnergyLossToMuon( xAOD::Muon& muon ) const {

    if(!muon.inDetTrackParticleLink().isValid()) {
      ATH_MSG_WARNING("Missing ID track particle link in addEnergyLossToMuon!");
      return;
    }
    
    // get ID track particle
    const Trk::Track* trk = (*(muon.inDetTrackParticleLink()))->track();
    if(!trk) {
      ATH_MSG_WARNING("Missing ID trk::track in addEnergyLossToMuon!");
      return;    
    }

    // find last ID TSOS with track parameters
    const Trk::TrackStateOnSurface* lastID = nullptr;
    DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator it = trk->trackStateOnSurfaces()->rbegin();
    DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator itEnd = trk->trackStateOnSurfaces()->rend();
    for(; it!=itEnd; ++it) {
      if((*it)->trackParameters()) {
        lastID = *it;
        break;
      }
    }
    if(!lastID) {
      ATH_MSG_WARNING("Missing ID TSOS with track parameters in addEnergyLossToMuon!");
      return;        
    }
    
    // get calorimeter TSOS
    std::vector<const Trk::TrackStateOnSurface*>* caloTSOS = 
      m_caloMaterialProvider->getCaloTSOS(*((*it)->trackParameters()), *trk);
    
    if(!caloTSOS) {
      ATH_MSG_WARNING("Unable to find calo TSOS in addEnergyLossToMuon!");
      return;              
    }

    // fill muon parameters for eloss
    fillEnergyLossFromTrack(muon, caloTSOS);  
    
    // delete caloTSOS
    std::vector<const Trk::TrackStateOnSurface*>::const_iterator it2 = caloTSOS->begin();
    std::vector<const Trk::TrackStateOnSurface*>::const_iterator itEnd2 = caloTSOS->end();
    for (; it2 != itEnd2; ++it2) delete *it2;
    delete caloTSOS;

  }

  void MuonCreatorTool::fillEnergyLossFromTrack(xAOD::Muon& muon, const std::vector<const Trk::TrackStateOnSurface*>* tsosVector) const
  {    
    
    // Ensure these are set for every muon
    if (!tsosVector){
      muon.setParameter(static_cast<float>( 0.0 ), xAOD::Muon::EnergyLoss);
      muon.setParameter(static_cast<float>( 0.0 ), xAOD::Muon::ParamEnergyLoss);
      muon.setParameter(static_cast<float>( 0.0 ), xAOD::Muon::MeasEnergyLoss);
      muon.setParameter(static_cast<float>( 0.0 ), xAOD::Muon::EnergyLossSigma);
      muon.setParameter(static_cast<float>( 0.0 ), xAOD::Muon::MeasEnergyLossSigma);
      muon.setParameter(static_cast<float>( 0.0 ), xAOD::Muon::ParamEnergyLossSigmaPlus);
      muon.setParameter(static_cast<float>( 0.0 ), xAOD::Muon::ParamEnergyLossSigmaMinus);

      muon.setEnergyLossType( xAOD::Muon::Parametrized ); // Not so nice! Add 'unknown' type?
      muon.setParameter(static_cast<float>( 0.0 ), xAOD::Muon::FSR_CandidateEnergy);
      if (m_fillExtraELossInfo) muon.auxdata< unsigned int >("numEnergyLossPerTrack")   = 0;
      
      return;
    }
    
    float energyloss=0.0;
    unsigned int numEnergyLossPerTrack=0;
    bool problem=false;
    for (  auto tsos : *tsosVector  ){
      
      const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack () );
      if ( meot ){
      	const Trk::EnergyLoss* el = meot->energyLoss();
      	// tsos->type(Trk::TrackStateOnSurface::CaloDeposit)
      	if (false && el) {
      	  numEnergyLossPerTrack++;
      	  energyloss+=el->deltaE(); // FIXME - should we be summing these?
      	  if (el->deltaE()<0) {
      	    ATH_MSG_WARNING("Found negative energy loss (i.e. energy gain!) for this object: "<<(*el));
      	    problem=true;
      	  }
      	}
        
      	const CaloEnergy* caloEnergy = dynamic_cast<const CaloEnergy*>(el);
      	if (caloEnergy){
      	  numEnergyLossPerTrack++;
	  
      	  muon.setParameter(static_cast<float>( caloEnergy->deltaE() ),                xAOD::Muon::EnergyLoss);
      	  muon.setParameter(static_cast<float>( caloEnergy->deltaEParam() ),           xAOD::Muon::ParamEnergyLoss);
      	  muon.setParameter(static_cast<float>( caloEnergy->deltaEMeas() ),            xAOD::Muon::MeasEnergyLoss);
      	  muon.setParameter(static_cast<float>( caloEnergy->sigmaDeltaE() ),           xAOD::Muon::EnergyLossSigma);
      	  muon.setParameter(static_cast<float>( caloEnergy->sigmaDeltaEMeas() ),       xAOD::Muon::MeasEnergyLossSigma);
      	  muon.setParameter(static_cast<float>( caloEnergy->sigmaPlusDeltaEParam() ),  xAOD::Muon::ParamEnergyLossSigmaPlus);
      	  muon.setParameter(static_cast<float>( caloEnergy->sigmaMinusDeltaEParam() ), xAOD::Muon::ParamEnergyLossSigmaMinus);
	  
      	  muon.setEnergyLossType(static_cast<xAOD::Muon::EnergyLossType>( caloEnergy->energyLossType() ) );
      	  muon.setParameter(static_cast<float>( caloEnergy->fsrCandidateEnergy() ), xAOD::Muon::FSR_CandidateEnergy);
      	}
      }
    }
    if (numEnergyLossPerTrack>1) {
      ATH_MSG_VERBOSE("More than one e loss per track... ");
      problem=true;
    }
    if (m_fillExtraELossInfo) muon.auxdata< unsigned int >("numEnergyLossPerTrack")   = numEnergyLossPerTrack;
    if (problem) ATH_MSG_VERBOSE("Dumping problematic muon: "<<m_muonPrinter->print(muon));
  }
  
  void MuonCreatorTool::collectCells( xAOD::Muon& muon, xAOD::CaloClusterContainer* clusterContainer ) const {
    muon.auxdata< float >("ET_Core")     = 0;
    muon.auxdata< float >("ET_EMCore")   = 0;
    muon.auxdata< float >("ET_TileCore") = 0;
    muon.auxdata< float >("ET_HECCore")  = 0;

    const xAOD::TrackParticle* tp = muon.primaryTrackParticle();
    if(!tp || !clusterContainer){
      if (!tp) ATH_MSG_WARNING("Can not get primary track.");
      return;
    }

    // get ParticleCellAssociation
    ATH_MSG_DEBUG(" Selected track: pt " << tp->pt() << " eta " << tp->eta() << " phi " << tp->phi() );

    std::unique_ptr<Trk::CaloExtension> caloExtension =m_caloExtTool->caloExtension(*tp);
    if(!caloExtension){
      ATH_MSG_WARNING("Can not get caloExtension.");
      return;
    };

    SG::ReadHandle<CaloCellContainer> container(m_cellContainerName);

    xAOD::CaloCluster* cluster = m_cellCollector.collectCells( *caloExtension, *container, *clusterContainer );
    if( !cluster ){
      ATH_MSG_WARNING("Failed to create cluster from ParticleCellAssociation");
      return;
    }else{
      ATH_MSG_DEBUG(" New cluster: eta " << cluster->eta() << " phi " << cluster->phi() << " cells " << cluster->size() );
    }
	
    // create element links
    ElementLink< xAOD::CaloClusterContainer >   clusterLink(*clusterContainer,clusterContainer->size()-1);
    muon.setClusterLink(clusterLink);

    const CaloNoise* caloNoise = nullptr;
    if (!m_caloNoiseKey.empty()) {
      SG::ReadCondHandle<CaloNoise> noiseH (m_caloNoiseKey);
      caloNoise = noiseH.cptr();
    }

    // collect the core energy
    std::vector<float> etcore(4, 0);
    m_cellCollector.collectEtCore( *cluster, etcore, caloNoise, m_sigmaCaloNoiseCut );
    muon.auxdata< float >("ET_Core")     = etcore[Rec::CaloCellCollector::ET_Core];
    muon.auxdata< float >("ET_EMCore")   = etcore[Rec::CaloCellCollector::ET_EMCore];
    muon.auxdata< float >("ET_TileCore") = etcore[Rec::CaloCellCollector::ET_TileCore];
    muon.auxdata< float >("ET_HECCore")  = etcore[Rec::CaloCellCollector::ET_HECCore];  

    if( m_caloNoiseKey.empty() )
      ATH_MSG_DEBUG("NO Tool for calo noise,sigma: "
                    << m_sigmaCaloNoiseCut);
    else
      ATH_MSG_DEBUG("sigma: " << m_sigmaCaloNoiseCut);

    ATH_MSG_DEBUG("Etcore: tot/em/tile/hec " << etcore[Rec::CaloCellCollector::ET_Core] << "/"    
                  << etcore[Rec::CaloCellCollector::ET_EMCore] << "/"    
                  << etcore[Rec::CaloCellCollector::ET_TileCore] << "/"    
                  << etcore[Rec::CaloCellCollector::ET_HECCore]);
  }

  void MuonCreatorTool::addAlignmentEffectsOnTrack( xAOD::TrackParticleContainer* trkCont ) const {

    for(auto tp : *trkCont) {
      std::vector<std::vector<unsigned int> > chId;
      std::vector<float> deltaTrans;
      std::vector<float> sigmaDeltaTrans;
      std::vector<float> deltaAngle;
      std::vector<float> sigmaDeltaAngle;

      const Trk::Track* trk = tp->track();
      if(trk && trk->trackStateOnSurfaces()) {
	int nAEOT=0;
	for(auto tsos : *(trk->trackStateOnSurfaces())) {
	  if (!tsos->type(Trk::TrackStateOnSurface::Alignment)) continue;

	  const Trk::AlignmentEffectsOnTrack* aeot = tsos->alignmentEffectsOnTrack();
	  if(aeot) {
	    nAEOT++;
	    std::set<unsigned int> chIdSet;
	    for(auto id : aeot->vectorOfAffectedTSOS()) {
	      if( !id.is_valid() || !m_idHelperSvc->isMuon(id) ) continue;
	      chIdSet.insert( m_idHelperSvc->chamberIndex(id) );
	    }
	    std::vector<unsigned int> chIdVec;
	    std::copy(chIdSet.begin(), chIdSet.end(), std::back_inserter(chIdVec));

	    chId.push_back( chIdVec );
	    deltaTrans.push_back( aeot->deltaTranslation() );
	    sigmaDeltaTrans.push_back( aeot->sigmaDeltaTranslation() );
	    deltaAngle.push_back( aeot->deltaAngle() ); 
	    sigmaDeltaAngle.push_back( aeot->sigmaDeltaAngle() );
	  }
	}
      }

      tp->auxdecor< std::vector< std::vector<unsigned int> > >("alignEffectChId") = chId;
      tp->auxdecor< std::vector<float> >("alignEffectDeltaTrans")       = deltaTrans;
      tp->auxdecor< std::vector<float> >("alignEffectSigmaDeltaTrans")  = sigmaDeltaTrans;
      tp->auxdecor< std::vector<float> >("alignEffectDeltaAngle")       = deltaAngle;
      tp->auxdecor< std::vector<float> >("alignEffectSigmaDeltaAngle")  = sigmaDeltaAngle;
    }
  }

  void MuonCreatorTool::addMSIDScatteringAngles(xAOD::Muon& muon) const {
    const xAOD::TrackParticle* tp = muon.primaryTrackParticle();
    int nscatter=0;
    if( tp && tp->track() && tp->track()->trackStateOnSurfaces() && tp != muon.trackParticle( xAOD::Muon::InnerDetectorTrackParticle) ){
      for(auto tsos : *(tp->track()->trackStateOnSurfaces())) {
        if(tsos->materialEffectsOnTrack()){
          const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack () );
          if(!meot->energyLoss() || !meot->scatteringAngles()) continue;
          if(meot->energyLoss()->deltaE()==0){ //artificial scatterer found                                                                                                  
            if(nscatter==0){
              muon.auxdata<float>("deltaphi_0")=meot->scatteringAngles()->deltaPhi();
              muon.auxdata<float>("deltatheta_0")=meot->scatteringAngles()->deltaTheta();
	      muon.auxdata<float>("sigmadeltaphi_0")=meot->scatteringAngles()->sigmaDeltaPhi();
              muon.auxdata<float>("sigmadeltatheta_0")=meot->scatteringAngles()->sigmaDeltaTheta();
            }
            else if(nscatter==1){
              muon.auxdata<float>("deltaphi_1")=meot->scatteringAngles()->deltaPhi();
              muon.auxdata<float>("deltatheta_1")=meot->scatteringAngles()->deltaTheta();
              muon.auxdata<float>("sigmadeltaphi_1")=meot->scatteringAngles()->sigmaDeltaPhi();
              muon.auxdata<float>("sigmadeltatheta_1")=meot->scatteringAngles()->sigmaDeltaTheta();
            }
            nscatter++;
          }
        }
        if(nscatter>1) break;
      }
    }
    if(nscatter<=1){
      muon.auxdata<float>("deltaphi_1")=-999;
      muon.auxdata<float>("deltatheta_1")=-999;
      muon.auxdata<float>("sigmadeltaphi_1")=-999;
      muon.auxdata<float>("sigmadeltatheta_1")=-999;
    }
    if(nscatter==0){
      muon.auxdata<float>("deltaphi_0")=-999;
      muon.auxdata<float>("deltatheta_0")=-999;
      muon.auxdata<float>("sigmadeltaphi_0")=-999;
      muon.auxdata<float>("sigmadeltatheta_0")=-999;
    }
  }

  void MuonCreatorTool::addMSIDScatteringAngles(const xAOD::TrackParticle& tp) const{
    int nscatter=0;
    if(tp.track() && tp.track()->trackStateOnSurfaces()){
      for(auto tsos : *(tp.track()->trackStateOnSurfaces())) {
	if(tsos->materialEffectsOnTrack()){
          const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack () );
          if(!meot->energyLoss() || !meot->scatteringAngles()) continue;
          if(meot->energyLoss()->deltaE()==0){ //artificial scatterer found
            if(nscatter==0){
	      tp.auxdecor<float>("deltaphi_0")=meot->scatteringAngles()->deltaPhi();
              tp.auxdecor<float>("deltatheta_0")=meot->scatteringAngles()->deltaTheta();
              tp.auxdecor<float>("sigmadeltaphi_0")=meot->scatteringAngles()->sigmaDeltaPhi();
              tp.auxdecor<float>("sigmadeltatheta_0")=meot->scatteringAngles()->sigmaDeltaTheta();
            }
            else if(nscatter==1){
              tp.auxdecor<float>("deltaphi_1")=meot->scatteringAngles()->deltaPhi();
              tp.auxdecor<float>("deltatheta_1")=meot->scatteringAngles()->deltaTheta();
              tp.auxdecor<float>("sigmadeltaphi_1")=meot->scatteringAngles()->sigmaDeltaPhi();
              tp.auxdecor<float>("sigmadeltatheta_1")=meot->scatteringAngles()->sigmaDeltaTheta();
            }
            nscatter++;
          }
	}
        if(nscatter>1) break;
      }
    }
    if(nscatter<=1){
      tp.auxdecor<float>("deltaphi_1")=-999;
      tp.auxdecor<float>("deltatheta_1")=-999;
      tp.auxdecor<float>("sigmadeltaphi_1")=-999;
      tp.auxdecor<float>("sigmadeltatheta_1")=-999;
    }
    if(nscatter==0){
      tp.auxdecor<float>("deltaphi_0")=-999;
      tp.auxdecor<float>("deltatheta_0")=-999;
      tp.auxdecor<float>("sigmadeltaphi_0")=-999;
      tp.auxdecor<float>("sigmadeltatheta_0")=-999;
    }
  }
 
}	// end of namespace
