/*
   Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUONRDO_VERSION_NRPCRDO_V1_H
#define XAODMUONRDO_VERSION_NRPCRDO_V1_H

#include "AthContainers/AuxElement.h"

namespace xAOD {

    /// @class NRPCRDO_v1

    class NRPCRDO_v1 : public SG::AuxElement {

    public:

        /// Default constructor
        NRPCRDO_v1() = default;
        /// Virtual destructor
        virtual ~NRPCRDO_v1() = default;

        // Method to access stored quantities
        
        /// Bunch crossing identifier
        uint32_t bcid() const;
        /// Time of the trigger signal within the bunch crossing
        float time() const;
        /// Global identifier of the RPC detector region from the online side
        uint16_t subdetector() const;
        /// Identifier of the sector within the subdetector
        uint16_t tdcsector() const;
        /// Identifier of the readout card on the chamber
        uint16_t tdc() const;
        /// Fired channel on the read out card
        uint16_t channel() const;
        /// Measured time in which the signal was above the electronics threshold
        float timeoverthr() const;

        /// Set the bunch crossing identifier
        void setBcid(uint32_t Bcid);
        /// Set the trigger time [ns]
        void setTime(float Time);
        /// Set the time over threshold
        void setTimeoverthr(float Timeoverthr);
        /// Set the sub detector
        void setSubdetector(uint16_t SubDet);
        /// Set the sector of the tdc within the subdetector
        void setTdcsector(uint16_t Tdcsector);
        /// Set the number of the TDC channel
        void setTdc(uint16_t Tdc);
        /// Set the fire channel number
        void setChannel(uint16_t Channel);
    };

}

#include "AthContainers/DataVector.h"
SG_BASE( xAOD::NRPCRDO_v1, SG::AuxElement );
//DATAVECTOR_BASE( xAOD::NRPCRDO_v1, SG::AuxElement);

#endif // XAODMUONRDO_VERSION_NRPCRDO_V1_H
