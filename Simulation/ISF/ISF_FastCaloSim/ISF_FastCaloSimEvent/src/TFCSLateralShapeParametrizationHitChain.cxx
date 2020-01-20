/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSLateralShapeParametrizationHitChain.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

//=============================================
//======= TFCSLateralShapeParametrizationHitChain =========
//=============================================

TFCSLateralShapeParametrizationHitChain::TFCSLateralShapeParametrizationHitChain(const char* name, const char* title):TFCSLateralShapeParametrization(name,title),m_number_of_hits_simul(nullptr)
{
}

TFCSLateralShapeParametrizationHitChain::TFCSLateralShapeParametrizationHitChain(TFCSLateralShapeParametrizationHitBase* hitsim):TFCSLateralShapeParametrization(TString("hit_chain_")+hitsim->GetName(),TString("hit chain for ")+hitsim->GetTitle()),m_number_of_hits_simul(nullptr)
{
  set_pdgid_Ekin_eta_Ekin_bin_calosample(*hitsim);
  
  m_chain.push_back(hitsim);
}

void TFCSLateralShapeParametrizationHitChain::set_geometry(ICaloGeometry* geo)
{
  TFCSLateralShapeParametrization::set_geometry(geo);
  if(m_number_of_hits_simul) m_number_of_hits_simul->set_geometry(geo);
}

int TFCSLateralShapeParametrizationHitChain::get_number_of_hits(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  // TODO: should we still do it?
  if(m_number_of_hits_simul) {
    int n=m_number_of_hits_simul->get_number_of_hits(simulstate,truth,extrapol);
    if(n<1) n=1;
    return n;
  }
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    int n=hitsim->get_number_of_hits(simulstate,truth,extrapol);
    if(n>0) return n;
  } 
  return 1;
}

float TFCSLateralShapeParametrizationHitChain::getMinWeight() const
{
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    float weight=hitsim->getMinWeight();
    if(weight>0.) return weight;
  } 
  return -1.;
}
  
float TFCSLateralShapeParametrizationHitChain::getMaxWeight() const
{
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    float weight=hitsim->getMaxWeight();
    if(weight>0.) return weight;
  }
  return -1.;
}


float TFCSLateralShapeParametrizationHitChain::get_E_hit(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{  
  const int nhits = get_number_of_hits(simulstate,truth,extrapol);
  const int sample = calosample();
  if(nhits<=0 || sample<0) return -1.;
  const float maxWeight = getMaxWeight();// maxWeight = -1 if  shapeWeight class is not in m_chain  
  
  if(maxWeight>0) return simulstate.E(sample)/(maxWeight*nhits); // maxWeight is used only if shapeWeight class is in m_chain
  else return simulstate.E(sample)/nhits; // Otherwise, old definition of E_hit is used
}

float TFCSLateralShapeParametrizationHitChain::get_sigma2_fluctuation(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  if(m_number_of_hits_simul) {
    double sigma2=m_number_of_hits_simul->get_sigma2_fluctuation(simulstate,truth,extrapol);
    if(sigma2>0) return sigma2;
  }
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    double sigma2=hitsim->get_sigma2_fluctuation(simulstate,truth,extrapol);
    if(sigma2>0) return sigma2;
  } 
  //Limit to factor s_max_sigma2_fluctuation fluctuations
  return s_max_sigma2_fluctuation;
}

FCSReturnCode TFCSLateralShapeParametrizationHitChain::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* extrapol) const
{
  // Call get_number_of_hits() only once, as it could contain a random number
  int nhit = get_number_of_hits(simulstate, truth, extrapol);
  if (nhit <= 0) {
    ATH_MSG_ERROR("TFCSLateralShapeParametrizationHitChain::simulate(): number of hits could not be calculated");
    return FCSFatal;
  }

  const float Elayer=simulstate.E(calosample());
  const float Ehit=get_E_hit(simulstate,truth,extrapol);
  float sumEhit=0;

  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) {
    ATH_MSG_DEBUG("E("<<calosample()<<")="<<simulstate.E(calosample())<<" #hits~"<<nhit);
  }

  int ihit=0;
  TFCSLateralShapeParametrizationHitBase::Hit hit;
  hit.reset_center();
  do {
    hit.reset();
    hit.E()=Ehit;
    for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
      if (debug) {
        if (ihit < 2) hitsim->setLevel(MSG::DEBUG);
        else hitsim->setLevel(MSG::INFO);
      }

      for (int i = 0; i <= FCS_RETRY_COUNT; i++) {
        //TODO: potentially change logic in case of a retry to redo the whole hit chain from an empty hit instead of just redoing one step in the hit chain
        if (i > 0) ATH_MSG_WARNING("TFCSLateralShapeParametrizationHitChain::simulate(): Retry simulate_hit call " << i << "/" << FCS_RETRY_COUNT);
  
        FCSReturnCode status = hitsim->simulate_hit(hit, simulstate, truth, extrapol);

        if (status == FCSSuccess) {
          //if(sumEhit+hit.E()>Elayer) hit.E()=Elayer-sumEhit;//sum of all hit energies needs to be Elayer: correct last hit accordingly
          break;
        } else {
          if (status == FCSFatal) return FCSFatal;
        }    

        if (i == FCS_RETRY_COUNT) {
          ATH_MSG_ERROR("TFCSLateralShapeParametrizationHitChain::simulate(): simulate_hit call failed after " << FCS_RETRY_COUNT << "retries");
        }
      }
    }
    sumEhit+=hit.E();
    ++ihit;
    
    if( (ihit==10*nhit) || (ihit==100*nhit) ) {
      ATH_MSG_WARNING("TFCSLateralShapeParametrizationHitChain::simulate(): Iterated " << ihit << " times, expected " << nhit <<" times. Deposited E("<<calosample()<<")="<<sumEhit);
    }                                                                                                                         
    if(ihit>1000*nhit && ihit>1000) {
      ATH_MSG_WARNING("TFCSLateralShapeParametrizationHitChain::simulate(): Aborting hit chain, iterated " << 1000*nhit << " times, expected " << nhit <<" times. Deposited E("<<calosample()<<")="<<sumEhit<<" expected E="<<Elayer);
      break;
    }  
  } while (sumEhit<Elayer);

  return FCSSuccess;
}

void TFCSLateralShapeParametrizationHitChain::Print(Option_t *option) const
{
  TFCSLateralShapeParametrization::Print(option);
  TString opt(option);
  bool shortprint=opt.Index("short")>=0;
  bool longprint=msgLvl(MSG::DEBUG) || (msgLvl(MSG::INFO) && !shortprint);
  TString optprint=opt;optprint.ReplaceAll("short","");

  if(m_number_of_hits_simul) {
    if(longprint) ATH_MSG_INFO(optprint <<"#:Number of hits simulation:");
    m_number_of_hits_simul->Print(opt+"#:");
  }
  if(longprint) ATH_MSG_INFO(optprint <<"- Simulation chain:");
  char count='A';
  for(TFCSLateralShapeParametrizationHitBase* hitsim : m_chain) {
    hitsim->Print(opt+count+" ");
    count++;
  } 
}
