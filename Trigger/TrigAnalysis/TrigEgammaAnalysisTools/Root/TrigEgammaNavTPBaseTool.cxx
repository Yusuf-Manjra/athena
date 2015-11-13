/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaNavTPBaseTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Contributors:
 * Date: Feb 2015
 * Description:
 *      Trigger e/gamma Zee Tag&Probe Base tool class. Inherits from TrigEgammaAnalysisBaseTool.
 *      Provides methods for selecting T&P pairs, 
 *      matching probes to objects in TE containers.
 *      Creates a vector of pairs with 
 *      offline electrons probes and the last TE with a match.
 *      Relies heavily on TrigNavigation, since we use the TriggerElement.
 *      All derived classes work with list of probes for a given trigger.
 *      As input property, pass a list of triggers to study.
 **********************************************************************/
#include "egammaMVACalib/IegammaMVATool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavTPBaseTool.h"
#include "TrigConfxAOD/xAODConfigTool.h"
#include "xAODEventInfo/EventInfo.h"

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
TrigEgammaNavTPBaseTool::
TrigEgammaNavTPBaseTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname)
{
  declareProperty("MVACalibTool", m_MVACalibTool);
  declareProperty("ApplyMVACalib", m_applyMVACalib=false);
  declareProperty("ElectronIsEMSelector", m_electronIsEMTool);
  declareProperty("ElectronLikelihoodTool", m_electronLHTool);
  declareProperty("TrigDecisionTool", m_trigdec, "iTool to access the trigger decision");
  declareProperty("ZeeLowerMass",m_ZeeMassMin=80);
  declareProperty("ZeeUpperMass",m_ZeeMassMax=100);
  declareProperty("OfflineTagSelector",m_offTagTightness="Tight");
  declareProperty("OfflineProbeSelector",m_defaultProbeTightness="Loose"); // new
  declareProperty("ForceProbePid", m_forceProbePid=false); // new
  declareProperty("OppositeCharge",m_oppositeCharge=true);
  declareProperty("OfflineTagMinEt",m_tagMinEt=25);
  declareProperty("OfflineProbeMinEt",m_probeMinEt=24); // new
  declareProperty("TagTriggerList", m_tagTrigList);
  declareProperty("TriggerList",m_trigInputList);
  declareProperty("CategoryList",m_categories);
  declareProperty("RemoveCrack", m_rmCrack=true); //new
  declareProperty("OfflineProbeIsolation", m_offProbeIsolation="Loose");
  declareProperty("ForceProbeIsolation", m_forceProbeIsolation=false);

  m_PidToolMap["Tight"]=0;
  m_PidToolMap["Medium"]=1;
  m_PidToolMap["Loose"]=2;

  m_offElectrons=nullptr;
  m_jets=nullptr;
  m_eventInfo=nullptr;
  m_applyJetNearProbeSelection=true; 
  // Maps should be static
  // Make a wrapper function to set map and return value
    
}

//**********************************************************************

