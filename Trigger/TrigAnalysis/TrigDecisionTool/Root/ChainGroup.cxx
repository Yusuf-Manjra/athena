/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : ChainGroup
 *
 * @brief simple container to hold trigger chains
 *
 * @author Michael Begel  <michael.begel@cern.ch> - Brookhaven National Laboratory
 * @author Alexander Mann  <mann@cern.ch> - University of Goettingen
 *
 ***********************************************************************************/
#include <limits>
#include "boost/regex.hpp"
#include "boost/range/adaptor/reversed.hpp"

#include "CxxUtils/bitmask.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTStreamTag.h"
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTUtils/HLTUtils.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/TriggerItemNode.h"
#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/Lvl1Item.h"
#include "TrigNavStructure/ComboIterator.h"
#include "xAODTrigger/TrigCompositeContainer.h"

#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/TDTUtilities.h"
#include "TrigDecisionTool/Logger.h"



using namespace std;

Trig::ChainGroup::ChainGroup(const std::vector< std::string >& triggerNames,
                             Trig::CacheGlobalMemory& parent)
  :
  m_patterns(triggerNames),
  m_cgm(parent),
  m_prescale(0.)
{}

const Trig::ChainGroup& Trig::ChainGroup::operator+(const Trig::ChainGroup& rhs) {
  std::vector< std::string > v;
  v.resize(patterns().size()+rhs.patterns().size());
  merge(patterns().begin(),patterns().end(),
	(rhs.patterns()).begin(),(rhs.patterns()).end(),
	v.begin());
  return *(cgm().createChainGroup(v));
}


bool Trig::ChainGroup::operator==(const Trig::ChainGroup& rhs) {
  std::vector< std::string > v1=Trig::keyWrap(names());
  std::vector< std::string > v2=Trig::keyWrap(rhs.names());
  if (v1==v2) return true;
  return false;
}

bool Trig::ChainGroup::operator!=(const Trig::ChainGroup& rhs) {
  std::vector< std::string > v1=Trig::keyWrap(names());
  std::vector< std::string > v2=Trig::keyWrap(rhs.names());
  if (v1!=v2) return true;
  return false;
}

void Trig::ChainGroup::addAlias(const std::string& alias) {
  cgm().createChainGroup(patterns(),alias);
}

const Trig::CacheGlobalMemory& Trig::ChainGroup::cgm_assert() const {
  // thread-safe because assert_decision is locked
  auto nonconst_cgm ATLAS_THREAD_SAFE = const_cast<Trig::CacheGlobalMemory*>(&cgm());
  nonconst_cgm->assert_decision();

  return cgm();
}

bool Trig::ChainGroup::HLTResult(const std::string& chain, unsigned int condition) const {
  bool chainRESULT = false;
  if (chain=="") return chainRESULT;
  const HLT::Chain* fchain=cgm_assert().chain(chain);
  if (fchain==0) return chainRESULT;
  
  
  bool RAW =         fchain->chainPassedRaw();
  bool PASSTHROUGH = fchain->isPassedThrough();
  bool PRESCALED   = fchain->isPrescaled();
  bool RESURRECTED = fchain->isResurrected();
  
  // Resurrection overwrites the value in RAW but sets the RESURRECTED flag
  // we should therefore fix RAW appropriately
  if (~condition & TrigDefs::allowResurrectedDecision) {
    if (RESURRECTED) {
      RAW=false;
    }
  }
  //
  // Do we accept the result?
  //
  if (condition & TrigDefs::passedThrough) {
    if (PASSTHROUGH) {chainRESULT=true;}
  }
  if (condition & TrigDefs::requireDecision) {
    if (RAW && !PRESCALED) {chainRESULT=true;}
    if ( condition & TrigDefs::allowResurrectedDecision ) { // prescaling does not matter for RR (it runs in fact because of that
      if (RAW) {chainRESULT=true;}      
    }

  }
  // respects resurrection -- is this the appropriate behavior???
  if (condition & TrigDefs::eventAccepted) {
    if ( (RAW  && !PRESCALED) ||  PASSTHROUGH) {chainRESULT=true;}
  }
  ATH_MSG_DEBUG("ChainGroup::HLTResult Counter = " << std::setw(4) << fchain->getChainCounter()
		<< " name = "  << fchain->getChainName()
		<< " level = " << fchain->getConfigChain()->level()
		<< " success (raw) = " << fchain->chainPassedRaw()
		<< " pass-through = " << fchain->isPassedThrough()
		<< " prescaled = " << fchain->isPrescaled()
		<< " rerun = " << fchain->isResurrected()
		<< " lastActiveStep = " << fchain->getChainStep()
		<< " name = " << std::setw(35) << fchain->getChainName() 
		<< " result = " << chainRESULT);

  return chainRESULT;
}

