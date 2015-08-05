#include <map>
#include <numeric>
#include <algorithm>
#include <sstream>

#include "GaudiKernel/MsgStream.h"
#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"

#include "TrigT1CaloEvent/CMXJetTob.h"
#include "TrigT1CaloEvent/CMXJetHits.h"
#include "TrigT1CaloEvent/JEMTobRoI.h"
#include "TrigT1CaloEvent/JetROI.h"
#include "TrigT1CaloUtils/JetAlgorithm.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloTools/L1JetCMXTools.h"

namespace LVL1 {

/** Constructor */

L1JetCMXTools::L1JetCMXTools(const std::string& type,
                             const std::string& name,
                             const IInterface*  parent)
  :
  AthAlgTool(type, name, parent),
  m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
  m_crates(2), m_modules(16), m_maxTobs(4), m_sysCrate(1),
  m_debug(false)
{
  declareInterface<IL1JetCMXTools>(this);
  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
}

/** Destructor */

L1JetCMXTools::~L1JetCMXTools()
{       
}


/** Initialisation */

StatusCode L1JetCMXTools::initialize()
{
  m_debug = msgLvl(MSG::DEBUG);

  // Connect to the LVL1ConfigSvc for the trigger configuration:

  StatusCode sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Couldn't connect to " << m_configSvc.typeAndName() 
                    << endreq;
    return sc;
  } else if (m_debug) {
    msg(MSG::DEBUG) << "Connected to " << m_configSvc.typeAndName()
                    << endreq;
  }
  
  msg(MSG::INFO) << "Initialization completed" << endreq;
  
  return sc;
}

/** Finalisation */

StatusCode L1JetCMXTools::finalize()
{
  return StatusCode::SUCCESS;
}

/** JetAlgorithm to JEMTobRoI conversion */

void L1JetCMXTools::formJEMTobRoI(const DataVector<JetAlgorithm>* jetAlgorithmVec,
                                        DataVector<JEMTobRoI>*    jemRoiVec) const
{
  JEPRoIDecoder decoder;
  DataVector<JetAlgorithm>::const_iterator pos  = jetAlgorithmVec->begin();
  DataVector<JetAlgorithm>::const_iterator pose = jetAlgorithmVec->end();
  for (; pos != pose; ++pos) {
    // Temporary until JetAlgorithm/RoIWord updated
    if ((*pos)->Hits()) {
      const unsigned int roiWord = (*pos)->RoIWord();
      const int crate = (roiWord>>28)&0x1;
      const int jem   = (roiWord>>24)&0xf;
      const int frame = (roiWord>>21)&0x7;
      const int loc   = (roiWord>>19)&0x3;
      const int energyLg = (*pos)->ET8x8();                     //<<== CHECK
      const int energySm = (*pos)->ET4x4();                     //<<== CHECK
      JEMTobRoI* roi = new JEMTobRoI(crate, jem, frame, loc, energyLg, energySm);
      jemRoiVec->push_back(roi);
    }
  }
}

/** JetROI to JEMTobRoI conversion */

void L1JetCMXTools::formJEMTobRoI(const DataVector<JetROI>* jetRoiVec,
                                        DataVector<JEMTobRoI>* jemRoiVec) const
{
  DataVector<JetROI>::const_iterator pos  = jetRoiVec->begin();
  DataVector<JetROI>::const_iterator pose = jetRoiVec->end();
  for (; pos != pose; ++pos) {
    // Temporary until JetROI/roiWord updated
    const unsigned int roiWord = (*pos)->roiWord();
    if (roiWord&0xfff) {
      const int crate = (roiWord>>28)&0x1;
      const int jem   = (roiWord>>24)&0xf;
      const int frame = (roiWord>>21)&0x7;
      const int loc   = (roiWord>>19)&0x3;
      const int energyLg = (*pos)->clusterEnergy8();                     //<<== CHECK
      const int energySm = (*pos)->clusterEnergy4();                     //<<== CHECK
      JEMTobRoI* roi = new JEMTobRoI(crate, jem, frame, loc, energyLg, energySm);
      jemRoiVec->push_back(roi);
    }
  }
}

/** form CMX-Jet TOBs from RoIs - single slice */

