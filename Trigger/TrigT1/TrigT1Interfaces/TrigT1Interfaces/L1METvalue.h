/***************************************************************************
                          L1MET.h  -  description
                             -------------------
    begin                : 19-10-2007
    copyright            : (C) 2007 Alan Watson
    email                : Alan.Watson@cern.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
                                                                                
#ifndef L1METvalue_H
#define L1METvalue_H

#include <stdlib.h> 
                                                                                
namespace LVL1 {

/**
L1METvalue encoding is used for transmission of ET/Ex/Ey sums from JEM to CMM.
This class compresses/uncompresses 8 bit ET (JEM energy sums) to/from quad linear scale
	\todo this should probably be a static class.
  */
class L1METvalue {

public: 
   /** Return MET value as output by MET LUT (7 bit value + 2 bit range)*/
  void calcL1MET(int Ex, int Ey, int& MET, unsigned int& Range, bool& Overflow);
 /** Return MET value at floating point precision used in threshold calculation */
  void calcL1MET(int Ex, int Ey, float& MET, bool& Overflow);
  /** Return MET**2 value at precision used in threshold calculation */
  void calcL1METQ(int Ex, int Ey, int& METQ, bool& Overflow);
 /** Return MET value at floating point precision used in threshold calculation */
  void calcL1METSig(int Ex, int Ey, int SumET, float a, float b,
                    int XEmin, int XEmax, int sqrtTEmin, int sqrtTEmax,
                    float& XS, int& OutOfRange);
  void calcL1METSig(int MET, unsigned int Range, int SumET, float a, float b,
                    int XEmin, int XEmax, int sqrtTEmin, int sqrtTEmax,
                    float& XS, int& OutOfRange);
 
private: 
  /** Number of bits in range */
  static const unsigned int m_nBits = 6;
  /** Number of ET ranges to encode in */
  static const unsigned int m_nRanges = 4;
  /** Mask to select Ex/Ey bits */
  static const unsigned int m_mask = 0x3F;
  /** Masks for integer MET word setting */
  static const unsigned int m_valueMask = 0x7F;
  static const unsigned int m_rangeMask = 0x180;
  /** Limits on inputs to XS LUT */
  static const int m_xsXEmax = 127;
  static const int m_xsSqrtTEmax  = 63;
};

}//end of ns

#endif