// this logic fails for passthrough especially with enforceLogicalFlow!!!!
bool Trig::ChainGroup::L1Result(const std::string& item, unsigned int condition) const {
  bool r = false;
  if (item=="") return r;
  if (item.find(',')!=std::string::npos) {
    std::vector< std::string > items = convertStringToVector(item);
    for(const std::string& item : items) {
      if(L1Result(item,condition)) return true;
    }
    return false;
  }
  const LVL1CTP::Lvl1Item* fitem=cgm_assert().item(item);
  if (fitem==0) {
    return r;
  }
  ATH_MSG_DEBUG(" success (raw) = " << fitem->isPassedBeforePrescale()
		<< " prescaled = " << fitem->isPrescaled()
		<< " vetoed = " << fitem->isVeto()
		<< " name = " << std::setw(35) << fitem->name()); 

  r = fitem->isPassedAfterVeto();

  if (condition & TrigDefs::allowResurrectedDecision)
    r = fitem->isPassedBeforePrescale();

  //if (condition & TrigDefs::eventAccepted) 
  //  r = fitem->isPassedAfterVeto();
  //else
  //  r = fitem->isPassedBeforePrescale();
  return r;
}


std::string Trig::ChainGroup::getLowerName(const std::string& name) const {
  if ( name == "" )
    return "";
  const TrigConf::HLTChain* cchain = cgm().config_chain(name);
  if (cchain==0){
    ATH_MSG_WARNING(" Lower chain name used by:  " << name << " is not in the configuration ");
    return "BAD NAME";
  }
  return cchain->lower_chain_name();
}

  

bool Trig::ChainGroup::isPassed(unsigned int condition) const 
{

  //ATH_MSG_DEBUG(" Got CG to work with " << patterns());
  //ATH_MSG_DEBUG(" Got CG to work with " << names());

  ChainGroup::const_conf_chain_iterator chIt;

  bool RESULT = false;
  std::string nexttwo;

  for ( chIt = conf_chain_begin(); chIt != conf_chain_end(); ++chIt) {
    bool chainRESULT=HLTResult((*chIt)->chain_name(),condition);
    if (chainRESULT && (condition & TrigDefs::enforceLogicalFlow)) {
      // enforceLogicalFlow
      if ((*chIt)->level()=="EF") {
        nexttwo=getLowerName((*chIt)->chain_name());
        chainRESULT = chainRESULT && HLTResult(nexttwo,condition);
        chainRESULT = chainRESULT && L1Result(getLowerName(nexttwo),condition);

      } else if ((*chIt)->level()=="L2") {
        chainRESULT = chainRESULT && L1Result(getLowerName((*chIt)->chain_name()),condition);      

      } else if ((*chIt)->level()=="HLT"){
	    chainRESULT = chainRESULT && L1Result(getLowerName((*chIt)->chain_name()),condition);
      }
      
    }
    RESULT = RESULT || chainRESULT;
  }
  ChainGroup::const_conf_item_iterator iIt;
  for ( iIt = conf_item_begin(); iIt != conf_item_end(); ++iIt) {
    RESULT = RESULT || L1Result((*iIt)->name(),condition);
  }

  if (condition == TrigDefs::Express_passed) {
    ATH_MSG_ERROR("Calling isPassed with TrigDefs::Express_passed is currently not supported");
  }

  return RESULT;
}