void L1JetCMXTools::formCMXJetTob(const DataVector<JEMTobRoI>* jemRoiVec,
                                        DataVector<CMXJetTob>* cmxTobVec) const
{
  std::vector<const DataVector<JEMTobRoI>*> jemRoiColls(1, jemRoiVec);
  formCMXJetTob(jemRoiColls, cmxTobVec, 0);
}

/** form CMX-Jet TOBs from RoIs - multiple slices */

void L1JetCMXTools::formCMXJetTob(
                    const std::vector<const DataVector<JEMTobRoI>*>& jemRoiColls,
		    DataVector<CMXJetTob>* cmxTobVec, int peak) const
{
  std::map<uint32_t, const JEMTobRoI*> jemRoiMap;
  std::map<int, CMXJetTob*>    cmxTobMap;
  DataVector<JEMTobRoI>::const_iterator it;
  int timeslices = jemRoiColls.size();
  for (int slice = 0; slice < timeslices; ++slice) {
    const DataVector<JEMTobRoI>* jemRoiVec = jemRoiColls[slice];
    jemRoiMap.clear();
    std::vector<unsigned int> presenceMaps(m_crates*m_modules);
    std::vector<int>          tobCount(m_crates*m_modules);
    DataVector<JEMTobRoI>::const_iterator it  = jemRoiVec->begin();
    DataVector<JEMTobRoI>::const_iterator itE = jemRoiVec->end();
    for (; it != itE; ++it) { // get sorted list
      const JEMTobRoI* roi = *it;
      const int crate = roi->crate();
      const int jem = roi->jem();
      const int index = crate*m_modules + jem;
      const int presenceBit = roi->frame();                                     // <<== CHECK
      presenceMaps[index] |= (1<<presenceBit);
      tobCount[index]++;
      uint32_t key = roi->roiWord();
      jemRoiMap.insert(std::make_pair(key, roi));
    }
    std::map<uint32_t, const JEMTobRoI*>::const_iterator mit  = jemRoiMap.begin();
    std::map<uint32_t, const JEMTobRoI*>::const_iterator mitE = jemRoiMap.end();
    for (; mit != mitE; ++mit) {
      const JEMTobRoI* roi = mit->second;
      const int crate = roi->crate();
      const int jem = roi->jem();
      const int frame = roi->frame();
      const int loc = roi->location();
      const int index = crate*m_modules + jem;
      const int energyLg = roi->energyLarge();
      const int energySm = roi->energySmall();
      const unsigned int presence = presenceMaps[index];
      int error = 0;
      if (tobCount[index]>m_maxTobs) { // overflow
	int count = 0;
        for (int bit = 0; bit <= frame; ++bit) count += (presence>>bit)&0x1;
	if (count > m_maxTobs) continue;
        LVL1::DataError err;
	err.set(LVL1::DataError::Overflow);
	error = err.error();
      }
      const int key = (((((crate<<4)|jem)<<3)|frame)<<2)|loc;
      CMXJetTob* tob = 0;
      std::map<int, CMXJetTob*>::iterator xit = cmxTobMap.find(key);
      if (xit == cmxTobMap.end()) {
        tob = new CMXJetTob(crate, jem, frame, loc);
	if (timeslices > 1) {
	  std::vector<int> vecI(timeslices);
	  std::vector<unsigned int> vecU(timeslices);
	  tob->addTob(vecI, vecI, vecI, vecU);
	  tob->setPeak(peak);
        }
	cmxTobMap.insert(std::make_pair(key, tob));
	cmxTobVec->push_back(tob);
      } else tob = xit->second;
      std::vector<int> energyLgVec(tob->energyLgVec());
      std::vector<int> energySmVec(tob->energySmVec());
      std::vector<int> errorVec(tob->errorVec());
      std::vector<unsigned int> presenceMapVec(tob->presenceMapVec());
      energyLgVec[slice] = energyLg;
      energySmVec[slice] = energySm;
      errorVec[slice] = error;
      presenceMapVec[slice] = presence;
      tob->addTob(energyLgVec, energySmVec, errorVec, presenceMapVec);
    }
  }
}

/** form complete CMX-Jet hits from CMX-Jet TOBs */

