/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <numeric>
#include <set>
#include <utility>

#include "ByteStreamCnvSvcBase/FullEventAssembler.h"

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigT1CaloEvent/CPMTobRoI.h"

#include "CpmRoiSubBlockV2.h"
#include "L1CaloErrorByteStreamTool.h"
#include "L1CaloSrcIdMap.h"
#include "L1CaloUserHeader.h"

#include "CpmRoiByteStreamV2Tool.h"

namespace LVL1BS
{

// Interface ID

static const InterfaceID IID_ICpmRoiByteStreamV2Tool("CpmRoiByteStreamV2Tool",
        1, 1);

const InterfaceID &CpmRoiByteStreamV2Tool::interfaceID()
{
    return IID_ICpmRoiByteStreamV2Tool;
}

// Constructor

CpmRoiByteStreamV2Tool::CpmRoiByteStreamV2Tool(const std::string &type,
        const std::string &name,
        const IInterface  *parent)
    : AthAlgTool(type, name, parent),
      m_robDataProvider("ROBDataProviderSvc", name),
      m_errorTool("LVL1BS::L1CaloErrorByteStreamTool/L1CaloErrorByteStreamTool"),
      m_crates(4), m_modules(14), m_srcIdMap(0), m_subBlock(0), m_rodStatus(0),
      m_fea(0)
{
    declareInterface<CpmRoiByteStreamV2Tool>(this);

    declareProperty("ErrorTool", m_errorTool,
                    "Tool to collect errors for monitoring");
    declareProperty("CrateOffsetHw",  m_crateOffsetHw = 8,
                    "Offset of CP crate numbers in bytestream");
    declareProperty("CrateOffsetSw",  m_crateOffsetSw = 0,
                    "Offset of CP crate numbers in RDOs");

    // Properties for reading bytestream only
    declareProperty("ROBSourceIDs",       m_sourceIDs,
                    "ROB fragment source identifiers");
    declareProperty("ROBSourceIDsRoIB",   m_sourceIDsRoIB,
                    "ROB fragment source identifiers for RoIBs");

    // Properties for writing bytestream only
    declareProperty("DataVersion",    m_version       = 2,
                    "Format version number in sub-block header");
    declareProperty("DataFormat",     m_dataFormat    = 1,
                    "Format identifier (0-1) in sub-block header");
    declareProperty("SlinksPerCrate", m_slinks        = 1,
                    "The number of S-Links per crate");
    declareProperty("CrateMin",       m_crateMin = 0,
                    "Minimum crate number, allows partial output");
    declareProperty("CrateMax",       m_crateMax = m_crates - 1,
                    "Maximum crate number, allows partial output");

    declareProperty("IsM7Format", m_isM7Format = false,
                    "Set it for M7 raw data");

}

// Destructor

CpmRoiByteStreamV2Tool::~CpmRoiByteStreamV2Tool()
{
}

// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode CpmRoiByteStreamV2Tool::initialize()
{
    msg(MSG::INFO) << "Initializing " << name() << " - package version "
                   << PACKAGE_VERSION << endmsg;

    StatusCode sc = m_errorTool.retrieve();
    if (sc.isFailure())
    {
        msg(MSG::ERROR) << "Failed to retrieve tool " << m_errorTool << endmsg;
        return sc;
    }
    else msg(MSG::INFO) << "Retrieved tool " << m_errorTool << endmsg;

    m_subDetector = eformat::TDAQ_CALO_CLUSTER_PROC_ROI;
    m_srcIdMap    = new L1CaloSrcIdMap();
    m_rodStatus   = new std::vector<uint32_t>(2);
    m_subBlock    = new CpmRoiSubBlockV2();
    m_fea         = new FullEventAssembler<L1CaloSrcIdMap>();
    return StatusCode::SUCCESS;
}

// Finalize

StatusCode CpmRoiByteStreamV2Tool::finalize()
{
    delete m_fea;
    delete m_subBlock;
    delete m_rodStatus;
    delete m_srcIdMap;
    return StatusCode::SUCCESS;
}

// Convert ROB fragments to CPM RoIs

StatusCode CpmRoiByteStreamV2Tool::convert(
    const std::string& sgKey,
    DataVector<LVL1::CPMTobRoI> *const collection)
{
 const std::vector<uint32_t>& vID(sourceIDs(sgKey));
  // // get ROB fragments
  IROBDataProviderSvc::VROBFRAG robFrags;
  m_robDataProvider->getROBData(vID, robFrags, "CpmRoiByteStreamV2Tool");
  ATH_MSG_DEBUG("Number of ROB fragments:" << robFrags.size());
  return convert(robFrags, collection);
}

StatusCode CpmRoiByteStreamV2Tool::convert(
    const IROBDataProviderSvc::VROBFRAG &robFrags,
    DataVector<LVL1::CPMTobRoI> *const roiCollection)
{
    const bool debug = msgLvl(MSG::DEBUG);
    if (debug) msg(MSG::DEBUG);

    // Loop over ROB fragments

    int robCount = 0;
    std::set<uint32_t> dupCheck;
    std::set<uint32_t> dupRoiCheck;
    ROBIterator rob    = robFrags.begin();
    ROBIterator robEnd = robFrags.end();
    for (; rob != robEnd; ++rob)
    {

        if (debug)
        {
            ++robCount;
            msg() << "Treating ROB fragment " << robCount << endmsg;
        }

        // Skip fragments with ROB status errors

        uint32_t robid = (*rob)->source_id();
        if ((*rob)->nstatus() > 0)
        {
            ROBPointer robData;
            (*rob)->status(robData);
            if (*robData != 0)
            {
                m_errorTool->robError(robid, *robData);
                if (debug) msg() << "ROB status error - skipping fragment" << endmsg;
                continue;
            }
        }

        // Skip duplicate fragments

        if (!dupCheck.insert(robid).second)
        {
            m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_DUPLICATE_ROB);
            if (debug) msg() << "Skipping duplicate ROB fragment" << endmsg;
            continue;
        }

        // Unpack ROD data (slinks)

        RODPointer payloadBeg;
        RODPointer payload;
        RODPointer payloadEnd;
        (*rob)->rod_data(payloadBeg);
        payloadEnd = payloadBeg + (*rob)->rod_ndata();
        payload = payloadBeg;
        if (payload == payloadEnd)
        {
            if (debug) msg() << "ROB fragment empty" << endmsg;
            continue;
        }

        // Check identifier
        const uint32_t sourceID = (*rob)->rod_source_id();
        if (m_srcIdMap->getRobID(sourceID) != robid           ||
                m_srcIdMap->subDet(sourceID)   != m_subDetector   ||
                m_srcIdMap->daqOrRoi(sourceID) != 1               ||
                (m_srcIdMap->slink(sourceID) != 0 && m_srcIdMap->slink(sourceID) != 2) ||
                m_srcIdMap->crate(sourceID)    <  m_crateOffsetHw ||
                m_srcIdMap->crate(sourceID)    >= m_crateOffsetHw + m_crates)
        {
            m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_ROD_ID);
            if (debug)
            {
                msg() << "Wrong source identifier in data: "
                      << MSG::hex << sourceID << MSG::dec << endmsg;
            }
            continue;
        }

