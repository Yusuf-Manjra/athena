/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTErrMonAlg.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "StoreGate/ReadHandle.h"

using namespace std;
using namespace SCT_Monitoring;

SCTErrMonAlg::SCTErrMonAlg(const std::string& name, ISvcLocator* pSvcLocator)
  :AthMonitorAlgorithm(name,pSvcLocator) {
}

StatusCode SCTErrMonAlg::initialize() {
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));
  ATH_CHECK(m_byteStreamErrTool.retrieve());
  ATH_CHECK(m_configurationTool.retrieve());
  if (m_useDCS) ATH_CHECK(m_dcsTool.retrieve());
  else m_dcsTool.disable();
  ATH_CHECK(m_pSummaryTool.retrieve());

  // Retrieve geometrical information
  const InDetDD::SCT_DetectorManager* sctManager{nullptr};
  ATH_CHECK(detStore()->retrieve(sctManager, "SCT"));
  const unsigned int maxHash{static_cast<unsigned int>(m_pSCTHelper->wafer_hash_max())}; // 8176
  moduleGeo_t moduleGeo; // dummy value
  m_geo.resize(maxHash, moduleGeo);
  double rz{0.};
  const double deltaZ{0.};
  for (unsigned int i{0}; i<maxHash; i++) {
    IdentifierHash hash{i};
    const InDetDD::SiDetectorElement* newElement{sctManager->getDetectorElement(hash)};
    newElement->getEtaPhiRegion(deltaZ,
                                moduleGeo.first.first,  moduleGeo.first.second,
                                moduleGeo.second.first, moduleGeo.second.second,
                                rz);
    m_geo[i] = moduleGeo;
  }

  // Fill existing wafers
  m_indicesVector.reserve(maxHash);
  SCT_ID::const_id_iterator waferIterator{m_pSCTHelper->wafer_begin()};
  SCT_ID::const_id_iterator waferEnd{m_pSCTHelper->wafer_end()};
  for (; waferIterator not_eq waferEnd; ++waferIterator) {
    const Identifier waferId{*waferIterator};
    int layer{m_pSCTHelper->layer_disk(waferId)};
    int side{m_pSCTHelper->side(waferId)};
    int barrel_ec{m_pSCTHelper->barrel_ec(waferId)};
    int ieta{m_pSCTHelper->eta_module(waferId)};
    int iphi{m_pSCTHelper->phi_module(waferId)};
    layer = layer * 2 + side;
    int regionIndex{GENERAL_INDEX};
    if ((barrel_ec == BARREL) and (layer >= 0) and (layer < N_BARRELSx2)) regionIndex = BARREL_INDEX;
    else if (barrel_ec == ENDCAP_A) regionIndex = ENDCAP_A_INDEX;
    else if (barrel_ec == ENDCAP_C) regionIndex = ENDCAP_C_INDEX;
    else continue;
    m_indicesVector.push_back(indices_t{regionIndex, layer, ieta, iphi});
  }

  return AthMonitorAlgorithm::initialize();
}

StatusCode SCTErrMonAlg::fillHistograms(const EventContext& ctx) const {
  SG::ReadHandle<xAOD::EventInfo> pEvent{m_EventInfoKey, ctx};
  if (not pEvent.isValid()) {
    ATH_MSG_WARNING("Could not retrieve event info!");
    return StatusCode::SUCCESS;
  }

  bool sctFlag{false};
  if (pEvent->errorState(xAOD::EventInfo::SCT) == xAOD::EventInfo::Error) {
    sctFlag = true;
  }
  /// Fill NumberOfSCTFlagErrorsVsLB, NumberOfEventsVsLB and FractionOfSCTFlagErrorsPerLB
  /// under /SCT/GENERAL/Conf/ ///
  auto lumiBlockAcc{Monitored::Scalar<int>("lumiBlock", pEvent->lumiBlock())};
  auto is1DAcc{Monitored::Scalar<bool>("is1D", true)};
  auto sctFlagAcc{Monitored::Scalar<bool>("sctFlag", sctFlag)};
  fill("SCTErrMonitor", lumiBlockAcc, is1DAcc, sctFlagAcc);

  if (sctFlag) {
    return StatusCode::SUCCESS;
  }

  // The numbers of disabled modules, links, strips do not change during a run.
  if (m_isFirstConfigurationDetails) {
    std::lock_guard{m_mutex};
    if (m_isFirstConfigurationDetails) {
      ATH_CHECK(fillConfigurationDetails(ctx));
      m_isFirstConfigurationDetails = false;
    }
  }

  ATH_CHECK(fillByteStreamErrors(ctx));

  /// Fill /SCT/GENERAL/Conf/SCTConfOutM ///
  // TODO: fill only at the end of a LB
  if (m_makeConfHisto) {
    int moduleOut{0};
    SCT_ID::const_id_iterator waferIterator{m_pSCTHelper->wafer_begin()};
    SCT_ID::const_id_iterator waferEnd{m_pSCTHelper->wafer_end()};
    for (; waferIterator not_eq waferEnd; ++waferIterator) {
      Identifier waferId{*waferIterator};
      if (m_pSCTHelper->side(waferId) == 0) { // Use only side 0 to check module level
        if (not m_configurationTool->isGood(waferId, ctx, InDetConditions::SCT_SIDE)) {
          moduleOut++;
        }
      }
    }
    auto moduleOutBinAcc{Monitored::Scalar<int>("moduleOutBin", 0)};
    auto moduleOutAcc{Monitored::Scalar<int>("moduleOut", moduleOut)};
    fill("SCTErrMonitor", moduleOutBinAcc, moduleOutAcc);
  }

  return StatusCode::SUCCESS;
}