void L1JetCMXTools::formCMXJetHits(const DataVector<CMXJetTob>* cmxTobVec,
                                   DataVector<CMXJetHits>* cmxHitsVec) const
{
  DataVector<CMXJetHits>* cmxHitsCrate = new DataVector<CMXJetHits>;
  DataVector<CMXJetHits>* cmxHitsSys   = new DataVector<CMXJetHits>;
  DataVector<CMXJetHits>* cmxHitsTopo  = new DataVector<CMXJetHits>;
  formCMXJetHitsCrate(cmxTobVec, cmxHitsCrate);
  formCMXJetHitsSystem(cmxHitsCrate, cmxHitsSys);
  formCMXJetHitsTopo(cmxTobVec, cmxHitsTopo);
  mergeCMXJetHits(cmxHitsVec, cmxHitsCrate);
  mergeCMXJetHits(cmxHitsVec, cmxHitsSys);
  mergeCMXJetHits(cmxHitsVec, cmxHitsTopo);
  delete cmxHitsCrate;
  delete cmxHitsSys;
  delete cmxHitsTopo;
}

/** form partial CMX-Jet hits (crate) from CMX-Jet TOBs */

void L1JetCMXTools::formCMXJetHitsCrate(const DataVector<CMXJetTob>* cmxTobVec,
                                        DataVector<CMXJetHits>* cmxHitsCrate) const
{
  int peakm = 0;
  std::vector<HitsVector> hitVecM(2*m_crates);
  std::vector<HitsVector> hitVecF(2*m_crates);
  std::vector<ErrorVector> errVecM(2*m_crates); // Need overflow
  std::vector<ErrorVector> errVecF(2*m_crates);
  HitsVector hit0;
  HitsVector hit1;
  DataVector<CMXJetTob>::const_iterator pos  = cmxTobVec->begin();
  DataVector<CMXJetTob>::const_iterator pose = cmxTobVec->end();
  for (; pos != pose; ++pos) {
    const CMXJetTob* tob = *pos;
    const int index = 2*tob->crate();
    const int jem = tob->jem();
    const bool forward = (jem == 0 || jem == 7 || jem == 8 || jem == 15);
    const HitsType type = (forward) ? FORWARD_HITS : MAIN_HITS;
    const std::vector<int>& error(tob->errorVec());
    hit0.clear();
    hit1.clear();
    getHits(tob, hit0, hit1);
    if (forward) {
      addCMXJetHits(hitVecF[index],   hit0, type);
      addCMXJetHits(hitVecF[index+1], hit1, type);
    } else {
      addCMXJetHits(hitVecM[index],   hit0, type);
      addCMXJetHits(hitVecM[index+1], hit1, type);
    }
    addOverflow(errVecF[index],   error);
    addOverflow(errVecF[index+1], error);
    addOverflow(errVecM[index],   error);
    addOverflow(errVecM[index+1], error);
    const int peak = tob->peak();
    if (peak > peakm) peakm = peak;
  }
  // Save non-zero crate totals
  for (int crate = 0; crate < m_crates; ++crate) {
    const int index = crate*2;
    saveCMXJetHits(cmxHitsCrate, hitVecM[index], hitVecM[index+1],
                   errVecM[index], errVecM[index+1], crate,
		   LVL1::CMXJetHits::LOCAL_MAIN, peakm);
    saveCMXJetHits(cmxHitsCrate, hitVecF[index], hitVecF[index+1],
                   errVecF[index], errVecF[index+1], crate,
		   LVL1::CMXJetHits::LOCAL_FORWARD, peakm);
    if (crate != m_sysCrate) { // REMOTE totals
      saveCMXJetHits(cmxHitsCrate, hitVecM[index], hitVecM[index+1],
                     errVecM[index], errVecM[index+1], m_sysCrate,
		     LVL1::CMXJetHits::REMOTE_MAIN, peakm);
      saveCMXJetHits(cmxHitsCrate, hitVecF[index], hitVecF[index+1],
                     errVecF[index], errVecF[index+1], m_sysCrate,
		     LVL1::CMXJetHits::REMOTE_FORWARD, peakm);
    }
  }
}

/** form partial CMX-Jet hits (system) from crate CMX-Jet hits */

