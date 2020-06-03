/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtROD_Decoder.h"

#include "eformat/Issue.h"

#include <algorithm> 

using eformat::helper::SourceIdentifier; 

static const InterfaceID IID_IMdtROD_Decoder
            ("MdtROD_Decoder", 1, 0);

/** constructor 
*/
MdtROD_Decoder::MdtROD_Decoder
( const std::string& type, const std::string& name,const IInterface* parent )
:  AthAlgTool(type,name,parent), 
   m_hid2re(0), m_rodReadOut(0), m_csmReadOut(0), 
   m_amtReadOut(0), m_hptdcReadOut(0), m_BMEpresent(false), m_BMGpresent(false), m_BMEid(-1), m_BMGid(-1)
{
  declareInterface< MdtROD_Decoder  >( this );

  declareProperty("SpecialROBNumber",m_specialROBNumber=-1);
}

const InterfaceID& MdtROD_Decoder::interfaceID( ) { 
  return IID_IMdtROD_Decoder;  
}

StatusCode MdtROD_Decoder::initialize() {
  ATH_CHECK( m_idHelperSvc.retrieve() );

  // Here the mapping service has to be initialized
  m_hid2re=new MDT_Hid2RESrcID();
  ATH_CHECK(m_hid2re->set(&m_idHelperSvc->mdtIdHelper())); 

  // Initialize decoding classes
  m_rodReadOut = new MdtRODReadOut();
  m_csmReadOut = new MdtCsmReadOut();
  m_amtReadOut = new MdtAmtReadOut();
  m_hptdcReadOut = new MdtHptdcReadOut();

  // check if the layout includes elevator chambers
  m_BMEpresent = m_idHelperSvc->mdtIdHelper().stationNameIndex("BME") != -1;
  if(m_BMEpresent){
    ATH_MSG_INFO("Processing configuration for layouts with BME chambers.");
    m_BMEid = m_idHelperSvc->mdtIdHelper().stationNameIndex("BME");
  }
  m_BMGpresent = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG") != -1;
  if(m_BMGpresent){
    ATH_MSG_INFO("Processing configuration for layouts with BMG chambers.");
    m_BMGid = m_idHelperSvc->mdtIdHelper().stationNameIndex("BMG");
  }

  ATH_CHECK( m_readKey.initialize() );
  
  return StatusCode::SUCCESS;
}

StatusCode MdtROD_Decoder::finalize() {

  delete m_rodReadOut;
  delete m_csmReadOut;
  delete m_amtReadOut;
  delete m_hptdcReadOut;
  
  if (m_hid2re) delete m_hid2re;

  if(m_nCache>0 || m_nNotCache>0) {
    const float cacheFraction = ((float)m_nCache) / ((float)(m_nCache + m_nNotCache));
    ATH_MSG_INFO("Fraction of fills that use the cache = " << cacheFraction);
  }

  return StatusCode::SUCCESS ; 
} 


