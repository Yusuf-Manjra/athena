#include "TauDiscriminant/TauDiscriBuilder.h"
#include "TauDiscriminant/TauIDVarCalculator.h"
#include "TauDiscriminant/TauJetBDT.h"
#include "TauDiscriminant/TauEleBDT.h"
#include "TauDiscriminant/TauMuonVeto.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TauDiscriBuilder )

DECLARE_TOOL_FACTORY( TauIDVarCalculator )
DECLARE_TOOL_FACTORY( TauJetBDT )
DECLARE_TOOL_FACTORY( TauEleBDT )
DECLARE_TOOL_FACTORY( TauMuonVeto )

DECLARE_FACTORY_ENTRIES(TauDiscriminant)
{
    DECLARE_ALGORITHM( TauDiscriBuilder )

    DECLARE_TOOL( TauIDVarCalculator )
    DECLARE_TOOL( TauJetBDT )
    DECLARE_TOOL( TauEleBDT )
    DECLARE_TOOL( TauMuonVeto )
}
