/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
//#include "L1CaloFEXToolInterfaces/IjFEXtauAlgo.h"
//#include "L1CaloFEXToolInterfaces/IjFEXegAlgo.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
//#include "L1CaloFEXSim/jFEXOutputCollection.h"

namespace LVL1 {
  
  //Doxygen class description below:
  /** The jFEXFPGA class defines the structure of a single jFEX FPGA
      Its purpose is:
      - to emulate the steps taken in processing data for a single jFEX FPGA in hardware and firmware
      - It will need to interact with jTowers and produce the eTOBs.  It will be created and handed data by jFEXSim
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

    virtual void SetTowersAndCells_SG( int [][17] ) override ;
    virtual void SetTowersAndCells_SG( int [][24] ) override ;

    /** Internal data */
  private:

    int m_id;
    int m_jfexid;

    int m_jTowersIDs_Wide [16*2][17];
    int m_jTowersIDs_Thin [16*2][24];
    std::map<int,jTower> m_jTowersColl;

    CaloCellContainer m_sCellsCollection;

    SG::ReadHandleKey<LVL1::jTowerContainer> m_jFEXFPGA_jTowerContainerKey {this, "MyETowers", "jTowerContainer", "Input container for jTowers"};

    //SG::ReadHandleKey<jFEXOutputCollection> m_jFEXFPGA_jFEXOutputCollectionKey {this, "MyOutputs", "jFEXOutputCollection", "Input container for jFEXOutputCollection"};

    //ToolHandle<IjFEXtauAlgo> m_jFEXtauAlgoTool {this, "jFEXtauAlgoTool", "LVL1::jFEXtauAlgo", "Tool that runs the jFEX tau algorithm"};
    //ToolHandle<IjFEXegAlgo> m_jFEXegAlgoTool {this, "jFEXegAlgoTool", "LVL1::jFEXegAlgo", "Tool that runs the jFEX e/gamma algorithm"};
    
  };
  
} // end of namespace

//CLASS_DEF( LVL1::jFEXFPGA , 136060357 , 1 )


#endif
