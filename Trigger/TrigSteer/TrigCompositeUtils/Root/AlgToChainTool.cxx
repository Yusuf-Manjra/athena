/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/AlgToChainTool.h"

#ifndef XAOD_STANDALONE

TrigCompositeUtils::AlgToChainTool::AlgToChainTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent)
    : AthAlgTool (type, name, parent) 
    {}


TrigCompositeUtils::AlgToChainTool::~AlgToChainTool() {}


StatusCode TrigCompositeUtils::AlgToChainTool::initialize() {
    ATH_CHECK( m_HLTMenuKey.initialize() );

    return StatusCode::SUCCESS;
}


StatusCode TrigCompositeUtils::AlgToChainTool::start() {
    SG::ReadHandle<TrigConf::HLTMenu>  hltMenuHandle = SG::makeHandle( m_HLTMenuKey );
    ATH_CHECK( hltMenuHandle.isValid() );

    // Fill the maps
    for ( const TrigConf::Chain& chain : *hltMenuHandle ) {
        for ( const std::string& sequencer : chain.sequencers() ) {
            m_sequencerToChainMap[sequencer].push_back(chain);
        }
    }

    for ( const auto& sequencer : hltMenuHandle->sequencers() ) {
        for ( const std::string& algorithm : sequencer.second ) {
            // Save just second part of algorithm ex. RoRSeqFilter/FFastCaloElectron -> FFastCaloElectron
            m_algToSequencersMap[algorithm.substr(algorithm.find('/') + 1)]
                .push_back(sequencer.first);
        }
    }

    return StatusCode::SUCCESS;
}


std::set<std::string> TrigCompositeUtils::AlgToChainTool::getChainsNamesForAlg(const std::string& algorithmName) const {
    std::set<std::string> result;

    std::vector<TrigConf::Chain> chainsSet = getChainsForAlg(algorithmName);
    for (const TrigConf::Chain& chain : chainsSet) {
        result.insert(chain.name());
    }

    return result;
}


std::vector<TrigConf::Chain> TrigCompositeUtils::AlgToChainTool::getChainsForAlg(const std::string& algorithmName) const {
    std::vector<TrigConf::Chain> result;

    try {
        for ( const std::string& sequencer : m_algToSequencersMap.at(algorithmName) ) {
            result.insert(
                result.end(),
                m_sequencerToChainMap.at(sequencer).begin(), 
                m_sequencerToChainMap.at(sequencer).end()
            );
        }
    } catch ( const std::out_of_range & ex ) {
        ATH_MSG_DEBUG ( algorithmName << " is not part of the menu!" );
    }

    return result;
}


std::set<std::string> TrigCompositeUtils::AlgToChainTool::getActiveChainsForAlg(const std::string& algorithmName, const EventContext& context) const {
    std::set<std::string> result;

    std::set<std::string> allActiveChains = retrieveActiveChains(context);
    std::set<std::string> allAlgChains = getChainsNamesForAlg(algorithmName);

    // Save the chains that are used by selected algorithm and active
    std::set_intersection(allAlgChains.begin(), allAlgChains.end(),
        allActiveChains.begin(), allActiveChains.end(),
        std::inserter(result, result.begin()));

    return result;
}

std::set<std::string> TrigCompositeUtils::AlgToChainTool::retrieveActiveChains(const EventContext& context) const {
    std::set<TrigCompositeUtils::DecisionID> activeChainsID;

    // Retrieve EventStore and keys
    IProxyDict* storeProxy = Atlas::getExtendedEventContext(context).proxy();
    SmartIF<SGImplSvc> eventStore (storeProxy);

    std::vector<std::string> keys;
    eventStore->keys(static_cast<CLID>( ClassID_traits<TrigCompositeUtils::DecisionContainer>::ID() ), keys);

    // Retrieve active chains
    std::set<std::string> activeChains;

    for ( const std::string& key : keys ) {
        if( key.find("HLTNav") != 0 || key == "HLTNav_Summary" ) {
            continue;
        }

        SG::DataProxy* dp = eventStore->proxy(
            static_cast<CLID>(ClassID_traits<TrigCompositeUtils::DecisionContainer>::ID()), key, true);

        SG::ReadHandle<TrigCompositeUtils::DecisionContainer> dc (dp);
        if ( !dc.isValid() ) {            
            ATH_MSG_WARNING("Failed to retrieve " << key << " from event store.");            
            continue;        
        }

        for ( const TrigCompositeUtils::Decision* d : *dc ) {
            TrigCompositeUtils::DecisionIDContainer chainsID;
            TrigCompositeUtils::decisionIDs( d, chainsID );

            // Save the active chains IDs
            activeChainsID.insert( chainsID.begin(), chainsID.end() );
        } 
    }

    // Convert DecisionID to names
    std::set<std::string> activeChainsNames;

    for ( const TrigCompositeUtils::DecisionID& id : activeChainsID ) {
        activeChainsNames.insert( HLT::Identifier(id).name() );
    }

    return activeChainsNames;
}

std::map<std::string, std::vector<TrigConf::Chain>> TrigCompositeUtils::AlgToChainTool::getChainsForAllAlgs() const{
    std::map<std::string, std::vector<TrigConf::Chain>> algToChain;

    for (const auto& algSeqPair : m_algToSequencersMap){
        algToChain[algSeqPair.first] = getChainsForAlg(algSeqPair.first);
    }
    return algToChain;
}

#endif // XAOD_STANDALONE