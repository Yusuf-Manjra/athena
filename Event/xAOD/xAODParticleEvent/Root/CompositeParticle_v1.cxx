/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CompositeParticle_v1.cxx 641075 2015-01-22 16:12:10Z kkoeneke $

// standard includes
#include <math.h>       /* remainder and M_PI */

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "xAODBase/IParticle.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

// Local include(s):
#include "xAODParticleEvent/versions/CompositeParticle_v1.h"


namespace xAOD {

  CompositeParticle_v1::CompositeParticle_v1()
    : //IParticle(),
      m_p4(),
      m_p4Cached( false ) {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //     Functions implementing four-momentum
  //

  static SG::AuxElement::Accessor< float > accPx( "px" );
  static SG::AuxElement::Accessor< float > accPy( "py" );
  static SG::AuxElement::Accessor< float > accPz( "pz" );
  static SG::AuxElement::Accessor< float > accE( "e" );


  double CompositeParticle_v1::pt() const {
    return std::sqrt( std::pow( px(), 2 ) + std::pow( py(), 2 ) );
  }

  double CompositeParticle_v1::eta() const {
    return p4().Eta();
  }

  double CompositeParticle_v1::phi() const {
    return p4().Phi();
  }

  double CompositeParticle_v1::m() const {
    return p4().M();
  }

  double CompositeParticle_v1::rapidity() const {
    return p4().Rapidity();
  }

  const CompositeParticle_v1::FourMom_t& CompositeParticle_v1::p4() const {
    // Check if we need to reset the cached object:
    if( ! m_p4Cached ) {
      double px(0.0);
      double py(0.0);
      double pz(0.0);
      double e(0.0);
      if ( accPx.isAvailable(*this) ) { px = this->px(); }
      if ( accPy.isAvailable(*this) ) { py = this->py(); }
      if ( accPz.isAvailable(*this) ) { pz = this->pz(); }
      if ( accE.isAvailable(*this)  ) { e  = this->e(); }
      m_p4.SetPxPyPzE( px, py, pz, e );
      m_p4Cached = true;
    }
    // Return the cached object:
    return m_p4;
  }

  Type::ObjectType CompositeParticle_v1::type() const {
    return Type::Other;
  }


  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( CompositeParticle_v1, float, double, e)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( CompositeParticle_v1, float, double, px)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( CompositeParticle_v1, float, double, py)
  AUXSTORE_PRIMITIVE_GETTER_WITH_CAST( CompositeParticle_v1, float, double, pz)


  double CompositeParticle_v1::et() const {
    return p4().Et();
  }


  void CompositeParticle_v1::setP4( const FourMom_t& vec ) {
    accPx( *this ) = static_cast<float>(vec.Px());
    accPy( *this ) = static_cast<float>(vec.Py());
    accPz( *this ) = static_cast<float>(vec.Pz());
    accE( *this )  = static_cast<float>(vec.E());
    //Need to recalculate m_p4 if requested after update
    m_p4Cached=false;
  }


  void CompositeParticle_v1::setPxPyPzE( double px,
                                         double py,
                                         double pz,
                                         double e ) {
    static Accessor< float > acc1( "px" );
    acc1( *this ) = (float)px;
    static Accessor< float > acc2( "py" );
    acc2( *this ) = (float)py;
    static Accessor< float > acc3( "pz" );
    acc3( *this ) = (float)pz;
    static Accessor< float > acc4( "e" );
    acc4( *this ) = (float)e;
    //Need to recalculate m_p4 if requested after update
    m_p4Cached=false;
  }


  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( CompositeParticle_v1,
                                       float, double,
                                       px, setPx )
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( CompositeParticle_v1,
                                       float, double,
                                       py, setPy )
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( CompositeParticle_v1,
                                       float, double,
                                       pz, setPz )
  AUXSTORE_PRIMITIVE_SETTER_WITH_CAST( CompositeParticle_v1,
                                       float, double,
                                       e, setE )

  // End: Functions implementing four-momentum
  /////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////
  //
  //     Functions implementing other particly-type properties
  //

  bool CompositeParticle_v1::hasCharge() const {
    static SG::AuxElement::Accessor< float > chargeAcc( "charge" );
    return chargeAcc.isAvailable( *this );
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CompositeParticle_v1, float,
                                        charge, setCharge )


  bool CompositeParticle_v1::hasPdgId() const {
    static SG::AuxElement::Accessor< int > pdgAcc( "pdgId" );
    return pdgAcc.isAvailable( *this );
  }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CompositeParticle_v1, int,
                                        pdgId, setPdgId )


