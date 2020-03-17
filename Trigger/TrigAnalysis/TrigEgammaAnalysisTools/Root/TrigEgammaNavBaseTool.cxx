/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaNavZeeTPBaseTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Contributors:
 *      Joao Victor Pinto <joao.victor.da.fonseca.pinto@cern.ch>
 * Date: Feb 2015
 * Description:
 *      Inherits from TrigEgammaAnalysisBaseTool.
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaNavBaseTool.h"
#include "TrigConfxAOD/xAODConfigTool.h"
#include "PATCore/AcceptData.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "string"
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
//**********************************************************************
using namespace Trig;
using namespace TrigConf;
using namespace xAOD;
using namespace boost;
TrigEgammaNavBaseTool::
TrigEgammaNavBaseTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname),
    m_dir(myname)
{
  declareProperty("Analysis",m_anatype="Analysis");
  declareProperty("TriggerList"               , m_trigInputList               );
  declareProperty("PhotonPid"                 , m_photonPid = "Tight"         );
  declareProperty("doUnconverted"             , m_doUnconverted=false         );
  declareProperty("OfflineProbeIsolation"     , m_offProbeIsolation="Loose"   );
  declareProperty("RemoveCrack"               , m_rmCrack=true                ); 
  declareProperty("ForceProbeIsolation"       , m_forceProbeIsolation=false   );
  declareProperty("ForcePidSelection"         , m_forcePidSelection=true      );
  declareProperty("ForceEtThreshold"          , m_forceEtThr=true             ); 
  declareProperty("ForceVetoVeryLoose"        , m_forceVetoVeryLoose=false    ); 

  m_offElectrons=nullptr;
  m_offPhotons=nullptr;
  m_eventInfo=nullptr;
  m_truthContainer=nullptr;
  m_jets=nullptr;
    
}

//**********************************************************************

