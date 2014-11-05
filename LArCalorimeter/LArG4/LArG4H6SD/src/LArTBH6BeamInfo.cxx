/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArTBH6BeamInfo.h"

#include "HitManagement/AthenaHitsVector.h"

#include "LArG4TBSimEvent/LArG4H6FrontHitCollection.h"
#include "TBEvent/TBTrack.h"
#include "TBEvent/TBEventInfo.h"

#include "StoreGate/StoreGateSvc.h"

// Gaudi Includes
#include "GaudiKernel/SystemOfUnits.h"

using namespace Gaudi::Units;

LArTBH6BeamInfo::LArTBH6BeamInfo(const std::string& name, ISvcLocator* pSvcLocator)
 :AthAlgorithm(name, pSvcLocator),
  m_Primary(true),
  m_pcode(999),
  m_cryoX(0.),
  m_numEv(0)
{
   declareProperty("HitsContainer",m_HitsCollNames);
//   m_HitsCollNames.push_back("LArTBFrontHitCollection");
   declareProperty("PrimaryTrackOnly",m_Primary);
   declareProperty("PrimaryParticle",m_pcode);
}

LArTBH6BeamInfo::~LArTBH6BeamInfo()
{}

//****************************************************************************
//* Initialization
//****************************************************************************

StatusCode LArTBH6BeamInfo::initialize()
{

   if((!m_Primary) && (m_pcode == 999)) {
      ATH_MSG_ERROR ( "Pcode should be in jO, if not PrimaryTrackOnly !" );
      return StatusCode::FAILURE;
   }

// End of Initialization
   ATH_MSG_INFO ( "LArTBH6BeamInfo initialisation completed" );
   return StatusCode::SUCCESS;
}

//****************************************************************************
//* Execution
//****************************************************************************

StatusCode LArTBH6BeamInfo::execute()
{
   ATH_MSG_INFO ( "LArTBH6BeamInfo in execute" );
   CLHEP::Hep3Vector pos;

   if ( m_numEv == 0 ){
      const TBEventInfo* theEventInfo;
      if ( evtStore()->retrieve(theEventInfo,"TBEventInfo").isFailure() ) {
         ATH_MSG_ERROR ( "cannot retrieve TBEventInfo from StoreGate" );
         return StatusCode::FAILURE;
      }
      m_cryoX = theEventInfo->getCryoX();
   }

   typedef AthenaHitsVector<LArG4H6FrontHit> CONTAINER;
   const DataHandle< CONTAINER > hitcoll ;

   std::vector<std::string>::iterator it = m_HitsCollNames.begin();


   dVect v_x;
   dVect v_y;
   dVect v_xz;
   dVect v_yz;
   dVect v_ex;
   dVect v_ey;

// loop hit containers
   for (; it < m_HitsCollNames.end(); ++it) {

      ATH_MSG_DEBUG (" hit container name: "<< *it );

      if(StatusCode::SUCCESS == evtStore()->retrieve(hitcoll, *it) ) {

        CONTAINER::const_iterator f_hit = hitcoll->begin();
        CONTAINER::const_iterator l_hit = hitcoll->end();

        ATH_MSG_INFO (" hit container: "<< *it <<" size: "<<hitcoll->size() );

        for ( ; f_hit!=l_hit; ++f_hit) {
            LArG4H6FrontHit* hit = (*f_hit) ;
            // loop over hits, find track==0 and make a fit, store a TBTrack to StoreGate
            if(hit->GetSC() > 0) continue; // scintilator hit
            if(m_Primary) {
              if(hit->GetTrackID() != 1) continue; // not a primary particle
            } else {
              if(hit->GetPcode() != m_pcode) continue; // not a beam particle
            }
            pos = hit->GetPos();
            if(hit->GetX() > 0) { // X-coordinate
               v_x.push_back(pos.x());
               v_xz.push_back(pos.z()+21600.*mm);
               v_ex.push_back(0.01);   // take the error from somewhere !!!
            } else {
               v_y.push_back(pos.y());
               v_yz.push_back(pos.z()+21600.*mm);
               v_ey.push_back(0.01);   // take the error from somewhere !!!
            }
        }

      } else {
          ATH_MSG_ERROR ( " unable to retrieve hit container: "<< *it );
      }
   }

   if(v_x.size() < 2 || v_y.size() < 2) { // Could not fit
      ATH_MSG_DEBUG ( "Could not fit, setting zero. "<<v_x.size()<<"/"<<v_y.size() );
      TBTrack *track = new TBTrack(0,0);
      track->setUintercept(0.);
      track->setVintercept(0.);
      track->setUslope(0.);
      track->setVslope(0.);
//      track->setResidualu(0, 0);
//      track->setResidualv(0, 0);
      track->setChi2_u(1000.);
      track->setChi2_v(1000.);
      track->setCryoHitu(0.);
      track->setCryoHitv(0.);
      track->setCryoHitw(30000.);

      if ( evtStore()->record(track,"Track").isFailure( ) ) {
        ATH_MSG_FATAL ( "Cannot record Track" );
      }
      return StatusCode::FAILURE;
   }
   // Now fit somehow
   double chi2_x = 0;
   double chi2_y = 0;
   std::vector<double> residual_x, residual_y;
   double a1_x = 0;
   double a2_x = 0;
   double a1_y = 0;
   double a2_y = 0;
   bool check = true;

   check = fitVect(v_x, v_xz, v_ex, a1_x, a2_x, chi2_x, residual_x);

   if(!check){
      ATH_MSG_ERROR ( "Fit in X-coordinate failure." );
      return StatusCode::FAILURE;
   }

   check = fitVect(v_y, v_yz, v_ey, a1_y, a2_y, chi2_y, residual_y);
   if(!check){
      ATH_MSG_ERROR ( "Fit in Y-coordinate failure." );
      return StatusCode::FAILURE;
    }

    // Setting the slopes & intercepts for each track //
    ATH_MSG_DEBUG ( "Setting fit parameters of track." );
    ATH_MSG_DEBUG ( "Intercepts: "<<a1_x<<" "<<a1_y );
    ATH_MSG_DEBUG ( "Slopes: "<<a2_x<<" "<<a2_y );

    TBTrack *track = new TBTrack(v_x.size(), v_y.size());

    track->setUintercept(a1_x);
    track->setVintercept(a1_y);
    track->setUslope(a2_x);
    track->setVslope(a2_y);

    // Setting the residual for plane u //
    for(size_t i = 0; i < v_x.size(); ++i){
      track->setResidualu(i, residual_x[i]);
    }

    // Setting the residual for plane v //
    for(size_t i = 0; i < v_y.size(); ++i){
      track->setResidualv(i, residual_y[i]);
    }

    ATH_MSG_DEBUG ( "chi2 in X: " << chi2_x );
    ATH_MSG_DEBUG ( "chi2 in Y: " << chi2_y );
    ATH_MSG_DEBUG ( "Setting chi2s of track." );

    track->setChi2_u(chi2_x);
    track->setChi2_v(chi2_y);

    // Setting the cryo plane (30000 mm in TB coordinate)
    track->setCryoHitu(a2_x*30000.+a1_x+m_cryoX);
    track->setCryoHitv(a2_y*30000.+a1_y);
    track->setCryoHitw(30000.);

    if ( evtStore()->record(track,"Track").isFailure( ) ) {
      ATH_MSG_FATAL ( "Cannot record Track" );
      return StatusCode::FAILURE;
    }

  return  StatusCode::SUCCESS;
}