unsigned int Trig::ChainGroup::HLTBits(const std::string& chain, const std::string& level) const {
  unsigned int chainRESULT = 0;
  if (chain=="") return chainRESULT;
  const HLT::Chain* fchain = cgm_assert().chain(chain);
  if (fchain==0) return chainRESULT;
  if (level=="L2") {
    if (fchain->chainPassedRaw())  chainRESULT = chainRESULT | TrigDefs::L2_passedRaw;
    if (fchain->isPassedThrough()) chainRESULT = chainRESULT | TrigDefs::L2_passThrough;
    if (fchain->isPrescaled())     chainRESULT = chainRESULT | TrigDefs::L2_prescaled;
    if (fchain->isResurrected())   chainRESULT = chainRESULT | TrigDefs::L2_resurrected;
  } else {//L2EF merged use same EF bits
    if (fchain->chainPassedRaw())  chainRESULT = chainRESULT | TrigDefs::EF_passedRaw;
    if (fchain->isPassedThrough()) chainRESULT = chainRESULT | TrigDefs::EF_passThrough;
    if (fchain->isPrescaled())     chainRESULT = chainRESULT | TrigDefs::EF_prescaled;
    if (fchain->isResurrected())   chainRESULT = chainRESULT | TrigDefs::EF_resurrected;
  }
  return chainRESULT;
}

unsigned int Trig::ChainGroup::L1Bits(const std::string& item) const {
  unsigned int r = 0;
  if (item=="") return r;
  if (item.find(',')!=std::string::npos) {
    std::vector< std::string > items = convertStringToVector(item);
    for(const std::string& item : items) {
      r |= L1Bits(item);
    }
    return r;
  }
  const LVL1CTP::Lvl1Item* fitem = cgm_assert().item(item);
  if (fitem==0) return r;
  if (fitem->isPassedBeforePrescale()) r = r | TrigDefs::L1_isPassedBeforePrescale;
  if (fitem->isPassedAfterPrescale())  r = r | TrigDefs::L1_isPassedAfterPrescale;
  if (fitem->isPassedAfterVeto())      r = r | TrigDefs::L1_isPassedAfterVeto;
  return r;
}

unsigned int Trig::ChainGroup::isPassedBits() const 
{

  ChainGroup::const_conf_chain_iterator chIt;

  unsigned int RESULT = 0;
  //  unsigned int chainRESULT=0;
  std::string nexttwo;

  for ( chIt = conf_chain_begin(); chIt != conf_chain_end(); ++chIt) {

    RESULT = RESULT | HLTBits((*chIt)->chain_name(),(*chIt)->level());

    if ((*chIt)->level()=="EF") {
      nexttwo=getLowerName((*chIt)->chain_name());
      RESULT = RESULT | HLTBits(nexttwo,"L2");
      RESULT = RESULT | L1Bits(getLowerName(nexttwo));
      
    } else if ((*chIt)->level()=="L2") {
      RESULT = RESULT | L1Bits(getLowerName((*chIt)->chain_name()));
      
    } else if ((*chIt)->level()=="HLT") {
      RESULT = RESULT | L1Bits(getLowerName((*chIt)->chain_name()));
    }
    //    cout << "After looking at :" << (*chIt)->chain_name()
    //	 << " " << std::hex << RESULT << endl;
  }


  ChainGroup::const_conf_item_iterator iIt;
  for ( iIt = conf_item_begin(); iIt != conf_item_end(); ++iIt) {
    RESULT = RESULT | L1Bits((*iIt)->name());
    //    cout << "After looking at :" << (*iIt)->name() << std::hex << RESULT << endl;
  }
  return RESULT;
}


