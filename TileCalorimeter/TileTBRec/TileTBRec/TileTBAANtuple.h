/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
/// Filename : TileTBAANtuple.h
/// Author   : Luca Fiorini (based on TileTBNtuple)
/// Created  : Mar, 2007
///
/// DESCRIPTION
/// 
///    To create TileCal Ntuple file with all digits and rawChannels 
///
/// Properties (JobOption Parameters):
///
///    TileDigitsContainer     string   key value of Digits in TDS 
///    TileBeamElemContainer   string   key of BeamElems in TDS
///    TileRawChannelContainerFlat      key of flat filtered RawChannels in TDS
///    TileRawChannelContainerFit       key of fit filtered RawChannels in TDS
///    TileRawChannelContainerFitCool   key of fit filtered RawChannels in TDS
///    CalibrateEnergy         bool     If calibration should be applied to energy
///    CalibMode               bool     If data is in calibration mode
///    NtupleLoc               string   pathname of ntuple file
///    NtupleID                int      ID of ntuple
///    BC1X1                   float    TDC Conv param
///    BC1X2                   float    TDC Conv param
///    BC1Y1                   float    TDC Conv param
///    BC1Y2                   float    TDC Conv param
///    BC1Z                    float    TDC Conv param
///    BC2X1                   float    TDC Conv param
///    BC2X2                   float    TDC Conv param
///    BC2Y1                   float    TDC Conv param
///    BC2Y2                   float    TDC Conv param
///    BC2Z                    float    TDC Conv param
///
/// TODO: Use 7 samples in bigain runs. Now all array variables are stored in Ntuple as 9 elements long. 
///       Make the code store informations about modules even if they appear during the run.
///       Some change is necessary for the new feature of the reconstruction that can mask empty fragments. 
/// History:
///  
///  
//****************************************************************************
#ifndef TileTBAANtuple_H
#define TileTBAANtuple_H

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "StoreGate/ReadCondHandleKey.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"

// Tile includes
#include "TileConditions/TileSamplingFraction.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileRecUtils/TileRawChannelBuilderFlatFilter.h"

#include "TFile.h"
#include "TMatrixT.h"
#include "TTree.h"
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

class ITHistSvc;
class TileID;
class TileHWID;
//class TileCablingSvc;
class TileBeamElemContByteStreamCnv;
class TileLaserObject;
class TileHit;

