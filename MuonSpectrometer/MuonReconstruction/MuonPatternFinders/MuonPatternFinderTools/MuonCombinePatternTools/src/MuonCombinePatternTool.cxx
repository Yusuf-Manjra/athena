/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinePatternTools/MuonCombinePatternTool.h"



#include "CxxUtils/sincos.h"
#include "MuonHoughPatternEvent/MuonHoughPattern.h"
#include "MuonPattern/MuonPatternChamberIntersect.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"  
#include <cmath> //std::abs, M_PI etc
#include <memory> //std::unique_ptr
#include <iterator>

namespace{
double rotatePhi(double phi, double rotationFraction) {
    // check whether we rotate to a large value than pi, if so add additional
    // rotation by -2pi
    if (phi + rotationFraction * M_PI > M_PI) return phi + (rotationFraction - 2.) * M_PI;
    return phi + rotationFraction * M_PI;
}
}
MuonCombinePatternTool::MuonCombinePatternTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_maximum_xydistance(3500),
    m_maximum_rzdistance(1500),
    m_use_cosmics(false),
    m_splitpatterns(true),
    m_nodiscarding(true),
    m_bestphimatch(false),
    m_flipdirectionforcosmics(false) {
    declareInterface<IMuonCombinePatternTool>(this);
    declareProperty("UseCosmics", m_use_cosmics);
    declareProperty("SplitPatterns", m_splitpatterns);
    declareProperty("NoDiscarding", m_nodiscarding);
    declareProperty("BestPhiMatch", m_bestphimatch);
    declareProperty("FlipDirectionForCosmics", m_flipdirectionforcosmics);
    declareProperty("UseTightAssociation", m_useTightAssociation = false);
    declareProperty("MaxSizePhiPatternLooseCuts", m_maxSizePhiPatternLoose = 40);
    declareProperty("MaxSizeEtaPatternLooseCuts", m_maxSizeEtaPatternLoose = 200);
}

StatusCode MuonCombinePatternTool::initialize() {
    ATH_MSG_DEBUG("MuonCombinePatternTool::initialize");
    ATH_CHECK(m_idHelperSvc.retrieve());
    if (!m_use_cosmics) { m_splitpatterns = false; }
    if (m_use_cosmics) { m_bestphimatch = true; }
    ATH_MSG_DEBUG(" UseCosmics: " << m_use_cosmics << " Split Patterns: " << m_splitpatterns << " NoDiscarding: " << m_nodiscarding
                                  << " BestPhiMatch: " << m_bestphimatch);
    return StatusCode::SUCCESS;
}