  // End: Functions implementing other particly-type properties
  /////////////////////////////////////////////////////////////////////////////



  /////////////////////////////////////////////////////////////////////////////
  //
  //     Functions returning variables that are calculated from 4-momentum
  //     information from constituents.
  //

  // Get the delta-phi between two constituents (missingET is at index=-1)
  double CompositeParticle_v1::deltaPhi( int constitAIdx, int constitBIdx ) const
  {
    // Defaul values
    double phiA(0.0);
    double phiB(0.0);

    // Get the constituent number A
    if ( constitAIdx >= 0 ) {
      const xAOD::IParticle* part = this->constituent(constitAIdx);
      if ( !part ) {
        // This should not be... throw an error.
        throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when calling deltaPhi for constituent number A");
        return 0.0;
      }
      phiA = part->phi();
    }
    else if ( constitAIdx == -1 ) { // Now, we need to check for missingET
      const xAOD::MissingET* met = this->missingET();
      if ( !met ) {
        // This should not be... throw an error.
        throw std::runtime_error("Got a zero pointer to an xAOD::MissingET when calling deltaPhi for constituent number A");
        return 0.0;
      }
      phiA = met->phi();
    }
    else {
      // This should not be... throw an error.
      throw std::runtime_error("Got a non-valid first index when calling deltaPhi");
      return 0.0;
    }

    // Get the constituent number B
    if ( constitBIdx >= 0 ) {
      const xAOD::IParticle* part = this->constituent(constitBIdx);
      if ( !part ) {
        // This should not be... throw an error.
        throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when calling deltaPhi for constituent number B");
        return 0.0;
      }
      phiB = part->phi();
    }
    else if ( constitBIdx == -1 ) { // Now, we need to check for missingET
      const xAOD::MissingET* met = this->missingET();
      if ( !met ) {
        // This should not be... throw an error.
        throw std::runtime_error("Got a zero pointer to an xAOD::MissingET when calling deltaPhi for constituent number B");
        return 0.0;
      }
      phiB = met->phi();
    }
    else {
      // This should not be... throw an error.
      throw std::runtime_error("Got a non-valid second index when calling deltaPhi");
      return 0.0;
    }

    return -remainder( -phiA + phiB, 2*M_PI );
  }


  // Get the delta-eta between two constituents
  double CompositeParticle_v1::deltaEta( int constitAIdx, int constitBIdx ) const
  {
    // Check that we got valid indices
    if ( constitAIdx <= 0 || constitBIdx <=0 ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a negative index... this should not happen");
      return 0.0;
    }

    // Defaul values
    double etaA(0.0);
    double etaB(0.0);

    // Get the constituent number A
    const xAOD::IParticle* partA = this->constituent(constitAIdx);
    if ( !partA ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when calling deltaEta for constituent number A");
      return 0.0;
    }
    etaA = partA->eta();

    // Get the constituent number B
    const xAOD::IParticle* partB = this->constituent(constitBIdx);
    if ( !partB ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when calling deltaEta for constituent number B");
      return 0.0;
    }
    etaB = partB->eta();

    return etaA - etaB;
  }



  // Get the delta-rapidity between two constituents
  double CompositeParticle_v1::deltaRapidity( int constitAIdx, int constitBIdx ) const
  {
    // Check that we got valid indices
    if ( constitAIdx <= 0 || constitBIdx <=0 ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a negative index... this should not happen");
      return 0.0;
    }

    // Defaul values
    double yA(0.0);
    double yB(0.0);

    // Get the constituent number A
    const xAOD::IParticle* partA = this->constituent(constitAIdx);
    if ( !partA ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when calling deltaEta for constituent number A");
      return 0.0;
    }
    yA = partA->rapidity();

    // Get the constituent number B
    const xAOD::IParticle* partB = this->constituent(constitBIdx);
    if ( !partB ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when calling deltaEta for constituent number B");
      return 0.0;
    }
    yB = partB->rapidity();

    return yA - yB;
  }



