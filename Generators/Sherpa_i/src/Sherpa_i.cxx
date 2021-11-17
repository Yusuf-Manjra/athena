/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AtlasHepMC/GenEvent.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "Sherpa_i/Sherpa_i.h"

//needed from Sherpa EvtGeneratorz
//#include "SHERPA/Main/Sherpa.H"
/** Begin of  content of Sherpa.H */
#ifndef SHERPA_Main_Sherpa_H
#define SHERPA_Main_Sherpa_H

#include "ATOOLS/Org/CXXFLAGS.H"
#include "ATOOLS/Org/CXXFLAGS_PACKAGES.H"
#include "ATOOLS/Org/Exception.H"
#ifdef HEPMC3
#undef USING__HEPMC2
#else
#undef USING__HEPMC3
#endif


namespace ATOOLS {
  class Blob_List;
  class Cluster_Amplitude;
}

#ifdef USING__HEPMC2
namespace HepMC{ class GenEvent; }
namespace SHERPA { class HepMC2_Interface; }
#endif

#ifdef USING__HEPMC3
namespace HepMC3{ class GenEvent; }
namespace SHERPA { class HepMC3_Interface; }
#endif

namespace SHERPA {
  class Initialization_Handler;
  class Event_Handler;
  class Input_Output_Handler;

  class Sherpa: public ATOOLS::Terminator_Object {
  private:
    std::string   m_path;
    long int      m_trials, m_debugstep, m_debuginterval, m_displayinterval;
    long int      m_evt_output, m_evt_output_start;
    double        m_evt_starttime;

    Initialization_Handler * p_inithandler;
    Event_Handler          * p_eventhandler;
#ifdef USING__HEPMC2
    HepMC2_Interface       * p_hepmc2;
#endif
#ifdef USING__HEPMC3
    HepMC3_Interface       * p_hepmc3;
#endif

    void DrawLogo(const int mode);  

    void PrepareTerminate();

  public:

    Sherpa();
    ~Sherpa();

    bool InitializeTheRun(int argc,char * argv[]);
    bool InitializeTheEventHandler();

    bool GenerateOneEvent(bool reset=true);
#ifdef USING__HEPMC2
    void FillHepMCEvent(HepMC::GenEvent& event);
#endif
#ifdef USING__HEPMC3
    void FillHepMCEvent(HepMC3::GenEvent& event);
#endif

    double TotalXS();
    double TotalErr();

    std::string PDFInfo();

    bool SummarizeRun();

    long int NumberOfEvents() const;

    const ATOOLS::Blob_List &GetBlobList() const;

    double GetMEWeight(const ATOOLS::Cluster_Amplitude &ampl,const int mode=0) const;
    
    inline Initialization_Handler * GetInitHandler() const 
    { return p_inithandler; }
    inline Event_Handler * GetEventHandler() const 
    { return p_eventhandler; }
    
  };
}
#endif 
 /*End of content of Sherpa.H */
#include "SHERPA/Initialization/Initialization_Handler.H"
#include "SHERPA/Tools/Variations.H"
#include "ATOOLS/Org/Exception.H"
#include "ATOOLS/Org/Run_Parameter.H"

#include <cstdio>
#include <cstring>
#include <signal.h>
#include <stdlib.h>
#include "CLHEP/Random/RandFlat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

CLHEP::HepRandomEngine* p_rndEngine;

Sherpa_i::Sherpa_i(const std::string& name, ISvcLocator* pSvcLocator) 
  : GenModule(name, pSvcLocator), p_sherpa(NULL)
{
  declareProperty("RunCard", m_runcard = "");
  declareProperty("Parameters", m_params);
  declareProperty("OpenLoopsLibs", m_openloopslibs);
  declareProperty("ExtraFiles", m_extrafiles);
  declareProperty("NCores", m_ncores=1);
  declareProperty("MemoryMB", m_memorymb=2500.);
  declareProperty("PluginCode", m_plugincode = "");

  declareProperty("VariationWeightCap", m_variation_weight_cap=10.0);
  declareProperty("CrossSectionScaleFactor", m_xsscale=1.0);
  declareProperty("CleanupGeneratedFiles", m_cleanup=true);
}



