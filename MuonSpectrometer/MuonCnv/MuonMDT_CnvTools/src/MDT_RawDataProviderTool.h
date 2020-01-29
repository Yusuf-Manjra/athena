/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDTCNVTOOLS_MUONMDTRAWDATAPROVIDERTOOL_H
#define MUONMDTCNVTOOLS_MUONMDTRAWDATAPROVIDERTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MDT_RawDataProviderToolCore.h"
#include <vector>
#include <string>

#include "MuonRDO/MdtCsm_Cache.h"

namespace Muon 
{

/** @class MDT_RawDataProviderTool       
    @author  Edward Moyse <Edward.Moyse@cern.ch>
    @author  Mark Owen <markowen@cern.ch>
*/  

class MDT_RawDataProviderTool : virtual public IMuonRawDataProviderTool, public MDT_RawDataProviderToolCore
{
  public:    
  MDT_RawDataProviderTool(const std::string&,const std::string&,const IInterface*);

  /** default destructor */
  virtual ~MDT_RawDataProviderTool ();
    
  /** standard Athena-Algorithm method */
  virtual StatusCode initialize();
    
  /** standard Athena-Algorithm method */
  virtual StatusCode finalize  ();
  
  /** Convert method - declared in Muon::IMuonRdoToPrepDataTool*/
  virtual StatusCode convert( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs);
  virtual StatusCode convert( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs, const std::vector<IdentifierHash>&);
  /** the new ones */
  virtual StatusCode convert(); //!< for the entire event 
  virtual StatusCode convert(const std::vector<IdentifierHash>& HashVec);
  virtual StatusCode convert(const std::vector<uint32_t>& robIds);//!< for a particular vector of ROBId's


};
}

#endif