  // Get the deltaR-squared between two constituents
  // Is useRapidity=true, then the true rapidity will be used. Otherwise,
  // the pseudorapidity eta will be used.
  double CompositeParticle_v1::deltaR2( int constitAIdx, int constitBIdx, bool useRapidity ) const
  {
    // Check that we got valid indices
    if ( constitAIdx <= 0 || constitBIdx <=0 ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a negative index... this should not happen");
      return 0.0;
    }

    // Defaul values
    double deltaPhi(0.0);
    double deltaRap(0.0);

    // Get deltaPhi
    deltaPhi = this->deltaPhi( constitAIdx, constitBIdx );

    // Get the other part (either deltaEta or deltaRapidity)
    if (useRapidity) {
      deltaRap = this->deltaRapidity( constitAIdx, constitBIdx );
    }
    else {
      deltaRap = this->deltaEta( constitAIdx, constitBIdx );
    }

    return deltaPhi*deltaPhi + deltaRap*deltaRap;
  }



  // Get the deltaR between two constituents
  // Is useRapidity=true, then the true rapidity will be used. Otherwise,
  // the pseudorapidity eta will be used.
  double CompositeParticle_v1::deltaR( int constitAIdx, int constitBIdx, bool useRapidity ) const
  {
    double deltaR2 = this->deltaR2(constitAIdx, constitBIdx, useRapidity);
    return std::sqrt(deltaR2);
  }


  /// Get the delta-|\vec{p}| between two constituents
  double CompositeParticle_v1::deltaAbsP( int constitAIdx, int constitBIdx ) const
  {
    // Check that we got valid indices
    if ( constitAIdx <= 0 || constitBIdx <=0 ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a negative index... this should not happen");
      return 0.0;
    }

    // Defaul values
    double absPA(0.0);
    double absPB(0.0);

    // Get the constituent number A
    const xAOD::IParticle* partA = this->constituent(constitAIdx);
    if ( !partA ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when calling deltaEta for constituent number A");
      return 0.0;
    }
    absPA = (partA->p4()).P();

    // Get the constituent number B
    const xAOD::IParticle* partB = this->constituent(constitBIdx);
    if ( !partB ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when calling deltaEta for constituent number B");
      return 0.0;
    }
    absPB = (partB->p4()).P();

    return absPA - absPB;
  }



  /// Get the delta-|\vec{p_T}| between two constituents
  double CompositeParticle_v1::deltaAbsPt( int constitAIdx, int constitBIdx ) const
  {
    // Check that we got valid indices
    if ( constitAIdx <= 0 || constitBIdx <=0 ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a negative index... this should not happen");
      return 0.0;
    }

    // Defaul values
    double absPtA(0.0);
    double absPtB(0.0);

    // Get the constituent number A
    const xAOD::IParticle* partA = this->constituent(constitAIdx);
    if ( !partA ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when calling deltaEta for constituent number A");
      return 0.0;
    }
    absPtA = partA->pt();

    // Get the constituent number B
    const xAOD::IParticle* partB = this->constituent(constitBIdx);
    if ( !partB ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when calling deltaEta for constituent number B");
      return 0.0;
    }
    absPtB = partB->pt();

    return absPtA - absPtB;
  }



