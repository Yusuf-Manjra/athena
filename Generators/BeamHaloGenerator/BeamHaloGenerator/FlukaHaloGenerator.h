/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FLUKAHALOGENERATOR_H
#define FLUKAHALOGENERATOR_H

#include "BeamHaloGenerator/BeamHaloGenerator.h"
#include "BeamHaloGenerator/FlukaParticle.h"

/** @class FlukaHaloGenerator

    @author W. H. Bell <W.Bell@cern.ch>
    
    @brief A class to provide conversion from a FLUKA format ASCII
    input record into HepMC format, with or without sampling.
*/
class FlukaHaloGenerator: public BeamHaloGenerator {
 public:
  
  FlukaHaloGenerator(int type, // Type of input Fluka particle record
                     const HepPDT::ParticleDataTable* particleTable,
                     const std::string& inputFile,
                     const std::vector<std::string>& generatorSettings);

  virtual ~FlukaHaloGenerator() = default;

  /** A function to initialise the generator. */
  virtual int genInitialize();

  /** A function to finalise the generator. */
  virtual int genFinalize();

  /** A function to create one event in HepMC format. */
  virtual int fillEvt(HepMC::GenEvent* evt,
                      CLHEP::HepRandomEngine* engine);
  
 protected:

  /** A function to read one event in a simplified format. */
  virtual int readEvent(std::vector<BeamHaloParticle> *beamHaloEvent,
                        CLHEP::HepRandomEngine* engine);

  /** A function to read one particle from the input ASCII file. */
  virtual int readParticle(BeamHaloParticle *beamHaloParticle);

 private:

  bool m_sameEvent;
  bool m_firstEvent;
  FlukaParticle m_flukaParticle;
  FlukaParticle m_lastFlukaParticle;
};

#endif
