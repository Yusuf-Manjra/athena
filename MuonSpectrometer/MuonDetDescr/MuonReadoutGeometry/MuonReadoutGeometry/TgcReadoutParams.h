/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONGEOMODEL_TGCREADOUTPARAMS_H
#define MUONGEOMODEL_TGCREADOUTPARAMS_H

// ******************************************************************************
// class TgcReadoutParams
// ******************************************************************************
//
// Description
// -----------
// Provides access to the TGC readout parameters.  Created from one of the
// "sources": TGC_ZebraDetDescrSource, TGC_NovaDetDescrSource, etc.  One
// object is instantiated per chamber type and is pointed to by the
// TGC_DetDescriptor object corresponding to that type.
//
// Inheritance
// -----------
// None.
//
// Dependencies
// ------------
// None.
//
// Author
// ------
// Steven Goldfarb <Steven.Goldfarb@cern.ch>
//
// ******************************************************************************

#include <string>
#include <memory>

namespace MuonGM {

class TgcReadoutParams
{
public:

   TgcReadoutParams(std::string name, int iCh, int Version, float WireSp, const float NCHRNG,
                    const float* NWGS, const float* IWGS1, const float* IWGS2, const float* IWGS3,
                    const float* ROFFST, const float* NSPS, const float* POFFST);

   // Another constructor for the layout Q
   TgcReadoutParams(std::string name, int iCh, int Version, float WireSp, const int NCHRNG,
                    const float* NWGS, const float* IWGS1, const float* IWGS2, const float* IWGS3,
                    float PDIST, const float* SLARGE, const float* SSHORT,
                    const float* ROFFST, const float* NSPS, const float* POFFST);

   ~TgcReadoutParams();

   inline const std::string GetName() const;
   int chamberType() const;
   int readoutVersion() const;
   int nPhiChambers() const;
   int nGaps() const;

   // Access to wire gang parameters

   float wirePitch() const;
   inline float gangThickness() const;
   int nGangs(int gasGap) const;
   int totalWires(int gasGap) const;
   int nWires(int gasGap, int gang) const;
   int gangOffset(int gasGap) const;

   // Access to strip parameters

   inline float stripThickness() const;
   int nStrips(int gasGap) const;
   float stripOffset(int gasGap) const;
   float physicalDistanceFromBase() const;
   float stripPositionOnLargeBase(int strip) const;
   float stripPositionOnShortBase(int strip) const;

private:

   // Must be built with the parameters
   TgcReadoutParams();

   // Copy constructor and equivalence operator
   TgcReadoutParams(const TgcReadoutParams& other);
   TgcReadoutParams &operator= (const TgcReadoutParams& other);

   // Readout array sizes
   enum TgcReadoutArraySizes
    {
        MaxNGaps   =   3,
        MaxNGangs  = 180,
        MaxNStrips =  33
    };

   // Data members
   std::string m_chamberName;
   int m_chamberType;
   int m_readoutVersion;
   float m_wirePitch;
   int m_nPhiChambers;

   int m_nGangs[MaxNGaps];
   int m_nWires[MaxNGaps][MaxNGangs];
   int m_gangOffset[MaxNGaps];
   int m_nStrips[MaxNGaps];
   float m_stripOffset[MaxNGaps];
   int m_totalWires[MaxNGaps]; 

   // strip postion on the bases for the first layer in +Z
   float m_physicalDistanceFromBase;
   float m_stripPositionOnLargeBase[MaxNStrips];
   float m_stripPositionOnShortBase[MaxNStrips];

   // Hard-coded data
   const float m_gangThickness = 0.05;
   const float m_stripThickness = 0.03;

};
float TgcReadoutParams::stripThickness() const
{
  return m_stripThickness;
}
float TgcReadoutParams::gangThickness() const
{
  return m_gangThickness;
}

const std::string TgcReadoutParams::GetName() const
{
  return m_chamberName;
}
} // namespace MuonGM

#endif // MUONGEOMODEL_TGCREADOUTPARAMS_H
