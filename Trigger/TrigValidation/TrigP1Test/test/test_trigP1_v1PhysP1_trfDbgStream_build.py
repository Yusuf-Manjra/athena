#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: transform test of BSRDOtoRAW (including debug recovery) , using v1PhysP1 menu
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

#====================================================================================================
# HLT BS_RDO->RAW
hlt = ExecStep.ExecStep('BSRDOtoRAW')
hlt.type = 'Trig_reco_tf'
hlt.forks = 1
hlt.threads = 1
hlt.concurrent_events = 1
hlt.max_events = -1 # Debug recovery does not handle maxEvents/skipEvents flag
hlt.args  = '--precommand=\\\"setMenu=\\\'PhysicsP1_pp_run3_v1\\\'\\\"'
hlt.args += ' --streamSelection=All'
hlt.args += ' --prodSysBSRDO True'
hlt.args += ' --outputBSFile=RAW.pool.root'
hlt.args += ' --outputHIST_DEBUGSTREAMMONFile=HIST_DEBUGSTREAMMON.testing.data' # Turn on debug recovery step
hlt.input = 'data_run3'


#====================================================================================================
# The full test
test = Test.Test()
test.art_type = 'build'
test.exec_steps = [hlt]
test.check_steps = CheckSteps.default_check_steps(test)

# Overwrite default MessageCount settings
msgcount = test.get_step("MessageCount")
msgcount.thresholds = {
   'INFO': 300
}
msgcount.required = True

test.check_steps.remove(test.get_step("ZeroCounts"))

import sys
sys.exit(test.run())