//****************************************************************************
//* Finalize
//****************************************************************************

StatusCode LArTBH6BeamInfo::finalize()
{
   ATH_MSG_INFO ( "LArTBH6BeamInfo::finalize()" );
   return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
bool LArTBH6BeamInfo::fitVect(const dVect &vec, const dVect &vec_z, const dVect &vec_e,
                              double &a1, double &a2,double &chi2, dVect &residual)
///////////////////////////////////////////////////////////////////////////
{
   // v_u = vector of u data points
  // v_w = vector of w data points
  // v_eu = vector of errors in u data points
  // v_ew = vector of errors in w data points
  // a1 and a2 = fit parameters: u = a1 + a2*w

  // *** note that the fit algorithm used  (given in 'Numerical Methods
  // in C' section 15.2) assumes that the w data points are known exactly
  // i.e. that v_ew[i]=0 for all i

  // 'Numerical Methods' notes that the task of fitting a straight
  // line model with errors in both coordinates is "considerably harder"
  // (section 15.3) - but clearly it could be done

  int i;
  double m_s = 0;
  double m_su = 0;
  double m_sww = 0;
  double m_sw = 0;
  double m_suw = 0;

  int hitNum = vec.size();
  for(i = 0; i < hitNum; ++i){

    ATH_MSG_DEBUG ( "Position in X: " << vec[i] );
    ATH_MSG_DEBUG ( "Position in Z: " << vec_z[i] );
    ATH_MSG_DEBUG ( "Error in X: " << vec_e[i] );

    m_s += 1 / (vec_e[i]*vec_e[i]);
    m_su += vec[i] / (vec_e[i]*vec_e[i]);
    m_sww += vec_z[i]*vec_z[i] / (vec_e[i]*vec_e[i]);
    m_sw += vec_z[i] / (vec_e[i]*vec_e[i]);
    m_suw += vec[i]*vec_z[i] / (vec_e[i]*vec_e[i]);
  }

  double denum = (m_s*m_sww-m_sw*m_sw);
  if(denum == 0){
    ATH_MSG_ERROR ( " Invalid denumerator" );
    return false;
  }

  a1 = (m_su*m_sww - m_sw*m_suw)/ denum;
  a2 = (m_s*m_suw - m_su*m_sw)/ denum;
  ATH_MSG_DEBUG ( "Fit results:" << " intercept = " << a1 << " and slope = " << a2 );

  // Fill residual
  residual.clear();
  for (i = 0; i < hitNum; ++i) {
    residual.push_back(vec[i] - a1 - a2*vec_z[i]);
  }
  // Fill Chi2
  chi2 = 0;
  for(i = 0; i < hitNum; ++i){
    chi2 += (vec[i] - a1 - a2*vec_z[i])*(vec[i] - a1 - a2*vec_z[i])/(vec_e[i]*vec_e[i]);
  }

  return true;
}
