/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonDetectorManager_H
#define MuonDetectorManager_H

#include "GeoPrimitives/GeoPrimitives.h"
/// Ensure that the Athena extensions are properly loaded
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/StatusCode.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "MuonAlignmentData/CorrContainer.h"
#include "MuonAlignmentData/NswAsBuiltDbData.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonReadoutGeometry/GenericCSCCache.h"
#include "MuonReadoutGeometry/GenericMDTCache.h"
#include "MuonReadoutGeometry/GenericRPCCache.h"
#include "MuonReadoutGeometry/GenericTGCCache.h"

#ifndef SIMULATIONBASE
#include "MuonNSWAsBuilt/StripCalculator.h"
#include "MuonNSWAsBuilt/StgcStripCalculator.h"
#endif

typedef ALineMapContainer::const_iterator ciALineMap;
typedef BLineMapContainer::const_iterator ciBLineMap;
typedef CscInternalAlignmentMapContainer::const_iterator ciCscInternalAlignmentMap;
typedef MdtAsBuiltMapContainer::const_iterator ciMdtAsBuiltMap;

namespace MuonGM {

    constexpr bool optimRE = true;

    class CscReadoutElement;
    class TgcReadoutElement;
    class RpcReadoutElement;
    class MdtReadoutElement;
    class sTgcReadoutElement;
    class MMReadoutElement;
    class MuonClusterReadoutElement;
    class MuonStation;
    class TgcReadoutParams;

    /**
       The MuonDetectorManager stores the transient representation of the Muon Spectrometer geometry
       and provides access to all main blocks of information needed by generic clients of the
       geometry:
       XxxReadoutElements, MuonStations, MuonIdHelpers, geometryVersion, etc.
       More details below.
    */

    class MuonDetectorManager : public GeoVDetectorManager, public AthMessaging {
    public:
        // Constructor:
        MuonDetectorManager();

        // Destructor:
        ~MuonDetectorManager();

        // Gets the number of tree tops:  required:
        virtual unsigned int getNumTreeTops() const;

        // Gets the ith tree top:  required:
        virtual PVConstLink getTreeTop(unsigned int i) const;

        // Add a tree top:
        void addTreeTop(PVLink);

        // Add a XxxReadoutElement to the Collection
        void addMdtReadoutElement(MdtReadoutElement*);    //!< store the MdtReadoutElement using as "key" the identifier
        void addRpcReadoutElement(RpcReadoutElement*);    //!< store the RpcReadoutElement using as "key" the identifier
        void addTgcReadoutElement(TgcReadoutElement*);    //!< store the TgcReadoutElement using as "key" the identifier
        void addCscReadoutElement(CscReadoutElement*);    //!< store the CscReadoutElement using as "key" the identifier
        void addsTgcReadoutElement(sTgcReadoutElement*);  //!< store the sTGCReadoutElement using as "key" the identifier
        void addMMReadoutElement(MMReadoutElement*);      //!< store the MMReadoutElement using as "key" the identifier
       
        // storeTgcReadoutParams
        void storeTgcReadoutParams(std::unique_ptr<const TgcReadoutParams> x);

        // storeCscInternalAlignmentParams
        void storeCscInternalAlignmentParams(const CscInternalAlignmentPar& x);

        void storeMdtAsBuiltParams(const MdtAsBuiltPar& x);

        // access to Readout Elements
        const MdtReadoutElement* getMdtReadoutElement(const Identifier&) const;    //!< access via extended identifier (requires unpacking)
        const RpcReadoutElement* getRpcReadoutElement(const Identifier&) const;    //!< access via extended identifier (requires unpacking)
        const TgcReadoutElement* getTgcReadoutElement(const Identifier&) const;    //!< access via extended identifier (requires unpacking)
        const CscReadoutElement* getCscReadoutElement(const Identifier&) const;    //!< access via extended identifier (requires unpacking)
        const MMReadoutElement* getMMReadoutElement(const Identifier&) const;      //!< access via extended identifier (requires unpacking)
        const sTgcReadoutElement* getsTgcReadoutElement(const Identifier&) const;  //!< access via extended identifier (requires unpacking)

        //// Non const version
        MdtReadoutElement* getMdtReadoutElement(const Identifier& id);
        CscReadoutElement* getCscReadoutElement(const Identifier& id);
        TgcReadoutElement* getTgcReadoutElement(const Identifier& id);
      