StatusCode
SCTErrMonAlg::fillConfigurationDetails(const EventContext& ctx) const {
  ATH_MSG_DEBUG("Inside fillConfigurationDetails()");
  unsigned int nBadMods{static_cast<unsigned int>(m_configurationTool->badModules()->size())}; // bad modules
  const map<IdentifierHash, pair<bool, bool>>* badLinks{m_configurationTool->badLinks(ctx)}; // bad links
  unsigned int nBadLink0{0}, nBadLink1{0}, nBadLinkBoth{0};
  for (const pair<IdentifierHash, pair<bool, bool>>& link: *badLinks) {
    pair<bool, bool> status{link.second};
    if ((status.first == false) and (status.second == true)) {
      ++nBadLink0;
    }
    if ((status.first == true) and (status.second == false)) {
      ++nBadLink1;
    }
    if ((status.first == false) and (status.second == false)) {
      ++nBadLinkBoth;
    }
  }

  const map<Identifier, unsigned int>* badChips{m_configurationTool->badChips(ctx)}; // bad chips
  unsigned int nBadChips{0};
  for (const pair<Identifier, unsigned int>& chip : *badChips) {
    unsigned int status{chip.second};
    for (unsigned int i{0}; i < CHIPS_PER_MODULE; i++) {
      nBadChips += ((status & (1 << i)) == 0 ? 0 : 1);
    }
  }

  set<Identifier> badStripsAll; // bad strips
  m_configurationTool->badStrips(badStripsAll, ctx);
  unsigned int nBadStrips{static_cast<unsigned int>(badStripsAll.size())};

  set<Identifier> badStripsExclusive; // bad strips w/o bad modules and chips
  m_configurationTool->badStrips(badStripsExclusive, ctx, true, true);
  int nBadStripsExclusive{static_cast<int>(badStripsExclusive.size())};
  int nBadStripsExclusiveBEC[N_REGIONS] = {
    0, 0, 0
  };
  for (const Identifier& strip: badStripsExclusive) {
    int bec{m_pSCTHelper->barrel_ec(strip)};
    nBadStripsExclusiveBEC[bec2Index(bec)] += 1;
  }

  /// Fill /SCT/GENERAL/Conf/SCTConfDetails ///
  auto detailedConfBinAcc{Monitored::Scalar<int>("detailedConfBin")};
  auto nBadAcc{Monitored::Scalar<double>("nBad")};
  for (unsigned int i{0}; i<ConfbinsDetailed; i++) {
    detailedConfBinAcc = i;
    if (i==0) nBadAcc = nBadMods;
    else if (i==1) nBadAcc = nBadLink0;
    else if (i==2) nBadAcc = nBadLink1;
    else if (i==3) nBadAcc = nBadChips;
    else if (i==4) nBadAcc = static_cast<double>(nBadStripsExclusive) / 100.;
    fill("SCTErrMonitor", detailedConfBinAcc, nBadAcc);
  }

  ATH_MSG_DEBUG("-----------------------------------------------------------------------");
  ATH_MSG_DEBUG("Number of bad modules                          = " << nBadMods);
  ATH_MSG_DEBUG("Number of bad link 0                           = " << nBadLink0);
  ATH_MSG_DEBUG("Number of bad link 1                           = " << nBadLink1);
  ATH_MSG_DEBUG("Number of bad link both                        = " << nBadLinkBoth);
  ATH_MSG_DEBUG("Number of bad chips                            = " << nBadChips);
  ATH_MSG_DEBUG("Number of bad strips                           = " << nBadStrips);
  ATH_MSG_DEBUG("Number of bad strips exclusive                 = " << nBadStripsExclusive);
  ATH_MSG_DEBUG("Number of bad strips exclusive (ECC, B, ECA)   = "
                << nBadStripsExclusiveBEC[ENDCAP_C_INDEX] << ", "
                << nBadStripsExclusiveBEC[BARREL_INDEX] << ", "
                << nBadStripsExclusiveBEC[ENDCAP_A_INDEX] << ", ");
  ATH_MSG_DEBUG("-----------------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

StatusCode
SCTErrMonAlg::fillByteStreamErrors(const EventContext& ctx) const {
  SG::ReadHandle<xAOD::EventInfo> pEvent{m_EventInfoKey, ctx};
  if (not pEvent.isValid()) {
    ATH_MSG_WARNING("Could not retrieve event info!");
    return StatusCode::SUCCESS;
  }

  //--- Fill 1D histograms (vs LumiBlock) for each BS
  for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
    int nBSErrors{0};
    // get number of BS errors
    numByteStreamErrors(m_byteStreamErrTool->getErrorSet(errType), nBSErrors);
    /// Fill /SCT/GENERAL/Conf/SCT_*VsLbs ///
    auto lumiBlockAcc{Monitored::Scalar<int>("lumiBlock", pEvent->lumiBlock())};
    auto nBSErrorsAcc{Monitored::Scalar<int>("n_"+SCT_ByteStreamErrors::errorTypesDescription[errType], nBSErrors)};
    fill("SCTErrMonitor", lumiBlockAcc, nBSErrorsAcc);
  }

  categoryErrorMap_t categoryErrorMap;
  int total_errors{0};
  for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
    total_errors += fillByteStreamErrorsHelper(m_byteStreamErrTool->getErrorSet(errType), errType, categoryErrorMap);
  }
  /// Fill /SCT/GENERAL/errors/SCT_LinksWith*VsLbs ///
  for (int errCate{0}; errCate < CategoryErrors::N_ERRCATEGORY; ++errCate) {
    auto lumiBlockAcc{Monitored::Scalar<int>("lumiBlock", pEvent->lumiBlock())};
    auto nCategoryErrorsAcc{Monitored::Scalar<int>("n_"+CategoryErrorsNames[errCate],
                                                   categoryErrorMap.count(errCate))};
    fill("SCTErrMonitor", lumiBlockAcc, nCategoryErrorsAcc);

    for (const indices_t& indices : m_indicesVector) {
      const int region{indices[REGIONINDEX]};
      const int layer{indices[LAYERINDEX]};
      const int eta{indices[ETAINDEX]};
      const int phi{indices[PHIINDEX]};
      auto etaAcc{Monitored::Scalar<int>("eta", eta)};
      auto phiAcc{Monitored::Scalar<int>("phi", phi)};
      auto hasErrorAcc{Monitored::Scalar<bool>("hasError_"+CategoryErrorsNames[errCate]+"_"+subDetNameShort[region].Data()+"_"+to_string(layer/2)+"_"+to_string(layer%2),
                                               categoryErrorMap[errCate][region][layer][eta][phi])};
      fill("SCTErrMonitor", etaAcc, phiAcc, hasErrorAcc);
    }
  }
  
  if (m_coverageCheck) {
    ATH_MSG_INFO("Detector Coverage calculation starts" );

    static const std::string names[numberOfProblemForCoverage] = {
      "SCT_AllRegion", // All
      "SCT_MapOfDisabledLinks", // Disabled
      "SCT_MapOfLinksWithBadLinkLevelErrors", // BadLinkLevelError
      "SCT_MapOfLinksWithBadRODLevelErrors", // BadRODLevelError
      "SCT_MapOfLinksWithBadErrors", // BadError
      "SCT_MapOfLinksWithPSTrip", // PSTrip (DCS)
      "SCT_MapOfLinksWithAnyProbelm" // Summary
    };
    static const std::string titles[numberOfProblemForCoverage] = {
      "Map of All Region", // All
      "Map of Disabled Links", // Disabled
      "Map of Links with bad LinkLevelErrors", // BadLinkLevelError
      "Map of Links with Bad RODLevelErrors", // BadRODLevelError
      "Map of Links with Bad Errors", // BadError
      "Map of Links Affected by PS Trip", // PSTrip (DCS)
      "Map of Links with Any Bad Problem" // Summary
    };

    std::vector<TH2F> mapSCT; // TODO: Check if we need to record these histograms
    for (int iProblem{0}; iProblem<numberOfProblemForCoverage; iProblem++) {
      mapSCT.push_back(TH2F(names[iProblem].c_str(), titles[iProblem].c_str(),
                            s_nBinsEta, -s_rangeEta, s_rangeEta, s_nBinsPhi, -M_PI, M_PI));
      mapSCT[iProblem].GetXaxis()->SetTitle("#eta");
      mapSCT[iProblem].GetYaxis()->SetTitle("#phi");
    }

    std::set<IdentifierHash> sctHash[numberOfProblemForCoverage]{{}};
    disabledSCT(sctHash[disabled]);
    errorSCT(sctHash[badLinkError], sctHash[badRODError], sctHash[badError]);
    summarySCT(sctHash[allRegion], sctHash[summary]);
    float psTripModules{0.};
    psTripDCSSCT(sctHash[psTripDCS], psTripModules);

    for (int iProblem{0}; iProblem<numberOfProblemForCoverage; iProblem++) {
      for (const IdentifierHash& hash: sctHash[iProblem]) {
        fillWafer(m_geo[hash], mapSCT[iProblem]);
      }

      if (iProblem==allRegion) continue;

      double detector_coverage{calculateDetectorCoverage(mapSCT[iProblem], mapSCT[allRegion])};
      /// Fill /SCT/DetectorCoverage/SCT_Coverage*VsLbs ///
      auto lumiBlockAcc{Monitored::Scalar<int>("lumiBlock", pEvent->lumiBlock())};
      auto detectorCoverageAcc{Monitored::Scalar<double>("detectorCoverage"+coverageVarNames[iProblem], detector_coverage)};
      fill("SCTErrMonitor", lumiBlockAcc, detectorCoverageAcc);
    }
    
    /// Fill /SCT/DetectorCoverage/SCT_ModulesWithPSTripVsLbs ///
    // Modules affected by PS Tirp
    auto lumiBlockAcc{Monitored::Scalar<int>("lumiBlock", pEvent->lumiBlock())};
    auto psTripModulesAcc{Monitored::Scalar<int>("psTripModules", psTripModules)};
    fill("SCTErrMonitor", lumiBlockAcc, psTripModulesAcc);
  }

  return StatusCode::SUCCESS;
}

