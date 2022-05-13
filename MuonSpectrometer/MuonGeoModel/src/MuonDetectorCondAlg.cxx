/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MuonDetectorCondAlg.h"

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/CondCont.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "MuonDetDescrUtils/BuildNSWReadoutGeometry.h"
#include "MuonGeoModel/MuonDetectorFactory001.h"
#include "MuonGeoModel/MuonDetectorTool.h"

#include <fstream>

MuonDetectorCondAlg::MuonDetectorCondAlg(const std::string &name, ISvcLocator *pSvcLocator) : AthAlgorithm(name, pSvcLocator)
{ }

StatusCode MuonDetectorCondAlg::initialize() {
    ATH_MSG_DEBUG("Initializing ...");

    // Retrieve the MuonDetectorManager from the detector store to get
    // the applyCscIntAlignment() and applyMdtAsBuiltParams() flags
    std::string managerName = "Muon";
    const MuonGM::MuonDetectorManager *MuonDetMgrDS;
    if (detStore()->retrieve(MuonDetMgrDS).isFailure()) {
        ATH_MSG_INFO("Could not find the MuonGeoModel Manager: " << managerName << " from the Detector Store! ");
        return StatusCode::FAILURE;
    } else {
        ATH_MSG_DEBUG(" Found the MuonGeoModel Manager from the Detector Store");
    }

    // Read Handles
    ATH_CHECK(m_iGeoModelTool.retrieve());

    ATH_CHECK(m_readALineKey.initialize());
    ATH_CHECK(m_readBLineKey.initialize());
    ATH_CHECK(m_readILineKey.initialize(MuonDetMgrDS->applyCscIntAlignment()));
    ATH_CHECK(m_readMdtAsBuiltKey.initialize(MuonDetMgrDS->applyMdtAsBuiltParams()));
    ATH_CHECK(m_readNswAsBuiltKey.initialize(MuonDetMgrDS->applyNswAsBuiltParams()));

    ATH_CHECK(m_condMmPassivKey.initialize(m_applyMmPassivation));
    ATH_CHECK(m_idHelperSvc.retrieve());

    // Write Handles
    // std::string ThisKey = "MuonDetectorManager";
    // std::size_t pos = name().find("MuonDetectorCondAlg");
    // m_writeDetectorManagerKey = ThisKey + name().substr (pos);
    ATH_CHECK(m_writeDetectorManagerKey.initialize());

    return StatusCode::SUCCESS;
}

