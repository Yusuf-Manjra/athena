#!/usr/bin/env python

# art-description: Trigger BS->BS athena test of the Dev_pp_run3_v1 menu
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Step

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
ex.threads = 1
# LS2_v1 soon to be renamed to Dev_pp_run3_v1
ex.args = '-c "setMenu=\'LS2_v1\';doWriteBS=True;doWriteRDOTrigger=False;"'

checkBS = Step.Step("CheckBS")
checkBS.executable = 'trigbs_dumpHLTContentInBS_run3.py'
checkBS.args = ' --l1 --hlt --hltres --stag --sizeSummary'
checkBS.args += ' `find . -name \'*Single_Stream.daq.RAW.*Athena.*.data\' | tail -n 1`'
checkBS.required = True
checkBS.auto_report_result = True

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = [checkBS] + CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
