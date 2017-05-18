/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILECELLBUILDERFROMHIT_H
#define TILERECUTILS_TILECELLBUILDERFROMHIT_H

/********************************************************************
 *
 * NAME:     TileCellBuilderFromHit
 * PACKAGE:  offline/TileCalorimeter/TileRecUtils
 *
 * AUTHOR :  A. Solodkov
 * CREATED:  10-Oct-2015
 *
 * PURPOSE:  Create Cells from Hits and store them in container
 *
 *  Input: TileHit from TileHitContainer
 *  Output: Container or collection with TileCells
 *  Parameters:
 *    TileHitContainer - Name of input container
 *   
 ********************************************************************/

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "CLHEP/Random/RandomEngine.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/HWIdentifier.h"
#include "AthenaKernel/IOVSvcDefs.h"

// Calo includes
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloConditions/CaloAffectedRegionInfo.h"

// Tile includes
#include "TileEvent/TileCellContainer.h"
#include "TileIdentifier/TileFragHash.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileRecUtils/TileCellBuilder.h"

// forward declarations
class TileID;
class TileTBID;
class TileHWID;
class TileInfo;
class TileCell;
class MbtsDetDescrManager;
class TileDetDescrManager;
class TileCellCollection;
class CaloCellContainer;
class ITileBadChanTool;
class TileCondToolEmscale;
class TileCablingService;
class ICalorimeterNoiseTool;
class IAtRndmGenSvc;

// C++ STL includes
#include <string>
#include <vector>

/**
 @class TileCellBuilderFromHit
 @brief This class creates Cells from RawChannels and stores them in a container
 
 */
class TileCellBuilderFromHit: public AthAlgTool, virtual public ICaloCellMakerTool {
  public:
    TileCellBuilderFromHit(const std::string& type, const std::string& name, const IInterface* parent); //!< Contructor

    virtual ~TileCellBuilderFromHit(); //!< Destructor

    virtual StatusCode initialize();                     //!< initialize mehtod

    //!< Callback added to handle Data-driven GeoModel initialisation
    virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

    virtual StatusCode finalize(); //!< finalize method

    virtual StatusCode process(CaloCellContainer* theCellContainer); // method to process all raw channels and store them in container

    template<class ITERATOR, class COLLECTION>
    void build(const ITERATOR & begin, const ITERATOR & end, COLLECTION * coll); //!< method to process raw channels from a given vector and store them in collection

    /** method to check if channels are good or bad. Puts zero if both channels are bad
     or recovers from single-channel failure. It returns true if cell was changed, false otherwise
     */
    bool maskBadChannel(TileCell* pCell);
    bool maskBadChannels(TileCell* pCell, bool single_PMT_C10, bool Ecell);

    //AlgTool InterfaceID
    static const InterfaceID& interfaceID();
    //static const InterfaceID& interfaceID() { return ICaloCellMakerTool; };

  private:

    // properties
    std::string m_hitContainer;
    std::string m_MBTSContainer;
    std::string m_E4prContainer;
    std::string m_infoName;

    float m_eneForTimeCut;        //!< keep time for channels with energy above cut
    float m_eneForTimeCutMBTS;    //!< similar cut for MBTS in pC
    float m_zeroEnergy;           //!< energy to store in every PMT if both PMT are bad
    int m_qualityCut;           //!< cut on channel quality (set energy to m_zeroEnergy for them)

    float m_maxTime;              //!< maximum time for the PMTs in the cels
    float m_minTime;              //!< minimum time for the PMTs in the cels
    bool m_maskBadChannels;      //!< if true=> bad channels are masked
    bool m_fakeCrackCells;       //!< if true=> fake E3/E4 cells added
    bool m_useNoiseTool;         //!< if true=> add noise to all cells
    float m_noiseSigma;          //!< cell electroing noise if the CaloNoiseTool is switched off 
    float m_channelNoiseSigma;   //!< cell electroing noise divided by sqrt(2.);

    const TileID* m_tileID;   //!< Pointer to TileID
    const TileTBID* m_tileTBID; //!< Pointer to TileTBID
    const TileHWID* m_tileHWID; //!< Pointer to TileHWID
    const TileInfo* m_tileInfo; //!< Pointer to TileInfo
    const TileCablingService* m_cabling; //!< Pointer to TileCabling

    CLHEP::HepRandomEngine * m_pHRengine;    //!< Random number generator engine to use
    ServiceHandle<IAtRndmGenSvc> m_rndmSvc;  //!< Random number service to use

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool
    ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool
    ToolHandle<ICalorimeterNoiseTool> m_noiseTool; //!< Calo Noise tool with noise values

    const TileDetDescrManager* m_tileMgr; //!< Pointer to TileDetDescrManager
    const MbtsDetDescrManager* m_mbtsMgr; //!< Pointer to MbtsDetDescrManager

    std::vector<TileCell*> m_allCells;  //!< vector to of pointers to TielCells
    std::vector<TileCell*> m_MBTSVec;   //!< vector to of pointers to MBTS cells
    std::vector<TileCell*> m_E4prVec;   //!< vector to of pointers to E4' cells
    TileCellContainer* m_MBTSCells;     //!< Pointer to MBTS cell container
    TileCellContainer* m_E4prCells;     //!< Pointer to E4'  cell container

    TileFragHash::TYPE m_RChType;        //!< Type of TileRawChannels (Fit, OF2, etc.)
    TileRawChannelUnit::UNIT m_RChUnit;  //!< Unit for TileRawChannels (ADC, pCb, etc.)
    //unsigned int m_bsflags;              //!< other flags stored in TileRawChannelContainer
    float m_maxTimeCorr;                 //!< max possible time when time correction is applied

    TileDrawerEvtStatus m_drawerEvtStatus[5][64]; //!< status of every drawer in every event
    TileDrawerRunStatus m_drawerRunStatus[5][64]; //!< overall status of drawer in whole run
    int m_eventErrorCounter[4]; //!< number of events with no errors(0), warnings(1), error(2), total(3)

    std::vector<CaloAffectedRegionInfo> m_affectedRegionInfo_global;
    std::vector<CaloAffectedRegionInfo> m_affectedRegionInfo_current_run;

    void correctCell(TileCell* pCell, int correction, int pmt, int gain, float ener, float time,
        unsigned char iqual, unsigned char qbit); //!< Compute calibrated energy, time, etc. for TileCell and adjust it.

    unsigned char iquality(float qual)  {//!< method to compute the cell quality
         return std::min(255, abs((int) qual));
    } // keep quality within 8 bits make it "unsigned char"

    unsigned char qbits(int ros, int drawer, bool count_over, bool good_time, bool good_ener,
        bool overflow, bool underflow, bool good_overflowfit); //!< method to compute the cell quality bits

    int m_RUN2;
    int m_E1_TOWER;

    static const int nSide = 2;
    static const int nPhi = 8;
    static const int nEta = 2;
    static const int nCellMBTS = nSide * nPhi * nEta;
    inline int mbts_index(int side, int phi, int eta) const {
      return (side * nPhi + phi) * nEta + eta;
    }
    static const int E4side = -1;
    static const int E4eta  = 2;
    static const int E4nPhi = 4;
    static const int nCellE4pr = E4nPhi;
    inline int e4pr_index(int phi) const {
      return  phi;
    }

};

#endif
