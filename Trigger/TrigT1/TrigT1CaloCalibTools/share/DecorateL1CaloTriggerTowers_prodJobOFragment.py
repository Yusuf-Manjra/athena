# This jobO should not be included more than once:
include.block( "TrigT1CaloCalibTools/DecorateL1CaloTriggerTowers_prodJobOFragment.py" )

# This should not be included in any L1Calo jobOptions files
# This is included in Calorimeter/CaloRec/share/CaloRec_jobOptions.py
# This way it's run whenever the CaloRec is run

include("TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py")

from TrigT1CaloCalibTools.TrigT1CaloCalibToolsConf import LVL1__L1CaloxAODOfflineTriggerTowerTools
ToolSvc += CfgMgr.LVL1__L1CaloxAODOfflineTriggerTowerTools( "LVL1__L1CaloxAODOfflineTriggerTowerTools" )
# decorate L1Calo Trigger Towers
from TrigT1CaloCalibTools.TrigT1CaloCalibToolsConf import LVL1__L1CaloTriggerTowerDecoratorAlg
topSequence += CfgMgr.LVL1__L1CaloTriggerTowerDecoratorAlg( TriggerTowerTools = ToolSvc.LVL1__L1CaloxAODOfflineTriggerTowerTools )