HLT::ErrorCode Trig::ChainGroup::error() const {
  HLT::ErrorCode errorCode = HLT::OK;  
  ChainGroup::const_conf_chain_iterator chIt;
  for ( chIt = conf_chain_begin(); chIt != conf_chain_end(); ++chIt) {
    const HLT::Chain* fchain = cgm_assert().chain((*chIt)->chain_name());
    if (fchain==0) continue; 
    HLT::ErrorCode ec = fchain->getErrorCode();
    errorCode = errorCode > ec ? errorCode : ec;
  }
  return errorCode;
}

float Trig::ChainGroup::HLTPrescale(const std::string& chain, unsigned int /*condition*/) const {
  if (chain=="") return 0.;

  const TrigConf::HLTChain* fchain=cgm().config_chain(chain);
  if (fchain==0) { // this is error condition, we always need configuration of the chains in the chaon group!
    ATH_MSG_WARNING("Configuration for the chain: " << chain << " not known");
    return std::numeric_limits<float>::quiet_NaN();
  }
  float chainRESULT = fchain->prescale();
  
  if (chainRESULT < 1)
    chainRESULT = 0.;
    
  return chainRESULT;
}


bool Trig::ChainGroup::isCorrelatedL1items(const std::string& item) const {
  if( (item == "L1_MU20,L1_MU21") || (item == "L1_MU21,L1_MU20") ) return true;
  return false;
}

float Trig::ChainGroup::correlatedL1Prescale(const std::string& item) const {
  if( (item == "L1_MU20,L1_MU21") || (item == "L1_MU21,L1_MU20") ) {
    //see discussion in ATR-16612
    auto l1mu20 = cgm().config_item("L1_MU20");
    if (l1mu20==nullptr) {
      ATH_MSG_WARNING("Configuration for the item L1_MU20 not known");
      return std::numeric_limits<float>::quiet_NaN();
    }
    float l1mu20ps = cgm().item_prescale(l1mu20->ctpId());
 
    auto l1mu21 = cgm().config_item("L1_MU21");
    if (l1mu21==nullptr) {
      ATH_MSG_WARNING("Configuration for the item L1_MU21 not known");
      return std::numeric_limits<float>::quiet_NaN();
    }
    float l1mu21ps = cgm().item_prescale(l1mu21->ctpId());
    
    if( (l1mu20ps  < 1.0) && (l1mu21ps < 1.0) ) return 0.0;
    if( (l1mu20ps  < 1.0) ) return l1mu21ps;
    if( (l1mu21ps  < 1.0) ) return l1mu20ps;
    if(l1mu20ps == 1.0) return 1.0;
    return 0.0;
  }
  return 0.0;
}

float Trig::ChainGroup::L1Prescale(const std::string& item, unsigned int /*condition*/) const {
  if (item=="") return 0;

  if(item.find(',')==std::string::npos) {
    const  TrigConf::TriggerItem* fitem=cgm().config_item(item);
    if (fitem==0) {
      ATH_MSG_WARNING("Configuration for the item: " << item << " not known");
      return std::numeric_limits<float>::quiet_NaN();
    }
    // now we can;t access the prescale value because this information doe not come togehther as in HLT
    // we need to go to the cache of L1 items and get it from there  
    int ctpid = fitem->ctpId();
    float itemprescale = cgm().item_prescale(ctpid);
    if ( itemprescale < 1)
      itemprescale = 0;
    return itemprescale;
  } else if(isCorrelatedL1items(item)) {
    return correlatedL1Prescale(item);
  } else {
    float minprescale=0;
    std::vector< std::string > items = convertStringToVector(item);
    for(const std::string& item : items) {

      const  TrigConf::TriggerItem* fitem=cgm().config_item(item);
      if (fitem==0) {
        ATH_MSG_WARNING("Configuration for the item: " << item << " not known");
        return std::numeric_limits<float>::quiet_NaN();
      }
      int ctpid = fitem->ctpId();
      float itemprescale = cgm().item_prescale(ctpid);
      if ( itemprescale < 1)
	itemprescale = 0;
      minprescale = (minprescale&&(minprescale<itemprescale)?minprescale:itemprescale); // takes min, except the first time
    }
    return minprescale;
  }
}