  // Get the transverse mass.
  // Specify which calculation method to use as an optional additional argument.
  double CompositeParticle_v1::mt( MT::Method method ) const
  {
    // Use the different calculation methods, depending on the given input
    if ( method == xAOD::CompositeParticle_v1::MT::DEFAULT ) {
      // Calculate m_T according to the ATLAS Higgs->WW publication
      // from 2014/2015 (http://arxiv.org/pdf/1412.2641v1.pdf, page 3)

      // Get all the visible constituents and sum up their 4-momenta
      xAOD::IParticle::FourMom_t fourMom;
      const std::size_t nConstits( this->nConstituents() );
      for ( std::size_t i=0; i<nConstits; ++i ) {
        const xAOD::IParticle* part = this->constituent(i);
        if ( !part ) {
          // This should not be... throw an error.
          throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when calling mt");
          return -999.0;
        }
        fourMom += part->p4();
      }

      // Get the missing e_T
      const xAOD::MissingET* metObj = this->missingET();
      if ( !metObj ) {
        // This should not be... throw an error.
        throw std::runtime_error("Got a zero pointer to an xAOD::MissingET when calling mt");
        return -9999.0;
      }
      double mpx( metObj->mpx() );
      double mpy( metObj->mpy() );
      double met( metObj->met() );

      // Now, actually calculate the result (in two parts)
      double px( fourMom.Px() );
      double py( fourMom.Px() );
      double part2 = (px+mpx)*(px+mpx) + (py+mpy)*(py+mpy);

      double mll2( fourMom.M2() );
      double etll( std::sqrt( (px*px + py*py) + mll2 ) );
      double part1( (etll+met)*(etll+met) );

      // One last sanity check
      if ( part1 < part2 ) {
        // This should not be... throw an error.
        throw std::runtime_error("Got an invalid mt calculation");
        return -99999.0;
      }
      return std::sqrt( part1 - part2 );
    }

    return 0.0;
  }




  // End: Functions returning variables that are calculated from 4-momentum
  //      information from constituents.
  /////////////////////////////////////////////////////////////////////////////




  /////////////////////////////////////////////////////////////////////////////
  //
  //     Functions implementing handling of constituents
  //

  // Get the accessor for the vector of element links to the constituents
  static SG::AuxElement::Accessor< xAOD::IParticleLinkContainer >
              constitLinksAcc( "constituentLinks" );



  void
  CompositeParticle_v1::addConstituent( const xAOD::IParticle* part, bool updateFourMom ) {
    // Check if we have a valid pointer
    if ( !part ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when adding constituent!");
      return;
    }
    xAOD::IParticleLinkContainer& constitLinks = constitLinksAcc( *this );

    // Update the four-momentum of this composite particle, if requested
    if ( updateFourMom ) {
      this->setP4( this->p4() + part->p4() );
      // Cannot add charge automatically as this is NOT part of the xAOD::IParticle interface
      //this->setCharge( this->charge() + (*partLink)->charge() );
    }

    // Add the given ElementLink as a constituent of this composite particle
    const std::size_t partIdx = part->index();
    const xAOD::IParticleContainer* partCont =
      dynamic_cast<const xAOD::IParticleContainer*>(part->container());
    if ( !partCont ) {
      throw std::runtime_error( "ERROR: Could not find the container for the particle when adding constituent" );
    }
    constitLinks.push_back( IParticleLink( *partCont, partIdx ) );
    return;
  }



  void
  CompositeParticle_v1::addConstituent( const IParticleLink& partLink, bool updateFourMom ) {
    // Check if we have a valid ElementLink
    if ( ! partLink.isValid() ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got an invalid ElementLink when adding constituent!");
      return;
    }
    xAOD::IParticleLinkContainer& constitLinks = constitLinksAcc( *this );

    // Update the four-momentum of this composite particle, if requested
    if ( updateFourMom ) {
      this->setP4( this->p4() + (*partLink)->p4() );
      // Cannot add charge automatically as this is NOT part of the xAOD::IParticle interface
      //this->setCharge( this->charge() + (*partLink)->charge() );
    }

    // Add the given ElementLink as a constituent of this composite particle
    constitLinks.push_back( partLink );
    return;
  }


  void
  CompositeParticle_v1::addConstituents( const xAOD::IParticleLinkContainer& partLinkCont,
                                         bool updateFourMom  ) {
    for ( xAOD::IParticleLinkContainer::const_iterator
            constitItr = partLinkCont.begin(),
            constitItrEnd = partLinkCont.end();
          constitItr != constitItrEnd;
          ++constitItr ) {
      const xAOD::IParticleLink& currentLink = *constitItr;
      this->addConstituent( currentLink, updateFourMom );
    }
    return;
  }



