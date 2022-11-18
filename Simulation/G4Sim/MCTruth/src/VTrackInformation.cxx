/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/VTrackInformation.h"

VTrackInformation::VTrackInformation(TrackClassification tc):m_classify(tc)
{
}

void VTrackInformation::SetPrimaryHepMCParticle(HepMC::GenParticlePtr p)
{
  m_thePrimaryParticle=p;
}

bool VTrackInformation::GetReturnedToISF() const
{
  return false;
}

void VTrackInformation::SetParticle(HepMC::GenParticlePtr /*p*/)
{
  // you should not call this, perhaps throw an exception?
  std::cerr<<"ERROR  VTrackInformation::SetParticle() not supported  "<<std::endl;
 
}

void VTrackInformation::SetBaseISFParticle(ISF::ISFParticle* /*p*/)
{
  // you should not call this, perhaps throw an exception?
  std::cerr<<"ERROR  VTrackInformation::SetBaseISFParticle() not supported  "<<std::endl;
 
}

void VTrackInformation::SetReturnedToISF(bool)
{
  std::cerr<<"ERROR  VTrackInformation::SetReturnedToISF() not supported  "<<std::endl;
}