        // Check minor version
        const int minorVersion = (*rob)->rod_version() & 0xffff;
        if (minorVersion <= m_srcIdMap->minorVersionPreLS1())
        {
            if (debug) msg() << "Skipping pre-LS1 data" << endmsg;
            continue;
        }
        const int rodCrate = m_srcIdMap->crate(sourceID);
        if (debug)
        {
            msg() << "Treating crate " << rodCrate
                  << " slink " << m_srcIdMap->slink(sourceID) << endmsg;
        }

        // First word may be User Header
        if (L1CaloUserHeader::isValid(*payload))
        {
            L1CaloUserHeader userHeader(*payload);
            userHeader.setVersion(minorVersion);
            const int headerWords = userHeader.words();
            if (headerWords != 1 )
            {
                m_errorTool->rodError(robid, L1CaloSubBlock::ERROR_USER_HEADER);
                if (debug) msg() << "Unexpected number of user header words: "
                                     << headerWords << endmsg;
                continue;
            }
            for (int i = 0; i < headerWords; ++i) ++payload;
        }

        // Loop over sub-blocks if there are any

        unsigned int rodErr = L1CaloSubBlock::ERROR_NONE;
        while (payload != payloadEnd)
        {

            if (L1CaloSubBlock::wordType(*payload) == L1CaloSubBlock::HEADER)
            {
                m_subBlock->clear();
                payload = m_subBlock->read(payload, payloadEnd);
                if (debug)
                {
                    msg() << "CPM RoI sub-block: Crate " << m_subBlock->crate()
                          << "  Module " << m_subBlock->module() << endmsg;
                }
                // Unpack sub-block
                if (m_subBlock->dataWords() && !m_subBlock->unpack())
                {
                    if (debug)
                    {
                        std::string errMsg(m_subBlock->unpackErrorMsg());
                        msg() << "CPM RoI sub-block unpacking failed: " << errMsg << endmsg;
                    }
                    rodErr = m_subBlock->unpackErrorCode();
                    break;
                }
                const int numChips = 8;
                const int numLocs  = 2;
                const int numTypes = 2;
                for (int chip = 0; chip < numChips; ++chip)
                {
                    for (int loc = 0; loc < numLocs; ++loc)
                    {
                        for (int type = 0; type < numTypes; ++type)
                        {
                            const LVL1::CPMTobRoI roi = m_subBlock->roi(chip, loc, type);
                            if (roi.energy() || roi.isolation())
                            {
                                roiCollection->push_back(new LVL1::CPMTobRoI(roi));
                            }
                        }
                    }
                }
            }
            else
            {
                // Just RoI word
                LVL1::CPMTobRoI roi;
                uint32_t roiWord = *payload;
                if (m_isM7Format) 
                    roiWord |= 0x80000000;
                
                if (roi.setRoiWord(roiWord))
                {
                    if (roi.crate() != rodCrate - m_crateOffsetHw)
                    {
                        if (debug) msg() << "Inconsistent RoI crate number: "
                                             << roi.crate() << endmsg;
                        rodErr = L1CaloSubBlock::ERROR_CRATE_NUMBER;
                        break;
                    }
                    if (roi.cpm() == 0 || roi.cpm() > m_modules)
                    {
                        if (debug) msg() << "Invalid CPM number: "
                                             << roi.cpm() << endmsg;
                        rodErr = L1CaloSubBlock::ERROR_MODULE_NUMBER;
                        break;
                    }
                    const uint32_t location = roiWord & 0xffff0000;
                    if (dupRoiCheck.insert(location).second)
                    {
                        if (roi.energy() || roi.isolation())
                        {
                            roiCollection->push_back(new LVL1::CPMTobRoI(roiWord));
                        }
                    }
                    else
                    {
                        if (debug) msg() << "Duplicate RoI word "
                                             << MSG::hex << roiWord << MSG::dec << endmsg;
                        rodErr = L1CaloSubBlock::ERROR_DUPLICATE_DATA;
                        break;
                    }
                }
                else
                {
                    if (debug) msg() << "Invalid RoI word "
                                         << MSG::hex << roiWord << MSG::dec << endmsg;
                    rodErr = L1CaloSubBlock::ERROR_ROI_TYPE;
                    break;
                }
                ++payload;
            }
        }
        if (rodErr != L1CaloSubBlock::ERROR_NONE)
            m_errorTool->rodError(robid, rodErr);
    }
    if (debug)
    {
        msg() << "Number of RoIs read = " << roiCollection->size() << endmsg;
    }

    return StatusCode::SUCCESS;
}