int
SCTErrMonAlg::fillByteStreamErrorsHelper(const set<IdentifierHash>& errors,
                                         int err_type,
                                         categoryErrorMap_t& categoryErrorMap) const {
  //--- Check categories of the BS error
  std::array<bool, CategoryErrors::N_ERRCATEGORY> b_category;
  b_category.fill(false);

  b_category[CategoryErrors::MASKEDLINKALL] =
    (err_type == SCT_ByteStreamErrors::MaskedLink) or (err_type == SCT_ByteStreamErrors::MaskedROD);

  b_category[CategoryErrors::SUMMARY] = true;

  b_category[CategoryErrors::BADERR] = false;
  for (SCT_ByteStreamErrors::errorTypes tmpBadError: SCT_ByteStreamErrors::BadErrors) {
    if (err_type == tmpBadError) {
      b_category[CategoryErrors::BADERR] = true;
      break;
    }
  }

  b_category[CategoryErrors::LINKLEVEL] = false;
  for (SCT_ByteStreamErrors::errorTypes linkLevelError: SCT_ByteStreamErrors::LinkLevelErrors) {
    if (err_type == linkLevelError) {
      b_category[CategoryErrors::LINKLEVEL] = true;
      break;
    }
  }

  b_category[CategoryErrors::RODLEVEL] = false;
  for (SCT_ByteStreamErrors::errorTypes rodLevelError: SCT_ByteStreamErrors::RodLevelErrors) {
    if (err_type == rodLevelError) {
      b_category[CategoryErrors::RODLEVEL] = true;
      break;
    }
  }

  b_category[CategoryErrors::MASKEDCHIP] =
    (err_type == SCT_ByteStreamErrors::TempMaskedChip0) or (err_type == SCT_ByteStreamErrors::TempMaskedChip1) or
    (err_type == SCT_ByteStreamErrors::TempMaskedChip2) or (err_type == SCT_ByteStreamErrors::TempMaskedChip3) or
    (err_type == SCT_ByteStreamErrors::TempMaskedChip4) or (err_type == SCT_ByteStreamErrors::TempMaskedChip5);

  std::vector<int> numErrorsPerLumi[N_REGIONS];
  if (m_doPerLumiErrors) {
    for (int reg{0}; reg<N_REGIONS; reg++) {
      const int nLayers{n_layers[reg]*2};
      numErrorsPerLumi[reg].resize(nLayers, 0);
    }
  }

  //--- Count BS errors
  int nerrors{0};
  int nMaskedLinks[N_REGIONS_INC_GENERAL]{0, 0, 0, 0};
  for (const auto& hash: errors) {
    nerrors++;
    if (not hash.is_valid()) continue;

    //--- FIll module information with BS error
    const Identifier fitId{m_pSCTHelper->wafer_id(hash)};
    int layer{m_pSCTHelper->layer_disk(fitId)};
    int side{m_pSCTHelper->side(fitId)};
    int barrel_ec{m_pSCTHelper->barrel_ec(fitId)};
    int ieta{m_pSCTHelper->eta_module(fitId)};
    int iphi{m_pSCTHelper->phi_module(fitId)};
    layer = layer * 2 + side;
    // barrel_ec = {ENDCAP_C=-2, BARREL=0, ENDCAP_A=2}
    // -> regionIndex = {ENDCAP_C_INDEX=0, BARREL_INDEX=1, ENDCAP_A_INDEX=2, GENERAL_INDEX=3}
    int regionIndex{GENERAL_INDEX};
    if ((barrel_ec == BARREL) and (layer >= 0) and (layer < N_BARRELSx2)) regionIndex = BARREL_INDEX;
    else if (barrel_ec == ENDCAP_A) regionIndex = ENDCAP_A_INDEX;
    else if (barrel_ec == ENDCAP_C) regionIndex = ENDCAP_C_INDEX;

    if (b_category[CategoryErrors::MASKEDLINKALL]) {
      nMaskedLinks[GENERAL_INDEX]++;
      if (regionIndex!=GENERAL_INDEX) {
        nMaskedLinks[regionIndex]++;
      }
    }

    if (m_doPerLumiErrors) numErrorsPerLumi[regionIndex][layer]++;

    for (int errCate{0}; errCate < CategoryErrors::N_ERRCATEGORY; ++errCate) {
      if (b_category[errCate] and regionIndex!=GENERAL_INDEX) {
        categoryErrorMap[errCate][regionIndex][layer][ieta][iphi] = true;
      }
    }
  }

  /// Fill /SCT/GENERAL/errors/Masked Links ///
  // TODO: reset the histogram every event
  for (int reg{0}; reg<N_REGIONS_INC_GENERAL; reg++) {
    auto maskedLinksBinAcc{Monitored::Scalar<int>("maskedLinksBin", reg)};
    auto maskedLinksAcc{Monitored::Scalar<int>("maskedLinks", nMaskedLinks[reg])};
    fill("SCTErrMonitor", maskedLinksBinAcc, maskedLinksAcc);
  }

  if (m_doPerLumiErrors) {
    for (int reg{0}; reg<N_REGIONS; reg++) {
      const int nLayers{n_layers[reg]*2};
      for (int layerSide{0}; layerSide<nLayers; layerSide++) {
        float num_modules{static_cast<float>(getNumModules(reg, layerSide))};
        /// Fill /run_x/lb_y/SCT/SCT*/RateErrorsPerLumi ///
        auto errorTypeAcc{Monitored::Scalar<int>("errorType", err_type)};
        auto layerSideAcc{Monitored::Scalar<int>("layerSide", layerSide)};
        auto errorFractionAcc{Monitored::Scalar<float>("errorFraction", static_cast<float>(numErrorsPerLumi[reg][layerSide])/num_modules)};
        auto isECAcc{Monitored::Scalar<bool>("isEC", reg==ENDCAP_C_INDEX)};
        auto isBAcc{Monitored::Scalar<bool>("isB", reg==BARREL_INDEX)};
        auto isEAAcc{Monitored::Scalar<bool>("isEA", reg==ENDCAP_A_INDEX)};
        fill("SCTErrMonitor", errorTypeAcc, layerSideAcc, errorFractionAcc, isECAcc, isBAcc, isEAAcc);
      }
    }
  }

  if (b_category[CategoryErrors::SUMMARY]) return nerrors;
  return 0;
}