class TileTBAANtuple: public AthAlgorithm {
  public:
    //Constructor
    TileTBAANtuple(const std::string& name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TileTBAANtuple() = default;
    StatusCode ntuple_initialize();
    StatusCode ntuple_clear();

    //Gaudi Hooks
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

  private:

    /**
     * @brief Name of TileSamplingFraction in condition store
     */
    SG::ReadCondHandleKey<TileSamplingFraction> m_samplingFractionKey{this,
        "TileSamplingFraction", "TileSamplingFraction", "Input Tile sampling fraction"};


    StatusCode storeRawChannels(std::string cntID
                                , bool calibMode
                                , std::vector<float*>* eneVec
                                , std::vector<float*>* timeVec
                                , std::vector<float*>* chi2Vec
                                , std::vector<float*>* pedVec
                                , std::vector<short*>* ROD_GlobalCRCVec
                                , std::vector<short*>* ROD_DMUBCIDVec
                                , std::vector<short*>* ROD_DMUmemoryErrVec
                                , std::vector<short*>* ROD_DMUSstrobeErrVec
                                , std::vector<short*>* ROD_DMUDstrobeErrVec
                                , std::vector<short*>* ROD_DMUHeadformatErrVec
                                , std::vector<short*>* ROD_DMUHeadparityErrVec
                                , std::vector<short*>* ROD_DMUDataformatErrVec
                                , std::vector<short*>* ROD_DMUDataparityErrVec
                                , std::vector<short*>* ROD_DMUMaskVec);

    StatusCode storeDigits();
    StatusCode storeDigitsFlx();
    StatusCode storeBeamElements();
    StatusCode storeCells();
    StatusCode storeLaser();
    StatusCode storeHitVector();
    StatusCode storeHitContainer();
    void storeHit(const TileHit *hit, int fragType, int fragId, float* ehitVec, float* thitVec,
                  const TileSamplingFraction* samplingFraction);

    StatusCode initList(void);
    StatusCode initListFlx(void);
    StatusCode initNTuple(void);
    //StatusCode connectFile(void);

    void getEta(void);

    void TRIGGER_addBranch(void);
    void MUON_addBranch(void);
    void ECAL_addBranch(void);
    void QDC_addBranch(void);
    void LASER_addBranch(void);
    void ADDER_addBranch(void);
    void CISPAR_addBranch(void);
    void BEAM_addBranch(void);
    void DIGI_addBranch(void);
    void FELIX_addBranch(void);
    //void RAW_addBranch(void);
    void HIT_addBranch(void);
    void ENETOTAL_addBranch(void);
    void COINCBOARD_addBranch(void);
    void LASEROBJ_addBranch(void);

    void TRIGGER_clearBranch(void);
    void MUON_clearBranch(void);
    void ECAL_clearBranch(void);
    void QDC_clearBranch(void);
    void LASER_clearBranch(void);
    void ADDER_clearBranch(void);
    void CISPAR_clearBranch(void);
    void BEAM_clearBranch(void);
    void DIGI_clearBranch(void);
    void FELIX_clearBranch(void);
    //void RAW_clearBranch(void);
    void HIT_clearBranch(void);
    void ENETOTAL_clearBranch(void);
    void COINCBOARD_clearBranch(void);
    void LASEROBJ_clearBranch(void);

    void clear_int(std::vector<int*> & vec, int nchan=48);
    void clear_uint32(std::vector<uint32_t*> & vec, int nchan=48);
    void clear_short(std::vector<short*> & vec, int nchan=48);
    void clear_float(std::vector<float*> & vec, int nchan=48);
    void clear_intint(std::vector<int**> & vec, int nchan=48);
    
    inline int digiChannel2PMT(int fragType, int chan) {
      return (abs(m_cabling->channel2hole(fragType, chan)) - 1);
    }

    /// bit_31 of the DMU header must be 1 and
    /// bit_17 of the DMU header must be 0
    inline short CheckDMUFormat(uint32_t header) {
      if (((header >> 31 & 0x1) == 1) && ((header >> 17 & 0x1) == 0))
        return 0; // no error
      else
        return 1; //error
    }

    /// Parity of the DMU header should be odd

    inline short CheckDMUParity(uint32_t header) {
      uint32_t parity(0);
      for (int i = 0; i < 32; ++i)
        parity += header >> i & 0x1;

      if ((parity % 2) == 1)
        return 0; //no error
      else
        return 1; //error
    }

    void checkIsPropertySetup(float property, const std::string& name) {
      if (property < NOT_SETUP) {
        ATH_MSG_ERROR("The following property should be set up via JO: " << name);
      }
    }

    void setupPropertyDefaultValue(float property, float defaultValue, const std::string& name) {
      if (property < NOT_SETUP) {
        property = defaultValue;
        ATH_MSG_INFO("The following property is not set up via JO, using default value: " << name << "=" << defaultValue);
      }
    }

    void setupBeamChambersBeforeTB2015(void);
    void setupBeamChambersTB2015(void);
    void setupBeamChambersTB2016_2020(void);

    // If data should be put in calib mode
    bool m_calibMode;

    //handle to THistSvc
    ServiceHandle<ITHistSvc> m_thistSvc;

    // The ntuple
    TTree* m_ntuplePtr;
    bool m_ntupleCreated;

    // event number
    int m_evtNr;

    // Trigger items
    int m_evTime;
    int m_run;
    int m_evt;
    int m_trigType;
    int m_dspFlags;

    // 0 - Beam, 1 neg eta, 2 pos eta
    std::vector<int>* m_l1ID;
    std::vector<int>* m_l1Type;
    std::vector<int>* m_evBCID;
    std::vector<int>* m_evType;
    std::vector<int>* m_frBCID;

    // Muon items
    float m_muBackHit;
    float m_muBackSum;
    float* m_muBack; // MUON/MuBack
    float* m_muCalib; // MUON/MuCalib

    // Ecal
    float* m_ecal;

    // QDC
    uint32_t* m_qdc;

    // laser items
    int m_las_BCID;

    int m_las_Filt;
    double m_las_ReqAmp;
    double m_las_MeasAmp;

    int m_las_D1_ADC;
    int m_las_D2_ADC;
    int m_las_D3_ADC;
    int m_las_D4_ADC;

    double m_las_D1_Ped;
    double m_las_D2_Ped;
    double m_las_D3_Ped;
    double m_las_D4_Ped;

    double m_las_D1_Ped_RMS;
    double m_las_D2_Ped_RMS;
    double m_las_D3_Ped_RMS;
    double m_las_D4_Ped_RMS;

    double m_las_D1_Alpha;
    double m_las_D2_Alpha;
    double m_las_D3_Alpha;
    double m_las_D4_Alpha;

    double m_las_D1_Alpha_RMS;
    double m_las_D2_Alpha_RMS;
    double m_las_D3_Alpha_RMS;
    double m_las_D4_Alpha_RMS;

    double m_las_D1_AlphaPed;
    double m_las_D2_AlphaPed;
    double m_las_D3_AlphaPed;
    double m_las_D4_AlphaPed;

    double m_las_D1_AlphaPed_RMS;
    double m_las_D2_AlphaPed_RMS;
    double m_las_D3_AlphaPed_RMS;
    double m_las_D4_AlphaPed_RMS;

    int m_las_PMT1_ADC;
    int m_las_PMT2_ADC;

    int m_las_PMT1_TDC;
    int m_las_PMT2_TDC;

    double m_las_PMT1_Ped;
    double m_las_PMT2_Ped;

    double m_las_PMT1_Ped_RMS;
    double m_las_PMT2_Ped_RMS;

    double m_las_Temperature;

    int m_lasFlag;
    float m_las0;
    float m_las1;
    float m_las2;
    float m_las3;
    float* m_lasExtra;

    // pattern Unit in common beam crate
    int m_commonPU;

    // Adder items
    int** m_adder;
    //std::vector<int>* m_addx;
    float* m_eneAdd;
    float* m_timeAdd;

    // Cispar
    int m_cispar[16];

    // TDC/BEAM Items
    uint32_t m_s1cou;
    uint32_t m_s2cou;
    uint32_t m_s3cou;
    uint32_t m_cher1;
    uint32_t m_cher2;
    uint32_t m_cher3;
    uint32_t m_muTag;
    uint32_t m_muHalo;
    uint32_t m_muVeto;

    int m_s2extra;
    int m_s3extra;

    int m_sc1;
    int m_sc2;

    int* m_tof;
    int* m_btdc1;
    int* m_btdc2;
    std::vector<std::vector<int> > *m_btdc;
    int m_tjitter;
    int m_tscTOF;
    int m_btdcNhit[16];
    int m_btdcNchMultiHit[2];

    float m_xChN2;
    float m_yChN2;
    float m_xChN1;
    float m_yChN1;
    float m_xCha0;
    float m_yCha0;

    float m_xCha1;
    float m_yCha1;
    float m_xCha2;
    float m_yCha2;
    float m_xCha1_0;
    float m_yCha1_0;
    float m_xCha2_0;
    float m_yCha2_0;
    float m_xImp;
    float m_yImp;

    float m_xImp_0;
    float m_yImp_0;
    float m_xImp_90;
    float m_yImp_90;
    float m_xImp_min90;
    float m_yImp_min90;
    // Digi/Energy items
    std::vector<int*> m_evtVec;
    std::vector<short*> m_rodBCIDVec;
    std::vector<short*> m_sizeVec;

    std::vector<int*> m_evtflxVec;
    std::vector<short*> m_rodBCIDflxVec;
    std::vector<short*> m_sizeflxVec;

    std::vector<int*> m_gainflxVec;
    std::vector<int**> m_sampleflxVec;

    std::vector<int*> m_bcidVec;
    std::vector<uint32_t*> m_DMUheaderVec;
    std::vector<short*> m_DMUformatErrVec;
    std::vector<short*> m_DMUparityErrVec;
    std::vector<short*> m_DMUmemoryErrVec;
    std::vector<short*> m_DMUDstrobeErrVec;
    std::vector<short*> m_DMUSstrobeErrVec;
    std::vector<int*> m_dmuMaskVec;
    std::vector<int*> m_slinkCRCVec;
    std::vector<int*> m_gainVec;
    //std::vector< TMatrixT<double> * >  m_sampleVec;
    std::vector<int**> m_sampleVec;
    std::vector<int*> m_feCRCVec; //we use int, because vector<bool> and shorts are bugged
    std::vector<int*> m_rodCRCVec;

    std::vector<float*> m_eneVec;
    std::vector<float*> m_timeVec;
    std::vector<float*> m_pedFlatVec;
    std::vector<float*> m_chi2FlatVec;
    std::vector<float*> m_efitVec;
    std::vector<float*> m_tfitVec;
    std::vector<float*> m_pedfitVec;
    std::vector<float*> m_chi2Vec;
    std::vector<float*> m_efitcVec;
    std::vector<float*> m_tfitcVec;
    std::vector<float*> m_pedfitcVec;
    std::vector<float*> m_chi2cVec;
    std::vector<float*> m_eOptVec;
    std::vector<float*> m_tOptVec;
    std::vector<float*> m_pedOptVec;
    std::vector<float*> m_chi2OptVec;
    std::vector<float*> m_eDspVec;
    std::vector<float*> m_tDspVec;
    std::vector<float*> m_chi2DspVec;

    std::vector<float*> m_eflxfitVec;
    std::vector<float*> m_tflxfitVec;
    std::vector<float*> m_chi2flxfitVec;
    std::vector<float*> m_pedflxfitVec;
    std::vector<float*> m_eflxoptVec;
    std::vector<float*> m_tflxoptVec;
    std::vector<float*> m_chi2flxoptVec;
    std::vector<float*> m_pedflxoptVec;

    std::vector<short*> m_ROD_GlobalCRCVec;
    std::vector<short*> m_ROD_DMUBCIDVec;
    std::vector<short*> m_ROD_DMUmemoryErrVec;
    std::vector<short*> m_ROD_DMUSstrobeErrVec;
    std::vector<short*> m_ROD_DMUDstrobeErrVec;
    std::vector<short*> m_ROD_DMUHeadformatErrVec;
    std::vector<short*> m_ROD_DMUHeadparityErrVec;
    std::vector<short*> m_ROD_DMUDataformatErrVec;
    std::vector<short*> m_ROD_DMUDataparityErrVec;
    std::vector<short*> m_ROD_DMUMaskVec;

    std::vector<int*> m_mdL1idVec;
    std::vector<int*> m_mdBcidVec;
    std::vector<int*> m_mdModuleVec;
    std::vector<int*> m_mdRunTypeVec;
    std::vector<int*> m_mdRunVec;
    std::vector<int*> m_mdChargeVec;
    std::vector<int*> m_mdChargeTimeVec;
    std::vector<int*> m_mdCapacitorVec;

    std::vector<int*> m_mdL1idflxVec;
    std::vector<int*> m_mdBcidflxVec;
    std::vector<int*> m_mdModuleflxVec;
    std::vector<int*> m_mdRunTypeflxVec;
    std::vector<int*> m_mdPedLoflxVec;
    std::vector<int*> m_mdPedHiflxVec;
    std::vector<int*> m_mdRunflxVec;
    std::vector<int*> m_mdChargeflxVec;
    std::vector<int*> m_mdChargeTimeflxVec;
    std::vector<int*> m_mdCapacitorflxVec;

    float* m_LarEne;
    float* m_BarEne;
    float* m_ExtEne;
    float* m_GapEne;

    unsigned int* m_coincTrig1;
    unsigned int* m_coincTrig2;
    unsigned int* m_coincTrig3;
    unsigned int* m_coincTrig4;
    unsigned int* m_coincTrig5;
    unsigned int* m_coincTrig6;
    unsigned int* m_coincTrig7;
    unsigned int* m_coincTrig8;

    int m_coincFlag1;
    int m_coincFlag2;
    int m_coincFlag3;
    int m_coincFlag4;
    int m_coincFlag5;
    int m_coincFlag6;
    int m_coincFlag7;
    int m_coincFlag8;

    std::vector<std::string> m_rosName; // name of arrays in ntuple for different ROSes
    std::vector<std::string> m_drawerList; // list of frag IDs in correct order
    std::vector<int> m_drawerType; // type of every drawer 1-4: B+, B-, EB+, EB-
    std::map<unsigned int, unsigned int, std::less<unsigned int> > m_drawerMap; // map for frag IDs -> index
    typedef std::map<unsigned int, unsigned int, std::less<unsigned int> >::iterator drawerMap_iterator;

    std::vector<std::string> m_beamFragList; // list of beam frag IDs to store in the ntuple
    bool m_beamIdList[32]; // list of beam frag IDs to store in the ntuple

    // Params for Beam TDC
    // Beam chambers -1 and -2
    float m_beamBN2X1;
    float m_beamBN2X2;
    float m_beamBN2Y1;
    float m_beamBN2Y2;
    float m_beamBN2Z;

    float m_beamBN1X1;
    float m_beamBN1X2;
    float m_beamBN1Y1;
    float m_beamBN1Y2;
    float m_beamBN1Z;

    // Beam chamber 0
    float m_beamBC0X1;
    float m_beamBC0X2;
    float m_beamBC0Y1;
    float m_beamBC0Y2;
    float m_beamBC0Z;

    // Beam chambers 1 and 2
    float m_beamBC1X1;
    float m_beamBC1X2;
    float m_beamBC1Y1;
    float m_beamBC1Y2;
    float m_beamBC1Z;

    float m_beamBC2X1;
    float m_beamBC2X2;
    float m_beamBC2Y1;
    float m_beamBC2Y2;
    float m_beamBC2Z;
    float m_beamBC2Zperiod1;
    float m_beamBC2Zperiod2;

    float m_beamBC1Z_0;
    float m_beamBC1Z_90;
    float m_beamBC1Z_min90;
    float m_beamBC2Z_0;
    float m_beamBC2Z_90;
    float m_beamBC2Z_min90;
    //run number
    int m_runNumber;
    float m_eta;
    float m_theta;
    std::string m_etaFileName;

    //inner radius of calo
    float m_radius;

    //MC truth info
    std::vector<float*> m_ehitVec;
    std::vector<float*> m_thitVec;
    std::vector<float*> m_ehitCnt;
    std::vector<float*> m_thitCnt;
    // Container Parameters
    std::string m_digitsContainer;
    std::string m_digitsContainerFlx;
    std::string m_flxFitRawChannelContainer;
    std::string m_flxOptRawChannelContainer;
    std::string m_beamElemContainer;
    std::string m_flatRawChannelContainer;
    std::string m_fitRawChannelContainer;
    std::string m_fitcRawChannelContainer;
    std::string m_optRawChannelContainer;
    std::string m_dspRawChannelContainer;
    std::string m_laserObject;
    std::string m_hitVector;
    std::string m_hitContainer;
    std::string m_infoName;

    bool m_calibrateEnergyThisEvent;
    bool m_calibrateEnergy;
    bool m_useDspUnits;
    bool m_unpackAdder;
    bool m_completeNtuple;
    bool m_commitNtuple;
    bool m_bsInput;
    bool m_pmtOrder;  //!< change channel ordering to pmt ordering in ntuple
    int m_finalUnit;  //!< calibrate everything to this level
    int m_TBperiod;

    // NTuple parameters
    std::string m_ntupleID;
    std::string m_ntupleLoc;
    Long64_t m_treeSize;
    int m_nSamples;
    int m_nSamplesFlx;
    unsigned int m_nDrawers;
    TileRawChannelUnit::UNIT m_rchUnit;  //!< Unit for TileRawChannels (ADC, pCb, MeV)
    TileRawChannelUnit::UNIT m_dspUnit;  //!< Unit for TileRawChannels in DSP

    ToolHandle<TileRawChannelBuilderFlatFilter> m_adderFilterAlgTool;

    // Identifiers
    const TileID* m_tileID{nullptr};
    const TileHWID* m_tileHWID{nullptr};
    const TileCablingService* m_cabling{nullptr};

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool

    TileBeamElemContByteStreamCnv* m_beamCnv;

    std::string m_currentNtupleLoc;
    int m_currentFileNum;
    int m_eventsPerFile;

    std::string m_streamName;

    std::map<int, int> m_nSamplesInDrawerMap;
    bool m_saveFelixData{false};

    static const int MAX_MINIDRAWERS = 4;
    static const int NOT_SETUP = -9999;
};

#endif