MuonPrdPatternCollection* MuonCombinePatternTool::combineEtaPhiPatterns(
    const MuonPrdPatternCollection* phiPatternCollection, const MuonPrdPatternCollection* etaPatternCollection,
    /** phi eta association map, eta prds are key*/
    const std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>* phiEtaHitAssMap) const {
    bool myDebug = false;
    std::vector<std::unique_ptr<const Muon::MuonPrdPattern>> patternsToDelete;
    std::vector<std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*>>
        candidates;  // vector of etapatterns (key) and phipatterns (value), that are
                     // candidates for combining. Both etapatterns and phipatterns
                     // can occur multiple times

    if (m_use_cosmics && m_splitpatterns) {
        patternsToDelete.reserve(6 * etaPatternCollection->size());  // split eta (2) and phi patterns (2) + associated (2) ,
                                                                     // might be more when more than one match is found
    } else {
        patternsToDelete.reserve(etaPatternCollection->size());  // only associated patterns
    }
    // strategy
    // associate eta pattern to phi patterns
    // are phi hits on a eta and eta hits on phi pattern?

    // some printout
    ATH_MSG_DEBUG(" combineEtaPhiPatterns:  eta patterns: " << etaPatternCollection->size()
                                                            << "  phi patterns: " << phiPatternCollection->size());

    for (unsigned int etalevel = 0; etalevel < etaPatternCollection->size(); etalevel++) {
        const Muon::MuonPrdPattern* etapattern = etaPatternCollection->at(etalevel);
        if (etapattern->numberOfContainedPrds() == 0) continue;

        double theta = etapattern->globalDirection().theta();
        const double phieta = etapattern->globalDirection().phi();
        CxxUtils::sincos scphieta(phieta);
        CxxUtils::sincos sctheta(theta);
        double dx_eta = scphieta.cs * sctheta.sn;
        double dy_eta = scphieta.sn * sctheta.sn;
        double dz_eta = sctheta.cs;
        double z0 = etapattern->globalPosition().z();  // 0 for non -cosmics
        double rz0 = z0 * sctheta.sn;                  // closest distance in rz

        double eta_x = etapattern->globalPosition().x();
        double eta_y = etapattern->globalPosition().y();
        double pattern_z0 =
            z0 + std::sqrt(eta_x * eta_x + eta_y * eta_y) * sctheta.cs / sctheta.sn;  // z belonging to (x0,y0) -> noncosmics just close to
                                                                                      // 0 (0.001 * sin/cos)
        double eta_r0 = MuonHoughMathUtils::signedDistanceOfLineToOrigin2D(eta_x, eta_y, phieta);
        double charge = 1.;
        if (eta_r0 < 0) charge = -1.;
        //           Get inverse curvature from eta pattern
        double curvature = etapattern->globalDirection().mag();
        double invcurvature = 0.;
        if (curvature > 2) invcurvature = charge / curvature;

        ATH_MSG_DEBUG(" eta pattern info level: " << etalevel << " phi " << phieta << " theta " << theta << " x0 " << eta_x << " y0 "
                                                  << eta_y << " z0 " << z0 << " hits " << etapattern->numberOfContainedPrds());
        // flags for cosmics:
        double min_average_distance = m_maximum_xydistance + m_maximum_rzdistance;
        const Muon::MuonPrdPattern* max_phipattern = nullptr;
        int max_philevel = -1;
        // flags
        double dotprodbest = -1.;
        int phibest = -1;
        bool ismatched = false;
        for (unsigned int philevel = 0; philevel < phiPatternCollection->size(); philevel++) {
            const Muon::MuonPrdPattern* phipattern = phiPatternCollection->at(philevel);
            if (phipattern->numberOfContainedPrds() == 0) continue;
            bool useTightAssociation = false;

            if (m_useTightAssociation && (phipattern->numberOfContainedPrds() > m_maxSizePhiPatternLoose ||
                                          etapattern->numberOfContainedPrds() > m_maxSizeEtaPatternLoose)) {
                if (phipattern->numberOfContainedPrds() > m_maxSizePhiPatternLoose)
                    ATH_MSG_DEBUG(" using tight cuts due to phi hits " << phipattern->numberOfContainedPrds() << " cut "
                                                                       << m_maxSizePhiPatternLoose);
                if (etapattern->numberOfContainedPrds() > m_maxSizeEtaPatternLoose)
                    ATH_MSG_DEBUG(" using tight cuts due to eta hits " << etapattern->numberOfContainedPrds() << " cut "
                                                                       << m_maxSizeEtaPatternLoose);
                useTightAssociation = true;
            }

            double phi = phipattern->globalDirection().phi();  // should be in rad
            const double thetaphi = phipattern->globalDirection().theta();
            CxxUtils::sincos scphi(phi);
            CxxUtils::sincos scthetaphi(thetaphi);
            double dx_phi = scphi.cs * scthetaphi.sn;
            double dy_phi = scphi.sn * scthetaphi.sn;
            double dz_phi = scthetaphi.cs;
            double dotprod = dx_phi * dx_eta + dy_phi * dy_eta + dz_phi * dz_eta;

            if (dotprod > dotprodbest) {
                dotprodbest = dotprod;
                phibest = philevel;
            }

            if (!m_use_cosmics) {
                ATH_MSG_DEBUG(" eta nr " << etalevel << " phi nr " << philevel << " inproduct " << dotprod << " sin angle "
                                         << std::sin(std::acos(dotprod)));
            }

            double r0 = 0;
            double phipattern_x = 0;
            double phipattern_y = 0;

            if (m_use_cosmics) {
                // new information: update parameters
                std::array<double,4> new_pars = updateParametersForCosmics(phipattern, etapattern);
                r0 = new_pars[0];
                phi = new_pars[1];
                scphi = CxxUtils::sincos(phi);
                theta = new_pars[3];
                sctheta = CxxUtils::sincos(theta);
                rz0 = new_pars[2];
                phipattern_x = r0 * scphi.sn;
                phipattern_y = r0 * scphi.cs;
            }

            else {
                const Amg::Vector3D& posphipattern = phipattern->globalPosition();
                phipattern_x = posphipattern.x();
                phipattern_y = posphipattern.y();
                r0 = MuonHoughMathUtils::signedDistanceOfLineToOrigin2D(phipattern_x, phipattern_y, phi);
            }

            // is etapattern on phi pattern?
            // Loop over hits etapattern

            // int nhits_in_average_eta=0;
            // int nhits_inside_distance_cut_eta=0;

            if (dotprod <= 0.5 && !m_use_cosmics) continue;

            ATH_MSG_DEBUG(" Matched Eta/phi pattern ");

            // keep track of the number of eta/phi trigger and CSC hits per chamber
            std::map<Identifier, ChamberInfo> infoPerChamber;
            std::map<Muon::MuonStationIndex::StIndex, ChamberInfo> infoPerStation;
            // Loop over hits phi pattern
            double average_distance = 0;
            int nhits_in_average = 0;
            int nhits_inside_distance_cut = 0;

            double phiPatMin = 1e9;
            double phiPatMax = -1e9;

            for (unsigned int phihitid = 0; phihitid < phipattern->numberOfContainedPrds(); phihitid++) {
                const Trk::PrepRawData* prd = phipattern->prd(phihitid);
                const Amg::Vector3D& globalposhit = globalPrdPos(prd);
                double radius_hit = globalposhit.perp();
                double dotprodradius = sctheta.apply(radius_hit, globalposhit[Amg::z]);  // (radius_hit) * sctheta.sn + hitz * sctheta.cs;
                ATH_MSG_VERBOSE("combine hit: " << m_idHelperSvc->toString(prd->identify()) << " dotprod: " << dotprodradius);
                if (dotprodradius >= 0 || m_use_cosmics)  // should be on
                {
                    double residu_distance_mm = 1000000.;

                    if (m_use_cosmics) {
                        const double perp = scphi.apply(globalposhit[Amg::y], globalposhit[Amg::x]);  // hitx*scphi.cs + hity*scphi.sn;

                        residu_distance_mm = MuonHoughMathUtils::signedDistanceToLine(globalposhit[Amg::z], perp, rz0, theta);
                    } else {
                        residu_distance_mm = MuonHoughMathUtils::signedDistanceCurvedToHit(pattern_z0, theta, invcurvature, globalposhit);
                    }

                    double distancetoline = std::abs(residu_distance_mm);

                    ATH_MSG_VERBOSE(" distance RZ: " << residu_distance_mm);

                    if (distancetoline < m_maximum_rzdistance) {
                        ATH_MSG_VERBOSE(" accepted ");
                        nhits_inside_distance_cut++;
                        nhits_in_average++;
                        average_distance += distancetoline;

                        if (useTightAssociation) {
                            Identifier chId = m_idHelperSvc->chamberId(prd->identify());
                            std::map<Identifier, ChamberInfo>::iterator chPos = infoPerChamber.find(chId);
                            ChamberInfo* chInfo = nullptr;
                            if (chPos != infoPerChamber.end()) {
                                chInfo = &chPos->second;
                            } else {
                                chInfo = &infoPerChamber[chId];
                            }
                            ++chInfo->nphi;
                            double hitphi = globalposhit.phi();
                            if (hitphi < chInfo->phiMin) chInfo->phiMin = hitphi;
                            if (hitphi > chInfo->phiMax) chInfo->phiMax = hitphi;

                            Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(prd->identify());
                            std::map<Muon::MuonStationIndex::StIndex, ChamberInfo>::iterator stPos = infoPerStation.find(stIndex);
                            if (stPos != infoPerStation.end()) {
                                chInfo = &stPos->second;
                            } else {
                                chInfo = &infoPerStation[stIndex];
                            }
                            ++chInfo->nphi;
                            if (hitphi < chInfo->phiMin) chInfo->phiMin = hitphi;
                            if (hitphi > chInfo->phiMax) chInfo->phiMax = hitphi;
                            if (hitphi < phiPatMin) phiPatMin = hitphi;
                            if (hitphi > phiPatMax) phiPatMax = hitphi;
                        }
                    }
                }  // dotprodradius
            }      // size muonpattern

            if (nhits_in_average > 0) average_distance /= nhits_in_average;

            ATH_MSG_DEBUG(" Result for phi pattern: accepted hits " << nhits_inside_distance_cut << " average distance "
                                                                    << average_distance);

            bool etapattern_passed = false;
            for (unsigned int etahitid = 0; etahitid < etapattern->numberOfContainedPrds(); etahitid++) {
                const Trk::PrepRawData* prd = etapattern->prd(etahitid);
                const Amg::Vector3D& etaglobalposhit = globalPrdPos(prd);
                const double etahitx = etaglobalposhit.x();
                const double etahity = etaglobalposhit.y();

                if (!m_use_cosmics) {
                    double etadotprod = scphi.apply(etahity, etahitx);  // etahitx * scphi.cs + etahity * scphi.sn; //
                                                                        // same as in maketruetracksegment
                    if (etadotprod < 0) continue;                       // hit in wrong hemisphere
                }

                const double xdiff = phipattern_x - etahitx;
                const double ydiff = phipattern_y - etahity;
                const double etahitr = std::sqrt(xdiff * xdiff + ydiff * ydiff);

                //		const double etahitz=etaglobalposhit.z();

                // 		double scale = std::sqrt(etahitx*etahitx +
                // etahity*etahity
                // + etahitz*etahitz)/7000.; 		if (scale < 1 ) scale = 1.;

                bool hit_passed = false;
                double etadistancetoline = std::abs(MuonHoughMathUtils::distanceToLine(etahitx, etahity, r0, phi));

                ATH_MSG_VERBOSE("combine: " << m_idHelperSvc->toString(prd->identify()) << " distance xy " << etadistancetoline);

                if (m_use_cosmics) {  // phi cone does not work for cosmics since hits
                                      // might be close to position of pattern

                    const double scale = etahitr / 7000.;
                    if (etadistancetoline < scale * m_maximum_xydistance) { hit_passed = true; }
                } else {                                    // phi within 30 degrees
                    if (2 * etadistancetoline < etahitr) {  // this corresponds with 30 degrees , normal formula:
                                                            // (etadistancetoline/etahitr) < sin( Pi/180 * degrees)
                        hit_passed = true;
                    }
                }

                if (hit_passed) {
                    // if (m_verbose) m_log << MSG::VERBOSE << "hit in xy distance cut: "
                    // << nhits_inside_distance_cut_eta << endmsg;
                    // nhits_inside_distance_cut_eta++;
                    // nhits_in_average_eta++;
                    etapattern_passed = true;  // only one hit required
                    // break;
                    ATH_MSG_VERBOSE(" accepted");

                    if (useTightAssociation) {
                        Identifier chId = m_idHelperSvc->chamberId(prd->identify());
                        std::map<Identifier, ChamberInfo>::iterator chPos = infoPerChamber.find(chId);
                        ChamberInfo* chInfo = nullptr;
                        if (chPos != infoPerChamber.end()) {
                            chInfo = &chPos->second;
                        } else {
                            chInfo = &infoPerChamber[chId];
                        }
                        ++chInfo->neta;
                        if (m_idHelperSvc->isMdt(prd->identify())) {
                            Muon::MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(prd->identify());
                            ChamberInfo& stInfo = infoPerStation[stIndex];

                            const MuonGM::MdtReadoutElement* mdtDetEl =
                                dynamic_cast<const MuonGM::MdtReadoutElement*>(prd->detectorElement());
                            if (!mdtDetEl) continue;

                            const Identifier& id = prd->identify();
                            const Trk::Surface& surf = mdtDetEl->surface(id);
                            int layer = m_idHelperSvc->mdtIdHelper().tubeLayer(id);
                            int tube = m_idHelperSvc->mdtIdHelper().tube(id);
                            double halfLength = 0.5 * mdtDetEl->getWireLength(layer, tube);
                            Amg::Vector2D lpLeft(0, -halfLength);
                            Amg::Vector3D gposLeft = surf.localToGlobal(lpLeft);
                            double phiLeft = gposLeft.phi();

                            Amg::Vector2D lpRight(0, halfLength);
                            const Amg::Vector3D gposRight = surf.localToGlobal(lpRight);
                            double phiRight = gposRight.phi();
                            double phiMin = phiRight < phiLeft ? phiRight : phiLeft;
                            double phiMax = phiRight < phiLeft ? phiLeft : phiRight;

                            Amg::Vector3D tubePos = mdtDetEl->tubePos(id);
                            Amg::Vector3D ROPos = mdtDetEl->ROPos(id);
                            Amg::Vector3D HVPos = 2 * tubePos - ROPos;
                            double tubeL = (HVPos - ROPos).mag();
                            double phiRO = ROPos.phi();
                            double phiHV = HVPos.phi();
                            double phiMinPos = phiHV < phiRO ? phiHV : phiRO;
                            double phiMaxPos = phiHV < phiRO ? phiRO : phiHV;

                            if (myDebug && (fabs(phiMin - phiMinPos) > 0.01 || fabs(phiMax - phiMaxPos) > 0.01)) {
                                ATH_MSG_DEBUG(" inconsistent Phi!!: from locToGlob (" << phiMin << "," << phiMax << "), from positions ("
                                                                                      << phiMinPos << "," << phiMaxPos << ")");
                            }
                            double rotationFraction = 0.;
                            if (phiMin < 0 && phiMax > 0) {
                                if (phiMin < -0.75 * M_PI || phiMax > 0.75 * M_PI)
                                    rotationFraction = 1.5;
                                else
                                    rotationFraction = 0.5;
                            } else if (phiMax < 0) {
                                rotationFraction = 1.;
                            }
                            double phiMinR = rotatePhi(phiMin, rotationFraction);
                            double phiMaxR = rotatePhi(phiMax, rotationFraction);
                            phiMin = phiMinR < phiMaxR ? phiMinR : phiMaxR;
                            phiMax = phiMinR > phiMaxR ? phiMinR : phiMaxR;

                            phiMinR = rotatePhi(phiMinPos, rotationFraction);
                            phiMaxR = rotatePhi(phiMaxPos, rotationFraction);
                            phiMinPos = phiMinR < phiMaxR ? phiMinR : phiMaxR;
                            phiMaxPos = phiMinR > phiMaxR ? phiMinR : phiMaxR;

                            // enlarge range by 0.1 rad
                            phiMin = phiMin > 0 ? phiMin - 0.1 : phiMin + 0.1;
                            phiMax = phiMax > 0 ? phiMax + 0.1 : phiMax - 0.1;

                            double phiMinSec = 1e9;
                            double phiMaxSec = -1e9;
                            if (stInfo.nphi > 0 && stInfo.phiMin < 1000) {
                                phiMinR = rotatePhi(stInfo.phiMin, rotationFraction);
                                phiMaxR = rotatePhi(stInfo.phiMax, rotationFraction);
                                phiMinSec = phiMinR < phiMaxR ? phiMinR : phiMaxR;
                                phiMaxSec = phiMinR > phiMaxR ? phiMinR : phiMaxR;

                                bool inside = true;

                                // easy case
                                if (phiMinSec > 0 && phiMaxSec > 0) {
                                    if (phiMin > phiMaxSec || phiMax < phiMinSec) inside = false;
                                } else if (phiMinSec < 0 && phiMaxSec < 0) {
                                    // easy case (2), always outside
                                    inside = false;
                                } else {
                                    // finaly check if phiMaxSec large than maxPhi
                                    if (phiMax < phiMaxSec) inside = false;
                                }
                                // case with a
                                if (inside) {
                                    ++stInfo.ninside;
                                    ++chInfo->ninside;
                                    if (myDebug) { ATH_MSG_DEBUG(" Inside  "); }
                                } else {
                                    ++stInfo.noutside;
                                    ++chInfo->noutside;
                                    if (myDebug) { ATH_MSG_DEBUG(" Outside "); }
                                }
                            }

                            phiMinR = rotatePhi(phiPatMin, rotationFraction);
                            phiMaxR = rotatePhi(phiPatMax, rotationFraction);
                            double phiMinPat = phiMinR < phiMaxR ? phiMinR : phiMaxR;
                            double phiMaxPat = phiMinR > phiMaxR ? phiMinR : phiMaxR;

                            bool insidePat = true;
                            // easy case
                            if (phiMinPat > 0 && phiMaxPat > 0) {
                                if (phiMin > phiMaxPat || phiMax < phiMinPat) insidePat = false;
                            } else if (phiMinPat < 0 && phiMaxPat < 0) {
                                // easy case (2), always outside
                                insidePat = false;
                            } else {
                                // finaly check if phiMaxPat large than maxPhi
                                if (phiMax < phiMaxPat) insidePat = false;
                            }

                            // case with a
                            if (insidePat) {
                                ++stInfo.ninsidePat;
                                ++chInfo->ninsidePat;
                                if (myDebug) { ATH_MSG_DEBUG(" InPat  "); }
                            } else {
                                ++stInfo.noutsidePat;
                                ++chInfo->noutsidePat;
                                if (myDebug) { ATH_MSG_DEBUG(" OutPat "); }
                            }
                            if (myDebug) {
                                ATH_MSG_DEBUG(" : Phi MDT ("
                                              << std::setprecision(3) << std::setw(4) << phiMin << "," << std::setw(4) << phiMax << ") "
                                              << " from pos (" << std::setprecision(3) << std::setw(4) << phiMinPos << "," << std::setw(4)
                                              << phiMaxPos << ") ");
                                if (stInfo.nphi > 0 && stInfo.phiMin < 1000) {
                                    ATH_MSG_DEBUG(" phi range (" << std::setprecision(3) << std::setw(4) << stInfo.phiMin << ","
                                                                 << std::setw(4) << stInfo.phiMax << ")  ");
                                }
                                ATH_MSG_DEBUG(" pat range (" << std::setprecision(3) << std::setw(4) << phiMinPat << "," << std::setw(4)
                                                             << phiMaxPat << ")  " << m_idHelperSvc->toString(prd->identify()));
                                if (mdtDetEl->hasCutouts()) { ATH_MSG_DEBUG(" hasCutOuts "); }
                                ATH_MSG_DEBUG(" ATL " << mdtDetEl->getActiveTubeLength(layer, tube) << " WL "
                                                      << mdtDetEl->getWireLength(layer, tube) << " GL "
                                                      << mdtDetEl->getGasLength(layer, tube) << " POSL " << tubeL);
                            }
                        }
                    }
                }

            }  // eta pattern

            if (!etapattern_passed) continue;  // no etahit close to phipattern, try next phi pattern

            unsigned int netaPhiPairs = 0;
            if (useTightAssociation) {
                // now we have calculated the eta/phi hit content of the 'merged'
                // pattern
                std::map<Identifier, ChamberInfo>::iterator chit = infoPerChamber.begin();
                std::map<Identifier, ChamberInfo>::iterator chit_end = infoPerChamber.end();
                std::map<int, ChamberInfo> hitsPerSector;
                for (; chit != chit_end; ++chit) {
                    if (myDebug) {
                        ATH_MSG_DEBUG("  " << std::setw(32) << m_idHelperSvc->toStringChamber(chit->first) << "  eta hits "
                                           << chit->second.neta << "  phi hits " << chit->second.nphi << "  ninside  "
                                           << chit->second.ninside << "  noutside  " << chit->second.noutside << "  ninside  "
                                           << chit->second.ninsidePat << "  noutside  " << chit->second.noutsidePat);
                    }
                    if (chit->second.neta > 0 && chit->second.nphi) ++netaPhiPairs;
                }
                if (myDebug) {
                    ATH_MSG_DEBUG(" eta/phi pattern hit overlap " << netaPhiPairs);
                    if (!etapattern_passed) { ATH_MSG_DEBUG("  failed eta hit match "); }
                    if (nhits_inside_distance_cut < (phipattern->numberOfContainedPrds() * 0.25)) {
                        ATH_MSG_DEBUG("  failed phi hit match ");
                    }
                    if (netaPhiPairs == 0) { ATH_MSG_DEBUG("  Bad match, no overlap "); }
                }
                std::map<int, ChamberInfo>::iterator secIt = hitsPerSector.begin();
                std::map<int, ChamberInfo>::iterator secIt_end = hitsPerSector.end();
                for (; secIt != secIt_end; ++secIt) {
                    if (myDebug) {
                        ATH_MSG_DEBUG(" sector " << std::setw(4) << secIt->first << "  eta hits " << chit->second.neta << "  phi hits "
                                                 << chit->second.nphi << "  ninside  " << chit->second.ninside << "  noutside  "
                                                 << chit->second.noutside << "  ninside  " << chit->second.ninsidePat << "  noutside  "
                                                 << chit->second.noutsidePat);
                    }
                }
            }
            if (msgLvl(MSG::DEBUG)) {
                ATH_MSG_DEBUG(" Eta pattern compatible with phi pattern, eta/phi overlap " << netaPhiPairs << " ass phi hits "
                                                                                           << nhits_inside_distance_cut << " tot phi hits "
                                                                                           << phipattern->numberOfContainedPrds());
                if (useTightAssociation) { ATH_MSG_DEBUG(" using tight association"); }
            }

            // at least 25% matched, to be more robust than 1!
            if ((!useTightAssociation || netaPhiPairs > 0) && nhits_inside_distance_cut >= (phipattern->numberOfContainedPrds() * 0.25)) {
                ismatched = true;

                // for non-cosmics try every candidate
                // for cosmics keep track of best candidate, possible eff loss when phi
                // patterns are split
                if (m_use_cosmics) {
                    std::array<double,4> new_pars{r0, phi, pattern_z0, theta};
                    std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*> updatedpatterns =
                        updatePatternsForCosmics(phipattern, etapattern, new_pars);
                    phipattern = updatedpatterns.first;
                    etapattern = updatedpatterns.second;

                    patternsToDelete.emplace_back(updatedpatterns.first);
                    patternsToDelete.emplace_back(updatedpatterns.second);
                    ATH_MSG_DEBUG(" Combination accepted with cosmic selection ");
                } else if (useTightAssociation) {
                    ATH_MSG_DEBUG(" Tight association, cleaning patterns");

                    // clean up hits using local phi info
                    Muon::MuonPrdPattern* etaPat = new Muon::MuonPrdPattern(etapattern->globalPosition(), etapattern->globalDirection());
                    Muon::MuonPrdPattern* phiPat = new Muon::MuonPrdPattern(phipattern->globalPosition(), phipattern->globalDirection());
                    for (unsigned int etahitid = 0; etahitid < etapattern->numberOfContainedPrds(); etahitid++) {
                        const Trk::PrepRawData* prd = etapattern->prd(etahitid);
                        const Identifier& id = prd->identify();
                        Identifier chId = m_idHelperSvc->chamberId(id);
                        std::map<Identifier, ChamberInfo>::iterator chPos = infoPerChamber.find(chId);
                        if (chPos == infoPerChamber.end()) continue;

                        if (m_idHelperSvc->isMdt(id)) {
                            if (chPos->second.ninside == 0 && chPos->second.noutside > 0) continue;
                            if (chPos->second.ninsidePat == 0 && chPos->second.noutsidePat > 0) continue;
                        } else {
                            if (chPos->second.nphi == 0) continue;
                        }
                        etaPat->addPrd(prd);
                    }
                    for (unsigned int phihitid = 0; phihitid < phipattern->numberOfContainedPrds(); phihitid++) {
                        const Trk::PrepRawData* prd = phipattern->prd(phihitid);
                        const Identifier& id = prd->identify();
                        Identifier chId = m_idHelperSvc->chamberId(id);
                        std::map<Identifier, ChamberInfo>::iterator chPos = infoPerChamber.find(chId);
                        if (chPos == infoPerChamber.end()) continue;

                        if (chPos->second.neta == 0) continue;
                        phiPat->addPrd(prd);
                    }
                    patternsToDelete.emplace_back(etaPat);
                    patternsToDelete.emplace_back(phiPat);
                    phipattern = phiPat;
                    etapattern = etaPat;
                }

                if (!m_bestphimatch) {
                    addCandidate(etapattern, phipattern, candidates, false, patternsToDelete, phiEtaHitAssMap);
                    ATH_MSG_DEBUG("Candidate FOUND eta " << etalevel << " phi " << philevel << " dotprod: " << dotprod);
                } else {
                    if (average_distance < min_average_distance) {
                        ATH_MSG_DEBUG(" Selected as best candidate ");
                        min_average_distance = average_distance;
                        max_phipattern = phipattern;
                        max_philevel = philevel;
                    }
                    ATH_MSG_DEBUG(" theta pattern " << etapattern->globalDirection().theta() << " phi "
                                                    << phipattern->globalDirection().phi() << "average distance " << average_distance
                                                    << " number of hits " << nhits_inside_distance_cut << " etalevel: " << etalevel);
                }

                // add recovery for the case we have an inefficiency in the eta hits
            } else if (useTightAssociation && netaPhiPairs == 0 &&
                       nhits_inside_distance_cut >= (phipattern->numberOfContainedPrds() * 0.25)) {
                ATH_MSG_DEBUG(" Combination rejected by phi/eta overlap: average distance " << average_distance);

                if (average_distance < min_average_distance) {
                    ATH_MSG_DEBUG("  but selected as best candidate ");
                    min_average_distance = average_distance;
                    max_phipattern = phipattern;
                    max_philevel = philevel;
                }
            }  // nhits>=25%
        }      // size phi level

        // for cosmics only match best phi pattern
        if (m_bestphimatch) {
            if (ismatched) {
                //      const Muon::MuonPrdPattern* phipattern =
                //      phiPatternCollection->at(max_philevel);
                addCandidate(etapattern, max_phipattern, candidates, true, patternsToDelete, phiEtaHitAssMap);
                ATH_MSG_DEBUG("Candidate FOUND eta " << etalevel << " phi " << max_philevel);
            }
        }
        // make associated phi pattern for every etapattern and as candidate for
        // robustness not needed for cosmics when matched, since already done for
        // first match:

        if (!(m_use_cosmics && m_splitpatterns && ismatched)) {
            const Muon::MuonPrdPattern* assphipattern = makeAssPhiPattern(etapattern, phiEtaHitAssMap, true);
            ATH_MSG_DEBUG("No match found, trying to create associated phi pattern ");
            if (assphipattern) {
                // make sure ass phi pattern is not a complete subset of one of the
                // other phi patterns:

                bool subsetcheck = false;

                std::reverse_iterator<std::vector<std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*>>::iterator> rit =
                    candidates.rbegin();
                for (; rit != candidates.rend(); ++rit) {
                    if ((*rit).first != etapattern) { break; }
                    // 	    for (;phi_it!=range.second;++phi_it) {
                    if (subset(assphipattern, (*rit).second)) {
                        subsetcheck = true;
                        break;
                    }
                }
                if (subsetcheck) {
                    delete assphipattern;
                    assphipattern = nullptr;
                } else {
                    // these associated phi patterns should be deleted at end of routine:
                    patternsToDelete.emplace_back(assphipattern);

                    ATH_MSG_DEBUG("Candidate FOUND eta " << etalevel << " and associated phipattern ");

                    ismatched = true;

                    // print associated pattern:
                    if (msgLvl(MSG::VERBOSE)) printPattern(assphipattern);

                    if (m_use_cosmics) {
                        std::array<double,4> new_pars = updateParametersForCosmics(assphipattern, etapattern);
                        std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*> updatedpatterns =
                            updatePatternsForCosmics(assphipattern, etapattern, new_pars);
                        assphipattern = updatedpatterns.first;
                        etapattern = updatedpatterns.second;

                        patternsToDelete.emplace_back(updatedpatterns.first);
                        patternsToDelete.emplace_back(updatedpatterns.second);
                    }
                    ATH_MSG_DEBUG(" adding eta pattern with recalculated associated phi pattern ");

                    addCandidate(etapattern, assphipattern, candidates, false, patternsToDelete, phiEtaHitAssMap);
                }
            }
        }
        if (!ismatched && max_philevel > -1) {
            addCandidate(etapattern, max_phipattern, candidates, true, patternsToDelete, phiEtaHitAssMap);
            ismatched = true;
            ATH_MSG_DEBUG("No good candidate found, adding best phi pattern " << etalevel << " phi " << max_philevel);
        }
        if (!ismatched) {
            if (msgLvl(MSG::DEBUG)) {
                ATH_MSG_DEBUG("NO COMBINED Candidate FOUND eta " << etalevel << " phi " << phibest);
                if (!m_use_cosmics) ATH_MSG_DEBUG("dotprodbest: " << dotprodbest);
                ATH_MSG_DEBUG("writing out eta pattern (no cleanup)");
            }
            Muon::MuonPrdPattern* phi_dummy = nullptr;
            candidates.emplace_back(etapattern, phi_dummy);
        } else {
            if (msgLvl(MSG::DEBUG)) ATH_MSG_DEBUG("Candidate was associated to a phi pattern ");
        }
    }  // size rtheta level

    MuonPrdPatternCollection* combinedpatterns = makeCombinedPatterns(candidates);

    return combinedpatterns;

}  // combineEtaPhiPatterns