float Trig::ChainGroup::getPrescale(unsigned int condition) const {
  if ( condition != TrigDefs::Physics )
    return 0.0;
  return m_prescale;
}

float Trig::ChainGroup::calculatePrescale(unsigned int condition)
{
  bool singleTrigger = (m_confChains.size()+m_confItems.size()==1);
  
  ChainGroup::const_conf_chain_iterator chIt;
  for ( chIt = conf_chain_begin(); chIt != conf_chain_end(); ++chIt) {
    
    const std::string & hltChainName = (*chIt)->chain_name();
    float chainRESULT = HLTPrescale(hltChainName,condition);
    
    if (condition & TrigDefs::enforceLogicalFlow) {
      // enforceLogicalFlow
      if ((*chIt)->level()=="EF") {
        std::string hltChainNameL2 = getLowerName(hltChainName);
        std::string l1ItemName     = getLowerName(hltChainNameL2);
        chainRESULT *= HLTPrescale(hltChainNameL2,condition);
        chainRESULT *= L1Prescale(l1ItemName,condition);
        if(l1ItemName.find(',')!=std::string::npos) singleTrigger=false;
	
      } else if ((*chIt)->level()=="L2") {
        std::string l1ItemName       = getLowerName(hltChainName);
        chainRESULT *= L1Prescale(l1ItemName,condition);
        if(l1ItemName.find(',')!=std::string::npos) singleTrigger=false;

	//todo: not clear how to handle prescales in the merged system??
      } else if ((*chIt)->level()=="HLT") {
        std::string l1ItemName       = getLowerName(hltChainName);
        chainRESULT *= L1Prescale(l1ItemName,condition);
        if(l1ItemName.find(',')!=std::string::npos and !isCorrelatedL1items(l1ItemName) ) singleTrigger=false;
      }

    }

    if (singleTrigger) return chainRESULT;  // for a single trigger we are done

    bool UNPRESCALED = (fabs(chainRESULT-1.0)<1e-5);

    if (UNPRESCALED) return 1.0; // any unprescaled trigger and we are done too
  }


  ChainGroup::const_conf_item_iterator iIt;
  for ( iIt = conf_item_begin(); iIt != conf_item_end(); ++iIt) {
    const std::string & l1ItemName = (*iIt)->name();
    float itemRESULT = L1Prescale(l1ItemName,condition);
    if(l1ItemName.find(',')!=std::string::npos) singleTrigger=false;

    if (singleTrigger) return itemRESULT; // for a single trigger we are done

    bool UNPRESCALED = (itemRESULT==1);

    if (UNPRESCALED) return 1.0; // any unprescaled trigger and we are done too
  }

  return 0.0; // multiple triggers and all are prescaled
}


std::vector< std::string > Trig::ChainGroup::getListOfTriggers() const {
  std::vector< std::string > v;
  v.assign(m_names.begin(),m_names.end());
  return v;
}


std::vector< std::string > Trig::ChainGroup::getListOfStreams() const {
   std::set< std::string > s;
   std::set<const TrigConf::HLTChain*>::const_iterator foo;
   for (foo=conf_chain_begin(); foo != conf_chain_end(); ++foo) {
      const std::vector<TrigConf::HLTStreamTag*>& streamTagList = (*foo)->streams();
      std::vector<TrigConf::HLTStreamTag*>::const_iterator bar;
      for (bar=streamTagList.begin(); bar != streamTagList.end(); ++bar) {
         s.insert((*bar)->stream());
      }
   }
   std::vector< std::string > v;
   v.assign(s.begin(),s.end());
   return v;
}

//
// Groups
//

