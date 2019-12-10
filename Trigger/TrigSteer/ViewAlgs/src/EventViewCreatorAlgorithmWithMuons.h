/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_EventViewCreatorAlgorithmWithMuons_h
#define ViewAlgs_EventViewCreatorAlgorithmWithMuons_h

#include "EventViewCreatorAlgorithm.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"

 /**
   * @class EventViewCreatorAlgorithmWithMuons
   * @brief Used at the start of a sequence to create the EventViews: retrieves filtered collection via menu decision from previous step and writes it out directly so it can be used as input by the reco alg that follows in sequence.
   **/

class EventViewCreatorAlgorithmWithMuons : public EventViewCreatorAlgorithm {
 public:
    EventViewCreatorAlgorithmWithMuons( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~EventViewCreatorAlgorithmWithMuons();

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext&) const override;

 private:

    StatusCode placeMuonInView( const xAOD::Muon* theObject,
			       SG::View* view,
			       const EventContext& context ) const;

    EventViewCreatorAlgorithmWithMuons();

    SG::WriteHandleKey< ConstDataVector<xAOD::MuonContainer> > m_inViewMuons {this,"InViewMuons","Unspecified",
      "Name with which the Muons should be inserted into the views"};
    SG::WriteHandleKey< ConstDataVector<MuonCandidateCollection> > m_inViewMuonCandidates {this,"InViewMuonCandidates","Unspecified",
      "Name with which the Muon Candidates should be inserted into the views"};
    SG::WriteHandleKey< TrigRoiDescriptorCollection > m_roisWriteHandleKey {this,"RoisWriteHandleKey","Unspecified",
      "Name of the global-scope collection used to persistify the new ROIDescriptor(s) created around the muon candidate(s)"};


    Gaudi::Property< std::string > m_muonsLink {this,"MuonsLink","Unspecified","Name of EL to Muon object linked to the decision"};
    Gaudi::Property< std::string > m_lateRoIsLink {this,"LateRoIsLink","Unspecified","Name of EL to late roi object linked to the decision"};
    Gaudi::Property<bool> m_doFSRoI {this, "CreateFSRoI", false, "Make Full Scan RoI"};
    Gaudi::Property<bool> m_doLateMu {this, "DoLateMu", false, "Use Out-Of-Time Muon RoIs"};
    double m_roiEtaWidth;
    double m_roiPhiWidth;
};

#endif