void
SCTErrMonAlg::numByteStreamErrors(const set<IdentifierHash>& errors, int& ntot) const {
  for (const auto& fit: errors) {
    if (fit.is_valid()) {
      ntot++;
    }
  }
}

bool SCTErrMonAlg::disabledSCT(set<IdentifierHash>& sctHashDisabled) const {
  bool altered{false};
  sctHashDisabled.clear();
  const set<Identifier>* badModules{m_configurationTool->badModules()};

  for (const Identifier& badModule: *badModules) {
    altered = true;
    IdentifierHash hashSide0{m_pSCTHelper->wafer_hash(badModule)};
    IdentifierHash hashSide1;
    m_pSCTHelper->get_other_side(hashSide0, hashSide1);
    sctHashDisabled.insert(hashSide0);
    sctHashDisabled.insert(hashSide1);
  }
  return altered;
}

bool SCTErrMonAlg::errorSCT(set<IdentifierHash>& sctHashBadLinkError,
                            set<IdentifierHash>& sctHashBadRODError,
                            set<IdentifierHash>& sctHashBadError) const {
  sctHashBadLinkError.clear();
  sctHashBadRODError.clear();
  sctHashBadError.clear();
 
  //BadLinkLevelError
  for (SCT_ByteStreamErrors::errorTypes linkLevelBadErrors: SCT_ByteStreamErrors::LinkLevelBadErrors) {
    const set<IdentifierHash> sctErrors{m_byteStreamErrTool->getErrorSet( linkLevelBadErrors )};
    for (const IdentifierHash& waferHash : sctErrors) {
      sctHashBadLinkError.insert(waferHash);
    }
  }

  //BadRODLevelError
  for (SCT_ByteStreamErrors::errorTypes RodLevelBadErrors: SCT_ByteStreamErrors::RodLevelBadErrors) {
    const set<IdentifierHash> sctErrors{m_byteStreamErrTool->getErrorSet( RodLevelBadErrors )};
    for (const IdentifierHash& waferHash : sctErrors) {
      sctHashBadRODError.insert(waferHash);
    }
  }

  //BadError = BadLinkLevelError + BadRODLevelError
  for (SCT_ByteStreamErrors::errorTypes tmpBadError: SCT_ByteStreamErrors::BadErrors) {
    const set<IdentifierHash> sctErrors{m_byteStreamErrTool->getErrorSet( tmpBadError )};
    for (const IdentifierHash& waferHash : sctErrors) {
      sctHashBadError.insert(waferHash);
    }
  }
  return true;
}