StatusCode TrigEgammaNavTPBaseTool::childExecute() {

   ATH_MSG_VERBOSE( "child Execute tool " << name() );

   return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavTPBaseTool::childFinalize() {

   ATH_MSG_VERBOSE( "child Finalize tool " << name() );
   m_offElectrons->clearDecorations();

   return StatusCode::SUCCESS;
}
//**********************************************************************
StatusCode
TrigEgammaNavTPBaseTool::childInitialize() {
    ATH_MSG_VERBOSE( "child Initialize tool " << name() );
    if ( (m_trigdec.retrieve()).isFailure() ){
        ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
        return StatusCode::FAILURE;
    }
    if ( (m_electronIsEMTool.retrieve()).isFailure() ){
        ATH_MSG_ERROR( "Could not retrieve Selector Tool! Can't work");
        return StatusCode::FAILURE;
    }
    if ( (m_electronLHTool.retrieve()).isFailure() ){
        ATH_MSG_ERROR( "Could not retrieve Selector Tool! Can't work");
        return StatusCode::FAILURE;
    }
    if( (m_MVACalibTool.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Failed to retrieve " << m_MVACalibTool);
        m_applyMVACalib = false;
        m_MVACalibTool = ToolHandle<IegammaMVATool>();
        return StatusCode::FAILURE; 
    }
    else {
        ATH_MSG_DEBUG("Retrieved tool " << m_MVACalibTool);   
    }
    m_offProbeTightness = m_defaultProbeTightness;

    return StatusCode::SUCCESS;
}
StatusCode TrigEgammaNavTPBaseTool::childBook() {
    return StatusCode::SUCCESS;
}
bool TrigEgammaNavTPBaseTool::EventWiseSelection(){

    ATH_MSG_DEBUG("Apply EventWise selection");

    m_eventInfo=0;
    m_offElectrons = 0;
    m_jets = 0;
    if ( (m_storeGate->retrieve(m_eventInfo, "EventInfo")).isFailure() ){
        ATH_MSG_WARNING("Failed to retrieve eventInfo ");
        return false;
    }
    if (m_eventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error) {
        ATH_MSG_DEBUG("Event not passing LAr");
        return false;
    }
    hist1("CutCounter")->Fill("LAr",1);

    
    if ( (m_storeGate->retrieve(m_offElectrons,m_offElContKey)).isFailure() ){
        ATH_MSG_WARNING("Failed to retrieve offline Electrons ");
	return false;
    }
    hist1("CutCounter")->Fill("RetrieveElectrons",1);
    // Check Size of Electron Container
    if ( m_offElectrons->size() < 2 ) { // Not enough events for T&P
	ATH_MSG_DEBUG("Not enough Electrons for T&P");
	return false;
    }
    hist1("CutCounter")->Fill("TwoElectrons",1);
    // get jet container of interest

    m_applyJetNearProbeSelection=true;
    StatusCode sc=m_storeGate->retrieve(m_jets,"AntiKt4EMTopoJets" );
    if (sc.isFailure()) {
        ATH_MSG_DEBUG("Failed to retrieve AntiKt4EMTopoJets, not applying jet-electron selection");
        m_applyJetNearProbeSelection=false;
    }

    ATH_MSG_DEBUG("Total container size  " << m_offElectrons->size());

    // missing more selections
    // check Minimal Trigger Requirements
    if ( !MinimalTriggerRequirement() ) return false;
    hist1("CutCounter")->Fill("PassTrigger",1);
    return true;
}

bool TrigEgammaNavTPBaseTool::MinimalTriggerRequirement(){
    ATH_MSG_DEBUG("Apply Minimal trigger requirements");
    
    for(unsigned int ilist = 0; ilist != m_tagTrigList.size(); ilist++) {
        std::string tag = m_tagTrigList.at(ilist);
        if ( m_trigdec->isPassed("HLT_"+tag) )
            return true;
    }
    return false; // nothing passed
}


void TrigEgammaNavTPBaseTool::executeTandP(){
    
    clearProbeList(); // Clear Probes after each trigger
    ATH_MSG_DEBUG("Execute TandP BaseTool " << m_offElectrons->size());
    for(const auto& elTag : *m_offElectrons){
        if( ! isTagElectron(elTag) ) continue;
        for(const auto& elProbe : *m_offElectrons){  // Dress the probes with updated Pid decision
            hist1("ProbeCutCounter")->Fill("Electrons",1);
            if(elProbe==elTag) continue;
            hist1("ProbeCutCounter")->Fill("NotTag",1);
            // Check opposite charge
            if(m_oppositeCharge && (elProbe->charge() == elTag->charge()) ) continue;
            hist1("ProbeCutCounter")->Fill("OS",1);
            if(!m_oppositeCharge && (elProbe->charge() != elTag->charge()) ) continue;
            hist1("ProbeCutCounter")->Fill("SS",1);
            ATH_MSG_DEBUG("Execute TandP BaseTool OS"); 
            if(m_doJpsiee){

              float  Jpsieelifetime = GetPseudoLifetime(elTag,elProbe);

              if(  dR(elTag->caloCluster()->eta(),elTag->caloCluster()->phi(),
                      elProbe->caloCluster()->eta(),elProbe->caloCluster()->phi()) <= 0.2 ){
                ATH_MSG_DEBUG("dR(elTag,elProbe)<0.2");
                continue;

              }
              else if(Jpsieelifetime<-1 || 0.2<Jpsieelifetime){
                ATH_MSG_DEBUG("tag and probe pair not in Jpsi lifetime window");
                continue;
              }

            }
            //Must be an easy way with IParticle
            TLorentzVector el1;
            TLorentzVector el2;
            el1.SetPtEtaPhiE(elTag->pt(), elTag->trackParticle()->eta(), elTag->trackParticle()->phi(), elTag->e());
            el2.SetPtEtaPhiE(elProbe->pt(), elProbe->trackParticle()->eta(), elProbe->trackParticle()->phi(), elProbe->e());
            float tpPairMass = (el1 + el2).M();
            if( !((tpPairMass > m_ZeeMassMin*1.e3) && (tpPairMass < m_ZeeMassMax*1.e3))){
                ATH_MSG_DEBUG("tag and probe pair not in Z mass window");
                continue;
            } else {
                hist1("ProbeCutCounter")->Fill("ZMass",1);
                ATH_MSG_DEBUG("tag and probe pair in Z mass window");
                // Probe available. Good Probe?
                if(!isGoodProbeElectron(elProbe)) continue;//,probeTrigItem,etthr,pidname)) continue; //Ensure passing offline electron selection
                hist1("ProbeCutCounter")->Fill("GoodProbe",1);
                // Shallow copy of pointer to avoid decoration of original object
                m_probeElectrons.push_back(const_cast<xAOD::Electron*>(elProbe));
                hist1("Mee")->Fill(tpPairMass/1.e3);
            }
        } // end of for in Probe
    } // end of for in Tags
    ATH_MSG_DEBUG("Dress N Probes " << m_probeElectrons.size());
    for(unsigned int i=0;i<m_probeElectrons.size();i++) DressPid(m_probeElectrons[i]);
}

void TrigEgammaNavTPBaseTool::matchObjects(const std::string probeTrigItem){

    clearPairList();
    for(unsigned int i=0;i<m_probeElectrons.size();i++){
        const HLT::TriggerElement *finalFC;

        // Use matching tool and create pair of offline probe and TE
        if ( m_matchTool->match(m_probeElectrons[i], probeTrigItem, finalFC)){
            std::pair<const xAOD::Electron*,const HLT::TriggerElement*> pairProbe(m_probeElectrons[i],finalFC);
            m_pairObj.push_back(pairProbe);
        } // end of check Probe
        else {
            std::pair<const xAOD::Electron*,const HLT::TriggerElement*> pairProbe(m_probeElectrons[i],NULL);
            m_pairObj.push_back(pairProbe);
        } // still include the probe
    }
}

void TrigEgammaNavTPBaseTool::clearProbeList(){
    m_probeElectrons.clear();
    m_probePhotons.clear();
}

void TrigEgammaNavTPBaseTool::clearPairList(){
    m_pairObj.clear();
}

void TrigEgammaNavTPBaseTool::clearDecorations(){
    if(m_pairObj.size()!=0) m_offElectrons->clearDecorations();
}

bool TrigEgammaNavTPBaseTool::isTagElectron(const xAOD::Electron *el){

    hist1("TagCutCounter")->Fill("Electrons",1);
    // Tag the event
    // Require offline tight electron
    // Match to e24_tight1_L1EM20V
    ATH_MSG_DEBUG("Selecting Tag Electron");
    double GeV = 1000.;

    //Check constituents
    const xAOD::TrackParticle *trk = el->trackParticle();
    if(!el->trackParticle()){
        ATH_MSG_DEBUG("No track Particle");
        return false;
    }
    hist1("TagCutCounter")->Fill("HasTrack",1);
    ATH_MSG_DEBUG("Track pt " << trk->pt());
    const xAOD::CaloCluster *clus = el->caloCluster();
    if(!el->caloCluster()){
        ATH_MSG_DEBUG("No caloCluster");
        return false;
    }
    hist1("TagCutCounter")->Fill("HasCluster",1);

    ATH_MSG_DEBUG("Cluster E "<<clus->e());
    ATH_MSG_DEBUG("Selecting Tag Electron PID");
    if (!ApplyElectronPid(el, m_offTagTightness)) return false;
    hist1("TagCutCounter")->Fill("GoodPid",1);
    ATH_MSG_DEBUG("Selecting Tag Electron Et");
    //Require Et > 25 GeV
    if( !(el->e()/cosh(el->trackParticle()->eta())  > m_tagMinEt*GeV) ){
        return false;
    }
    hist1("TagCutCounter")->Fill("Et",1);
    ATH_MSG_DEBUG("Selecting Tag Electron Eta");
    //fiducial detector acceptance region
    float absEta = fabs(el->caloCluster()->etaBE(2));
    if ((absEta > 1.37 && absEta < 1.52) || absEta > 2.47) {
        return false;
    }
    hist1("TagCutCounter")->Fill("Eta",1);

    ATH_MSG_DEBUG("Checking electron object quality");
    if (!el->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) return false;
    hist1("TagCutCounter")->Fill("IsGoodOQ",1);

    ATH_MSG_DEBUG("Selecting Tag Electron Decision");
    // Check matching to a given trigger
    // The statement below is more general
    bool tagPassed=false;
    for(unsigned int ilist = 0; ilist != m_tagTrigList.size(); ilist++) {
      std::string tag = m_tagTrigList.at(ilist);
      if(m_trigdec->isPassed("HLT_"+tag)){ 
        if(m_doJpsiee){
          std::string p1trigger;
          std::string p2trigger;
          if(splitTriggerName(tag,p1trigger,p2trigger)){
            if(fabs(p1trigger.find("tight"))<10) tag=p1trigger;
            if(fabs(p2trigger.find("tight"))<10) tag=p2trigger;
          }
          if( m_matchTool->match(el,"HLT_"+tag) )
            tagPassed=true;
        }
        else{
          tagPassed=true; 
        }
      }
    }
    if(!tagPassed) {
        ATH_MSG_DEBUG("Failed tag trigger "); 
        return false;
    }
    hist1("TagCutCounter")->Fill("PassTrigger",1);
    ATH_MSG_DEBUG("Matching Tag Electron FC");
    bool tagMatched=false;
    for(unsigned int ilist = 0; ilist != m_tagTrigList.size(); ilist++) {
        std::string tag = m_tagTrigList.at(ilist);
        if( m_matchTool->match(el,"HLT_"+tag) )
                tagMatched=true;
    }
    if(!tagMatched){
        ATH_MSG_DEBUG("Failed a match ");
        return false; // otherwise, someone matched!
    }
    hist1("TagCutCounter")->Fill("MatchTrigger",1);
    ATH_MSG_DEBUG("Found a tag electron");
    return true;
}


void TrigEgammaNavTPBaseTool::DressPid(const xAOD::Electron *eg){
    for(int ipid=0;ipid<3;ipid++){
        const Root::TAccept& accept=m_electronIsEMTool[ipid]->accept(eg);
        const std::string pidname="ElectronPass"+m_isemname[ipid];
        eg->auxdecor<bool>(pidname)=static_cast<bool>(accept);
    }
    for(int ipid=0;ipid<3;ipid++){
        const Root::TAccept& accept=m_electronLHTool[ipid]->accept(eg);
        const std::string pidname="ElectronPass"+m_lhname[ipid];
        eg->auxdecor<bool>(pidname)=static_cast<bool>(accept);
    }
    eg->auxdecor<bool>("Isolated")=isIsolated(eg, m_offProbeIsolation);
}

bool TrigEgammaNavTPBaseTool::ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname){
    
    if (pidname == "Tight"){
        const Root::TAccept& accept=m_electronIsEMTool[0]->accept(eg);
        return static_cast<bool>(accept);
    }
    else if (pidname == "Medium"){
        const Root::TAccept& accept=m_electronIsEMTool[1]->accept(eg);
        return static_cast<bool>(accept);
    }
    else if (pidname == "Loose"){
        const Root::TAccept& accept=m_electronIsEMTool[2]->accept(eg);
        return static_cast<bool>(accept);
    }
    else if (pidname == "LHTight"){
        const Root::TAccept& accept=m_electronLHTool[0]->accept(eg);
        return static_cast<bool>(accept);
    }
    else if (pidname == "LHMedium"){
        const Root::TAccept& accept=m_electronLHTool[1]->accept(eg);
        return static_cast<bool>(accept);
    }
    else if (pidname == "LHLoose"){
        const Root::TAccept& accept=m_electronLHTool[2]->accept(eg);
        return static_cast<bool>(accept);
    }
    else ATH_MSG_DEBUG("No Pid tool, continue without PID");
    return false;
}

bool TrigEgammaNavTPBaseTool::isGoodProbeElectron(const xAOD::Electron *el){

    double GeV = 1000.;
    //Check constituents
    if(!el->trackParticle()){
        ATH_MSG_DEBUG("No track Particle");
        return false;
    }
    hist1("ProbeCutCounter")->Fill("HasTrack",1);
    if(!el->caloCluster()){
        ATH_MSG_DEBUG("No caloCluster");
        return false;
    }
    hist1("ProbeCutCounter")->Fill("HasCluster",1);
    //fiducial detector acceptance region
    if(m_rmCrack){
        float absEta = fabs(el->caloCluster()->etaBE(2));
        if ((absEta > 1.37 && absEta < 1.52) || absEta > 2.47) {
            return false; 
        }
    }
    hist1("ProbeCutCounter")->Fill("Eta",1);
    /*if( !(el->e()/cosh(el->trackParticle()->eta())  > (etthr-5.0)*GeV) ){
        return false;
    }
    hist1("ProbeCutCounter")->Fill("Et",1);*/

    ATH_MSG_DEBUG("Checking electron object quality");
    if (!el->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) return false;
    hist1("ProbeCutCounter")->Fill("IsGoodOQ",1);
    //DressPid(el);
    hist1("ProbeCutCounter")->Fill("GoodPid",1);
    if(m_applyJetNearProbeSelection){
        TLorentzVector probeCandidate;
        probeCandidate.SetPtEtaPhiE(el->pt(), el->trackParticle()->eta(), el->trackParticle()->phi(), el->e());
        Int_t jetsAroundProbeElectron = 0; 
        for(const auto &i_jet : *m_jets){
            TLorentzVector jet;
            jet.SetPtEtaPhiE(i_jet->pt(), i_jet->eta(), i_jet->phi(), i_jet->e());
            if( (jet.Et() > 20*GeV) && (jet.DeltaR(probeCandidate) < 0.4)) jetsAroundProbeElectron++;
        }
        //reject if more than 1 jet close to the probe electron
        if ( jetsAroundProbeElectron >= 2 ) {
            //ATH_MSG_DEBUG("too many jets around object");
            return false; 
        }
    }
    hist1("ProbeCutCounter")->Fill("NearbyJet",1);
    return true; // Good probe electron
}

bool TrigEgammaNavTPBaseTool::passedTrigger(const HLT::TriggerElement* obj){
    bool passed=false;
    if ( obj->getActiveState() ) passed = true;
    return passed;
}
float TrigEgammaNavTPBaseTool::GetPseudoLifetime(const xAOD::Electron *el1,const xAOD::Electron *el2){

  TLorentzVector el1track;
  TLorentzVector el2track;

  float Et1=hypot(el1->caloCluster()->m(),el1->caloCluster()->pt())/cosh(el1->trackParticle()->eta());
  float Et2=hypot(el2->caloCluster()->m(),el2->caloCluster()->pt())/cosh(el1->trackParticle()->eta());

  el1track.SetPtEtaPhiM(Et1, el1->trackParticle()->eta(), el1->trackParticle()->phi(),0.511);
  el2track.SetPtEtaPhiM(Et2, el2->trackParticle()->eta(), el2->trackParticle()->phi(), 0.511);

  float lxy=simple_lxy(0,
                       el1->trackParticle()->d0() , el2->trackParticle()->d0(),
                       el1->trackParticle()->phi(),   el2->trackParticle()->phi(),
                       Et1,              Et2,
                       0.0,                  0.0);

  float  m_ptEECalo  = (el1track+el2track).Pt();

  return lxy*3096.916/(0.299792458*m_ptEECalo);

}
double TrigEgammaNavTPBaseTool::simple_lxy(int flag,   double d1,  double d2, double phi1, double phi2, double pt1, double pt2, double vx, double vy)
{
  double simple = -99999.;

  //require minimum opening angle of 1 microradian.                                                                                                                                                                 
  if(fabs(phi1 - phi2) < 1e-6) return simple;

  double simpleXv = (-d2*cos(phi1) + d1*cos(phi2)) / sin(phi2-phi1);
  double simpleYv = (-d2*sin(phi1) + d1*sin(phi2)) / sin(phi2-phi1);

  double rxy  = sqrt((simpleXv-vx)*(simpleXv-vx) +
                     (simpleYv-vy)*(simpleYv-vy) );

  double f1 = (fabs(pt1)*cos(phi1)+fabs(pt2)*cos(phi2));
  double f2 = (fabs(pt1)*sin(phi1)+fabs(pt2)*sin(phi2));
  double  c = sqrt( f1*f1 + f2*f2 );

  if ( c == 0 ) return simple;

  double a =  (simpleXv-vx)*f1;
  double b =  (simpleYv-vy)*f2;

  if (flag == 1)
    return rxy;
  else
    return (a+b)/c;
}