MuonPrdPatternCollection* MuonCombinePatternTool::makeCombinedPatterns(
    std::vector<std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*>>& candidates) const {
    ATH_MSG_DEBUG("Number of Candidates: " << candidates.size());

    //  if (m_use_cosmics == true) {
    cleanCandidates(candidates);
    ATH_MSG_DEBUG("Number of Candidates after cleaning: " << candidates.size());
    //}

    MuonPrdPatternCollection* combinedPatternCollection = new MuonPrdPatternCollection();
    int number_comb_patterns = 0;
    std::vector<std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*>>::iterator it = candidates.begin();

    for (; it != candidates.end(); ++it) {
        ATH_MSG_DEBUG("Next Candidate");

        const Muon::MuonPrdPattern* etapattern = (*it).first;
        const Muon::MuonPrdPattern* phipattern = (*it).second;

        if (phipattern) {
            Muon::MuonPrdPattern* combinedpattern = makeCombinedPattern(phipattern, etapattern);
            if (combinedpattern) {
                combinedPatternCollection->push_back(combinedpattern);
                if (msgLvl(MSG::VERBOSE)) { printPattern(combinedpattern); }
                number_comb_patterns++;
            } else
                ATH_MSG_WARNING("combined pattern lost");
        } else {  // no combined match found &&  no associated phi hits found
            if (m_splitpatterns && m_use_cosmics) {
                ATH_MSG_DEBUG(
                    "No combined pattern, eta pattern split based on phi "
                    "direction of eta pattern ");

                std::vector<std::pair<Muon::MuonPrdPattern*, Muon::MuonPrdPattern*>> splitetapatterns =
                    splitPatternsCylinder(nullptr, etapattern);
                if (splitetapatterns.empty()) {
                    combinedPatternCollection->push_back(etapattern->clone());
                } else {
                    for (unsigned int i = 0; i < splitetapatterns.size(); i++) {
                        if (splitetapatterns[i].second->numberOfContainedPrds() != 0) {
                            combinedPatternCollection->push_back(splitetapatterns[i].second);
                        } else {
                            delete splitetapatterns[i].second;
                        }
                    }
                }
            } else {  // don't split pattern
                ATH_MSG_DEBUG("No combined pattern, eta pattern copied ");
                Muon::MuonPrdPattern* etapattern_copy =
                    new Muon::MuonPrdPattern(etapattern->globalPosition(), etapattern->globalDirection());
                for (unsigned int etahitnr = 0; etahitnr < etapattern->numberOfContainedPrds(); etahitnr++) {
                    etapattern_copy->addPrd(etapattern->prd(etahitnr));
                }  // size eta pattern
                combinedPatternCollection->push_back(etapattern_copy);
                if (msgLvl(MSG::VERBOSE)) { printPattern(etapattern_copy); }
            }
        }
    }

    ATH_MSG_DEBUG("Number of combined patterns: " << number_comb_patterns << " Number of unmatched etapatterns: "
                                                  << combinedPatternCollection->size() - number_comb_patterns);

    return combinedPatternCollection;
}

