/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigT1Result/RoIBResult.h"
#include "CTPUnpackingTool.h"

using namespace HLT;


CTPUnpackingTool::CTPUnpackingTool( const std::string& type,
				    const std::string& name, 
				    const IInterface* parent ) 
  : AthAlgTool(type, name, parent) {
  declareProperty("CTPToChainMapping", m_ctpToChainProperty, "Mapping of the form: '34:HLT_x', '35:HLT_y', ..., both CTP ID and chain may appear many times");
  declareProperty("ForceEnableAllChains", m_forceEnable=false, "Enables all chains in each event, testing mode");
}


CTPUnpackingTool::~CTPUnpackingTool()
{}

StatusCode CTPUnpackingTool::decodeCTPToChainMapping() {
  std::istringstream input;
  for ( auto entry: m_ctpToChainProperty ) {
    input.clear();
    input.str(entry);
    size_t ctpId;
    input >> ctpId;
    char delim;
    input >> delim;    
    if ( delim != ':' ) {
      ATH_MSG_ERROR( "Error in conf. entry: " << entry << " missing ':'" );
      return StatusCode::FAILURE;
    }
    std::string chainName;
    input >> chainName;
    ATH_MSG_DEBUG( "Chain " << chainName << " seeded from CTP item of ID " << ctpId );
    m_ctpToChain[ctpId].push_back( HLT::Identifier(chainName) );
  }
  return StatusCode::SUCCESS;
}


StatusCode CTPUnpackingTool::decode( const ROIB::RoIBResult& roib,  HLT::IDVec& enabledChains ) const {
  size_t numberPfActivatedBits= 0;
  
  auto tav = roib.cTPResult().TAV();
  const size_t tavSize = tav.size();

  for ( size_t wordCounter = 0; wordCounter < tavSize; ++wordCounter ) {
    for ( size_t bitCounter = 0;  bitCounter < 32; ++bitCounter ) {
      const size_t ctpIndex = 32*wordCounter + bitCounter;
      const bool decision = ( tav[wordCounter].roIWord() & (1 << bitCounter) ) > 0;

      if ( decision == true or m_forceEnable ) {
	if ( decision ) 
	  ATH_MSG_DEBUG( "L1 item " << ctpIndex << " active, enabling chains");
	numberPfActivatedBits++;
	auto itr = m_ctpToChain.find(ctpIndex);
	if ( itr != m_ctpToChain.end() ) 
	  enabledChains.insert( enabledChains.end(), itr->second.begin(), itr->second.end() );
      }
    }    
  }
  for ( auto chain: enabledChains ) {
    ATH_MSG_DEBUG( "Enabling chain: " << chain );
  }
  if ( numberPfActivatedBits == 0 ) {
    ATH_MSG_ERROR( "All CTP bits were disabled, this event shoudl not have shown here" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