// Total (SCT_ConditionsSummaryTool)
bool SCTErrMonAlg::summarySCT(set<IdentifierHash>& sctHashAll, set<IdentifierHash>& sctHashSummary) const  {
  bool altered{false};
  sctHashAll.clear();//All
  sctHashSummary.clear();

  const unsigned int maxHash{static_cast<unsigned int>(m_pSCTHelper->wafer_hash_max())}; // 8176
  for (unsigned int i{0}; i<maxHash; i++) {
    IdentifierHash hash{i};
    sctHashAll.insert(hash);//All
    if (not m_pSummaryTool->isGood(hash)) {
      sctHashSummary.insert(hash);
    }
  }
  return altered;
}

// Power supply trip (SCT_DCSConditionsTool)
bool SCTErrMonAlg::psTripDCSSCT(set<IdentifierHash>& sctHashPSTripDCS, float& psTripModules) const {
  bool altered{false};
  sctHashPSTripDCS.clear();

  const unsigned int maxHash{static_cast<unsigned int>(m_pSCTHelper->wafer_hash_max())}; // 8176
  int npsw{0};
  for (unsigned int i{0}; i<maxHash; i++) {
    IdentifierHash hash{i};
    if (m_useDCS and (not m_dcsTool->isGood(hash))) {
      npsw++; //Counting the number of PS sides
      altered = true;
      sctHashPSTripDCS.insert(hash);
    }
  }
  psTripModules = npsw/2.;
  return altered;
}

