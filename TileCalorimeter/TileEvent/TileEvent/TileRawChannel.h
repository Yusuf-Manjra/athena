// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// Filename : TileRawChannel.h
// Author   : Ed Frank, Ambreesh Gupta, Frank Merritt
// Created  : Mar, 2002
//
// DESCRIPTION:
//    A TileRawChannel is the final result of simulation, and represents the
//    raw information (pulse height, time, quality) for the in-time beam
//    crossing in the Tile channel.
//    Optimal filtering has been carried out, but energy has not been
//    extracted.
//
// HISTORY:
//    02Mar02  Created to replace the (now obsolete) TileRawCell class.
//    20Sep02  Move to DataVector (A.Solodkov)
//    02Oct02  Now sub-class of TileRawData (A.Solodkov)
//
// BUGS:
//    Question:  the amplitude in a TileRawChannel is in ADC counts (not
//    calibrated).  What about time?
//
// ***************************************************************************

#ifndef TILEEVENT_TILERAWCHANNEL_H
#define TILEEVENT_TILERAWCHANNEL_H

#include "TileEvent/TileRawData.h"

class TileRawChannel: public TileRawData {
  public:

    /* Constructors */

    TileRawChannel()
        : m_pedestal(0) {
    }

    TileRawChannel(const Identifier& id, float amplitude, float time, float quality, float ped = 0.0);
    TileRawChannel(const HWIdentifier& HWid, float amplitude, float time, float quality, float ped = 0.0);

    /* Destructor */

    ~TileRawChannel() {
    }

    int add(float amplitude, float time, float quality);
    void setPedestal(float ped) { m_pedestal = ped;  }
    void scaleAmplitude(float scale);
    void insertTime(float time);
    int insert(float amplitude, float time, float quality);

    /* Inline access methods */

    inline float amplitude(int ind = 0) const { return m_amplitude[ind]; }
    inline float time(int ind = 0)      const { return m_time[ind]; }
    inline float uncorrTime()           const {  return m_time[m_time.size() - 1]; }
    inline float quality(int ind = 0)   const { return m_quality[ind]; }
    inline float pedestal(void)         const { return m_pedestal; }

    inline int size(void) const { return m_amplitude.size(); }
    inline int sizeTime(void) const { return m_time.size(); }

    std::string whoami(void) const { return "TileRawChannel"; }
    void print(void) const;
    // Conversion operator to a std::string 
    // Can be used in a cast operation : (std::string) TileRawChannel
    operator std::string() const;

  private:

    std::vector<float> m_amplitude;  // amplitude in ADC counts (max=1023)
    std::vector<float> m_time;       // time relative to triggering bunch
    std::vector<float> m_quality;    // quality of the sampling distribution
    float m_pedestal;   // reconstructed pedestal value

    // quality is a number in [0,1] obtained by integrating the parent
    // distribution for the "goodness" from the DSP.  In hypothesis testing
    // terms, it is the significance of the hypothisis that the input
    // to the DSP matches the signal-model used to tune the DSP.
};

#endif  //TILEEVENT_TILERAWCHANNEL_H

