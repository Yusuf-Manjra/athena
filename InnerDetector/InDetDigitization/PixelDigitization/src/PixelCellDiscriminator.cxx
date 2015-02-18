/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// PixelCellDiscriminator.cxx 
//   Implementation file for class PixelCellDiscriminator 
/////////////////////////////////////////////////////////////////// 
// (c) ATLAS Detector software 
/////////////////////////////////////////////////////////////////// 

#include "PixelCellDiscriminator.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SiDigitization/SiHelper.h"
#include "CalibSvc.h"
#include "TimeSvc.h"
// random number service
#include "CLHEP/Random/RandomEngine.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

using namespace InDetDD;

static const InterfaceID IID_IPixelCellDiscriminator("PixelCellDiscriminator", 1, 0);
const InterfaceID& PixelCellDiscriminator::interfaceID( ){ return IID_IPixelCellDiscriminator; }

// Constructor with parameters:
PixelCellDiscriminator::PixelCellDiscriminator(const std::string& type, const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_TimeSvc("TimeSvc",name),
  m_CalibSvc("CalibSvc",name),
  m_rndmSvc("AtDSFMTGenSvc",name),
  m_rndmEngineName("PixelDigitization"),
  m_rndmEngine(0)

{  
	declareInterface< PixelCellDiscriminator >( this );
	declareProperty("RndmSvc",m_rndmSvc,"Random Number Service used in Pixel digitization");
	declareProperty("RndmEngine",m_rndmEngineName,"Random engine name");
	declareProperty("CalibSvc",m_CalibSvc);
	declareProperty("TimeSvc",m_TimeSvc);	
}

// Destructor:
PixelCellDiscriminator::~PixelCellDiscriminator()
{}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode PixelCellDiscriminator::initialize() {
  StatusCode sc = AthAlgTool::initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelCellDiscriminator::initialize() failed");
    return sc ;
  }
  if (m_TimeSvc.retrieve().isFailure()) {
	ATH_MSG_ERROR("Can't get TimeSvc");
	return StatusCode::FAILURE;
  }
  if (m_CalibSvc.retrieve().isFailure()) {
	ATH_MSG_ERROR("Can't get CalibSvc");
	return StatusCode::FAILURE;
  }
  if (m_rndmSvc.retrieve().isFailure()) {
	ATH_MSG_ERROR("Can't get RndmSvc");
	return StatusCode::FAILURE;
  }
  else {
	ATH_MSG_DEBUG("Retrieved RndmSvc");
  }
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (m_rndmEngine==0) {
	ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
	return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG ( "PixelCellDiscriminator::initialize()");
  return sc ;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode PixelCellDiscriminator::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelCellDiscriminator::finalize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "PixelCellDiscriminator::finalize()");
  return sc ;
}

// process the collection of charged diodes
void PixelCellDiscriminator::process(SiChargedDiodeCollection &collection) const
{   
  // discriminator is applied to all cells, even unconnected ones to be
  /// able to use the unconnected cells as well

  for(SiChargedDiodeIterator i_chargedDiode=collection.begin() ;
      i_chargedDiode!=collection.end() ; ++i_chargedDiode) {

    int BCN = m_TimeSvc->getTimeBCN();

    Identifier diodeID = collection.getId((*i_chargedDiode).first);

    //
    // calculate the threshold:
    //    th0      : mean value
    //    th0 sigma: variation of whatever sample the mean was obtained
    //    noise    : actually a noise level that logically should be added to
    //               the charge <- TODO!
    //
    double th0  = m_CalibSvc->getCalThreshold(diodeID);
    double ith0 = m_CalibSvc->getCalIntimeThreshold(diodeID);
    // Flers: here I rely on CalibSvc providing correct values for th0, ith0
    // if that's not true, need to figure out if we are in dbm and set
    // th0, ith0, e.g.
    //                      if (dbm) { th0=1200.; ith0=1500.; }
    double threshold = th0 +
      m_CalibSvc->getCalThresholdSigma(diodeID)*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine) +
      m_CalibSvc->getCalNoise(diodeID)*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
     
    double intimethreshold  =  (ith0/th0)*threshold;
    // double overdrive        =  intimethreshold - threshold ;
    // compute overcharge for this diode
    double overcharge=(*i_chargedDiode).second.charge()-threshold;
    // if the discriminator fires
    if (overcharge>0) {
      // compute the relative bunch number 
      // if hit comes from track or Xtalk calculates Bunch Crossing with TimeWalk (timing algoritm)
      // else (i.e. is a noise hit or ...) assigns a random BC
      int bunch;
      if( (*i_chargedDiode).second.totalCharge().fromTrack()){
        bunch=m_TimeSvc->relativeBunch(threshold,
                                       intimethreshold,
                                       (*i_chargedDiode).second.totalCharge());
      } else {
        bunch=CLHEP::RandFlat::shootInt(m_rndmEngine,BCN);
      }
      if (bunch<0 || bunch>BCN) { //If bunch is out of range, set below threshold (keep hits with +1 BC, for hit duplication). 
        SiHelper::belowThreshold((*i_chargedDiode).second,true,true);
      } else {  
        SiHelper::SetBunch((*i_chargedDiode).second,bunch);
      }
      //ATH_MSG_DEBUG ( (*i_chargedDiode).second.totalCharge().fromTrack() <<" Charge = " << (*i_chargedDiode).second.charge()
      //<< " , bunch = " << bunch << " , BCN = " << BCN);
    } else {
      //Set the Below threshold flag if charge < threshold
      SiHelper::belowThreshold((*i_chargedDiode).second,true,true);
    }
  }
}
