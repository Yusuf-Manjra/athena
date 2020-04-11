/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAGFIELDCONDITIONS_ATLASMTFIELDCONDOBJ
#define MAGFIELDCONDITIONS_ATLASMTFIELDCONDOBJ

// MagField includes
#include "AthenaKernel/CondCont.h" 
#include "GaudiKernel/ServiceHandle.h"
#include "MagFieldElements/AtlasFieldCache.h"

// forward declarations
namespace MagField {
    class AtlasFieldMap;
}


class AtlasFieldCacheCondObj {

public:
    AtlasFieldCacheCondObj();

    ~AtlasFieldCacheCondObj();

    /// Temporary flag for switching between 'old' and 'new' magField usage
    bool useNewBfieldCache() { return m_useNewBfieldCache; }

    /** get B field cache for evaluation as a function of 2-d (solenoid) or 3-d (toroid) position. 
        Resets cache to an initialized state */
    inline void getInitializedCache (MagField::AtlasFieldCache& cache) const;

    /** access to solenoid field scale factor */
    double solenoidFieldScaleFactor() const { return m_solFieldScale; } ;

    /** access to toroid field scale factor */
    double toriodFieldScaleFactor() const { return m_torFieldScale; } ;

    /** set values for field scale and service to be able to build the cache **/
    bool initialize(double solFieldScale, double torFieldScale, 
                    const MagField::AtlasFieldMap* fieldMap,
                    bool useNewBfieldCache);

private:
    /// Temporary flag for switching between 'old' and 'new' magField usage
    bool                             m_useNewBfieldCache{false};
    double                           m_solFieldScale{1};
    double                           m_torFieldScale{1};
    const MagField::AtlasFieldMap*   m_fieldMap{nullptr};
};

// inline function(s)

void
AtlasFieldCacheCondObj::getInitializedCache (MagField::AtlasFieldCache& cache) const
{

    // setup with field scale and magnetic field service for first access to field */
    cache = MagField::AtlasFieldCache(m_solFieldScale, m_torFieldScale, m_fieldMap, m_useNewBfieldCache);
}


#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( AtlasFieldCacheCondObj, 258146572, 1)
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF (AtlasFieldCacheCondObj, 3622068);


#endif // MAGFIELDCONDITIONS_ATLASMTFIELDCONDOBJ