        const MdtReadoutElement* getMdtReadoutElement(const IdentifierHash& id) const;  //!< access via detector-element hash id
        const RpcReadoutElement* getRpcReadoutElement(const IdentifierHash& id) const;  //!< access via detector-element hash id
        const TgcReadoutElement* getTgcReadoutElement(const IdentifierHash& id) const;  //!< access via detector-element hash id
        const CscReadoutElement* getCscReadoutElement(const IdentifierHash& id) const;  //!< access via detector-element hash id

        inline unsigned int nMuonStation() const;  //!< Number of MuonStations

        inline unsigned int nMdtRE() const;   //!< Number of Mdt ReadoutElements
        inline unsigned int nsTgcRE() const;  //!< Number of sTgc ReadoutElements
        inline unsigned int nMMRE() const;    //!< Number of MM ReadoutElements
        inline unsigned int nCscRE() const;   //!< Number of Csc ReadoutElements
        inline unsigned int nRpcRE() const;   //!< Number of Rpc ReadoutElements
        inline unsigned int nTgcRE() const;   //!< Number of Tgc ReadoutElements

        inline unsigned int nMdtDE() const;  //!< Number of Mdt DetectorElements
        inline unsigned int nCscDE() const;  //!< Number of Csc DetectorElements
        inline unsigned int nRpcDE() const;  //!< Number of Rpc DetectorElements
        inline unsigned int nTgcDE() const;  //!< Number of Tgc DetectorElements

        // Geometry versioning
        inline std::string geometryVersion() const;  //!< it can be Rome-Initial or P03, or ... it's the name of the layout
        void setGeometryVersion(const std::string& version);
        inline std::string get_DBMuonVersion() const;  //!< the name of the MuonSpectrometer tag (in the geometry DB) actually accessed
        void set_DBMuonVersion(const std::string& version);

        // Access to identifier helpers
        inline const MdtIdHelper* mdtIdHelper() const;
        inline const CscIdHelper* cscIdHelper() const;
        inline const RpcIdHelper* rpcIdHelper() const;
        inline const TgcIdHelper* tgcIdHelper() const;
        inline const sTgcIdHelper* stgcIdHelper() const;
        inline const MmIdHelper* mmIdHelper() const;

        // Generic Technology descriptors
        void setGenericMdtDescriptor(const GenericMDTCache& mc);
        inline const GenericMDTCache* getGenericMdtDescriptor() const;
        void setGenericRpcDescriptor(const GenericRPCCache& rc);
        inline const GenericRPCCache* getGenericRpcDescriptor() const;
        void setGenericCscDescriptor(const GenericCSCCache& cc);
        inline const GenericCSCCache* getGenericCscDescriptor() const;
        void setGenericTgcDescriptor(const GenericTGCCache& tc);
        inline const GenericTGCCache* getGenericTgcDescriptor() const;

        void setCachingFlag(int value);
        inline int cachingFlag() const;
        void setCacheFillingFlag(int value);
        inline int cacheFillingFlag() const;

        void setNSWABLineAsciiPath(const std::string &);
        void setNSWAsBuiltAsciiPath(const std::string &, const std::string &);

        void setMinimalGeoFlag(int flag);
        inline int  MinimalGeoFlag() const;
        void setCutoutsFlag(int flag);
        inline int  IncludeCutoutsFlag() const;
        void setCutoutsBogFlag(int flag);
        inline int  IncludeCutoutsBogFlag() const;
        void setMdtDeformationFlag(int flag) { m_applyMdtDeformations = flag; }
        inline int  mdtDeformationFlag() const { return m_applyMdtDeformations; }
        void setMdtAsBuiltParamsFlag(int flag) { m_applyMdtAsBuiltParams = flag; }
        inline int  mdtAsBuiltParamsFlag() const { return m_applyMdtAsBuiltParams; }
        void setNswAsBuiltParamsFlag(int flag) { m_applyNswAsBuiltParams = flag; }
        inline int  nswAsBuiltParamsFlag() const { return m_applyNswAsBuiltParams; }
        void setControlAlinesFlag(int flag) { m_controlAlines = flag; }
        inline int  controlAlinesFlag() const { return m_controlAlines; }
        void setApplyCscIntAlignment(bool x) { m_useCscIntAlign = x; }
        inline bool applyMdtDeformations() const { return (bool)m_applyMdtDeformations; }
        inline bool applyMdtAsBuiltParams() const { return (bool)m_applyMdtAsBuiltParams; }
        inline bool applyNswAsBuiltParams() const { return (bool)m_applyNswAsBuiltParams; }
        inline bool applyCscIntAlignment() const { return m_useCscIntAlign; }
        void setCscIlinesFlag(int flag) { m_controlCscIlines = flag; }
        inline int  CscIlinesFlag() const { return m_controlCscIlines; }
        void setCscFromGM(bool x) { m_useCscIlinesFromGM = x; }
        inline bool CscFromGM() const { return m_useCscIlinesFromGM; }
        
