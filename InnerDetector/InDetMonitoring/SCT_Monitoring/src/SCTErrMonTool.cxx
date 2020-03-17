/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*    @file SCTErrMonTool.cxx
 *
 *    @author Martin White, based on code by Luca Fiorini, Shaun Roe, Manuel Diaz & Rob McPherson
 *    Major tidying/restructuring by Martin Flechl
 */

#include "SCTErrMonTool.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "StoreGate/ReadHandle.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TH1I.h"
#include "TH2F.h"
#include "TProfile.h"

#include <iostream>
#include <map>
#include <utility>

using namespace std;
using namespace SCT_Monitoring;

namespace {
  // anon. namespace for file-scoped functions
  // test offline the online code
  static const bool testOffline{false};

  int
  numberOfInefficientSides(TH2* pHistogramArray[], const int xbin, const int ybin, const float threshold) {
    double histogramBinContent0{pHistogramArray[0]->GetBinContent(xbin, ybin)};
    double histogramBinContent1{pHistogramArray[1]->GetBinContent(xbin, ybin)};
    int side0Inefficient{histogramBinContent0 > threshold};
    int side1Inefficient{histogramBinContent1 > threshold};

    return side0Inefficient + side1Inefficient;
  }

  bool
  eitherSideIsOverThreshold(TH2* pHistogramArray[], const int xbin, const int ybin, const float threshold) {
    double histogramBinContent0{pHistogramArray[0]->GetBinContent(xbin, ybin)};
    double histogramBinContent1{pHistogramArray[1]->GetBinContent(xbin, ybin)};

    return ((histogramBinContent0 > threshold) or (histogramBinContent1 > threshold));
  }

  bool
  thereAreEnoughEntries(TH2* pHistogramArray[], const float threshold) {
    double histogramEntries0{pHistogramArray[0]->GetEntries()};
    double histogramEntries1{pHistogramArray[1]->GetEntries()};

    return ((histogramEntries0 > threshold) and (histogramEntries1 > threshold));
  }

  void
  fillRateHistogram(const TH2F_LW* sourceHisto, TProfile2D_LW* targetHisto, const int xbin, const int ybin,
                    const float centreX, const float centreY, const int nEvent) {
    const double content{sourceHisto->GetBinContent(xbin, ybin)};

    targetHisto->Fill(centreX, centreY, 1, content);
    targetHisto->Fill(centreX, centreY, 0, nEvent - content);
  }

  void
  countNoisyModules(const int regionIndex, const int generalIndex, TH2* pHistogram[],
                    const float threshold, int countArray[]) {
    const int xbins{pHistogram[0]->GetNbinsX() + 1};
    const int ybins{pHistogram[0]->GetNbinsY() + 1};

    for (int xb{1}; xb < xbins; ++xb) {
      for (int yb{1}; yb < ybins; ++yb) {
        if (eitherSideIsOverThreshold(pHistogram, xb, yb, threshold)) {
          countArray[generalIndex]++;
          countArray[regionIndex]++;
        }
      }
    }
  }
}


// ====================================================================================================
/** Constructor, calls base class constructor with parameters
 *
 *  several properties are "declared" here, allowing selection of the filepath for histograms,
 *  the first and second plane numbers to be used, and the timebin.
 */
// ====================================================================================================
SCTErrMonTool::SCTErrMonTool(const string& type, const string& name, const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent) {
}

// ====================================================================================================
//====================================================================================================
StatusCode SCTErrMonTool::initialize() {
  ATH_CHECK(detStore()->retrieve(m_pSCTHelper, "SCT_ID"));

  ATH_CHECK(m_dataObjectName.initialize());
  ATH_CHECK(m_eventInfoKey.initialize());

  moduleGeo_t moduleGeo; // dummy value
  const unsigned int maxHash{static_cast<unsigned int>(m_pSCTHelper->wafer_hash_max())}; // 8176
  m_geo.resize(maxHash, moduleGeo);
  
  double rz{0.};
  const double deltaZ{0.};

  const InDetDD::SCT_DetectorManager* sctManager{nullptr};
  ATH_CHECK(detStore()->retrieve(sctManager, "SCT"));

  for (unsigned int i{0}; i<maxHash; i++) {
    IdentifierHash hash{i};
    const InDetDD::SiDetectorElement* newElement{sctManager->getDetectorElement(hash)};
    newElement->getEtaPhiRegion(deltaZ,
                                moduleGeo.first.first,  moduleGeo.first.second,
                                moduleGeo.second.first, moduleGeo.second.second,
                                rz);
    m_geo[i] = moduleGeo;
  }
  ////
  return ManagedMonitorToolBase::initialize();
}

//====================================================================================================
// ====================================================================================================
SCTErrMonTool::~SCTErrMonTool() {

  for (int reg{0}; reg < NREGIONS_INC_GENERAL; ++reg) {
    for (int layer{0}; layer < N_ENDCAPSx2; ++layer) {
      for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
        if (m_allErrsPerLumi[errType][reg][layer]) LWHist::safeDelete(m_allErrsPerLumi[errType][reg][layer]);
        if (m_pallErrsPerLumi[errType][reg][layer]) LWHist::safeDelete(m_pallErrsPerLumi[errType][reg][layer]);
      }
    }
  }
}

// ====================================================================================================
//             SCTErrMonTool :: transfer [enum CategoryErrors] -> [TString ErrorName]
// ====================================================================================================
TString
SCTErrMonTool::errorsString(int errtype) const {
  if (errtype == MASKEDLINKALL) {
    return "MaskedLinkALL";
  }
  if (errtype == SUMMARY) {
    return "Errors";
  }
  if (errtype == BADERR) {
    return "BadErrors";
  }
  if (errtype == LINKLEVEL) {
    return "LinkLevelErrors";
  }
  if (errtype == RODLEVEL) {
    return "RODLevelErrors";
  }
  if (errtype == MASKEDCHIP) {
    return "MaskedChipALL";
  }
  return "";
}

