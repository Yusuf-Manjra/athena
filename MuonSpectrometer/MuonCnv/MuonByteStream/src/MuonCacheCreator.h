/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
 
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonRDO/MdtCsm_Cache.h"
#include "MuonRDO/CscRawDataCollection_Cache.h"
#include "MuonRDO/RpcPad_Cache.h"
#include "MuonRDO/TgcRdo_Cache.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <atomic>

class MuonCacheCreator : public AthReentrantAlgorithm {
 public:

  /// Constructor
  MuonCacheCreator(const std::string &name,ISvcLocator *pSvcLocator);
  /// Destructor
  virtual ~MuonCacheCreator()=default;

  /// Initialize the algorithm
  virtual StatusCode initialize () override;

  /// Execture the algorithm
  virtual StatusCode execute (const EventContext& ctx) const override;

protected:

  template<typename T>
  StatusCode createContainer(const SG::WriteHandleKey<T>& , long unsigned int , const EventContext& ) const;
  
  /// Write handle key for the MDT CSM cache container
  SG::WriteHandleKey<MdtCsm_Cache> m_MdtCsmCacheKey;
  SG::WriteHandleKey<CscRawDataCollection_Cache> m_CscCacheKey;
  SG::WriteHandleKey<RpcPad_Cache> m_RpcCacheKey;
  SG::WriteHandleKey<TgcRdo_Cache> m_TgcCacheKey;
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  BooleanProperty m_disableWarning{this,"DisableViewWarning",false};
  mutable std::atomic_bool m_disableWarningCheck;
  bool isInsideView(const EventContext&) const;

};//class MuonCacheCreator

// copied from http://acode-browser1.usatlas.bnl.gov/lxr/source/athena/InnerDetector/InDetRecAlgs/InDetPrepRawDataFormation/src/CacheCreator.h#0062
// maybe should figure out if this code can be shared
template<typename T>
StatusCode MuonCacheCreator::createContainer(const SG::WriteHandleKey<T>& containerKey, long unsigned int size, const EventContext& ctx) const{
  if(containerKey.key().empty()){
    ATH_MSG_DEBUG( "Creation of container "<< containerKey.key() << " is disabled (no name specified)");
    return StatusCode::SUCCESS;
  }
  SG::WriteHandle<T> ContainerCacheKey(containerKey, ctx);
  ATH_CHECK( ContainerCacheKey.recordNonConst ( std::make_unique<T>(IdentifierHash(size), nullptr) ));
  ATH_MSG_DEBUG( "Container "<< containerKey.key() << " created to hold " << size );
  return StatusCode::SUCCESS;
}