void L1JetCMXTools::formCMXJetHitsSystem(
                                 const DataVector<CMXJetHits>* cmxHitsCrate,
				 DataVector<CMXJetHits>* cmxHitsSys) const
{
  int peakm = 0;
  HitsVector systemMain0(1);
  HitsVector systemMain1(1);
  HitsVector systemFwd0(1);
  HitsVector systemFwd1(1);
  ErrorVector errVec(1);
  DataVector<CMXJetHits>::const_iterator pos  = cmxHitsCrate->begin();
  DataVector<CMXJetHits>::const_iterator pose = cmxHitsCrate->end();
  for (; pos != pose; ++pos) {
    const CMXJetHits* hits = *pos;
    if (hits->crate() != m_sysCrate) continue;
    int source = hits->source();
    if (source != CMXJetHits::LOCAL_MAIN    &&
        source != CMXJetHits::LOCAL_FORWARD &&
        source != CMXJetHits::REMOTE_MAIN   &&
	source != CMXJetHits::REMOTE_FORWARD) continue;
    int peak   = hits->peak();
    if (peak > peakm) peakm = peak;
    HitsVector hitsVec0(hits->hitsVec0());
    HitsVector hitsVec1(hits->hitsVec1());
    if (source == CMXJetHits::LOCAL_MAIN || source == CMXJetHits::REMOTE_MAIN) {
      addCMXJetHits(systemMain0, hitsVec0, MAIN_HITS);
      addCMXJetHits(systemMain1, hitsVec1, MAIN_HITS);
    } else {
      addCMXJetHits(systemFwd0,  hitsVec0, FORWARD_HITS);
      addCMXJetHits(systemFwd1,  hitsVec1, FORWARD_HITS);
    }
    ErrorVector error(hits->errorVec0()); // all have same error so redundant?
    addOverflow(errVec, error);
  }
  // Save non-zero system totals
  saveCMXJetHits(cmxHitsSys, systemMain0, systemMain1, errVec, errVec,
                 m_sysCrate, CMXJetHits::TOTAL_MAIN, peakm);
  saveCMXJetHits(cmxHitsSys, systemFwd0, systemFwd1, errVec, errVec,
                 m_sysCrate, CMXJetHits::TOTAL_FORWARD, peakm);
}

/** form partial CMX-Jet hits (topo) from CMX-Jet TOBs */                   // Temporary for testing

void L1JetCMXTools::formCMXJetHitsTopo(const DataVector<CMXJetTob>* cmxTobVec,
                                       DataVector<CMXJetHits>* cmxHitsTopo) const
{
  int peakm = 0;
  int timeslices = 0;
  std::vector<HitsVector> hitVec(4*m_crates);
  DataVector<CMXJetTob>::const_iterator pos  = cmxTobVec->begin();
  DataVector<CMXJetTob>::const_iterator pose = cmxTobVec->end();
  for (; pos != pose; ++pos) {
    const CMXJetTob* tob = *pos;
    const int crate = tob->crate();
    const int jem = tob->jem();
    const int frame = tob->frame();
    const int loc = tob->location();
    const int index = crate*4;
    const std::vector<int>& energyLg(tob->energyLgVec());
    const std::vector<int>& energySm(tob->energySmVec());
    //const std::vector<int>& error(tob->errorVec());
    timeslices = energyLg.size();
    HitsVector& checksum(hitVec[index]);
    HitsVector& map(hitVec[index+1]);
    HitsVector& countsLow(hitVec[index+2]);
    HitsVector& countsHigh(hitVec[index+3]);
    checksum.resize(timeslices);
    map.resize(timeslices);
    countsLow.resize(timeslices);
    countsHigh.resize(timeslices);
    for (int slice = 0; slice < timeslices; ++slice) {
      if (energyLg[slice] == 0 && energySm[slice] == 0) continue;
      // checksum
      //LVL1::DataError err(error[slice]);
      //const int overflow = err.get(LVL1::DataError::Overflow);
      const int overflow = 0;// don't include overflow as not in slink data
      checksum[slice] += jem + frame + loc + energyLg[slice]
                             + energySm[slice] + overflow;
      checksum[slice] &= 0xffff;
      // occupancy map
      map[slice] |= (1<<jem);
      // occupancy counts
      if (jem < 8) {
        countsLow[slice] += (1<<(3*jem)); // can't saturate
      } else {
        countsHigh[slice] += (1<<(3*(jem-8)));
      }
    }
    const int peak = tob->peak();
    if (peak > peakm) peakm = peak;
  }
  // Save non-zero crate totals
  HitsVector dummy(timeslices);
  ErrorVector dummyE(timeslices);
  for (int crate = 0; crate < m_crates; ++crate) {
    const int index = crate*4;
    saveCMXJetHits(cmxHitsTopo, hitVec[index], dummy,
                   dummyE, dummyE,
                   crate, CMXJetHits::TOPO_CHECKSUM, peakm);
    saveCMXJetHits(cmxHitsTopo, hitVec[index+1], dummy,
                   dummyE, dummyE,
                   crate, CMXJetHits::TOPO_OCCUPANCY_MAP, peakm);
    saveCMXJetHits(cmxHitsTopo, hitVec[index+2], hitVec[index+3],
                   dummyE, dummyE,
                   crate, CMXJetHits::TOPO_OCCUPANCY_COUNTS, peakm);
  }
}