vector<string>
Trig::ChainGroup::getListOfGroups() const {

   vector< string > v;

   for( const TrigConf::HLTChain* ch : m_confChains )
      v.assign( ch->groups().begin(), ch->groups().end() );

   return v;
}

//
// Signatures
//

std::vector< std::string > Trig::ChainGroup::getListOfSignatures() const {
  std::set< std::string > s;
  std::set<const TrigConf::HLTChain*>::const_iterator foo;
  // HLT Chain
  for (foo=conf_chain_begin(); foo != conf_chain_end(); ++foo) {
    const std::vector<TrigConf::HLTSignature*>& signatureList = (*foo)->signatureList();
    std::vector<TrigConf::HLTSignature*>::const_iterator bar;
    for (bar=signatureList.begin(); bar != signatureList.end(); ++bar) {
      s.insert((*bar)->label());
    }
  }
  std::vector< std::string > v;
  v.assign(s.begin(),s.end());
  return v;
}


//
// Return level 1 thresholds
//

vector<string>
Trig::ChainGroup::getListOfThresholds() const {

   set<string> s;   // using a set makes the items in the result vector unique
   std::stack<const TrigConf::TriggerItemNode*> nodes;
   const TrigConf::TriggerItemNode* node;

   for( const TrigConf::TriggerItem* item : m_confItems ) {
      nodes.push( item->topNode() );
      while (!nodes.empty()) {
         node = nodes.top(); nodes.pop();
         if (node == NULL)
            continue;
         if (node->isThreshold()) {
            if (node->triggerThreshold()) {
               // available if thresholds have been read in
               if (!node->triggerThreshold()->name().empty())
                  s.insert(node->triggerThreshold()->name());
            } else if (!node->thresholdName().empty()) {
               // fall back solution
               s.insert(node->thresholdName());
            }
         } else {
           for(TrigConf::TriggerItemNode* childnode : node->children()) {
               nodes.push(childnode);
            }
         }
      }
      // I am not using (*it)->topNode()->getAllThresholds() here, because it returns nothing when only ItemDef (and not the thresholds themselves) are defined
   }

   vector<string> v;
   v.assign(s.begin(),s.end());
   return v;

}


//
// Trigger Elements
//

std::vector< std::vector< std::string > > Trig::ChainGroup::getListOfTriggerElements() const {
  std::set< std::vector< std::string > > s;

  std::vector< std::string > t;

  std::set<const TrigConf::HLTChain*>::const_iterator foo;
  for (foo=conf_chain_begin(); foo != conf_chain_end(); ++foo) {
    const std::vector<TrigConf::HLTSignature*>& signatureList = (*foo)->signatureList();
    std::vector<TrigConf::HLTSignature*>::const_iterator bar;
    for (bar=signatureList.begin(); bar != signatureList.end(); ++bar) {
      t.clear();
      const std::vector<TrigConf::HLTTriggerElement*>& outputTEs = (*bar)->outputTEs();
      std::vector<TrigConf::HLTTriggerElement*>::const_iterator foobar;
      for (foobar=outputTEs.begin(); foobar != outputTEs.end(); ++foobar) {
	t.push_back( (*foobar)->name());
      }
      s.insert(t);
    }
  }
  std::vector< std::vector< std::string > > v;
  v.assign(s.begin(),s.end());
  return v;
}


//
// get vector of vector with all Trigger Elements
//

std::vector< std::vector< TrigConf::HLTTriggerElement* > > Trig::ChainGroup::getHLTTriggerElements() const {
  std::set< std::vector< TrigConf::HLTTriggerElement* > > s;

  std::vector< TrigConf::HLTTriggerElement* > t;

  std::set<const TrigConf::HLTChain*>::const_iterator foo;
  for (foo=conf_chain_begin(); foo != conf_chain_end(); ++foo) {
    const std::vector<TrigConf::HLTSignature*>& signatureList = (*foo)->signatureList();
    std::vector<TrigConf::HLTSignature*>::const_iterator bar;
    for (bar=signatureList.begin(); bar != signatureList.end(); ++bar) {
      t.clear();
      const std::vector<TrigConf::HLTTriggerElement*>& outputTEs = (*bar)->outputTEs();
      std::vector<TrigConf::HLTTriggerElement*>::const_iterator foobar;
      for (foobar=outputTEs.begin(); foobar != outputTEs.end(); ++foobar) {
	t.push_back( (*foobar) );
      }
      s.insert(t);
    }
  }
  std::vector< std::vector< TrigConf::HLTTriggerElement* > > v;
  v.assign(s.begin(),s.end());
  return v;
}


