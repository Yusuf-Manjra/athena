#!/usr/bin/env python

# art-description: CalibPeb test with forks=4, threads=6, concurrent_events=3
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TrigExPartialEB/MTCalibPeb.py'
ex.input = 'data'
ex.forks = 4
ex.threads = 6
ex.concurrent_events = 3

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Make RootComp step required
rc = test.get_step('RootComp')
rc.required = True
rc.args += ' --sortLabels'

import sys
sys.exit(test.run())