StatusCode MdtROD_Decoder::fillCollections(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
					   MdtCsmContainer& rdoIDC)
{

  //  m_debug = (m_log.level() <= MSG::DEBUG); // use to control if output debug info.

  ATH_MSG_DEBUG("fillCollection: starting");
  uint32_t wordPos   = 0;

  // Check the ROB and ROD fragment for lenght and version consistency. E.P.

  try 
  {
    robFrag.check ();
  }

  catch (eformat::Issue &ex)
  {
    ATH_MSG_DEBUG(ex.what ());
    return StatusCode::FAILURE;  // error in fragment - we search for no collection
  }

  // Check the ROB status word for truncation or corruption E.P.

  uint32_t nstat = robFrag.nstatus ();

  if (nstat)
  {
    const uint32_t *it;
    robFrag.status (it);

    if (*it)
    {
      ATH_MSG_DEBUG ("Error in ROB status word: 0x" 
		      << std::hex << *it << std::dec);

      // the status word analysis in case of eformat version > 3.0 
      //if ( (robFrag.version() >> 16) > 0x0300 ) {
	
	// various checks to be defined

      //}
      return StatusCode::FAILURE;
    }
  }

  // here go the checks on the ROD header



  const unsigned int size = robFrag.rod_ndata(); // Think this is right - it was size() before. Ed.  // Yes it is... Enrico
  
  // FIXME - Removed debug output for the moment. EJWM
//   if (m_debug) log << MSG::DEBUG 
//       << "Collection identifier : " << csmOfflineId.getString() << endmsg;
// 
//   if (m_debug) log << MSG::DEBUG 
//               << "**********Decoder dumping the words******** " << endmsg;
//   if (size > 0) {
//     log << MSG::DEBUG << "The size of this ROD-read is " << size << endmsg;
//     for (unsigned int i=0; i < size; i++)
//       if (m_debug) log << MSG::DEBUG << "word " << i 
//                           << " = " << MSG::hex << robFrag[i] << MSG::dec << endmsg;
//   }
//   else {
//       if (m_debug) log << MSG::DEBUG << " Buffer size 0 ! " 
//                                           << endmsg;
//       return;
//   }


  leading_amt_map leadingHitMap = leading_amt_map();

  // Decode the ROD Header
  //m_rodReadOut->decodeHeader(p);
  //uint16_t subdetId = m_rodReadOut->subdetId();
  //uint16_t mrodId = m_rodReadOut->mrodId();

  // Decode the SourceId from the ROD Header
  SourceIdentifier sid(robFrag.rod_source_id()); 
  
  uint16_t subdetId = sid.subdetector_id();
  uint16_t mrodId   = sid.module_id();
  
  // Get the first word of a buffer:
  OFFLINE_FRAGMENTS_NAMESPACE::PointerType vint;
  robFrag.rod_data( vint );
  
  
  ATH_MSG_DEBUG("**********Decoder dumping the words******** ");

  if (size > 0) {
      ATH_MSG_DEBUG("The size of this ROD-read is ");
      for (unsigned int i=0; i < size; i++)
        ATH_MSG_DEBUG("word " << i
		      << " = " << MSG::hex << vint[i] << MSG::dec);
  } else {
        ATH_MSG_DEBUG(" Buffer size 0 ! ");
        return StatusCode::FAILURE;
  }

  
  m_csmReadOut->decodeWord(vint[wordPos]);

  if (m_csmReadOut->is_BOB()) 
    {
      ATH_MSG_DEBUG ("Found the beginning of buffer ");      
      // Check that Lvl1d matches the one from the ROD header
      ATH_MSG_DEBUG ("Level 1 Id : " << m_csmReadOut->lvl1Id());
    }
  else
    {
      ATH_MSG_ERROR(" Beginning of block not found ! ");
      ATH_MSG_ERROR(" Subdetector, ROD ID: 0x" << MSG::hex << subdetId 
		    << MSG::dec << ", 0x" << mrodId << MSG::dec);
    }

/////////////////////////
// This part was needed only for the data with duplicated ROD-header
//
//  if (m_csmReadOut->lvl1Id() != m_rodReadOut->lvl1Id()) {
//#ifndef NDEBUG
//    m_log << MSG::ERROR << " LVL1 ID from ROD Header and BOB don't match " << endmsg;
//#endif
//  }
/////////////////////////

  while (!m_csmReadOut->is_EOB()) {

    while ((!m_csmReadOut->is_BOL()) && (!m_csmReadOut->is_EOB())) {
      wordPos += 1;
      if (wordPos >= size) {
        ATH_MSG_DEBUG("Error: data corrupted");
        return StatusCode::FAILURE;
      }
      m_csmReadOut->decodeWord(vint[wordPos]);
    }
    
    if (m_csmReadOut->is_BOL())  {
      ATH_MSG_DEBUG("Found the Beginnning of Link ");
    }
    else if (m_csmReadOut->is_EOB()) {
      ATH_MSG_DEBUG ("Error: collection not found ");
      return StatusCode::FAILURE;
    }
    
    //uint16_t subdetId = 0x61;
    //uint16_t mrodId = m_csmReadOut->mrodId();
    
    uint16_t csmId = m_csmReadOut->csmId();
    
    // Get the offline identifier from the MDT cabling service
    // TDC and Tube identifier are dummy, only the chamber map is needed here
    int StationEta = 0;
    int StationPhi = 0;
    int StationName = 0;
    // These arguments are dummy here
    int MultiLayer = 0;
    int TubeLayer  = 0;
    int Tube = 0;
    
    ATH_MSG_DEBUG("subdetId : "  
		  << subdetId << " " 
		  << "mrodId : " << mrodId << " "
		  << "csmId : " << csmId);

    uint16_t tdc = 0xff;
    uint16_t cha = 0xff;
    Identifier moduleId;

    bool cab;

    SG::ReadCondHandle<MuonMDT_CablingMap> readHandle{m_readKey};
    const MuonMDT_CablingMap* readCdo{*readHandle};
    if(readCdo==nullptr){
      ATH_MSG_ERROR("Null pointer to the read conditions object");
      return StatusCode::FAILURE;
    }
    cab = readCdo->getOfflineId(subdetId, mrodId, csmId, tdc, cha,
				  StationName, StationEta, StationPhi,
				  MultiLayer, TubeLayer, Tube);

    ATH_MSG_DEBUG("getOfflineIdfromOnlineID result: ");
    ATH_MSG_DEBUG("Name : " << StationName);
    ATH_MSG_DEBUG("Eta  : " << StationEta);
    ATH_MSG_DEBUG("Phi  : " << StationPhi);	
      
    if (m_BMEpresent) {
      // for layouts with BMEs (read out by 2 CSMs) the RDOs have to be registered with the detectorElement hash
      // registration of common chambers is always done with detectorElement hash of 1st multilayer
      // boundary in BME when 2nd CSM starts is (offline!) tube 43, 1st CMS is registered with ML1 hash, 2nd CSM is ML2 hash
      if (StationName == m_BMEid && Tube > 42)
        moduleId = m_idHelperSvc->mdtIdHelper().channelID(StationName, StationEta, StationPhi, 2, 1, 1);
      else
        moduleId = m_idHelperSvc->mdtIdHelper().channelID(StationName, StationEta, StationPhi, 1, 1, 1);
    } else
      // for layouts with no BME the module hash keeps being used for registration
      moduleId = m_idHelperSvc->mdtIdHelper().elementID(StationName, StationEta, StationPhi);

    if (!cab) {
      ATH_MSG_DEBUG("Cabling not understood");
      ATH_MSG_DEBUG("Skip decoding of CSM link, subdetId : "  
		    << MSG::hex << std::showbase << subdetId << " " 
		    << "mrodId : " << mrodId << " " << MSG::dec
		    << "csmId : " << csmId);
      //assert(false);
      //continue;  // Go to next link instead of crashing (here or later) E.P.
      
      // In case of failure this produce an infinite loop because
      // you do not decode the next word before doing "continue".
      // I commented it out.
      
      // The previous implementation was ok, the bad BOL was simply
      // skipped. This means that you skip decoding this chamber.
      
      // I'm not sure how you can recover this error. I think you 
      // simple have to store the error condition in the
      // StatusCode word returned to the client.
      //    } else if (!isValid) {
      // ATH_MSG_DEBUG("Offline identifier returned by MdtIdHelper is not valid");
      // ATH_MSG_DEBUG("Skip decoding of CSM link, subdetId : "  
      //		    << MSG::hex << std::showbase << subdetId << " " 
      //		    << "mrodId : " << mrodId << " " << MSG::dec
      //		    << "csmId : " << csmId);
      
    } else {
    
    // EJWM Removed: if ( moduleId == csmOfflineId) 
    // Can't see how to keep that in here now, and also not really sure what the point of it is
    // Might be wrong though - job for experts to look into.
    // FIXME

      // get IdentifierHash for this module ID
      auto idHash = getHash(moduleId);

      // Create MdtCsm and try to get it from the cache via the IDC_WriteHandle
      std::unique_ptr<MdtCsm> collection(nullptr);
      MdtCsmContainer::IDC_WriteHandle lock = rdoIDC.getWriteHandle( idHash.first );
      if( lock.alreadyPresent() ) {
          ATH_MSG_DEBUG("collections already found, do not convert");
          ++m_nCache;
      }else{
	ATH_MSG_DEBUG(" Collection ID = " <<idHash.second.getString()
		      << " does not exist, create it ");
	collection = std::make_unique<MdtCsm>(idHash.second, idHash.first);
        ++m_nNotCache;
      }


      // Set values (keep Identifier and IdentifierHash the same though)
      if(collection) collection->set_values(collection->identify(), collection->identifyHash(), subdetId, mrodId, csmId);
      
      wordPos += 1;
      if (wordPos >= size) {
        ATH_MSG_DEBUG("Error: data corrupted");
        return StatusCode::FAILURE;
      }
      m_csmReadOut->decodeWord(vint[wordPos]);
      if (!m_csmReadOut->is_TLP()) {
        ATH_MSG_DEBUG("Error: TDC Link Present not found ");
      }
      
      // Loop on the TDCs blocks, if present 
      wordPos += 1;
      if (wordPos >= size) {
        ATH_MSG_DEBUG("Error: data corrupted");
        return StatusCode::FAILURE;
      }
      StationName == m_BMGid ? m_hptdcReadOut->decodeWord(vint[wordPos])
                             : m_amtReadOut->decodeWord(vint[wordPos]);
      m_csmReadOut->decodeWord(vint[wordPos]);
      while (!m_csmReadOut->is_TWC()) {

        uint16_t tdcNum = StationName == m_BMGid ? m_hptdcReadOut->tdcId()
                                                 : m_amtReadOut->tdcId();
        
        ATH_MSG_DEBUG(" Decoding data from TDC number : " << tdcNum);
                
	// increase word count by one
	wordPos += 1;
	if (wordPos >= size) {
	  ATH_MSG_DEBUG("Error: data corrupted");
	  return StatusCode::FAILURE;
	}
	
	StationName == m_BMGid ? m_hptdcReadOut->decodeWord(vint[wordPos])
	                       : m_amtReadOut->decodeWord(vint[wordPos]);

        MdtAmtHit* amtHit;
        // Loop on the TDC data words and create the corresponding
        // RDO's
        while (!( (StationName == m_BMGid ? m_hptdcReadOut->is_EOT() : m_amtReadOut->is_EOT())
               || (StationName == m_BMGid ? m_hptdcReadOut->is_BOT() : m_amtReadOut->is_BOT())
               || (StationName == m_BMGid ? m_csmReadOut->is_TWC() : m_amtReadOut->is_TWC())
               ) ) {

          StationName == m_BMGid ? m_hptdcReadOut->decodeWord(vint[wordPos])
                                 : m_amtReadOut->decodeWord(vint[wordPos]);
          int tdcCounts;
          uint16_t chanNum;
          // decode the tdc channel number 

          // Check whether this channel has already been 
          // created for this CSM

          if ( (StationName == m_BMGid ? m_hptdcReadOut->is_TSM() : m_amtReadOut->is_TSM())
            && (StationName == m_BMGid ? m_hptdcReadOut->isLeading() : m_amtReadOut->isLeading())
            && collection ) {
            
            chanNum = StationName == m_BMGid ? m_hptdcReadOut->channel()
                                             : m_amtReadOut->channel();

            amtHit = new MdtAmtHit(tdcNum, chanNum);
            amtHit->setValues((StationName == m_BMGid ? m_hptdcReadOut->coarse() : m_amtReadOut->coarse()),
                              (StationName == m_BMGid ? m_hptdcReadOut->fine() : m_amtReadOut->fine()),
                              0);
            amtHit->addData(vint[wordPos]);
            std::pair <leading_amt_map::iterator,bool> ins = 
                leadingHitMap.insert(leading_amt_map::value_type(chanNum,amtHit));
	    if(!ins.second)
	    {
	      ATH_MSG_DEBUG("not including leading " <<  *amtHit);
              ATH_MSG_DEBUG("on the same channel is already decoded the " 
	                    <<  *amtHit);
	      delete amtHit;
	    }
	    //leadingHitMap[chanNum] = amtHit;
          }
          else if ( (StationName == m_BMGid ? m_hptdcReadOut->is_TSM() : m_amtReadOut->is_TSM())
                 && !(StationName == m_BMGid ? m_hptdcReadOut->isLeading() : m_amtReadOut->isLeading())
                 && collection ) {
            
            chanNum = StationName == m_BMGid ? m_hptdcReadOut->channel()
                                             : m_amtReadOut->channel();
            leading_amt_map::iterator chanPosition = leadingHitMap.find(chanNum);

            if (chanPosition != leadingHitMap.end() ) {
              // get the amt hit corresponding to the first data word
              amtHit = leadingHitMap[chanNum];
              uint16_t coarse = amtHit->coarse();
              uint16_t fine   = amtHit->fine();
              int tdcCountsFirst = coarse*32+fine;

              // get the tdc counts of the current data word
              tdcCounts = StationName == m_BMGid ? m_hptdcReadOut->coarse()*32+m_hptdcReadOut->fine()
                                                 : m_amtReadOut->coarse()*32+m_amtReadOut->fine();
              int width = tdcCounts-tdcCountsFirst;

              amtHit->setValues(coarse,fine,width);
              amtHit->addData(vint[wordPos]);
              collection->push_back(amtHit);

              // delete the channel from the map
              leadingHitMap.erase(chanPosition);
            }
            else {
              ATH_MSG_DEBUG("Error: corresponding leading edge not found for the trailing edge tdc: "
			    << tdcNum << " chan: " << chanNum);
            }

          }
          else if ((StationName == m_BMGid ? m_hptdcReadOut->is_TCM() : m_amtReadOut->is_TCM())
                 && collection ) {
            uint16_t chanNum = StationName == m_BMGid ? m_hptdcReadOut->channel()
                                                      : m_amtReadOut->channel();

            amtHit = new MdtAmtHit(tdcNum, chanNum);
            amtHit->setValues((StationName == m_BMGid ? m_hptdcReadOut->coarse() : m_amtReadOut->coarse()),
                              (StationName == m_BMGid ? m_hptdcReadOut->fine() : m_amtReadOut->fine()),
                              (StationName == m_BMGid ? m_hptdcReadOut->width() : m_amtReadOut->width()));
            amtHit->addData(vint[wordPos]);
            collection->push_back(amtHit);
          }

	  // increase word count by one
          wordPos += 1;
          if (wordPos >= size) {
            ATH_MSG_DEBUG("Error: data corrupted");
            return StatusCode::FAILURE;
          }

          StationName == m_BMGid ? m_hptdcReadOut->decodeWord(vint[wordPos])
                                 : m_amtReadOut->decodeWord(vint[wordPos]);

          if ( StationName == m_BMGid ) m_csmReadOut->decodeWord(vint[wordPos]);

        } // End of loop on AMTs

	
	while ( leadingHitMap.begin() != leadingHitMap.end() )
	{
	  
          leading_amt_map::iterator itHit = leadingHitMap.begin();
        //for ( ; itHit != leadingHitMap.end() ; ) {
          if(collection) collection->push_back((*itHit).second); 
          else delete (*itHit).second;
	  leadingHitMap.erase(itHit);
        }
        
	// increase the decoding position only if it's end of TDC 
	// i.e. not operating in TDC trailer suppression mode
	if ( (StationName == m_BMGid ? m_hptdcReadOut->is_EOT() : m_amtReadOut->is_EOT()) ) {
	  wordPos += 1;
	  if (wordPos >= size) {
	    ATH_MSG_ERROR("Error: data corrupted");
	    return StatusCode::FAILURE;
	  }
	}
        m_csmReadOut->decodeWord(vint[wordPos]);
        StationName == m_BMGid ? m_hptdcReadOut->decodeWord(vint[wordPos])
                               : m_amtReadOut->decodeWord(vint[wordPos]);
      }  // End of loop on TDCs
      if(collection) ATH_CHECK(lock.addOrDelete(std::move(collection)));
      // Collection has been found, go out
      // return; 
    }  // Check for the chamber offline id = collection offline id
    wordPos += 1;
    if (wordPos >= size) {
      ATH_MSG_ERROR("Data corrupted");
      return StatusCode::FAILURE;
    }
    m_csmReadOut->decodeWord(vint[wordPos]);
    
  }
  
  //clear the renmant trailing edge from the map
  //leading_amt_map::iterator itHit;
  while ( leadingHitMap.begin() != leadingHitMap.end() )
    {
      leading_amt_map::iterator itHit = leadingHitMap.begin();
      //for ( ; itHit != leadingHitMap.end() ;) {
      delete (*itHit).second; 
      leadingHitMap.erase(itHit);
    }
  
  return StatusCode::SUCCESS; 
}

std::pair<IdentifierHash, Identifier>  MdtROD_Decoder::getHash ( Identifier ident)  {    
    //get hash from identifier.
    IdentifierHash idHash;
    Identifier regid;
    if (m_BMEpresent) {
      regid = m_idHelperSvc->mdtIdHelper().channelID(m_idHelperSvc->mdtIdHelper().stationName(ident),
				       m_idHelperSvc->mdtIdHelper().stationEta(ident),
				       m_idHelperSvc->mdtIdHelper().stationPhi(ident),
				       m_idHelperSvc->mdtIdHelper().multilayer(ident), 1, 1 );
      m_idHelperSvc->mdtIdHelper().get_detectorElement_hash(regid, idHash);
    } else {
      regid = ident;
      m_idHelperSvc->mdtIdHelper().get_module_hash(regid, idHash);
    }
    return std::make_pair(idHash, regid);
}







