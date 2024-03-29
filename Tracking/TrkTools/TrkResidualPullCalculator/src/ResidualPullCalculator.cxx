/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ResidualPullCalculator.cxx
//   Source file for class ResidualPullCalculator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Sebastian.Fleischmann@cern.ch
///////////////////////////////////////////////////////////////////

#include "ResidualPullCalculator.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventUtils/MeasurementTypeID.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkTrack/AlignmentEffectsOnTrack.h"
#include <Eigen/Geometry> 
#include <algorithm>
#include <cmath>

//////////////////////////////
/// constructor
//////////////////////////////
Trk::ResidualPullCalculator::ResidualPullCalculator(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type,name,parent),
m_SCTresidualTool("InDet::SCT_ResidualPullCalculator/SCT_ResidualPullCalculator"),
m_RPCresidualTool("Muon::RPC_ResidualPullCalculator/RPC_ResidualPullCalculator"),
m_TGCresidualTool("Muon::TGC_ResidualPullCalculator/TGC_ResidualPullCalculator"),
m_idHelper(nullptr) {
    declareInterface<IResidualPullCalculator>(this);
    declareProperty("ResidualPullCalculatorForSCT", m_SCTresidualTool,  "Tool to calculate residuals and pulls in the SCT (including module rotation)");
    declareProperty("ResidualPullCalculatorForRPC", m_RPCresidualTool,  "Tool to calculate residuals and pulls in the RPC (including phi/eta detection)");
    declareProperty("ResidualPullCalculatorForTGC", m_TGCresidualTool,  "Tool to calculate residuals and pulls in the SCT (including module rotation)");

}

///////////////////////////////
/// initialize
///////////////////////////////
StatusCode Trk::ResidualPullCalculator::initialize() {
    //StatusCode sc = AthAlgTool::initialize();
    //if (sc.isFailure()) return sc;
    //StatusCode sc=StatusCode::SUCCESS;

    // ---------------------------------------
    //Set up ATLAS ID helper to be able to identify the RIO's det-subsystem.
    ATH_CHECK(detStore()->retrieve(m_idHelper, "AtlasID"));
    // ----------------------------------
    // get the SCT residual/pull calculator
    if ( ! m_SCTresidualTool.empty() ) {
        ATH_CHECK( m_SCTresidualTool.retrieve());
    } else {
        ATH_MSG_DEBUG ("No residual calculator for SCT given, will ignore SCT measurements!");
    }

    // ----------------------------------
    // get the RPC residual/pull calculator
    if ( ! m_RPCresidualTool.empty() ) {
        ATH_CHECK( m_RPCresidualTool.retrieve());
    } else {
        ATH_MSG_DEBUG ("No residual calculator for RPC given, will ignore RPC measurements!");
    }

    // ----------------------------------
    // get the TGC residual/pull calculator
    if ( ! m_TGCresidualTool.empty() ) {
        ATH_CHECK( m_TGCresidualTool.retrieve());
    } else {
        ATH_MSG_DEBUG ("No residual calculator for TGC given, will ignore TGC measurements!");
    }
  
    return StatusCode::SUCCESS;
}

