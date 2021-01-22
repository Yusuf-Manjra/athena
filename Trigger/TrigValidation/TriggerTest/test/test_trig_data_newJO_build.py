#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger AthenaMT test running new-style job options
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, Step, ExecStep, CheckSteps, Input

# Copy the job options to the working directory
copy_jo = ExecStep.ExecStep('CopyJO')
copy_jo.type = 'other'
copy_jo.input = ''
copy_jo.executable = 'get_files'
copy_jo.args = '-remove -jo TriggerJobOpts/runHLT_standalone_newJO.py'
copy_jo.prmon = False
copy_jo.auto_report_result = False  # Do not set art-result for this step
copy_jo.output_stream = Step.Step.OutputStream.STDOUT_ONLY  # Do not create a log file for this step

# Generate configuration run file
run = ExecStep.ExecStep('athena')
run.type = 'other'
run.input = 'data'
run.executable = 'python'
run.args = 'runHLT_standalone_newJO.py'
run.args += ' --filesInput='+Input.get_input('data').paths[0]
run.args += ' Trigger.triggerMenuSetup="LS2_v1"'
run.prmon = False


# The full test configuration
test = Test.Test()
test.art_type = 'build'
#test.check_steps = CheckSteps.default_check_steps(test)
test.exec_steps = [copy_jo, run]
check_log = CheckSteps.CheckLogStep('CheckLog')
check_log.log_file = run.get_log_file_name()
test.check_steps = [check_log]

# Change RegTest pattern
#regtest = test.get_step('RegTest')
#regtest.regex = r'TrigSignatureMoniMT\s*INFO\sHLT_.*|TrigSignatureMoniMT\s*INFO\s-- #[0-9]+ (Events|Features).*'

import sys
sys.exit(test.run())