Muon::MuonPrdPattern* MuonCombinePatternTool::makeCombinedPattern(const Muon::MuonPrdPattern* phipattern,
                                                                  const Muon::MuonPrdPattern* etapattern) const {
    ATH_MSG_DEBUG("make combined pattern");

    double phi = phipattern->globalDirection().phi();
    double theta = etapattern->globalDirection().theta();
    CxxUtils::sincos scphi(phi);
    CxxUtils::sincos sctheta(theta);

    double phieta = etapattern->globalDirection().phi();
    double eta_x = etapattern->globalPosition().x();
    double eta_y = etapattern->globalPosition().y();
    double eta_r0 = MuonHoughMathUtils::signedDistanceOfLineToOrigin2D(eta_x, eta_y, phieta);
    double charge = 1.;
    if (!m_use_cosmics && eta_r0 < 0) charge = -1.;
    double curvature = etapattern->globalDirection().mag();

    // Store charge in sign of r0 (like in Muon eta pattern)

    double x0 = charge * (phipattern->globalPosition().x());
    double y0 = charge * (phipattern->globalPosition().y());

    double z0_phi = 0.;  // for non-cosmics

    if (m_use_cosmics) {  // calculate average z_0 for eta hits
        z0_phi = calculateRz0(etapattern, phi, theta) / sctheta.sn;
    }

    const Amg::Vector3D& dir = Amg::Vector3D(curvature * scphi.cs * sctheta.sn, curvature * scphi.sn * sctheta.sn, curvature * sctheta.cs);
    const Amg::Vector3D& pos = Amg::Vector3D(x0, y0, z0_phi);

    Muon::MuonPrdPattern* combinedpattern = new Muon::MuonPrdPattern(pos, dir);

    for (unsigned int etahitnr = 0; etahitnr < etapattern->numberOfContainedPrds(); etahitnr++) {
        combinedpattern->addPrd(etapattern->prd(etahitnr));
    }

    for (unsigned int phihitnr = 0; phihitnr < phipattern->numberOfContainedPrds(); phihitnr++) {
        //      if
        //      (!(combinedhoughpattern.hitInHoughPattern(phipattern->getHit(phihitnr))))
        //      // should not be possible, maybe still good to check this
        //{
        combinedpattern->addPrd(phipattern->prd(phihitnr));
    }  // size phi pattern

    ATH_MSG_DEBUG("Combined pattern with charge " << charge << " curvature " << curvature);
    ATH_MSG_DEBUG("direction combined pattern: " << scphi.cs * sctheta.sn << " " << scphi.sn * sctheta.sn << " " << sctheta.cs);
    ATH_MSG_DEBUG("position combined pattern: " << x0 << " " << y0 << " " << z0_phi);
    ATH_MSG_DEBUG("etapatternsize: " << etapattern->numberOfContainedPrds());
    ATH_MSG_DEBUG("phipatternsize: " << phipattern->numberOfContainedPrds());
    ATH_MSG_DEBUG("Combined Track size: " << combinedpattern->numberOfContainedPrds());

    if (m_use_cosmics) {
        if (msgLvl(MSG::VERBOSE)) ATH_MSG_VERBOSE("No Cleaning for Cosmics!");

        if (msgLvl(MSG::VERBOSE)) { printPattern(combinedpattern); }
        return combinedpattern;
    }

    ATH_MSG_DEBUG("Start Cleaning and Recalculating of Combined Pattern");

    bool change = true;

    while (change) {
        int size_before_cleanup = combinedpattern->numberOfContainedPrds();
        ATH_MSG_DEBUG("size before cleanup: " << size_before_cleanup);

        Muon::MuonPrdPattern* cleaneduppattern = cleanupCombinedPattern(combinedpattern);

        // swap(cleaneduppattern,combinedpattern);
        delete combinedpattern;
        combinedpattern = nullptr;

        int size_after_cleanup = cleaneduppattern->numberOfContainedPrds();
        ATH_MSG_DEBUG("size after cleanup: " << size_after_cleanup);

        if (size_before_cleanup == size_after_cleanup || size_after_cleanup == 0) {
            if (msgLvl(MSG::VERBOSE)) { printPattern(cleaneduppattern); }
            return cleaneduppattern;
        } else if (size_after_cleanup < size_before_cleanup) {
            combinedpattern = cleaneduppattern;
        } else {
            change = false;
            ATH_MSG_FATAL("Cosmic Muon through computer bit? ");
        }
    }  // while
    return nullptr;
}  // makeCombinedPattern

std::vector<std::pair<Muon::MuonPrdPattern*, Muon::MuonPrdPattern*>> MuonCombinePatternTool::splitPatterns2D(
    const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern) const {
    std::vector<std::pair<Muon::MuonPrdPattern*, Muon::MuonPrdPattern*>> splitPatterns;
    splitPatterns.reserve(2);

    double phi = phipattern->globalDirection().phi();
    double theta = etapattern->globalDirection().theta();

    CxxUtils::sincos scphi(phi);
    CxxUtils::sincos sctheta(theta);

    Amg::Vector3D dir1 = Amg::Vector3D(scphi.cs * sctheta.sn, scphi.sn * sctheta.sn, sctheta.cs);
    Amg::Vector3D dir2 = dir1;

    Muon::MuonPrdPattern* phipattern1 = new Muon::MuonPrdPattern(phipattern->globalPosition(), dir1);  // "lower" pattern (y<0)
    Muon::MuonPrdPattern* etapattern1 = new Muon::MuonPrdPattern(etapattern->globalPosition(), dir1);  // "lower" pattern (y<0)

    if (m_flipdirectionforcosmics) {
        const double newphi = phi + M_PI;
        const double newtheta = M_PI - etapattern->globalDirection().theta();
        CxxUtils::sincos scnewphi(newphi);
        CxxUtils::sincos scnewtheta(newtheta);

        // flip phi and theta for second pattern:
        dir2 = Amg::Vector3D(scnewphi.cs * scnewtheta.sn, scnewphi.sn * scnewtheta.sn, scnewtheta.cs);
    }

    Muon::MuonPrdPattern* phipattern2 = new Muon::MuonPrdPattern(phipattern->globalPosition(), dir2);  // "upper" pattern (y>0)
    Muon::MuonPrdPattern* etapattern2 = new Muon::MuonPrdPattern(etapattern->globalPosition(), dir2);  // "upper" pattern (y>0)

    ATH_MSG_DEBUG(" split pattern1 theta: " << phipattern1->globalDirection().theta() << " phi: " << phipattern1->globalDirection().phi());
    ATH_MSG_DEBUG(" split pattern2 theta: " << phipattern2->globalDirection().theta() << " phi: " << phipattern2->globalDirection().phi());

    splitPatterns.emplace_back(phipattern1, etapattern1);
    splitPatterns.emplace_back(etapattern1, etapattern2);

    for (unsigned int hitid = 0; hitid < phipattern->numberOfContainedPrds(); hitid++) {
        const Trk::PrepRawData* prd = phipattern->prd(hitid);
        const Amg::Vector3D& globalposhit = globalPrdPos(prd);
        const double hitx = globalposhit.x();
        const double hity = globalposhit.y();
        const double dotprod = scphi.apply(hity, hitx);  // hity * scphi.sn + hitx * scphi.cs;
        if (dotprod >= 0) {
            phipattern1->addPrd(prd);
        } else {
            phipattern2->addPrd(prd);
        }
    }
    for (unsigned int hitid = 0; hitid < etapattern->numberOfContainedPrds(); hitid++) {
        const Trk::PrepRawData* prd = etapattern->prd(hitid);
        const Amg::Vector3D& globalposhit = globalPrdPos(prd);
        const double hitx = globalposhit.x();
        const double hity = globalposhit.y();
        const double dotprod = scphi.apply(hity, hitx);  // hity * scphi.sn + hitx * scphi.cs;
        if (dotprod >= 0) {
            etapattern1->addPrd(prd);
        } else {
            etapattern2->addPrd(prd);
        }
    }

    return splitPatterns;
}

std::vector<std::pair<Muon::MuonPrdPattern*, Muon::MuonPrdPattern*>> MuonCombinePatternTool::splitPatterns3D(
    const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern) const {
    // phi pattern may be 0

    std::vector<std::pair<Muon::MuonPrdPattern*, Muon::MuonPrdPattern*>> splitPatterns;
    splitPatterns.reserve(2);

    // phi and etapattern should have identical directions and positions (except
    // for z_0), but new built anyway to be robust

    Amg::Vector3D globalpos;
    if (phipattern) {
        globalpos = phipattern->globalPosition();
    } else {
        globalpos = etapattern->globalPosition();
    }

    double phi;
    if (phipattern) {
        phi = phipattern->globalDirection().phi();
    } else {
        phi = etapattern->globalDirection().phi();
    }

    const double theta = etapattern->globalDirection().theta();

    CxxUtils::sincos scphi(phi);
    CxxUtils::sincos sctheta(theta);

    Amg::Vector3D dir1 = Amg::Vector3D(scphi.cs * sctheta.sn, scphi.sn * sctheta.sn, sctheta.cs);
    Amg::Vector3D dir2 = dir1;

    if (m_flipdirectionforcosmics) {
        const double newphi = phi + M_PI;
        const double newtheta = M_PI - theta;

        CxxUtils::sincos scnewphi(newphi);
        CxxUtils::sincos scnewtheta(newtheta);

        // flip phi and theta for second pattern:
        dir2 = Amg::Vector3D(scnewphi.cs * scnewtheta.sn, scnewphi.sn * scnewtheta.sn, scnewtheta.cs);
    }

    Muon::MuonPrdPattern* etapattern1 = new Muon::MuonPrdPattern(globalpos, dir1);  //
    Muon::MuonPrdPattern* etapattern2 = new Muon::MuonPrdPattern(globalpos, dir2);  // "upper" pattern (y>0)
    Muon::MuonPrdPattern* phipattern1 = nullptr;
    Muon::MuonPrdPattern* phipattern2 = nullptr;

    if (phipattern) {
        phipattern1 = new Muon::MuonPrdPattern(globalpos, dir1);  // "lower" pattern (y<0)
        phipattern2 = new Muon::MuonPrdPattern(globalpos, dir2);  // "upper" pattern (y>0)
    }

    splitPatterns.emplace_back(phipattern1, etapattern1);
    splitPatterns.emplace_back(phipattern2, etapattern2);

    if (msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG(" split pattern theta: " << theta << " phi: " << phi);
        ATH_MSG_DEBUG(" split pattern1 theta: " << etapattern1->globalDirection().theta()
                                                << " phi: " << etapattern1->globalDirection().phi());
        ATH_MSG_DEBUG(" split pattern2 theta: " << etapattern2->globalDirection().theta()
                                                << " phi: " << etapattern2->globalDirection().phi());
        Amg::Vector3D splitpoint = MuonHoughMathUtils::shortestPointOfLineToOrigin(globalpos, phi, theta);
        ATH_MSG_DEBUG(" splitpoint, x: " << splitpoint[0] << " y: " << splitpoint[1] << " z: " << splitpoint[2]);
    }

    double d_x = scphi.cs * sctheta.sn;
    double d_y = scphi.sn * sctheta.sn;

    if (phipattern) {
        for (unsigned int hitid = 0; hitid < phipattern->numberOfContainedPrds(); hitid++) {
            const Trk::PrepRawData* prd = phipattern->prd(hitid);
            const Amg::Vector3D& globalposhit = globalPrdPos(prd);
            const double hitx = globalposhit.x();
            const double hity = globalposhit.y();
            const double hitz = globalposhit.z();
            const double dotprod = hitx * d_x + hity * d_y + hitz * sctheta.cs;
            if (dotprod >= 0) {
                phipattern1->addPrd(prd);
            } else {
                phipattern2->addPrd(prd);
            }
            ATH_MSG_VERBOSE(" dotprod: " << dotprod);
        }
    }

    for (unsigned int hitid = 0; hitid < etapattern->numberOfContainedPrds(); hitid++) {
        const Trk::PrepRawData* prd = etapattern->prd(hitid);
        const Amg::Vector3D& globalposhit = globalPrdPos(prd);
        const double hitx = globalposhit.x();
        const double hity = globalposhit.y();
        const double hitz = globalposhit.z();
        const double dotprod = hitx * d_x + hity * d_y + hitz * sctheta.cs;
        if (dotprod >= 0) {
            etapattern1->addPrd(prd);
        } else {
            etapattern2->addPrd(prd);
        }
        ATH_MSG_VERBOSE(" dotprod: " << dotprod);
    }

    if (msgLvl(MSG::DEBUG)) {
        if (phipattern) {
            ATH_MSG_DEBUG("Final size, phi: " << phipattern1->numberOfContainedPrds() << " " << phipattern2->numberOfContainedPrds());
        }
        ATH_MSG_DEBUG("Final size, eta: " << etapattern1->numberOfContainedPrds() << " " << etapattern2->numberOfContainedPrds());
    }

    return splitPatterns;
}

