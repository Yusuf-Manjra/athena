#!/usr/bin/env python

# art-description: Runs athenaHLT writing BS output and then runs BS decoding
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

##################################################
# Helper functions to build the test steps
##################################################

def findFile(pattern):
    '''Bash inline file name finder'''
    return '`find . -name \'{:s}\' | tail -n 1`'.format(pattern)

def filterBS(stream_name):
    '''Extract ByteStream data for a given stream from a file with multiple streams'''
    filterStep = ExecStep.ExecStep('FilterBS_'+stream_name)
    filterStep.type = 'other'
    filterStep.executable = 'athenaHLT-select-PEB-stream.py'
    filterStep.input = ''
    filterStep.args = '-s ' + stream_name + ' ' + findFile('*_HLTMPPy_output.*.data')
    return filterStep

def decodeBS(stream_name):
    '''Deserialise HLT data from ByteStream and save to an ESD file'''
    decodeStep = ExecStep.ExecStep('DecodeBS_'+stream_name)
    decodeStep.type = 'athena'
    decodeStep.job_options = 'TriggerJobOpts/decodeBS.py'
    decodeStep.input = ''
    decodeStep.explicit_input = True
    decodeStep.args = '--filesInput='+findFile('*'+stream_name+'*._athenaHLT*.data')
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
writeBS.args += ' -c "setMenu=\'LS2_v1\';"'  # LS2_v1 to be renamed to Dev_pp_run3_v1

# Extract and decode physics_Main
filterMain = filterBS("Main")
decodeMain = decodeBS("Main")

# Extract and decode calibration_CostMonitoring
filterCost = filterBS("CostMonitoring")
decodeCost = decodeBS("CostMonitoring")
decodeCost.args += ' -c "ModuleID=1"'

# Test definition with updated CheckFile inputs
test = Test.Test()
test.art_type = 'build'
test.exec_steps = [writeBS, filterMain, decodeMain, filterCost, decodeCost]
test.check_steps = CheckSteps.default_check_steps(test)
test.get_step('CheckFile').input_file = 'ESD.pool.root,ESD.Module1.pool.root'

import sys
sys.exit(test.run())