        enum readoutElementHashMax {
            MdtRElMaxHash = 2500,
            CscRElMaxHash = 130,
            RpcRElMaxHash = 2600,
            TgcRElMaxHash = 1600,
        };
        enum detElementHashMax {
            MdtDetElMaxHash = 1200,
            CscDetElMaxHash = 65,
            RpcDetElMaxHash = 1300,
            TgcDetElMaxHash = 1600,
        };
        enum MdtGMRanges {
            NMdtStatType = 26,
            NMdtStatEta = 17,
            NMdtStEtaOffset = 8,
            NMdtStatPhi = 8,
            NMdtMultilayer = 2,
        };
        enum RpcGMRanges {
            NRpcStatType = 12,  // there are 12 station types where RPCs can be installed: BML/BMS/BOL/BOS/BMF/BOF/BOG/BME/BIR/BIM/BIL/BIS
            NRpcStatEta = 17,
            NRpcStEtaOffset = 8,
            NRpcStatPhi = 8,
            NDoubletR = 2,
            NDoubletZ = 4
        };  // using some trick to save space: dbz=4 if rib's chambers and doubletphi=2;
        enum TgcGMRanges {
            NTgcStatType = 8,
            NTgcStatTypeOff = -41,
            NTgcStatEta = 10,
            NTgcStEtaOffset = 5,
            NTgcStatPhi = 48,
        };
        enum CscGMRanges {
            NCscStatType = 2,
            NCscStatTypeOff = -50,
            NCscStatEta = 2,
            NCscStEtaOffset = 1,
            NCscStatPhi = 8,
            NCscChamberLayer = 2
        };
        enum sTgcGMRanges {
            NsTgStatEta = 6,      /// 3 x 2 sides (-3,-2,-1 and 1,2,3)
            NsTgStEtaOffset = 3,  /// needed offest to map (-3,-2,-1,1,2,3) to (0,1,2,3,4,5)
            NsTgStatPhi = 16,     // large and small sector together
            NsTgChamberLayer = 2
        };
        enum mmGMRanges {
            NMMcStatEta = 4,      /// 2 x 2 sides (-2,-1 and 1,2)
            NMMcStEtaOffset = 2,  /// needed offest to map (-2,-1,1,2) to (0,1,2,3)
            NMMcStatPhi = 16,     // large and small sector together
            NMMcChamberLayer = 2
        };

        // Add a MuonStation to the list
        void addMuonStation(MuonStation* mst);
        const MuonStation* getMuonStation(const std::string& stName, int eta, int phi) const;
        MuonStation* getMuonStation(const std::string& stName, int eta, int phi);
        //<! access to the MuonStation by StationName, Jzz, Jff (amdb indices!!!! not stationPhi and Eta)
        std::string muonStationKey(const std::string& stName, int statEtaIndex, int statPhiIndex) const;

        void clearCache();
        void refreshCache();
        void fillCache();

        void clearMdtCache();
        void clearRpcCache();
        void clearCscCache();
        void clearTgcCache();
        void clearsTgcCache();
        void clearMMCache();

        void refreshMdtCache();
        void refreshRpcCache();
        void refreshCscCache();
        void refreshTgcCache();
        void refreshsTgcCache();
        void refreshMMCache();

        void fillMdtCache();
        void fillRpcCache();
        void fillCscCache();
        void fillTgcCache();
        void fillsTgcCache();
        void fillMMCache();