void
Trig::ChainGroup::update(const TrigConf::HLTChainList* confChains,
                         const TrigConf::ItemContainer* confItems,
                         TrigDefs::Group prop) {

   m_confChains.clear();
   m_confItems.clear();
   m_names.clear();
   m_names.reserve(m_patterns.size());


   // protect against genConf failure
   if (!(confChains && confItems) ) return;

   if (!CxxUtils::test(prop, TrigDefs::Group::NoRegex)) {

     for(const std::string& pat : m_patterns) {
        // find chains matching pattern
        boost::regex compiled(pat);
        boost::cmatch what;

        for(TrigConf::HLTChain* ch : *confChains) {
           if ( boost::regex_match(ch->chain_name().c_str(), what, compiled) ) {
              m_confChains.insert(ch);
              m_names.push_back(ch->chain_name());
           }
        }

        for(TrigConf::TriggerItem* item : *confItems) {
           if ( boost::regex_match( item->name().c_str(), what, compiled) ) {
              m_confItems.insert(item);
              m_names.push_back(item->name());
           }
        }
     }
   
   } else { // Do not parse as regex

     for(const std::string& what : m_patterns) {

        bool found_it = false;

        for(TrigConf::HLTChain* ch : *confChains) {
           if (ch->chain_name() == what) {
              m_confChains.insert(ch);
              m_names.push_back(ch->chain_name());
              found_it = true;
              break;
           }
        }

        if (found_it) {
           continue;
        }

        for(TrigConf::TriggerItem* item : *confItems) {
           if (item->name() == what) {
              m_confItems.insert(item);
              m_names.push_back(item->name());
              found_it = true;
              break;
           }
        }

        if (found_it) {
           continue;
        }

        ATH_MSG_WARNING("Explicitly requested '" << what << "' be added to a ChainGroup"
          << " but this item or chain could not be found in the menu");
     }

   } // parseAsRegex

   m_prescale = calculatePrescale(TrigDefs::Physics);
}

/////////////////////////////////////////////////////////////////////////////
// features

namespace ChainGroup_impl {
   using namespace HLT;
   using namespace Trig;

   bool allActive(const std::vector<TriggerElement*>& tes) {
     for(TriggerElement* te : tes){
         if (te->getActiveState() == false)
            return false;
      }
      return true;
   }


  
   void collectCombinations( const TrigConf::HLTChain* conf, const CacheGlobalMemory& cgm, FeatureContainer& fc, unsigned int condition ){
      // go over the steps of the chain and collecte TEs combinations for each of the chain step (signature)
      bool last_step=true;
      const TrigConf::HLTSignature* previous_sig(0);
      for(const TrigConf::HLTSignature* sig : boost::adaptors::reverse(conf->signatureList())) {
         // chain without signatures
         if (!sig) break;

         // the signatures size changes from step to step, this needs custom treatement and the iteration eeds to be stoped here
         if ( previous_sig && previous_sig->outputTEs().size() != sig->outputTEs().size() )
            break;
         previous_sig = sig;

         std::vector<std::vector<HLT::TriggerElement*> > tes(sig->outputTEs().size()); // preallocate     
         size_t idx = 0;
         for(const TrigConf::HLTTriggerElement* confte : sig->outputTEs() ) {

            // here the condition enters; if we take only accepted objects we need to pick only the active TEs
            cgm.navigation()->getAllOfType(confte->id(), tes[idx], condition & TrigDefs::Physics );
            idx++;
         }
         HLT::ComboIterator combination(tes, cgm.navigation());

         // build the combinations, sometimes a huge list
         while (combination.isValid()) {
            // very very tricky, if all TEs in the combination are active then it means they were already picked up by previous combinations
            // but we can not do this for the last chain step, (we woudl be unable to pick objects whcih made trigger passing)
            //std::cerr << "emitting new combination last_step: "<< last_step << std::endl;	
            if (!allActive(*combination) || last_step) {
               fc.addWithChecking(Combination(*combination, &cgm));
            }
            ++combination;
         }

         if ( condition & TrigDefs::Physics ) // here again athe condition calls for the loop breaking
            break;
         else
            last_step = false; // and go deeper
      }
   }
}// eof namespace

