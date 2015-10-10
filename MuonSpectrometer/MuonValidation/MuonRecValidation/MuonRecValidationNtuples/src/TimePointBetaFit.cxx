/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRecValidationNtuples/TimePointBetaFit.h"
#include <iostream>

namespace Muon {

  float TimePointBetaFit::m_invSpeedOfLight = 1./(299.792458);

  TimePointBetaFit::FitResult TimePointBetaFit::fit( TimePointBetaFit::HitVec& hits ) const {
    
    /** Formula
       chi2 = sum( t_meas-t_pred)^2/sigma_meas^2 )
       t_pred = dist / v = dist / ( beta*c )
       
       minimize chi2 -> d chi2 / d beta = 0 
       
       beta = sum(  ( d^2/( c * sigma_meas^2 ) ) )/ sum( (d*t_meas)/( sigma_meas^2 ) )
     */

    // we need at least one hit
    if( hits.empty() ) return FitResult();

    if( m_debugLevel > 0 ) std::cout << " TimePointBetaFit: performing fit, hits " << hits.size() << std::endl;
    
    // loop over hits and calculate beta
    float sum1 = 0.;
    float sum2 = 0.;
    for( HitVec::const_iterator it=hits.begin(); it!=hits.end(); ++it ){
      if( !it->useInFit ) continue;
      sum1 += it->distance*it->distance*m_invSpeedOfLight*it->weight2;
      sum2 += it->distance*it->time*it->weight2;
      if( m_debugLevel > 5 ) std::cout << " TimePointBetaFit: hit, tof " << it->distance*m_invSpeedOfLight << " time " << it->time << " error " << it->error << std::endl;
    } 
    // check if sum2 is none zero
    if( sum2 == 0 ) return FitResult();

    // calculate beta and chi2
    float beta = sum1/sum2;
    float invBeta = 1./beta;
    if( m_debugLevel > 1 ) std::cout << " TimePointBetaFit: beta " << beta << " sum1 " << sum1 << " sum2 " << sum2 << std::endl;
    
    float chi2 = 0;
    int ndof = hits.size()-1;
    for( HitVec::iterator it=hits.begin(); it!=hits.end(); ++it ){
      float res = it->time - it->distance*m_invSpeedOfLight*invBeta;
      it->residual = res;
      if( m_debugLevel > 4 ) std::cout << " TimePointBetaFit: hit residual " << res << " pull " << res/it->error << " chi2 " << res*res*it->weight2 << std::endl;
      chi2 += res*res*it->weight2;
    } 

    if( m_debugLevel > 0 ) std::cout << " TimePointBetaFit: beta " << beta << " chi2 " << chi2 << " ndof " <<  ndof << " chi2/ndof " << chi2/ndof << std::endl;
    
    return FitResult(1,beta,chi2,ndof);
    
  }


  TimePointBetaFit::FitResult TimePointBetaFit::fitWithOutlierLogic( TimePointBetaFit::HitVec& hits ) const {
    if( m_debugLevel > 0 ) std::cout << " TimePointBetaFit: performing fit with outlier logic, hits " << hits.size() << std::endl;
    
    // initial fit, stop if it fails
    TimePointBetaFit::FitResult result = fit( hits );
    
    // return result if the fit failed, there were less than three hits or we are happy with the chi2
    if( result.status == 0 || hits.size() < 3 || result.chi2/result.ndof < 5 ) return result;
    
    // if we get here, run the outlier logic
    // strategy: refit all combinations removing one of the hits, select the combination with the best chi2
    
    int worstHit = -1;
    float bestChi2Ndof = result.chi2/result.ndof;
    for( unsigned int i=0;i<hits.size();++i ){
      // flag hit so it is not used in fit
      hits[i].useInFit = false;
      
      // fit and select if fit is ok and better that the best
      TimePointBetaFit::FitResult resultNew = fit( hits );      
      if( resultNew.status != 0 ){
        float chi2Ndof = resultNew.chi2/resultNew.ndof;
        if( chi2Ndof < bestChi2Ndof ){
          bestChi2Ndof = chi2Ndof;
          worstHit = i;
        }
      }

      // put hit back
      hits[i].useInFit = true;
    }
    
    // if we didn't find an improvement return the initial result
    if( worstHit == -1 ) return result;

    // now refit once more removing the worst hit and return the result so the residuals are calculated correctly
    hits[worstHit].useInFit = false;
    return fit( hits );      
  }

}
