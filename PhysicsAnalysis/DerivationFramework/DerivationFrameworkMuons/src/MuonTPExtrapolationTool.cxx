/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// MuonTPExtrapolationTool.cxx
#include "DerivationFrameworkMuons/MuonTPExtrapolationTool.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
#ifndef XAOD_ANALYSIS
  #include "TrkSurfaces/DiscSurface.h"
  #include "TrkSurfaces/CylinderSurface.h"
  #include "TrkExInterfaces/IExtrapolator.h"
#endif
#include "TVector2.h"

//**********************************************************************

MuonTPExtrapolationTool::MuonTPExtrapolationTool(std::string myname) :
            AsgTool(myname),
#ifndef XAOD_ANALYSIS
        m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
#endif
         m_endcapPivotPlaneZ(15525.),
         m_endcapPivotPlaneMinimumRadius(0.),
         m_endcapPivotPlaneMaximumRadius(11977),
         m_barrelPivotPlaneRadius(8000.),
         m_barrelPivotPlaneHalfLength(9700),
         m_is_on_DAOD(false) {
    declareProperty("EndcapPivotPlaneZ", m_endcapPivotPlaneZ = 15525.); // z position of pivot plane in endcap region
    declareProperty("EndcapPivotPlaneMinimumRadius", m_endcapPivotPlaneMinimumRadius = 0.); // minimum radius of pivot plane in endcap region
    declareProperty("EndcapPivotPlaneMaximumRadius", m_endcapPivotPlaneMaximumRadius = 11977.); // maximum radius of pivot plane in endcap region
    declareProperty("BarrelPivotPlaneRadius", m_barrelPivotPlaneRadius = 8000.); // radius of pivot plane in barrel region
    declareProperty("BarrelPivotPlaneHalfLength", m_barrelPivotPlaneHalfLength = 9700.); // half length of pivot plane in barrel region
#ifndef XAOD_ANALYSIS
    declareProperty("Extrapolator", m_extrapolator);
#endif
    declareProperty("IsRunOnDAOD", m_is_on_DAOD);
}

MuonTPExtrapolationTool::~MuonTPExtrapolationTool() {
}

//**********************************************************************

StatusCode MuonTPExtrapolationTool::initialize() {
    if (!m_is_on_DAOD) ATH_CHECK(m_extrapolator.retrieve());

    return StatusCode::SUCCESS;
}

//**********************************************************************

bool MuonTPExtrapolationTool::extrapolateAndDecorateTrackParticle(const xAOD::TrackParticle* particle, float & eta, float & phi) const {

    // decorators used to access or store the information
    static SG::AuxElement::Decorator<char> dec_Decorated("DecoratedPivotEtaPhi");
    static SG::AuxElement::Decorator<float> dec_Eta("EtaTriggerPivot");
    static SG::AuxElement::Decorator<float> dec_Phi("PhiTriggerPivot");

    static SG::AuxElement::Accessor<char> acc_Decorated("DecoratedPivotEtaPhi");
    static SG::AuxElement::Accessor<float> acc_Eta("EtaTriggerPivot");
    static SG::AuxElement::Accessor<float> acc_Phi("PhiTriggerPivot");
    
    if (!acc_Decorated.isAvailable(*particle) || !acc_Decorated(*particle)) {
        if (!m_is_on_DAOD) {
            // in the athena release, we can run the extrapolation if needed
            const Trk::TrackParameters* pTag = extrapolateToTriggerPivotPlane(*particle);
            if (!pTag) {
                // complain only if the particle has sufficient pt to actually make it to the MS...
                if (particle->pt() > 3500) ATH_MSG_WARNING("Warning - Pivot plane extrapolation failed for a track particle with IP pt " << particle->pt() << ", eta " << particle->eta() << ", phi " << particle->phi());
                dec_Decorated(*particle) = 2;
                return false;
            }
            dec_Eta(*particle) = pTag->position().eta();
            dec_Phi(*particle) = pTag->position().phi();
            dec_Decorated(*particle) = 1;
            delete pTag;
        }
        // if running in Athena on DAOD then the decorations should be there
        // it could be missing for low pT tracks, not used for final TP analysis so go ahead
        else {
            if (particle->pt() > 3500) ATH_MSG_WARNING("Pivot plane extrapolation not decorated to a track particle at pt " << particle->pt() << ", eta " << particle->eta() << ", phi " << particle->phi());
            return true;
        }
    }
    
    if (m_is_on_DAOD || (acc_Decorated.isAvailable(*particle) && acc_Decorated(*particle) == 1) ) {
        eta = acc_Eta(*particle);
        phi = acc_Phi(*particle);
    } else {
        if (!m_is_on_DAOD && particle->pt() > 3500) ATH_MSG_WARNING("Warning - Pivot plane extrapolation failed for track with pt " << particle->pt() << ", eta " << particle->eta() << ", phi " << particle->phi());
        eta = phi = -5;
    } 
    
    return true;
}