const Trig::FeatureContainer 
Trig::ChainGroup::features(unsigned int condition) const {
   using namespace ChainGroup_impl;
   FeatureContainer f(&cgm_assert());

   // this loop only applies to L2 and EF chain groups
   std::set<const TrigConf::HLTChain*>::const_iterator chIt;
   for (chIt=conf_chain_begin(); chIt != conf_chain_end(); ++chIt) { 
      const HLT::Chain* fchain = cgm_assert().chain(**chIt);
      if (fchain) {
         collectCombinations(fchain->getConfigChain(), cgm_assert(), f, condition);
      }
   }


   // this part only applies to L1 chain groups
   std::vector< std::vector< HLT::TriggerElement*> > tes;
   std::vector< std::vector< HLT::TriggerElement*> >::iterator tesit;
   const_conf_item_iterator it; // iterator over configuration items, *it is TrigConf::TriggerItem*

   for(const TrigConf::TriggerItem* item : m_confItems) {

      std::set< std::string > threshold_names;
      std::stack<const TrigConf::TriggerItemNode*>nodes;

      threshold_names.clear();
      //node = item->topNode();
      nodes.push( item->topNode() );

      // collect unique list (= set) of threshold names for this item
      while (!nodes.empty()) {
         const TrigConf::TriggerItemNode* node = nodes.top();
         nodes.pop();
         if (node == NULL)
            continue;
         if (node->isThreshold()) {
            if (node->triggerThreshold()) {
               // available if thresholds have been read in
               if (!node->triggerThreshold()->name().empty())
                  threshold_names.insert(node->triggerThreshold()->name());
            } else if (!node->thresholdName().empty()) {
               // fall back solution
               threshold_names.insert(node->thresholdName());
            }
         }
         for(TrigConf::TriggerItemNode* childnode : node->children()) {
            nodes.push(childnode);
         }
      }

      // collect corresponding TEs and add them using appendFeatures()
      tes.clear();
      tes.resize(threshold_names.size());   
      tesit = tes.begin();
      std::set< std::string >::iterator setstrit;

      for (setstrit = threshold_names.begin(); setstrit != threshold_names.end(); ++setstrit, ++tesit) {
         cgm_assert().navigation()->getAllOfType(TrigConf::HLTUtils::string2hash(*setstrit), *tesit, true);
      }

      appendFeatures(tes, f);    
   }
    
   ATH_MSG_DEBUG("features: features container size: "<< f.getCombinations().size());
   return f;
}


void Trig::ChainGroup::appendFeatures(std::vector< std::vector< HLT::TriggerElement*> >& tes, Trig::FeatureContainer& fc) const {

  // appends (combinations of) TriggerElements to FeatureContainer
  if (tes.size() == 0) // ComboIterator::isValid would return true in this case
    return;

  HLT::ComboIterator combination(tes, cgm_assert().navigation());
  while (combination.isValid()) {
    fc.addWithChecking(Combination(*combination, &cgm_assert()));

    ATH_MSG_VERBOSE(" adding combination" << Combination(*combination, &cgm_assert()));

    ++combination;
  }
}