std::vector<std::pair<Muon::MuonPrdPattern*, Muon::MuonPrdPattern*>> MuonCombinePatternTool::splitPatternsCylinder(
    const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern) const {
    // phi pattern may be 0
    Amg::Vector3D patternpos;
    double phi;

    if (phipattern) {
        patternpos = phipattern->globalPosition();
        phi = phipattern->globalDirection().phi();
    } else {
        patternpos = etapattern->globalPosition();
        phi = etapattern->globalDirection().phi();
    }

    const double theta = etapattern->globalDirection().theta();

    // decide if track is split:

    // calculate intersection with pattern and calorimeter cylinder (r0=4000,
    // c0=6000) if there is one, then track will be split

    bool intersect = MuonHoughMathUtils::lineThroughCylinder(patternpos, phi, theta, MuonHough::radius_cylinder, MuonHough::z_cylinder);

    if (!intersect) {  // no split
        ATH_MSG_DEBUG("Pattern not through calorimeter -> do not split ");
        return std::vector<std::pair<Muon::MuonPrdPattern*,
                                     Muon::MuonPrdPattern*>>();  // return empty vector , probably
                                                                 // better way to save this creation
    }

    return splitPatterns3D(phipattern, etapattern);
}

Muon::MuonPrdPattern* MuonCombinePatternTool::cleanupCombinedPattern(Muon::MuonPrdPattern*& combinedpattern) const {
    const double phipattern = combinedpattern->globalDirection().phi();
    const double thetapattern = combinedpattern->globalDirection().theta();

    CxxUtils::sincos scthetapattern(thetapattern);

    const Amg::Vector3D& patternpos = combinedpattern->globalPosition();
    const double posx = patternpos.x();
    const double posy = patternpos.y();
    const double posz = patternpos.z();

    double invcurvature = 0.;
    double r0 = MuonHoughMathUtils::signedDistanceOfLineToOrigin2D(posx, posy, phipattern);
    double charge = 1.;
    if (r0 < 0) charge = -1.;
    double curvature = combinedpattern->globalDirection().mag();
    if (curvature > 2) invcurvature = charge / curvature;

    ATH_MSG_DEBUG("cleaned up pattern: phi " << phipattern << " theta: " << thetapattern << " position: " << posx << " " << posy << " "
                                             << posz);
    ATH_MSG_DEBUG("Cleanup pattern charge " << charge << " curvature " << curvature);

    Muon::MuonPrdPattern* combinedpattern_cleaned =
        new Muon::MuonPrdPattern(combinedpattern->globalPosition(), combinedpattern->globalDirection());

    for (unsigned int hitid = 0; hitid < combinedpattern->numberOfContainedPrds(); hitid++) {
        const Trk::PrepRawData* prd = combinedpattern->prd(hitid);
        const Amg::Vector3D& globalposhit = globalPrdPos(prd);
       
        double r0 = MuonHoughMathUtils::signedDistanceOfLineToOrigin2D(posx, posy, phipattern);
        double distance_xy = MuonHoughMathUtils::distanceToLine(globalposhit[Amg::x], globalposhit[Amg::y], r0, phipattern);

        double radius_pattern = globalposhit.perp();
        double z0 = posz - radius_pattern * scthetapattern.cs / scthetapattern.sn;

        double distance_rz = MuonHoughMathUtils::signedDistanceCurvedToHit(z0, thetapattern, invcurvature, globalposhit);

        const double scale = std::max(1., globalposhit.mag() / 7000.);
        ATH_MSG_VERBOSE("hit: " << globalposhit);
        ATH_MSG_VERBOSE("CLEAN distancetopattern: "
                        << " dist xy " << distance_xy << " dist rz " << distance_rz << " scale: " << scale);
        if (std::abs(distance_xy) < scale * m_maximum_xydistance && std::abs(distance_rz) < m_maximum_rzdistance) {
            combinedpattern_cleaned->addPrd(prd);
        } else {
            if (msgLvl(MSG::DEBUG)) {
                ATH_MSG_DEBUG("Hit discarded: " << hitid << " dis xy " << distance_xy << " dis rz " << distance_rz);
                ATH_MSG_DEBUG("Hit info: ");
                m_idHelperSvc->mdtIdHelper().print(prd->identify());
            }
        }
    }

    ATH_MSG_DEBUG("size of cleaned pattern: " << combinedpattern_cleaned->numberOfContainedPrds());

    if (combinedpattern_cleaned->numberOfContainedPrds() == 0 && combinedpattern->numberOfContainedPrds() != 0) {
        ATH_MSG_DEBUG(
            "cleaned up pattern is empty (should happen only when initially no phi "
            "pattern found and phi hits are added by ascociation map)");
    }

    return combinedpattern_cleaned;
}

Muon::MuonPrdPattern* MuonCombinePatternTool::makeAssPhiPattern(
    const Muon::MuonPrdPattern* muonpattern,
    /** phi eta association map, eta prds are key*/
    const std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>* phiEtaHitAssMap,
    bool check_already_on_pattern) const {
    // bool hits_added = false;
    const unsigned int size = muonpattern->numberOfContainedPrds();

    std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess> hits;
    if (check_already_on_pattern) {
        for (unsigned int i = 0; i < size; i++) { hits.insert(muonpattern->prd(i)); }
    }
    std::vector<const Trk::PrepRawData*> phihits;
    phihits.reserve(20);  // just a sophisticated guess

    std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>::const_iterator it;
    for (unsigned int i = 0; i < size; i++) {
        const Trk::PrepRawData* prd = muonpattern->prd(i);
        // check on type of prd?
        const Muon::MuonCluster* muoncluster = dynamic_cast<const Muon::MuonCluster*>(prd);
        if (muoncluster) {
            // test on measuresphi?
            it = phiEtaHitAssMap->find(prd);
            if (it != phiEtaHitAssMap->end()) {  // check if hit has associated hit
                std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>::iterator set_it = (*it).second.begin();
                for (; set_it != (*it).second.end(); ++set_it) {
                    if (!check_already_on_pattern || hits.find(*set_it) == hits.end()) {  // check if associated hit already in pattern
                        phihits.push_back(*set_it);
                        if (check_already_on_pattern) { hits.insert(*set_it); }
                        ATH_MSG_VERBOSE("Associated Phi Hit Added to Pattern");
                    }
                }
            }
        }
    }
    Muon::MuonPrdPattern* phipattern = nullptr;

    if (phihits.empty()) { return phipattern; }

    std::vector<const Trk::PrepRawData*>::iterator vec_it = phihits.begin();
    double phi = 0., sin_phi = 0., cos_phi = 0.;
    if (m_use_cosmics) {
        phi = muonpattern->globalDirection().phi();
    } else {
        for (; vec_it != phihits.end(); ++vec_it) {
            const Amg::Vector3D& globalposhit = globalPrdPos(*vec_it);
            CxxUtils::sincos scphihit(globalposhit.phi());
            sin_phi += scphihit.sn;
            cos_phi += scphihit.cs;
        }
        phi = std::atan2(sin_phi, cos_phi);
    }

    const double curvature = muonpattern->globalDirection().mag();
    const double theta = muonpattern->globalDirection().theta();
    CxxUtils::sincos scphi(phi);
    CxxUtils::sincos sctheta(theta);

    if (m_use_cosmics) {
        phipattern = new Muon::MuonPrdPattern(muonpattern->globalPosition(), muonpattern->globalDirection());
    } else {
        const Amg::Vector3D& globalpos = Amg::Vector3D(0.001, 0.001, 0.);
        const Amg::Vector3D& globaldir =
            Amg::Vector3D(curvature * scphi.cs * sctheta.sn, curvature * scphi.sn * sctheta.sn, curvature * sctheta.cs);
        phipattern = new Muon::MuonPrdPattern(globalpos, globaldir);
    }

    vec_it = phihits.begin();
    for (; vec_it != phihits.end(); ++vec_it) { phipattern->addPrd(*vec_it); }

    // perform cleaning on newly created phipattern:
    Muon::MuonPrdPattern* phipatternclean = cleanPhiPattern(phipattern);
    delete phipattern;

    if (phipatternclean->numberOfContainedPrds() <= 0) {
        delete phipatternclean;
        return nullptr;
    }
    return phipatternclean;
}