        inline const ALineMapContainer* ALineContainer() const;
        inline const BLineMapContainer* BLineContainer() const;
        inline const CscInternalAlignmentMapContainer* CscInternalAlignmentContainer() const;
        inline const MdtAsBuiltMapContainer* MdtAsBuiltContainer() const;
        inline ciALineMap ALineMapBegin() const;
        inline ciBLineMap BLineMapBegin() const;
        inline ciALineMap ALineMapEnd() const;
        inline ciBLineMap BLineMapEnd() const;
        inline ciCscInternalAlignmentMap CscALineMapBegin() const;
        inline ciCscInternalAlignmentMap CscALineMapEnd() const;
        inline ciMdtAsBuiltMap MdtAsBuiltMapBegin() const;
        inline ciMdtAsBuiltMap MdtAsBuiltMapEnd() const;
        StatusCode updateAlignment(const ALineMapContainer& a, bool isData = true);
        StatusCode updateDeformations(const BLineMapContainer& a, bool isData = true);
        StatusCode updateMdtAsBuiltParams(const MdtAsBuiltMapContainer& a);
        StatusCode initCSCInternalAlignmentMap();
        StatusCode updateCSCInternalAlignmentMap(const CscInternalAlignmentMapContainer& cscIntAline);

        void initABlineContainers();
        void setMMAsBuiltCalculator(const NswAsBuiltDbData* nswAsBuiltData);
        void setStgcAsBuiltCalculator(const NswAsBuiltDbData* nswAsBuiltData);
#ifndef SIMULATIONBASE
        const NswAsBuilt::StripCalculator* getMMAsBuiltCalculator() const { return m_MMAsBuiltCalculator.get(); }
        const NswAsBuilt::StgcStripCalculator* getStgcAsBuiltCalculator() const { return m_StgcAsBuiltCalculator.get(); }
#endif

        // get Mdt AsBuilt parameters for chamber specified by Identifier
        const MdtAsBuiltPar* getMdtAsBuiltParams(const Identifier& id) const;

        // map the RPC station indices (0-NRpcStatType) back to the RpcIdHelper stationNames
        int rpcStationName(const int stationIndex) const;
      
    private:
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{"Muon::MuonIdHelperSvc/MuonIdHelperSvc", "MuonDetectorManager"};
        void loadStationIndices();
        unsigned int rpcStationTypeIdx(const int stationName) const;  // map the RPC stationNames from the RpcIdHelper to 0-NRpcStatType
        enum RpcStatType { BML = 0, BMS, BOL, BOS, BMF, BOF, BOG, BME, BIR, BIM, BIL, BIS, UNKNOWN };
        /// Helper method to convert the Identifier into the corresponding index accessing the array
        int rpcIdentToArrayIdx(const Identifier& id) const;        
        int tgcIdentToArrayIdx(const Identifier& id) const;
        int cscIdentToArrayIdx(const Identifier& id) const;
        int stgcIdentToArrayIdx(const Identifier& id) const;
        int mmIdenToArrayIdx(const Identifier& id) const;
        int mdtIdentToArrayIdx(const Identifier& id) const;

        /// The doublet z index is required during the initialization of the
        /// detector element
        int rpcIdentToArrayIdx(const Identifier& id, int& dbz_index) const;

        int m_cachingFlag{1};
        int m_cacheFillingFlag{1};
        int m_minimalgeo{0};
        int m_includeCutouts{0};
        int m_includeCutoutsBog{0};
        int m_controlAlines{111111};
        int m_applyMdtDeformations{0};
        int m_applyMdtAsBuiltParams{0};
        int m_applyNswAsBuiltParams{0};
        bool m_useCscIntAlign{false};
        int m_controlCscIlines{111111};
        bool m_useCscIlinesFromGM{true};

        std::vector<PVLink> m_envelope;  // Tree-top...

        GenericCSCCache m_genericCSC;
        GenericMDTCache m_genericMDT;
        GenericRPCCache m_genericRPC;
        GenericTGCCache m_genericTGC;

        // Geometry versioning
        std::string m_geometryVersion{};  // generic name of the Layout
        std::string m_DBMuonVersion{};    // name of the MuonVersion table-collection in Oracle

        std::string m_NSWABLineAsciiPath{};
        bool m_NSWAsBuiltAsciiOverrideMM{false};
        bool m_NSWAsBuiltAsciiOverrideSTgc{false};
       
        // 115.6 kBytes.
        static constexpr int s_NumMaxRpcElements = NRpcStatType * NRpcStatEta * NRpcStatPhi * NDoubletR * NDoubletZ;
    
