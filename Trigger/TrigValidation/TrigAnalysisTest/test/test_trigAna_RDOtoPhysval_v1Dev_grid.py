#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# art-description: Test of transform RDO->RDO_TRIG->ESD->AOD with threads=1 followed by AOD->NTUP_PHYSVAL with serial athena and PHYSVAL_WEB stage
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 4
# art-memory: 7000
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.out
# art-output: *.err
# art-output: *.log.tar.gz
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: prmon*
# art-output: *.check*
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml
# art-output: PHYSVAL_WEB
# art-html: PHYSVAL_WEB

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigAnalysisTest.TrigAnalysisSteps import add_physvalweb_steps

preExec = ';'.join([
  'setMenu=\'LS2_v1_TriggerValidation_prescale\'',
  'from TriggerJobOpts.TriggerFlags import TriggerFlags',
  'TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\")',
])

rdo2aod = ExecStep.ExecStep('RDOtoAOD')
rdo2aod.type = 'Reco_tf'
rdo2aod.input = 'ttbar'
rdo2aod.max_events = 800
rdo2aod.threads = 4
rdo2aod.concurrent_events = 4
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG" --valid=True'
rdo2aod.args += ' --preExec="all:{:s};"'.format(preExec)

physval = ExecStep.ExecStep('PhysVal')
physval.type = 'Reco_tf'
physval.input = ''
physval.explicit_input = True
physval.args = '--inputAODFile=AOD.pool.root --outputNTUP_PHYSVALFile=NTUP_PHYSVAL.pool.root --valid=True'

validationFlags = 'doTrigEgamma,doTrigBphys,doTrigMET,doTrigJet,doTrigMuon,doTrigHLTResult,doTrigCalo,doTrigMinBias,doTrigTau,doTrigIDtrk,doTrigBjet'
validationPreExec = 'from TrigEDMConfig import ContainerRemapping_Run2Run3; ContainerRemapping_Run2Run3.remapHLTContainerNames();'
physval.args += ' --validationFlags="{:s}" --preExec="{:s}"'.format(validationFlags, validationPreExec)

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [rdo2aod,physval]
test.check_steps = CheckSteps.default_check_steps(test)

# Add web display steps
slice_names = [
    'JetMon', 'TauMon', 'MuonMon', 'IDMon',
    'BphysMon', 'HLTCaloESD', 'ResultMon', 'BjetMon',
    'METMon', 'MinBiasMon', 'Egamma']
download = CheckSteps.DownloadRefStep()
add_physvalweb_steps(test, slice_names, download)

import sys
sys.exit(test.run())