StatusCode TrigEgammaNavBaseTool::childExecute() {

   ATH_MSG_VERBOSE( "child Execute tool " << name() );

   return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavBaseTool::childFinalize() {

   ATH_MSG_VERBOSE( "child Finalize tool " << name() );
   clearList();

   return StatusCode::SUCCESS;
}

//**********************************************************************
StatusCode
TrigEgammaNavBaseTool::childInitialize() {
    ATH_MSG_VERBOSE( "child Initialize tool " << name() );
    if ( (m_electronIsEMTool.retrieve()).isFailure() ){
        ATH_MSG_ERROR( "Could not retrieve Selector Tool! Can't work");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavBaseTool::childBook() {
    return StatusCode::SUCCESS;
}


bool TrigEgammaNavBaseTool::EventWiseSelection( ){

    // Check Size of Electron Container
    m_offElectrons = 0;
    m_offPhotons = 0;
    m_eventInfo=0;
    m_truthContainer = 0;

    if ( (m_storeGate->retrieve(m_eventInfo, "EventInfo")).isFailure() ){
        ATH_MSG_WARNING("Failed to retrieve eventInfo ");
        return false;
    }
   
    if(m_storeGate->contains<xAOD::TruthParticleContainer>("egammaTruthParticles")){
      if(m_storeGate->retrieve(m_truthContainer,"egammaTruthParticles").isFailure()){
        ATH_MSG_WARNING("Could not retrieve xAOD::TruthParticleContainer 'egammaTruthParticles'");
        return false;
      }
    }// protection

    
    if ( (m_storeGate->retrieve(m_offElectrons,m_offElContKey)).isFailure() ){
        ATH_MSG_WARNING("Failed to retrieve offline Electrons ");
        return false; 
    }
    
    if ( (m_storeGate->retrieve(m_offPhotons,m_offPhContKey)).isFailure() ){
        ATH_MSG_WARNING("Failed to retrieve offline Photons ");
        return false; 
    }


    for(const auto& eg : *m_offElectrons ){
        ATH_MSG_DEBUG("ApplyElectronPid...");
        if(ApplyElectronPid(eg,"Loose")) hist1(m_anatype+"_electrons")->AddBinContent(1);
        if(ApplyElectronPid(eg,"Medium")) hist1(m_anatype+"_electrons")->AddBinContent(2);
        if(ApplyElectronPid(eg,"Tight")) hist1(m_anatype+"_electrons")->AddBinContent(3); 
        if(ApplyElectronPid(eg,"LHLoose")) hist1(m_anatype+"_electrons")->AddBinContent(4);
        if(ApplyElectronPid(eg,"LHMedium")) hist1(m_anatype+"_electrons")->AddBinContent(5);
        if(ApplyElectronPid(eg,"LHTight")) hist1(m_anatype+"_electrons")->AddBinContent(6); 
        // if(ApplyElectronPid(eg,"LHMediumHI")) hist1(m_anatype+"_electrons")->AddBinContent(7); 
    }
   
    //Calculate number of vertex 
    TrigEgammaAnalysisBaseTool::calculatePileupPrimaryVertex();   


    return true; 
}

StatusCode TrigEgammaNavBaseTool::executeNavigation( const TrigInfo info ){

  clearList();
  ATH_MSG_DEBUG("Apply navigation selection");
  
  if(info.trigType == "electron") return executeElectronNavigation( info.trigName,info.trigThrHLT,info.trigPidType );
  else if(info.trigType=="photon") return executePhotonNavigation( info.trigName,info.trigThrHLT);

  ATH_MSG_DEBUG("BaseTool::TEs " << m_objTEList.size() << " found.");
  return StatusCode::SUCCESS;
}

bool TrigEgammaNavBaseTool::ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname){
    try{

	ATH_MSG_DEBUG("Applying Electron PID with pidname =  " << pidname);
	bool accept=false;
	if (pidname == "Tight"){
	    accept = (bool) m_electronIsEMTool[0]->accept(eg);
	}
	else if (pidname == "Medium"){
	    accept = (bool) m_electronIsEMTool[1]->accept(eg);
	    return static_cast<bool>(accept);
	}
	else if (pidname == "Loose"){
	    accept = (bool) m_electronIsEMTool[2]->accept(eg);
	}
	else if (pidname == "LHTight"){
	    accept = (bool) m_electronLHTool[0]->accept(eg);
	}
	else if (pidname == "LHMedium"){
	    accept = (bool) m_electronLHTool[1]->accept(eg);
	}
	else if (pidname == "LHLoose"){
	    accept = (bool) m_electronLHTool[2]->accept(eg);
	}
	// HI lh medium not working in master. Commenting out untill fixed
	// else if (pidname == "LHMediumHI"){
	//     bool accept = (bool) m_electronLHTool[3]->accept(eg);
	//     return static_cast<bool>(accept);
	// }
	else {
	    ATH_MSG_DEBUG("No Pid tool, continue without PID");
	}
	ATH_MSG_DEBUG("ElectronLHTool for " << pidname << " = " <<  (int) accept );
	return accept;
    } catch (...) {
        ATH_MSG_WARNING("Failed to apply pid " << pidname);
	return false;
    }
}


StatusCode TrigEgammaNavBaseTool::executeElectronNavigation( std::string trigItem,float etthr,std::string pidname ){

  clearList(); //Clear Probe list before each execution -- not in derived class
  ATH_MSG_DEBUG("Apply navigation selection "); 



  const std::string decor="is"+pidname;
  for(const auto& eg : *m_offElectrons ){
      const HLT::TriggerElement *te = nullptr;
      if(!eg->trackParticle()){
          ATH_MSG_DEBUG("No track Particle");
          continue;
      }
      if(!eg->caloCluster()){
          ATH_MSG_DEBUG("No caloCluster");
          continue;
      }

      if(m_forceEtThr){///default is true
        if( !( getEt(eg)  > (etthr-5.)*1.e3) ) continue;
      }

      if(m_rmCrack){///default is true
        if ( (fabs(eg->eta())>1.37 && fabs(eg->eta())<1.52) || fabs(eg->eta())>2.47 )  continue; 
      }

      if(m_forcePidSelection){///default is true
        if(!ApplyElectronPid(eg,pidname)){
	        ATH_MSG_DEBUG("Fails ElectronID "<< pidname);
	        continue;
	      }
	      ATH_MSG_DEBUG("Passes ElectronID "<< pidname);
      }

      if (m_forceProbeIsolation) {///default is false
        if (!isIsolated(eg, m_offProbeIsolation))  continue;///default is Loose
      }

      if(m_forceVetoVeryLoose){///default is false
        bool veto = (bool)this->m_electronLHVLooseTool->accept(eg);
        if(veto)  continue;
      }

      xAOD::Electron *el = new xAOD::Electron(*eg);
      el->auxdecor<bool>(decor)=static_cast<bool>(true);

      bool isEmulation = getTrigInfo(trigItem).trigIsEmulation;
      if(isEmulation && getEmulation()){
        emulation()->match(el,te);
        std::pair< const xAOD::Electron*, const HLT::TriggerElement* > pair(el,te);
        m_objTEList.push_back(pair);
      }else{
        if (match()->match(el, trigItem, te)){
           std::pair< const xAOD::Electron*, const HLT::TriggerElement* > pair(el,te);
           m_objTEList.push_back(pair);
        }else {
           std::pair< const xAOD::Electron*, const HLT::TriggerElement* > pair(el,nullptr);
           m_objTEList.push_back(pair);
        }
      }

  }

  ATH_MSG_DEBUG("BaseTool::Electron TEs " << m_objTEList.size() << " found.");
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavBaseTool::executePhotonNavigation( std::string trigItem, float etthr){

  clearList();
  ATH_MSG_DEBUG("Apply navigation selection");
 
  const std::string decor="is"+m_photonPid;

  for(const auto& eg : *m_offPhotons ){
      const HLT::TriggerElement *te = nullptr;
      if(!eg->caloCluster()){
          ATH_MSG_DEBUG("No caloCluster");
          continue;
      } 
      if( !(getCluster_et(eg) > (etthr-5.)*Gaudi::Units::GeV)) continue; //Take 2GeV above threshold
      if(!eg->passSelection(m_photonPid)) continue;
      if(m_doUnconverted){
          if (eg->vertex()){
              ATH_MSG_DEBUG("Removing converted photons, continuing...");
              continue;
          }
      }
      xAOD::Photon *ph = new xAOD::Photon(*eg);
      ph->auxdecor<bool>(decor)=static_cast<bool>(true);
      bool isEmulation = getTrigInfo(trigItem).trigIsEmulation;
      if(isEmulation && getEmulation()){
        emulation()->match(ph,te);
        std::pair< const xAOD::Photon*, const HLT::TriggerElement* > pair(ph,te);
        m_objTEList.push_back(pair);
      }else{
        if (match()->match(ph, trigItem, te)){
           std::pair< const xAOD::Photon*, const HLT::TriggerElement* > pair(ph,te);
           m_objTEList.push_back(pair);
        }else {
            std::pair< const xAOD::Photon*, const HLT::TriggerElement* > pair(ph,nullptr);
            m_objTEList.push_back(pair);
        }
      }

  }

  ATH_MSG_DEBUG("BaseTool::Photon TEs " << m_objTEList.size() << " found.");
  return StatusCode::SUCCESS;
}

void TrigEgammaNavBaseTool::clearList(){
    for( auto paitIt : m_objTEList )
      delete paitIt.first;

    m_objTEList.clear();
}

bool TrigEgammaNavBaseTool::passedTrigger(const HLT::TriggerElement* obj){
    bool passed=false;
    if ( obj->getActiveState() ) passed = true;
    return passed;
}

