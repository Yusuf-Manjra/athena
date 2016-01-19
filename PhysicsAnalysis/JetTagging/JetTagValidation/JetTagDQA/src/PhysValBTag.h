///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhysValBTag.h
// Header file for class PhysValBTag
// Author: E.Schopf<elisabeth.schopf@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef JETTAGDQA_PHYSVALBTag_H
#define JETTAGDQA_PHYSVALBTag_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TrkValHistUtils/PlotBase.h"
#include "BTaggingValidationPlots.h"

// Root includes
#include "TH1.h"

// Forward declaration

namespace JetTagDQA {

  class PhysValBTag
    : public ManagedMonitorToolBase
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    // Copy constructor:

    /// Constructor with parameters:
    PhysValBTag( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

    /// Destructor:
    virtual ~PhysValBTag();

    // Athena algtool's Hooks
    virtual StatusCode initialize();
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();


    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
  private:

    /// Default constructor:
    PhysValBTag();

    // isData flag
    bool m_isData;

    // Containers
    std::string m_jetName1;
    std::string m_jetName2;
    //  std::string m_jetName3;
    std::string m_jetName4;
    std::string m_jetName5;
    std::string m_jetName7;
    std::string m_jetName6;
    std::string m_jetName8;
    std::string m_jetName9;

    std::string m_trackName;
    std::string m_vertexName;

    // Hists
    //  JetTagDQA::BTaggingValidationPlots m_jetPlots;
    //  PhysVal::TrkAndVtxPlots m_trkvtxPlots;

    std::map<std::string, JetTagDQA::BTaggingValidationPlots> m_btagplots;
    JetTagDQA::BTaggingValidationPlots m_antiKt4EMTopoPlots;
    JetTagDQA::BTaggingValidationPlots m_antiKt4LCTopoPlots;
    //  JetTagDQA::BTaggingValidationPlots m_antiKt4TruthPlots;
    JetTagDQA::BTaggingValidationPlots m_antiKt4TruthWZPlots;
    JetTagDQA::BTaggingValidationPlots m_antiKt10LCTopoPlots;
    //  JetTagDQA::BTaggingValidationPlots m_antiKt10TruthPlots;
    JetTagDQA::BTaggingValidationPlots m_antiKt10TruthWZPlots;
    JetTagDQA::BTaggingValidationPlots m_antiKt2PV0TrackJetPlots;
    JetTagDQA::BTaggingValidationPlots m_antiKt3PV0TrackJetPlots;
    JetTagDQA::BTaggingValidationPlots m_antiKt4PV0TrackJetPlots;

    int m_nevents;
    int m_nTruthB;
    int m_nTruthNonB;

    StatusCode book(PlotBase& plots);
  };

  // I/O operators
  //////////////////////

  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////
}

#endif //> !JETTAGDQA_PHYSVALBTag_H
