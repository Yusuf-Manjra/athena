/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDTOOLOFCCOOL_H
#define TILECONDITIONS_TILECONDTOOLOFCCOOL_H

// Tile includes
#include "TileConditions/TileCalibData.h"
#include "TileConditions/ITileCondToolOfc.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadCondHandleKey.h"



#include <vector>

#define PHASE_STEP 0.1

/**
 *
 * @class TileCondToolOfcCool
 * @brief extracts OFCs from database
 */
class TileCondToolOfcCool: public AthAlgTool, public ITileCondToolOfc {
  public:

    static const InterfaceID& interfaceID();
    TileCondToolOfcCool(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondToolOfcCool();

    StatusCode initialize();
    StatusCode finalize();

    const TileOfcWeightsStruct* getOfcWeights(unsigned int drawerIdx, unsigned int channel
                                              , unsigned int adc, float& phase, bool of2);

    int getOfcWeights(unsigned int drawerIdx, unsigned int channel, unsigned int adc, float& phase,
                      float *a, float *b, float *c, float *g, float *dg);

    void getOfcParams(unsigned int drawerIdx, int &NPhases, int &NFields, int &Phamin, int &Phamax, int &NSamples);

    int getNSamples(void) {return m_NSamples; };

  private:

    SG::ReadCondHandleKey<TileCalibData<TileCalibDrawerOfc>> m_calibOfcKey{this,
        "TileOfc", "TileOfc", "Input Tile OFC constants"};


    TileOfcWeightsStruct* m_weights;

    int m_NPhases;
    int m_NFields;
    int m_NSamples;
    bool m_first = true;
};

#endif