void SCTErrMonAlg::fillWafer(moduleGeo_t module, TH2F& histo) const {
  double etaMin{module.first.first}, etaMax{module.first.second};
  double phiMin{module.second.first}, phiMax{module.second.second};
  unsigned int nRep{1};
  if (etaMin<-s_rangeEta) { etaMin = -s_rangeEta; }
  if (etaMax> s_rangeEta) { etaMax =  s_rangeEta; }
  if (phiMin>phiMax) {
    phiMin = -M_PI;
    nRep=2;
  }
  for (unsigned int iRep{0}; iRep<nRep; iRep++) {
    if (iRep==1) {
      phiMin = module.second.first;
      phiMax = M_PI;
    }
    const int ixMin{static_cast<int>((etaMin/s_rangeEta+1.)*s_nBinsEta/2)+1};
    const int ixMax{static_cast<int>((etaMax/s_rangeEta+1.)*s_nBinsEta/2)};
    const int iyMin{static_cast<int>((phiMin/M_PI+1.)*s_nBinsPhi/2)+1};
    const int iyMax{static_cast<int>((phiMax/M_PI+1.)*s_nBinsPhi/2)};
    const double xMin{(static_cast<double>(ixMin)/s_nBinsEta*2-1.)*s_rangeEta};
    const double xMax{(static_cast<double>(ixMax)/s_nBinsEta*2-1.)*s_rangeEta};
    const double yMin{(static_cast<double>(iyMin)/s_nBinsPhi*2-1.)*M_PI};
    const double yMax{(static_cast<double>(iyMax)/s_nBinsPhi*2-1.)*M_PI};
    const double wxMin{(xMin-etaMin)/s_rangeEta*s_nBinsEta/2};
    const double wxMax{(etaMax-xMax)/s_rangeEta*s_nBinsEta/2};
    const double wxOne{(etaMax-etaMin)/s_rangeEta*s_nBinsEta/2};
    const double wyMin{(yMin-phiMin)/M_PI*s_nBinsPhi/2};
    const double wyMax{(phiMax-yMax)/M_PI*s_nBinsPhi/2};
    const double wyOne{(phiMax-phiMin)/M_PI*s_nBinsPhi/2};
    for (int ix{ixMin}; ix<=ixMax+1; ix++) {
      double weightx{1.};
      if (ixMin==ixMax+1) weightx = wxOne;
      else if (ix==ixMin) weightx = wxMin;
      else if (ix==ixMax+1) weightx = wxMax;
      for (int iy{iyMin}; iy<=iyMax+1; iy++) {
        double weight{weightx};
        if (iyMin==iyMax+1) weight *= wyOne;
        else if (iy==iyMin) weight *= wyMin;
        else if (iy==iyMax+1) weight *= wyMax;
        histo.SetBinContent(ix, iy, histo.GetBinContent(ix,iy)+weight); //Fill(ix, iy, weight);//
      }
    }
  }
  return;
}

