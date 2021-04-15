#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
Definitions of additional validation steps in Trigger ART tests relevant only for TrigInDetValidation
The main common check steps are defined in the TrigValSteering.CheckSteps module.
'''

import os
import json

from TrigValTools.TrigValSteering.ExecStep import ExecStep
from TrigValTools.TrigValSteering.Step import Step
from TrigValTools.TrigValSteering.CheckSteps import RefComparisonStep
from AthenaCommon.Utils.unixtools import FindFile

##################################################
# Exec (athena) steps for Reco_tf
##################################################

class TrigInDetReco(ExecStep):

    def __init__(self, name='TrigInDetReco', postinclude_file='', preinclude_file='' ):
        ExecStep.__init__(self, name)
##        super(TrigInDetReco, self).__init__(name)
        self.type = 'Reco_tf'
        self.max_events=-1
        self.required = True
        self.threads = 1 # TODO: change to 4
        self.concurrent_events = 1 # TODO: change to 4
        self.perfmon = False
        self.timeout = 18*3600
        self.slices = []
        self.preexec_trig = ' '
        self.postinclude_trig = postinclude_file
        self.preinclude_trig  = preinclude_file
        self.release = 'latest'
        self.preexec_reco =  ';'.join([
            'from RecExConfig.RecFlags import rec',
            'rec.doForwardDet=False',
            'rec.doEgamma=False',
            'rec.doMuonCombined=False',
            'rec.doJetMissingETTag=False',
            'rec.doTau=False'
        ])

        self.preexec_aod = ';'.join([
            self.preexec_reco,
            'from ParticleBuilderOptions.AODFlags import AODFlags',
            'AODFlags.ThinGeantTruth.set_Value_and_Lock(False)',
            'AODFlags.ThinNegativeEnergyCaloClusters.set_Value_and_Lock(False)',
            'AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False)',
            'AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False)'
        ])

        self.preexec_all = ';'.join([
            'from TriggerJobOpts.TriggerFlags import TriggerFlags',
            'TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\")',
        ])
        self.postexec_trig = "from AthenaCommon.AppMgr import ServiceMgr; ServiceMgr.AthenaPoolCnvSvc.MaxFileSizes=['tmp.RDO_TRIG=100000000000']"


        self.postexec_reco = "from AthenaCommon.AppMgr import ServiceMgr; ServiceMgr.AthenaPoolCnvSvc.MaxFileSizes=['tmp.ESD=100000000000']"
        self.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG"'
       
        if ( self.postinclude_trig != '' ) : 
            print( "postinclude_trig: ", self.postinclude_trig )

        if ( self.preinclude_trig != '' ) : 
            print( "preinclude_trig:  ", self.preinclude_trig  )


    def configure(self, test):
        chains = '['
        flags = ''
        for i in self.slices:
            if (i=='L2muonLRT') :
                chains += "'HLT_mu6_LRT_idperf_l2lrt_L1MU6',"
                chains += "'HLT_mu6_idperf_L1MU6',"
                flags += 'doMuonSlice=True;'
            if (i=='FSLRT') :
                chains += "'HLT_unconvtrk0_fslrt_L1All',"
                flags  += 'doUnconventionalTrackingSlice=True;'
            if (i=='muon') :
                chains += "'HLT_mu6_idperf_L1MU6',"
                chains += "'HLT_mu24_idperf_L1MU20',"
                chains += "'HLT_mu26_ivarperf_L1MU20',"
                flags += 'doMuonSlice=True;'
            if (i=='electron') :
                chains +=  "'HLT_e5_etcut_L1EM3',"  ## need an idperf chain once one is in the menu
                chains +=  "'HLT_e17_lhvloose_nod0_L1EM15VH'," 
                chains +=  "'HLT_e26_lhtight_gsf_L1EM22VHI',"
                flags += 'doEgammaSlice=True;'
            if (i=='tau') :
                chains +=  "'HLT_tau25_idperf_tracktwo_L1TAU12IM',"
                chains +=  "'HLT_tau25_idperf_tracktwoMVA_L1TAU12IM',"
                flags += 'doTauSlice=True;'
            if (i=='bjet') :
                chains += "'HLT_j45_subjesgscIS_ftf_boffperf_split_L1J20',"
                flags  += 'doBjetSlice=True;'
            if ( i=='fsjet' or i=='fs' or i=='jet' ) :
                chains += "'HLT_j45_ftf_L1J15',"
                flags  += 'doJetSlice=True;'
            if (i=='beamspot') :
                chains += "'HLT_beamspot_allTE_trkfast_BeamSpotPEB_L1J15','HLT_beamspot_trkFS_trkfast_BeamSpotPEB_L1J15',"
                flags  += 'doBeamspotSlice=True;'
            if (i=='minbias') :
                chains += "'HLT_mb_sptrk_L1RD0_FILLED',"
                flags  += "doMinBiasSlice=True;setMenu='LS2_v1';"
            if (i=='cosmic') :
                chains += "'HLT_mu4_cosmic_L1MU4'"
                flags  += "doMuonSlice=True;doCosmics=True;setMenu='Cosmic_run3_v1';"
        if ( flags=='' ) : 
            print( "ERROR: no chains configured" )

        chains += ']'
        self.preexec_trig = 'doEmptyMenu=True;'+flags+'selectChains='+chains


        if (self.release == 'current'):
            print( "Using current release for offline Reco steps  " )
        else:
            # get the current atlas base release, and the previous base release
            import os
            DVERSION=os.getenv('Athena_VERSION')
            if (self.release == 'latest'):
                if ( DVERSION is None ) :
                    AVERSION = "22.0.20"
                else:
                    BASE=DVERSION[:5]
                    SUB=int(DVERSION[5:])
                    SUB -= 1
                    AVERSION=BASE+str(SUB)
            else:
                AVERSION = self.release
            self.args += ' --asetup "RAWtoESD:Athena,'+AVERSION+'" "ESDtoAOD:Athena,'+AVERSION+'" '
            print( "remapping athena base release version for offline Reco steps: ", DVERSION, " -> ", AVERSION )


        self.args += ' --preExec "RDOtoRDOTrigger:{:s};" "all:{:s};" "RAWtoESD:{:s};" "ESDtoAOD:{:s};"'.format(
            self.preexec_trig, self.preexec_all, self.preexec_reco, self.preexec_aod)
        if (self.postexec_trig != ' '):
            self.args += ' --postExec "RDOtoRDOTrigger:{:s};" "RAWtoESD:{:s};" '.format(self.postexec_trig, self.postexec_reco)
        if (self.postinclude_trig != ''):
            self.args += ' --postInclude "{:s}" '.format(self.postinclude_trig)
        if (self.preinclude_trig != ''):
            self.args += ' --preInclude "{:s}" '.format(self.preinclude_trig)
        super(TrigInDetReco, self).configure(test)


##################################################
# Additional exec (athena) steps - AOD to TrkNtuple
##################################################

class TrigInDetAna(ExecStep):
    def __init__(self, name='TrigInDetAna', extraArgs=None):
        ExecStep.__init__(self, name )
        self.type = 'athena'
        self.job_options = 'TrigInDetValidation/TrigInDetValidation_AODtoTrkNtuple.py'
        self.max_events=-1
        self.required = True
        self.depends_on_previous = True
        #self.input = 'AOD.pool.root'
        self.input = ''
        self.perfmon=False
        self.imf=False
        if extraArgs is not None:
            self.args = extraArgs


##################################################
# Additional exec (athena) steps - RDO to CostMonitoring
##################################################

class TrigCostStep(Step):
    def __init__(self, name='TrigCostStep'):
        super(TrigCostStep, self).__init__(name)
        self.required = True
        self.depends_on_previous = True
        self.input = 'tmp.RDO_TRIG'
        self.args = ' --MCCrossSection=0.5 Input.Files=\'["tmp.RDO_TRIG"]\' '
        self.executable = 'RunTrigCostAnalysis.py'



##################################################
# Additional post-processing steps
##################################################

class TrigInDetRdictStep(Step):
    '''
    Execute TIDArdict for TrkNtuple files.
    '''
    def __init__(self, name='TrigInDetdict', args=None, testbin='Test_bin.dat'):
        super(TrigInDetRdictStep, self).__init__(name)
        self.args=args + "  -b " + testbin + " "
        self.auto_report_result = True
        self.required = True
        self.executable = 'TIDArdict'
        self.timeout = 10*60

    def configure(self, test):
        os.system( 'get_files -data TIDAbeam.dat &> /dev/null' )
        os.system( 'get_files -data Test_bin.dat &> /dev/null' )
        os.system( 'get_files -data Test_bin_larged0.dat &> /dev/null' )
        os.system( 'get_files -data Test_bin_lrt.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata-chains-run3.dat &> /dev/null' )
        os.system( 'get_files -data TIDAhisto-panel.dat &> /dev/null' )
        os.system( 'get_files -data TIDAhisto-panel-vtx.dat &> /dev/null' )
        os.system( 'get_files -data TIDAhistos-vtx.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata-run3.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata-run3-larged0.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata-run3-larged0-el.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata-run3-lrt.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata-run3-minbias.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata_cuts.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata-run3-offline.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata-run3-offline-larged0.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata-run3-offline-larged0-el.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata-run3-offline-lrt.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata-run3-offline-vtx.dat &> /dev/null' )
        os.system( 'get_files -data TIDAdata_cuts-offline.dat &> /dev/null' )
        super(TrigInDetRdictStep, self).configure(test)


def json_chains( slice ) : 
    json_file     = 'TrigInDetValidation/comparitor.json'
    json_fullpath = FindFile(json_file, os.environ['DATAPATH'].split(os.pathsep), os.R_OK)

    if not json_fullpath:
        print('Failed to determine full path for input JSON %s', json_file)
        return None
        
    with open(json_fullpath) as f:
        data = json.load(f)

    chainmap = data[slice]

    return chainmap['chains']



class TrigInDetCompStep(RefComparisonStep):
    '''
    Execute TIDAcomparitor for data.root files.
    '''
    def __init__( self, name='TrigInDetComp', slice=None, args=None, file=None ):
        super(TrigInDetCompStep, self).__init__(name)

        self.input_file = file    
        self.slice = slice 
        self.auto_report_result = True
        self.required   = True
        self.args = args
        self.executable = 'TIDAcomparitor'
    

    def configure(self, test):
        RefComparisonStep.configure(self, test)
        if self.reference is None :
            self.args  = self.args + " " + self.input_file + "  " + self.input_file + " --noref --oldrms "
        else:
            self.args  = self.args + " " + self.input_file + "  " + self.reference + " --oldrms "
        self.chains = json_chains( self.slice )
        self.args += " " + self.chains
        print( "\033[0;32mTIDAcomparitor "+self.args+" \033[0m" )
        Step.configure(self, test)




class TrigInDetCpuCostStep(RefComparisonStep):
    '''
    Execute TIDAcpucost for data.root files.
    '''
    def __init__( self, name='TrigInDetCpuCost', outdir=None, infile=None, extra=None ):
        super(TrigInDetCpuCostStep, self).__init__(name)

        self.input_file = infile
        self.output_dir = outdir
        self.auto_report_result = True
        self.required   = True
        self.extra = extra
        self.executable = 'TIDAcpucost'
    

    def configure(self, test):
        RefComparisonStep.configure(self, test)
        if self.reference is None :
            self.args  = self.input_file + " -o " + self.output_dir + " " + self.extra + " --noref --logx "
        else:
            self.args  = self.input_file + " " + self.reference + " -o " + self.output_dir + " " + self.extra + " --logx "
        Step.configure(self, test)

