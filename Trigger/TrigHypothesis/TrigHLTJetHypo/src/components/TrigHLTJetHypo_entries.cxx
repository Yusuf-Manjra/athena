/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "../TrigJetHypoAlgMT.h"
#include "../TrigJetHypoToolConfig_fastreduction.h"
#include "../TrigJetHypoToolConfig_conditionfilter.h"
#include "../TrigJetHypoToolConfig_passthroughfilter.h"
#include "../TrigJetHypoToolConfig_rangefilter.h"
//
#include "../TrigJetConditionConfig_abs_eta.h"
#include "../TrigJetConditionConfig_signed_eta.h"
#include "../TrigJetConditionConfig_phi.h"
#include "../TrigJetConditionConfig_et.h"
#include "../TrigJetConditionConfig_pt.h"
#include "../TrigJetConditionConfig_htfr.h"
#include "../TrigJetConditionConfig_dijet_mass.h"
#include "../TrigJetConditionConfig_dijet_dphi.h"
#include "../TrigJetConditionConfig_dijet_deta.h"
#include "../TrigJetConditionConfig_qjet_mass.h"
#include "../TrigJetConditionConfig_smc.h"
#include "../TrigJetConditionConfig_jvt.h"
#include "../TrigJetConditionConfig_acceptAll.h"
#include "../TrigJetConditionConfig_moment.h"
#include "../TrigJetConditionConfig_repeated.h"

//
#include "../TrigJetHypoToolMT.h"
#include "../TrigJetHypoToolHelperNoGrouper.h"
#include "../TrigJetTLAHypoAlgMT.h"
#include "../TrigJetTLAHypoToolMT.h"

DECLARE_COMPONENT(TrigJetHypoToolConfig_fastreduction)
DECLARE_COMPONENT(TrigJetHypoToolConfig_conditionfilter)
DECLARE_COMPONENT(TrigJetHypoToolConfig_passthroughfilter)
DECLARE_COMPONENT(TrigJetHypoToolConfig_rangefilter)
DECLARE_COMPONENT(TrigJetConditionConfig_phi)
DECLARE_COMPONENT(TrigJetConditionConfig_signed_eta)
DECLARE_COMPONENT(TrigJetConditionConfig_abs_eta)
DECLARE_COMPONENT(TrigJetConditionConfig_et)
DECLARE_COMPONENT(TrigJetConditionConfig_pt)
DECLARE_COMPONENT(TrigJetConditionConfig_htfr)
DECLARE_COMPONENT(TrigJetConditionConfig_dijet_mass)
DECLARE_COMPONENT(TrigJetConditionConfig_dijet_deta)
DECLARE_COMPONENT(TrigJetConditionConfig_dijet_dphi)
DECLARE_COMPONENT(TrigJetConditionConfig_smc)
DECLARE_COMPONENT(TrigJetConditionConfig_jvt)
DECLARE_COMPONENT(TrigJetConditionConfig_acceptAll)
DECLARE_COMPONENT(TrigJetConditionConfig_moment)
DECLARE_COMPONENT(TrigJetConditionConfig_repeated)
DECLARE_COMPONENT(TrigJetConditionConfig_qjet_mass)

DECLARE_COMPONENT(TrigJetHypoAlgMT)
DECLARE_COMPONENT(TrigJetHypoToolMT)
DECLARE_COMPONENT(TrigJetTLAHypoAlgMT)
DECLARE_COMPONENT(TrigJetTLAHypoToolMT)
DECLARE_COMPONENT(TrigJetHypoToolHelperNoGrouper)

