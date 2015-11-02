/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Geant4TruthIncident.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4TOOLS_Geant4TruthIncident_H
#define ISF_GEANT4TOOLS_Geant4TruthIncident_H

// std
#include <vector>

// Atlas G4 Helpers
#include "SimHelpers/SecondaryTracksHelper.h"

// ISF includes
#include "ISF_Event/ITruthIncident.h"

// HepMC includes
#include "HepMC/SimpleVector.h"

class G4Step;
class G4Track;

namespace ISF {


  /** @class Geant4TruthIncident

      @TODO: class description

      @author Andreas.Schaelicke@cern.ch
   */

  /* comments:
     renamed to Geant4TruthIncident inorder to avoid confusion with G4 internal classes.

   */

  class Geant4TruthIncident : public ITruthIncident {
    public:
      Geant4TruthIncident(const G4Step*, AtlasDetDescr::AtlasRegion geoID, int numChildren, SecondaryTracksHelper &sHelper);
      virtual ~Geant4TruthIncident() {};

      /** Return HepMC position of the truth vertex */
      const HepMC::FourVector&  position() const override final;

      /** Return category of the physics process represented by the truth incident (eg hadronic, em, ..) */
      int                       physicsProcessCategory() const override final;
      /** Return specific physics process code of the truth incident (eg ionisation, bremsstrahlung, ..)*/
      Barcode::PhysicsProcessCode physicsProcessCode() const override final;

      /** Return p^2 of the parent particle */
      double                    parentP2() const override final;
      /** Return pT^2 of the parent particle */
      double                    parentPt2() const override final;
      /** Return Ekin of the parent particle */
      double                    parentEkin() const override final;
      /** Return the PDG Code of the parent particle */
      int                       parentPdgCode() const override final;
      /** Return the barcode of the parent particle */
      Barcode::ParticleBarcode  parentBarcode() const override final;
      /** Return a boolean whether or not the parent particle survives the incident */
      bool                      parentSurvivesIncident() const override final;
      /** Return the parent particle after the TruthIncident vertex (and give
          it a new barcode) */
      HepMC::GenParticle*       parentParticleAfterIncident(Barcode::ParticleBarcode newBC,
                                                             bool setPersistent) override final;

      /** Return p of the i-th child particle */
      const G4ThreeVector       childP(unsigned short index) const;
      /** Return p^2 of the i-th child particle */
      double                    childP2(unsigned short index) const override final;
      /** Return pT^2 of the i-th child particle */
      double                    childPt2(unsigned short index) const override final;
      /** Return Ekin of the i-th child particle */
      double                    childEkin(unsigned short index) const override final;
      /** Return the PDG Code of the i-th child particle */
      int                       childPdgCode(unsigned short index) const override final;
      /** Set the the barcode of all child particles to the given bc */
      void                      setAllChildrenBarcodes(Barcode::ParticleBarcode bc) override final;

      // only called once accepted

      /** Return the parent particle as a HepMC particle type */
      HepMC::GenParticle*       parentParticle(bool setPersistent) const override final;
      /** Return the i-th child as a HepMC particle type and assign the given
          Barcode to the simulator particle */
      HepMC::GenParticle*       childParticle(unsigned short index,
                                              Barcode::ParticleBarcode bc,
                                              bool setPersistent) const override final;

    private:
      Geant4TruthIncident();
      /** prepare the child particles, using the SecondaryTracksHelper */
      inline void prepareChildren() const;

      /** check if the given G4Track represents a particle that is alive in ISF or ISF-G4 */
      inline bool particleAlive(const G4Track *track) const;

      HepMC::GenParticle* convert(const G4Track *particle) const; //*AS* might be put static

      mutable bool                  m_positionSet;
      mutable HepMC::FourVector     m_position;
      const G4Step*                 m_step;

      SecondaryTracksHelper        &m_sHelper;
      mutable bool                  m_childrenPrepared;
      mutable std::vector<G4Track*> m_children;

      HepMC::GenParticle           *m_parentParticleAfterIncident;
   };

}

#endif // ISF_GEANT4TOOLS_Geant4TruthIncident_H