StatusCode Trk::ResidualPullCalculator::finalize() {
    ATH_MSG_DEBUG ("starting finalize() in " << name());
    return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
//  calc residuals with determination of detector/MBase type
////////////////////////////////////////////////////////////////////////////////
std::array<double,5> 
Trk::ResidualPullCalculator::residuals(
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkPar,
    const Trk::ResidualPull::ResidualType resType,
    const Trk::TrackState::MeasurementType detType) const {

    std::array<double,5> residuals{-999.,-999.,-999.,-999.,-999};
    Trk::TrackState::MeasurementType measType = detType;
    if (detType == Trk::TrackState::unidentified) {
        Trk::MeasurementTypeID helper = MeasurementTypeID(m_idHelper);
        measType = helper.defineType(measurement);
    }
    switch(measType) {
        case Trk::TrackState::Pixel:
            ATH_MSG_VERBOSE ("Pixel dim calculation ");
            // calc residual and pull for the second coordinate, first coord postponed to 1-dim case
            residuals[Trk::loc1] = measurement->localParameters()[Trk::loc1] - trkPar->parameters()[Trk::loc1];
            residuals[Trk::loc2] = measurement->localParameters()[Trk::loc2] - trkPar->parameters()[Trk::loc2];
        break;
        case Trk::TrackState::TRT:
        case Trk::TrackState::MDT:
        case Trk::TrackState::CSC:
        case Trk::TrackState::MM:
            ATH_MSG_VERBOSE ("One dim calculation ");
            // 1-dim measurement
            // calc residual and pull for the first coordinate
            residuals[Trk::loc1] = measurement->localParameters()[Trk::loc1] - trkPar->parameters()[Trk::loc1];
            break;
        case Trk::TrackState::STGC:
            ATH_MSG_VERBOSE ("One dim calculation ");
            // 1-dim measurement
            // calc residual and pull for the first coordinate
            if( measurement->localParameters().contains(Trk::loc1) )
                residuals[Trk::loc1] = measurement->localParameters()[Trk::loc1] - trkPar->parameters()[Trk::loc1];
            else 
                residuals[Trk::loc2] = measurement->localParameters()[Trk::loc2] - trkPar->parameters()[Trk::loc2];
            break;
        case Trk::TrackState::SCT:
            // special case, has to be handed down to the SCT tool
            if ( ! m_SCTresidualTool.empty() ) {
                ATH_MSG_VERBOSE ("Calling SCT tool ");
                return m_SCTresidualTool->residuals(measurement, trkPar, resType, Trk::TrackState::SCT);
            } else {
                ATH_MSG_WARNING ("No SCT ResidualPullCalculator given, cannot calculate residual and pull for SCT measurement!");
            }
            break;
        case Trk::TrackState::RPC:
            // special case, has to be handed down to the RPC tool
            if ( ! m_RPCresidualTool.empty() ) {
                ATH_MSG_VERBOSE ("Calling RPC tool ");
                return m_RPCresidualTool->residuals(measurement, trkPar, resType, Trk::TrackState::RPC);
            } else {
                ATH_MSG_WARNING ("No RPC ResidualPullCalculator given, cannot calculate residual and pull for RPC measurement!");
            }
            break;
        case Trk::TrackState::TGC:
            // special case, has to be handed down to the TGC tool
            if ( ! m_TGCresidualTool.empty() ) {
                ATH_MSG_VERBOSE ("Calling TGC tool ");  
                return m_TGCresidualTool->residuals(measurement, trkPar, resType, Trk::TrackState::TGC);
            } else {
                ATH_MSG_WARNING ("No TGC ResidualPullCalculator given, cannot calculate residual and pull for TGC measurement!");
            }
            break;
        case Trk::TrackState::Segment:
        case Trk::TrackState::Pseudo:
        case Trk::TrackState::Vertex:
        case Trk::TrackState::SpacePoint:
        default:
            ATH_MSG_VERBOSE ("Bit-key calculation ");
            ParamDefsAccessor PDA;
 
            // PMs, Segments etc: use LocalParameters bit-key scheme
            for (unsigned int i=0; i<5; ++i) {
                Trk::ParamDefs iPar = PDA.pardef[i];
                if (measurement->localParameters().contains(iPar)) {
                    residuals[i] = measurement->localParameters()[iPar]
                        - trkPar->parameters()[iPar];
                }
            }
            break;
    }
    return residuals;

}

////////////////////////////////////////////////////////////////////////////////
//  calc residual and pull with determination of detector/MBase type
////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<Trk::ResidualPull> Trk::ResidualPullCalculator::residualPull(
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* trkPar,
    const Trk::ResidualPull::ResidualType resType,
    const Trk::TrackState::MeasurementType detType) const {

    if (!measurement || !trkPar) return nullptr;

    // if no covariance for the track parameters is given the pull calculation is not valid
    bool pullIsValid = trkPar->covariance();

    Trk::TrackState::MeasurementType measType = detType;
    if (detType == Trk::TrackState::unidentified) {
        Trk::MeasurementTypeID helper = MeasurementTypeID(m_idHelper);
        measType = helper.defineType(measurement);
    }
    unsigned int dimOfLocPars = (unsigned int)measurement->localParameters().dimension();
    std::vector<double> residual(dimOfLocPars);
    std::vector<double> pull(dimOfLocPars);

    // has to live here as it does not compile if code is switch statement
    ParamDefsAccessor PDA;
    unsigned int iColRow=0;

    ATH_MSG_VERBOSE ("Calculating residual for type " << measType << " dimension " << dimOfLocPars);

    // do the calculations for the different detector types
    switch(measType) {
        case Trk::TrackState::Pixel:
        ATH_MSG_VERBOSE ("Pixel dim calculation ");
        // calc residual and pull for the second coordinate, first coord postponed to 1-dim case
        residual[Trk::loc2] = measurement->localParameters()[Trk::loc2] - trkPar->parameters()[Trk::loc2];
        if (pullIsValid) {
            pull[Trk::loc2] = calcPull(residual[Trk::loc2],
            measurement->localCovariance()(Trk::loc2,Trk::loc2),
            (*trkPar->covariance())(Trk::loc2,Trk::loc2),
            resType);
        } else {
            pull[Trk::loc2] = calcPull(residual[Trk::loc2],
            measurement->localCovariance()(Trk::loc2,Trk::loc2),
            0,
            resType);
        }
        // do not break here, but also fill the first coordinate...
        /* FALLTHROUGH */
        case Trk::TrackState::TRT:
        case Trk::TrackState::MDT:
        case Trk::TrackState::CSC:
        case Trk::TrackState::STGC:
        case Trk::TrackState::MM:
        ATH_MSG_VERBOSE ("One dim calculation ");
        // 1-dim measurement
        // calc residual and pull for the first coordinate
        residual[Trk::loc1] = measurement->localParameters()[Trk::loc1] - trkPar->parameters()[Trk::loc1];
        if (pullIsValid) {
            pull[Trk::loc1] = calcPull(residual[Trk::loc1],
            measurement->localCovariance()(Trk::loc1,Trk::loc1),
            (*trkPar->covariance())(Trk::loc1,Trk::loc1),
            resType);
        } else {
            pull[Trk::loc1] = calcPull(residual[Trk::loc1],
            measurement->localCovariance()(Trk::loc1,Trk::loc1),
            0,
            resType);
        }
        break;
        case Trk::TrackState::SCT:
        // special case, has to be handed down to the SCT tool
        if ( ! m_SCTresidualTool.empty() ) {
            ATH_MSG_VERBOSE ("Calling SCT tool ");
            return m_SCTresidualTool->residualPull(measurement, trkPar, resType, Trk::TrackState::SCT);
        } else {
            ATH_MSG_WARNING ("No SCT ResidualPullCalculator given, cannot calculate residual and pull for SCT measurement!");
            return nullptr;
        }
        break;
        case Trk::TrackState::RPC:
        // special case, has to be handed down to the RPC tool
        if ( ! m_RPCresidualTool.empty() ) {
            ATH_MSG_VERBOSE ("Calling RPC tool ");
            return m_RPCresidualTool->residualPull(measurement, trkPar, resType, Trk::TrackState::RPC);
        } else {
            ATH_MSG_WARNING ("No RPC ResidualPullCalculator given, cannot calculate residual and pull for RPC measurement!");
            return nullptr;
        }
        break;
        case Trk::TrackState::TGC:
        // special case, has to be handed down to the TGC tool
        if ( ! m_TGCresidualTool.empty() ) {
            ATH_MSG_VERBOSE ("Calling TGC tool ");
            return m_TGCresidualTool->residualPull(measurement, trkPar, resType, Trk::TrackState::TGC);
        } else {
            ATH_MSG_WARNING ("No TGC ResidualPullCalculator given, cannot calculate residual and pull for TGC measurement!");
            return nullptr;
        }
        break;
        case Trk::TrackState::Segment:
        case Trk::TrackState::Pseudo:
        case Trk::TrackState::Vertex:
        case Trk::TrackState::SpacePoint:
        ATH_MSG_VERBOSE ("Bit-key calculation ");

        // PMs, Segments etc: use LocalParameters bit-key scheme
        for (unsigned int i=0; i<5; ++i) {
            Trk::ParamDefs iPar = PDA.pardef[i];
            if (measurement->localParameters().contains(iPar)) {
                residual[iColRow] = measurement->localParameters()[iPar]
                    - trkPar->parameters()[iPar];
                if (pullIsValid)
                    pull[iColRow]     = calcPull(residual[iColRow],
                measurement->localCovariance()(PDA.pardef[iColRow],PDA.pardef[iColRow]),
                (*trkPar->covariance())(PDA.pardef[iColRow],PDA.pardef[iColRow]),
                resType);
                ++iColRow;
            }
        }
        break;
        default:
        ATH_MSG_VERBOSE ("Default calculation ");
        // use HEPvector to calculate the residual
        // r = m - H.p
        Amg::VectorX HEPresidual = measurement->localParameters() - (measurement->localParameters().expansionMatrix() * trkPar->parameters());
        residual.resize(HEPresidual.rows());
        pull.resize(HEPresidual.rows());
        for (int i = 0; i < HEPresidual.rows(); i++) {
            residual[i] = HEPresidual[i];
        }
    
    }
    return std::make_unique<Trk::ResidualPull>(
        std::move(residual), std::move(pull), pullIsValid, resType,
        measurement->localParameters().parameterKey());
}

