///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// AthExParticles.h 
// Header file for class AthExParticles
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXTHINNING_ATHEXPARTICLES_H 
#define ATHEXTHINNING_ATHEXPARTICLES_H 

// STL includes

// Gaudi includes

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

// AthExThinning includes
#include "AthExThinning/AthExParticle.h"
#include "AthExThinning/AthExIParticle.h"
#include "AthExThinning/AthExIParticles.h"

// Forward declaration

#ifndef DATAVECTOR_ATHEXPARTICLE_ATHEXIPARTICLE_H 
#define DATAVECTOR_ATHEXPARTICLE_ATHEXIPARTICLE_H 
// Needed for constructing the mirror container hierarchy
// The use of the guard macro prevents this from instantiating the static data member
// more than once.
DATAVECTOR_BASE( AthExParticle, AthExIParticle );
#endif

class AthExParticles : public DataVector<AthExParticle>
{ 
 public: 

  /** Default constructor: 
   */
  AthExParticles();

  /** Copy constructor: 
   */
  AthExParticles( const AthExParticles& rhs );

  /** Assignment operator: 
   */
  AthExParticles& operator=( const AthExParticles& rhs ); 

  /** Constructor with parameters: 
   */
  AthExParticles( const SG::OwnershipPolicy own );

  /** Destructor: 
   */
  virtual ~AthExParticles();

 protected: 

}; 


SG_BASE( AthExParticles, DataVector<AthExParticle> );
CLASS_DEF( AthExParticles , 104018905, 1 )

#endif //> ATHEXTHINNING_ATHEXPARTICLES_H
