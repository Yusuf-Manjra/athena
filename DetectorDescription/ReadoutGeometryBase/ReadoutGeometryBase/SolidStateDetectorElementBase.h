/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SolidStateDetectorElementBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef READOUTGEOMETRYBASE_SOLIDSTATEDETECTORELEMENTBASE_H
#define READOUTGEOMETRYBASE_SOLIDSTATEDETECTORELEMENTBASE_H

// Base class.
#include "TrkDetElementBase/TrkDetElementBase.h"

namespace InDetDD {

class SolidStateDetectorElementBase : public Trk::TrkDetElementBase {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
    public:

        // Constructor
        SolidStateDetectorElementBase(const Identifier &id,
                                      const GeoVFullPhysVol *geophysvol);

        // // Constructor
        // SolidStateDetectorElementBase(const Identifier &id,
        //                               const SiDetectorDesign *design,
        //                               const GeoVFullPhysVol *geophysvol,
        //                               SiCommonItems * commonItems);

        // Destructor
        ~SolidStateDetectorElementBase();

    ///////////////////////////////////////////////////////////////////
    // Protected data:
    ///////////////////////////////////////////////////////////////////
    protected:
        Identifier m_id; // identifier of this detector element
        IdentifierHash m_idHash; // hash id
        //const SiDetectorDesign *m_design; // local description of this detector element

};

} // namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SIDETECTORELEMENT_H
