/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTRawContByteStreamTool.h"

#include "ByteStreamCnvSvcBase/FullEventAssembler.h" 
#include "ByteStreamCnvSvcBase/SrcIdMap.h" 

#include "GaudiKernel/MsgStream.h"

#include "InDetIdentifier/TRT_ID.h"


#include "TRT_RodEncoder.h"



// ------------------------------------------------------------------------
// default contructor 

TRTRawContByteStreamTool::TRTRawContByteStreamTool
( const std::string& type, const std::string& name,const IInterface* parent )
  :  base_class(type,name,parent),
     m_trt_CablingSvc ("TRT_CablingSvc", name ),
     m_trt_idHelper(nullptr)
{
  declareInterface< ITRTRawContByteStreamTool  >( this );
  declareProperty("RodBlockVersion",m_RodBlockVersion=3);

  return;
}

// ------------------------------------------------------------------------
// destructor 
 
TRTRawContByteStreamTool::~TRTRawContByteStreamTool()
{
   return;
}
 
// ------------------------------------------------------------------------
// initialize the tool
 
StatusCode
TRTRawContByteStreamTool::initialize()
{
   ATH_CHECK( AlgTool::initialize() );

   ATH_CHECK( m_trt_CablingSvc.retrieve() );
   ATH_MSG_INFO( "Retrieved tool " << m_trt_CablingSvc );

   ATH_CHECK( detStore()->retrieve(m_trt_idHelper, "TRT_ID") );

   ATH_CHECK( m_byteStreamCnvSvc.retrieve() );

  return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------
// finalize the tool
 
StatusCode
TRTRawContByteStreamTool::finalize()
{
   ATH_CHECK( AlgTool::finalize() );
   return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------
// New convert method which makes use of the encoder class (as done for
// other detectors)

StatusCode
TRTRawContByteStreamTool::convert(TRT_RDO_Container* cont) const
{
   StatusCode sc(StatusCode::SUCCESS);

  FullEventAssembler<SrcIdMap>* fea = nullptr;
  ATH_CHECK( m_byteStreamCnvSvc->getFullEventAssembler (fea,
                                                        "TRTRawCont") );
  FullEventAssembler<SrcIdMap>::RODDATA*  theROD ; 

  // set ROD Minor version
  fea->setRodMinorVersion(m_RodBlockVersion);
  ATH_MSG_DEBUG( " Setting Minor Version Number to "<<m_RodBlockVersion );
   
  // a map for ROD ID onto Encoder
  std::map<uint32_t, TRT_RodEncoder> mapEncoder; 
   
  ATH_MSG_DEBUG( " number of collections "<< cont->size() );

  //loop over the collections in the TRT RDO container
  TRT_RDO_Container::const_iterator it_coll     = cont->begin(); 
  TRT_RDO_Container::const_iterator it_coll_end = cont->end();

  for( ; it_coll!=it_coll_end;++it_coll)
  {
    const TRTRawCollection* coll = (*it_coll) ;

    if ( 0 != coll )
    {
      // Collection Id
      Identifier id = coll->identify() ;

      // Layer Id associated to the collection Id (Mapping)
      Identifier            IdLayer = m_trt_idHelper->layer_id(id); 

      std::vector<uint32_t> robids  = m_trt_CablingSvc->getRobID(IdLayer);

      eformat::helper::SourceIdentifier sid_rob(robids[0]);

      // Building the rod ID
      eformat::helper::SourceIdentifier sid_rod( sid_rob.subdetector_id(),
						 sid_rob.module_id() );
      uint32_t rodId = sid_rod.code();  

      // loop over RDOs in the collection; 
      TRTRawCollection::const_iterator it_b = coll->begin(); 
      TRTRawCollection::const_iterator it_e = coll->end(); 
     
      for(; it_b!=it_e; ++it_b)           // loop over RDOs in the collection;
      {
	const RDO* theRdo = *it_b;          // fill rdo vector VRDO in 
	mapEncoder[rodId].addRdo(theRdo);   // RodEncoder with the collection
      }
    }
     
  }  // End loop over collections
   
  // loop over Encoder map and fill all ROD Data Blocks
   std::map<uint32_t,TRT_RodEncoder>::iterator it_map     = mapEncoder.begin(); 
   std::map<uint32_t,TRT_RodEncoder>::iterator it_map_end = mapEncoder.end();

   for (; (it_map != it_map_end) && (sc == StatusCode::SUCCESS); ++it_map)
   { 

    TRT_RodEncoder theEncoder = (*it_map).second; 

    // set the idhelper, mapping and minor version
    theEncoder.set_trt_IdHelper  (m_trt_idHelper);
    theEncoder.set_trt_cabling (m_trt_CablingSvc);
    theEncoder.setRodMinorVersion(m_RodBlockVersion);
    // use encoder to get ROD fragment data
    theROD = fea->getRodData((*it_map).first); // get ROD data address


    if ( 1 == m_RodBlockVersion )
       sc = theEncoder.fillROD( *theROD ) ;  // fill ROD data
    else if ( 3 == m_RodBlockVersion )
       sc = theEncoder.fillROD3( *theROD ) ;  // fill ROD data
    else
    {
      ATH_MSG_WARNING( "Unsupported TRT Rod Block Version Number" );
      sc = StatusCode::RECOVERABLE;
    }
   } 

   if ( sc == StatusCode::FAILURE )
      ATH_MSG_ERROR( "TRT ROD Encoder Failed" );
   else if ( sc == StatusCode::RECOVERABLE )
      ATH_MSG_WARNING( "TRT ROD Encoder has RECOVERABLE error" );


   return sc;
}