// Convert CPM RoI to bytestream

StatusCode CpmRoiByteStreamV2Tool::convert(
    const DataVector<LVL1::CPMTobRoI> *const roiCollection,
    RawEventWrite *const re)
{
    const bool debug = msgLvl(MSG::DEBUG);
    if (debug) msg(MSG::DEBUG);

    // Clear the event assembler

    m_fea->clear();
    const uint16_t minorVersion = m_srcIdMap->minorVersion();
    m_fea->setRodMinorVersion(minorVersion);
    m_rodStatusMap.clear();

    // Pointer to ROD data vector

    FullEventAssembler<L1CaloSrcIdMap>::RODDATA *theROD = 0;

    // Set up the container map

    setupCpmRoiMap(roiCollection);
    if (debug)
    {
        msg() << "Number of RoIs to be written = " << roiCollection->size()
              << " (collection), " << m_roiMap.size() << " (map)"
              << endmsg;
    }
    int count = 0;
    CpmRoiMap::const_iterator mapIter    = m_roiMap.begin();
    CpmRoiMap::const_iterator mapIterEnd = m_roiMap.end();

    // Loop over data

    const bool neutralFormat = m_dataFormat == L1CaloSubBlock::NEUTRAL;
    const int  modulesPerSlink = m_modules / m_slinks;
    for (int crate = m_crateMin; crate <= m_crateMax; ++crate)
    {
        const int hwCrate = crate + m_crateOffsetHw;

        // CPM modules are numbered 1 to m_modules
        for (int module = 1; module <= m_modules; ++module)
        {
            const int mod = module - 1;

            // Pack required number of modules per slink

            if (mod % modulesPerSlink == 0)
            {
                const int daqOrRoi = 1;
                const int slink = (m_slinks == 2) ? 2 * (mod / modulesPerSlink)
                                  : mod / modulesPerSlink;
                if (debug)
                {
                    msg() << "Treating crate " << hwCrate
                          << " slink " << slink << endmsg
                          << "Data Version/Format: " << m_version
                          << " " << m_dataFormat << endmsg;
                }
                const uint32_t rodIdCpm = m_srcIdMap->getRodID(hwCrate, slink, daqOrRoi,
                                          m_subDetector);
                theROD = m_fea->getRodData(rodIdCpm);
                if (neutralFormat)
                {
                    const L1CaloUserHeader userHeader;
                    theROD->push_back(userHeader.header());
                }
                m_rodStatusMap.insert(make_pair(rodIdCpm, m_rodStatus));
            }
            if (debug) msg() << "Module " << module << endmsg;

            // Create a sub-block (Neutral format only)

            if (neutralFormat)
            {
                m_subBlock->clear();
                m_subBlock->setRoiHeader(m_version, hwCrate, module);
            }

            // Find CPM RoIs for this module

            for (; mapIter != mapIterEnd; ++mapIter)
            {
                const LVL1::CPMTobRoI *const roi = mapIter->second;
                if (roi->crate() < crate)  continue;
                if (roi->crate() > crate)  break;
                if (roi->cpm()   < module) continue;
                if (roi->cpm()   > module) break;
                if (roi->energy() || roi->isolation())
                {
                    if (neutralFormat) m_subBlock->fillRoi(*roi);
                    else theROD->push_back(roi->roiWord());
                    ++count;
                }
            }

            // Pack and write the sub-block

            if (neutralFormat)
            {
                if ( !m_subBlock->pack())
                {
                    msg(MSG::ERROR) << "CPMTobRoI sub-block packing failed" << endmsg;
                    return StatusCode::FAILURE;
                }
                if (debug)
                {
                    msg() << "CPMTobRoI sub-block data words: "
                          << m_subBlock->dataWords() << endmsg;
                }
                m_subBlock->write(theROD);
            }
        }
    }
    if (debug)
    {
        msg() << "Number of RoIs written = " << count << endmsg;
    }

    // Fill the raw event

    m_fea->fill(re, msg());
    if (debug) msg() << MSG::dec; // fill seems to leave it in hex

    // Set ROD status words

    //L1CaloRodStatus::setStatus(re, m_rodStatusMap, m_srcIdMap);

    return StatusCode::SUCCESS;
}