std::array<double,4> MuonCombinePatternTool::updateParametersForCosmics(const Muon::MuonPrdPattern* phipattern,
                                                           const Muon::MuonPrdPattern* etapattern) const {
    // method returns r0, phi, rz0, theta

    const unsigned int etasize = etapattern->numberOfContainedPrds();
    const unsigned int phisize = phipattern->numberOfContainedPrds();

    const Amg::Vector3D& etaglobalpos = etapattern->globalPosition();
    const Amg::Vector3D& etaglobaldir = etapattern->globalDirection();

    const Amg::Vector3D& phiglobalpos = phipattern->globalPosition();
    const Amg::Vector3D& phiglobaldir = phipattern->globalDirection();

    std::array<double,4> old_pars{0};

    old_pars[0] = MuonHoughMathUtils::signedDistanceOfLineToOrigin2D(phiglobalpos.x(), phiglobalpos.y(), phiglobaldir.phi());
    old_pars[1] = phiglobaldir.phi();

    const double theta_orig = etaglobaldir.theta();
    old_pars[2] = etaglobalpos.z() * std::sin(theta_orig);
    old_pars[3] = theta_orig;

    if (phisize + etasize <= 1) return old_pars;

    // first calculate new phi, r0 (initial value , -Pi/2):
    std::pair<double, double> rphi_start = calculateR0Phi(phipattern, etapattern);
    // for stabilising (can be cpu-optimised greatly):
    std::pair<double, double> rphi = calculateR0Phi(phipattern, etapattern, rphi_start.first);

    if (msgLvl(MSG::DEBUG) && std::abs(std::sin(rphi.first - rphi_start.first)) > 0.15 &&
        std::abs(std::sin(etaglobaldir.phi() - phiglobaldir.phi())) < 0.15) {
        ATH_MSG_DEBUG("unexpected phi change!");
        ATH_MSG_DEBUG("phi first: " << rphi_start.first << " phi second: " << rphi.first);
        ATH_MSG_DEBUG("phi etapattern: " << etaglobaldir.phi() << " phi phipattern: " << phiglobaldir.phi());
    }

    const double phi = rphi.first;
    const double r0 = rphi.second;

    CxxUtils::sincos scphi(phi);

    // calculate new theta and rz0: (not using phi hits this time, as eta pattern
    // is larger in general

    double av_radii = 0.;
    double av_z = 0.;

    for (unsigned int i = 0; i < etasize; ++i) {
        const Trk::PrepRawData* prd = etapattern->prd(i);
        const Amg::Vector3D& globalposhit = globalPrdPos(prd);
        av_radii += scphi.apply(globalposhit.y(),
                                globalposhit.x());  // globalposhit.x()*scphi.cs + globalposhit.y()*scphi.sn;
        av_z += globalposhit.z();
    }

    if (etasize > 0) {
        av_radii /= etasize;
        av_z /= etasize;
    }
    double sumr = 0.;
    double sumz = 0.;
    for (unsigned int i = 0; i < etasize; i++) {
        const Trk::PrepRawData* prd = etapattern->prd(i);
        const Amg::Vector3D& globalposhit = globalPrdPos(prd);
        
        double radius = scphi.apply(globalposhit[Amg::y], globalposhit[Amg::x]);  // hitx*scphi.cs + hity*scphi.sn;
        double r_offset = radius - av_radii;
        double z_offset = globalposhit[Amg::z] - av_z;
        double weight = r_offset * r_offset + z_offset * z_offset;
        int sign = 1;
        if (r_offset * std::cos(theta_orig) + z_offset * std::sin(theta_orig) < 0) { sign = -1; }
        sumr += weight * sign * r_offset;
        sumz += weight * sign * z_offset;
    }

    //  const double sum_tan = sum_tanweight/sum_weight;

    ATH_MSG_VERBOSE("av_z : " << av_z << " av_radii: " << av_radii << " sumr: " << sumr << " sumz: " << sumz);
    if (std::abs(sumr) < 0.000001 || std::abs(sumz) < 0.000001) return old_pars;

    double theta = std::atan2(sumr, sumz);

    if (theta < 0) theta += M_PI;

    double rz0 = calculateRz0(etapattern, phi, theta);

    // ATH_MSG_DEBUG("old method rz0: " << sctheta.apply(av_z,-av_radii) );
    // const double rz0 = sctheta.apply(av_z,-av_radii); // (av_z * sctheta.sn) -
    // av_radii * sctheta.cs;

    std::array<double,4> new_pars {r0,phi,rz0,theta};

    ATH_MSG_DEBUG("updated parameters: r0: " << new_pars[0] << " phi: " << new_pars[1] << " rz0: " << new_pars[2]
                                             << " theta: " << new_pars[3]);
    ATH_MSG_DEBUG("old parameters: r0: " << old_pars[0] << " phi: " << old_pars[1] << " rz0: " << old_pars[2] << " theta: " << old_pars[3]);

    if (msgLvl(MSG::VERBOSE)) {
        ATH_MSG_VERBOSE("phisize: " << phisize << " etasize: " << etasize);
        for (unsigned int i = 0; i < phisize; i++) {
            const Trk::PrepRawData* prd = phipattern->prd(i);
            const Amg::Vector3D& globalposhit = globalPrdPos(prd);
            double distance = MuonHoughMathUtils::signedDistanceToLine(globalposhit[Amg::x], globalposhit[Amg::y], r0, phi);
            ATH_MSG_VERBOSE("distance to updated parameters in xy: " << distance);
            distance = MuonHoughMathUtils::signedDistanceToLine(globalposhit[Amg::x], globalposhit[Amg::y], old_pars[0], old_pars[1]);
            ATH_MSG_VERBOSE("old distance phi hit: " << distance);
        }
        for (unsigned int i = 0; i < etasize; i++) {
            const Trk::PrepRawData* prd = etapattern->prd(i);
            const Amg::Vector3D& globalposhit = globalPrdPos(prd);
            double distance = MuonHoughMathUtils::signedDistanceToLine(globalposhit[Amg::x], globalposhit[Amg::y], r0, phi);
            ATH_MSG_VERBOSE("distance to updated parameters in xy: " << distance);
            distance = MuonHoughMathUtils::signedDistanceToLine(globalposhit[Amg::x], globalposhit[Amg::y], old_pars[0], old_pars[1]);
            ATH_MSG_VERBOSE("old distance eta hit: " << distance);
        }
        for (unsigned int i = 0; i < etasize; ++i) {
            const Trk::PrepRawData* prd = etapattern->prd(i);
            const Amg::Vector3D& globalposhit = globalPrdPos(prd);
            double perp = scphi.apply(globalposhit[Amg::y],
                                      globalposhit[Amg::x]);  // globalposhit.x()*scphi.cs + globalposhit.y()*scphi.sn;
            double distance = MuonHoughMathUtils::signedDistanceToLine(globalposhit[Amg::z], perp, rz0, theta);
            ATH_MSG_VERBOSE("distance to updated parameters in Rz: " << distance);
            distance = MuonHoughMathUtils::signedDistanceToLine(globalposhit[Amg::z], perp, old_pars[2], old_pars[3]);
            ATH_MSG_VERBOSE("old distance: " << distance);
        }
    }
    return new_pars;
}

std::pair<double, double> MuonCombinePatternTool::calculateR0Phi(const Muon::MuonPrdPattern* phipattern,
                                                                 const Muon::MuonPrdPattern* etapattern, double phi_est) const {
    // use eta pattern as well, since phi patterns consist sometimes of only 1
    // station etahit error 200mrad (2Pi/16*2), phi hit 20mrad (should be hough
    // binsize (18 mrad), but prefer ~ factor 10 for stabilility)

    // test if lever_arm > 2 m before updating , if not old values are used

    ATH_MSG_VERBOSE("calculateR0Phi");

    CxxUtils::sincos scphi_est(phi_est);

    const unsigned int etasize = etapattern->numberOfContainedPrds();
    const unsigned int phisize = phipattern->numberOfContainedPrds();

    const Amg::Vector3D& etaglobaldir = etapattern->globalDirection();
    const double phi_etapattern = etaglobaldir.phi();
    CxxUtils::sincos scphi_eta(phi_etapattern);

    const Amg::Vector3D& phiglobaldir = phipattern->globalDirection();
    const Amg::Vector3D& phiglobalpos = phipattern->globalPosition();
    const double phi_phipattern = phiglobaldir.phi();
    CxxUtils::sincos scphi_phi(phi_phipattern);

    const double phi_error_inv = 1. / 20.;
    const double phi_error_inv2 = phi_error_inv * phi_error_inv;
    const double eta_error_inv = 1. / 400.;
    const double eta_error_inv2 = eta_error_inv * eta_error_inv;

    // from MuonHoughPattern::updateParametersRPhi (partial code duplication.. :(
    // )

    double sum_etax{0.}, sum_etay{0.}, sum_phix{0.}, sum_phiy{0.};

    // calculate average point

    for (unsigned int i = 0; i < etasize; i++) {
        const Trk::PrepRawData* prd = etapattern->prd(i);
        const Amg::Vector3D& globalposhit = globalPrdPos(prd);
        sum_etax += globalposhit.x();
        sum_etay += globalposhit.y();
    }

    for (unsigned int i = 0; i < phisize; i++) {
        const Trk::PrepRawData* prd = phipattern->prd(i);
        const Amg::Vector3D& globalposhit = globalPrdPos(prd);
        sum_phix += globalposhit.x();
        sum_phiy += globalposhit.y();
    }

    const double av_x = (eta_error_inv2 * sum_etax + phi_error_inv2 * sum_phix) / (eta_error_inv2 * etasize + phi_error_inv2 * phisize);
    const double av_y = (eta_error_inv2 * sum_etay + phi_error_inv2 * sum_phiy) / (eta_error_inv2 * etasize + phi_error_inv2 * phisize);

    if (msgLvl(MSG::VERBOSE)) { ATH_MSG_VERBOSE(" av_x: " << av_x << " av_y: " << av_y); }

    // calculate weighted sum:

    double sumx {0.}, sumy {0.};

    // keep track of extreme points

    double x_min {0.}, x_max {0.}, y_min {0.}, y_max {0.}, lever_min {0.}, lever_max{0.};

    for (unsigned int i = 0; i < etasize; i++) {
        const Trk::PrepRawData* prd = etapattern->prd(i);
        const Amg::Vector3D& globalposhit = globalPrdPos(prd);
        double x_offset = globalposhit[Amg::x] - av_x;
        double y_offset = globalposhit[Amg::y] - av_y;
        double height_squared = x_offset * x_offset + y_offset * y_offset;
        double weight = height_squared * eta_error_inv2;
        int sign = 1;
        if (x_offset * scphi_est.cs + y_offset * scphi_est.sn < 0) { sign = -1; }
        sumx += weight * sign * x_offset;
        sumy += weight * sign * y_offset;

        if (sign == 1 && height_squared > lever_max) {
            lever_max = height_squared;
            x_max = globalposhit[Amg::x];
            y_max = globalposhit[Amg::y];
        } else if (sign == -1 && height_squared > lever_min) {
            lever_min = height_squared;
            x_min = globalposhit[Amg::x];
            y_min = globalposhit[Amg::y];
        }
    }

    for (unsigned int i = 0; i < phisize; i++) {
        const Trk::PrepRawData* prd = phipattern->prd(i);
        const Amg::Vector3D& globalposhit = globalPrdPos(prd);
        double x_offset = globalposhit[Amg::x] - av_x;
        double y_offset = globalposhit[Amg::y] - av_y;
        double height_squared = x_offset * x_offset + y_offset * y_offset;
        double weight = height_squared * phi_error_inv2;
        int sign = 1;
        if (x_offset * scphi_est.cs + y_offset * scphi_est.sn < 0) { sign = -1; }
        sumx += weight * sign * x_offset;
        sumy += weight * sign * y_offset;

        if (sign == 1 && height_squared > lever_max) {
            lever_max = height_squared;
            x_max = globalposhit[Amg::x];
            y_max = globalposhit[Amg::y];
        } else if (sign == -1 && height_squared > lever_min) {
            lever_min = height_squared;
            x_min = globalposhit[Amg::x];
            y_min = globalposhit[Amg::y];
        }
    }

    ATH_MSG_VERBOSE("av_x : " << av_x << " av_y: " << av_y << " sumx: " << sumx << " sumy: " << sumy);

    if (std::abs(sumx) < 0.000001 || std::abs(sumy) < 0.000001) {
        ATH_MSG_DEBUG(" sum too small to update");

        return std::make_pair(MuonHoughMathUtils::signedDistanceOfLineToOrigin2D(phiglobalpos.x(), phiglobalpos.y(), phi_phipattern),
                              phi_phipattern);
    }

    // lever arm has to be larger than 2 m, else no update:
    if (std::hypot(x_max - x_min , y_max - y_min) < 2000) {
        ATH_MSG_VERBOSE("lever arm too small: av_x : " << std::sqrt((x_max - x_min) * (x_max - x_min) + (y_max - y_min) * (y_max - y_min))
                                                       << " x_max: " << x_max << " x_min: " << x_min << " y_max: " << y_max
                                                       << " y_min: " << y_min);
        return std::make_pair(MuonHoughMathUtils::signedDistanceOfLineToOrigin2D(phiglobalpos.x(), phiglobalpos.y(), phi_phipattern),
                              phi_phipattern);
    }

    double phi_fit = std::atan2(sumy, sumx);
    if (phi_fit > 0) phi_fit -= M_PI;  // phi between 0,-Pi for cosmics!
    CxxUtils::sincos scphi(phi_fit);
    const double r0_fit = scphi.apply(av_x, -av_y);  // av_x * scphi.sn - av_y * scphi.cs;

    return std::make_pair(phi_fit, r0_fit);
}