double SCTErrMonAlg::calculateDetectorCoverage(const TH2F& histo, const TH2F& histoAll) const {
  double occupancy{0.};

  for (unsigned int i{0}; i < s_nBinsEta; i++) {
    for (unsigned int j{0}; j < s_nBinsPhi; j++) {
      double waferCell{histoAll.GetBinContent(i+1, j+1) - histo.GetBinContent(i+1, j+1)};

      if (waferCell >= s_wafersThreshold) {
        occupancy += 1.0;
      } else if (waferCell > s_wafersThreshold - 1.0 ) {
        occupancy += waferCell - (s_wafersThreshold - 1.0);
        //Calculating the bin occupancy which has less than 1. 
        //For example, bin have a 2.3. In this case, we can understand that 30% of the bin is coverd by 3 sides/wafers and 70% of the bin is coverd by 2 sides/wafers.
        //And it means that occupancy of the bin is 0.3 So, in this line, I take difference between s_wafersThreshold(3)-1 and waferCell, and add it to the occupancy.
      }
    }
  }
  double detector_coverage{100. * occupancy / ( static_cast<double>( s_nBinsEta ) * static_cast<double>( s_nBinsPhi ) )};
  return detector_coverage;
}

const unsigned int SCTErrMonAlg::s_nBinsEta = 100;
const double SCTErrMonAlg::s_rangeEta = 2.5;
const unsigned int SCTErrMonAlg::s_nBinsPhi = 100;
const double SCTErrMonAlg::s_wafersThreshold = 3.0;
