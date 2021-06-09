#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: Test running on ALFACalib data with ALFA ROB Monitor chains
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data_ALFACalib'
precommand = ''.join([
  "setMenu='PhysicsP1_pp_run3_v1';",
  "doL1Sim=True;",
  "selectChains=['HLT_mb_alfaperf_L1RD0_FILLED','HLT_mb_alfaperf_L1RD0_EMPTY'];",
])
ex.args = '-c "{:s}"'.format(precommand)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
