#!/usr/bin/env python

# art-description: Test of P1+Tier0 workflow, runs athenaHLT with PhysicsP1_pp_run3_v1 menu followed by offline reco and monitoring
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps


def findFile(pattern):
    return '`find . -name \'{:s}\' | tail -n 1`'.format(pattern)


# HLT step (BS->BS)
hlt = ExecStep.ExecStep()
hlt.type = 'athenaHLT'
hlt.job_options = 'TriggerJobOpts/runHLT_standalone.py'
hlt.input = 'data'
hlt.args = '-c "setMenu=\'PhysicsP1_pp_run3_v1\';"'
hlt.args += ' -o output'

# Extract the physics_Main stream out of the BS file with many streams
filter_bs = ExecStep.ExecStep('FilterBS')
filter_bs.type = 'other'
filter_bs.executable = 'athenaHLT-select-PEB-stream.py'
filter_bs.input = ''
filter_bs.args = '-s Main ' + findFile('*_HLTMPPy_output.*.data')

# Tier-0 reco step (BS->ESD->AOD)
tzrecoPreExec = ' '.join([
  "from TriggerJobOpts.TriggerFlags import TriggerFlags;",
  "TriggerFlags.configForStartup=\'HLToffline\';",
  "TriggerFlags.triggerMenuSetup=\'PhysicsP1_pp_run3_v1\';",
  "TriggerFlags.inputHLTconfigFile.set_Value_and_Lock(\'NONE\');",
  "TriggerFlags.AODEDMSet.set_Value_and_Lock(\'AODFULL\');"
])

tzreco = ExecStep.ExecStep('Tier0Reco')
tzreco.type = 'Reco_tf'
tzreco.threads = 1
tzreco.input = ''
tzreco.explicit_input = True
tzreco.args = '--inputBSFile=' + findFile('*.physics_Main*._athenaHLT*.data')  # output of the previous step
tzreco.args += ' --outputESDFile=ESD.pool.root --outputAODFile=AOD.pool.root'
tzreco.args += ' --conditionsTag=\'CONDBR2-BLKPA-2018-11\' --geometryVersion=\'ATLAS-R2-2016-01-00-01\''
tzreco.args += ' --preExec="{:s}"'.format(tzrecoPreExec)
tzreco.args += ' --postInclude="TriggerTest/disableChronoStatSvcPrintout.py"'

# Tier-0 monitoring step (AOD->HIST)
tzmon = ExecStep.ExecStep('Tier0Mon')
tzmon.type = 'other'
tzmon.executable = 'Run3DQTestingDriver.py'
tzmon.input = ''
tzmon.args = '--dqOffByDefault Input.Files="[\'AOD.pool.root\']" DQ.Steering.doHLTMon=True'

# The full test
test = Test.Test()
test.art_type = 'build'
test.exec_steps = [hlt, filter_bs, tzreco, tzmon]
test.check_steps = CheckSteps.default_check_steps(test)

# Overwrite default histogram file name for checks
for step in [test.get_step(name) for name in ['HistCount', 'RootComp']]:
    step.input_file = 'ExampleMonitorOutput.root'

import sys
sys.exit(test.run())
