/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//   @file    RoiUtil.cxx         
//   
//            non-member, non friend RoiDescriptor utility functions
//            to improve encapsulation
//
//   @author M.Sutton
// 
//   Copyright (C) 2015 M.Sutton (sutt@cern.ch)    
//
//   $Id: RoiUtil.cxx, v0.0   Sat 31 Oct 2015 09:54:33 CET sutt $


#include "IRegionSelector/RoiUtil.h"

#include <cmath>

#ifndef M_2PI
static const double M_2PI = 2*M_PI;
#endif

#ifndef M_PIF
static const float  M_PIF = float(M_PI);
#endif



/// test whether a stub is contained within the roi
bool RoiUtil::contains( const IRoiDescriptor& roi, double z0, double dzdr ) {
  static const double maxR = 1100; // maximum radius of RoI - outer TRT radius ~1070 mm - should be configurable? 
  double zouter = dzdr*maxR + z0; 
  if ( roi.composite() ) { 
    for ( IRoiDescriptor::roi_iterator itr=roi.begin() ; itr!=roi.end() ; itr++ ) if ( RoiUtil::contains_internal( *(*itr), z0, zouter ) ) return true;
    return false;
  }
  else return contains_internal( roi, z0, zouter ); 
} 




bool RoiUtil::contains_internal( const IRoiDescriptor& roi, double z0, double zouter )  {
  if ( z0<=roi.zedPlus() && z0>=roi.zedMinus() && zouter<=roi.zedOuterPlus() && zouter>=roi.zedOuterMinus() ) return true;
  return false;
} 
  


/// test whether a stub is contained within the roi
bool RoiUtil::containsPhi( const IRoiDescriptor& roi, double phi ) {
  if ( roi.composite() ) { 
    for ( IRoiDescriptor::roi_iterator itr=roi.begin() ; itr!=roi.end() ; itr++ ) if ( RoiUtil::containsPhi( *(*itr), phi ) ) return true;
      return false;
  }
  else {
    if ( roi.isFullscan() ) return true; 
    if ( roi.phiPlus()>roi.phiMinus() ) return ( phi<roi.phiPlus() && phi>roi.phiMinus() );
    else                                return ( phi<roi.phiPlus() || phi>roi.phiMinus() );
  } 
}




/// non member, non friend interface function 
/// test whether a stub is contained within the roi                                                                                                                                                                  
bool RoiUtil::roiContainsZed( const IRoiDescriptor& roi, double z, double r ) {
  if ( roi.composite() ) { 
    for ( unsigned int i=0 ; i<roi.size() ; i++ ) if ( roiContainsZed( *roi.at(i), z, r ) ) return true;
    return false;
  }
  if ( roi.isFullscan() ) return true;
  double zminus = r*roi.dzdrMinus()+roi.zedMinus();
  double zplus  = r*roi.dzdrPlus() +roi.zedPlus();
  return ( z>=zminus && z<=zplus );
}



bool RoiUtil::containsZed( const IRoiDescriptor& roi, double z, double r ) {
  if ( roi.composite() ) { 
    for ( IRoiDescriptor::roi_iterator itr=roi.begin() ; itr!=roi.end() ; itr++ ) if ( RoiUtil::containsZed( *(*itr), z, r ) ) return true;
    return false;
  }
  else { 
    if ( roi.isFullscan() ) return true;
    double zminus = r*roi.dzdrMinus() + roi.zedMinus();
    double zplus  = r*roi.dzdrPlus()  + roi.zedPlus();
    return ( z>=zminus && z<=zplus );
  }
}




bool RoiUtil::contains( const IRoiDescriptor& roi, double z, double r, double phi ) {
  if ( roi.composite() ) { 
    for ( IRoiDescriptor::roi_iterator itr=roi.begin() ; itr!=roi.end() ; itr++ ) if ( RoiUtil::contains( *(*itr), z, r, phi ) ) return true;
    return false;
  }
  else { 
    return ( RoiUtil::containsZed( roi, z, r ) && RoiUtil::containsPhi( roi, phi ) );
  }
}
  


bool RoiUtil::roiContains( const IRoiDescriptor& roi, double z, double r, double phi ) { 
  if ( roi.composite() ) { 
    for ( unsigned int i=0 ; i<roi.size() ; i++ ) if ( RoiUtil::roiContains( *roi.at(i), z, r, phi ) ) return true;
    return false;
  } 
  if ( roi.isFullscan() ) return true;
  return ( RoiUtil::roiContainsZed( roi, z, r ) && RoiUtil::containsPhi( roi, phi ) ); 
}






double RoiUtil::phicheck(double phi) { 
  while ( phi> M_PIF ) phi-=M_2PI;
  while ( phi<-M_PIF ) phi+=M_2PI;
  if ( !(phi >= -M_PIF && phi <= M_PIF) ) { // use ! of range rather than range to also catch nan etc
    throw std::exception(); 
  } 
  return phi;
}


double RoiUtil::etacheck(double eta) {
  if ( !(eta>-100  && eta<100) ) { // check also for nan
    throw std::exception();
  } 
  return eta;
}


double RoiUtil::zedcheck(double zed ) {
  if ( !(zed>-100000  && zed<100000 ) ) { // check also for nan
    throw std::exception();
  } 
  return zed;
}