double MuonCombinePatternTool::calculateRz0(const Muon::MuonPrdPattern* pattern, double phi, double theta) {
    double nhits = pattern->numberOfContainedPrds();
    CxxUtils::sincos sctheta(theta);
    CxxUtils::sincos scphi(phi);

    /*
      x = r_0 sin(phi) + t*cos(phi)sin(theta)
      y = - r_0 cos(phi) + t*sin(phi)sin(theta)
      z = z_0 + t*cos(theta)

      2 methods (average point of average_z0):

      r^2 = x^2+y^2 = r_0^2 + t^2*sin^2(theta)
      (this projects the hit radially onto the line)

      Not so good as the radius of the hits can be smaller than r_0

      method based on:

      x*cos(phi) + y*sin(phi) = t*sin(theta)
      (projects the hit on the line in x-y and calculates the distance to r_0)

      works best
    */

    // method 3:

    double rz0 = 0.;
    for (unsigned int i = 0; i < nhits; i++) {
        const Trk::PrepRawData* prd = pattern->prd(i);
        const Amg::Vector3D& poshit = globalPrdPos(prd);
        int sign = (poshit[Amg::x] * scphi.cs + poshit[Amg::y] * scphi.sn > 0) ? 1 : -1;
        rz0 += poshit[Amg::z] * sctheta.sn - sign * sctheta.cs * poshit.perp();
    }

    if (nhits > 0) rz0 /= nhits;
    return rz0;
}

std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*> MuonCombinePatternTool::updatePatternsForCosmics(
    const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern, const std::array<double,4>&  new_pars) {
    double phi = new_pars[1];
    double theta = new_pars[3];

    CxxUtils::sincos scphi(phi);
    CxxUtils::sincos sctheta(theta);

    double x0 = new_pars[0] * scphi.sn;
    double y0 = -new_pars[0] * scphi.cs;
    double z0_phi = new_pars[2];
    double z0_eta = new_pars[2];
    if (std::abs(sctheta.sn) > 1e-7) {
        z0_phi = (new_pars[2] + new_pars[0] * sctheta.cs) / sctheta.sn;  // z0 belonging to (x0,y0)
        z0_eta = new_pars[2] / sctheta.sn;                               // z0 of rz0
    }

    const Amg::Vector3D& globalDir = Amg::Vector3D(scphi.cs * sctheta.sn, scphi.sn * sctheta.sn, sctheta.cs);
    const Amg::Vector3D& globalPosPhi = Amg::Vector3D(x0, y0, z0_phi);
    const Amg::Vector3D& globalPosEta = Amg::Vector3D(x0, y0, z0_eta);

    Muon::MuonPrdPattern* updatedphipattern = new Muon::MuonPrdPattern(globalPosPhi, globalDir);
    Muon::MuonPrdPattern* updatedetapattern = new Muon::MuonPrdPattern(globalPosEta, globalDir);

    for (unsigned int phihitnr = 0; phihitnr < phipattern->numberOfContainedPrds(); phihitnr++) {
        updatedphipattern->addPrd(phipattern->prd(phihitnr));
    }  // size phi pattern

    for (unsigned int etahitnr = 0; etahitnr < etapattern->numberOfContainedPrds(); etahitnr++) {
        updatedetapattern->addPrd(etapattern->prd(etahitnr));
    }  // size eta pattern

    return (std::make_pair(updatedphipattern, updatedetapattern));
}

const Amg::Vector3D& MuonCombinePatternTool::globalPrdPos(const Trk::PrepRawData* prd) {
    const Muon::MuonCluster* cprd = dynamic_cast<const Muon::MuonCluster*>(prd);
    if (cprd) return cprd->globalPosition();

    const Muon::MdtPrepData* mdtprd = dynamic_cast<const Muon::MdtPrepData*>(prd);
    if (mdtprd) return mdtprd->globalPosition();

    return prd->detectorElement()->surface(prd->identify()).center();
}

MuonPatternCombinationCollection* MuonCombinePatternTool::makePatternCombinations(const MuonPrdPatternCollection* muonpatterns) const {
    ATH_MSG_DEBUG("makePatternCombinations");

    MuonPatternCombinationCollection* patterncombinations = new MuonPatternCombinationCollection();

    MuonPrdPatternCollection::const_iterator pit = muonpatterns->begin();
    MuonPrdPatternCollection::const_iterator pit_end = muonpatterns->end();

    for (; pit != pit_end; ++pit) {
        const Amg::Vector3D& roadmom = (*pit)->globalDirection();
        const Amg::Vector3D& roadpos = (*pit)->globalPosition();
        ATH_MSG_DEBUG("phi: " << roadmom.phi() << " eta: " << roadmom.eta());
        ATH_MSG_DEBUG("x: " << roadpos.x() << " y: " << roadpos.y() << " z: " << roadpos.z());

        // sort pattern per chamber
        std::map<Identifier, std::vector<const Trk::PrepRawData*>> chamberMap;
        std::map<Identifier, std::vector<const Trk::PrepRawData*>>::iterator chit;
        std::map<Identifier, std::vector<const Trk::PrepRawData*>>::iterator chit_end = chamberMap.end();
        for (unsigned int i = 0; i < (*pit)->numberOfContainedPrds(); i++) {
            const Trk::PrepRawData* prd = (*pit)->prd(i);
            Identifier channelId = prd->identify();
            Identifier moduleId;
            if (m_idHelperSvc->isMdt(channelId)) {
                moduleId = m_idHelperSvc->mdtIdHelper().elementID(channelId);
            } else if (m_idHelperSvc->isCsc(channelId)) {
                moduleId = m_idHelperSvc->cscIdHelper().elementID(channelId);
            } else if (m_idHelperSvc->isTgc(channelId)) {
                moduleId = m_idHelperSvc->tgcIdHelper().elementID(channelId);
            } else if (m_idHelperSvc->isRpc(channelId)) {
                moduleId = m_idHelperSvc->rpcIdHelper().elementID(channelId);
            } else {
                ATH_MSG_ERROR("prd is not a muonhit?!");
            }

            chit = chamberMap.find(moduleId);
            if (chit != chit_end) {
                (*chit).second.push_back(prd);
            } else {
                std::vector<const Trk::PrepRawData*> prdVec;
                prdVec.reserve(10);
                prdVec.push_back(prd);
                chamberMap[moduleId] = prdVec;
            }
        }

        // build chamberintersect vector
        std::vector<Muon::MuonPatternChamberIntersect> mpciVec;
        mpciVec.reserve(chamberMap.size());
        Amg::Vector3D patpose;
        Amg::Vector3D patdire;
        chit = chamberMap.begin();
        for (; chit != chit_end; ++chit) {
            const Trk::PrepRawData* prd = *((*chit).second.begin());
            // Identifier id = prd->identify();
            const Amg::Vector3D& globalpos = globalPrdPos(prd);

            if (m_use_cosmics) {
                // not flip

                patdire = roadmom.unit();
                patpose = roadpos;
            } else {
                MuonHoughMathUtils::extrapolateCurvedRoad(roadpos, roadmom, globalpos, patpose, patdire);
            }

            Muon::MuonPatternChamberIntersect mpci = Muon::MuonPatternChamberIntersect(patpose, patdire, (*chit).second);
            mpciVec.push_back(mpci);
        }

        Amg::Vector3D tvertex = Amg::Vector3D(0., 0., 0.);
        Trk::TrackParameters* parameters = new Trk::Perigee(roadpos, roadmom, 1., tvertex);  // if -1 then charge flipped anyway
        Muon::MuonPatternCombination* combination = new Muon::MuonPatternCombination(parameters, mpciVec);
        patterncombinations->push_back(combination);
    }
    return patterncombinations;
}

int MuonCombinePatternTool::overlap(
    const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern, std::vector<const Trk::PrepRawData*>& hits_to_be_added,
    /** phi eta association map, eta prds are key*/
    const std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>* phiEtaHitAssMap) const {
    /** method no longer used */

    if (!phiEtaHitAssMap) return 0;

    int overlap = 0;
    std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess> phihits;
    for (unsigned int i = 0; i < phipattern->numberOfContainedPrds(); i++) { phihits.insert(phipattern->prd(i)); }

    std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>::const_iterator it;
    ATH_MSG_DEBUG("number of prds in eta pattern: " << etapattern->numberOfContainedPrds());
    for (unsigned int i = 0; i < etapattern->numberOfContainedPrds(); i++) {
        const Trk::PrepRawData* prd = etapattern->prd(i);
        // check on type of prd?
        const Muon::MuonCluster* muoncluster = dynamic_cast<const Muon::MuonCluster*>(prd);
        if (muoncluster) {
            it = phiEtaHitAssMap->find(prd);
            if (it != phiEtaHitAssMap->end()) {  // check if hit has associated hit
                std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>::iterator set_it = (*it).second.begin();
                for (; set_it != (*it).second.end(); ++set_it) {
                    if (phihits.find(*set_it) != phihits.end()) {  // check if associated hit is on phi pattern
                        ATH_MSG_VERBOSE("overlapping hit found!");
                        overlap++;
                    } else {  // associated hit not on phi pattern
                        hits_to_be_added.push_back(*set_it);
                        ATH_MSG_VERBOSE("Associated Phi Hit Added to Pattern");
                    }
                }
            } else {
                ATH_MSG_VERBOSE("prd not associated to any phi hit: " << prd);
            }
        }
    }
    return overlap;
}

bool MuonCombinePatternTool::subset(const Muon::MuonPrdPattern* pattern1, const Muon::MuonPrdPattern* pattern2) {
    /** is pattern1 a complete subset of other pattern2? */

    // first check if pattern 1 is not larger than 2:
    if (pattern1->numberOfContainedPrds() > pattern2->numberOfContainedPrds()) { return false; }

    bool subset = true;

    std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess> hits;
    for (unsigned int hitnr = 0; hitnr < pattern2->numberOfContainedPrds(); hitnr++) { hits.insert(pattern2->prd(hitnr)); }

    for (unsigned int hitnr = 0; hitnr < pattern1->numberOfContainedPrds(); hitnr++) {
        if (hits.find(pattern1->prd(hitnr)) == hits.end()) {
            subset = false;
            break;
        }
    }
    return subset;
}

bool MuonCombinePatternTool::subset(std::pair<std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>,
                                              std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>& candidate1,
                                    std::pair<std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>,
                                              std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>& candidate2) {
    /** is candidate1 a complete subset of other candidate2? */

    // first check if pattern 1 is not larger than 2:
    if (candidate1.first.size() > candidate2.first.size() || candidate1.second.size() > candidate2.second.size()) { return false; }

    std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>::iterator set_it;

    for (set_it = candidate1.first.begin(); set_it != candidate1.first.end(); ++set_it) {
        if (candidate2.first.find(*set_it) == candidate2.first.end()) { return false; }
    }
    for (set_it = candidate1.second.begin(); set_it != candidate1.second.end(); ++set_it) {
        if (candidate2.second.find(*set_it) == candidate2.second.end()) { return false; }
    }

    return true;
}

