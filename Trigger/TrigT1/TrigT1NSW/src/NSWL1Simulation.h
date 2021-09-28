/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1NSW_NSWL1SIMULATION_H
#define TRIGT1NSW_NSWL1SIMULATION_H

// Basic includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaMonitoring/IMonitorToolBase.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"

// NSWL1SimTools includes
#include "TrigT1NSWSimTools/IPadTdsTool.h"
#include "TrigT1NSWSimTools/IStripTdsTool.h"
#include "TrigT1NSWSimTools/IStripClusterTool.h"
#include "TrigT1NSWSimTools/IStripSegmentTool.h"
#include "TrigT1NSWSimTools/IMMStripTdsTool.h"
#include "TrigT1NSWSimTools/IMMTriggerTool.h"
#include "TrigT1NSWSimTools/IPadTriggerLogicTool.h"
#include "TrigT1NSWSimTools/IPadTriggerLookupTool.h"

#include "MuonRDO/NSW_PadTriggerDataContainer.h"
#include "TrigT1NSWSimTools/PadTriggerAdapter.h"

#include "xAODEventInfo/EventInfo.h"

// Forward includes
class StoreGateSvc;
class IMonitorToolBase;
class TTree;


// namespace for the NSW LVL1 related classes
namespace NSWL1 {


  /**
   *
   *   @short NSW L1 simulation algorithm
   *
   * This is the algorithm that masters the simulation of the Level-1 into the NSW.
   * This algorithm will manage the configuration of the NSW LV1 and handle the tools
   * used for simulating the hardware components, for monitoring the basic functionalities
   * and for collecting the data into an ntuple (parameter definition and performance 
   * studies). Depending of the job configuration it executes the pure offline simulation
   * or the detailed hardware simulation.
   *
   *  @authors Alessandro Di Mattia <dimattia@cern.ch>, Geraldine Conti <geraldine.conti@cern.ch>
   *  Major updates for Release 22 processing: Francesco Giuseppe Gravili <francesco.giuseppe.gravili@cern.ch>
   *
   */

  class ATLAS_NOT_THREAD_SAFE  // use of TTree in execute
  NSWL1Simulation: public AthAlgorithm {

  public:

    NSWL1Simulation( const std::string& name, ISvcLocator* pSvcLocator );

    virtual StatusCode initialize() override;
    virtual StatusCode start() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

    //! handle to result builder (for easy data access), now returning a dummy int
    int resultBuilder() const;

  

  private:

    // Needed tools:
    ToolHandleArray < IMonitorToolBase > m_monitors;        //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink 
    ToolHandle < IPadTdsTool >           m_pad_tds;         //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink
    ToolHandle < IPadTriggerLogicTool >  m_pad_trigger;     //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink
    ToolHandle < IPadTriggerLookupTool >  m_pad_trigger_lookup;
    ToolHandle < IStripTdsTool >         m_strip_tds;       //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink
    ToolHandle < IStripClusterTool >     m_strip_cluster;   //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink
    ToolHandle < IStripSegmentTool >     m_strip_segment;   //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink
    ToolHandle < IMMStripTdsTool >       m_mmstrip_tds;     //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink                                                   
    ToolHandle < IMMTriggerTool >        m_mmtrigger;       //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink

    // put analysis variables here
    TTree*       m_tree;                                    //!< analysis ntuple
    unsigned int m_current_run;                             //!< current run number
    unsigned int m_current_evt;                             //!< current event number


    // properties: steering flags
    bool        m_doOffline;                                //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink
    bool        m_useLookup;
    bool        m_doNtuple;                                 //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink
    bool        m_doMM;                                     //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink 
    bool        m_doMMDiamonds;                             //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink
    bool        m_dosTGC;                                   //!< property, see @link NSWL1Simulation::NSWL1Simulation @endlink
    
    
    protected:
    SG::ReadHandleKey<xAOD::EventInfo> m_xaodevtKey{this,"xAODEventInfoKey","EventInfo","Key of xAOD::EventInfo"};
    SG::WriteHandleKey<Muon::NSW_TrigRawDataContainer> m_trigRdoContainer;
    SG::WriteHandleKey<Muon::NSW_PadTriggerDataContainer> m_padTriggerRdoKey;

  };  // end of NSWL1Simulation class

} // namespace NSWL1



#endif