////////////////////////////////////////////////////////////////////////////////
//  calc residual and pull with determination of detector/MBase type
////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<Trk::ResidualPull> Trk::ResidualPullCalculator::residualPull(
    const Trk::MeasurementBase* measurement,
    const Trk::TrackParameters* originalTrkPar,
    const Trk::ResidualPull::ResidualType resType,
    const Trk::TrackState::MeasurementType detType,
    const std::vector<const Trk::AlignmentEffectsOnTrack*>& aeots) const {

    //bool veryVerbose = true;

    Trk::TrackState::MeasurementType measType = detType;
    if (detType == Trk::TrackState::unidentified) {
        Trk::MeasurementTypeID helper = MeasurementTypeID(m_idHelper);
        measType = helper.defineType(measurement);
    }

    // time to shift the parameter - do this rather than the measurement so we can call the original method.
    Amg::Vector3D loc3Dframe;
    Amg::Vector2D localPos;
    Amg::Vector2D localPosSimple;
    Amg::Vector3D globalPos;
    AmgVector(5) parameters;
    
    Trk::TrackParameters*  trkPar = originalTrkPar->clone();
    parameters =  trkPar->parameters();
   
    ATH_MSG_VERBOSE( " ResidualPullCalculator aeots size  " <<  aeots.size() << " parameters[0] " <<  parameters[0] ); 
    double residual = measurement->localParameters()[Trk::loc1] - trkPar->parameters()[Trk::loc1];
    ATH_MSG_VERBOSE( " parameters[0] " << parameters[0] << " trkPar->parameters()[Trk::loc1] " << trkPar->parameters()[Trk::loc1] << " measurement->localParameters()[Trk::loc1] " << measurement->localParameters()[Trk::loc1] << " resi " << residual );

    localPos[0] = parameters[0];
    localPosSimple[0] = parameters[0];
    for ( const auto & aeot : aeots ){
        ATH_MSG_VERBOSE( " ResidualPullCalculator aeots deltaTranslation  " <<  aeot->deltaTranslation() << " angle " << aeot->deltaAngle()); 

        Trk::DistanceSolution solution = aeot->associatedSurface().straightLineDistanceEstimate(originalTrkPar->position(),originalTrkPar->momentum().unit());
        double distance = solution.currentDistance(true); 
//      calculate sign of distance
        Amg::Vector3D* displacementVector  = new Amg::Vector3D(originalTrkPar->position().x()-aeot->associatedSurface().center().x(),originalTrkPar->position().y()-aeot->associatedSurface().center().y(),originalTrkPar->position().z()-aeot->associatedSurface().center().z()); 
        if(displacementVector->dot(originalTrkPar->momentum().unit())<0) distance = - distance;

        if(measType == Trk::TrackState::MDT) {
// MDT 
          double distanceX = distance*originalTrkPar->momentum().unit().x();
          double distanceY = distance*originalTrkPar->momentum().unit().y();
          const double originalPhi=originalTrkPar->momentum().phi();
          double distanceR = std::cos(originalPhi)*distanceX + std::sin(originalPhi)*distanceY; //< Is this quicker than hypotenuse?
          ATH_MSG_VERBOSE( " originalTrkPar->position() x " << originalTrkPar->position().x() << " y " << originalTrkPar->position().y() << " z " << originalTrkPar->position().z() );
          ATH_MSG_VERBOSE( " aeot->associatedSurface().center() x " << aeot->associatedSurface().center().x() << " y " << aeot->associatedSurface().center().y() << " z " << aeot->associatedSurface().center().z() );
          
//        use drift (circle) derivatives as in TrkiPatFitterUtils 

          Amg::Vector3D sensorDirection  = Amg::Vector3D(measurement->associatedSurface().transform().rotation().col(2));
          Amg::Vector3D driftDirection = Amg::Vector3D(sensorDirection.cross(originalTrkPar->momentum().unit()));
          driftDirection = driftDirection.unit();
          ATH_MSG_VERBOSE( " cos(theta) " << std::cos(originalTrkPar->momentum().theta()) << " phi " << originalPhi );
          ATH_MSG_VERBOSE( " sensorDirection x " << sensorDirection.x() << " y " << sensorDirection.y() << " z " << sensorDirection.z() );
          ATH_MSG_VERBOSE( " driftDirection x " << driftDirection.x() << " y " << driftDirection.y() << " z " << driftDirection.z() );
          

// Barrel 
          double projection = driftDirection.z();
// dz because of a dhteta change: factor = dz/dtheta
          const double originalTheta=originalTrkPar->momentum().theta();
          const double sinOriginalTheta=std::sin(originalTheta);
          double factor = (distanceR/sinOriginalTheta)/sinOriginalTheta;    //preserve calc. order     
          const Surface&  surface = aeot->associatedSurface();
          ATH_MSG_VERBOSE( " fabs(surface.normal().z()) " << std::fabs(surface.normal().z()) );
          if(std::fabs(surface.normal().z()) > 0.5) {
// endcap 
             projection = (driftDirection.x()*surface.center().x() + driftDirection.y()*surface.center().y()) /
                                 surface.center().perp();
// dr because of a dhteta change : factor = dR/dtheta
             factor = -(distanceR/std::sin(originalTheta))/(std::cos(originalTheta)); //preserve calc. order        
             ATH_MSG_VERBOSE( " distance " << distance << " Endcap projection " << projection << " factor " << factor );
          } else {
             ATH_MSG_VERBOSE( " distance " << distance << " Barrel projection " << projection  << " factor " << factor );
          }


          localPos[0] += projection*aeot->deltaTranslation() - projection*factor*aeot->deltaAngle(); // Shift in precision direction.
          
          ATH_MSG_VERBOSE( " MDT old localPos " << parameters[0] << " new localPos " << localPos[0] ); 

        } else {

// CSC or MM clusters

//        use cluster derivatives as in TrkiPatFitterUtils 
//            double distance     = fm->surface()->normal().dot(measurement.intersection(FittedTrajectory).position() - fm->intersection(FittedTrajectory).position());
//            double projection   = fm->surface()->normal().dot(measurement.intersection(FittedTrajectory).direction());


          double projection = aeot->associatedSurface().normal().dot(originalTrkPar->momentum().unit()); 
          localPos[0] += aeot->deltaTranslation()*projection + distance*aeot->deltaAngle();

          ATH_MSG_VERBOSE( " CSC old localPos " << parameters[0] << " distance " << distance << " proj " << projection << " new localPos " << localPos[0] << " simple " << localPosSimple[0] ); 
          
        } 
    }

    parameters[0] = localPos[0];
    residual = measurement->localParameters()[Trk::loc1] -  parameters[0];
    ATH_MSG_VERBOSE( " new residual after end aeots loop " << residual );

    // Set parameters to the new values;
    const AmgSymMatrix(5)* originalCov = trkPar->covariance();
    if(originalCov) {
       trkPar->updateParameters(parameters, AmgSymMatrix(5)( *originalCov ) );
    } else {
       trkPar->updateParameters(parameters); 
    }
  
    // Now call original method.
    std::unique_ptr<Trk::ResidualPull> resPull = residualPull(measurement, trkPar, resType, detType );
    delete trkPar;
    return resPull;
}


/////////////////////////////////////////////////////////////////////////////
/// calc pull in 1 dimension
/////////////////////////////////////////////////////////////////////////////
double Trk::ResidualPullCalculator::calcPull(
    const double residual,
    const double locMesCov,
    const double locTrkCov,
    const Trk::ResidualPull::ResidualType& resType ) const {

    double CovarianceSum = 0.0;
    if (resType == Trk::ResidualPull::Unbiased) {
        CovarianceSum = locMesCov + locTrkCov;
    } else if (resType == Trk::ResidualPull::Biased) {
        CovarianceSum = locMesCov - locTrkCov;
    } else CovarianceSum = locMesCov;

    if (CovarianceSum <= 0.0) {
        ATH_MSG_DEBUG("instable calculation: total covariance is non-positive, MeasCov = "<<
            locMesCov<<", TrkCov = "<<locTrkCov<<", resType = "<<resType);
        return 0.0;
    }
    return residual/sqrt(CovarianceSum);
}