void MuonCombinePatternTool::printPattern(const Muon::MuonPrdPattern* muonpattern) const {
    if (msgLvl(MSG::VERBOSE)) {
        ATH_MSG_VERBOSE("Printout of Pattern: ");

        const Amg::Vector3D& pos = muonpattern->globalPosition();
        const Amg::Vector3D& dir = muonpattern->globalDirection();

        ATH_MSG_VERBOSE("pos: x: " << pos.x() << " y: " << pos.y() << " z: " << pos.z());
        ATH_MSG_VERBOSE("dir: x: " << dir.x() << " y: " << dir.y() << " z: " << dir.z());
        ATH_MSG_VERBOSE("phi: " << dir.phi() << " theta: " << dir.theta() << " rz0: " << pos.z() * std::sin(dir.theta()));

        for (unsigned int k = 0; k < muonpattern->numberOfContainedPrds(); k++) {
            const Trk::PrepRawData* prd = muonpattern->prd(k);
            const Muon::MdtPrepData* mdtprd = dynamic_cast<const Muon::MdtPrepData*>(prd);
            if (mdtprd) {
                const Trk::Surface& surface = mdtprd->detectorElement()->surface(mdtprd->identify());
                const Amg::Vector3D& gpos = surface.center();
                ATH_MSG_VERBOSE("mdt " << k << " x: " << gpos.x() << " y: " << gpos.y() << " z: " << gpos.z());
            } else if (!mdtprd) {
                const Muon::MuonCluster* muoncluster = dynamic_cast<const Muon::MuonCluster*>(prd);
                if (muoncluster) {
                    const Amg::Vector3D& gpos = muoncluster->globalPosition();
                    ATH_MSG_VERBOSE("cluster " << k << " x: " << gpos.x() << " y: " << gpos.y() << " z: " << gpos.z());
                }
                if (!muoncluster) { ATH_MSG_VERBOSE("no muon prd? "); }
            }
        }
    }
}

const Muon::MuonPrdPattern* MuonCombinePatternTool::updatePhiPattern(const Muon::MuonPrdPattern* phipattern,
                                                                     std::vector<const Trk::PrepRawData*> missedphihits) const {
    std::unique_ptr<Muon::MuonPrdPattern> updatedphipattern = std::make_unique< Muon::MuonPrdPattern>(phipattern->globalPosition(), phipattern->globalDirection());
    for (unsigned int phihitnr = 0; phihitnr < phipattern->numberOfContainedPrds(); phihitnr++) {
        updatedphipattern->addPrd(phipattern->prd(phihitnr));
        // ATH_MSG_DEBUG(" prd: " << phipattern->prd(phihitnr) );
    }  // size phi pattern

    std::vector<const Trk::PrepRawData*>::iterator it = missedphihits.begin();
    for (; it != missedphihits.end(); ++it) {
        // check on compatibility?
        updatedphipattern->addPrd(*it);
    }

    // clean updated phi pattern:

    return cleanPhiPattern(updatedphipattern.get());
 }

Muon::MuonPrdPattern* MuonCombinePatternTool::cleanPhiPattern(const Muon::MuonPrdPattern* phipattern) const {
    const Amg::Vector3D& olddir = phipattern->globalDirection();
    const double theta = olddir.theta();
    const unsigned int size = phipattern->numberOfContainedPrds();

    if (msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG("Start Phi hits cleaning with " << size << " hits "
                                                      << " theta " << theta);
        const Amg::Vector3D& oldpos = phipattern->globalPosition();
        double r0 = MuonHoughMathUtils::signedDistanceOfLineToOrigin2D(oldpos.x(), oldpos.y(), olddir.phi());
        ATH_MSG_DEBUG("Start Phi: " << olddir.phi() << " r0: " << r0);
    }

    // need internal class to be able to remove hits fast
    std::unique_ptr<MuonHoughPattern> newpattern = std::make_unique< MuonHoughPattern>(MuonHough::hough_xy, true);  // patterns owns hits
    for (unsigned int phihitnr = 0; phihitnr < size; phihitnr++) { newpattern->addHit(new MuonHoughHit(phipattern->prd(phihitnr))); }
    newpattern->updateParametersRPhi(m_use_cosmics);
    double phi = newpattern->getEPhi();
    double r0 = newpattern->getERPhi();

    CxxUtils::sincos scphi(phi);

    constexpr int number_of_iterations = 4;
    std::array<double,number_of_iterations> cutvalues {1000., 500., 250., 125.};

    if (m_use_cosmics) {
        cutvalues[0] = 5000.;
        cutvalues[1] = 2500.;
        cutvalues[2] = 1250.;
        cutvalues[3] = 1250.;
    }

    for (int it = 0; it < number_of_iterations; it++) {
        ATH_MSG_VERBOSE("iteration " << it << " cutvalue: " << cutvalues[it]);
        bool change = true;
        while (change) {
            ATH_MSG_VERBOSE("size: " << newpattern->size() << " r0: " << r0 << " phi: " << phi);

            double max_dist = 0.;
            unsigned int max_i = 99999;
            for (unsigned int i = 0; i < newpattern->size(); i++) {
                double dist =
                    scphi.apply(newpattern->getHitx(i), -newpattern->getHity(i)) - r0;  //  newpattern->getHitx(i) * scphi.sn -
                                                                                        //  newpattern->getHity(i) * scphi.cs - r0;
                ATH_MSG_VERBOSE("Dist: " << dist);
                if (fabs(dist) > fabs(max_dist)) {
                    max_dist = dist;
                    max_i = i;
                }
            }
            if (fabs(max_dist) < cutvalues[it]) {
                change = false;
            } else {
                newpattern->removeHit(max_i);
                newpattern->updateParametersRPhi(m_use_cosmics);
                phi = newpattern->getEPhi();
                r0 = newpattern->getERPhi();
                scphi = CxxUtils::sincos(phi);
            }
        }
    }

    ATH_MSG_DEBUG("Final size: " << newpattern->size() << " r0: " << r0 << " phi: " << phi);

    // update parameters rz (not very important as later overwritten)
    // put r0 to IP for collisions

    double thetanew = 0.;
    double r0_new = 1.;  // put 1 mm r0 value

    if (m_use_cosmics) { r0_new = r0; }

    unsigned int nPatterns = newpattern->size();
    for (unsigned int i = 0; i < nPatterns; i++) { thetanew += newpattern->getTheta(i); }

    if (nPatterns > 0) thetanew /= nPatterns;

    double z0_new = 0.;

    double x0_new = r0_new * scphi.sn;
    double y0_new = -r0_new * scphi.cs;
    CxxUtils::sincos sctheta(thetanew);

    const Amg::Vector3D pos = Amg::Vector3D(x0_new, y0_new, z0_new);
    const Amg::Vector3D dir = Amg::Vector3D(sctheta.sn * scphi.cs, sctheta.sn * scphi.sn, sctheta.cs);

    Muon::MuonPrdPattern* cleanpattern = new Muon::MuonPrdPattern(pos, dir);

    for (unsigned int i = 0; i < newpattern->size(); i++) { cleanpattern->addPrd(newpattern->getPrd(i)); }

    return cleanpattern;
}

void MuonCombinePatternTool::addCandidate(
    const Muon::MuonPrdPattern* etapattern, const Muon::MuonPrdPattern* phipattern,
    std::vector<std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*>>& candidates, bool add_asspattern,
    std::vector<std::unique_ptr<const Muon::MuonPrdPattern>>& patternsToDelete,
    const std::map<const Trk::PrepRawData*, std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>>* phiEtaHitAssMap) const {
    if (!m_use_cosmics || !m_splitpatterns) {
        candidates.emplace_back(etapattern, phipattern);
        return;
    }

    std::vector<std::pair<Muon::MuonPrdPattern*, Muon::MuonPrdPattern*>> splitpatterns = splitPatternsCylinder(phipattern, etapattern);

    if (splitpatterns.empty()) {
        candidates.emplace_back(etapattern, phipattern);
    }

    else {
        for (unsigned int i = 0; i < splitpatterns.size(); i++) {
            // skip when empty eta pattern , possible duplication when associated phi
            // pattern is found, but then will be cleaned later
            if (splitpatterns[i].second->numberOfContainedPrds() == 0) {
                delete splitpatterns[i].first;
                delete splitpatterns[i].second;
            } else {
                candidates.emplace_back(splitpatterns[i].second, splitpatterns[i].first);

                // these associated phi patterns should be deleted at end of routine:
                patternsToDelete.emplace_back(splitpatterns[i].first);
                patternsToDelete.emplace_back(splitpatterns[i].second);
            }
        }
    }

    // make associated pattern don't split eta pattern yet, but split based on phi
    // of ass. pattern bool asspattern_added = false;
    if (add_asspattern) {
        const Muon::MuonPrdPattern* assphipattern = makeAssPhiPattern(etapattern, phiEtaHitAssMap, true);
        if (assphipattern) {
            // print associated pattern:
            if (msgLvl(MSG::VERBOSE)) {
                ATH_MSG_VERBOSE("Associated Pattern: ");
                printPattern(assphipattern);
            }

            // update parameters:

            std::array<double,4> new_pars = updateParametersForCosmics(assphipattern, etapattern);
            std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*> updatedpatterns =
                updatePatternsForCosmics(assphipattern, etapattern, new_pars);
            delete assphipattern;

            assphipattern = updatedpatterns.first;
            etapattern = updatedpatterns.second;

            patternsToDelete.emplace_back(updatedpatterns.first);
            patternsToDelete.emplace_back(updatedpatterns.second);
        
            std::vector<std::pair<Muon::MuonPrdPattern*, Muon::MuonPrdPattern*>> splitpatterns_ass =
                splitPatternsCylinder(assphipattern, etapattern);

            if (splitpatterns_ass.empty()) {
                candidates.emplace_back(etapattern, assphipattern);
            }

            else {
                for (unsigned int i = 0; i < splitpatterns_ass.size(); i++) {
                    if (splitpatterns_ass[i].first->numberOfContainedPrds() == 0 ||
                        splitpatterns_ass[i].second->numberOfContainedPrds() == 0) {
                        delete splitpatterns_ass[i].first;
                        delete splitpatterns_ass[i].second;
                    } else {
                        candidates.emplace_back(splitpatterns_ass[i].second, splitpatterns_ass[i].first);

                        // these associated phi patterns should be deleted at end of
                        // routine:
                        patternsToDelete.emplace_back(splitpatterns_ass[i].first);
                        patternsToDelete.emplace_back(splitpatterns_ass[i].second);
                    }
                }
            }
        }
    }
}

void MuonCombinePatternTool::cleanCandidates(std::vector<std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*>>& candidates) {
    std::vector<std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*>>::iterator it1;
    std::vector<std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*>>::iterator it2;
    using SetOfPrd_t = std::set<const Trk::PrepRawData*, Muon::IdentifierPrdLess>;
    // map between set of prd's (eta and phi) and candidates , stored for speed
    std::map<std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*>, std::pair<SetOfPrd_t, SetOfPrd_t>> hitsMap;

    // fill map
    for (it1 = candidates.begin(); it1 != candidates.end(); ++it1) {
        SetOfPrd_t etahits;
        for (unsigned int hitnr = 0; hitnr < (*it1).first->numberOfContainedPrds(); hitnr++) { etahits.insert((*it1).first->prd(hitnr)); }
        SetOfPrd_t phihits;
        if ((*it1).second) {  // phi pattern might be 0!
            for (unsigned int hitnr = 0; hitnr < (*it1).second->numberOfContainedPrds(); hitnr++) {
                phihits.insert((*it1).second->prd(hitnr));
            }
        }
        hitsMap.insert(std::make_pair((*it1), std::make_pair(etahits, phihits)));
    }
    for (it1 = candidates.begin(); it1 != candidates.end(); ++it1) {
        std::pair<SetOfPrd_t, SetOfPrd_t>& hits1 = hitsMap[(*it1)];
        it2 = it1 + 1;
        while (it2 != candidates.end()) {
            std::pair<SetOfPrd_t, SetOfPrd_t>& hits2 = hitsMap[(*it2)];
            if (subset((hits2), (hits1))) {         // 2 subset of 1, remove 2 // in case of
                                                    // equality best (earliest) is kept
                it2 = candidates.erase(it2);        // it2 points to next item, it1 not invalidated!
            } else if (subset((hits1), (hits2))) {  // 1 subset of 2, remove 1
                it1 = candidates.erase(it1);        // it1 points to next item, it2 invalidated!
                it2 = it1 + 1;
                //cppcheck-suppress selfAssignment
                hits1 = hitsMap[(*it1)];  // redefine hits1
            } else {
                ++it2;
            }
        }
    }
}