        static constexpr int s_NumMaxSTgcElemets = NsTgStatEta * NsTgStatPhi * NsTgChamberLayer;
        static constexpr int s_NumMaxMMElements = NMMcStatEta * NMMcStatPhi * NMMcChamberLayer;
        std::array<std::unique_ptr<MdtReadoutElement>, MdtRElMaxHash> m_mdtArray;
        std::array<std::unique_ptr<CscReadoutElement>, CscRElMaxHash> m_cscArray;
        std::array<std::unique_ptr<TgcReadoutElement>, TgcRElMaxHash> m_tgcArray;
        
        std::array<std::unique_ptr<RpcReadoutElement>, s_NumMaxRpcElements> m_rpcArray;
        std::array<std::unique_ptr<sTgcReadoutElement>, s_NumMaxSTgcElemets> m_stgArray;
        std::array<std::unique_ptr<MMReadoutElement>, s_NumMaxMMElements> m_mmcArray;
        //
        std::array<const RpcReadoutElement*, RpcRElMaxHash> m_rpcArrayByHash{nullptr};
        std::map<std::string, std::unique_ptr<MuonStation> > m_MuonStationMap;

        unsigned int m_n_mdtRE{0};
        unsigned int m_n_cscRE{0};
        unsigned int m_n_rpcRE{0};
        unsigned int m_n_tgcRE{0};
        unsigned int m_n_stgRE{0};
        unsigned int m_n_mmcRE{0};

        unsigned int m_n_mdtDE{0};
        unsigned int m_n_cscDE{0};
        unsigned int m_n_rpcDE{0};
        unsigned int m_n_tgcDE{0};

        // pointers to the XxxDetectorElements (with granularity a la EDM)
        std::vector<std::unique_ptr<const TgcReadoutParams> > m_TgcReadoutParamsVec;

        ALineMapContainer m_aLineContainer;
        BLineMapContainer m_bLineContainer;
        CscInternalAlignmentMapContainer m_cscALineContainer;
        MdtAsBuiltMapContainer m_AsBuiltParamsMap;
        /// RPC name caches
        std::map<int, int> m_rpcStatToIdx;
        std::map<int, int> m_rpcIdxToStat;
        /// Cache the MDT station name integers and retrieve them from the 
        /// dict parser. Needed in stationIndex -> mdtStationName conversion
        int m_mdt_EIS_stName{-1}; //49
        int m_mdt_BIM_stName{-1}; //52
        int m_mdt_BME_stName{-1}; //53
        int m_mdt_BMG_stName{-1}; //54

#ifndef SIMULATIONBASE
        std::unique_ptr<NswAsBuilt::StripCalculator> m_MMAsBuiltCalculator;
        std::unique_ptr<NswAsBuilt::StgcStripCalculator> m_StgcAsBuiltCalculator;
#endif

        template <typename read_out, size_t N> void clearCache(std::array<std::unique_ptr<read_out>, N>& array);
        template <typename read_out, size_t N> void fillCache(std::array<std::unique_ptr<read_out>, N>& array);
        template <typename read_out, size_t N> void refreshCache(std::array<std::unique_ptr<read_out>, N>& array);
    };

    const MdtIdHelper* MuonDetectorManager::mdtIdHelper() const { 
        return m_idHelperSvc->hasMDT() ? &(m_idHelperSvc->mdtIdHelper()) : nullptr;
    }
    const CscIdHelper* MuonDetectorManager::cscIdHelper() const { 
        return m_idHelperSvc->hasCSC() ?  &(m_idHelperSvc->cscIdHelper()) : nullptr; 
    }
    const RpcIdHelper* MuonDetectorManager::rpcIdHelper() const { 
        return m_idHelperSvc->hasRPC() ? &(m_idHelperSvc->rpcIdHelper()): nullptr; 
    }
    const TgcIdHelper* MuonDetectorManager::tgcIdHelper() const { 
        return m_idHelperSvc->hasTGC() ? &(m_idHelperSvc->tgcIdHelper()) : nullptr; 
    }
    const sTgcIdHelper* MuonDetectorManager::stgcIdHelper() const { 
        return  m_idHelperSvc->hasSTGC() ? &(m_idHelperSvc->stgcIdHelper()) : nullptr; 
    }
    const MmIdHelper* MuonDetectorManager::mmIdHelper() const { 
        return  m_idHelperSvc->hasMM() ? &(m_idHelperSvc->mmIdHelper()) : nullptr; 
    }