StatusCode Sherpa_i::genInitialize(){
  if (m_plugincode != "") {
    compilePlugin(m_plugincode);
    m_params.push_back("SHERPA_LDADD=Sherpa_iPlugin");
  }
  
  ATH_MSG_INFO("Sherpa initialising...");

  p_rndEngine = atRndmGenSvc().GetEngine("SHERPA");
  const long* sip = p_rndEngine->getSeeds();
  long int si1 = sip[0];
  long int si2 = sip[1];
  atRndmGenSvc().CreateStream(si1, si2, "SHERPA");

  p_sherpa = new SHERPA::Sherpa();


  /***
      translate ATOOLS:SignalHandler
  ***/
  std::set_terminate(ATOOLS::Terminate);
  std::set_unexpected(ATOOLS::Terminate);
  signal(SIGSEGV,ATOOLS::SignalHandler);
  signal(SIGINT,ATOOLS::SignalHandler);
  signal(SIGBUS,ATOOLS::SignalHandler);
  signal(SIGFPE,ATOOLS::SignalHandler);
  signal(SIGABRT,ATOOLS::SignalHandler);
  signal(SIGTERM,ATOOLS::SignalHandler);
  signal(SIGXCPU,ATOOLS::SignalHandler);

  try {
    int argc;
    char** argv;
    getParameters(argc, argv);
    p_sherpa->InitializeTheRun(argc,(char **)argv);
    delete [] argv;

    p_sherpa->InitializeTheEventHandler();
  }
  catch (const ATOOLS::Exception& exception) {
    if (exception.Class()=="Matrix_Element_Handler" && exception.Type()==ATOOLS::ex::normal_exit) {
      ATH_MSG_ERROR("Have to compile Amegic libraries");
      ATH_MSG_ERROR("You probably want to run ./makelibs");
    }
    else {
      ATH_MSG_ERROR("Unwanted ATOOLS::exception caught.");
      ATH_MSG_ERROR(exception);
    }
    return StatusCode::FAILURE;
  }
  catch (const std::exception& exception) {
    ATH_MSG_ERROR("std::exception caught.");
    return StatusCode::FAILURE;
  }

#ifdef HEPMC3
  m_runinfo = std::make_shared<HepMC3::GenRunInfo>();
  /// Here one can fill extra information, e.g. the used tools in a format generator name, version string, comment.
  struct HepMC3::GenRunInfo::ToolInfo generator={std::string("SHERPA"), std::string(SHERPA_VERSION)+ "." + std::string(SHERPA_SUBVERSION), std::string("Used generator")};
  m_runinfo->tools().push_back(generator);  
#endif
  return StatusCode::SUCCESS;
}


StatusCode Sherpa_i::callGenerator() {
  ATH_MSG_DEBUG("Sherpa_i in callGenerator()");

  do {
    ATH_MSG_DEBUG("Trying to generate event with Sherpa");
  } while (p_sherpa->GenerateOneEvent()==false);
  
  if (ATOOLS::rpa->gen.NumberOfGeneratedEvents()%1000==0) {
    ATH_MSG_INFO("Passed "<<ATOOLS::rpa->gen.NumberOfGeneratedEvents()<<" events.");
  }

  return StatusCode::SUCCESS;
}

StatusCode Sherpa_i::fillEvt(HepMC::GenEvent* event) {
  ATH_MSG_DEBUG( "Sherpa_i Filling event");
#ifdef HEPMC3
  if (!event->run_info()) event->set_run_info(m_runinfo);
#endif
  p_sherpa->FillHepMCEvent(*event);

  if (event->weights().size()>2) {
    //double weight_normalisation = event->weights()[2];
    for (size_t i=0; i<event->weights().size(); ++i) {
      //if (i==0 || i>3) event->weights()[i] /= weight_normalisation;
      if (i>3) { // cap variation weights
        // cap variation weights at m_variation_weight_cap*nominal to avoid spikes from numerical instability
        if (fabs(event->weights()[i]) > m_variation_weight_cap*fabs(event->weights()[0])) {
          ATH_MSG_INFO("Capping variation" << i << " = " << event->weights()[i]/event->weights()[0] << "*nominal");
          event->weights()[i] *= m_variation_weight_cap*std::abs(event->weights()[0])/std::abs(event->weights()[i]);
        }
      }
      ATH_MSG_DEBUG("Sherpa WEIGHT " << i << " value="<< event->weights()[i]);
    }
  }

#ifdef HEPMC3
// units correction
      event->set_units(HepMC3::Units::GEV, HepMC3::Units::MM);
  GeVToMeV(event);
//uncomment to list HepMC3 events
//    std::cout << " print::listing Sherpa " << std::endl;
//    HepMC3::Print::listing(std::cout, *event);
#else
  GeVToMeV(event); //unit check
//    std::cout << " print::printing Sherpa " << std::endl;
//    event->print();
#endif
  
  return StatusCode::SUCCESS;
}