// Return reference to vector with all possible Source Identifiers

const std::vector<uint32_t> &CpmRoiByteStreamV2Tool::sourceIDs(
    const std::string &sgKey)
{
    const std::string flag("RoIB");
    const std::string::size_type pos = sgKey.find(flag);
    const bool roiDaq =
        (pos == std::string::npos || pos != sgKey.length() - flag.length());
    const bool empty  = (roiDaq) ? m_sourceIDs.empty() : m_sourceIDsRoIB.empty();
    if (empty)
    {
        const int maxCrates = m_crates + m_crateOffsetHw;
        const int maxSlinks = m_srcIdMap->maxSlinks();
        for (int hwCrate = m_crateOffsetHw; hwCrate < maxCrates; ++hwCrate)
        {
            for (int slink = 0; slink < maxSlinks; ++slink)
            {
                const int daqOrRoi = 1;
                const uint32_t rodId = m_srcIdMap->getRodID(hwCrate, slink, daqOrRoi,
                                       m_subDetector);
                const uint32_t robId = m_srcIdMap->getRobID(rodId);
                if (roiDaq)
                {
                    if (slink < 2) m_sourceIDs.push_back(robId);
                }
                else if (slink >= 2) m_sourceIDsRoIB.push_back(robId);
            }
        }
    }
    return (roiDaq) ? m_sourceIDs : m_sourceIDsRoIB;
}

// Set up CPM RoI map

void CpmRoiByteStreamV2Tool::setupCpmRoiMap(const CpmRoiCollection *
        const roiCollection)
{
    m_roiMap.clear();
    if (roiCollection)
    {
        CpmRoiCollection::const_iterator pos  = roiCollection->begin();
        CpmRoiCollection::const_iterator pose = roiCollection->end();
        for (; pos != pose; ++pos)
        {
            const LVL1::CPMTobRoI *const roi = *pos;
            const int type = roi->type();
            const int crate = roi->crate();
            const int cpm = roi->cpm();
            const int chip = roi->chip();
            const int loc = roi->location() >> 2;
            const uint32_t key = (((((((crate << 4) | cpm) << 3) | chip) << 1) | loc) << 1) | type;
            m_roiMap.insert(std::make_pair(key, roi));
        }
    }
}

} // end namespace
