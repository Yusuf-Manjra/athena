#!/usr/bin/env python
# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger AthenaMT test running new-style job options
# art-type: build
# art-include: master/Athena
# art-include: 23.0/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Input

ex = ExecStep.ExecStep('athena')
ex.type = 'other'
ex.input = 'data'
ex.executable = 'runHLT_standalone_newJO.py'
ex.args  = ' --filesInput='+Input.get_input('data').paths[0]
ex.args += ' Trigger.triggerMenuSetup="Dev_pp_run3_v1"'
ex.args += ' Trigger.doRuntimeNaviVal=True'
ex.prmon = False

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

chaindump = test.get_step("ChainDump")
chaindump.args = '--json --yaml ref_data_newJO_build.new'
# In future, when this is primary test will enable strict reference checking 
# The code below does so.
# refcomp = CheckSteps.ChainCompStep("CountRefComp")
# refcomp.input_file = 'ref_data_newJO_build.new'
# refcomp.required = True # Final exit code depends on this step
# CheckSteps.add_step_after_type(test.check_steps, CheckSteps.ChainDumpStep, refcomp)

import sys
sys.exit(test.run())
