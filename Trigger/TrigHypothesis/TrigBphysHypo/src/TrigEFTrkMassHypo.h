/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGEFTRKMASSHYPO_H
#define TRIGEFTRKMASSHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigT1Interfaces/RecMuonRoI.h" 
#include "GaudiKernel/IIncidentListener.h"

#include "TrigBphysHypo/Constants.h"

class TriggerElement;

class TrigEFTrkMassHypo: public HLT::HypoAlgo
{

public:
    TrigEFTrkMassHypo(const std::string& name, ISvcLocator* pSvcLocator);
    ~TrigEFTrkMassHypo();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
	
    bool execHLTAlgorithm(TriggerElement* );

private:
    
    BooleanProperty m_acceptAll;

    int m_lastEvent;
    int m_lastEventPassed;
    unsigned int m_countTotalEvents;
    unsigned int m_countTotalRoI;
    unsigned int m_countPassedEvents;
    unsigned int m_countPassedRoIs;
//     StoreGateSvc * m_pStoreGate;
    void handle(const Incident &); 

    /* monitored variables */
    int mon_cutCounter;
    int mon_NBphys;
    //std::vector<float> mon_Mass;
    //std::vector<float> mon_FitMass;
    //std::vector<float> mon_Chi2;
    //std::vector<float> mon_Chi2Prob;
    
};

#endif 
