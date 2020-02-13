/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MmRdoToPrepDataTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONMmRdoToPrepDataTool_H
#define MUONMmRdoToPrepDataTool_H

#include "MmRdoToPrepDataToolCore.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonRDO/MM_RawDataContainer.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"

#include <string>
#include <vector>
class AtlasDetectorID;
class Identifier;
class MuonIdHelper;
class MM_RawDataCollection;

namespace MuonGM
{    
    class MMReadoutElement;
}


namespace Muon 
{

  class IMuonRawDataProviderTool;
  class IMMClusterBuilderTool;

  class MmRdoToPrepDataTool : virtual public MmRdoToPrepDataToolCore
  {
  public:
    MmRdoToPrepDataTool(const std::string&,const std::string&,const IInterface*);
    
    /** default destructor */
    virtual ~MmRdoToPrepDataTool ();
    
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize() override;
  
  protected:
    virtual SetupMM_PrepDataContainerStatus setupMM_PrepDataContainer() override;
  }; 
} // end of namespace

#endif 
