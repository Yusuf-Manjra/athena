#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: Runs athenaHLT writing BS output and then runs BS decoding
# art-type: grid
# art-include: master/Athena
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
# art-memory: 7000

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigValTools.TrigValSteering.Common import find_file

##################################################
# Helper functions to build the test steps
##################################################

def filterBS(stream_name):
    '''Extract ByteStream data for a given stream from a file with multiple streams'''
    filterStep = ExecStep.ExecStep('FilterBS_'+stream_name)
    filterStep.type = 'other'
    filterStep.executable = 'trigbs_extractStream.py'
    filterStep.input = ''
    filterStep.args = '-s ' + stream_name + ' ' + find_file('*_HLTMPPy_output.*.data')
    return filterStep

def decodeBS(stream_name):
    '''Deserialise HLT data from ByteStream and save to an ESD file'''
    decodeStep = ExecStep.ExecStep('DecodeBS_'+stream_name)
    decodeStep.type = 'athena'
    decodeStep.job_options = 'TriggerJobOpts/decodeBS.py'
    decodeStep.input = ''
    decodeStep.explicit_input = True
    decodeStep.args = '--filesInput='+find_file('*'+stream_name+'*._athenaHLT*.data')
    decodeStep.perfmon = False  # no need to run PerfMon for this step
    return decodeStep

##################################################
# Test definition
##################################################

# Run standard athenaHLT BS->BS job
writeBS = ExecStep.ExecStep("WriteBS")
writeBS.type = 'athenaHLT'
writeBS.job_options = 'TriggerJobOpts/runHLT_standalone.py'
writeBS.input = 'data'
writeBS.args = '-o output'
writeBS.args += ' -c "setMenu=\'LS2_v1_TriggerValidation_prescale\';doL1Sim=True;rewriteLVL1=True;"'  # LS2_v1 to be renamed to Dev_pp_run3_v1

# Extract and decode physics_Main
filterMain = filterBS("Main")
decodeMain = decodeBS("Main")

# Extract and decode calibration_CostMonitoring
filterCost = filterBS("CostMonitoring")
decodeCost = decodeBS("CostMonitoring")
decodeCost.args += ' -c "ModuleID=1"'

# Test definition with updated CheckFile inputs
test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [writeBS, filterMain, decodeMain, filterCost, decodeCost]
test.check_steps = CheckSteps.default_check_steps(test)
test.get_step('CheckFile').input_file = 'ESD.pool.root,ESD.Module1.pool.root'

import sys
sys.exit(test.run())
