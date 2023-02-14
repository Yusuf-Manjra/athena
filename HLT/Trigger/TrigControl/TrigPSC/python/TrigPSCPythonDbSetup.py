# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

###############################################################
## @file   TrigPSCPythonDbSetup.py
## @brief  Minimal Python setup for running from DB/JSON
## @author Frank Winklmeier
###############################################################

## This modules provides a minimal Python setup. It is used when running
## with athenaHLT from DB/JSON. It is not used in a partition!
## Besides providing basic python bindings it also takes care of
## switching the OutputLevel in case the "-l" option was used.

## !!! Do NOT import theApp. It will screw up the configuration !!!

from GaudiPython import InterfaceCast, gbl
from GaudiPython.Bindings import iProperty
from TrigCommon.TrigPyHelper import trigApp
from TrigPSC import PscConfig
from TrigServices.TriggerUnixStandardSetup import _Conf

## If HLT PSK is set on command line read it from DB instead of COOL (ATR-25974)
if PscConfig.forcePSK:
   trigApp.changeJobProperties('HLTPrescaleCondAlg', 'Source', 'DB')

## Set OutputLevel in JobOptionsSvc if "-l" option was used in athenaHLT
logLevel = PscConfig.optmap['LOGLEVEL'].split(',')[0]
if logLevel!="INFO":
   outputLevel = int(locals()[logLevel])
   trigApp.service("MessageSvc", gbl.IMessageSvc).setOutputLevel(outputLevel)
   trigApp.changeJobProperties('.*', 'OutputLevel', str(locals()[logLevel]))

## For running with offline THistSvc from online DB
if not _Conf.useOnlineTHistSvc:
   isvcMgr = InterfaceCast(gbl.ISvcManager)(gbl.Gaudi.svcLocator())
   ## Change service type from TrigMonTHistSvc to THistSvc
   isvcMgr.declareSvcType("THistSvc","THistSvc")
   ## Set standard output files
   from TriggerJobOpts.TriggerHistSvcConfig import setTHistSvcOutput
   output = []
   setTHistSvcOutput(output)
   setattr(iProperty("THistSvc"), "Output", output)
