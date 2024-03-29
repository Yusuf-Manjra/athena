/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_CSCRAWDATAPROVIDER_H
#define MUONBYTESTREAM_CSCRAWDATAPROVIDER_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "IRegionSelector/IRegSelTool.h"
#include "MuonAlignmentData/CorrContainer.h"  // for ALineMapContainer
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

namespace Muon {

    class CscRawDataProvider : public AthReentrantAlgorithm {
    public:
        //! Constructor.
        CscRawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

        //! Initialize
        virtual StatusCode initialize();

        //! Execute
        virtual StatusCode execute(const EventContext &ctx) const;

        //! Destructor
        ~CscRawDataProvider() = default;

    private:
        ToolHandle<Muon::IMuonRawDataProviderTool> m_rawDataTool{this, "ProviderTool",
                                                                 "Muon::CSC_RawDataProviderToolMT/CscRawDataProviderTool"};

        /// Handle for region selector service
        ToolHandle<IRegSelTool> m_regsel_csc{this, "RegionSelectionTool", "RegSelTool/RegSelTool_CSC", "CSC Region Selector Tool"};

        /// Property to decide whether or not to do RoI based decoding
        Gaudi::Property<bool> m_seededDecoding{this, "DoSeededDecoding", false, "If true do decoding in RoIs"};

        /// ReadHandle for the input RoIs
        SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{this, "RoIs", "OutputRoIs", "Name of RoI collection to read in"};

        SG::ReadCondHandleKey<ALineMapContainer> m_ALineKey{
            this, "ALineMapContainer", "ALineMapContainer",
            "Name of muon alignment ALine condition data"};  // !!! REMOVEME: when MuonDetectorManager in cond store
    };
}  // namespace Muon

#endif
