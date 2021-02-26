/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArTimeTuningNtuple.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "StoreGate/ReadHandle.h"

LArTimeTuningNtuple::LArTimeTuningNtuple (const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_ntuplePtr(0),
  m_larOnlineHelper(0)
{
  declareProperty("CellTimeOffsetKey",m_CellTimeOffsetKey);
}

LArTimeTuningNtuple::~LArTimeTuningNtuple() 
{}
                       
StatusCode LArTimeTuningNtuple::initialize(){
  ATH_CHECK( detStore()->retrieve(m_larOnlineHelper,"LArOnlineID") );

  const std::string ntuplePath="/NTUPLES/FILE1/TIMETUNING";
  const std::string ntupleTitle="LAr Time Tuning";
  NTuplePtr nt(ntupleSvc(),ntuplePath);
  if (!nt) {
    nt=ntupleSvc()->book(ntuplePath,CLID_ColumnWiseTuple,ntupleTitle);
  }

  if (!nt) {
    ATH_MSG_ERROR ( "Booking of NTuple failed" );
    return StatusCode::FAILURE;
  }
  if (!m_GlobalTimeKey.empty()) {
    ATH_CHECK( nt->addItem("GlobalTimeOffset",m_globalTimeNt,-100.0,100.0) );
  }
  if (!m_TBPhaseKey.empty()) {
    ATH_CHECK( nt->addItem("PhaseTimeOffset",m_phaseNt,-100.0,100.0) );
  }
  if (!m_FebTimeKey.empty()) {
    ATH_CHECK( nt->addItem("FebIndex",m_nFebNt,0,1500) );
    ATH_CHECK( nt->addItem("FebTimeOffset",m_nFebNt,m_febTimeNt) );
    ATH_CHECK( nt->addItem("FebSlot",m_nFebNt,m_febSlotNt) );
    ATH_CHECK( nt->addItem("FebFT",m_nFebNt,m_febFTNt) );
    ATH_CHECK( nt->addItem("FebID",m_nFebNt,m_febIDNt) );
  }
  
  if (!m_CellTimeOffsetKey.empty()) {
    ATH_CHECK( nt->addItem("CellIndex",m_nCellNt,0,1500) );
    ATH_CHECK( nt->addItem("CellTimeOffset",m_nCellNt,m_cellTimeNt) );
    ATH_CHECK( nt->addItem("CellSlot",m_nCellNt,m_cellSlotNt) );
    ATH_CHECK( nt->addItem("CellFT",m_nCellNt,m_cellFTNt) );
    ATH_CHECK( nt->addItem("CellID",m_nCellNt,m_cellIDNt) );
    ATH_CHECK( nt->addItem("CellRegion",m_nCellNt,m_cellRegionNt) );
    ATH_CHECK( nt->addItem("CellLayer",m_nCellNt,m_cellLayerNt) );
    ATH_CHECK( nt->addItem("CellEta",m_nCellNt,m_cellEtaNt) );
    ATH_CHECK( nt->addItem("CellPhi",m_nCellNt,m_cellPhiNt) );
    ATH_CHECK( nt->addItem("CellGain",m_nCellNt,m_cellGainNt) );
    ATH_CHECK( nt->addItem("CellChannel",m_nCellNt,m_cellChannelNt) );
    ATH_CHECK( nt->addItem("CellCalibLine",m_nCellNt,m_cellCalibLineNt) );
  }


  ATH_CHECK( m_GlobalTimeKey.initialize (SG::AllowEmpty) );
  ATH_CHECK( m_TBPhaseKey.initialize (SG::AllowEmpty) );
  ATH_CHECK( m_FebTimeKey.initialize (SG::AllowEmpty) );

  m_ntuplePtr=nt;

  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_CLKey.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode LArTimeTuningNtuple::execute()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  if (!m_GlobalTimeKey.empty()) {
    SG::ReadHandle<LArGlobalTimeOffset> larGlobalTimeOffset
      (m_GlobalTimeKey, ctx);
    m_globalTimeNt = larGlobalTimeOffset->TimeOffset();
    //std::cout << "Global Time Offset= " << m_globalTimeNt << std::endl;
  }
  else {
    m_globalTimeNt = -1;
  }
  
  if (!m_TBPhaseKey.empty()) {
    SG::ReadHandle<TBPhase> tbPhase (m_TBPhaseKey, ctx);
    m_phaseNt = tbPhase->getPhase();
  }
  else {
    m_phaseNt = -999;
  }
    
  if (!m_FebTimeKey.empty()) {
    SG::ReadHandle<LArFEBTimeOffset> larFebTimeOffset (m_FebTimeKey, ctx);
    if (larFebTimeOffset->size()>0) {
      std::vector<HWIdentifier>::const_iterator it=m_larOnlineHelper->feb_begin();
      std::vector<HWIdentifier>::const_iterator it_e=m_larOnlineHelper->feb_end();
      m_nFebNt=0;
      for (;it!=it_e;++it) {
	m_febTimeNt[m_nFebNt] = larFebTimeOffset->TimeOffset(*it, -999);
	m_febSlotNt[m_nFebNt] = m_larOnlineHelper->slot(*it);
	m_febFTNt[m_nFebNt]   = m_larOnlineHelper->feedthrough(*it);
	m_febIDNt[m_nFebNt]   = (*it).get_identifier32().get_compact();
	++m_nFebNt;
      }
    }
    else 
      ATH_MSG_WARNING ( "Could not retrieve LArFebTimeOffset with key '" << m_FebTimeKey << "'" );
  }

  ATH_CHECK( ntupleSvc()->writeRecord(m_ntuplePtr) );
  return StatusCode::SUCCESS;
}


StatusCode LArTimeTuningNtuple::stop(){

  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  const LArEM_ID* emId = idHelper->em_idHelper();
  if (!emId) {
    ATH_MSG_ERROR ( "Could not get lar EM ID helper" );
    return StatusCode::FAILURE;
  }
  
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling){
       ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
       return StatusCode::FAILURE;
  } 
  SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_CLKey};
  const LArCalibLineMapping *clCont {*clHdl};
  if(!clCont) {
     ATH_MSG_ERROR( "Do not have calib line mapping !!!" );
     return StatusCode::FAILURE;
  }
  if (m_CellTimeOffsetKey.length()>0) {
    
    LArCellTimeOffset *cellTimeOffset=nullptr;
    StatusCode sc=evtStore()->retrieve(cellTimeOffset,m_CellTimeOffsetKey);
    if (sc.isSuccess() && cellTimeOffset->size()>0) {
    
      cellTimeOffset->setDefaultReturnValue(-999);
      
      std::map <HWIdentifier,float>::const_iterator it=cellTimeOffset->begin();
      std::map <HWIdentifier,float>::const_iterator it_e=cellTimeOffset->end();
      m_nCellNt=0;
      for (;it!=it_e;++it) {
        
	Identifier id = cabling->cnvToIdentifier(it->first);     

	m_cellTimeNt[m_nCellNt]      = cellTimeOffset->TimeOffset(it->first);
	m_cellSlotNt[m_nCellNt]      = m_larOnlineHelper->slot(it->first);
	m_cellFTNt[m_nCellNt]        = m_larOnlineHelper->feedthrough(it->first);
	m_cellIDNt[m_nCellNt]        = (it->first).get_identifier32().get_compact();
	m_cellRegionNt[m_nCellNt]    = emId->region(id) ;
	m_cellLayerNt[m_nCellNt]     = emId->sampling(id) ;
	m_cellEtaNt[m_nCellNt]       = emId->eta(id) ;
	m_cellPhiNt[m_nCellNt]       = emId->phi(id) ;
	m_cellGainNt[m_nCellNt]      = -999 ; /*** FIX ME ***/
	m_cellChannelNt[m_nCellNt]   = m_larOnlineHelper->channel(it->first);
	const std::vector<HWIdentifier>& calibLineV = clCont->calibSlotLine(it->first);
        std::vector<HWIdentifier>::const_iterator calibLineIt = calibLineV.begin();   
	m_cellCalibLineNt[m_nCellNt] = m_larOnlineHelper->channel(*calibLineIt) ;
	
	std::cout << " *** Ch = " << std::hex << it->first.get_compact() << std::dec
                  << " Time = " << cellTimeOffset->TimeOffset(it->first) << std::endl ;
	
	++m_nCellNt;
      }
    }
    else
      ATH_MSG_WARNING ( "Could not retrieve LArCellTimeOffset with key '" << m_CellTimeOffsetKey << "'" );
  
    ATH_CHECK( ntupleSvc()->writeRecord(m_ntuplePtr) );
  }

  return StatusCode::SUCCESS;
}
