#!/usr/bin/env python

# art-description: Legacy trigger test on MC with MC_HI_v4 menu
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerTest/testCommonSliceAthenaTrigRDO.py'
ex.input = 'ttbar_rel21'
ex.args = '-c \'setMenu="MC_HI_v4";\''

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