/** Temporary for testing, mostly lifted from JetAlgorithm */

void L1JetCMXTools::getHits(const CMXJetTob* tob,
			    HitsVector& hit0, HitsVector& hit1) const
{
  using namespace TrigConf;
  const std::vector<int>& energyLg(tob->energyLgVec());
  const std::vector<int>& energySm(tob->energySmVec());
  const int jem = tob->jem();
  const bool forward = (jem == 0 || jem == 7 || jem == 8 || jem == 15);
  const int timeslices = energyLg.size();
  hit0.assign(timeslices, 0);
  hit1.assign(timeslices, 0);
  L1DataDef def;
  // Get eta/phi using old decoder
  JEPRoIDecoder decoder;
  const uint32_t oldRoiWord = ((((((tob->crate()<<4)|jem)<<3)|tob->frame())<<2)|tob->location())<<19;
  const CoordinateRange coord = decoder.coordinate(oldRoiWord);
  const double eta = coord.eta();
  const double phi = coord.phi();
  /** Use reference JE for coordinate.
      Thresholds can be set at full granularity in eta, but module granularity in phi */
  // protect against rounding errors (as JE centre == TT edge)
  int ieta = int((eta + (eta>0 ? 0.005 : -0.005))/0.1);
  int iphi = int((phi-0.005)*32/M_PI);
  iphi = 16*(iphi/16) + 8;
  std::string jetTriggerType = def.jetType();
  if (forward) jetTriggerType = ( (eta > 0) ? def.jfType() : def.jbType() );
  //std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu()->thresholdVector(); // vkousk@@
  std::vector<TrigConf::TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TriggerThreshold*>::const_iterator it;
  std::vector<TriggerThreshold*>::const_iterator itE = thresholds.end();
  for (int slice = 0; slice < timeslices; ++slice) {
    if (energyLg[slice] == 0 && energySm[slice] == 0) continue;
    for (it = thresholds.begin(); it != itE; ++it) {
      if ((*it)->type()==jetTriggerType){  //only use Jet Thresholds
        int threshNum = (*it)->thresholdNumber();
        if (threshNum >= 0 && threshNum < (int)TrigT1CaloDefs::numOfJetThresholds) {
          int size = 0;
          int value = 1023;  // Set impossible default in case no threshold found
          TriggerThresholdValue* tv = (*it)->triggerThresholdValue(ieta,iphi);
          if (tv != 0) { 
            JetThresholdValue* jtv;
            jtv = dynamic_cast<JetThresholdValue*> (tv);
            if (jtv) {
              size=jtv->window();
              value = jtv->thresholdValueCount();
            }
          }
          if (m_debug) {
            msg(MSG::DEBUG) << "JetAlgorithm: Test jet threshold " << threshNum
	                    << " with type = " << jetTriggerType << ", threshold = "
			    << value << " and window = " << size << endreq;
          }
          bool passes = false;
          switch (size){
            case 4 :
                    passes = (energySm[slice] > value);                            //<<== CHECK
                    break;
            case 6 :
                    //passes = (ET6x6() > value);
                    break;
            case 8 :
                    passes = (energyLg[slice] > value);
                    break;
            default :
	            if (m_debug) {
	              msg(MSG::DEBUG) << "ERROR IN JetAlgorithm WITH COORDS "
		                      << phi << ", " << eta << ". WINDOW SIZE OF "
		    		      << size << " NOT RECOGNISED" << endreq;
	            }
          }//end switch

          /** Set bit if passed */
          if (passes) {
	    if (forward) {
	      if (threshNum < 8) hit0[slice] |= (1<<(threshNum*2));
	      else if(TrigT1CaloDefs::numOfJetThresholds < 8) hit1[slice] |= (1<<((threshNum-8)*2)); // for coverity issue # 29171
            } else {
	      if (threshNum < 5) hit0[slice] |= (1<<(threshNum*3));
	      else               hit1[slice] |= (1<<((threshNum-5)*3));
	    }
          }
        } // end if valid threshold
      }//endif - is jet threshold
    }//end thresh for-loop
  }//end slice for-loop
}