  void CompositeParticle_v1::removeConstituent( const xAOD::IParticle* part,
                                                bool updateFourMom ) {
    // Check if we have a valid pointer
    if ( !part ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to an xAOD::IParticle when adding constituent!");
      return;
    }

    // Add the ElementLink of the given particle and pass it on
    const std::size_t partIdx = part->index();
    const xAOD::IParticleContainer* partCont =
      dynamic_cast<const xAOD::IParticleContainer*>(part->container());
    if ( !partCont ) {
      throw std::runtime_error( "ERROR: Could not find the container for the particle when adding constituent" );
    }
    this->removeConstituent( IParticleLink( *partCont, partIdx ), updateFourMom );
    return;
  }



  void
  CompositeParticle_v1::removeConstituent( const xAOD::IParticleLink& partLink,
                                           bool updateFourMom ) {
    // Check if we have a valid ElementLink
    if ( ! partLink.isValid() ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got an invalid ElementLink when adding constituent!");
      return;
    }
    xAOD::IParticleLinkContainer& constitLinks = constitLinksAcc( *this );

    // Find the constituent particle that should be removed
    for ( xAOD::IParticleLinkContainer::iterator
            constitItr = constitLinks.begin(),
            constitItrEnd = constitLinks.end();
          constitItr != constitItrEnd;
          ++constitItr ) {
      const xAOD::IParticleLink& currentLink = *constitItr;
      if ( partLink == currentLink ) {
        // Update the four-momentum of this composite particle, if requested
        if ( updateFourMom ) {
          this->setP4( this->p4() - (*partLink)->p4() );
          // Cannot add charge automatically as this is NOT part of the xAOD::IParticle interface
          //this->setCharge( this->charge() - (*partLink)->charge() );
        }

        // Remove the found link
        constitLinks.erase( constitItr );
        return;
      }
    }
    return;
  }



  // Get the accessor for the element link to the MissingET object
  static SG::AuxElement::Accessor< ElementLink<xAOD::MissingETContainer> >
          metLinkAcc( "missingETLink" );


  const xAOD::MissingET*
  CompositeParticle_v1::missingET() const {
    if ( !(metLinkAcc.isAvailable(*this)) ) {
      return 0;
    }
    const ElementLink<xAOD::MissingETContainer>& metLink = metLinkAcc(*this);
    // Check if we have a valid ElementLink
    if ( ! metLink.isValid() ) {
      return 0;
    }
    return *metLink;
  }



  void
  CompositeParticle_v1::setMissingET( const xAOD::MissingET* met,
                                      bool updateFourMom ) {
    // Check if we have a valid pointer
    if ( !met ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got a zero pointer to xAOD::MissingET when adding it!");
      return;
    }

    // Update the four-momentum of this composite particle, if requested
    if ( updateFourMom ) {
      xAOD::IParticle::FourMom_t metP4;
      double metpx = met->mpx();
      double metpy = met->mpy();
      metP4.SetPxPyPzE( metpx, metpy, 0.0, std::sqrt(metpx*metpx + metpy*metpy) );
      this->setP4( this->p4() + metP4 );
    }

    // Add the given ElementLink as a constituent of this composite particle
    const std::size_t metIdx = met->index();
    const xAOD::MissingETContainer* metCont =
      dynamic_cast<const xAOD::MissingETContainer*>(met->container());
    if ( !metCont ) {
      throw std::runtime_error( "ERROR: Could not find the MissingETContainer when adding MET" );
    }
    metLinkAcc(*this) = ElementLink<xAOD::MissingETContainer>( *metCont, metIdx );
    return;
  }