// ====================================================================================================
//                            SCTErrMonTool :: copyHistograms
//    This function is used for copying histograms which is required to change their directories/names.
//    If you change original ones immediately, you cannot see them in the web display for a while
// until new configuration is applied.
//    So you should copy histograms in the new directories in this function once,
// and then if you have new configuration for the webdisplay, you can delete this function and change original histograms.
// ====================================================================================================
StatusCode
SCTErrMonTool::copyHistograms() {

  //RODLevelErrors histograms
  for (int reg{0}; reg < NREGIONS_INC_GENERAL; ++reg) {
    const unsigned int xbins{m_LinksWithCategorisedErrorsVsLB[CategoryErrors::RODLEVEL][reg]->GetNbinsX() + 1};
    for (unsigned int xb{1}; xb < xbins; ++xb) {
      double nentry{m_LinksWithCategorisedErrorsVsLB[CategoryErrors::RODLEVEL][reg]->GetBinContent(xb)};
      for (int ientry{0}; ientry<nentry; ientry++) {
        m_LinksWithRODErrorsVsLB_check[reg]->Fill(m_LinksWithCategorisedErrorsVsLB[CategoryErrors::RODLEVEL][reg]->GetXaxis()->GetBinCenter(xb),
                                                  m_LinksWithCategorisedErrorsVsLB[CategoryErrors::RODLEVEL][reg]->GetBinContent(xb));
      }
    }
  }

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                            SCTErrMonTool :: copyHistograms
//    This function is used for copying histograms which is required to change their directories/names.
//    If you change original ones immediately, you cannot see them in the web display for a while
// until new configuration is applied.
//    So you should copy histograms in the new directories in this function once,
// and then if you have new configuration for the webdisplay, you can delete this function and change original histograms.
// ====================================================================================================
StatusCode
SCTErrMonTool::copyHistograms_book() {
  //RODLevelErrors histograms
  static const TString regLabel[NREGIONS_INC_GENERAL] = {
    "EndcapC", "Barrel", "EndcapA", ""
  };
  static const TString regTitle[NREGIONS_INC_GENERAL] = {
    "EndcapC", "Barrel", "EndcapA", "All Region"
  };

  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup ConfHist[NREGIONS_INC_GENERAL] = {
      MonGroup{this, "SCT/SCTEC/Conf",   ManagedMonitorToolBase::run, ATTRIB_UNMANAGED},
      MonGroup{this, "SCT/SCTB/Conf",    ManagedMonitorToolBase::run, ATTRIB_UNMANAGED},
      MonGroup{this, "SCT/SCTEA/Conf",   ManagedMonitorToolBase::run, ATTRIB_UNMANAGED},
      MonGroup{this, "SCT/GENERAL/Conf", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED}
    };

    for (int reg{0}; reg < NREGIONS_INC_GENERAL; ++reg) {
      m_LinksWithRODErrorsVsLB_check[reg] =
        TProfile_LW::create("SCTModulesWithRODLevelErrorsCheck" + regLabel[reg],
                            "Ave. Number of RODLevelErrors per LB in " + regTitle[reg],
                            NBINS_LBs, 0.5, NBINS_LBs+0.5);
      if (ConfHist[reg].regHist(m_LinksWithRODErrorsVsLB_check[reg]).isFailure()) {
        ATH_MSG_WARNING("Cannot book Histogram:SCTModulesWithRODLevelErrorsCheck");
      }
    }
  }

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                            SCTErrMonTool :: bookHistograms
// ====================================================================================================
StatusCode
SCTErrMonTool::bookHistograms() {

  ATH_MSG_DEBUG(" initialize being called ");
  if (newRunFlag()) {
    m_numberOfEvents = 0;
    if (AthenaMonManager::dataType() == AthenaMonManager::cosmics) {
      m_checkrate = 100;
    }
  }
  if (ManagedMonitorToolBase::newLumiBlockFlag()) {
    m_numberOfEventsLumi = 0;
  }

  // Services for Summary Histograms: SCT_ModuleConditionsTool from CondDB
  ATH_MSG_INFO("Checking for CondDB");
  ATH_CHECK(m_ConfigurationTool.retrieve());

  // get a handle on the histogramming service //
  ATH_CHECK(m_thistSvc.retrieve());

  /// SUMMARY HISTOGRAM
  if (bookConfMapsGen().isFailure()) {
    ATH_MSG_WARNING("Error in bookConfMapsGen()");
  }
  if (bookErrHistosGen().isFailure()) {
    ATH_MSG_WARNING("Error in bookErrHistosGen()");
  }

  for (int reg{0}; reg<N_REGIONS; reg++) {
    if (bookConfMaps(reg).isFailure()) {
      ATH_MSG_WARNING("Error in bookConfMaps(): " << "SCT" + subDetNameShort[reg]);
    }
    if (bookErrHistos(reg).isFailure()) {
      ATH_MSG_WARNING("Error in bookErrHistos(): " << "SCT" + subDetNameShort[reg]);
    }
  }

  if (copyHistograms_book().isFailure()) {
    ATH_MSG_WARNING("Error in copyHistograms_book() ");
  }

  ATH_CHECK(m_byteStreamErrTool.retrieve());
  if (m_useDCS) ATH_CHECK(m_dcsTool.retrieve());
  else m_dcsTool.disable();
  ATH_CHECK(m_pSummaryTool.retrieve());
  m_initialize = true;
  return StatusCode::SUCCESS;
}

//====================================================================================================
//          SCTErrMonTool :: bookHistogramsRecurrent, Keisuke Koda 12.09.2016
//====================================================================================================
StatusCode SCTErrMonTool::bookHistogramsRecurrent() {
  static const string profNames[numberOfProblemForCoverage] = {
    "", // All
    "SCT_CoverageOfEnabledLinksVsLbs", // All - Disabled
    "SCT_CoverageWithNoBadLinkLevelErrorVsLbs", // All - BadLinkLevelError
    "SCT_CoverageWithNoBadRODLevelErrorVsLbs", // All - BadRODLevelError
    "SCT_CoverageWithNoBadErrorVsLbs", // All - BadError
    "SCT_CoverageWithNoPSTripVsLbs", // All - PSTrip(DCS)
    "SCT_CoverageOfLinksWithNoBadProblemVsLbs" // All - Summary
  };
  static const string profTitles[numberOfProblemForCoverage] = {
    "", // All
    "Ave. Coverage of Enabled Links per LB", // All - Disabled
    "Ave. Coverage of Links with No Bad LinkLevelError per LB", // All - BadLinkLevelError
    "Ave. Coverage of Links with No Bad RODLevelError per LB", // All - BadRODLevelError
    "Ave. Coverage of Links with No Bad Error per LB", // All - BadError
    "Ave. Coverage of links Not Affected by PS Trip", // All - PSTrip(DCS)
    "Ave. Coverage of Links With No Bad Problem per LB" // All - Summary
  };

  MonGroup monGr_shift{this, "SCT/DetectorCoverage", run, ATTRIB_UNMANAGED};

  bool status{true};
  if ( not m_CoverageCheck ) {
    return StatusCode::SUCCESS;
  }

  if (ManagedMonitorToolBase::newRunFlag()) {

    //All SCT module for counting good module
    m_mapSCT[all] = new TH2F( "SCT_AllRegion", "Map of All Region",
                              m_nBinsEta, -m_rangeEta, m_rangeEta, m_nBinsPhi, -M_PI, M_PI );
    //Disabled
    m_mapSCT[disabled] = new TH2F( "SCT_MapOfDisabledLinks", "Map of Disabled Links",
                                   m_nBinsEta, -m_rangeEta, m_rangeEta, m_nBinsPhi, -M_PI, M_PI );
    //BadLinkLevelError
    m_mapSCT[badLinkError] = new TH2F( "SCT_MapOfLinksWithBadLinkLevelErrors", "Map of Links with bad LinkLevelErrors",
                                       m_nBinsEta, -m_rangeEta, m_rangeEta, m_nBinsPhi, -M_PI, M_PI );
    //BadRODLevelError
    m_mapSCT[badRODError] = new TH2F( "SCT_MapOfLinksWithBadRODLevelErrors", "Map of Links with Bad RODLevelErrors",
                                      m_nBinsEta, -m_rangeEta, m_rangeEta, m_nBinsPhi, -M_PI, M_PI );
    //BadError = BadLinkLevelError + BadRODLevelError
    m_mapSCT[badError] = new TH2F( "SCT_MapOfLinksWithBadErrors", "Map of Links with Bad Errors",
                                   m_nBinsEta, -m_rangeEta, m_rangeEta, m_nBinsPhi, -M_PI, M_PI );
    //Power supply trip (SCT_DCSConditionsTool)
    m_mapSCT[psTripDCS] = new TH2F( "SCT_MapOfLinksWithPSTrip", "Map of Links Affected by PS Trip",
                                    m_nBinsEta, -m_rangeEta, m_rangeEta, m_nBinsPhi, -M_PI, M_PI );
    //Total (SCT_ConditionsSummaryTool)
    m_mapSCT[summary] = new TH2F( "SCT_MapOfLinksWithAnyProbelm", "Map of Links with Any Bad Problem",
                                  m_nBinsEta, -m_rangeEta, m_rangeEta, m_nBinsPhi, -M_PI, M_PI );
    
    //Detector Coverage vs LumiBlock
    for (int iProblem{0}; iProblem<numberOfProblemForCoverage ; iProblem++) {
      m_mapSCT[iProblem]->GetXaxis()->SetTitle("#eta");
      m_mapSCT[iProblem]->GetYaxis()->SetTitle("#phi");
      status &= monGr_shift.regHist( m_mapSCT[iProblem] ).isSuccess();
      m_mapSCT[iProblem]->SetStats(0);
    }

    for (int iProblem{0}; iProblem<numberOfProblemForCoverage; iProblem++) {
      if (iProblem==all) continue;

      m_detectorCoverageVsLbs[iProblem] = new TProfile(profNames[iProblem].c_str(), profTitles[iProblem].c_str(), NBINS_LBs,0.5,NBINS_LBs+0.5);
      m_detectorCoverageVsLbs[iProblem]->GetXaxis()->SetTitle("LumiBlock");
      m_detectorCoverageVsLbs[iProblem]->GetYaxis()->SetTitle("Detector Coverage [%]");
      if (monGr_shift.regHist(m_detectorCoverageVsLbs[iProblem]).isFailure()) {
        ATH_MSG_WARNING("Cannot book " << profNames[iProblem] << ", " << profTitles[iProblem]);
      }
    }

    //Number of Modules affected by PS Trip vs LumiBlock
    m_PSTripModulesVsLbs = new TProfile("SCT_ModulesWithPSTripVsLbs",
                                        "Ave. Num of Modules Affected by PS Trip per LB in All Region",
                                        NBINS_LBs,0.5,NBINS_LBs+0.5);
    m_PSTripModulesVsLbs->GetXaxis()->SetTitle("LumiBlock");
    m_PSTripModulesVsLbs->GetYaxis()->SetTitle("Num. of Modules Affected by PS Trip");
    if ( monGr_shift.regHist(m_PSTripModulesVsLbs).isFailure() ) {
      ATH_MSG_WARNING("Cannot book Histogram:SCTpstripWafer" );
    }
  }

  return ( status ) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

// ====================================================================================================
//                            SCTErrMonTool :: fillHistograms
//  This is the real workhorse, called for each event. It retrieves the data each time
// ====================================================================================================
StatusCode
SCTErrMonTool::fillHistograms() {

  SG::ReadHandle<xAOD::EventInfo> pEvent{m_eventInfoKey};
  if (not pEvent.isValid()) {
    ATH_MSG_WARNING("Could not retrieve event info!");
    return StatusCode::RECOVERABLE;
  }
  m_current_lb = pEvent->lumiBlock();
  bool sctflag{false};
  if (pEvent->errorState(xAOD::EventInfo::SCT) == xAOD::EventInfo::Error) {
    // ATH_MSG_WARNING("SCT_Flag==FALSE:: LVL1ID Errors >500 ");
    m_NumberOfSCTFlagErrorsVsLB->Fill(m_current_lb);
    m_FractionOfSCTFlagErrorsPerLB->Fill(m_current_lb, 1);
    sctflag = true;
  } else {
    m_FractionOfSCTFlagErrorsPerLB->Fill(m_current_lb, 0);
  }
  m_NumberOfEventsVsLB->Fill(m_current_lb);

  ATH_CHECK(fillByteStreamErrors());

  if (sctflag) {
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<SCT_RDO_Container> p_rdocontainer{m_dataObjectName};

  // Define variables for error histograms
  int numFirstHit{0}, numSecondHit{0}, numSCTRDOs{0};
  int numFirstHit_ECp{0}, numSecondHit_ECp{0}, numSCTRDOs_ECp{0};
  int numFirstHit_ECm{0}, numSecondHit_ECm{0}, numSCTRDOs_ECm{0};

  int count_SCT_RDO{0};
  for (const InDetRawDataCollection<SCT_RDORawData>*SCT_Collection: *p_rdocontainer) {
    if (SCT_Collection==nullptr) continue;  // select only SCT RDOs

    for (const SCT_RDORawData* rdo: *SCT_Collection) {
      count_SCT_RDO++;
      const SCT3_RawData* rdo3{dynamic_cast<const SCT3_RawData*>(rdo)};
      if (rdo3==nullptr) continue;

      Identifier SCT_Identifier{rdo3->identify()};
      int barrel_ec{m_pSCTHelper->barrel_ec(SCT_Identifier)};
      if (barrel_ec == BARREL) ++numSCTRDOs;
      if ((barrel_ec == ENDCAP_A) and m_doPositiveEndcap) ++numSCTRDOs_ECp;
      if ((barrel_ec == ENDCAP_C) and m_doNegativeEndcap) ++numSCTRDOs_ECm;

      if (not SCT_Identifier.is_valid()) {
        ATH_MSG_ERROR("The SCT identifier is not valid.");
        return StatusCode::RECOVERABLE;
      }

      // Start filling histograms, whilst counting the number of each type of errors. A map of vectors contains
      // histograms for each type of error. The vectors contain one histogram for each plane of the SCT
      if (rdo3) {
        if (rdo3->FirstHitError()) {
          if (barrel_ec == BARREL) numFirstHit++;
          else if ((barrel_ec == ENDCAP_A) and m_doPositiveEndcap) numFirstHit_ECp++;
          else if ((barrel_ec == ENDCAP_C) and m_doNegativeEndcap) numFirstHit_ECm++;
        }
        if ((rdo3)->SecondHitError()) {
          if (barrel_ec == BARREL) numSecondHit++;
          else if ((barrel_ec == ENDCAP_A) and m_doPositiveEndcap) numSecondHit_ECp++;
          else if ((barrel_ec == ENDCAP_C) and m_doNegativeEndcap) numSecondHit_ECm++;
        }
      }
    }
  }// end for (SCT_RDO_Container...

  if (numSCTRDOs > 0) {
    double scale{100. / static_cast<double>(numSCTRDOs)};
    m_firstHit[BARREL_INDEX]->Fill(static_cast<double>(numFirstHit)*scale, 1.);
    m_secondHit[BARREL_INDEX]->Fill(static_cast<double>(numSecondHit)*scale, 1.);
  }
  if ((numSCTRDOs_ECp > 0) and m_doPositiveEndcap) {
    double scale{100. / static_cast<double>(numSCTRDOs_ECp)};
    m_firstHit[ENDCAP_A_INDEX]->Fill(static_cast<double>(numFirstHit_ECp)*scale, 1.);
    m_secondHit[ENDCAP_A_INDEX]->Fill(static_cast<double>(numSecondHit_ECp)*scale, 1.);
  }
  if ((numSCTRDOs_ECm > 0) and m_doNegativeEndcap) {
    double scale{100. / static_cast<double>(numSCTRDOs_ECm)};
    m_firstHit[ENDCAP_C_INDEX]->Fill(static_cast<double>(numFirstHit_ECm)*scale, 1.);
    m_secondHit[ENDCAP_C_INDEX]->Fill(static_cast<double>(numSecondHit_ECm)*scale, 1.);
  }
  if (m_environment == AthenaMonManager::online) {
    if ((m_numberOfEvents == 1) or ((m_numberOfEvents > 1) and (m_numberOfEvents % m_checkrate) == 0)) {
      if (resetCondDBMaps().isFailure()) {
        ATH_MSG_WARNING("Error in resetCondDBMaps()");
      }
      if (fillCondDBMaps().isFailure()) {
        ATH_MSG_WARNING("Error in fillCondDBMaps()");
      }
      if ((m_numberOfEvents % (m_checkrate * 10)) == 0) {
        if (resetConfigurationDetails().isFailure()) {
          ATH_MSG_WARNING("Error in resetConfigurationDetails()");
        }
        if (fillConfigurationDetails().isFailure()) {
          ATH_MSG_WARNING("Error in fillConfigurationDetails()");
        }
      }
    }
  }
  if (m_environment == AthenaMonManager::online) {
    if ((m_current_lb % m_checkrecent == 0) and (m_current_lb > m_last_reset_lb)) {
      m_last_reset_lb = m_current_lb;
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                           SCTErrMonTool :: checkRateHists
// DD 17/08/2010
// Here we have the check of the histograms and we can build rate
// error rate histograms from the basic ones
// ====================================================================================================
StatusCode
SCTErrMonTool::checkRateHists() {
  /** TO BE REPLACED WHEN NEW VERSION OF BASE CLASS IS AVAILABLE: **/
  bool isEndOfEventsBlock{endOfLumiBlockFlag()};

  if (m_environment != AthenaMonManager::online) {
    if (m_initialize) {
      float content{0.};
      float cxb{0.};
      float cyb{0.};
      int evt{m_numberOfEvents};
      int evt_lumi{m_numberOfEventsLumi};
      for (int reg{0}; reg<N_REGIONS; reg++) {
        if (m_doPerLumiErrors and endOfLumiBlockFlag()) {
          const unsigned int xbins{m_numErrorsPerLumi[reg]->GetNbinsX() + 1};
          const unsigned int ybins{m_numErrorsPerLumi[reg]->GetNbinsY() + 1};
          for (unsigned int xb{1}; xb < xbins; ++xb) {
            cxb = m_numErrorsPerLumi[reg]->GetXaxis()->GetBinCenter(xb);
            for (unsigned int yb{1}; yb < ybins; ++yb) {
              cyb = m_numErrorsPerLumi[reg]->GetYaxis()->GetBinCenter(yb);
              int num_modules{getNumModules(reg, yb - 1)};
              content = m_numErrorsPerLumi[reg]->GetBinContent(xb, yb);
              if (num_modules > 0) {
                m_rateErrorsPerLumi[reg]->Fill(cxb, cyb, 1, content);
                m_rateErrorsPerLumi[reg]->Fill(cxb, cyb, 0, (evt_lumi * (static_cast<double>(num_modules)) - content));
              }
            }
          }
        }
        int nLayers{n_layers[reg]*2};
        for (int lyr{0}; lyr < nLayers; ++lyr) {
          const unsigned int xbins{m_pallErrs[0][reg][lyr]->GetNbinsX() + 1};
          const unsigned int ybins{m_pallErrs[0][reg][lyr]->GetNbinsY() + 1};
          for (unsigned int xb{1}; xb < xbins; ++xb) {
            cxb = m_pallErrs[0][reg][lyr]->GetXaxis()->GetBinCenter(xb);
            for (unsigned int yb{1}; yb < ybins; ++yb) {
              cyb = m_pallErrs[0][reg][lyr]->GetYaxis()->GetBinCenter(yb);
              if (isEndOfEventsBlock) {
                for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
                  fillRateHistogram(m_pallErrs[errType][reg][lyr], m_allErrs[errType][reg][lyr], xb, yb, cxb, cyb, evt);
                }
              }
              // per lumi hists
              if (m_doPerLumiErrors and endOfLumiBlockFlag() and m_doErr2DPerLumiHists) {
                for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
                  fillRateHistogram(m_pallErrsPerLumi[errType][reg][lyr], m_allErrsPerLumi[errType][reg][lyr], xb, yb, cxb, cyb, evt);
                }
              }
            }
          }
        }
      }

      if (isEndOfEventsBlock) {
        if (resetCondDBMaps().isFailure()) {
          ATH_MSG_WARNING("Error in resetCondDBMaps()");
        }
        if (fillCondDBMaps().isFailure()) {
          ATH_MSG_WARNING("Error in fillCondDBMaps()");
        }
        if (resetConfigurationDetails().isFailure()) {
          ATH_MSG_WARNING("Error in resetConfigurationDetails()");
        }
        if (fillConfigurationDetails().isFailure()) {
          ATH_MSG_WARNING("Error in fillConfigurationDetails()");
        }
      }
    } // mf moved brackets to allow online compilation
  }

  ATH_MSG_INFO("SCTErrMonTool, package version " << PACKAGE_VERSION);

  return StatusCode::SUCCESS;
}

//====================================================================================================
//                    SCTErrMonTool :: procHistograms
// ====================================================================================================
StatusCode
SCTErrMonTool::procHistograms() {
  /** TO BE REPLACED **/
  bool endOfEventsBlock{endOfLumiBlockFlag()};

  if (endOfEventsBlock or endOfLumiBlockFlag()) {
    ATH_MSG_DEBUG("finalHists()");
    ATH_MSG_DEBUG("Total Rec Event Number: " << m_numberOfEvents);
    ATH_MSG_DEBUG("Calling checkHists(true); true := end of run");
    if (checkRateHists().isFailure()) {
      ATH_MSG_WARNING("Error in checkRateHists()");
    }
    if (endOfEventsBlock and copyHistograms().isFailure()) {
      ATH_MSG_WARNING("Error in copyHistograms()");
    }
    ATH_MSG_DEBUG("Exiting finalHists");
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//          SCTErrMonTool :: fillByteStreamErrorsHelper, Martin Flechl 10/09/2009
// ====================================================================================================
int
SCTErrMonTool::fillByteStreamErrorsHelper(const set<IdentifierHash>& errors,
                                          TH2F_LW* histo[SCT_ByteStreamErrors::NUM_ERROR_TYPES][NREGIONS_INC_GENERAL][N_ENDCAPSx2],
                                          bool lumi2DHist, int err_type) {

  //--- Check categories of the BS error
  bool b_category[CategoryErrors::N_ERRCATEGORY];

  b_category[CategoryErrors::MASKEDLINKALL] = false;
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

  b_category[CategoryErrors::MASKEDCHIP] = false;
  b_category[CategoryErrors::MASKEDCHIP] =
    (err_type == SCT_ByteStreamErrors::TempMaskedChip0) or (err_type == SCT_ByteStreamErrors::TempMaskedChip1) or
    (err_type == SCT_ByteStreamErrors::TempMaskedChip2) or (err_type == SCT_ByteStreamErrors::TempMaskedChip3) or
    (err_type == SCT_ByteStreamErrors::TempMaskedChip4) or (err_type == SCT_ByteStreamErrors::TempMaskedChip5);

  //--- Count BS errors
  int nerrors{0};
  for (const auto& hash: errors) {
    nerrors++;
    if (not hash.is_valid()) continue;

    //--- FIll module information with BS error
    Identifier fitId{m_pSCTHelper->wafer_id(hash)};
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

    //--- Fill BS errors to the map
    histo[err_type][regionIndex][layer]->Fill(ieta, iphi);

    //--- Fill BS errors to the map for each categories
    if (b_category[CategoryErrors::MASKEDLINKALL]) m_MaskedAllLinks->Fill(GENERAL_INDEX);
    if (b_category[CategoryErrors::MASKEDLINKALL] and (regionIndex!=GENERAL_INDEX)) m_MaskedAllLinks->Fill(regionIndex);

    if (lumi2DHist) {
      if (m_doPerLumiErrors and m_doErr2DPerLumiHists) {
        if (b_category[CategoryErrors::MASKEDLINKALL]) m_pallErrsCatePerLumi[CategoryErrors::MASKEDLINKALL][regionIndex][layer]->Fill(ieta, iphi);
        if (b_category[CategoryErrors::SUMMARY])       m_pallErrsCatePerLumi[CategoryErrors::SUMMARY][regionIndex][layer]->Fill(ieta, iphi);
        if (b_category[CategoryErrors::BADERR])        m_pallErrsCatePerLumi[CategoryErrors::BADERR][regionIndex][layer]->Fill(ieta, iphi);
        if (b_category[CategoryErrors::LINKLEVEL])     m_pallErrsCatePerLumi[CategoryErrors::LINKLEVEL][regionIndex][layer]->Fill(ieta, iphi);
        if (b_category[CategoryErrors::RODLEVEL])      m_pallErrsCatePerLumi[CategoryErrors::RODLEVEL][regionIndex][layer]->Fill(ieta, iphi);
        if (b_category[CategoryErrors::MASKEDCHIP])    m_pallErrsCatePerLumi[CategoryErrors::MASKEDCHIP][regionIndex][layer]->Fill(ieta, iphi);
      }
    } else {
      if (m_doPerLumiErrors) m_numErrorsPerLumi[regionIndex]->Fill(err_type, layer);
      if (b_category[CategoryErrors::MASKEDLINKALL]) m_pallErrsCate[CategoryErrors::MASKEDLINKALL][regionIndex][layer]->Fill(ieta, iphi);
      if (b_category[CategoryErrors::SUMMARY])       m_pallErrsCate[CategoryErrors::SUMMARY][regionIndex][layer]->Fill(ieta, iphi);
      if (b_category[CategoryErrors::BADERR])        m_pallErrsCate[CategoryErrors::BADERR][regionIndex][layer]->Fill(ieta, iphi);
      if (b_category[CategoryErrors::LINKLEVEL])     m_pallErrsCate[CategoryErrors::LINKLEVEL][regionIndex][layer]->Fill(ieta, iphi);
      if (b_category[CategoryErrors::RODLEVEL])      m_pallErrsCate[CategoryErrors::RODLEVEL][regionIndex][layer]->Fill(ieta, iphi);
      if (b_category[CategoryErrors::MASKEDCHIP])    m_pallErrsCate[CategoryErrors::MASKEDCHIP][regionIndex][layer]->Fill(ieta, iphi);
    }
  }

  if (b_category[CategoryErrors::SUMMARY]) return nerrors;
  return 0;
}

// ====================================================================================================
//          SCTErrMonTool :: numByteStreamErrors, Daniel Damiani 04/07/2011
// ====================================================================================================
void
SCTErrMonTool::numByteStreamErrors(const set<IdentifierHash>& errors, int& ntot, int& nbar, int& neca, int& necc) const {

  for (const auto& fit: errors) {
    if (fit.is_valid()) {
      Identifier fitId{m_pSCTHelper->wafer_id(fit)};
      int layer{m_pSCTHelper->layer_disk(fitId)};
      int side{m_pSCTHelper->side(fitId)};
      int barrel_ec{m_pSCTHelper->barrel_ec(fitId)};
      layer = layer * 2 + side;
      ntot++;
      if ((barrel_ec == BARREL) and (layer >= 0) and (layer < N_BARRELSx2)) nbar++;
      else if (barrel_ec == ENDCAP_A) neca++;
      else if (barrel_ec == ENDCAP_C) necc++;
    }
  }

}

// ====================================================================================================
//          SCTErrMonTool :: fillByteStreamErrors
// ====================================================================================================
StatusCode
SCTErrMonTool::fillByteStreamErrors() {

  //--- Get event information
  SG::ReadHandle<xAOD::EventInfo> pEvent{m_eventInfoKey};
  ATH_MSG_INFO("INFO|| monitor ||INFO");
  if (not pEvent.isValid()) {
    ATH_MSG_ERROR("Could not retrieve event info!");
    return StatusCode::RECOVERABLE;
  }
  unsigned int current_lb{pEvent->lumiBlock()};

  bool sctflag{false};
  if (pEvent->errorState(xAOD::EventInfo::SCT) == xAOD::EventInfo::Error) {
    sctflag = true;
  }

  //--- Fill 1D histograms (vs LumiBlock) for each BS
  for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
    int bs_errs[NREGIONS_INC_GENERAL];
    bs_errs[GENERAL_INDEX] = 0; // ALL
    bs_errs[BARREL_INDEX] = 0; // BARREL
    bs_errs[ENDCAP_A_INDEX] = 0; // ECA
    bs_errs[ENDCAP_C_INDEX] = 0; // ECC
    // get number of BS errors
    numByteStreamErrors(m_byteStreamErrTool->getErrorSet(errType),
                        bs_errs[GENERAL_INDEX], bs_errs[BARREL_INDEX], bs_errs[ENDCAP_A_INDEX], bs_errs[ENDCAP_C_INDEX]);
    // fill number of BS errors vs LBs
    for (int reg{0}; reg < NREGIONS_INC_GENERAL; ++reg) {
      if (not sctflag) m_ByteStreamVsLB[errType][reg]->Fill(current_lb, static_cast<double>(bs_errs[reg]));
      else m_ByteStreamWithSctFlagVsLB[errType][reg]->Fill(current_lb, static_cast<double>(bs_errs[reg]));
    }
  }

  if (sctflag) {
    return StatusCode::SUCCESS;
  }

  //--- Reset Histograms
  m_MaskedAllLinks->Reset();

  int bytestreamCate_errs[CategoryErrors::N_ERRCATEGORY][NREGIONS_INC_GENERAL];
  int tot_mod_bytestreamCate_errs[CategoryErrors::N_ERRCATEGORY][NREGIONS_INC_GENERAL];
  int tot_mod_bytestreamCate_errs_layer[CategoryErrors::N_ERRCATEGORY][NREGIONS_INC_GENERAL][N_DISKSx2];

  for (int reg{0}; reg < NREGIONS_INC_GENERAL; ++reg) {
    int nLayers{n_layers[reg]*2};
    for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
      bytestreamCate_errs[errType][reg] = 0;
      tot_mod_bytestreamCate_errs[errType][reg] = 0;
      for (int lyr{0}; lyr < nLayers; ++lyr) {
        if (reg==NREGIONS_INC_GENERAL-1)continue;
        tot_mod_bytestreamCate_errs_layer[errType][reg][lyr] = 0;
        if (m_pallErrsCate[errType][reg][lyr]) m_pallErrsCate[errType][reg][lyr]->Reset();
        if (m_doPerLumiErrors and m_doErr2DPerLumiHists) {
          if (m_pallErrsCatePerLumi[errType][reg][lyr]) m_pallErrsCatePerLumi[errType][reg][lyr]->Reset();
        }
        if ((m_environment == AthenaMonManager::online) and
            (m_numberOfEvents % m_checkrate == 0) and m_runOnline) {
          m_allErrs[errType][reg][lyr]->Reset();
        }
      }
    }
    for (int lyr{0}; lyr < nLayers; ++lyr) {
      if ((m_environment == AthenaMonManager::online) and 
          (abs(reg)<NREGIONS_INC_GENERAL) and
          (m_current_lb % m_checkrecent == 0) and
          (m_current_lb > m_last_reset_lb) and
          m_summaryErrsRecent[reg][lyr]) {
        m_summaryErrsRecent[reg][lyr]->Reset();
      }
    }
  }

  //--- Fill map histograms for each BS
  int total_errors = 0;
  for (int errType = 0; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
    total_errors += fillByteStreamErrorsHelper(m_byteStreamErrTool->getErrorSet(errType), m_pallErrs, false, errType);
    if (m_doPerLumiErrors and m_doErr2DPerLumiHists) {
      fillByteStreamErrorsHelper(m_byteStreamErrTool->getErrorSet(errType), m_pallErrsPerLumi, false, errType);
    }
  }

  //--- Fill detector coverage histograms
  if ( m_CoverageCheck ) {
    ATH_MSG_INFO("Detector Coverage calculation starts" );

    for (int iProblem{0}; iProblem<numberOfProblemForCoverage; iProblem++) {
      m_mapSCT[iProblem]->Reset("ICE");
    }

    syncDisabledSCT(m_SCTHash[disabled]);
    syncErrorSCT(m_SCTHash[badLinkError], m_SCTHash[badRODError], m_SCTHash[badError]);
    summarySCT(m_SCTHash[all], m_SCTHash[summary]);
    float PSTripModules{0.};
    psTripDCSSCT(m_SCTHash[psTripDCS], PSTripModules);
    
    for (int iProblem{0}; iProblem<numberOfProblemForCoverage; iProblem++) {
      for (const IdentifierHash& hash: m_SCTHash[iProblem]) {
        fillWafer(m_geo[hash], m_mapSCT[iProblem]);
      }
    }
    
    //detector coverage
    for (int iProblem{0}; iProblem<numberOfProblemForCoverage; iProblem++) {
      if (iProblem==all) continue;

      double detector_coverage{calculateDetectorCoverage(m_mapSCT[iProblem])};
      m_detectorCoverageVsLbs[iProblem]->Fill(static_cast<double>(current_lb), detector_coverage);
      double good_detector_coverage{m_detectorCoverageVsLbs[iProblem]->GetBinContent(current_lb)};
      ATH_MSG_INFO("Detector Coverage : " << good_detector_coverage);
    }

    //Modules affected by PS Tirp
    m_PSTripModulesVsLbs ->Fill(static_cast<double>(current_lb), PSTripModules);
    
  }

  //--- Counting # of links with categorised errors
  for (int reg{0}; reg < NREGIONS_INC_GENERAL - 1; ++reg) {
    int nLayers{n_layers[reg]*2};
    for (int lyr{0}; lyr < nLayers; ++lyr) {
      const unsigned int nBinsX{m_pallErrsCate[CategoryErrors::SUMMARY][reg][lyr]->GetNbinsX() + 1};
      const unsigned int nBinsY{m_pallErrsCate[CategoryErrors::SUMMARY][reg][lyr]->GetNbinsY() + 1};
      for (unsigned int xb{1}; xb < nBinsX; ++xb) {
        double cxb{m_pallErrsCate[CategoryErrors::SUMMARY][reg][lyr]->GetXaxis()->GetBinCenter(xb)};
        for (unsigned int yb{1}; yb < nBinsY; ++yb) {
          double cyb{m_pallErrsCate[CategoryErrors::SUMMARY][reg][lyr]->GetYaxis()->GetBinCenter(yb)};
          for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
            bytestreamCate_errs[errType][reg] += m_pallErrsCate[errType][reg][lyr]->GetBinContent(xb, yb);
            if (m_pallErrsCate[errType][reg][lyr]->GetBinContent(xb, yb) > 0) {
              m_allErrsCate[errType][reg][lyr]->Fill(cxb, cyb, 1);
              if ((errType == CategoryErrors::SUMMARY) and (m_environment == AthenaMonManager::online)) {
                m_summaryErrsRecent[reg][lyr]->Fill(cxb, cyb, 1);
              }
              tot_mod_bytestreamCate_errs[errType][reg]++;
              tot_mod_bytestreamCate_errs_layer[errType][reg][lyr]++;
            } else {
              m_allErrsCate[errType][reg][lyr]->Fill(cxb, cyb, 0);
              if ((errType == CategoryErrors::SUMMARY) and (m_environment == AthenaMonManager::online)) {
                m_summaryErrsRecent[reg][lyr]->Fill(cxb, cyb, 0);
              }
            }
            if (m_doPerLumiErrors and m_doErr2DPerLumiHists) {
              if (m_pallErrsCatePerLumi[errType][reg][lyr]->GetBinContent(xb, yb) > 0) {
                m_allErrsCatePerLumi[errType][reg][lyr]->Fill(cxb, cyb, 1);
              } else {
                m_allErrsCatePerLumi[errType][reg][lyr]->Fill(cxb, cyb, 0);
              }
            }
          }

          if (m_environment == AthenaMonManager::online) {
            // mf cast to (int) below to avoid compiler warnings... we do want int, right? Some code duplication
            // below...
            if ((m_numberOfEvents % m_checkrate == 0) and m_runOnline) {
              for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
                int content{static_cast<int>(m_pallErrsCate[errType][reg][lyr]->GetBinContent(xb, yb))};
                int evt{m_numberOfEvents};
                m_allErrsCate[errType][reg][lyr]->Fill(cxb, cyb, 1, content);
                m_allErrsCate[errType][reg][lyr]->Fill(cxb, cyb, 0, evt - content);
              }
            }
          }
        } // Loop for BinY
      } // Loop for BinX
    } // Loop for Layer
  } // Loop for Region

  for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
    bytestreamCate_errs[errType][GENERAL_INDEX] =
      bytestreamCate_errs[errType][BARREL_INDEX] +
      bytestreamCate_errs[errType][ENDCAP_A_INDEX] +
      bytestreamCate_errs[errType][ENDCAP_C_INDEX];
    tot_mod_bytestreamCate_errs[errType][GENERAL_INDEX] =
      tot_mod_bytestreamCate_errs[errType][BARREL_INDEX] +
      tot_mod_bytestreamCate_errs[errType][ENDCAP_A_INDEX] +
      tot_mod_bytestreamCate_errs[errType][ENDCAP_C_INDEX];
  }

  for (int reg{0}; reg < NREGIONS_INC_GENERAL; ++reg) {
    for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
      m_ByteStreamCategorisedVsLB[errType][reg]->Fill(current_lb, (bytestreamCate_errs[errType][reg]));
      m_LinksWithCategorisedErrorsVsLB[errType][reg]->Fill(current_lb, (tot_mod_bytestreamCate_errs[errType][reg]));
      if (reg==NREGIONS_INC_GENERAL-1)continue;
      int nLayers{n_layers[reg]*2};
      for (int lyr{0}; lyr < nLayers; ++lyr) {
        m_LinksWithCategorisedErrorsVsLBLayer[errType][reg][lyr]->Fill(current_lb,
                                                                       (tot_mod_bytestreamCate_errs_layer[errType][reg][lyr]));
      }
    }
  }

  //--- Time Dependent SP plots only online
  m_nErrors_buf[m_nErrors_pos] = total_errors;
  m_nLinksWithErrors_buf[m_nErrors_pos] = tot_mod_bytestreamCate_errs[CategoryErrors::SUMMARY][GENERAL_INDEX];
  m_nErrors_pos++;
  if (m_nErrors_pos == m_evtsbins) {
    m_nErrors_pos = 0;
  }
  if (m_numberOfEvents % m_checkrate == 0) {
    m_nErrors->Reset();
    int latest_nErrors_pos{m_nErrors_pos};
    m_nLinksWithErrors->Reset();
    for (int i{1}; i < m_evtsbins; i++) {
      if (latest_nErrors_pos == m_evtsbins) {
        latest_nErrors_pos = 0;
      }
      if (m_numberOfEvents < m_evtsbins) {
        if (i < m_nErrors_pos) {
          m_nErrors->SetBinContent(i, m_nErrors_buf[i]);
          m_nLinksWithErrors->SetBinContent(i, m_nLinksWithErrors_buf[i]);
        } else {
          m_nErrors->SetBinContent(i, 0);
          m_nLinksWithErrors->SetBinContent(i, 0);
        }
      } else {
        m_nErrors->SetBinContent(i, m_nErrors_buf[latest_nErrors_pos]);
        m_nLinksWithErrors->SetBinContent(i, m_nLinksWithErrors_buf[latest_nErrors_pos]);
        m_nErrors->GetXaxis()->Set(m_evtsbins, m_numberOfEvents - m_evtsbins, m_numberOfEvents);
        m_nLinksWithErrors->GetXaxis()->Set(m_evtsbins, m_numberOfEvents - m_evtsbins, m_numberOfEvents);
      }
      latest_nErrors_pos++;
      if (latest_nErrors_pos == m_evtsbins) {
        latest_nErrors_pos = 0;
      }
    }
  }

  m_numberOfEvents++;
  m_numberOfEventsLumi++;
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                       SCTErrMonTool :: bookErrHistosHelper
// Avoids duplicate code in the bookErrHistosXXXX functions; added 8/9/09, Martin Flechl
// ====================================================================================================
StatusCode
SCTErrMonTool::bookErrHistosHelper(MonGroup& mg, TString name, TString title, TString titlehitmap,
                                   TProfile2D_LW*& tprof, TH2F_LW*& th, const int layer, const bool barrel) const {
  ostringstream streamhitmap;

  streamhitmap << layer / 2 << "_" << layer % 2;

  name += streamhitmap.str();
  title += streamhitmap.str();
  titlehitmap += streamhitmap.str();

  if (barrel) {
    const float xlo{FIRST_ETA_BIN - 0.5};
    const float xhi{LAST_ETA_BIN  + 0.5};
    const float ylo{FIRST_PHI_BIN - 0.5};
    const float yhi{LAST_PHI_BIN  + 0.5};
    tprof = TProfile2D_LW::create(name, title, N_ETA_BINS, xlo, xhi, N_PHI_BINS, ylo, yhi);
    th = TH2F_LW::create(titlehitmap, title, N_ETA_BINS, xlo, xhi, N_PHI_BINS, ylo, yhi);
  } else {
    const float xlo{FIRST_ETA_BIN_EC - 0.5};
    const float xhi{LAST_ETA_BIN_EC  + 0.5};
    const float ylo{FIRST_PHI_BIN_EC - 0.5};
    const float yhi{LAST_PHI_BIN_EC  + 0.5};
    tprof = TProfile2D_LW::create(name, title, N_ETA_BINS_EC, xlo, xhi, N_PHI_BINS_EC, ylo, yhi);
    th = TH2F_LW::create(titlehitmap, title, N_ETA_BINS_EC, xlo, xhi, N_PHI_BINS_EC, ylo, yhi);
  }
  StatusCode sc0{mg.regHist(th)};
  StatusCode sc1{mg.regHist(tprof)};
  if (sc0.isFailure() or sc1.isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                       SCTErrMonTool :: bookErrHistosHelper
// Avoids duplicate code in the bookErrHistosXXXX functions; added 08/08/11, Daniel Damiani
// ====================================================================================================
StatusCode
SCTErrMonTool::bookErrHistosHelper(MonGroup& mg, TString name, TString title, TProfile2D_LW*& tprof, const int layer,
                                   const bool barrel) const {
  ostringstream streamhitmap;

  streamhitmap << layer / 2 << "_" << layer % 2;

  name += streamhitmap.str();
  title += streamhitmap.str();
  if (barrel) {
    tprof = TProfile2D_LW::create(name, title, N_ETA_BINS, FIRST_ETA_BIN - 0.5, LAST_ETA_BIN + 0.5, N_PHI_BINS,
                                  FIRST_PHI_BIN - 0.5, LAST_PHI_BIN + 0.5);
  } else {
    tprof = TProfile2D_LW::create(name, title, N_ETA_BINS_EC, FIRST_ETA_BIN_EC - 0.5, LAST_ETA_BIN_EC + 0.5,
                                  N_PHI_BINS_EC, FIRST_PHI_BIN_EC - 0.5, LAST_PHI_BIN_EC + 0.5);
  }

  StatusCode sc{mg.regHist(tprof)};
  if (sc.isFailure()) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                       SCTErrMonTool :: bookErrHistos
//                       Book 1D and 2D Histograms of errors
// ====================================================================================================
StatusCode
SCTErrMonTool::bookErrHistos(int reg=-1) { // reg = 0:EC, 1:B, 2:EA

  if (reg==-1) return StatusCode::FAILURE;

  int nLayers{n_layers[reg]*2};
  string regName{("SCT" + subDetNameShort[reg]).Data()};
  string layerTitle{layerName[reg].Data()};
  layerTitle.at(0)=toupper(layerTitle.at(0));

  if (m_doPerLumiErrors) {
    MonGroup lumiErr{this, ("SCT/"+regName+"/errors").c_str(), lumiBlock, ATTRIB_UNMANAGED};
    if (ManagedMonitorToolBase::newLumiBlockFlag()) {
      //______________________________________________________________________________________
      m_numErrorsPerLumi[reg] =
        TH2F_LW::create("NumErrsPerLumi", ("Total Number of Error Types for "+layerTitle+" per Lumi-Block").c_str(),
                        SCT_ByteStreamErrors::NUM_ERROR_TYPES, -0.5, SCT_ByteStreamErrors::NUM_ERROR_TYPES-0.5,
                        nLayers, -0.5, nLayers-0.5);
      if (lumiErr.regHist(m_numErrorsPerLumi[reg]).isFailure()) {
        ATH_MSG_WARNING("Couldn't book NumErrsPerLumi");
      }
      //______________________________________________________________________________________
      m_rateErrorsPerLumi[reg] =
        TProfile2D_LW::create("RateErrorsPerLumi", ("Rate of Error Types for "+layerTitle+" per Lumi-Block").c_str(),
                              SCT_ByteStreamErrors::NUM_ERROR_TYPES, -0.5, SCT_ByteStreamErrors::NUM_ERROR_TYPES-0.5,
                              nLayers, -0.5, nLayers-0.5);
      if (lumiErr.regHist(m_rateErrorsPerLumi[reg]).isFailure()) {
        ATH_MSG_WARNING("Couldn't book RateErrorsPerLumi");
      }
      //______________________________________________________________________________________
      for (unsigned int bin{0}; bin < SCT_ByteStreamErrors::NUM_ERROR_TYPES; bin++) {
        m_numErrorsPerLumi[reg]->GetXaxis()->SetBinLabel(bin+1, SCT_ByteStreamErrors::errorTypesDescription[bin].c_str());
        m_rateErrorsPerLumi[reg]->GetXaxis()->SetBinLabel(bin+1, SCT_ByteStreamErrors::errorTypesDescription[bin].c_str());
      }
      for (int bin{0}; bin < nLayers; bin++) {
        m_numErrorsPerLumi[reg]->GetYaxis()->SetBinLabel(bin+1, (to_string(bin/2) +"_"+ to_string(bin%2)).c_str());
        m_rateErrorsPerLumi[reg]->GetYaxis()->SetBinLabel(bin+1, (to_string(bin/2) +"_"+ to_string(bin%2)).c_str());
      }
      bool somethingFailed{false};
      if (m_doErr2DPerLumiHists) {
        for (int layer{0}; layer < nLayers; ++layer) {
          for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
            MonGroup lumiErr2{this, "SCT/"+regName+"/errors/"+SCT_ByteStreamErrors::errorTypesDescription[errType], lumiBlock, ATTRIB_UNMANAGED};
            string name1{"SCT_" + SCT_ByteStreamErrors::errorTypesDescription[errType] + subDetNameShort[reg].Data() + "_PerLumi"};
            string title{SCT_ByteStreamErrors::errorTypesDescription[errType] + " per Lumi-Block per "+layerTitle};
            string name2{"SCT_T" + SCT_ByteStreamErrors::errorTypesDescription[errType]  + subDetNameShort[reg].Data() + "_PerLumi_"};
            somethingFailed |= bookErrHistosHelper(lumiErr2, name1, title, name2,m_allErrsPerLumi[errType][reg][layer],
                                                   m_pallErrsPerLumi[errType][reg][layer], layer, reg==BARREL_INDEX).isFailure();
          }
          for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
            MonGroup lumiErr2{this, "SCT/"+regName+"/errors/"+errorsString(errType).Data(), lumiBlock, ATTRIB_UNMANAGED};
            string name1{("SCT_NumberOf" + errorsString(errType)  + subDetNameShort[reg].Data() + "_PerLumi").Data()};
            string title{("Num of " + errorsString(errType) + " Errors per Lumi-Block per "+layerTitle).Data()};
            string name2{("SCT_T" + errorsString(errType)  + subDetNameShort[reg].Data() + "_PerLumi_").Data()};
            somethingFailed |= bookErrHistosHelper(lumiErr2, name1, title, name2, m_allErrsCatePerLumi[errType][reg][layer],
                                                   m_pallErrsCatePerLumi[errType][reg][layer], layer, reg==BARREL_INDEX).isFailure();
          }
        }
      }
      if (somethingFailed) {
        return StatusCode::FAILURE;
      }
    }
  }

  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup err{this, "SCT/"+regName+"/errors/", run, ATTRIB_UNMANAGED};
    bool somethingFailed{false};
    // Book percentage error histograms
    //______________________________________________________________________________________
    m_firstHit[reg] = TH1F_LW::create("FirstHit_"+subDetNameShort[reg], ("Percentage of FirstHit errors in "+regName).c_str(), 50, 0., 100.);
    m_firstHit[reg]->GetXaxis()->SetTitle("Percentage of FirstHit errors");
    if (err.regHist(m_firstHit[reg]).isFailure()) {
      ATH_MSG_WARNING("Cannot book Histogram:" << "FirstHit");
    }
    //______________________________________________________________________________________
    m_secondHit[reg] = TH1F_LW::create("SecondHit_"+subDetNameShort[reg], ("Percentage of SecondHit errors in "+regName).c_str(), 50, 0., 100.);
    m_secondHit[reg]->GetXaxis()->SetTitle("Percentage of SecondHit errors");
    if (err.regHist(m_secondHit[reg]).isFailure()) {
      ATH_MSG_WARNING("Cannot book Histogram:" << "SecondHit");
    }
    //______________________________________________________________________________________
    for (int layer{0}; layer < nLayers; ++layer) {
      for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
        MonGroup err2{this, "SCT/"+regName+"/errors/"+SCT_ByteStreamErrors::errorTypesDescription[errType], run, ATTRIB_UNMANAGED};
        string name1{"SCT_" + SCT_ByteStreamErrors::errorTypesDescription[errType]  + subDetNameShort[reg].Data() + "_"};
        string title{SCT_ByteStreamErrors::errorTypesDescription[errType] + " per "  + layerTitle};
        string name2{"SCT_T" + SCT_ByteStreamErrors::errorTypesDescription[errType]  + subDetNameShort[reg].Data() + "_"};
        //______________________________________________________________________________________
        somethingFailed |= bookErrHistosHelper(err2, name1, title, name2, m_allErrs[errType][reg][layer],
                                               m_pallErrs[errType][reg][layer], layer, reg==BARREL_INDEX).isFailure();
        m_allErrs[errType][reg][layer]->GetXaxis()->SetTitle("Index in the direction of #eta");
        m_allErrs[errType][reg][layer]->GetYaxis()->SetTitle("Index in the direction of #phi");
        //______________________________________________________________________________________
      }
      for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
        MonGroup err2{this, "SCT/"+regName+"/errors/"+errorsString(errType).Data(), lumiBlock, ATTRIB_UNMANAGED};
        string name1{("SCT_NumberOf" + errorsString(errType)  + subDetNameShort[reg].Data() + "_").Data()};
        string title{("Num of " + errorsString(errType) + " per "+layerTitle).Data()};
        string name2{("SCT_T" + errorsString(errType)  + subDetNameShort[reg].Data() + "_").Data()};
        somethingFailed |= bookErrHistosHelper(err2, name1, title, name2, m_allErrsCate[errType][reg][layer],
                                               m_pallErrsCate[errType][reg][layer], layer, reg==BARREL_INDEX).isFailure();
      }
      if (m_environment == AthenaMonManager::online) {
        somethingFailed |= bookErrHistosHelper(err, 
                                               ("summaryErrsRecent"+subDetNameShort[reg]+"_").Data(), 
                                               "summary recent Layer ", 
                                               m_summaryErrsRecent[reg][layer], 
                                               layer, reg==BARREL_INDEX).isFailure();
      }
    }
    if (somethingFailed) {
      ATH_MSG_DEBUG("Something went wrong in bookErrHistos "<< regName);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode
SCTErrMonTool::bookErrHistosGen() {

  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup MaskErrs{this, "SCT/GENERAL/errors", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED};
    m_MaskedAllLinks = new TH1I("Masked Links", "Number of Masked Links for SCT,ECA,B,ECC", 4, -0.5, 3.5); // should reorder to C,B,A,total ?
    m_MaskedAllLinks->GetXaxis()->SetBinLabel(1, "EndCapC");
    m_MaskedAllLinks->GetXaxis()->SetBinLabel(2, "Barrel");
    m_MaskedAllLinks->GetXaxis()->SetBinLabel(3, "EndCapA");
    m_MaskedAllLinks->GetXaxis()->SetBinLabel(4, "All");
    if (MaskErrs.regHist(m_MaskedAllLinks).isFailure()) {
      ATH_MSG_WARNING("Couldn't book MaskedLinks");
    }
    // Book errors vs event numbers
    MonGroup Errors{this, "SCT/GENERAL/errors", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED};
    m_nErrors = new TH1I("sct_errors_vs_en", "Number of Errors vs Event Number", m_evtsbins, 1, m_evtsbins + 1);
    m_nErrors->GetXaxis()->SetTitle("Event Number");
    m_nErrors->GetYaxis()->SetTitle("Num of Errors");
    m_nLinksWithErrors = new TH1I("sct_links_with_errors_vs_en","Number of Links with Errors vs Event Number",
                                  m_evtsbins, 1, m_evtsbins + 1);
    m_nLinksWithErrors->GetXaxis()->SetTitle("Event Number");
    m_nLinksWithErrors->GetYaxis()->SetTitle("Num of Links with Errors");
    m_nErrors_buf.reserve(m_evtsbins);
    m_nLinksWithErrors_buf.reserve(m_evtsbins);
    m_nErrors_pos = 0;
    if (Errors.regHist(m_nErrors).isFailure()) {
      ATH_MSG_WARNING("Couldn't book nErrors vs event number hist");
    }
    if (Errors.regHist(m_nLinksWithErrors).isFailure()) {
      ATH_MSG_WARNING("Couldn't book nLinksWithErrors vs event number hist");
    }

  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                         SCTErrMonTool :: bookConfMaps
// ====================================================================================================
StatusCode
SCTErrMonTool::bookConfMapsGen() {
  static const string SummaryBinNames[ConfbinsSummary] = {
    "Mod Out", "Flagged Links", "Masked Links", "Errors", "Inefficient", "Noisy"
  };
  static const string DetailedBinNames[ConfbinsDetailed] = {
    "Modules", "Link 0", "Link 1", "Chips", "Strips (10^{2})"
  };
  static const string OnlineBinNames[ConfbinsOnline] = {
    "Mod Out", "Flagged Links", "Masked Links", "Errors"
  };
  static const TString regLabel[NREGIONS_INC_GENERAL] = {
    "EndcapC", "Barrel", "EndcapA", ""
  };
  static const TString regTitle[NREGIONS_INC_GENERAL] = {
    "EndcapC", "Barrel", "EndcapA", "All Region"
  };

  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup ConfHist[NREGIONS_INC_GENERAL] = {
      MonGroup{this, "SCT/SCTEC/Conf",   ManagedMonitorToolBase::run, ATTRIB_UNMANAGED},
      MonGroup{this, "SCT/SCTB/Conf",    ManagedMonitorToolBase::run, ATTRIB_UNMANAGED},
      MonGroup{this, "SCT/SCTEA/Conf",   ManagedMonitorToolBase::run, ATTRIB_UNMANAGED},
      MonGroup{this, "SCT/GENERAL/Conf", ManagedMonitorToolBase::run, ATTRIB_UNMANAGED}
    };

    string streamName{streamNameFunction()->getStreamName(this, ConfHist[GENERAL_INDEX], "", false)};
    m_path = streamName.substr(0, streamName.rfind("SCT/GENERAL/Conf"));
    ATH_MSG_INFO("Global Path :" << m_path);

    if (m_makeConfHisto or testOffline) {
      m_DetailedConfiguration = TProfile_LW::create("SCTConfDetails", "Exclusion from the Configuration",
                                                    ConfbinsDetailed, -0.5, ConfbinsDetailed - 0.5);
      for (int bin{0}; bin < ConfbinsDetailed; bin++) {
        m_DetailedConfiguration->GetXaxis()->SetBinLabel(bin + 1, DetailedBinNames[bin].c_str());
      }

      for (int reg{0}; reg < NREGIONS_INC_GENERAL; ++reg) {
        m_Conf[reg] = TProfile_LW::create("SCTConf"+regLabel[reg], "Num of Problematic Modules in "+regTitle[reg],
                                          ConfbinsSummary, -0.5, ConfbinsSummary-0.5);
        m_ConfRN[reg] = TProfile_LW::create("SCTConf"+regLabel[reg]+"RN", "Num of Problematic Modules in "+regTitle[reg],
                                            ConfbinsSummary, -0.5, ConfbinsSummary-0.5);
        for (int bin{0}; bin < ConfbinsSummary; bin++) {
          m_Conf[reg]->GetXaxis()->SetBinLabel(bin + 1, SummaryBinNames[bin].c_str());
          m_ConfRN[reg]->GetXaxis()->SetBinLabel(bin + 1, SummaryBinNames[bin].c_str());
        }

        m_ConfOutModules[reg] = TProfile_LW::create("SCTConf"+regLabel[reg]+"OutM", "Num of Out Modules in "+regTitle[reg], 1, -0.5, 0.5);
        m_ConfNew[reg] = TProfile_LW::create("SCTConf"+regLabel[reg]+"New", "Num of Problematic Modules in "+regTitle[reg],
                                             ConfbinsSummary-1, -0.5, ConfbinsSummary-1-0.5);
        m_ConfOutModules[reg]->GetXaxis()->SetBinLabel(1, "Mod Out");
        for (int bin{1}; bin < ConfbinsSummary; bin++) {
          m_ConfNew[reg]->GetXaxis()->SetBinLabel(bin, SummaryBinNames[bin].c_str());
        }

        if ((m_environment == AthenaMonManager::online) or testOffline) {
          m_ConfOnline[reg] = TProfile_LW::create("SCTOnlineConf"+regLabel[reg], "Num of Out Links in "+regTitle[reg]+" Online",
                                                  ConfbinsOnline, -0.5, ConfbinsOnline-0.5);
          for (int bin{0}; bin < ConfbinsOnline; bin++) {
            m_ConfOnline[reg]->GetXaxis()->SetBinLabel(bin + 1, OnlineBinNames[bin].c_str());
          }
        }

        for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
          m_ByteStreamVsLB[errType][reg] =
            TProfile_LW::create("SCT_" + SCT_ByteStreamErrors::errorTypesDescription[errType] + "VsLbs" + regLabel[reg],
                                "Ave. " + SCT_ByteStreamErrors::errorTypesDescription[errType] + " per LB in " + regTitle[reg],
                                NBINS_LBs, 0.5, NBINS_LBs + 0.5);
          m_ByteStreamVsLB[errType][reg]->GetXaxis()->SetTitle("LumiBlock");
          m_ByteStreamVsLB[errType][reg]->GetYaxis()->SetTitle("Num of " + TString(SCT_ByteStreamErrors::errorTypesDescription[errType]));
          m_ByteStreamWithSctFlagVsLB[errType][reg] =
            TProfile_LW::create("SCT_" + SCT_ByteStreamErrors::errorTypesDescription[errType] + "WithSctFlagVsLbs" + regLabel[reg],
                                "Ave. " + SCT_ByteStreamErrors::errorTypesDescription[errType] + " with SCT flag per LB in " + regTitle[reg],
                                NBINS_LBs, 0.5, NBINS_LBs+0.5);
          m_ByteStreamWithSctFlagVsLB[errType][reg]->GetXaxis()->SetTitle("LumiBlock");
          m_ByteStreamWithSctFlagVsLB[errType][reg]->GetYaxis()->SetTitle(("Num of " + SCT_ByteStreamErrors::errorTypesDescription[errType]).c_str());
        }

        for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
          m_ByteStreamCategorisedVsLB[errType][reg] =
            TProfile_LW::create("SCT_NumberOf" + errorsString(errType) + "VsLbs" + regLabel[reg],
                                "Ave. Number of " + errorsString(errType) + " per LB in " + regTitle[reg],
                                NBINS_LBs, 0.5, NBINS_LBs+0.5);
          m_ByteStreamCategorisedVsLB[errType][reg]->GetXaxis()->SetTitle("LumiBlock");
          m_ByteStreamCategorisedVsLB[errType][reg]->GetYaxis()->SetTitle(("Num of "+errorsString(errType)));

          m_LinksWithCategorisedErrorsVsLB[errType][reg] =
            TProfile_LW::create("SCT_LinksWith" + errorsString(errType) + "VsLbs" + regLabel[reg],
                                "Ave. Num of Links with " + errorsString(errType) + " per LB in " + regTitle[reg],
                                NBINS_LBs, 0.5, NBINS_LBs+0.5);
          m_LinksWithCategorisedErrorsVsLB[errType][reg]->GetXaxis()->SetTitle("LumiBlock");
          m_LinksWithCategorisedErrorsVsLB[errType][reg]->GetYaxis()->SetTitle("Num of Links with "+errorsString(errType));
          if (reg==NREGIONS_INC_GENERAL-1) continue;
          const int nLayers{n_layers[reg]*2};
          for (int layer{0}; layer < nLayers; ++layer) {
            m_LinksWithCategorisedErrorsVsLBLayer[errType][reg][layer] =
              TProfile_LW::create("SCT_LinksWith" + errorsString(errType) + "VsLbs" + regLabel[reg] + "lyr" + to_string(layer/2) + "_" + to_string(layer%2),
                                  "Ave. num of links with " + errorsString(errType) + " per LB in " + regTitle[reg] + " layer" + to_string(layer/2) + "_" + to_string(layer%2),
                                  NBINS_LBs, 0.5, NBINS_LBs + 0.5);
            m_LinksWithCategorisedErrorsVsLBLayer[errType][reg][layer]->GetXaxis()->SetTitle("LumiBlock");
          }
        }
      }

      m_NumberOfSCTFlagErrorsVsLB = TH1F_LW::create("NumberOfSCTFlagErrorsVsLB","Num of SCT Flag errors per LB ",NBINS_LBs, 0.5, NBINS_LBs + 0.5);
      m_NumberOfSCTFlagErrorsVsLB->GetXaxis()->SetTitle("LumiBlock");

      m_NumberOfEventsVsLB = TH1F_LW::create("NumberOfEventsVsLB", "Num of events per LB ", NBINS_LBs, 0.5,NBINS_LBs + 0.5);
      m_NumberOfEventsVsLB->GetXaxis()->SetTitle("LumiBlock");

      m_FractionOfSCTFlagErrorsPerLB = TProfile_LW::create("FractionOfSCTFlagErrorsPerLB", "Frac of SCT Flag errors per LB ",NBINS_LBs, 0.5, NBINS_LBs + 0.5);
      m_FractionOfSCTFlagErrorsPerLB->GetXaxis()->SetTitle("LumiBlock");

      if ((m_environment == AthenaMonManager::online) or testOffline) {
        m_ConfEffOnline = new TProfile("SCTEffConf", "Number of Inefficient Modules Online",
                                       NREGIONS_INC_GENERAL, -0.5, NREGIONS_INC_GENERAL-0.5);
        m_ConfNoiseOnline = TProfile_LW::create("SCTNoiseConf", "Number of Noisy Modules Online",
                                                NREGIONS_INC_GENERAL, -0.5, NREGIONS_INC_GENERAL-0.5);
        m_ConfNoiseOnlineRecent = TProfile_LW::create("SCTNoiseConfRecent", "Number of Noisy Modules Online Recent",
                                                      NREGIONS_INC_GENERAL, -0.5, NREGIONS_INC_GENERAL - 0.5);
        for (int bin{0}; bin < NREGIONS_INC_GENERAL; bin++) {
          m_ConfEffOnline->GetXaxis()->SetBinLabel(bin + 1, ("Ineff "+subDetNameShort[bin]).Data());
          m_ConfNoiseOnline->GetXaxis()->SetBinLabel(bin + 1, ("Noisy "+subDetNameShort[bin]).Data());
          m_ConfNoiseOnlineRecent->GetXaxis()->SetBinLabel(bin + 1, ("Noisy "+subDetNameShort[bin]).Data());
        }
      }

      if (ConfHist[GENERAL_INDEX].regHist(m_DetailedConfiguration).isFailure()) {
        ATH_MSG_WARNING("Cannot book Histogram:SCTConfDetails");
      }
      for (int reg{0}; reg < NREGIONS_INC_GENERAL; ++reg) {
        if (ConfHist[reg].regHist(m_Conf[reg]).isFailure()) {
          ATH_MSG_WARNING("Cannot book Histogram:SCTConf");
        }
        if (ConfHist[reg].regHist(m_ConfRN[reg]).isFailure()) {
          ATH_MSG_WARNING("Cannot book Histogram:SCTConf");
        }
        if (ConfHist[reg].regHist(m_ConfNew[reg]).isFailure()) {
          ATH_MSG_WARNING("Cannot book Histogram:SCTConf");
        }
        if (ConfHist[reg].regHist(m_ConfOutModules[reg]).isFailure()) {
          ATH_MSG_WARNING("Cannot book Histogram:SCTConf");
        }

        if ((m_environment == AthenaMonManager::online) or testOffline) {
          if (ConfHist[reg].regHist(m_ConfOnline[reg]).isFailure()) {
            ATH_MSG_WARNING("Cannot book Histogram:SCTConfOnline");
          }
        }

        for (int errType{0}; errType < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++errType) {
          if (ConfHist[reg].regHist(m_ByteStreamVsLB[errType][reg]).isFailure()) {
            ATH_MSG_WARNING("Cannot book Histogram:" + SCT_ByteStreamErrors::errorTypesDescription[errType]);
          }
          if (ConfHist[reg].regHist(m_ByteStreamWithSctFlagVsLB[errType][reg]).isFailure()) {
            ATH_MSG_WARNING("Cannot book Histogram:" + SCT_ByteStreamErrors::errorTypesDescription[errType] << "with SCT flag");
          }
        }
        for (int errType{0}; errType < CategoryErrors::N_ERRCATEGORY; ++errType) {
          if (ConfHist[reg].regHist(m_ByteStreamCategorisedVsLB[errType][reg]).isFailure()) {
            ATH_MSG_WARNING("Cannot book Histogram:SCTNumberOf" + errorsString(errType) + "VsLbs" + regLabel[reg]);
          }
          if (ConfHist[reg].regHist(m_LinksWithCategorisedErrorsVsLB[errType][reg]).isFailure()) {
            ATH_MSG_WARNING("Cannot book Histogram:SCTLinksWith" + errorsString(errType) + "VsLbs" + regLabel[reg]);
          }
          if (reg==NREGIONS_INC_GENERAL-1)continue;
          int nLayers{n_layers[reg]*2};
          for (int layer{0}; layer < nLayers; ++layer) {
            if (ConfHist[reg].regHist(m_LinksWithCategorisedErrorsVsLBLayer[errType][reg][layer]).isFailure()) {
              ATH_MSG_WARNING("Cannot book Histogram:SCTLinksWith" + errorsString(errType) + "VsLbs" + regLabel[reg] + "lyr" + to_string(layer/2) + "_" + to_string(layer%2) << "with SCT flag");
            }
          }
        }
      }

      if (ConfHist[GENERAL_INDEX].regHist(m_NumberOfSCTFlagErrorsVsLB).isFailure()) {
        ATH_MSG_WARNING("Cannot book Histogram:NumberOfSCTFlagErrors");
      }

      if (ConfHist[GENERAL_INDEX].regHist(m_NumberOfEventsVsLB).isFailure()) {
        ATH_MSG_WARNING("Cannot book Histogram:NumberOfEventsVsLB");
      }

      if (ConfHist[GENERAL_INDEX].regHist(m_FractionOfSCTFlagErrorsPerLB).isFailure()) {
        ATH_MSG_WARNING("Cannot book Histogram:FractionOfSCTFlagErrorsPerLB");
      }

      if ((m_environment == AthenaMonManager::online) or testOffline) {
        if (ConfHist[GENERAL_INDEX].regHist(m_ConfEffOnline).isFailure()) {
          ATH_MSG_WARNING("Cannot book Histogram:SCTConfEffOnline");
        }
        if (ConfHist[GENERAL_INDEX].regHist(m_ConfNoiseOnline).isFailure()) {
          ATH_MSG_WARNING("Cannot book Histogram:SCTConfNoiseOnline");
        }
        if (ConfHist[GENERAL_INDEX].regHist(m_ConfNoiseOnlineRecent).isFailure()) {
          ATH_MSG_WARNING("Cannot book Histogram:SCTConfNoiseOnlineRecent");
        }
      }
      ATH_MSG_DEBUG("Finished registering Conf Histograms :" << m_path);
    }// end if m_makeConfHisto or testOffline
  }// end if isNewRun
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                   SCTErrMonTool :: bookConfMaps
// ====================================================================================================
StatusCode
SCTErrMonTool::bookConfMaps(int reg=-1) { // reg = 0:EC, 1:B, 2:EA
  if (reg==-1) return StatusCode::FAILURE;

  string regName{("SCT" + subDetNameShort[reg]).Data()};
  int nLayers{n_layers[reg]*2};

  if (ManagedMonitorToolBase::newRunFlag()) {
    MonGroup confMaps{this, ("SCT/"+regName+"/Conf").c_str(), ManagedMonitorToolBase::run, ATTRIB_UNMANAGED};
    m_p2DmapHistoVectorAll[reg].clear();
    for (int layer{0}; layer < nLayers; ++layer) {
      string mapName{string{"modulemap"} + subDetNameShort[reg].Data() + to_string(layer/2) + "_" + to_string(layer%2)};
      TH2F_LW* hitsHisto_tmp = TH2F_LW::create(mapName.c_str(),
                                               (string{"Module Out of Configuration : "}+subDetName[reg].Data()+", "+
                                                layerName[reg].Data()+" "+to_string(layer/2)+
                                                " side "+to_string(layer%2)).c_str(),
                                               n_etabins[reg], f_etabin[reg] - 0.5, l_etabin[reg] + 0.5,
                                               n_phibins[reg], f_phibin[reg] - 0.5, l_phibin[reg] + 0.5);
      hitsHisto_tmp->GetXaxis()->SetTitle("Index in the direction of #eta");
      hitsHisto_tmp->GetYaxis()->SetTitle("Index in the direction of #phi");
      if (confMaps.regHist(hitsHisto_tmp).isFailure()) {
        ATH_MSG_WARNING("Cannot book Histogram:" << mapName);
      }
      m_p2DmapHistoVectorAll[reg].push_back(hitsHisto_tmp);
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                          SCTErrMonTool :: fillCondDBMaps
// ====================================================================================================
StatusCode
SCTErrMonTool::fillCondDBMaps() {
  int Flagged[NREGIONS_INC_GENERAL] = {
    0, 0, 0, 0
  };
  int MOut[NREGIONS_INC_GENERAL] = {
    0, 0, 0, 0
  };
  int MaskedAllLinks[NREGIONS_INC_GENERAL] = {
    static_cast<int>(m_MaskedAllLinks->GetBinContent(1)), static_cast<int>(m_MaskedAllLinks->GetBinContent(2)),
    static_cast<int>(m_MaskedAllLinks->GetBinContent(3)), static_cast<int>(m_MaskedAllLinks->GetBinContent(4))
  };
  int ModErr[NREGIONS_INC_GENERAL] = {
    0, 0, 0, 0
  };
  int InEffModules[NREGIONS_INC_GENERAL] = {
    0, 0, 0, 0
  };
  int NoisyModules[NREGIONS_INC_GENERAL] = {
    0, 0, 0, 0
  };
  int NoisyModulesRecent[NREGIONS_INC_GENERAL] = {
    0, 0, 0, 0
  };
  int RNoisyModules[NREGIONS_INC_GENERAL] = {
    0, 0, 0, 0
  };

  bool failedbooking{false};
  TH2* hitsHisto_tmp[2];

  // Pointers are deleted by regHist() method
  string stem;
  SCT_ID::const_id_iterator planeIterator{m_pSCTHelper->wafer_begin()};
  SCT_ID::const_id_iterator planeEnd{m_pSCTHelper->wafer_end()};
  for (; planeIterator not_eq planeEnd; ++planeIterator) {
    Identifier planeId{*planeIterator};
    int eta{m_pSCTHelper->eta_module(planeId)};
    int phi{m_pSCTHelper->phi_module(planeId)};
    int barrel_ec{m_pSCTHelper->barrel_ec(planeId)};
    int reg{BARREL_INDEX};
    if (barrel_ec == ENDCAP_A) reg = ENDCAP_A_INDEX;
    if (barrel_ec == ENDCAP_C) reg = ENDCAP_C_INDEX;
    int element{2 * m_pSCTHelper->layer_disk(planeId) + m_pSCTHelper->side(planeId)};
    int IN{m_ConfigurationTool->isGood(planeId, InDetConditions::SCT_SIDE) ? 0 : 1};
    if (m_pSCTHelper->side(planeId) == 0) {
      if (IN == 1) {
        MOut[GENERAL_INDEX]++;
        MOut[reg]++;
      }
    }
    // danger: no check that barrel_ec is valid
    int y{phi + 1};
    int x{(barrel_ec == BARREL) ? (eta + 7) : (eta + 1)};
    m_p2DmapHistoVectorAll[reg][element]->SetBinContent(x, y, IN);
  }

  // region
  for (int reg{0}; reg<N_REGIONS; reg++) {
    int nLayers{n_layers[reg]};
    for (int lyr{0}; lyr < nLayers; ++lyr) {

      // Check for the number of Modules with more than m_errThreshold error rate
      const unsigned int xbins{m_allErrs[0][reg][lyr]->GetNbinsX() + 1};
      const unsigned int ybins{m_allErrs[0][reg][lyr]->GetNbinsY() + 1};
      for (unsigned int xb{1}; xb < xbins; ++xb) {
        for (unsigned int yb{1}; yb < ybins; ++yb) {
          if (m_allErrsCate[CategoryErrors::SUMMARY][reg][2 * lyr]->GetBinContent(xb,yb) > m_errThreshold or
              m_allErrsCate[CategoryErrors::SUMMARY][reg][2 * lyr + 1]->GetBinContent(xb, yb) > m_errThreshold) {
            ModErr[GENERAL_INDEX]++;
            ModErr[reg]++;
          }
        }
      }

      // noisy modules
      failedbooking = getHisto(lyr, reg, 0, hitsHisto_tmp);
      if (failedbooking == false) {
        countNoisyModules(reg, GENERAL_INDEX, hitsHisto_tmp, m_noiseThreshold, NoisyModules);
      }
      // noisy modules recent
      if (m_environment == AthenaMonManager::online) {
        failedbooking = getHistoRecent(lyr, reg, 0, hitsHisto_tmp);
        if (failedbooking == false) {
          countNoisyModules(reg, GENERAL_INDEX, hitsHisto_tmp, m_noiseThreshold, NoisyModulesRecent);
        }
      }

      // ineff modules
      failedbooking = getHisto(lyr, reg, 1, hitsHisto_tmp);
      if (failedbooking == false) {
        if ((m_environment != AthenaMonManager::online) or 
            thereAreEnoughEntries(hitsHisto_tmp, m_min_stat_ineff_mod) or
            testOffline) {
          const int xbins{hitsHisto_tmp[0]->GetNbinsX() + 1};
          const int ybins{hitsHisto_tmp[0]->GetNbinsY() + 1};
          // if (testOffline) printContents(hitsHisto_tmp[0]);
          for (int xb{1}; xb < xbins; ++xb) {
            for (int yb{1}; yb < ybins; ++yb) {
              float outOfConfig{static_cast<float>(m_p2DmapHistoVectorAll[reg][2 * lyr]->GetBinContent(xb, yb))};
              if (outOfConfig < 1.) { // i.e. its in the configuration
                int nSides{numberOfInefficientSides(hitsHisto_tmp, xb, yb, 1 - m_effThreshold)};
                InEffModules[GENERAL_INDEX] += nSides;
                InEffModules[reg] += nSides;
              }
            }
          }
        }
      }
    }
  }

  if (m_makeConfHisto) {
    for (int reg{0}; reg <= 3; ++reg) {
      m_Conf[reg]->Fill(0., static_cast<double>(MOut[reg]));
      m_ConfRN[reg]->Fill(0., static_cast<double>(MOut[reg]));
      m_ConfOutModules[reg]->Fill(0., static_cast<double>(MOut[reg]));
      m_Conf[reg]->Fill(1., static_cast<double>(Flagged[reg]));
      m_ConfRN[reg]->Fill(1., static_cast<double>(Flagged[reg]));
      m_ConfNew[reg]->Fill(0., static_cast<double>(Flagged[reg]));
      m_Conf[reg]->Fill(2., static_cast<double>(MaskedAllLinks[reg]));
      m_ConfRN[reg]->Fill(2., static_cast<double>(MaskedAllLinks[reg]));
      m_ConfNew[reg]->Fill(1., static_cast<double>(MaskedAllLinks[reg]));
      m_Conf[reg]->Fill(3., static_cast<double>(ModErr[reg]));
      m_ConfRN[reg]->Fill(3., static_cast<double>(ModErr[reg]));
      m_ConfNew[reg]->Fill(2., static_cast<double>(ModErr[reg]));
      m_Conf[reg]->Fill(4., static_cast<double>(InEffModules[reg]));
      m_ConfRN[reg]->Fill(4., static_cast<double>(InEffModules[reg]));
      m_ConfNew[reg]->Fill(3., static_cast<double>(InEffModules[reg]));
      m_Conf[reg]->Fill(5., static_cast<double>(NoisyModules[reg]));
      m_ConfRN[reg]->Fill(5., static_cast<double>(RNoisyModules[reg]));
      m_ConfNew[reg]->Fill(4., static_cast<double>(NoisyModules[reg]));
      if (m_environment == AthenaMonManager::online) {
        m_ConfOnline[reg]->Fill(0., static_cast<double>(MOut[reg]));
        m_ConfOnline[reg]->Fill(1., static_cast<double>(Flagged[reg]));
        m_ConfOnline[reg]->Fill(2., static_cast<double>(MaskedAllLinks[reg]));
        m_ConfOnline[reg]->Fill(3., static_cast<double>(ModErr[reg]));
      }
    }
    if ((m_environment == AthenaMonManager::online) or testOffline) {
      m_ConfEffOnline->Reset("ICE");
      for (int i{0}; i < 4; ++i) {
        const float f{static_cast<float>(i)};
        m_ConfEffOnline->Fill(f, static_cast<double>(InEffModules[i]));
        m_ConfNoiseOnline->Fill(f, static_cast<double>(NoisyModules[i]));
        m_ConfNoiseOnlineRecent->Fill(f, static_cast<double>(NoisyModulesRecent[i]));
      }
    }
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                                   SCTErrMonTool :: fillConfigurationDetails
// Fills Number of Bad Modules / Links / Chips / Strips in to a histogram
// ====================================================================================================
StatusCode
SCTErrMonTool::fillConfigurationDetails() {
  ATH_MSG_DEBUG("Inside fillConfigurationDetails()");
  unsigned int nBadMods{static_cast<unsigned int>(m_ConfigurationTool->badModules()->size())}; // bad modules
  const map<IdentifierHash, pair<bool, bool>>* badLinks{m_ConfigurationTool->badLinks()}; // bad links
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

  const map<Identifier, unsigned int>* badChips{m_ConfigurationTool->badChips()}; // bad chips
  unsigned int nBadChips{0};
  for (const pair<Identifier, unsigned int>& chip : *badChips) {
    unsigned int status{chip.second};
    for (unsigned int i{0}; i < CHIPS_PER_MODULE; i++) {
      nBadChips += ((status & (1 << i)) == 0 ? 0 : 1);
    }
  }

  set<Identifier> badStripsAll; // bad strips
  m_ConfigurationTool->badStrips(badStripsAll);
  unsigned int nBadStrips{static_cast<unsigned int>(badStripsAll.size())};

  set<Identifier> badStripsExclusive; // bad strips w/o bad modules and chips
  m_ConfigurationTool->badStrips(badStripsExclusive, true, true);
  int nBadStripsExclusive{static_cast<int>(badStripsExclusive.size())};
  int nBadStripsExclusiveBEC[N_REGIONS] = {
    0, 0, 0
  };
  for (const Identifier& strip: badStripsExclusive) {
    int bec{m_pSCTHelper->barrel_ec(strip)};
    nBadStripsExclusiveBEC[bec2Index(bec)] += 1;
  }

  m_DetailedConfiguration->Fill(0., static_cast<double>(nBadMods));
  m_DetailedConfiguration->Fill(1., static_cast<double>(nBadLink0));
  m_DetailedConfiguration->Fill(2., static_cast<double>(nBadLink1));
  m_DetailedConfiguration->Fill(3., static_cast<double>(nBadChips));
  m_DetailedConfiguration->Fill(4., static_cast<double>(nBadStripsExclusive) / 100.);

  ATH_MSG_DEBUG("-----------------------------------------------------------------------");
  ATH_MSG_DEBUG("Number of bad modules                          = " << nBadMods);
  ATH_MSG_DEBUG("Number of bad link 0                           = " << nBadLink0);
  ATH_MSG_DEBUG("Number of bad link 1                           = " << nBadLink1);
  ATH_MSG_DEBUG("Number of bad link both                        = " << nBadLinkBoth);
  ATH_MSG_DEBUG("Number of bad chips                            = " << nBadChips);
  ATH_MSG_DEBUG("Number of bad strips                           = " << nBadStrips);
  ATH_MSG_DEBUG("Number of bad strips exclusive                 = " << nBadStripsExclusive);
  ATH_MSG_DEBUG("Number of bad strips exclusive (ECC, B, ECA)   = "
                << nBadStripsExclusiveBEC[0] << ", "
                << nBadStripsExclusiveBEC[1] << ", "
                << nBadStripsExclusiveBEC[2] << ", ");
  ATH_MSG_DEBUG("-----------------------------------------------------------------------");

  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                          SCTErrMonTool :: resetCondDBMaps, Daniel Damiani 25/07/2011
// ====================================================================================================
StatusCode
SCTErrMonTool::resetCondDBMaps() {
  if (m_environment != AthenaMonManager::online) {
    return StatusCode::SUCCESS;
  }
  if (m_makeConfHisto) {
    for (int reg{0}; reg <= 3; ++reg) {
      m_Conf[reg]->Reset();
      m_ConfRN[reg]->Reset();
      m_ConfOnline[reg]->Reset();
    }
    m_ConfEffOnline->Reset();
    m_ConfNoiseOnline->Reset();
    m_ConfNoiseOnlineRecent->Reset();
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                          SCTErrMonTool :: resetConfigurationDetails, Daniel Damiani 25/07/2011
// ====================================================================================================
StatusCode
SCTErrMonTool::resetConfigurationDetails() {
  if (m_environment != AthenaMonManager::online) {
    return StatusCode::SUCCESS;
  }
  if (m_makeConfHisto) {
    m_DetailedConfiguration->Reset();
  }
  return StatusCode::SUCCESS;
}

// ====================================================================================================
//                          SCTErrMonTool :: getHisto, Martin Flechl 14/9/2009
// ====================================================================================================
bool
SCTErrMonTool::getHisto(const int layer, const int reg, const int type, TH2* histo[2]) const {
  static const string trm[2][N_REGIONS] = { // 2 is the number of types (noise, eff)
    {"SCT/SCTEC/Noise/noiseoccupancymapECm_","SCT/SCTB/Noise/noiseoccupancymap_", "SCT/SCTEA/Noise/noiseoccupancymapECp_"},
    {"SCT/SCTEC/eff/ineffm_", "SCT/SCTB/eff/ineff_", "SCT/SCTEA/eff/ineffp_"}
  };
  static const string trm_trig[2][N_REGIONS] = { // 2 is the number of types (noise, eff)
    {"SCT/SCTEC/Noise/noiseoccupancymaptriggerECm_", "SCT/SCTB/Noise/noiseoccupancymaptrigger_", "SCT/SCTEA/Noise/noiseoccupancymaptriggerECp_"},
    {"SCT/SCTEC/eff/ineffm_", "SCT/SCTB/eff/ineff_", "SCT/SCTEA/eff/ineffp_"}
  };
  string shname1{m_path + trm[type][reg] + to_string(layer)};
  string shname2{m_path + trm[type][reg] + to_string(layer)};

  if (testOffline or ((m_environment != AthenaMonManager::online) and
                      (AthenaMonManager::dataType() != AthenaMonManager::cosmics))) {
    shname1 = m_path + trm_trig[type][reg] + to_string(layer);
    shname2 = m_path + trm_trig[type][reg] + to_string(layer);
  }
  if ((type==0) or (type==1)) {
    shname1 += "_0";
    shname2 += "_1";
  }
  if (type==2) {
    shname2 = "";
  }
  bool failedBooking{false};
  histo[0]=0;
  histo[1]=0;
  if (shname1!="") failedBooking |= m_thistSvc->getHist(shname1, histo[0]).isFailure();
  if (shname2!="") failedBooking |= m_thistSvc->getHist(shname2, histo[1]).isFailure();
  if (failedBooking) {
    ATH_MSG_WARNING("Error getting Histogram from ITHistSvc : " << shname1 << "," << shname2);
    return true;
  }
  return false;
}

// ====================================================================================================
//                          SCTErrMonTool :: getHistoRecent, Dan Damiani  21/7/2011
// ====================================================================================================
bool
SCTErrMonTool::getHistoRecent(const int layer, const int reg, const int type, TH2* histo[2]) const {
  static const string trm[1][N_REGIONS] = {
    {"SCT/SCTEC/Noise/noiseoccupancymaprecentECm_", "SCT/SCTB/Noise/noiseoccupancymaprecent_", "SCT/SCTEA/Noise/noiseoccupancymaprecentECp_"}
  };
  string shname1{m_path + trm[type][reg] + to_string(layer) + "_0"};
  string shname2{m_path + trm[type][reg] + to_string(layer) + "_1"};
  bool failedBooking{false};

  failedBooking |= m_thistSvc->getHist(shname1, histo[0]).isFailure();
  failedBooking |= m_thistSvc->getHist(shname2, histo[1]).isFailure();
  if (failedBooking) {
    ATH_MSG_WARNING("Error getting Histogram from ITHistSvc : " << shname1 << "," << shname2);
    return true;
  }
  return false;
}

//====================================================================================================
//                          SCTErrMonTool :: fillWafer, Keisuke Kouda 12.09.2016
//====================================================================================================
void SCTErrMonTool::fillWafer(moduleGeo_t module, TH2F* histo) const {
  double etaMin{module.first.first}, etaMax{module.first.second};
  double phiMin{module.second.first}, phiMax{module.second.second};
  unsigned int nRep{1};
  if (etaMin<-2.5) { etaMin = -2.5; }
  if (etaMax>2.5) { etaMax = 2.5; }
  if (phiMin>phiMax) {
    phiMin = -M_PI;
    nRep=2;
  }
  for (unsigned int iRep{0}; iRep<nRep; iRep++) {
    if (iRep==1) {
      phiMin = module.second.first;
      phiMax = M_PI;
    }
    const int ixMin{static_cast<int>((etaMin/m_rangeEta+1.)*m_nBinsEta/2)+1};
    const int ixMax{static_cast<int>((etaMax/m_rangeEta+1.)*m_nBinsEta/2)};
    const int iyMin{static_cast<int>((phiMin/M_PI+1.)*m_nBinsPhi/2)+1};
    const int iyMax{static_cast<int>((phiMax/M_PI+1.)*m_nBinsPhi/2)};
    const double xMin{(static_cast<double>(ixMin)/m_nBinsEta*2-1.)*m_rangeEta};
    const double xMax{(static_cast<double>(ixMax)/m_nBinsEta*2-1.)*m_rangeEta};
    const double yMin{(static_cast<double>(iyMin)/m_nBinsPhi*2-1.)*M_PI};
    const double yMax{(static_cast<double>(iyMax)/m_nBinsPhi*2-1.)*M_PI};
    const double wxMin{(xMin-etaMin)/m_rangeEta*m_nBinsEta/2};
    const double wxMax{(etaMax-xMax)/m_rangeEta*m_nBinsEta/2};
    const double wxOne{(etaMax-etaMin)/m_rangeEta*m_nBinsEta/2};
    const double wyMin{(yMin-phiMin)/M_PI*m_nBinsPhi/2};
    const double wyMax{(phiMax-yMax)/M_PI*m_nBinsPhi/2};
    const double wyOne{(phiMax-phiMin)/M_PI*m_nBinsPhi/2};
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
        histo->SetBinContent(ix, iy, histo->GetBinContent(ix,iy)+weight); //Fill(ix, iy, weight);//
      }
    }
  }
  return;
}

//====================================================================================================
//                          SCTErrMonTool :: SyncSCT, Keisuke Kouda 12.09.2016
//====================================================================================================
bool SCTErrMonTool::syncErrorSCT(set<IdentifierHash>& sctHashBadLinkError,
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
    for (const IdentifierHash& waferHash: sctErrors) {
      sctHashBadRODError.insert(waferHash);
    }
  }

  //BadError = BadLinkLevelError + BadRODLevelError
  for (SCT_ByteStreamErrors::errorTypes tmpBadError: SCT_ByteStreamErrors::BadErrors) {
    const set<IdentifierHash> sctErrors{m_byteStreamErrTool->getErrorSet( tmpBadError )};
    for (const IdentifierHash& waferHash: sctErrors) {
      sctHashBadError.insert(waferHash);
    }
  }
  return true;
}

//Disabled
bool SCTErrMonTool::syncDisabledSCT(set<IdentifierHash>& sctHashDisabled) const {
  bool altered{false};
  sctHashDisabled.clear();
  const set<Identifier>* badModules{m_ConfigurationTool->badModules()};

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

//Total (SCT_ConditionsSummaryTool) //All
bool SCTErrMonTool::summarySCT(set<IdentifierHash>& sctHashAll, set<IdentifierHash>& sctHashSummary) const  {
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

//Power supply trip (SCT_DCSConditionsTool)
bool SCTErrMonTool::psTripDCSSCT(set<IdentifierHash>& sctHashPSTripDCS, float& PSTripModules) const {
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
  PSTripModules = npsw/2.;
  return altered;
}

//====================================================================================================
//                          SCTErrMonTool :: calculateDetectorCoverage, Keisuke Kouda 12.09.2016
//====================================================================================================
double SCTErrMonTool::calculateDetectorCoverage( const TH2F* histo ) const {
  double occupancy{0.};

  for (unsigned int i{0}; i < m_nBinsEta; i++) {
    for (unsigned int j{0}; j < m_nBinsPhi; j++) {
      double waferCell{m_mapSCT[all]->GetBinContent(i+1, j+1) - histo->GetBinContent(i+1, j+1)};

      if (waferCell >= m_WafersThreshold) {
        occupancy += 1.0;
      } else if (waferCell > m_WafersThreshold - 1.0 ) {
        occupancy += waferCell - (m_WafersThreshold - 1.0);
        //Calculating the bin occupancy which has less than 1. 
        //For example, bin have a 2.3. In this case, we can understand that 30% of the bin is coverd by 3 sides/wafers and 70% of the bin is coverd by 2 sides/wafers.
        //And it means that occupancy of the bin is 0.3 So, in this line, I take difference between m_WafersThreshold(3)-1 and waferCell, and add it to the occupancy.
      }
    }
  }
  double detector_coverage{100. * static_cast<double>( occupancy )/( static_cast<double>( m_nBinsEta ) * static_cast<double>( m_nBinsPhi ) )};
  return detector_coverage;
}
