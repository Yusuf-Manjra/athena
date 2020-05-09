/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCHAMBERT0S_CHAMBERT0S_H
#define MUONCHAMBERT0S_CHAMBERT0S_H

#include "Identifier/Identifier.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <algorithm>
#include <ext/functional>

namespace Muon {
/**
    Stores links between chambers and the reconstructed T0.
   @author edward.moyse@cern.ch   
*/
  class ChamberT0s {
  public:
      /** Value returned by ChamberT0s::getT0(...) when chamber is not known to ChamberT0s*/
      static float ChamberUnknown ;
      
      ChamberT0s( ) ;
      
      ChamberT0s( const std::vector< std::pair < Identifier, float > >& t0s ) ;
      ChamberT0s(       std::vector< std::pair < Identifier, float > >&& t0s ) ;
      
      ~ChamberT0s();
      
      /** Returns true if we have the T0 for this chamber? */
      bool haveChamber(const Identifier& id) const;
      
      /** Returns the T0 for this chamber (or ChamberT0s::ChamberUnknown if the chamber is not in list)
      
      You could use this as e.g.
      @code
      float t0 = chamberT0.getT0( chamberId ); 
      if (t0!=ChamberT0s::ChamberUnknown) {
        // we know about chamber, so use T0
      }
      @endcode
      */
      float getT0( const Identifier& id) const;

      /** Returns entire list of T0s. 
      It consists of a vector of pairs, where the first element is the Chamber Identifier, and the 
      second is the T0*/
      const std::vector< std::pair < Identifier, float > >& getAllT0s() const;
      
  private:
      std::vector< std::pair < Identifier, float > > m_t0s;
  };

inline bool ChamberT0s::haveChamber(const Identifier& id) const {
    std::vector< std::pair < Identifier, float > >::const_iterator it 
      = std::find_if(m_t0s.begin(), m_t0s.end(), __gnu_cxx::compose1(bind2nd(std::equal_to<Identifier>(), id), __gnu_cxx::select1st<std::pair <Identifier, float> >()));
    return (it!=m_t0s.end());
}

inline float ChamberT0s::getT0(const Identifier& id) const {
    std::vector< std::pair < Identifier, float > >::const_iterator it 
      = std::find_if(m_t0s.begin(), m_t0s.end(), __gnu_cxx::compose1(bind2nd(std::equal_to<Identifier>(), id), __gnu_cxx::select1st<std::pair <Identifier, float> >()));
    if (it==m_t0s.end()) return ChamberUnknown; // No such chamber known.
    return (it->second);
}

inline const std::vector< std::pair < Identifier, float > >& ChamberT0s::getAllT0s() const {
    return m_t0s;
}

}

CLASS_DEF( Muon::ChamberT0s , 42363316 , 1 )

#endif
