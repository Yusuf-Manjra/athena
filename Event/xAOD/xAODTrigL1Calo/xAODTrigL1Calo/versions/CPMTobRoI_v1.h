// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTobRoI_v1.h 652818 2015-03-09 22:54:24Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMTOBROI_V1_H
#define XAODTRIGL1CALO_VERSIONS_CPMTOBROI_V1_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD{

  /// Description of CPMTobRoI_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 652818 $
  /// $Date: 2015-03-09 23:54:24 +0100 (Mon, 09 Mar 2015) $

  class CPMTobRoI_v1 : public SG::AuxElement {
    public:
      /// Default constructor
      CPMTobRoI_v1();
      /// Default desturctor
      virtual ~CPMTobRoI_v1(){}         

      /// get roiWord
      uint32_t roiWord() const;
      /// set roiWord
      void setRoiWord(uint32_t);
      
      /// Return crate number (0-1)
      int crate()    const;
      /// Return CPM number (1-14)
      int cpm()      const;
      /// Return CP chip number (0-7)
      int chip()     const;
      /// Return location (RoI local coords) (0-7)
      int location() const;
      /// Return type em/tau (0/1)
      int type()     const;
      /// Return energy
      int energy()   const;
      /// Return isolation
      int isolation() const;  
      
    private:
      
      /// RoI word ID
      static const int s_wordIdVal       = 0xa;
      //  Data locations
      static const int s_wordIdBit       = 28;
      static const int s_crateBit        = 26;
      static const int s_cpmBit          = 22;
      static const int s_chipBit         = 19;
      static const int s_locationBit     = 16;
      static const int s_isolBit         = 8;
      static const int s_energyBit       = 0;
      //  Data masks
      static const int s_wordIdMask      = 0xf;
      static const int s_crateMask       = 0x3;
      static const int s_cpmMask         = 0xf;
      static const int s_chipMask        = 0x7;
      static const int s_locationMask    = 0x7;
      static const int s_isolMask        = 0x1f;
      static const int s_energyMask      = 0xff;
      static const int s_typeMask        = 0x1;      

  }; // class CPMTobRoI_v1
} // namespace xAOD
#endif

