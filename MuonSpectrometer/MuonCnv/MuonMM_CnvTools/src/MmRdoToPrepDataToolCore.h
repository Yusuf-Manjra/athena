/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMmRdoToPrepDataToolCore_H
#define MUONMmRdoToPrepDataToolCore_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonRDO/MM_RawDataContainer.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MMClusterization/IMMClusterBuilderTool.h"
#include "NSWCalibTools/INSWCalibTool.h"

#include <string>
#include <vector>

class MM_RawDataCollection;

namespace MuonGM
{    
    class MuonDetectorManager;
}

namespace Muon 
{

  class MmRdoToPrepDataToolCore : virtual public IMuonRdoToPrepDataTool, virtual public AthAlgTool
  {
  public:
    MmRdoToPrepDataToolCore(const std::string&,const std::string&,const IInterface*);
    
    /** default destructor */
    virtual ~MmRdoToPrepDataToolCore()=default;
    
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    
    /** Decode method - declared in Muon::IMuonRdoToPrepDataTool*/
    StatusCode decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect );
    //new decode methods for Rob based readout
    StatusCode decode( const std::vector<uint32_t>& robIds, const std::vector<IdentifierHash>& chamberHashInRobs );
    StatusCode decode( const std::vector<uint32_t>& robIds );
    
    StatusCode processCollection(const MM_RawDataCollection *rdoColl, 
   				 std::vector<IdentifierHash>& idWithDataVect);

    void printInputRdo();
    void printPrepData();
    
  protected:
    
    enum SetupMM_PrepDataContainerStatus {
      FAILED = 0, ADDED, ALREADYCONTAINED
    };

    virtual SetupMM_PrepDataContainerStatus setupMM_PrepDataContainer();

    const MM_RawDataContainer* getRdoContainer();

    void processRDOContainer( std::vector<IdentifierHash>& idWithDataVect );

    /// Muon Detector Descriptor
    const MuonGM::MuonDetectorManager * m_muonMgr;
    
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    
    bool m_fullEventDone;
    
    /// MdtPrepRawData containers
    Muon::MMPrepDataContainer * m_mmPrepDataContainer;
    SG::WriteHandleKey<Muon::MMPrepDataContainer> m_mmPrepDataContainerKey;
    SG::ReadHandleKey< MM_RawDataContainer >         m_rdoContainerKey;

    std::string m_outputCollectionLocation;            
    bool m_merge; 

    ToolHandle<IMMClusterBuilderTool> m_clusterBuilderTool;
    ToolHandle<INSWCalibTool> m_calibTool;

  }; 
} // end of namespace

#endif 