/** Add overflow bit */

void L1JetCMXTools::addOverflow(ErrorVector& hitErr, const ErrorVector& tobErr) const
{
  const int timeslices = tobErr.size();
  hitErr.resize(timeslices);
  for (int slice = 0; slice < timeslices; ++slice) {
    int error = tobErr[slice];
    if (error) {
      LVL1::DataError err(error);
      int overflow = err.get(LVL1::DataError::Overflow);
      LVL1::DataError err2;
      err2.set(LVL1::DataError::Overflow, overflow);
      hitErr[slice] |= err2.error();
    }
  }
}

/** Add hits from second vector to first */

void L1JetCMXTools::addCMXJetHits(HitsVector& vec1,
                            const HitsVector& vec2, HitsType type) const
{
  int size1 = vec1.size();
  int size2 = vec2.size();
  if (size1 < size2) vec1.resize(size2);
  HitsVector::iterator pos1  = vec1.begin();
  HitsVector::iterator pose1 = vec1.end();
  HitsVector::const_iterator pos2  = vec2.begin();
  HitsVector::const_iterator pose2 = vec2.end();
  for (; pos1 != pose1 && pos2 != pose2; ++pos1, ++pos2) {
    if (type == MAIN_HITS) *pos1 = addHits(*pos1, *pos2, 15, 15, 5);
    else                   *pos1 = addHits(*pos1, *pos2, 16, 16, 8);
  }
}

/** Increment JEM/CMX hit word */

unsigned int L1JetCMXTools::addHits(unsigned int hitMult, unsigned int hitVec,
			     int multBits, int vecBits, int nthresh) const
{
  if (m_debug) 
    msg(MSG::DEBUG) << "addHits: Original hitMult = " << std::hex << hitMult
                    << ". Add hitWord = " << hitVec << std::dec << endreq;
  
  int nbitsOut = multBits/nthresh;
  int nbitsIn  = vecBits/nthresh;
  
  if (m_debug) msg(MSG::DEBUG) << " Bits per threshold = " << nbitsOut
                               << endreq;
  
  int max = (1<<nbitsOut) - 1;
  unsigned int multMask = max;
  unsigned int hitMask = (1<<nbitsIn) - 1;
  unsigned int shift = 0;
  
  unsigned int hits = 0;
    
  for (int i = 0; i < nthresh; i++) {
    int mult = (hitMult&multMask) + (hitVec&hitMask);
    mult = ( (mult<=max) ? mult : max);
    hits += (mult<<shift);
    
    hitMult >>= nbitsOut;
    hitVec  >>= nbitsIn;
    shift += nbitsOut;
  }
  
  if (m_debug) msg(MSG::DEBUG) << "addHits returning hitMult = "
                     << std::hex << hits << std::dec << endreq;
  
  return hits;
}

/** Merge CMX-Jet hits vectors */

void L1JetCMXTools::mergeCMXJetHits(DataVector<CMXJetHits>* cmxHitsVec1,
                                    DataVector<CMXJetHits>* cmxHitsVec2) const
{
  int size = cmxHitsVec2->size();
  for (int index = 0; index < size; ++index) {
    CMXJetHits* hitsIn  = 0;
    CMXJetHits* hitsOut = 0;
    cmxHitsVec2->swapElement(index, hitsIn, hitsOut);
    cmxHitsVec1->push_back(hitsOut);
  }
  cmxHitsVec2->clear();
}

/** Save non-zero CMX-Jet hits */

void L1JetCMXTools::saveCMXJetHits(DataVector<CMXJetHits>* cmxHitsVec,
                                   const HitsVector& hits0,
				   const HitsVector& hits1,
				   const ErrorVector& err0,
				   const ErrorVector& err1,
                                   int crate, int source, int peak) const
{
  if (std::accumulate(hits0.begin(), hits0.end(), 0) ||
      std::accumulate(hits1.begin(), hits1.end(), 0)) {
    cmxHitsVec->push_back(new CMXJetHits(crate, source, hits0, hits1,
                                                err0, err1, peak));
  }
}

} // end of namespace
