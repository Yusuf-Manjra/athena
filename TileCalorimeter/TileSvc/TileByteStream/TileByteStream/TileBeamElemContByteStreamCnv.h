/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBYTESTREAM_TILEBEAMELEM_BYTESTREAMCNV_H
#define TILEBYTESTREAM_TILEBEAMELEM_BYTESTREAMCNV_H

// Gaudi includes
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ContextSpecificPtr.h"


// Athena includes
#include "AthenaBaseComps/AthMessaging.h"
#include "TileEvent/TileMutableBeamElemContainer.h"
#include "AthenaKernel/RecyclableDataObject.h"


#include "eformat/ROBFragment.h"
#include "eformat/FullEventFragment.h"

class DataObject;
class StatusCode;
class IAddressCreator;
class IROBDataProviderSvc;
class TileHid2RESrcID;
class TileROD_Decoder;

#include <stdint.h>
#include <vector> 


// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/**
 * @class TileBeamElemContByteStreamCnv
 * @brief This Converter class provides conversion from ByteStream to TileBeamElemContainer
 * @author Alexander Solodkov
 */

class TileBeamElemContByteStreamCnv
  : public Converter
  , public ::AthMessaging
{
  public:
    TileBeamElemContByteStreamCnv(ISvcLocator* svcloc);

    virtual StatusCode initialize() override;
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;
    virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;
    virtual StatusCode finalize() override;

    /// Storage type and class ID
    virtual long repSvcType() const override { return i_repSvcType(); }
    static long storageType();
    static const CLID& classID();

    // FIXME: Eliminate this backdoor access to RAW data.
    //        If clients need raw data, they should get it from the
    //        existing ByteStreamSvc interfaces.
    inline const eformat::FullEventFragment<const uint32_t*> * eventFragment() const { return m_event.get(); }
    inline const eformat::ROBFragment<const uint32_t*> * robFragment() const { return m_robFrag.get(); }
    inline bool  validBeamFrag() const { return m_robFrag.isValid(); }
  
  private: 
    
    std::string m_name;
    
    /** Pointer to IROBDataProviderSvc */
    ServiceHandle<IROBDataProviderSvc> m_robSvc;

    /** Pointer to TileROD_Decoder */
    ToolHandle<TileROD_Decoder> m_decoder;

    Gaudi::Hive::ContextSpecificPtr<
      const eformat::FullEventFragment<const uint32_t*> > m_event;

    Gaudi::Hive::ContextSpecificPtr<
      const eformat::ROBFragment<const uint32_t*> > m_robFrag;

    std::vector<uint32_t> m_ROBID;

    /** Pointer to TileHid2RESrcID */
    const TileHid2RESrcID* m_hid2re;

    /** Queue of data objects to recycle. */
    Athena::RecyclableDataQueue<TileMutableBeamElemContainer> m_queue;
};
#endif