    const GenericRPCCache* MuonDetectorManager::getGenericRpcDescriptor() const { return &m_genericRPC; }
    const GenericMDTCache* MuonDetectorManager::getGenericMdtDescriptor() const { return &m_genericMDT; }

    const GenericCSCCache* MuonDetectorManager::getGenericCscDescriptor() const { return &m_genericCSC; }
    const GenericTGCCache* MuonDetectorManager::getGenericTgcDescriptor() const { return &m_genericTGC; }

    int MuonDetectorManager::MinimalGeoFlag() const { return m_minimalgeo; }
    int MuonDetectorManager::IncludeCutoutsFlag() const { return m_includeCutouts; }
    int MuonDetectorManager::IncludeCutoutsBogFlag() const { return m_includeCutoutsBog; }

    std::string MuonDetectorManager::geometryVersion() const { return m_geometryVersion; }
    std::string MuonDetectorManager::get_DBMuonVersion() const { return m_DBMuonVersion; }

    
    unsigned int MuonDetectorManager::nMuonStation() const { return m_MuonStationMap.size(); }
    unsigned int MuonDetectorManager::nMdtRE() const { return m_n_mdtRE; }
    unsigned int MuonDetectorManager::nCscRE() const { return m_n_cscRE; }
    unsigned int MuonDetectorManager::nRpcRE() const { return m_n_rpcRE; }
    unsigned int MuonDetectorManager::nTgcRE() const { return m_n_tgcRE; }
    unsigned int MuonDetectorManager::nsTgcRE() const { return m_n_stgRE; }
    unsigned int MuonDetectorManager::nMMRE() const { return m_n_mmcRE; }

    unsigned int MuonDetectorManager::nMdtDE() const { return m_n_mdtDE; }
    unsigned int MuonDetectorManager::nCscDE() const { return m_n_cscDE; }
    unsigned int MuonDetectorManager::nRpcDE() const { return m_n_rpcDE; }
    unsigned int MuonDetectorManager::nTgcDE() const { return m_n_tgcDE; }

    const ALineMapContainer* MuonDetectorManager::ALineContainer() const { return &m_aLineContainer; }

    const BLineMapContainer* MuonDetectorManager::BLineContainer() const { return &m_bLineContainer; }

    const CscInternalAlignmentMapContainer* MuonDetectorManager::CscInternalAlignmentContainer() const { return &m_cscALineContainer; }

    const MdtAsBuiltMapContainer* MuonDetectorManager::MdtAsBuiltContainer() const { return &m_AsBuiltParamsMap; }

    ciALineMap MuonDetectorManager::ALineMapBegin() const { return m_aLineContainer.begin(); }
    ciBLineMap MuonDetectorManager::BLineMapBegin() const { return m_bLineContainer.begin(); }
    ciALineMap MuonDetectorManager::ALineMapEnd() const { return m_aLineContainer.end(); }
    ciBLineMap MuonDetectorManager::BLineMapEnd() const { return m_bLineContainer.end(); }
    ciCscInternalAlignmentMap MuonDetectorManager::CscALineMapBegin() const { return m_cscALineContainer.begin(); }
    ciCscInternalAlignmentMap MuonDetectorManager::CscALineMapEnd() const { return m_cscALineContainer.end(); }
    ciMdtAsBuiltMap MuonDetectorManager::MdtAsBuiltMapBegin() const { return m_AsBuiltParamsMap.begin(); }
    ciMdtAsBuiltMap MuonDetectorManager::MdtAsBuiltMapEnd() const { return m_AsBuiltParamsMap.end(); }

    int MuonDetectorManager::cacheFillingFlag() const { return m_cacheFillingFlag; }
    int MuonDetectorManager::cachingFlag() const { return m_cachingFlag; }

}  // namespace MuonGM

#ifndef GAUDI_NEUTRAL
namespace MuonGM {
    class MuonDetectorManager;
}
CLASS_DEF(MuonGM::MuonDetectorManager, 4500, 1);
CONDCONT_MIXED_DEF(MuonGM::MuonDetectorManager, 205781622);
#endif

#endif
