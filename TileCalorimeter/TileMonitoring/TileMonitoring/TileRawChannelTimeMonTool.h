/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TileRawChannelMonTool.h
// PACKAGE:  
//
// AUTHOR:   Lukas Plazak
//	This tool is based on TileRawChannelMonTool
//
// ********************************************************************
#ifndef TILEMONITORING_TILERAWCHANNELTIMEMONTOOL_H
#define TILEMONITORING_TILERAWCHANNELTIMEMONTOOL_H

#include "TileMonitoring/TileFatherMonTool.h"
#include "TileIdentifier/TileRawChannelUnit.h"

class TileRawChannelTime;
class TileBeamInfoProvider;
class ITileBadChanTool;
class TileDQstatus;

#include <vector>

/** @class TileRawChannelTimeMonTool
 *  @brief Class for TileCal monitoring at channel level
 */

class TileRawChannelTimeMonTool: public TileFatherMonTool {

  public:

    TileRawChannelTimeMonTool(const std::string & type, const std::string & name, const IInterface* parent);

    ~TileRawChannelTimeMonTool();

    StatusCode initialize();

    //pure virtual methods
    StatusCode bookHists();
    StatusCode fillHists();
    StatusCode finalHists();
    StatusCode checkHists(bool fromFinalize);

    void bookHists(int ros, int drawer);


  private:

    int m_runType;
    std::string m_contName;

    ToolHandle<TileBeamInfoProvider> m_beamInfo;
    ToolHandle<ITileBadChanTool> m_tileBadChanTool;

    const TileDQstatus* m_DQstatus;

    enum RunType {
      Unknown = 0, PhysRun = 1, // expect monogain
      LasRun = 2, // expect monogain
      LedRun = 3, // expect monogain
      PedRun = 4, // expect bigain
      CisRun = 8, // expect bigain
      MonoRun = 9, // expect monogain
      CisRamp = 10 // expect monogain
    };

    bool m_doOnline;
    int32_t m_old_lumiblock;
    int32_t m_delta_lumiblock;

    bool m_bigain;
    int m_nEvents;

    std::vector<bool> m_bookProfHistOnce;

    //Pointers to Histograms

    TProfile2D * profile2d_hist[5];
    TProfile * profile_hist[5][64][8];

    float m_lowGainThreshold;
    float m_hiGainThreshold;
    float m_thresholds[2];

};

#endif