//**********************************************************************

const xAOD::TrackParticle* MuonTPExtrapolationTool::getPreferredTrackParticle(const xAOD::IParticle* probe) const {
    const xAOD::TrackParticle* probeTrack = nullptr;
    if (probe->type() == xAOD::Type::ObjectType::TruthParticle){
        ATH_MSG_WARNING("Pivot plane extrapolation not supported for Truth probes!");
    } else if (probe->type() == xAOD::Type::ObjectType::TrackParticle){
        probeTrack = dynamic_cast<const xAOD::TrackParticle*>(probe);
    } else if (probe->type() == xAOD::Type::ObjectType::Muon) {
        const xAOD::Muon* probeMuon = dynamic_cast<const xAOD::Muon*>(probe);
        probeTrack = probeMuon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
        if (!probeTrack) {
            probeTrack = probeMuon->primaryTrackParticle();
            if (!probeTrack) {
                probeTrack = probeMuon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
            }
        }
    }
    if (!probeTrack) {
        ATH_MSG_WARNING("no valid track found for extrapolating the probe to the pivot plane!");
    }
    return probeTrack;

}
// **********************************************************************
StatusCode MuonTPExtrapolationTool::decoratePivotPlaneCoords(const xAOD::IParticle* particle) {

    const xAOD::TrackParticle* track = getPreferredTrackParticle(particle);
    float eta, phi = 0;
    if (!extrapolateAndDecorateTrackParticle(track, eta, phi)) {
        return StatusCode::FAILURE;
    }        
    return StatusCode::SUCCESS;
}

// **********************************************************************

double MuonTPExtrapolationTool::dROnTriggerPivotPlane(const xAOD::Muon& tag, const xAOD::IParticle* probe) const {
// should never happen, but better be safe than sorry
    if (!probe) {
        ATH_MSG_WARNING("No valid probe provided to MuonTPExtrapolationTool::dROnTriggerPivotPlane! ");
        return 0;
    }
    const xAOD::EventInfo* info = 0;
    if (evtStore()->retrieve(info, "EventInfo").isFailure()) {
        ATH_MSG_FATAL("Unable to retrieve Event Info");
    }
    int run = info->runNumber();
    int evt = info->eventNumber();

    // starting values: Track direction @ IP
    float tag_eta = tag.eta();
    float tag_phi = tag.phi();
    float probe_eta = probe->eta();
    float probe_phi = probe->phi();

    // Try to replace the starting values by the extrapolated pivot plane coordinates
    //
    // First, we need to pick the TrackParticle to extrapolate.
    const xAOD::TrackParticle* tagTrack = getPreferredTrackParticle(&tag);
    if (tagTrack) {
        // If we have the track particle, we load the existing decorations for the
        // pivot plane eta/phi, or (in full athena) to run the extrapolation and decorate.
        // If the method returns true, we know that eta/phi were successfully updated
        // If this does not work, the method returns a false.
        // In that case, eta/phi are not changed and we are stuck with the IP direction.
        // Print a warning just to let the user know.
        // std::cout << "     ===> Checking the tag muon, with type "<<tag.muonType()<<" and author "<<tag.author() << std::endl;
        if (!extrapolateAndDecorateTrackParticle(tagTrack, tag_eta, tag_phi)) {
            ATH_MSG_WARNING("Could not get the tag eta/phi @ pivot plane in event " << evt << " / run " << run << " , using direction at IP!");
        }
    }

// Now we repeat exactly the same exercise as above for the probe

// Special flag in case an ID track doesn't reach the MS, causing the extrap to fail.
// In this case, we fill an unphysical dummy value of -1 for the delta R, and -10 each eta / phi / dphi / deta.
// In the selection code, we accept these probes, since they can not cause a trigger-related bias
    bool IDProbeMissesMS = false;

    const xAOD::TrackParticle* probeTrack = getPreferredTrackParticle(probe);
    // std::cout << "     ===> Checking the probe"<< std::endl;
    if (probeTrack) {
        if (!extrapolateAndDecorateTrackParticle(probeTrack, probe_eta, probe_phi)) {
            // if an ID probe does not make it into the MS, we kick it out by returning an unphysical delta R of -100.
            if (dynamic_cast<const xAOD::TrackParticle*>(probe)) {
                ATH_MSG_DEBUG("ID probe does not reach MS, returning unphysical dummy value for the exTP branches!");
                IDProbeMissesMS = true;
            } else {
                const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(probe);
                ATH_MSG_DEBUG("Could not get the muon probe eta/phi @ pivot plane, muon type and author are " << mu->muonType() << " and " << mu->author());
            }
        }
    }

// here we compute the angle in eta and phi
    float deta = tag_eta - probe_eta;
    float dphi = TVector2::Phi_mpi_pi(tag_phi - probe_phi);
    float dr = sqrt(deta * deta + dphi * dphi);

// decorators for the delta eta and delta phi and dR
    static SG::AuxElement::Decorator<float> DecDeta("probe_dEtaTriggerPivot");
    static SG::AuxElement::Decorator<float> DecDphi("probe_dPhiTriggerPivot");
    static SG::AuxElement::Decorator<float> DecDR("probe_dRTriggerPivot");

// here, we copy the eta and phi decorations from the TrackParticles to the
// tag and probe IParticles - to make them easier to access during the
// further processing
// In the case of an unsuccessful extrap, this additionally ensures that the
// IP direction for eta/phi is written to the output tree
// instead of a garbage value that would result from a missing decoration
    static SG::AuxElement::Decorator<float> DecEta("probe_EtaTriggerPivot");
    static SG::AuxElement::Decorator<float> DecPhi("probe_PhiTriggerPivot");

// special unphysical values to signify an ID probe not reaching the pivot plane
    if (IDProbeMissesMS) {
        deta = dphi = probe_phi = probe_eta = -10;
        dr = -1;
    }

// note that here we only decorate the tag muon with the properties of the TP pair.
// This has the background that, for ID probes, the probe may be a non-const TrackParticle
// with a locked AuxStore.
// If we attempted to overwrite an existing (e.g from DAOD) decoration, we would not be able to
// and get an exception. The tag, being a shallow copy of a muon, is fine however and suited for our purpose
    DecDeta(tag) = deta;
    DecDphi(tag) = dphi;
    DecDR(tag) = dr;

// here, unlike the above, we can get away with simply ignoring the exception if the store is locked
// since the eta and the phi in the original decoration continue to be valid.
    try {
        DecEta(tag) = tag_eta;
        DecPhi(tag) = tag_phi;
        DecEta(*probe) = probe_eta;
        DecPhi(*probe) = probe_phi;
    } catch (SG::ExcStoreLocked &) {
        // Maintain a nice, passive agressive silence here.
    }

// the final variable we care about is the delta R.
    return dr;
}
//**********************************************************************

