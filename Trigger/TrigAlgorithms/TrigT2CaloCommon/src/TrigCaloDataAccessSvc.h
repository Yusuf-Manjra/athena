/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT2CaloCommon_TrigCaloDataAccessSvc_h
#define TrigT2CaloCommon_TrigCaloDataAccessSvc_h

#include <mutex>
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "LArByteStream/LArRodDecoder.h"
#include "TileByteStream/TileROD_Decoder.h"
#include "LArRawUtils/LArTT_Selector.h"
#include "TileByteStream/TileCellCont.h"
#include "TrigT2CaloCommon/LArCellCont.h"
#include "LArRecEvent/LArFebEnergyCollection.h"
#include "TileEvent/TileCellCollection.h"
#include "TileEvent/TileL2Container.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "TrigT2CaloCommon/ITrigCaloDataAccessSvc.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "CaloEvent/CaloBCIDAverage.h"

class TrigCaloDataAccessSvc : public extends<AthService, ITrigCaloDataAccessSvc> {
 public:
  TrigCaloDataAccessSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrigCaloDataAccessSvc();
  
  using ITrigCaloDataAccessSvc::Status;


  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  virtual StatusCode loadCollections ( const EventContext& context,
                                       const IRoiDescriptor& roi,
                                       const DETID detID,
                                       const int sampling,
                                       LArTT_Selector<LArCellCont>& loadedCells ) override;
  
  virtual StatusCode loadCollections ( const EventContext& context,
                                       const IRoiDescriptor& roi,
                                       TileCellCollection& loadedCells ) override;
  
  virtual StatusCode loadMBTS ( const EventContext& context,
                                                    TileCellCollection& loadedCells ) override;


  
  virtual StatusCode loadFullCollections ( const EventContext& context,
                                           CaloConstCellContainer& cont ) override;
  
 private:
  
  PublicToolHandle<LArRodDecoder> m_larDecoder { this, "LArDecoderTool", "LArRodDecoder/LArRodDecoder", "Tool to decode LAr raw data" };
  PublicToolHandle<TileROD_Decoder> m_tileDecoder { this, "TileDecoderTool", "TileROD_Decoder/TileROD_Decoder", "Tool to decode Tile raw data" };

  //!< LArRoI_Map used by LArTT_Selector, TB this is apparently a tool!
  PublicToolHandle<LArRoI_Map> m_roiMapTool { this, "LArRoIMapTool", "LArRoI_Map/LArRoI_Map", "Tool used by selectors" };           

  ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Tool to monitor performance of the service" };

  ServiceHandle<IROBDataProviderSvc>  m_robDataProvider{ this, "ROBDataProvider", "ROBDataProviderSvc/ROBDataProviderSvc", ""};
  ServiceHandle<IRegSelSvc>         m_regionSelector{ this, "RegionSelector", "RegSelSvc/RegSelSvc", ""};
  
  Gaudi::Property<bool> m_applyOffsetCorrection { this, "ApplyOffsetCorrection", true, "Enable offset correction" };

  SG::ReadHandleKey<CaloBCIDAverage> m_bcidAvgKey ;

  void reset_LArCol ( LArCellCollection* coll ){
    for(LArCellCollection::iterator ii=coll->begin();ii!=coll->end();++ii)
      (*ii)->setEnergyFast(0.0);
  }

  /**
   * @brief Convenience structure to keep together all ROBs and IdentifierHashes 
   * for whole detectors.
   **/
  struct FullDetIDs {
    std::vector<uint32_t> robs;
    std::vector<IdentifierHash> ids;
    DETID detid;
    void merge( const std::initializer_list<FullDetIDs>& list ) {
      for ( auto& el: list ) {
	std::copy( el.robs.begin(), el.robs.end(), std::back_inserter(robs) );
	std::copy( el.ids.begin(), el.ids.end(), std::back_inserter(ids) );
      }
    }

  };
  
  /**
   * @brief convience structure to keep together a collection and auxiliar full collection selectors
   */
  struct  HLTCaloEventCache {
    std::mutex mutex;    
    LArCellCont* larContainer;
    TileCellCont* tileContainer;
    CaloCellContainer* fullcont;
    unsigned int lastFSEvent;
  };

  
  SG::SlotSpecificObj< HLTCaloEventCache > m_hLTCaloSlot;

  std::mutex m_initMutex; // this will be gone once we move to new conditions
  std::mutex m_dataPrepMutex; // this will be gone when reg sel & Rob DP will become thread safe
  std::mutex m_getCollMutex; // this will be gone

  unsigned int lateInit();
  bool m_lateInitDone = false;

  unsigned int convertROBs(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& robFrags, LArCellCont* larcell );
  unsigned int convertROBs( const std::vector<IdentifierHash>& rIds, TileCellCont* tilecell );


  /**
   * @brief fill the set of missing robs given the request and response from RoBDatProvider
   **/
  void missingROBs( const std::vector<uint32_t>& request,
		    const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& response,
		    std::set<uint32_t>& missing ) const;

  /**
   * @brief clear fragments of the collection for which ROBs were not available
   **/
  void clearMissing( const std::vector<uint32_t>& request,
		     const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& response, 
		     LArCellCont* larcell );

  /**
   * @brief LAr TT collections preparation code
   **/
  unsigned int prepareLArCollections( const EventContext& context,
				const IRoiDescriptor& roi, 
				const int sampling,
				DETID detector );

  unsigned int prepareTileCollections( const EventContext& context,
				const IRoiDescriptor& roi, 
				DETID detector );

  unsigned int prepareMBTSCollections( const EventContext& context );

  unsigned int prepareFullCollections( const EventContext& context );

  unsigned int prepareLArFullCollections( const EventContext& context );
  unsigned int prepareTileFullCollections( const EventContext& context );

  std::vector<uint32_t> m_vrodid32fullDet;
  std::vector<uint32_t> m_vrodid32tile;
  std::vector<unsigned int> m_mbts_add_rods;
  const std::vector<unsigned int>* m_mbts_rods;
  std::vector<IdentifierHash> m_rIdstile;
  std::vector<std::vector<uint32_t> > m_vrodid32fullDetHG;
  size_t m_nSlots;
};


#endif


