/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimKernel.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_ALGS_SIMKERNEL_H
#define ISF_ALGS_SIMKERNEL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// ISF includes
#include "ISF_Event/SimSvcID.h"
#include "ISF_Interfaces/ISimulationSelector.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// forward declarations
namespace PMonUtils {
  class CustomBenchmark;
}
// THistogram service
class ITHistSvc;
// ROOT forward declarations
class TTree;

namespace ISF {

    class IParticleMgr;
    class ITruthSvc;
    class ISimHitSvc;
    class ISimulationSvc;
    class IMonitoringTool;
    class IEventFilterTool;


  /** @class SimKernel

      This is the purely sequential simulation kernel, executed once per event.
      The ISF::SimKernel is configured by providing:
        - one particle stack service (ISF::IParticleMgr )
        - one truth record service (ISF::ITruthSvc)
        - one simulation hit service (ISF::ISimHitSvc)
        - n simulation services for the subdetector geometries ( ISF::ISimulationSvc )

      The AthAlgorithm::initialize() framework call triggers:
        - retrieval of the given components, and subsequent inialize() calls on them
        - translation of the ServiceHandleArray<ISF::ISimulationSvc> into a std::vector<ISF::ISimulationSvc*>
          for fast access of the geometry service

      The AthAlgorithm::execute() framework call triggers the following sequence of calls :
        - (1) the stack filling call to the ParticleMgr
        - (2) the setupEvent() calls to the provided components
        - (3) the loop over the particle stack
        - (4) the releaseEvent() calls to the provided components

      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch

     */
  class SimKernel : public AthAlgorithm {

  public:
    //** Constructor with parameters */
    SimKernel( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~SimKernel();

    /** Athena algorithm's interface method initialize() */
    StatusCode  initialize(); /** Athena algorithm's interface method execute() */
    StatusCode  execute();

    /** Athena algorithm's interface method finalize() */
    StatusCode  finalize();

  private:
    StatusCode initSimSvcs( SimSelectorToolArray &simSelectorTools);

    /** Validation output with histogram service */
    bool                                 m_validationOutput; //!< turn validation mode on/off
    ServiceHandle<ITHistSvc>             m_thistSvc;         //!< the histogram service
    std::string                          m_validationStream; //!< validation THist stream name
    /** Validation output TTree (+variables) */
    TTree                               *m_t_simParticles;   //!< ROOT tree containing simulated particles
    float                                m_val_x;            //!< particle x coordinate
    float                                m_val_y;            //!< particle y coordinate
    float                                m_val_z;            //!< particle z coordinate
    float                                m_val_p;            //!< particle momentum
    float                                m_val_px;
    float                                m_val_py;
    float                                m_val_pz;
    float                                m_val_meta;
    float                                m_val_peta;
    int                                  m_val_pdg;          //!< particle PDG code
    int                                  m_val_simID;
    int                                  m_val_geoID;
    short                                m_val_sc;           //!< StatusCode returned from Simulator

    /** Central particle broker service */
    ServiceHandle<IParticleBroker>       m_particleBroker;
    /** Central truth service */
    ServiceHandle<ITruthSvc>             m_truthRecordSvc;
    /** Central simulation hit service, called afer particle loop */
    ServiceHandle<ISimHitSvc>            m_simHitSvc;

    /** The Simulation Selector Chains */
    ToolHandleArray<ISimulationSelector> m_simSelectors[AtlasDetDescr::fNumAtlasRegions];

    /** The Event Filters */
    ToolHandleArray<IEventFilterTool>    m_eventFilters;

    /** The Memory Info Tool */
    bool                                 m_doMemMon;
    ToolHandle<IMonitoringTool>          m_memMon;
    unsigned int                         m_memUsageEvts;

    /** Simulators to be used */
    std::vector<ISimulationSvc*>         m_simSvcs;     //!< SimSvc handles
    std::vector<std::string>             m_simSvcNames; //!< SimSvc names
    ISF::SimSvcID                        m_numSimSvcs;  //!< total number of SimSvcs used

    /** keep track of the number of events processed */
    unsigned int                         m_numISFEvents;

    /** Screen output refinement */
    std::string                          m_screenOutputPrefix;
    std::string                          m_screenEmptyPrefix;

    /** CPU Benchmarking */
    bool                                 m_doCPUMon;    //!< whether we use PMonUtils or not
    //PMonUtils::CustomBenchmark          *m_benchPDGCode; //TODO: implement this if feasible
    //PMonUtils::CustomBenchmark          *m_benchGeoID;   //TODO: implement this if feasible
    PMonUtils::CustomBenchmark          *m_benchSimID;

    /** Statistics */
    long int                             m_numParticles; //!< total number of particles handled by ISF

    /** tuning */
    size_t                               m_maxParticleVectorSize; //!< number of particles simultaneously sent to simulator
  };
}


#endif //> !ISF_ALGS_SIMKERNEL_H
