///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Photon_p4.h 
// Header file for class Photon_p4
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_PHOTON_P4_H 
#define EGAMMAEVENTTPCNV_PHOTON_P4_H 1

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egamma_p4.h"

// forward declarations
class PhotonCnv_p4;

class Photon_p4
{
  /////////////////////////////////////////////////////////////////// 
  // Friend classes
  /////////////////////////////////////////////////////////////////// 

  // Make the AthenaPoolCnv class our friend
  friend class PhotonCnv_p4;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 

  /** Default constructor: 
   */
  Photon_p4();

  /** Destructor: 
   */
  ~Photon_p4();

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

  /// the egamma part 
  egamma_p4 m_egamma;
  
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline Photon_p4::Photon_p4() :
  m_egamma()
{}

#endif //> EGAMMAEVENTTPCNV_PHOTON_P4_H