StatusCode MuonDetectorCondAlg::execute() {
    ATH_MSG_DEBUG("execute " << name());

    // =======================
    // Write ILine Cond Handle
    // =======================
    SG::WriteCondHandle<MuonGM::MuonDetectorManager> writeHandle{m_writeDetectorManagerKey};
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                                    << ". In theory this should not be called, but may happen"
                                    << " if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS;
    }

    // =======================
    // Create the MuonDetectorManager by calling the MuonDetectorFactory001
    // =======================
    const MuonDetectorTool *MuDetTool = m_iGeoModelTool.get();
    MuonGM::MuonDetectorFactory001 theFactory(detStore().operator->());
    if (MuDetTool->createFactory(theFactory).isFailure()) {
        ATH_MSG_FATAL("unable to create MuonDetectorFactory001 ");
        return StatusCode::FAILURE;
    }

    std::unique_ptr<MuonGM::MuonDetectorManager> MuonMgrData(theFactory.getDetectorManager());

    // =======================
    // Add NSW to the MuonDetectorManager by calling BuildReadoutGeometry from MuonAGDDToolHelper
    // =======================
    if (MuonMgrData->mmIdHelper() && MuonMgrData->stgcIdHelper()) {
        BuildNSWReadoutGeometry theBuilder = BuildNSWReadoutGeometry();
        bool success=false;
        if(m_applyMmPassivation){
            const EventContext& ctx = Gaudi::Hive::currentContext();
            SG::ReadCondHandle<NswPassivationDbData> readMmPass{m_condMmPassivKey, ctx};
            if(!readMmPass.isValid()){
              ATH_MSG_ERROR("Cannot find conditions data container for MM passivation!");
              return StatusCode::FAILURE;
            }
            success = theBuilder.BuildReadoutGeometry(MuonMgrData.get(), readMmPass.cptr());
        }
        else {
            success = theBuilder.BuildReadoutGeometry(MuonMgrData.get(), nullptr);
        }
        if(!success)
            ATH_MSG_FATAL("unable to add NSW ReadoutGeometry in the MuonDetectorManager in conditions store");
    }

    // =======================
    // Update CSC Internal Alignment if requested
    // =======================

    if (MuonMgrData->applyCscIntAlignment()) {
        SG::ReadCondHandle<CscInternalAlignmentMapContainer> readILinesHandle{m_readILineKey};
        const CscInternalAlignmentMapContainer *readILinesCdo{*readILinesHandle};

        writeHandle.addDependency(readILinesHandle);

        if (MuonMgrData->updateCSCInternalAlignmentMap(*readILinesCdo).isFailure())
            ATH_MSG_ERROR("Unable to update CSC/ILINES");
        else
            ATH_MSG_DEBUG("update CSC/ILINES DONE");
    }

    // =======================
    // Update MdtAsBuiltMapContainer if requested BEFORE updating ALINES and BLINES
    // =======================
    if (MuonMgrData->applyMdtAsBuiltParams()) {
        SG::ReadCondHandle<MdtAsBuiltMapContainer> readMdtAsBuiltHandle{m_readMdtAsBuiltKey};
        const MdtAsBuiltMapContainer *readMdtAsBuiltCdo{*readMdtAsBuiltHandle};
        writeHandle.addDependency(readMdtAsBuiltHandle);

        if (MuonMgrData->updateMdtAsBuiltParams(*readMdtAsBuiltCdo).isFailure())
            ATH_MSG_ERROR("Unable to update MDT AsBuilt parameters");
        else
            ATH_MSG_DEBUG("update MDT AsBuilt parameters DONE");
    }

    // =======================
    // Set NSW as-built geometry if requested
    // =======================
    if (MuonMgrData->applyNswAsBuiltParams()) {
        SG::ReadCondHandle<NswAsBuiltDbData> readNswAsBuilt{m_readNswAsBuiltKey};
        if(!readNswAsBuilt.isValid())
          ATH_MSG_ERROR("Cannot find conditions data container for NSW as-built!");
        else
            ATH_MSG_DEBUG("Retrieved conditions data container for NSW as-built");
        const NswAsBuiltDbData* nswAsBuiltData = readNswAsBuilt.cptr();
        MuonMgrData->setMMAsBuiltCalculator(nswAsBuiltData);
    }

    // =======================
    // Update Alignment, ALINES
    // =======================

    SG::ReadCondHandle<ALineMapContainer> readALinesHandle{m_readALineKey};
    if (MuonMgrData->updateAlignment(**readALinesHandle, m_isData).isFailure())
        ATH_MSG_ERROR("Unable to update Alignment");
    else
        ATH_MSG_DEBUG("update Alignment DONE");

    // =======================
    // Update Deformations, BLINES
    // =======================

    SG::ReadCondHandle<BLineMapContainer> readBLinesHandle{m_readBLineKey};

    writeHandle.addDependency(readALinesHandle, readBLinesHandle);

    if (MuonMgrData->updateDeformations(**readBLinesHandle, m_isData).isFailure())
        ATH_MSG_ERROR("Unable to update Deformations");
    else
        ATH_MSG_DEBUG("update Deformations DONE");

    // !!!!!!!! UPDATE ANYTHING ELSE ???????

    if (writeHandle.record(std::move(MuonMgrData)).isFailure()) {
        ATH_MSG_FATAL("Could not record MuonDetectorManager " << writeHandle.key() << " with EventRange " << writeHandle.getRange() << " into Conditions Store");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

    return StatusCode::SUCCESS;
}
