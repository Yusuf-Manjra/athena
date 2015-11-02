/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGL2LOOSEMUONHYPO_H
#define TRIGL2LOOSEMUONHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "GaudiKernel/IIncidentListener.h"

#include "TrigBphysHypo/Constants.h"

class TriggerElement;

class TrigL2LooseMuonHypo: public HLT::HypoAlgo
{

public:
    TrigL2LooseMuonHypo(const std::string& name, ISvcLocator* pSvcLocator);
    ~TrigL2LooseMuonHypo();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

    bool execHLTAlgorithm(TriggerElement* );

private:

    BooleanProperty m_acceptAll;

    StoreGateSvc * m_pStoreGate;
    void handle(const Incident &);

};

#endif
