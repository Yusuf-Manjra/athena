/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EMAmbiguityTool.cxx
PACKAGE:  offline/Reconstruction/egamma/egammaTrackTools

CREATED:  12/06/2008
AUTHOR : T. Koffas
MODIFIED:
          Dec. 28, 2009 (FD) add protections against bad pointers
          May  06, 2010 (JM) use expectHitInBLayer for dead pixels
          June 06, 2010 (JM) add expectHitInBLayer when tp != conversion tracks
          June 07, 2010 (JM) remove LocationOfConversion function
          June,    2010 (JM) expand role of tool and change interface
          July 18, 2010 (JM) make it return AmbiguityResult instead of bool
          Oct. 28, 2010 (JM) make it use expectHitInBLayer from track summary
          Nov. 10, 2010 (JM) allow it to use outliers
          Feb.  6, 2012 (JM) introduce MEDIUM AR for two-track conversions
          Feb. 20, 2014 (BL) xAOD migration, changing logic
********************************************************************/

#include "EMAmbiguityTool.h"
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"
#include "egammaRecEvent/egammaRec.h"

EMAmbiguityTool::EMAmbiguityTool(const std::string& type, const std::string& name, const IInterface* parent) :
  AthAlgTool(type, name, parent)
{
  declareInterface<IEMAmbiguityTool>(this);
  //Minimum silicon hits for electron

  declareProperty("minNoSiHits",  m_MinNoSiHits = 4, "Minimum number of silicon hits to be an electron==>not photon for sure");
  declareProperty("minNoPixHits", m_MinNoPixHits = 2, "Minimum number of Pixel hits to be an electron==>not photon for sure");
  declareProperty("maxEoverPCut", m_maxEoverPCut = 10,"Maximum EoverP , more that this is ambiguous");
  declareProperty("minPCut",      m_minPtCut = 1500 ,  "Minimum P, less than that is ambiguous");

}

EMAmbiguityTool::~EMAmbiguityTool() {}

StatusCode EMAmbiguityTool::initialize() {

  ATH_MSG_DEBUG("Initializing " << name() << "...");
  
  return StatusCode::SUCCESS;
}

StatusCode EMAmbiguityTool::finalize() {
  return StatusCode::SUCCESS;
}

// ====================================================================
// return value: AuthorElectron, AuthorPhoton, AuthorAmbiguous, AuthorUnknown

unsigned int EMAmbiguityTool::ambiguityResolve(const egammaRec *egRec) const
{

  const xAOD::CaloCluster* cluster = egRec->caloCluster();
  const xAOD::Vertex* vx =  egRec->vertex();
  const xAOD::TrackParticle* tp = egRec->trackParticle();


  uint8_t trkExpectBlayerHit(0), trkBlayerHits(0), trkPixelHits(0), trkSiHits(0);

  double ep(0);
  if(tp){
    ep= cluster->e() * fabs(tp->qOverP());
  }

  if (tp && !tp->summaryValue(trkExpectBlayerHit,xAOD::expectBLayerHit))
    ATH_MSG_WARNING("Could not retrieve expected BLayer hit from track");
  
  if (tp && !tp->summaryValue(trkBlayerHits,xAOD::numberOfBLayerHits))
    ATH_MSG_WARNING("Could not retrieve number of BLayer hits from track");
  
  if (tp && !tp->summaryValue(trkPixelHits,xAOD::numberOfPixelHits))
    ATH_MSG_WARNING("Could not retrieve number of pixel hits from track");
  
  if (tp && !tp->summaryValue(trkSiHits, xAOD::numberOfSCTHits))
    ATH_MSG_WARNING("Could not retrieve number of SCT hits from track");
  trkSiHits += trkPixelHits;
  
  xAOD::EgammaParameters::ConversionType convType(xAOD::EgammaHelpers::conversionType(vx));
  bool vxDoubleSi = (convType == xAOD::EgammaParameters::doubleSi);
  

  //See if the Si+Si conversion shares one track with the electron
  //if not we might have a trident.
  //if yes and the track is not good we definetely matched the conversion as electron.
  bool shareTrack=false;
  if(tp && trkSiHits>=m_MinNoSiHits && vxDoubleSi){
    const xAOD::TrackParticle *trk1 = ( vx->nTrackParticles() ? vx->trackParticle(0) : 0 );
    const xAOD::TrackParticle *trk2 = ( vx->nTrackParticles() > 1 ? vx->trackParticle(1) : 0 );
  
    if( (tp==trk1) || (tp==trk2)) {
      shareTrack=true;

    }
  }
  
  ATH_MSG_DEBUG("Vertex, SiSi: " << (vx != 0) << " , " << vxDoubleSi);
  ATH_MSG_DEBUG("Track, Si hits, pixel hits, bLayerHits, expectBlayerHit: " 
    << (tp != 0) << " , " << (int) trkSiHits << " , " << (int) trkPixelHits 
    << " , " <<  (int) trkBlayerHits << " , " << (int) trkExpectBlayerHit);
  ATH_MSG_DEBUG("Share track : " << shareTrack);

  //The Electron collection will NOT contain Photons
  //The Photon collection will NOT contain Electrons

  //So for conversions vs electrons better to go to ambiguous 
  //except  if we are sure.

  //Practically a photon here means definetely not Electron
  //And vice-versa

  //Photon ==> Surely not electron
  // - no track
  // - or no track with the minimum requirent hits to be an electron
  // - or Si+Si vertex and
  //    - No pixel
  //    - The electron track is part of the conversion
  // In this case we do not want this to be in Electrons

  if (!tp || 
      trkSiHits<m_MinNoSiHits || 
      (vxDoubleSi && !trkPixelHits && shareTrack)){
    ATH_MSG_DEBUG("Returning Photon");
    return xAOD::EgammaParameters::AuthorPhoton;
  }
  
  //Ambigous due to E/P and Min Pt 
  // - E/P >10  or track P < 1.5 GeV then Ambigoous
  if( ep > m_maxEoverPCut ||  tp->pt()<m_minPtCut) {
    ATH_MSG_DEBUG("Returning Ambiguous deu to E over P and min Pt ");
    return xAOD::EgammaParameters::AuthorAmbiguous;
  }

  //Electron ==> Surely not Photon
  // - Track with at least the minimum si hits (previous selection for photons)
  // - And has E/P < 10 and Pt > 1.5 GeV (previous for ambiguous)
  // - No vertex Matched
  // - Or if a vertex exists and is not  Si+Si
  //    - Either Blayer hit
  //    - Or (2) or more Pixel hits when not expecting a b-layer hit
  // In this case we do not want this to be in Photons
  
  if (!vx || 
      (trkBlayerHits && !vxDoubleSi) || 
      (!trkExpectBlayerHit && trkPixelHits>=m_MinNoPixHits  && !vxDoubleSi) ){
      ATH_MSG_DEBUG("Returning Electron");
      return xAOD::EgammaParameters::AuthorElectron;
  }

  // Ambiguous all else, these will go to both electrons and photons

  // This involves a matched Si+Si vertex and  a track with enough hits.
  // Or a non Si+Si vertex matched and a track with not enough hits.


  ATH_MSG_DEBUG("Returning Ambiguous");
  return xAOD::EgammaParameters::AuthorAmbiguous;
}

