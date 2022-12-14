/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XAODELECTRONFILTER_H
#define GENERATORFILTERS_XAODELECTRONFILTER_H

#include "GeneratorModules/GenFilter.h"


/// @brief  Filters and looks for electrons
///
/// This class allows the user to search for electrons or positrons. An event
/// will pass the filter successfully if there is an electron or positron with
/// p_t and eta in the specified range. Default is pt > 10 GeV and unlimited
/// eta.  Please note that the parameters (energy, momenta) are in ATLAS units,
/// i.e. CLHEP::MeV!
///
/// @author I Hinchliffe, December 2001
class xAODElectronFilter : public GenFilter {
public:

  xAODElectronFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent();

private:

	double m_Ptmin;
	double m_EtaRange;

};

#endif
