/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTNoise.h"

#include "TRTDigCondBase.h"
#include "TRTElectronicsProcessing.h"
#include "TRTElectronicsNoise.h"

#include "TRTDigSettings.h"

#include "GaudiKernel/ServiceHandle.h"

#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <cstdlib> //Always include this when including cmath!

#include "InDetSimEvent/TRTHitIdHelper.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include <algorithm>
#include <exception>

struct TRTDigitSorter {
  bool operator() (TRTDigit digit1, TRTDigit digit2) { return (digit1.GetStrawID()<digit2.GetStrawID());}
} TRTDigitSorterObject;
//_____________________________________________________________________________
TRTNoise::TRTNoise( const TRTDigSettings* digset,
		    const InDetDD::TRT_DetectorManager* detmgr,
		    ServiceHandle <IAtRndmGenSvc> atRndmGenSvc,
		    TRTDigCondBase* digcond,
		    TRTElectronicsProcessing * ep,
		    TRTElectronicsNoise * electronicsnoise,
		    const TRT_ID* trt_id,
		    bool UseArgonStraws,   // need for Argon
		    bool useConditionsHTStatus, // need for Argon
		    ServiceHandle<ITRT_StrawStatusSummarySvc> sumSvc // need for Argon
		  )
  : m_settings(digset),
    m_detmgr(detmgr),
    m_pDigConditions(digcond),
    m_pElectronicsProcessing(ep),
    m_pElectronicsNoise(electronicsnoise),
    m_id_helper(trt_id),
    m_digitPoolLength(5000),
    m_digitPoolLength_nextaccessindex(0),
    m_msg("TRTNoise"),
    m_UseArgonStraws(UseArgonStraws),
    m_useConditionsHTStatus(useConditionsHTStatus),
    m_sumSvc(sumSvc)
{
  if (msgLevel(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "TRTNoise::Constructor begin" << endreq; }

  m_noise_randengine = atRndmGenSvc->GetEngine("TRT_Noise");

  InitThresholdsAndNoiseAmplitudes_and_ProduceNoiseDigitPool();

  if ( m_settings->noiseInSimhits() )
    m_pElectronicsNoise->reinitElectronicsNoise( 1000 );

  if (msgLevel(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "Constructor done" << endreq; }
}

//_____________________________________________________________________________
TRTNoise::~TRTNoise() {

  if (msgLevel(MSG::VERBOSE)) {msg(MSG::VERBOSE) << "TRTNoise::Destructor" << endreq; }
}

//_____________________________________________________________________________
void TRTNoise::InitThresholdsAndNoiseAmplitudes_and_ProduceNoiseDigitPool() {

  /////////////////////////////////////////////////////////////////////
  //Strategy:                                                        //
  //                                                                 //
  //First we create a lookup table so that we can, based on the      //
  //noiselevel_i (i=straw_id), calculate for each straw: (LT/NA)_i   //
  //(NA=Noise Amplitude).                                            //
  //                                                                 //
  //We also know for each straw their relative noise amplitude, r_i. //
  //                                                                 //
  //C.f. the noise note, one finds that the lowthreshold and noise   //
  //amplitude for each straw is given by the formulas:               //
  //                                                                 //
  // LT_i = (LT/NA)_i * r_i * k   and   NA_i = r_i * k               //
  //                                                                 //
  // with k = <LT_i>/<(LT/NA)_i * r_i>                               //
  //                                                                 //
  // So now we first figure out the value of k, and afterwards we go //
  // through the straws and set LT_i and NA_i.                       //
  /////////////////////////////////////////////////////////////////////
  if (msgLevel(MSG::VERBOSE)) { msg(MSG::VERBOSE)
    << "TRTNoise::InitThresholdsAndNoiseAmplitudes_and_ProduceNoiseDigitPool Begin" << endreq;
  }

  ///////////////////////////////////////////////////////////////////
  // Step 1 - create lookup table for mapping: noiselevel -> LT/NA //
  ///////////////////////////////////////////////////////////////////

  // According to Anatoli, the noise shaping function is not very different for Argon and Xenon.
  // Fixme: This function used hardcoded noise signal shape in TRTSignalShape.cxx line 58.

  std::vector<float> maxLTOverNoiseAmp;
  m_pElectronicsNoise->getSamplesOfMaxLTOverNoiseAmp(maxLTOverNoiseAmp,10000);

  sort(    maxLTOverNoiseAmp.begin(), maxLTOverNoiseAmp.end() );
  reverse( maxLTOverNoiseAmp.begin(), maxLTOverNoiseAmp.end() );

  // If we have LT event-event fluctuations, we need to include that effect in the curve

  double relfluct = m_settings->relativeLowThresholdFluctuation();

  if ( relfluct > 0. ) {
    std::vector<float> nl_given_LT2Amp;
    float min_lt2amp, max_lt2amp;
    makeInvertedLookupTable( maxLTOverNoiseAmp, 0., 1.,
			     nl_given_LT2Amp, min_lt2amp, max_lt2amp);
    float new_min_lt2amp, new_max_lt2amp;
    evolve_LT2AmpVsNL_to_include_LTfluct( nl_given_LT2Amp,
					  min_lt2amp, max_lt2amp,
					  relfluct,
					  new_min_lt2amp, new_max_lt2amp,
					  static_cast<unsigned int>(0.1*nl_given_LT2Amp.size()));
    float min_nl,max_nl;  //Sanity check could be that ~0 and ~1 are returned.
    makeInvertedLookupTable( nl_given_LT2Amp,new_min_lt2amp, new_max_lt2amp, maxLTOverNoiseAmp,min_nl,max_nl);
  }

  ///////////////////////////////////////////////////////////////////
  // Step 2 - figure out constant, k = <LT_i> / <(LT/NA)_i * r_i>  //
  ///////////////////////////////////////////////////////////////////

  //Figure out <(LT/NA)_i * r_i>:

  unsigned long nstrawsBar_Xenon(0), nstrawsEC_Xenon(0);
  unsigned long nstrawsBar_Argon(0), nstrawsEC_Argon(0);
  double sumBar_Xenon(0.), sumEC_Xenon(0.);
  double sumBar_Argon(0.), sumEC_Argon(0.);
  int hitid(0);
  float noiselevel(0.), noiseamp(0.);

  m_pDigConditions->resetGetNextStraw();

  /// Loop through all non-dead straws:
  while ( m_pDigConditions->getNextStraw(hitid, noiselevel, noiseamp) ) {

    bool isArgonStraw = IsArgonStraw(getStrawIdentifier(hitid));

    if (isArgonStraw){
      if ( !(hitid & 0x00200000) ) {
	++nstrawsBar_Argon;
	sumBar_Argon += useLookupTable(noiselevel, maxLTOverNoiseAmp, 0., 1.) * noiseamp;
      } else {
	++nstrawsEC_Argon;
	sumEC_Argon += useLookupTable(noiselevel, maxLTOverNoiseAmp, 0., 1.) * noiseamp;
      }
    }
    else{
      if ( !(hitid & 0x00200000) ) {
	++nstrawsBar_Xenon;
	sumBar_Xenon += useLookupTable(noiselevel, maxLTOverNoiseAmp, 0., 1.) * noiseamp;
      } else {
	++nstrawsEC_Xenon;
	sumEC_Xenon += useLookupTable(noiselevel, maxLTOverNoiseAmp, 0., 1.) * noiseamp;
      }
    }

  };

  //This gives us k right away:
  double kBar_Argon(0.), kEC_Argon(0.), kBar_Xenon(0.), kEC_Xenon(0.);
  if (sumBar_Argon !=0.) kBar_Argon = m_settings->lowThresholdBar(true)  * (nstrawsBar_Argon/sumBar_Argon);
  if (sumEC_Argon  !=0.) kEC_Argon  = m_settings->lowThresholdEC(true)   * (nstrawsEC_Argon/sumEC_Argon);
  if (sumBar_Xenon !=0.) kBar_Xenon = m_settings->lowThresholdBar(false) * (nstrawsBar_Xenon/sumBar_Xenon);
  if (sumEC_Xenon  !=0.) kEC_Xenon  = m_settings->lowThresholdEC(false)  * (nstrawsEC_Xenon/sumEC_Xenon);

  ///////////////////////////////////////////////////////////////////
  // Step 3 - Calculate and set actual LT_i and NA_i               //
  ///////////////////////////////////////////////////////////////////

  std::vector<float> actual_LTs, actual_noiseamps;
  std::vector<bool> strawTypes;
  if ( m_settings->noiseInUnhitStraws() ) {
    int ns = m_pDigConditions->totalNumberOfActiveStraws();
    actual_LTs.reserve(ns);
    actual_noiseamps.reserve(ns);
  };

  float actualLT, actualNA;
  bool strawType; // true - Argon; false - Xenon

  m_pDigConditions->resetGetNextStraw();
  double sumLT_Argon(0.), sumLTsq_Argon(0.), sumNA_Argon(0.), sumNAsq_Argon(0.);
  double sumLT_Xenon(0.), sumLTsq_Xenon(0.), sumNA_Xenon(0.), sumNAsq_Xenon(0.);
  while ( m_pDigConditions->getNextStraw( hitid, noiselevel, noiseamp) ) {
    bool isArgonStraw = IsArgonStraw(getStrawIdentifier(hitid));
    if (isArgonStraw){
      strawType = true; // Argon straw
      actualNA = noiseamp*(  (!(hitid & 0x00200000)) ? kBar_Argon : kEC_Argon );
    }
    else{
      strawType = false; // Argon straw
      actualNA = noiseamp*(  (!(hitid & 0x00200000)) ? kBar_Xenon : kEC_Xenon );
    }
    actualLT = useLookupTable(noiselevel, maxLTOverNoiseAmp, 0., 1.)*actualNA;
    if (isArgonStraw){
      sumNA_Argon += actualNA; sumNAsq_Argon += actualNA*actualNA;
      sumLT_Argon += actualLT; sumLTsq_Argon += actualLT*actualLT;
    }
    else{
      sumNA_Xenon += actualNA; sumNAsq_Xenon += actualNA*actualNA;
      sumLT_Xenon += actualLT; sumLTsq_Xenon += actualLT*actualLT;
    }
    m_pDigConditions->setRefinedStrawParameters( hitid, actualLT, actualNA );
    if ( m_settings->noiseInUnhitStraws() ) {
      actual_LTs.push_back(actualLT);
      actual_noiseamps.push_back(actualNA);
      strawTypes.push_back(strawType);
    };
  };


  if (msgLevel(MSG::DEBUG)) {

    unsigned long nstraws_Argon = nstrawsBar_Argon + nstrawsEC_Argon;
    unsigned long nstraws_Xenon = nstrawsBar_Xenon + nstrawsEC_Xenon;

    if (nstraws_Argon!=0){
      msg(MSG::DEBUG) << "TRTNoise:: Average LT is " << sumLT_Argon/nstraws_Argon/CLHEP::eV << " CLHEP::eV, with an RMS of "
      << sqrt((sumLTsq_Argon/nstraws_Argon)-(sumLT_Argon/nstraws_Argon)*(sumLT_Argon/nstraws_Argon))/CLHEP::eV << " CLHEP::eV" << endreq;
      msg(MSG::DEBUG) << "TRTNoise:: Average NA is "<<sumNA_Argon/nstraws_Argon/CLHEP::eV <<" CLHEP::eV, with an RMS of "
      << sqrt((sumNAsq_Argon/nstraws_Argon)-(sumNA_Argon/nstraws_Argon)*(sumNA_Argon/nstraws_Argon))/CLHEP::eV << " CLHEP::eV" << endreq;
    }
    if (nstraws_Xenon!=0){
      msg(MSG::DEBUG) << "TRTNoise:: Average LT is " << sumLT_Xenon/nstraws_Xenon/CLHEP::eV << " CLHEP::eV, with an RMS of "
      << sqrt((sumLTsq_Xenon/nstraws_Xenon)-(sumLT_Xenon/nstraws_Xenon)*(sumLT_Xenon/nstraws_Xenon))/CLHEP::eV << " CLHEP::eV" << endreq;
      msg(MSG::DEBUG) << "TRTNoise:: Average NA is "<<sumNA_Xenon/nstraws_Xenon/CLHEP::eV <<" CLHEP::eV, with an RMS of "
      << sqrt((sumNAsq_Xenon/nstraws_Xenon)-(sumNA_Xenon/nstraws_Xenon)*(sumNA_Xenon/nstraws_Xenon))/CLHEP::eV << " CLHEP::eV" << endreq;
    }

  }

  ///////////////////////////////////////////////////////////////////
  // Step 4 - Produce pool of pure noise digits                    //
  ///////////////////////////////////////////////////////////////////

  if ( m_settings->noiseInUnhitStraws() ) {
    ProduceNoiseDigitPool( actual_LTs, actual_noiseamps, strawTypes );
  }
  if (msgLevel(MSG::VERBOSE)) { msg(MSG::VERBOSE)
      << "TRTNoise::InitThresholdsAndNoiseAmplitudes_and_ProduceNoiseDigitPool Done" << endreq;
  }
  return;
}

//_____________________________________________________________________________
void TRTNoise::ProduceNoiseDigitPool( const std::vector<float>& lowthresholds,
				      const std::vector<float>& noiseamps,
				      const std::vector<bool>& strawType ) {

  unsigned int nstraw = lowthresholds.size();
  unsigned int istraw;

  if (msgLevel(MSG::VERBOSE)) { msg(MSG::VERBOSE) << "TRTNoise::Producing noise digit pool" << endreq; }

  m_digitPool.resize( m_digitPoolLength );

  std::vector<TRTElectronicsProcessing::Deposit> deposits;
  int dummyhitid(0);
  TRTDigit digit;
  std::vector<int> m_highThresholdDiscriminator; 
  unsigned int nokdigits(0);
  unsigned int ntries(0);
  while ( nokdigits < m_digitPoolLength ) {

    // Once and a while, create new vectors of shaped electronics noise.
    // These are used as inputs to TRTElectronicsProcessing::ProcessDeposits
    // to create noise digits
    if ( ntries%400==0 ) {
      m_pElectronicsNoise->reinitElectronicsNoise(200);
    }
    // Initialize stuff (is that necessary)?
    digit = TRTDigit();
    deposits.clear();

    // Choose straw to simulate
    istraw = CLHEP::RandFlat::shootInt(m_noise_randengine, nstraw );

    // Process deposits this straw. Since there are no deposits, only noise
    //   will contrinute
    m_pElectronicsProcessing->ProcessDeposits( deposits,
					       dummyhitid,
					       digit,
					       lowthresholds.at(istraw),
					       noiseamps.at(istraw),
					       strawType.at(istraw) /// true - Argon; false - Xenon
 					    );

    // If this process produced a digit, store in pool
    if ( digit.GetDigit() ) {
      m_digitPool[nokdigits++] = digit.GetDigit();
    }
    ntries++;
  };

  if (msgLevel(MSG::VERBOSE)) {
    if(0==ntries) {
      if (msgLevel(MSG::FATAL)) { msg(MSG::FATAL) << "ntries==0 this should not be possible!" << endreq; }
      throw std::exception();
    }
    msg(MSG::VERBOSE)
    << "Produced noise digit pool of size " << m_digitPool.size()
    << " (efficiency was " << static_cast<double>(m_digitPool.size())/ntries << ")" << endreq;
  }

  m_digitPoolLength_nextaccessindex = 0;

}

//_____________________________________________________________________________
void TRTNoise::appendPureNoiseToProperDigits( std::vector<TRTDigit>& digitVect,
					      const std::set<int>& sim_hitids)
{

  const std::set<int>::const_iterator sim_hitids_end(sim_hitids.end());

  m_pDigConditions->resetGetNextNoisyStraw();
  int hitid;
  float noiselevel;

  while (m_pDigConditions->getNextNoisyStraw(m_noise_randengine,
					     hitid,
					     noiselevel) ) {
    //returned noiselevel not used for anything right now (fixme?).

    // If this strawID does not have a sim_hit, add a pure noise digit
    if ( sim_hitids.find(hitid) == sim_hitids_end ) {
      const int ndigit(m_digitPool[CLHEP::RandFlat::shootInt(m_noise_randengine,
						m_digitPoolLength)]);
      digitVect.push_back(TRTDigit(hitid,ndigit));
    }
  };
  digitVect.pop_back(); //Required since last hitID is occasionally corrupted.
  return;
}

//_____________________________________________________________________________

void TRTNoise::appendCrossTalkNoiseToProperDigits(std::vector<TRTDigit>& digitVect,
						  const std::set<Identifier>& simhitsIdentifiers,
						  ServiceHandle<ITRT_StrawNeighbourSvc> m_TRTStrawNeighbourSvc) {

  //id helper:
  TRTHitIdHelper* hitid_helper = TRTHitIdHelper::GetHelper();

  std::vector<Identifier> IdsFromChip;
  std::vector<Identifier> CrossTalkIds;
  std::vector<Identifier> CrossTalkIdsOtherEnd;
  std::set<Identifier>::const_iterator simhitsIdentifiers_end = simhitsIdentifiers.end();
  std::set<Identifier>::const_iterator simhitsIdentifiers_begin = simhitsIdentifiers.begin();
  std::set<Identifier>::iterator simhitsIdentifiersIter;

  for (simhitsIdentifiersIter=simhitsIdentifiers_begin; simhitsIdentifiersIter!=simhitsIdentifiers_end; ++simhitsIdentifiersIter) {

    m_TRTStrawNeighbourSvc->getStrawsFromChip(*simhitsIdentifiersIter,IdsFromChip);
    CrossTalkIds.assign(IdsFromChip.begin(),IdsFromChip.end());

    //for barrel only - treated exactly equally as id's on the right end
    if (!(abs(m_id_helper->barrel_ec(IdsFromChip[0]))==1)) { continue; }

    Identifier otherEndID = m_id_helper->straw_id((-1)*m_id_helper->barrel_ec(*simhitsIdentifiersIter),
						  m_id_helper->phi_module(*simhitsIdentifiersIter),
						  m_id_helper->layer_or_wheel(*simhitsIdentifiersIter),
						  m_id_helper->straw_layer(*simhitsIdentifiersIter),
						  m_id_helper->straw(*simhitsIdentifiersIter));
    if (otherEndID.get_compact()) { CrossTalkIdsOtherEnd.push_back(otherEndID); }

    for (unsigned int i=0;i<CrossTalkIds.size();++i) {

      if ( simhitsIdentifiers.find(CrossTalkIds[i]) == simhitsIdentifiers_end )  {
	if (m_pDigConditions->crossTalkNoise(m_noise_randengine)==1 ) {
	  const int ndigit(m_digitPool[CLHEP::RandFlat::shootInt(m_noise_randengine,
							  m_digitPoolLength)]);
	  int barrel_endcap, isneg;
	  switch ( m_id_helper->barrel_ec(CrossTalkIds[i]) ) {
	  case -1:  barrel_endcap = 0; isneg = 0; break;
	  case  1:  barrel_endcap = 0; isneg = 1; break;
	  default:
	    if (msgLevel(MSG::WARNING)) {msg(MSG::WARNING) << "TRTDigitization::TRTNoise - identifier problems - skipping detector element!!" <<  endreq; }
	    continue;
	  }
	  const int ringwheel(m_id_helper->layer_or_wheel(CrossTalkIds[i]));
	  const int phisector(m_id_helper->phi_module(CrossTalkIds[i]));
	  const int layer    (m_id_helper->straw_layer(CrossTalkIds[i]));
	  const int straw    (m_id_helper->straw(CrossTalkIds[i]));

	  //built hit id
	  int hitid = hitid_helper->buildHitId( barrel_endcap, isneg, ringwheel, phisector,layer, straw);
	  //add to digit vector
	  digitVect.push_back(TRTDigit(hitid,ndigit));
	}
      }
    }

    for (unsigned int i=0;i<CrossTalkIdsOtherEnd.size();++i) {
      if ( simhitsIdentifiers.find(CrossTalkIdsOtherEnd[i]) == simhitsIdentifiers_end )  {
	if (m_pDigConditions->crossTalkNoiseOtherEnd(m_noise_randengine)==1 ) {

	  const int ndigit(m_digitPool[CLHEP::RandFlat::shootInt(m_noise_randengine,m_digitPoolLength)]);

	  int barrel_endcap, isneg;
	  switch ( m_id_helper->barrel_ec(CrossTalkIdsOtherEnd[i]) ) {
	  case -1:  barrel_endcap = 0; isneg = 0; break;
	  case  1:  barrel_endcap = 0; isneg = 1; break;
	  default:
	    if (msgLevel(MSG::WARNING)) { msg(MSG::WARNING) << "TRTDigitization::TRTNoise - identifier problems - skipping detector element!!" <<  endreq; }
	    continue;
	    }

	  const int ringwheel(m_id_helper->layer_or_wheel(CrossTalkIdsOtherEnd[i]));
	  const int phisector(m_id_helper->phi_module(CrossTalkIdsOtherEnd[i]));
	  const int layer    (m_id_helper->straw_layer(CrossTalkIdsOtherEnd[i]));
	  const int straw    (m_id_helper->straw(CrossTalkIdsOtherEnd[i]));

	    //built hit id
	    int hitid = hitid_helper->buildHitId( barrel_endcap, isneg, ringwheel, phisector,layer, straw);
	    //add to digit vector
	    digitVect.push_back(TRTDigit(hitid,ndigit));
	}
      }
    }

    IdsFromChip.resize(0);
    CrossTalkIdsOtherEnd.resize(0);
    CrossTalkIds.resize(0);
  }
}

void TRTNoise::sortDigits(std::vector<TRTDigit>& digitVect)
{
  std::sort(digitVect.begin(), digitVect.end(), TRTDigitSorterObject);
  return;
}


//_____________________________________________________________________________
float TRTNoise::useLookupTable(const float& x, // noise_level
			       const std::vector<float>& y_given_x,
			       const float& min_x,
			       const float& max_x ) {

  double       bin_withfrac;
  unsigned int lower_index;
  double       fraction_into_bin;

  // Assumes that y_given_x is homogeneous (and not entirely flat)

  // Low x-value, return first y-value
  if ( x < min_x ) {
    return y_given_x.front();
  }

  // Which bin?
  bin_withfrac = (x-min_x)*(y_given_x.size()-1)/(max_x-min_x);
  lower_index = static_cast<unsigned int>(bin_withfrac);

  // High x-value, return last y-value
  if ( lower_index >= y_given_x.size()-1 ) {
    return y_given_x.back();
  }

  // Normal case: return weighted sum of two neighbouring bin values
  fraction_into_bin = bin_withfrac - lower_index;
  return (1.-fraction_into_bin) * y_given_x[lower_index] + fraction_into_bin * y_given_x[lower_index+1];

}

//_____________________________________________________________________________
void TRTNoise::makeInvertedLookupTable( const std::vector<float>& y_given_x,
					const float & min_x,
					const float & max_x,
					std::vector<float>& x_given_y,
					float & min_y,
					float & max_y ) {
  //Only works well when y_given_x.size() is large.
  //
  //Also assumes that y_given_x is homogeneous.

  //Figure out if rising or falling, and y limits:
  bool rising;

  if ( y_given_x.front() < y_given_x.back() ) {
    //NB: All use-cases have so far been on rising=false lookuptables
    rising = true;
    min_y = y_given_x.front();
    max_y = y_given_x.back();
  } else {
    rising = false;
    min_y = y_given_x.back();
    max_y = y_given_x.front();
  };

  const unsigned int n(y_given_x.size());
  if ( x_given_y.size() != n ) {
    x_given_y.resize(n);
  }
  //Fill new array:
  const float step_y((max_y-min_y)/(n-1));
  const float step_x((max_x-min_x)/(n-1));

  unsigned int searchindex = rising ? 0 : n-1;
  float yval;
  x_given_y.front() = rising ? min_x : max_x;
  for (unsigned int i = 1; i < n-1; ++i) {
    yval = min_y + i*step_y;
    if (rising) {
      // increase index in old array until we come to the index
      //   with a appropriate yval
      while ( y_given_x[searchindex] < yval ) { ++searchindex; };
      x_given_y[i] = min_x + searchindex*step_x;
    } else {
      while ( y_given_x[searchindex]<yval ) { --searchindex; };
      x_given_y[i] = min_x + searchindex*step_x;
    }
  };
  x_given_y.back() = rising ? max_x : min_x;

}

//_____________________________________________________________________________
void TRTNoise::evolve_LT2AmpVsNL_to_include_LTfluct( std::vector<float>& nl_given_lt2na,
						     const float & min_lt2na,
						     const float & max_lt2na,
						     const float relativeLTFluct,
						     float & new_min_lt2na,
						     float & new_max_lt2na,
						     const unsigned int& number_new_bins )
{
  //RelativeLTfluct should be less than 0.2.

  //  unsigned int n = nl_given_lt2na.size();
  std::vector<float> new_nl_given_lt2na(number_new_bins);

  new_min_lt2na = min_lt2na;
  new_max_lt2na =
    relativeLTFluct < 0.4 ? max_lt2na/(1.0-2.0*relativeLTFluct) : 5*max_lt2na;


  for (unsigned int i = 0; i<number_new_bins;++i) {
    const float new_lt2naval(new_min_lt2na +
			     (new_max_lt2na-new_min_lt2na)*static_cast<float>(i)/(number_new_bins-1));
    float sigma = new_lt2naval*relativeLTFluct;
    if ( sigma <= 0 ) {
      if (sigma<0) { sigma *= -1.0; }
      else { sigma = 1.0e-8; }
    }
    const float lowerintrange(new_lt2naval - 5.0*sigma);
    float upperintrange(new_lt2naval + 5.0*sigma);
    if (upperintrange>max_lt2na) {
      upperintrange = max_lt2na; //Since the NL is zero above anyway
    }
    const double du((upperintrange-lowerintrange)/100.0);
    double sum(0.);
    const double minusoneover2sigmasq(- 1.0 / (2.0*sigma*sigma));

    for (double u(lowerintrange); u < upperintrange; u += du) {
      sum += useLookupTable(u,nl_given_lt2na,min_lt2na,max_lt2na) *
	exp(minusoneover2sigmasq * (u-new_lt2naval) * (u-new_lt2naval));
    }
    sum *= du / ( sqrt (2.0 * 3.14159265358979 ) * sigma);
    new_nl_given_lt2na[i] = sum;
  };

  nl_given_lt2na.resize(number_new_bins);
  copy(new_nl_given_lt2na.begin(),
       new_nl_given_lt2na.end(),
       nl_given_lt2na.begin());
}

Identifier TRTNoise::getStrawIdentifier ( int hitID )
{
  Identifier IdStraw;
  Identifier IdLayer;

  const int mask(0x0000001F);
  const int word_shift(5);
  int trtID, ringID, moduleID, layerID, strawID;
  int wheelID, planeID, sectorID;

  const InDetDD::TRT_BarrelElement *barrelElement;
  const InDetDD::TRT_EndcapElement *endcapElement;

  if ( !(hitID & 0x00200000) ) {      // barrel
    strawID   = hitID & mask;
    hitID   >>= word_shift;
    layerID   = hitID & mask;
    hitID   >>= word_shift;
    moduleID  = hitID & mask;
    hitID   >>= word_shift;
    ringID    = hitID & mask;
    trtID     = hitID >> word_shift;

    barrelElement = m_detmgr->getBarrelElement(trtID, ringID, moduleID, layerID);
    if ( barrelElement ) {
      IdLayer = barrelElement->identify();
      IdStraw = m_id_helper->straw_id(IdLayer, strawID);
    } else {
      msg(MSG::ERROR) << "Could not find detector element for barrel identifier with "
		      << "(ipos,iring,imod,ilayer,istraw) = ("
		      << trtID << ", " << ringID << ", " << moduleID << ", "
		      << layerID << ", " << strawID << ")" << endreq;
    }
  } else {                           // endcap
    strawID   = hitID & mask;
    hitID   >>= word_shift;
    planeID   = hitID & mask;
    hitID   >>= word_shift;
    sectorID  = hitID & mask;
    hitID   >>= word_shift;
    wheelID   = hitID & mask;
    trtID     = hitID >> word_shift;

    // change trtID (which is 2/3 for endcaps) to use 0/1 in getEndcapElement
    if (trtID == 3) { trtID = 0; }
    else            { trtID = 1; }

    endcapElement =
      m_detmgr->getEndcapElement(trtID, wheelID, planeID, sectorID);

    if ( endcapElement ) {
      IdLayer = endcapElement->identify();
      IdStraw = m_id_helper->straw_id(IdLayer, strawID);
    } else {
      msg(MSG::ERROR) << "Could not find detector element for endcap identifier with "
		      << "(ipos,iwheel,isector,iplane,istraw) = ("
		      << trtID << ", " << wheelID << ", " << sectorID << ", "
		      << planeID << ", " << strawID << ")" << endreq;
      msg(MSG::ERROR) << "If this happens very rarely, don't be alarmed "
		      << "(it is a Geant4 'feature')" << endreq;
      msg(MSG::ERROR) << "If it happens a lot, you probably have misconfigured geometry "
		      << "in the sim. job." << endreq;
    }

  }

  return IdStraw;
}


//_____________________________________________________________________________
bool TRTNoise::IsArgonStraw(Identifier TRT_Identifier) {
  // EStatus { Undefined, Dead, Good(Xe) }
  bool isArgonStraw = m_UseArgonStraws; // If this is true then ALL straws are Argon
  // But TRT/Cond/StatusHT will override this with specific Xe/Ar geometry if available:
  if (m_useConditionsHTStatus) {
    if ( m_sumSvc->getStatusHT(TRT_Identifier) != TRTCond::StrawStatus::Good ) {
       isArgonStraw = true;  // Argon straw
    } else {
       isArgonStraw = false; // Xenon straw
    }
  }
  return isArgonStraw;
}