  void
  CompositeParticle_v1::removeMissingET( bool updateFourMom ) {
    const xAOD::MissingET* met = this->missingET();
    if ( !met ) {
      // There is no valid MissingET object assigned... nothing can done
      return;
    }

    // Update the four-momentum of this composite particle, if requested
    if ( updateFourMom ) {
      xAOD::IParticle::FourMom_t metP4;
      double metpx = met->mpx();
      double metpy = met->mpy();
      metP4.SetPxPyPzE( metpx, metpy, 0.0, std::sqrt(metpx*metpx + metpy*metpy) );
      this->setP4( this->p4() - metP4 );
    }
    // Assign an invalid ElementLink here
    metLinkAcc(*this) = ElementLink<xAOD::MissingETContainer>();
    return;
  }




  bool
  CompositeParticle_v1::contains( const xAOD::MissingET* met ) const {
    const ElementLink<xAOD::MissingETContainer>& metLink = metLinkAcc(*this);
    // Check if we have a valid ElementLink. If not, return false. This is
    // because the given met that we check against obviously exists, meaning
    // that if it would be the same, the ElementLink should have been valid.
    if ( ! metLink.isValid() ) {
      return false;
    }
    const xAOD::MissingET* thisMet = *metLink;

    // Check if the pointers are equal
    if ( thisMet == met ) {
      return true;
    }

    // Use the MissingET == operator
    if ( (*thisMet) == (*met) ) {
      return true;
    }

    // Otherwise, return false
    return false;
  }



  bool
  CompositeParticle_v1::contains( const xAOD::IParticle* part ) const {
    const xAOD::IParticleLinkContainer& constitLinks = this->constituentLinks();
    for ( xAOD::IParticleLinkContainer::const_iterator
            constitItr = constitLinks.begin(),
            constitItrEnd = constitLinks.end();
          constitItr != constitItrEnd;
          ++constitItr ) {
      const xAOD::IParticleLink& currentLink = *constitItr;
      if ( ! currentLink.isValid() ) continue;
      const xAOD::IParticle* currentPart = *currentLink;
      if ( part == currentPart ) {
        return true;
      }
    }
    return false;
  }


  bool
  CompositeParticle_v1::contains( const xAOD::IParticleLink& partLink ) const {
    const xAOD::IParticleLinkContainer& constitLinks = this->constituentLinks();
    for ( xAOD::IParticleLinkContainer::const_iterator
            constitItr = constitLinks.begin(),
            constitItrEnd = constitLinks.end();
          constitItr != constitItrEnd;
          ++constitItr ) {
      const xAOD::IParticleLink& currentLink = *constitItr;
      if ( partLink == currentLink ) {
        return true;
      }
    }
    return false;
  }


  std::size_t CompositeParticle_v1::nConstituents() const {
    if( constitLinksAcc.isAvailable( *this ) ) {
      return constitLinksAcc( *this ).size();
    }
    return 0;
  }


  const xAOD::IParticle*
  CompositeParticle_v1::constituent( std::size_t index ) const {
    if ( index >= this->nConstituents() ) {
       return 0;
	  }
	  const xAOD::IParticleLink & constitLink = constituentLink( index );
	  if ( ! constitLink.isValid() ) {
	     return 0;
	  }
	  return *constitLink;
  }


  const xAOD::IParticleLink&
  CompositeParticle_v1::constituentLink( std::size_t index ) const {
    if ( index >= this->nConstituents() ) {
	     static const xAOD::IParticleLink dummy;
	     return dummy;
    }
    return constituentLinks()[ index ];
  }



  AUXSTORE_OBJECT_SETTER_AND_GETTER( CompositeParticle_v1,
                                     xAOD::IParticleLinkContainer,
                                     constituentLinks,
                                     setConstituentLinks )

  // End: Functions implementing handling of constituents
  /////////////////////////////////////////////////////////////////////////////



  void CompositeParticle_v1::toPersistent() {
    if( constitLinksAcc.isAvailableWritable( *this ) ) {
      for ( xAOD::IParticleLinkContainer::iterator
              itr = constitLinksAcc( *this ).begin(),
              itrEnd = constitLinksAcc( *this ).end();
            itr != itrEnd;
            ++itr ) {
        itr->toPersistent();
      }
    }
    return;
  }


} // namespace xAOD
