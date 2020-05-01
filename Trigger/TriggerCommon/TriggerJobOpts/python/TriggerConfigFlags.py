# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createTriggerFlags():

    flags = AthConfigFlags()
    # enables L1 simulation
    flags.addFlag('Trigger.doLVL1', lambda prevFlags: prevFlags.Input.isMC)

    # enables L1 topological trigger simulation
    flags.addFlag('Trigger.doL1Topo', True )

    # writes additional info from Topo simulation
    flags.addFlag('Trigger.writeL1TopoValData', True )
    
    # need proper documentation
    flags.addFlag('Trigger.useL1CaloCalibration', False)

    # need proper documentation
    flags.addFlag('Trigger.useRun1CaloEnergyScale', False)

    # enable HLT part of the trigger
    flags.addFlag('Trigger.doHLT', True)

    # changes decoding of L1 so that allways all configured chains are enabled, testing mode
    flags.addFlag("Trigger.L1Decoder.forceEnableAllChains", False)

    # L1 decoding options
    flags.addFlag('Trigger.decodeLegacyL1', True)
    flags.addFlag('Trigger.decodePhaseIL1', False)
    
    # if 1, Run1 decoding version is set; if 2, Run2; if 3, Run 3 
    flags.addFlag('Trigger.EDMDecodingVersion', 3)

    # enables additional algorithms colecting MC truth infrmation  (this is only used by IDso maybe we need Trigger.ID.doTruth only?)
    flags.addFlag('Trigger.doTruth', False)

    # only enable services for analysis and BS -> ESD processing (we need better name)
    flags.addFlag('Trigger.doTriggerConfigOnly', False)

    # Enables collection and export of detailed monitoring data of the HLT execution
    flags.addFlag('Trigger.CostMonitoring.doCostMonitoring', False)
    flags.addFlag('Trigger.CostMonitoring.chain', 'HLT_costmonitor')
    flags.addFlag('Trigger.CostMonitoring.outputCollection', 'HLT_TrigCostContainer')
    flags.addFlag('Trigger.CostMonitoring.monitorAllEvents', True) # Defaulting to "True" is temporary


    # enable Bcm inputs simulation
    flags.addFlag('Trigger.L1.doBcm', True)

    # enable muon inputs simulation
    flags.addFlag('Trigger.L1.doMuons', True)

    # version of CTP data, int value up to 4
    flags.addFlag('Trigger.L1.CTPVersion', 4)

    # list of thresholds (not sure if we want to use new flags to generate L1, leaving out for now?)
    
    # partition name used to determine online vs offline BS result writing
    import os
    flags.addFlag('Trigger.Online.partitionName', os.getenv('TDAQ_PARTITION') or '')
    
    # write BS output file
    flags.addFlag('Trigger.writeBS', False)

    # Write transient BS before executing HLT algorithms (for running on MC RDO with clients which require BS inputs)
    flags.addFlag('Trigger.doTransientByteStream', False)

    # list of EDM objects to be written to AOD
    flags.addFlag('Trigger.AODEDMSet', 'AODSLIM')

    # list of objects to be written to ESD
    flags.addFlag('Trigger.ESDEDMSet', 'ESD')

    # tag to be used for condutions used by HLT code
    flags.addFlag('Trigger.OnlineCondTag', 'CONDBR2-HLTP-2018-01')

    # geometry version used by HLT online
    flags.addFlag('Trigger.OnlineGeoTag', 'ATLAS-R2-2016-01-00-01')

    # configuration tune for various years of Run2 (why string?)
    flags.addFlag('Trigger.run2Config', '2018')

    # comissionig options
    # one of:  'HltOnly',
    #    'Lvl1Only',
    #    'FullTrigger',
    #    'NoTrigger'
    flags.addFlag('Trigger.dataTakingConditions', 'FullTrigger')
                

    # use or not frontier proxies
    flags.addFlag('Trigger.triggerUseFrontier', False)

    # the configuration source
    # see https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerConfigFlag
    flags.addFlag('Trigger.triggerConfig', 'FILE')

    # name of the trigger menu
    flags.addFlag('Trigger.triggerMenuSetup', 'LS2_v1')

    # name of the trigger menu
    flags.addFlag('Trigger.generateMenuDiagnostics', False)

    # version of the menu
    from AthenaCommon.AppMgr import release_metadata
    flags.addFlag('Trigger.menuVersion',
                  lambda prevFlags:  release_metadata()['release'] )
    
    # generate or not the HLT configuration
    flags.addFlag('Trigger.generateHLTMenu', False)
    
    # HLT XML file name 
    flags.addFlag('Trigger.HLTMenuFile',
                  lambda prevFlags: 'HLTMenu_'+prevFlags.Trigger.triggerMenuSetup+'_' + prevFlags.Trigger.menuVersion + '.xml')

    # generate or not the L1 configuration
    flags.addFlag('Trigger.generateL1Menu', False)
    
    # L1 XML file name 
    flags.addFlag('Trigger.LVL1ConfigFile',
                  lambda prevFlags: 'LVL1config_'+prevFlags.Trigger.triggerMenuSetup+'_' + prevFlags.Trigger.menuVersion + '.xml')

    # L1 Json file name 
    flags.addFlag('Trigger.L1MenuFile',
                  lambda prevFlags: 'L1Menu_'+prevFlags.Trigger.triggerMenuSetup+'_' + prevFlags.Trigger.menuVersion + '.json')
    
    # L1 topo XML file name
    def _deriveTopoConfigName(prevFlags):
        import re
        menuSetup = prevFlags.Trigger.triggerMenuSetup
        m = re.match(r'(.*v\d).*', menuSetup)
        if m:
            menuSetup = m.groups()[0]
        return "L1Topoconfig_" + menuSetup + "_" + prevFlags.Trigger.menuVersion + ".xml"
    flags.addFlag('Trigger.LVL1TopoConfigFile', _deriveTopoConfigName)

    
    # trigger reconstruction 

    # enables the correction for pileup in cell energy calibration (should it be moved to some place where other calo flags are defined?)
    flags.addFlag('Trigger.calo.doOffsetCorrection', True )

    # helper to switch between versions
    def __tunes(default, ver2016, ver2017):
        from collections import defaultdict
        return lambda year:  defaultdict( lambda: default,
                                           (('2016', ver2016),
                                            ('2017', ver2017)))[year]

                                           
    # Particle ID tune
    flags.addFlag('Trigger.egamma.pidVersion',
                lambda prevFlags:
                __tunes(default='ElectronPhotonSelectorTools/trigger/rel21_mc16a/',
                        ver2016='ElectronPhotonSelectorTools/trigger/rel21_mc16a/',
                        ver2017='ElectronPhotonSelectorTools/trigger/rel21_20170214/')( prevFlags.Trigger.run2Config )
    )

    # cluster correction version, allowed value is: None or v12phiflip_noecorrnogap
    flags.addFlag('Trigger.egamma.clusterCorrectionVersion',
                lambda prevFlags:
                __tunes(default=None,
                        ver2016=None,
                        ver2017='v12phiflip_noecorrnogap')( prevFlags.Trigger.run2Config )
    )
    # tune of MVA
    flags.addFlag('Trigger.egamma.calibMVAVersion',
                  lambda prevFlags:
                  __tunes(default='egammaMVACalib/online/v3',
                          ver2016='egammaMVACalib/online/v3',
                          ver2017='egammaMVACalib/online/v6')( prevFlags.Trigger.run2Config )
    )

    # muons
    flags.addFlag('Trigger.muon.doEFRoIDrivenAccess', False)

    # muon offline reco flags varaint for trigger
    def __muon():
        from MuonConfig.MuonConfigFlags import createMuonConfigFlags
        return createMuonConfigFlags()
    flags.addFlagsCategory('Trigger.Offline', __muon)


    from TriggerJobOpts.MenuConfigFlags import createMenuFlags
    flags.join( createMenuFlags() )

    return flags
    # for reference, this flags are skipped as never used or never set in fact, or set identical to de default or used in a very old JO:
    # readLVL1Calo, readLVL1Muon, fakeLVL1, useCaloTTL
    # doCosmicSim - old JO
    # disableRandomPrescale - never used
    # doMergedHLTResult - not needed now
    # doAlwaysUnpackDSResult - never set
    # doTrt - fast TRT or trigger - never used
    # doLucid - not sure if ever set to False, identical rec flag exists
    # doZdc - never set to anything different than default
    # doHLTpersistency - never red
    # useOfflineSpacePoints - used only to set 
    # doNtuple - never red
    # configForStartup                 
    # the flags related to trigger DB are redundant of triggerConfig - need to decide if they are needed in this form
    # also not defined the Prescale sets yet
    # abortOnConfigurationError - never red
    # in signatures
    # egamma: ringerVersion - not used
    # muon: doMuonCalibrationStream - not used
    # tau: doTrackingApproach - not used


    
