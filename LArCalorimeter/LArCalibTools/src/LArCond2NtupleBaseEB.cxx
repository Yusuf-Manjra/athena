/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArCond2NtupleBaseEB.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

LArCond2NtupleBaseEB::LArCond2NtupleBaseEB(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_initialized(false)
  , m_nt(nullptr)
  , m_detStore(nullptr)
  , m_emId(nullptr)
  , m_hecId(nullptr)
  , m_fcalId(nullptr)
  , m_onlineId(nullptr)
  , m_caloId(nullptr)
  , m_FEBTempTool("LArFEBTempTool")
{
}

StatusCode LArCond2NtupleBaseEB::initialize() {

  ATH_MSG_DEBUG("Initializing LArCond2Ntuple base class");

  if (m_ntpath.size()==0 || m_ntTitle.size()==0) {
    ATH_MSG_ERROR( "Need to set variable 'm_ntpath' and 'm_ntTitle' in constructor of deriving class!" );
    return StatusCode::FAILURE;
  }

  size_t i=m_ntpath.rfind('/');
  if (i==std::string::npos) {
    ATH_MSG_ERROR( "Expected at least on '/' in path " << m_ntpath );
    return StatusCode::FAILURE;
  }
  std::string basepath(m_ntpath.begin(),m_ntpath.begin()+i);

  NTupleFilePtr file1(ntupleSvc(),basepath);
  if (!file1){
    ATH_MSG_ERROR( "Could not get NTupleFilePtr with path " << basepath << " failed" );
    return StatusCode::FAILURE;
  }
  NTuplePtr nt(ntupleSvc(),m_ntpath);
  if (!nt) {
    nt=ntupleSvc()->book(m_ntpath,CLID_ColumnWiseTuple,m_ntTitle);
  }
  if (!nt){
    ATH_MSG_ERROR( "Booking of NTuple at "<< m_ntpath << " and name " << m_ntTitle << " failed" );
    return StatusCode::FAILURE;
  }

  m_nt=nt;

  m_detStore= &(*detStore()); //for backward compatiblity

  const CaloIdManager* calo_id_manager;
  ATH_CHECK(detStore()->retrieve(calo_id_manager,"CaloIdManager"));

  StatusCode sc;
  ATH_CHECK(m_caloSuperCellMgrKey.initialize(m_realgeom && m_isSC));
  ATH_CHECK(m_caloMgrKey.initialize(m_realgeom && !m_isSC));
  if ( m_isSC ){
    const LArOnline_SuperCellID* ll;
    sc = detStore()->retrieve(ll, "LArOnline_SuperCellID");
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
      return StatusCode::FAILURE;
    } else {
      m_onlineId = ll;
      ATH_MSG_DEBUG("Found the LArOnlineID helper");
    }
    m_caloId = calo_id_manager->getCaloCell_SuperCell_ID();
  } else { // m_isSC
    const LArOnlineID* ll;
    sc = detStore()->retrieve(ll, "LArOnlineID");
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Could not get LArOnlineID helper !" << endmsg;
      return StatusCode::FAILURE;
    } else {
      m_onlineId = ll;
      ATH_MSG_DEBUG(" Found the LArOnlineID helper. ");
    }
    m_caloId = calo_id_manager->getCaloCell_ID();
  } // end of m_isSC if

  m_emId=m_caloId->em_idHelper();
  m_fcalId=m_caloId->fcal_idHelper();
  m_hecId=m_caloId->hec_idHelper();

  if (!m_emId) {
    ATH_MSG_ERROR( "Could not access lar EM ID helper" );
    return StatusCode::FAILURE;
  }
  if (!m_fcalId) {
    ATH_MSG_ERROR( "Could not access lar FCAL ID helper" );
    return StatusCode::FAILURE;
  }
  if (!m_hecId) {
    ATH_MSG_ERROR( "Could not access lar HEC ID helper" );
    return StatusCode::FAILURE;
  }


  if (m_addFEBTemp) {
    sc = m_FEBTempTool.retrieve();
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( " Can't get FEBTempTool." );
      return sc;
    }
  }

  ATH_CHECK( m_BCKey.initialize() );
  /// do we need both of them at some point when data has both SC and cells?
  ATH_CHECK( m_cablingSCKey.initialize(m_isSC) );
  ATH_CHECK( m_calibMapSCKey.initialize(m_addCalib && m_isSC) );
  ATH_CHECK( m_cablingKey.initialize(!m_isSC) );
  ATH_CHECK( m_calibMapKey.initialize(m_addCalib && !m_isSC) );

  //Online-identifier variables
  sc=nt->addItem("channelId",m_SC,m_onlChanId,0x38000000,0x3A000000);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'channelId' failed" );
    return StatusCode::FAILURE;
  }

  if(m_expandId) {

    sc=nt->addItem("barrel_ec",m_SC,m_barrel_ec,0,1);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'barrel_ec' failed" );
      return StatusCode::FAILURE;
    }

    sc=nt->addItem("pos_neg",m_SC,m_pos_neg,0,1);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'pos_neg' failed" );
      return StatusCode::FAILURE;
     }

    sc=nt->addItem("FT",m_SC,m_FT,0,32);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'FT' failed" );
      return StatusCode::FAILURE;
    }

    sc=nt->addItem("slot",m_SC,m_slot,1,15);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'slot' failed" );
      return StatusCode::FAILURE;
     }

    sc=nt->addItem("channel",m_SC,m_channel,0,127);
    if (sc!=StatusCode::SUCCESS){
      ATH_MSG_ERROR( "addItem 'channel' failed" );
      return StatusCode::FAILURE;
    }
  }//m_expandId

  if(m_addCalib) {
    sc=nt->addItem("calibLine",m_SC,m_calibLine,0,127);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'calibLine' failed" );
      return StatusCode::FAILURE;
    }


    sc=nt->addItem("isConnected",m_SC,m_isConnected,0,1);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'isConnected' failed" );
      return StatusCode::FAILURE;
    }
  }//m_addCalib


  if (m_addHash) {
    sc=nt->addItem("channelHash",m_SC,m_chanHash,0,200000);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'channelHash' failed" );
      return StatusCode::FAILURE;
    }

    sc=nt->addItem("febHash",m_SC,m_febHash,0,2000);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'febHash' failed" );
      return StatusCode::FAILURE;
    }

    if (m_OffId) {
      sc=m_nt->addItem("oflHash",m_SC,m_oflHash,0,200000);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'oflHash' failed" );
	return StatusCode::FAILURE;
      }
    }
  }//m_addHash


  //Offline-identifier variables
  if ( m_OffId ) {
    sc=nt->addItem("offlineId",m_SC,m_oflChanId,0x20000000,0x40000000);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'channelId' failed" );
      return StatusCode::FAILURE;
    }

    sc=nt->addItem("layer",m_SC,m_layer,0,4);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'layer' failed" );
      return StatusCode::FAILURE;
     }

    if(m_realgeom) {
      sc=nt->addItem("eta",m_SC,m_reta);
      if (sc!=StatusCode::SUCCESS) {
       ATH_MSG_ERROR( "addItem 'eta' failed" );
       return StatusCode::FAILURE;
      }
      sc=nt->addItem("phi",m_SC,m_rphi);
      if (sc!=StatusCode::SUCCESS) {
        ATH_MSG_ERROR( "addItem 'phi' failed" );
        return StatusCode::FAILURE;
      }
    }
    sc=nt->addItem("ieta",m_SC,m_eta,0,510);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'ieta' failed" );
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("iphi",m_SC,m_phi,0,1023);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'iphi' failed" );
      return StatusCode::FAILURE;
    }

    sc=nt->addItem("region",m_SC,m_region,0,5);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'region' failed" );
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("detector",m_SC,m_detector,0,2);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'detector' failed" );
      return StatusCode::FAILURE;

    }
  }// m_OffId

  if (m_addBC) {
    sc=nt->addItem("badChan",m_SC,m_badChanWord);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'badChan' failed" );
      return StatusCode::FAILURE;
    }
  }

  if (m_addFEBTemp) {
    sc=nt->addItem("FEBTemp1",m_SC,m_FEBTemp1);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'FEBTemp1' failed" );
      return StatusCode::FAILURE;
    }
    sc=nt->addItem("FEBTemp2",m_SC,m_FEBTemp2);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'FEBTemp2' failed" );
      return StatusCode::FAILURE;
    }
  }

  m_initialized=true;
  return StatusCode::SUCCESS;
}