StatusCode Sherpa_i::genFinalize() {

  ATH_MSG_INFO("Sherpa_i finalize()");

  std::cout << "MetaData: generator = Sherpa" << SHERPA_VERSION << "." << SHERPA_SUBVERSION << std::endl;
  std::cout << "MetaData: cross-section (nb)= " << p_sherpa->TotalXS()/1000.0 << std::endl;
  if (m_xsscale!=1.0) {
    std::cout << "MetaData: cross-section*CrossSectionScaleFactor (nb)= " 
              << p_sherpa->TotalXS()/1000.0*m_xsscale << std::endl;
    std::cout << "MetaData: CrossSectionScaleFactor = " << m_xsscale << std::endl;
  }

  std::cout << "MetaData: PDF = " << p_sherpa->PDFInfo() << std::endl;

  std::cout << "Named variations initialised by Sherpa:" << std::endl;
  std::cout << *p_sherpa->GetInitHandler()->GetVariations() << std::endl;

  p_sherpa->SummarizeRun();
  delete p_sherpa;

  if (m_cleanup) {
    ATH_MSG_INFO("Deleting left-over files from working directory.");
    system("rm -rf Results.db Process MIG_*.db MPI_*.dat libSherpa*.so libProc*.so");
  }
  
  return StatusCode::SUCCESS;
}


void Sherpa_i::getParameters(int &argc, char** &argv) {
  std::vector<std::string> params;

  // set some ATLAS specific default values as a starting point
  params.push_back("EXTERNAL_RNG=Atlas_RNG");

  /***
      Adopt Atlas Debug Level Scheme
  ***/

  std::string verbose_arg;
  MsgStream log(msgSvc(), name());
  if( log.level()==MSG::FATAL || log.level()==MSG::ERROR || log.level()==MSG::WARNING ){
    params.push_back("OUTPUT=0");
  }
  else if(log.level()==MSG::INFO){
    params.push_back("OUTPUT=2");
  }
  else if(log.level()==MSG::DEBUG){
    params.push_back("OUTPUT=3");
  }
  else{
    params.push_back("OUTPUT=15");
  }

  // disregard manual RUNDATA setting if run card given in JO
  if (m_runcard != "") m_params.push_back("RUNDATA=Run.dat");
  
  // allow to overwrite all parameters from JO file
  params.insert(params.begin()+params.size(), m_params.begin(), m_params.end());

  // create Run.dat file if runcard explicitely given
  if (m_runcard != "") {
    FILE *file = fopen("Run.dat","w");
    fputs(m_runcard.c_str(),file);
    fclose(file);
  }

  /***
      Convert into Sherpas argc/argv arguments
  ***/
  argc = 1+params.size();
  argv = new char * [ 1+params.size() ];
  argv[0] = new char[7];
  strcpy(argv[0], "Sherpa");

  ATH_MSG_INFO("Sherpa_i using the following Arguments");
  ATH_MSG_INFO(m_runcard);
  for(size_t i=0; i<params.size(); i++) {
    ATH_MSG_INFO(" [ " << params[i] << " ] ");
    argv[i+1] = new char[params[i].size()+1];
    strcpy(argv[i+1], params[i].c_str());
  }
  ATH_MSG_INFO("End Sherpa_i Argument List");
  ATH_MSG_INFO("Further Sherpa initialisation output will be redirected to the LOG_FILE specified above.");

}

void Sherpa_i::compilePlugin(std::string pluginCode) {
  // TODO: not very pretty, should we eventually do this in Python instead (base fragment)
  FILE *file = fopen("Sherpa_iPlugin.C","w");
  fputs(pluginCode.c_str(),file);
  fclose(file);
  std::string command;
  // Python -> C++ string conversion seems to add quote character as first
  // and last line if the string contains quotes (like always in a plugin)
  // thus removing them here
  command += "tail -n +2 Sherpa_iPlugin.C | head -n -1 > Sherpa_iPlugin.C.tmp; mv Sherpa_iPlugin.C.tmp Sherpa_iPlugin.C; ";
  command += "g++ -shared -std=c++0x -g ";
  command += "-I`Sherpa-config --incdir` ";
  command += "`Sherpa-config --ldflags` ";
  command += "-fPIC -o libSherpa_iPlugin.so Sherpa_iPlugin.C";
  ATH_MSG_INFO("Now compiling plugin library using: "+command);
  if (system(command.c_str())!=0) {
    ATH_MSG_ERROR("Error compiling plugin library.");
  }
}

/**
   Use ATLAS random number generator for Sherpa's internal random numbers
**/
using namespace ATOOLS;

Atlas_RNG::Atlas_RNG(CLHEP::HepRandomEngine* engine) :
  External_RNG(), p_engine(engine)
{
}

double Atlas_RNG::Get(){

  return CLHEP::RandFlat::shoot(p_engine);

}

// some getter magic to make this random number generator available to sherpa
DECLARE_GETTER(Atlas_RNG,"Atlas_RNG",External_RNG,RNG_Key);

External_RNG *ATOOLS::Getter<External_RNG,RNG_Key,Atlas_RNG>::operator()(const RNG_Key &) const
{ return new Atlas_RNG(p_rndEngine); }

void ATOOLS::Getter<External_RNG,RNG_Key,Atlas_RNG>::PrintInfo(std::ostream &str,const size_t) const
{ str<<"Atlas RNG interface"; }
