
/***********************************************************************
* Copyright 1998-2022 CERN for the benefit of the EvtGen authors       *
*                                                                      *
* This file is part of EvtGen.                                         *
*                                                                      *
* EvtGen is free software: you can redistribute it and/or modify       *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation, either version 3 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
* EvtGen is distributed in the hope that it will be useful,            *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU General Public License for more details.                         *
*                                                                      *
* You should have received a copy of the GNU General Public License    *
* along with EvtGen.  If not, see <https://www.gnu.org/licenses/>.     *
***********************************************************************/

#ifndef EVT_I_PHOTOS_HH
#define EVT_I_PHOTOS_HH

#include "EvtGenBase/EvtAbsRadCorr.hh"

#include <string>

class EvtParticle;
class EvtAbsExternalGen;

// Description: EvtGen's interface to PHOTOS for generation of
//              QED final state radiation.

class EvtPHOTOS : public EvtAbsRadCorr {
  public:
    void doRadCorr( EvtParticle* p ) override;

  private:
    EvtAbsExternalGen* m_photosEngine = nullptr;
};

#endif
