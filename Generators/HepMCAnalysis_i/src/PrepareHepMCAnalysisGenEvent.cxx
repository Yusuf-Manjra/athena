/**
 *  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 *  Author: Andrii Verbytsky andrii.verbytskyi@mpp.mpg.de
 * 
 *  This file implements helper functions that prepare the ATLAS MC events 
 *  to be used with the HepMCAnalysis package.
 */
#include <vector>
#include "HepMC/GenEvent.h"
#include "AthenaBaseComps/AthAlgorithm.h"
//This tricky convention is needed for usage with HepMC3.
#ifdef HEPMC3
#include "HepMC3/GenEvent.h"
#include "HepMCCompatibility.h"
StatusCode  GetEventsFromCollection(AthAlgorithm* a,std::vector<const HepMC3::GenEvent*>& evts, const std::string km);
HepMC::GenEvent* PrepareHepMCAnalysisGenEvent(const HepMC3::GenEvent* cevent)
{
HepMC::GenEvent* event = ConvertHepMCGenEvent_3to2(*cevent);
  for (auto p = event->particles_begin(); p != event->particles_end(); ++p) {
      auto fv=(*p)->momentum();
      fv.set(fv.x()*0.001,fv.y()*0.001,fv.z()*0.001,fv.e()*0.001);
      (*p)->set_momentum(fv);
  }
  return event;
}
StatusCode  GetEvents(AthAlgorithm* a,std::vector<HepMC::GenEvent*>& evts, const std::string km)
{
std::vector<const HepMC3::GenEvent*> input;
StatusCode ret =GetEventsFromCollection(a,input,km);
for (auto a: input) evts.push_back(PrepareHepMCAnalysisGenEvent(a));
return ret;
}
#else
StatusCode  GetEventsFromCollection(AthAlgorithm* a,std::vector<const HepMC::GenEvent*>& evts, const std::string km);
HepMC::GenEvent* PrepareHepMCAnalysisGenEvent(const HepMC::GenEvent* cevent)
{
//Note: the deep copy solves the issue described in the previous version.
HepMC::GenEvent* event = new HepMC::GenEvent(*cevent);
  for (auto p = event->particles_begin(); p != event->particles_end(); ++p) {
      auto fv=(*p)->momentum();
      fv.set(fv.x()*0.001,fv.y()*0.001,fv.z()*0.001,fv.e()*0.001);
      (*p)->set_momentum(fv);
  }
  return event;
}
StatusCode  GetEvents(AthAlgorithm* a,std::vector<HepMC::GenEvent*>& evts, const std::string km)
{
std::vector<const HepMC::GenEvent*> input;
StatusCode ret =GetEventsFromCollection(a,input,km);
for (auto a: input) evts.push_back(PrepareHepMCAnalysisGenEvent(a));
return ret;
}
#endif
