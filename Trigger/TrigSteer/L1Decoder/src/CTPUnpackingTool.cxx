/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <boost/algorithm/string.hpp>
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigT1Result/RoIBResult.h"
#include "AthenaMonitoring/Monitored.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "CTPUnpackingTool.h"

using namespace HLT;


CTPUnpackingTool::CTPUnpackingTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : CTPUnpackingToolBase(type, name, parent) { }


StatusCode CTPUnpackingTool::initialize()
{
  ATH_CHECK( m_lvl1ConfigSvc.retrieve() );
  ATH_CHECK( m_hltConfigSvc.retrieve() );
  ATH_CHECK( m_HLTMenuKey.initialize() );

  ATH_CHECK( CTPUnpackingToolBase::initialize() );

  return StatusCode::SUCCESS;
}




StatusCode CTPUnpackingTool::start() {
  // TODO switch to the L1 menu once available
  ATH_MSG_INFO( "Updating CTP bits decoding configuration");
  // iterate over all items and obtain the CPT ID for each item. Then, package that in the map: name -> CTP ID
  std::map<std::string, size_t> toCTPID;
  for ( const TrigConf::TriggerItem* item:   m_lvl1ConfigSvc->ctpConfig()->menu().itemVector() ) {
    toCTPID[item->name()] = item->ctpId();
  }

  auto addIfItemExists = [&]( const std::string& itemName, HLT::Identifier id ) -> StatusCode {
    if ( toCTPID.find( itemName ) != toCTPID.end() ) {
      m_ctpToChain[ toCTPID[itemName] ].push_back( id );
      return StatusCode::SUCCESS;
    }
    ATH_MSG_ERROR(itemName << " used to seed the chain " << id <<" not in the configuration ");
    return StatusCode::FAILURE;
  };

  SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
  ATH_CHECK( hltMenuHandle.isValid() );
  for ( const TrigConf::Chain& chain: *hltMenuHandle ) {
    HLT::Identifier chainID{ chain.name() };
    if ( chain.l1item().empty() ) { // unseeded chain
      m_ctpToChain[ s_CTPIDForUndeededChains ].push_back( chainID );
    } else if ( chain.l1item().find(',') != std::string::npos ) { // OR seeds

      std::vector<std::string> items;
      boost::split(items, chain.l1item(), [](char c){return c == ',';});
      for ( const std::string& i: items ) {
	ATH_CHECK( addIfItemExists( i, chainID ) );
      }
    } else { // regular chain
      ATH_CHECK( addIfItemExists( chain.l1item(), chainID ) );
    }
  }

  for ( auto ctpIDtoChain: m_ctpToChain ) {
    for ( auto chain: ctpIDtoChain.second ) {
      ATH_MSG_DEBUG( "CTP seed of " << ctpIDtoChain.first << " enables chains " << chain );
    }
  }

  return StatusCode::SUCCESS;
}



StatusCode CTPUnpackingTool::decode( const ROIB::RoIBResult& roib,  HLT::IDVec& enabledChains ) const {
  auto nTAVItems = Monitored::Scalar( "TAVItems", 0 );
  auto nChains   = Monitored::Scalar( "Chains", 0 );
  auto monitorit = Monitored::Group( m_monTool, nTAVItems, nChains );
  auto tav = roib.cTPResult().TAV();
  const size_t tavSize = tav.size();

  for ( size_t wordCounter = 0; wordCounter < tavSize; ++wordCounter ) {
    for ( size_t bitCounter = 0;  bitCounter < 32; ++bitCounter ) {
      const size_t ctpIndex = 32*wordCounter + bitCounter;
      const bool decision = ( tav[wordCounter].roIWord() & ((uint32_t)1 << bitCounter) ) > 0;

      if ( decision == true or m_forceEnable ) {
	if ( decision ) {
	  nTAVItems = nTAVItems + 1;
	  ATH_MSG_DEBUG( "L1 item " << ctpIndex << " active, enabling chains "
			 << (m_forceEnable ? " due to the forceEnable flag" : " due to the seed"));
	}

	auto itr = m_ctpToChain.find( ctpIndex );
	if ( itr != m_ctpToChain.end() ) {
	  enabledChains.insert( enabledChains.end(), itr->second.begin(), itr->second.end() );
	}
      }
    }
  }
  // the unseded chains are alwasys enabled at this stage
  auto itr  = m_ctpToChain.find( s_CTPIDForUndeededChains );
  if ( itr != m_ctpToChain.end() )
    enabledChains.insert( enabledChains.end(), itr->second.begin(), itr->second.end());

  nChains = enabledChains.size();
  for ( auto chain: enabledChains ) {
    ATH_MSG_DEBUG( "Enabling chain: " << chain );
  }
  if ( nTAVItems == 0 ) {
    ATH_MSG_ERROR( "All CTP bits were disabled, this event shoudl not have shown here" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