#ifndef XAOD_ANALYSIS
const Trk::TrackParameters* MuonTPExtrapolationTool::extrapolateToTriggerPivotPlane(const xAOD::TrackParticle& track) const {
// BARREL
    const Trk::Perigee& perigee = track.perigeeParameters();

// create the barrel as a cylinder surface centered at 0,0,0
    Amg::Vector3D barrelCentre(0., 0., 0.);
    Amg::Transform3D* matrix = new Amg::Transform3D(Amg::RotationMatrix3D::Identity(), barrelCentre);

    Trk::CylinderSurface* cylinder = new Trk::CylinderSurface(matrix, m_barrelPivotPlaneRadius, m_barrelPivotPlaneHalfLength);
    if (!cylinder) {
        ATH_MSG_WARNING("extrapolateToTriggerPivotPlane :: new Trk::CylinderSurface failed.");
        delete matrix;
        matrix = 0;
        return 0;
    }
// and then attempt to extrapolate our track to this surface, checking for the boundaries of the barrel
    bool boundaryCheck = true;
    const Trk::Surface* surface = cylinder;
    const Trk::TrackParameters* p = m_extrapolator->extrapolate(perigee, *surface, Trk::alongMomentum, boundaryCheck, Trk::muon);
    delete cylinder;
// if the extrapolation worked out (so we are in the barrel) we are done and can return the
// track parameters at this surface.
    if (p) return p;

// if we get here, the muon did not cross the barrel surface
// so we assume it is going into the endcap.
// ENDCAP

// After 2 years of using this code, we realised that ATLAS actually has endcaps on both sides ;-)
// So better make sure we place our endcap at the correct side of the detector!
// Hopefully no-one will ever read this comment...
    float SignOfEta = track.eta() > 0 ? 1. : -1.;

    Amg::Vector3D endcapCentre(0., 0., m_endcapPivotPlaneZ);
// much better!
    matrix = new Amg::Transform3D(Amg::RotationMatrix3D::Identity(), SignOfEta * endcapCentre);

    Trk::DiscSurface* disc = new Trk::DiscSurface(matrix, m_endcapPivotPlaneMinimumRadius, m_endcapPivotPlaneMaximumRadius);
    if (!disc) {
        ATH_MSG_WARNING("extrapolateToTriggerPivotPlane :: new Trk::DiscSurface failed.");
        delete matrix;
        matrix = 0;
        return 0;
    }

// for the endcap, we turn off the boundary check, extending the EC infinitely to catch stuff heading for the transition region
    boundaryCheck = false;
    surface = disc;
    p = m_extrapolator->extrapolate(perigee, *surface, Trk::alongMomentum, boundaryCheck, Trk::muon);
    delete disc;
    return p;
}
#endif
