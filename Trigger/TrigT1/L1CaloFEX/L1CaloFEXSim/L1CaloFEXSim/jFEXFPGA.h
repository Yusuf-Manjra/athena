/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXFPGA.h  -  
//                              -------------------
//     begin                : 15 10 2019
//     email                : jacob.julian.kempster@cern.ch
//  ***************************************************************************/


#ifndef jFEXFPGA_H
#define jFEXFPGA_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXToolInterfaces/IjFEXFPGA.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXToolInterfaces/IjFEXSmallRJetAlgo.h"
#include "L1CaloFEXToolInterfaces/IjFEXLargeRJetAlgo.h"
#include "L1CaloFEXToolInterfaces/IjFEXtauAlgo.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/jFEXOutputCollection.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"

namespace LVL1 {
  
  //Doxygen class description below:
  /** The jFEXFPGA class defines the structure of a single jFEX FPGA
      Its purpose is:
      - to emulate the steps taken in processing data for a single jFEX FPGA in hardware and firmware
      - It will need to interact with jTowers and produce the jTOBs.  It will be created and handed data by jFEXSim
  */
  
  class jFEXFPGA : public AthAlgTool, virtual public IjFEXFPGA {
    
  public:
    /** Constructors */
    jFEXFPGA(const std::string& type,const std::string& name,const IInterface* parent);

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    /** Destructor */
    virtual ~jFEXFPGA();

    virtual StatusCode init(int id, int efexid) override ;

    virtual StatusCode execute() override ;

    virtual void reset() override ;

    virtual int ID() override {return m_id;}

    virtual void SetTowersAndCells_SG( int [][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width] ) override ;
    virtual void SetTowersAndCells_SG( int [][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width] ) override ;

    /**Form a tob word out of the potential candidate SmallRJet tob */
    virtual uint32_t formSmallRJetTOB(int &, int &) override;
    virtual uint32_t formLargeRJetTOB(int &, int &) override;
    virtual std::vector <uint32_t> getSmallRJetTOBs() override;
    virtual std::vector <uint32_t> getLargeRJetTOBs() override;

    /**Form a tob word out of the potential candidate Tau tob */
    virtual uint32_t formTauTOB(int &, int &) override;
    virtual std::vector <uint32_t> getTauTOBs() override;    
    
 
   /** Internal data */
  private:
    static bool etSort(uint32_t i, uint32_t j){ return (((i >> 0 ) & 0x7ff)> ((j >> 0) & 0x7ff));}
    static bool etTauSort(uint32_t i, uint32_t j) {
      return (((i >> 0 ) & 0x7ff000)> ((j >> 0) & 0x7ff000));
    }
    int m_id;
    int m_jfexid;
    std::vector<uint32_t> m_SRJet_tobwords;
    std::vector<uint32_t> m_LRJet_tobwords;
    std::vector<uint32_t> m_tau_tobwords;

    int m_jTowersIDs_Wide [FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width] = {{0}};
    int m_jTowersIDs_Thin [FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width] = {{0}};
    int m_jTowersIDs      [FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_thin_algoSpace_width] = {{0}};
    std::map<int,jTower> m_jTowersColl;

    CaloCellContainer m_sCellsCollection;

    SG::ReadHandleKey<LVL1::jTowerContainer> m_jFEXFPGA_jTowerContainerKey {this, "MyETowers", "jTowerContainer", "Input container for jTowers"};

    SG::ReadHandleKey<jFEXOutputCollection> m_jFEXFPGA_jFEXOutputCollectionKey {this, "MyOutputs", "jFEXOutputCollection", "Input container for jFEXOutputCollection"};

    ToolHandle<IjFEXSmallRJetAlgo> m_jFEXSmallRJetAlgoTool {this, "jFEXSmallRJetAlgoTool", "LVL1::jFEXSmallRJetAlgo", "Tool that runs the jFEX Small R Jet algorithm"};
    ToolHandle<IjFEXLargeRJetAlgo> m_jFEXLargeRJetAlgoTool {this, "jFEXLargeRJetAlgoTool", "LVL1::jFEXLargeRJetAlgo", "Tool that runs the jFEX Large R Jet algorithm"};
    ToolHandle<IjFEXtauAlgo> m_jFEXtauAlgoTool             {this, "jFEXtauAlgoTool"      , "LVL1::jFEXtauAlgo"      , "Tool that runs the jFEX tau algorithm"};
    //ToolHandle<IjFEXegAlgo> m_jFEXegAlgoTool {this, "jFEXegAlgoTool", "LVL1::jFEXegAlgo", "Tool that runs the jFEX e/gamma algorithm"};
    
  };
  
} // end of namespace

//CLASS_DEF( LVL1::jFEXFPGA , 136060357 , 1 )


#endif
