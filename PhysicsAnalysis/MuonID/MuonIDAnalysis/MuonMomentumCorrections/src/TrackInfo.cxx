#define __TrackInfo_cxx__
#include "TrackInfo.h"

TrackInfo::TrackInfo(std::string type) {
    m_Type = type;
    Reset();
}

void TrackInfo::Reset() {
    m_Pt = -999.;
    m_CalibPt = -999.;
    m_Eta = -999.;
    m_Phi = -999.;
    m_QoverP = -999.;
    m_Chi2 = -999.;
    m_Charge = -999;
    m_NDoF = 0;
    TrackPars.clear();
    for (unsigned int i = 0; i < 5; i++) TrackPars.push_back(0.);
    TrackCovMatrix.clear();
    for (unsigned int i = 0; i < 5; i++)
        for (unsigned int j = 0; j < 5; j++) TrackCovMatrix.push_back(0.);
}

void TrackInfo::Register(TTree* t) {
    t->Branch(("Muon_" + m_Type + "_Pt").c_str(), &m_Pt);
    t->Branch(("Muon_" + m_Type + "_CalibPt").c_str(), &m_CalibPt);
    t->Branch(("Muon_" + m_Type + "_Eta").c_str(), &m_Eta);
    t->Branch(("Muon_" + m_Type + "_Phi").c_str(), &m_Phi);
    t->Branch(("Muon_" + m_Type + "_Charge").c_str(), &m_Charge);
    t->Branch(("Muon_" + m_Type + "_QoverP").c_str(), &m_QoverP);
    t->Branch(("Muon_" + m_Type + "_Chi2").c_str(), &m_Chi2);
    t->Branch(("Muon_" + m_Type + "_NDoF").c_str(), &m_NDoF);
    t->Branch(("Muon_" + m_Type + "_TrackPars").c_str(), &TrackPars);
    t->Branch(("Muon_" + m_Type + "_TrackCovMatrix").c_str(), &TrackPars);
}

void TrackInfo::Fill(const xAOD::TrackParticle* tp) {
    m_Pt = tp->pt() / CLHEP::GeV;
    m_Eta = tp->eta();
    m_Phi = tp->phi();
    m_Charge = (int)tp->charge();
    m_QoverP = tp->qOverP() * CLHEP::GeV;
    m_Chi2 = tp->chiSquared();
    m_NDoF = (int)tp->numberDoF();
    AmgVector(5) pars = tp->definingParameters();
    AmgSymMatrix(5) cov = tp->definingParametersCovMatrix();

    TrackPars.clear();
    for (unsigned int i = 0; i < 5; i++) TrackPars.push_back(pars[i]);
    TrackCovMatrix.clear();

    for (unsigned int i = 0; i < 5; i++)
        for (unsigned int j = 0; j < 5; j++) TrackCovMatrix.push_back(cov(i, j));
}

TLorentzVector TrackInfo::GetFourMomentum(bool calib) {
    TLorentzVector result;
    if(calib) result.SetPtEtaPhiM(m_CalibPt, m_Eta, m_Phi, 0.105658);
    else result.SetPtEtaPhiM(m_Pt, m_Eta, m_Phi, 0.105658);
    return result;
}