import unittest
class __YearDependentFlagTest(unittest.TestCase):    
    def runTest(self):
        """... Check if year dependent flags propagate the info correctly"""
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
        flags.Trigger.run2Config='2017'
        self.assertEqual(flags.Trigger.egamma.clusterCorrectionVersion, "v12phiflip_noecorrnogap", " dependent flag setting does not work")
        flags.dump()

class __UseOfOfflineRecoFlagsTest(unittest.TestCase):
    def runTest(self):
        """... Check if offline reco flags can be added to trigger"""
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
        flags.Trigger.Offline.Muon.doMDTs=False
        flags.Muon.doMDTs=True
        self.assertEqual(flags.Trigger.Offline.Muon.doMDTs, False, " dependent flag setting does not work")
        self.assertEqual(flags.Muon.doMDTs, True, " dependent flag setting does not work")

        newflags = flags.cloneAndReplace('Muon', 'Trigger.Offline.Muon')

        self.assertEqual(flags.Muon.doMDTs, True, " dependent flag setting does not work")
        self.assertEqual(newflags.Muon.doMDTs, False, " dependent flag setting does not work")
        newflags.dump()

if __name__ == "__main__":
    suite = unittest.TestSuite()
    suite.addTest(__YearDependentFlagTest())
    suite.addTest(__UseOfOfflineRecoFlagsTest())
    runner = unittest.TextTestRunner(failfast=False)
    runner.run(suite)