bool LArCond2NtupleBaseEB::fillFromIdentifier(const HWIdentifier& hwid, int C) {

 ATH_MSG_DEBUG("Starting fillFromIdentifier");
 const LArBadChannelCont *bcCont = nullptr;
 if ( m_addBC ) {
   SG::ReadCondHandle<LArBadChannelCont> readHandle{m_BCKey};
   bcCont =*readHandle;
   if( !bcCont) {
       ATH_MSG_WARNING( "Do not have Bad chan container " << m_BCKey.key() );
       return false;
   }
 }

 const LArCalibLineMapping *clCont=0;
 if(m_addCalib) {
   if(m_isSC){
     SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_calibMapSCKey};
     clCont={*clHdl};
   }
   else{
     SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_calibMapKey};
     clCont={*clHdl};
   }
   if(!clCont) {
       ATH_MSG_WARNING( "Do not have calib line mapping !!!" );
       return false;
   }
 }

 const LArOnOffIdMapping* cabling=0;
 if(m_isSC){
   SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingSCKey};
   cabling=*cablingHdl;
 }
 else{
   SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
   cabling=*cablingHdl;
 }

 if(!cabling) {
     ATH_MSG_WARNING( "Do not have cabling !" );
     return false;
 }

 m_onlChanId[C] = hwid.get_identifier32().get_compact();

 if(m_expandId) {
    m_barrel_ec[C] = m_onlineId->barrel_ec(hwid);
    m_pos_neg[C]   = m_onlineId->pos_neg(hwid);
    m_FT[C]        = m_onlineId->feedthrough(hwid);
    m_slot[C]      = m_onlineId->slot(hwid);
    m_channel[C]   = m_onlineId->channel(hwid);
 }

 if (m_addHash) {
   m_chanHash[C]=m_onlineId->channel_Hash(hwid);
   m_febHash[C]=m_onlineId->feb_Hash(m_onlineId->feb_Id(hwid));
 }

 if(m_addCalib) {
   m_calibLine[C]=NOT_VALID;
   const std::vector<HWIdentifier>& calibLineV=clCont->calibSlotLine(hwid);
   if(calibLineV.size()) m_calibLine[C] = m_onlineId->channel(calibLineV[0]);
 }


 if ( m_OffId ) {
   m_detector[C]=NOT_VALID;
   m_region[C]=NOT_VALID;
   m_eta[C]=NOT_VALID;
   m_phi[C]=NOT_VALID;

   if(m_realgeom){
      m_reta[C]=NOT_VALID;
      m_rphi[C]=NOT_VALID;
   }
  m_layer[C]=NOT_VALID;
  m_oflChanId[C]=NOT_VALID;
  if (m_addHash) m_oflHash[C]=NOT_VALID;
 }

 if (m_addBC) m_badChanWord[C]=0;
 bool connected=false;

 const CaloDetDescrManager_Base* dd_man = nullptr;
 if (m_realgeom) {
   if(m_isSC) {
     SG::ReadCondHandle<CaloSuperCellDetDescrManager> caloSuperCellMgrHandle{m_caloSuperCellMgrKey};
     dd_man = *caloSuperCellMgrHandle;
   }
   else {
     SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
     dd_man = *caloMgrHandle;
   }
 }

 try {
   if (cabling->isOnlineConnected(hwid)) {
     Identifier id=cabling->cnvToIdentifier(hwid);
     if ( m_OffId ) {
       m_oflChanId[C] = id.get_identifier32().get_compact();
       if (m_addHash) m_oflHash[C]=m_caloId->calo_cell_hash(id);

       if (dd_man) {
          const CaloDetDescrElement *elem = dd_man->get_element(id);
          if(!elem) {
             ATH_MSG_WARNING("Do not have CDDE for "<<id.getString());
          } else {
            m_reta[C] = elem->eta_raw();
            m_rphi[C] = elem->phi_raw();
          }
       }

       if (m_emId->is_lar_em(id)) {
         m_eta[C]       = m_emId->eta(id);
         m_phi[C]       = m_emId->phi(id);
         m_layer[C]     = m_emId->sampling(id);
         m_region[C]    = m_emId->region(id);
         m_detector[C]  = std::abs(m_emId->barrel_ec(id)) - 1; //0-barrel, 1-EMEC-OW, 2-EMEC-IW
       }
       else if (m_hecId->is_lar_hec(id)) {
         m_eta[C]       = m_hecId->eta(id);
         m_phi[C]       = m_hecId->phi(id);
         m_layer[C]     = m_hecId->sampling(id);
         m_region[C]    = m_hecId->region(id);
         m_detector[C]  = 3;
       }
       else if (m_fcalId->is_lar_fcal(id)) {
         m_eta[C]       = m_fcalId->eta(id);
         m_phi[C]       = m_fcalId->phi(id);
         m_layer[C]     = m_fcalId->module(id);
         m_region[C]    = 0;
         m_detector[C]  = 4;
       }
     } // m_OffId
     connected=true;
   }//end if is connected
 }catch (LArID_Exception & except) {}

 //bad-channel word
 if (m_addBC) m_badChanWord[C]=bcCont->status(hwid).packedData();
 // FEB temperatures
 if (m_addFEBTemp) {
     FEBTemp tv = m_FEBTempTool->getFebTemp(hwid);
     if( !tv.empty() ) {
       FEBTemp::const_iterator itb = tv.begin();
       FEBTemp::const_iterator ite = tv.end();
       for(;itb!=ite;++itb) {
          m_FEBTemp1[C] = (*itb).first;
          m_FEBTemp2[C] = (*itb).second;
       }
     }
 }

 if(m_addCalib) m_isConnected[C] = (long)connected;

 return connected;
}


const SG::ReadCondHandleKey<LArOnOffIdMapping>&
LArCond2NtupleBaseEB::cablingKey() const
{
  if(m_isSC) return m_cablingSCKey;
  return m_cablingKey;
}
