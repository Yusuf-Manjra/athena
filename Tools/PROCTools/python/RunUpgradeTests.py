#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## RunUpgradeTests.py - based heavily on RunTier0Tests, defines a set of standard tests for Running Phase 2 upgrade simulation and digi/reco for checking impacts of changes e.g. prior to merging
# $Id$
import sys
import subprocess
import os
import threading
import time
import uuid
import logging
import glob

### Setup global logging
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s %(levelname)-8s %(message)s',
                    datefmt='%m-%d %H:%M',
                    filename='./RunUpgradeTests.log',
                    filemode='w')
console = logging.StreamHandler()
console.setLevel(logging.INFO)
formatter = logging.Formatter('%(levelname)-8s %(message)s')
console.setFormatter(formatter)
logging.getLogger('').addHandler(console)

def RunCleanSTest(stest,input_file,pwd,release,extraArg,CleanRunHeadDir,UniqID):

    if "maxEvents" not in extraArg:
        extraArg += " --maxEvents=10 "

    s=stest
    trfcmd="echo \"NO VALID CONFIGURATION SELECTED!\""
    CleanDirName="clean_run_"+s+"_"+UniqID

    ###This is the config for the latest (and future) configuration, and differs in several ways from the older configurations - for the time being it relies on a local geometry
    if s == 'sim_updated':

        geotag = "ATLAS-P2-ITK-22-00-00_VALIDATION"
        logging.info("Running clean "+s)
        logging.info("\"Sim_tf.py "+s+" ("+geotag+") --inputEVNTFile "+ input_file + " --outputHITSFile myHITS.pool.root --imf False " + extraArg+"\"")

        trfcmd = " Sim_tf.py --randomSeed 873254 --geometryVersion "+geotag+" --conditionsTag OFLCOND-MC15c-SDR-14-03 --truthStrategy   MC15aPlus --DataRunNumber 242000 --preInclude  all:'InDetSLHC_Example/preInclude.SiliconOnly.py,InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SLHC_Setup.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py' --preExec all:'from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.UseLocalGeometry.set_Value_and_Lock(True)' --postInclude all:'PyJobTransforms/UseFrontier.py,InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py,InDetSLHC_Example/postInclude.SiHitAnalysis.py' --postExec EVNTtoHITS:'ServiceMgr.DetDescrCnvSvc.DoInitNeighbours=False;ServiceMgr.DetDescrCnvSvc.OutputLevel=VERBOSE; from AthenaCommon import CfgGetter; CfgGetter.getService(\"ISF_MC15aPlusTruthService\").BeamPipeTruthStrategies+=[\"ISF_MCTruthStrategyGroupIDHadInt_MC15\"];ServiceMgr.PixelLorentzAngleSvc.ITkL03D = True' --inputEVNTFile "+input_file+" --outputHITSFile myHITS.pool.root --imf False  "
     
    else:
     
        trfcmd = " Sim_tf.py --AMI "+s+" --inputEVNTFile "+input_file + " --outputHITSFile myHITS.pool.root"
        
        logging.info("Running clean "+s)
        logging.info("\"Sim_tf.py --AMIConfig "+s+"--inputEVNTFile "+ input_file + " --outputHITSFile myHITS.pool.root --imf False " + extraArg+"\"")



    cmd = ( " mkdir -p " + CleanRunHeadDir +" ;" + 
            " cd "       + CleanRunHeadDir +" ;" + 
            " mkdir -p " + CleanDirName    +" ;" +
            " cd "       + CleanDirName    +" ;" + 
            trfcmd + extraArg+" > "+s+".log 2>&1" )


    subprocess.call(cmd,shell=True)

    logging.info("Finished clean "+s)
    pass

def RunPatchedSTest(stest,input_file,pwd,release,extraArg,nosetup=False):

    if "maxEvents" not in extraArg:
        extraArg += " --maxEvents=10 "

    s=stest
    trfcmd="echo \"NO VALID CONFIGURATION SELECTED!\""

    if s == 'sim_updated':
       geotag = "ATLAS-P2-ITK-22-00-00_VALIDATION"  
       trfcmd = " Sim_tf.py --randomSeed 873254 --geometryVersion "+geotag+" --conditionsTag OFLCOND-MC15c-SDR-14-03 --truthStrategy   MC15aPlus --DataRunNumber 242000 --preInclude  all:'InDetSLHC_Example/preInclude.SiliconOnly.py,InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SLHC_Setup.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py' --preExec all:'from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.UseLocalGeometry.set_Value_and_Lock(True)' --postInclude all:'PyJobTransforms/UseFrontier.py,InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py,InDetSLHC_Example/postInclude.SiHitAnalysis.py' --postExec EVNTtoHITS:'ServiceMgr.DetDescrCnvSvc.DoInitNeighbours=False;ServiceMgr.DetDescrCnvSvc.OutputLevel=VERBOSE; from AthenaCommon import CfgGetter; CfgGetter.getService(\"ISF_MC15aPlusTruthService\").BeamPipeTruthStrategies+=[\"ISF_MCTruthStrategyGroupIDHadInt_MC15\"];ServiceMgr.PixelLorentzAngleSvc.ITkL03D = True' --inputEVNTFile "+input_file+" --outputHITSFile myHITS.pool.root --imf False  "

       logging.info("Running patched "+s)
       logging.info("\"Sim_tf.py --AMIConfig "+s+" ("+geotag+") --inputEVNTFile "+ input_file + " --outputHITSFile myHITS.pool.root --imf False " + extraArg+"\"")


    else:
       
        trfcmd = " Sim_tf.py --AMI "+s+" --inputEVNTFile "+input_file + " --outputHITSFile myHITS.pool.root"
            
        logging.info("Running patched "+s)
        logging.info("\"Sim_tf.py --AMIConfig "+s+" --inputEVNTFile "+ input_file + " --outputHITSFile myHITS.pool.root --imf False " + extraArg+"\"")

    cmd = " cd "+pwd+" ;"
    if nosetup:
        pass
    elif 'WorkDir_DIR' in os.environ:
        cmake_build_dir = (os.environ['WorkDir_DIR'])
        cmd += ( " source $AtlasSetup/scripts/asetup.sh "+release+"  >& /dev/null;" + 
                 " source "+cmake_build_dir+"/setup.sh ;" )
    else :
        cmd = ( " source $AtlasSetup/scripts/asetup.sh "+release+"  >& /dev/null;" )
    cmd += " mkdir -p run_"+s+"; cd run_"+s+";"
    cmd += trfcmd + extraArg+" > "+s+".log 2>&1"
    
    subprocess.call(cmd,shell=True)
    logging.info("Finished patched "+s)
    pass

def RunCleanQTest(qtest,pwd,release,extraArg,CleanRunHeadDir,UniqID):
    q=qtest
    trfcmd="echo \"NO VALID CONFIGURATION SELECTED!\""
    inputfile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetSLHC_Example/inputs/s3551.pool.root" 
    
    if q == "reco_updated":
        geotag = "ATLAS-P2-ITK-22-00-00"
        inputfile =  pwd+"/run_sim_updated/myHITS.pool.root"

        logging.info("Running patched "+q+" ("+geotag+") \"Reco_tf.py --AMI "+q+" --imf False "+extraArg+"\"")

        trfcmd = " Reco_tf.py --inputHITSFile "+inputfile+" --outputRDOFile myRDO.pool.root --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputDAOD_IDTRKVALIDFile myIDTRKVALID.pool.root --maxEvents 10 --digiSteeringConf StandardInTimeOnlyTruth --geometryVersion "+geotag+" --conditionsTag OFLCOND-MC15c-SDR-14-03 --DataRunNumber 242000 --postInclude all:'InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py' HITtoRDO:'InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' RAWtoESD:'InDetSLHC_Example/postInclude.DigitalClustering.py' --preExec all:'from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock(\"geant4\"); from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags; SLHC_Flags.doGMX.set_Value_and_Lock(True);SLHC_Flags.UseLocalGeometry.set_Value_and_Lock(True)' HITtoRDO:'from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=[\"SimLayout\",\"PhysicsList\"];' ESDtoDPD:'rec.DPDMakerScripts.set_Value_and_Lock([\"InDetPrepRawDataToxAOD/InDetDxAOD.py\",\"PrimaryDPDMaker/PrimaryDPDMaker.py\"]);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.useDCS.set_Value_and_Lock(True);from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc;ServiceMgr +=PixelCalibSvc();ServiceMgr.PixelCalibSvc.DisableDB=True' --preInclude  all:'InDetSLHC_Example/preInclude.SiliconOnly.py,InDetSLHC_Example/preInclude.SLHC_Setup.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu0.py' HITtoRDO:'InDetSLHC_Example/preInclude.SLHC.py' default:'InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' RDOMergeAthenaMP:'InDetSLHC_Example/preInclude.SiliconOnly.py,InDetSLHC_Example/preInclude.SLHC.py' POOLMergeAthenaMPAOD0:'InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py' POOLMergeAthenaMPDAODIDTRKVALID0:'InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py' --postExec HITtoRDO:'CfgMgr.MessageSvc().setError+=[\"HepMcParticleLink\"];' RAWtoESD:'ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True; from AthenaCommon.AppMgr import ToolSvc; ToolSvc.InDetTrackSummaryTool.OutputLevel=INFO' --imf False"
        
    else:
        if q == 'r11838':
         inputfile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetSLHC_Example/inputs/s3547.HITS.pool.root" 
        
        trfcmd = " Reco_tf.py --inputHITSFile "+inputfile+" --outputRDOFile myRDO.pool.root --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputDAOD_IDTRKVALIDFile myIDTRKVALID.pool.root --maxEvents 10 --AMI "+q

        logging.info("Running clean "+q+" \"Reco_tf.py --AMI "+q+"--imf False "+extraArg+"\"")

        
        
    #Check if CleanRunHead directory exists if not exist with a warning 

    CleanDirName="clean_run_"+q+"_"+UniqID

    cmd = ( " mkdir -p "+ CleanRunHeadDir +" ;" + 
            " cd "      + CleanRunHeadDir +" ;" + 
            " mkdir -p "+ CleanDirName    +" ;" + 
            " cd "      + CleanDirName    +" ;" + 
            " source $AtlasSetup/scripts/asetup.sh "+release+" >& /dev/null ;" +
            trfcmd + " > "+q+".log 2>&1" )
    subprocess.call(cmd,shell=True)
    logging.info("Finished clean \"Reco_tf.py --AMI "+q+"\"")
    pass

def RunPatchedQTest(qtest,pwd,release,extraArg, nosetup=False):
    q=qtest
    inputfile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetSLHC_Example/inputs/s3551.pool.root" 
    trfcmd = "echo \"NO VALID CONFIGURATION SELECTED!\""

    if q == "reco_updated":
        geotag = "ATLAS-P2-ITK-22-00-00"
        layoutoption = ""
        inputfile = pwd+"/run_sim_updated/myHITS.pool.root"

        logging.info("Running patched "+q+" ("+geotag+") \"Reco_tf.py --AMI "+q+" --imf False "+extraArg+"\"")

        trfcmd = " Reco_tf.py --inputHITSFile "+inputfile+" --outputRDOFile myRDO.pool.root --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputDAOD_IDTRKVALIDFile myIDTRKVALID.pool.root --maxEvents 10 --digiSteeringConf StandardInTimeOnlyTruth --geometryVersion "+geotag+" --conditionsTag OFLCOND-MC15c-SDR-14-03 --DataRunNumber 242000 --postInclude all:'InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py' HITtoRDO:'InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' RAWtoESD:'InDetSLHC_Example/postInclude.DigitalClustering.py' --preExec all:'from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock(\"geant4\"); from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags; SLHC_Flags.doGMX.set_Value_and_Lock(True);SLHC_Flags.UseLocalGeometry.set_Value_and_Lock(True)' HITtoRDO:'from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=[\"SimLayout\",\"PhysicsList\"];' ESDtoDPD:'rec.DPDMakerScripts.set_Value_and_Lock([\"InDetPrepRawDataToxAOD/InDetDxAOD.py\",\"PrimaryDPDMaker/PrimaryDPDMaker.py\"]);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.useDCS.set_Value_and_Lock(True);from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc;ServiceMgr +=PixelCalibSvc();ServiceMgr.PixelCalibSvc.DisableDB=True' --preInclude  all:'InDetSLHC_Example/preInclude.SiliconOnly.py,InDetSLHC_Example/preInclude.SLHC_Setup.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu0.py' HITtoRDO:'InDetSLHC_Example/preInclude.SLHC.py' default:'InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' RDOMergeAthenaMP:'InDetSLHC_Example/preInclude.SiliconOnly.py,InDetSLHC_Example/preInclude.SLHC.py' POOLMergeAthenaMPAOD0:'InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py' POOLMergeAthenaMPDAODIDTRKVALID0:'InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py' --postExec HITtoRDO:'CfgMgr.MessageSvc().setError+=[\"HepMcParticleLink\"];' RAWtoESD:'ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True; from AthenaCommon.AppMgr import ToolSvc; ToolSvc.InDetTrackSummaryTool.OutputLevel=INFO' --imf False"
  
    else:
        if q == 'r11838':
         inputfile = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/InDetSLHC_Example/inputs/s3547.HITS.pool.root"

        trfcmd = " Reco_tf.py --inputHITSFile "+inputfile+" --outputRDOFile myRDO.pool.root --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputDAOD_IDTRKVALIDFile myIDTRKVALID.pool.root --maxEvents 10 --AMI "+q

        logging.info("Running patched "+q+" \"Reco_tf.py --AMI "+q+" --imf False "+extraArg+"\"")

    cmd = " cd "+pwd+" ;"
    if nosetup:
        pass
    elif 'WorkDir_DIR' in os.environ:
        cmake_build_dir = (os.environ['WorkDir_DIR'])
        cmd += ( " source $AtlasSetup/scripts/asetup.sh "+release+"  >& /dev/null;" + 
                 " source "+cmake_build_dir+"/setup.sh ;" )
    else :
            cmd = ( " source $AtlasSetup/scripts/asetup.sh "+release+"  >& /dev/null;" )

    cmd += " mkdir -p run_"+q+"; cd run_"+q+";"
    cmd += trfcmd+" > "+q+".log 2>&1"
    
    subprocess.call(cmd,shell=True)

    logging.info("Finished patched \"Reco_tf.py --AMI "+q+"\"")
    pass

def pwd():
    Proc = subprocess.Popen('pwd', shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
    Out = (Proc.communicate()[0])[:-1]     
    return Out
    
def GetReleaseSetup(isCImode=False):

    if isCImode:
        logging.info("")    
        logging.info("No release information is extracted in CI-mode.")    
        logging.info("")    
        setup = ""
        return setup

    current_nightly = os.environ['AtlasBuildStamp']
    release_base=os.environ['AtlasBuildBranch']
    release_head=os.environ['Athena_VERSION']
    platform=os.environ['Athena_PLATFORM']
    project=os.environ['AtlasProject']
    builds_dir_searchStr='/cvmfs/atlas-nightlies.cern.ch/repo/sw/'+release_base+'/[!latest_]*/'+project+'/'+release_head
    # finds all directories matching above search pattern, and sorts by modification time
    # suggest to use latest opt over dbg
    sorted_list = sorted(glob.glob(builds_dir_searchStr), key=os.path.getmtime)
    latest_nightly = ''
    for folder in reversed(sorted_list):
        if not glob.glob(folder+'/../../'+release_base+'__'+project+'*-opt*.log') : continue
        latest_nightly = folder.split('/')[-3]
        break

    if current_nightly != latest_nightly:
        logging.info("Please be aware that you are not testing your tags in the latest available nightly, which is "+latest_nightly )

    setup="%s,%s,%s,Athena"%(release_base,platform.replace("-", ","),current_nightly)

    logging.info("Your tags will be tested in environment "+setup)

    return setup

###############################
########### List patch packages
def list_patch_packages(isCImode=False):

    if isCImode:
        logging.info("")    
        logging.info("No patch information is extracted in CI-mode.")    
        logging.info("")    
        return

    if 'WorkDir_DIR' in os.environ :                 
        logging.info("Patch packages in your build to be tested:\n")
        myfilepath = os.environ['WorkDir_DIR']                                                                                  
        fname = str(myfilepath) + '/packages.txt'                                                                                                   
        with open(fname) as fp:
            for line in fp:
                if '#' not in line:
                    logging.info(line)
    else: 
        logging.warning("")
        logging.warning("A release area with locally installed packages has not been setup.")
        logging.warning("quit by executing <CONTROL-C> if this is not your intention, and")
        logging.warning("source <YOUR_CMake_BUILD_AREA>/setup.sh")
        logging.warning("to pickup locally built packages in your environment.")
        logging.warning("")
    pass

###############################
########### Was the q test successful? To check simply count the number of lines containing the string "successful run"

def QTestsFailedOrPassed(q,qTestsToRun,CleanRunHeadDir,UniqID,RunPatchedOnly=False):
    logging.info("-----------------------------------------------------"  )
    logging.info("Did each step of the "+q+" test complete successfully?" )

    test_dir = CleanRunHeadDir+"/clean_run_"+q+"_"+UniqID

    _Test=True
    for step in qTestsToRun[q]:
        logging.info("")

        cmd = "grep \"successful run\" run_"+q+"/log."+str(step)
        test = subprocess.Popen(['/bin/bash', '-c',cmd], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0]

        if "successful run" in test:
            logging.info(step+" Patched test successful")
        else :
            logging.error(step+" Patched test failed")
            _Test = False

        if RunPatchedOnly : continue   # Skip checking reference test because in this mode the clean tests have not been run
            
        cmd = "grep \"successful run\" " + test_dir + "/log."+str(step)
        ref = subprocess.Popen(['/bin/bash', '-c',cmd], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0]

        if "successful run" in ref:
            logging.info(step+" Reference test successful")
        else :
            logging.error(step+" Reference test failed")
            _Test = False

    logging.info("")       
    if _Test == True:
        logging.info("All "+q+" athena steps completed successfully")
    else :
        logging.error("One or more "+q+" Athena steps failed. Please investigate the cause.")

    return _Test
             

############### Run Frozen Tier0 Policy Test 
def RunFrozenTier0PolicyTest(q,inputFormat,maxEvents,CleanRunHeadDir,UniqID,RunPatchedOnly=False):
    logging.info("---------------------------------------------------------------------------------------" )
    logging.info("Running "+q+" Frozen Tier0 Policy Test on "+inputFormat+" for "+str(maxEvents)+" events" )

    clean_dir = CleanRunHeadDir+"/clean_run_"+q+"_"+UniqID

    if RunPatchedOnly: #overwrite
        # Resolve the subfolder first. Results are stored like: main_folder/q-test/branch/.
        # This should work both in standalone and CI
        subfolder = os.environ['AtlasVersion'][0:4]
        # Use EOS if mounted, otherwise CVMFS
        clean_dir = '/eos/atlas/atlascerngroupdisk/data-art/grid-input/Tier0ChainTests/{0}/{1}'.format(q,subfolder)
        if(glob.glob(clean_dir)):
            logging.info("EOS is mounted, going to read the reference files from there instead of CVMFS")
            clean_dir = 'root://eosatlas.cern.ch/'+clean_dir # In case outside CERN
        else:
            logging.info("EOS is not mounted, going to read the reference files from CVMFS")
            clean_dir = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/{0}/{1}'.format(q,subfolder)

    logging.info("Reading the reference file from location "+clean_dir)

    comparison_command = "acmd.py diff-root "+clean_dir+"/my"+inputFormat+".pool.root run_"+q+"/my"+inputFormat+".pool.root --error-mode resilient --ignore-leaves  RecoTimingObj_p1_EVNTtoHITS_timings  RecoTimingObj_p1_HITStoRDO_timings  RecoTimingObj_p1_RAWtoESD_mems  RecoTimingObj_p1_RAWtoESD_timings  RAWtoESD_mems  RAWtoESD_timings  ESDtoAOD_mems  ESDtoAOD_timings  HITStoRDO_mems  HITStoRDO_timings --entries "+str(maxEvents)+" > run_"+q+"/diff-root-"+q+"."+inputFormat+".log 2>&1"   
    output,error = subprocess.Popen(['/bin/bash', '-c', comparison_command], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

    # We want to catch/print both container additions/subtractions as well as
    # changes in these containers.  `allGood_return_code` is meant to catch
    # other issues found in the diff (not expected, but just to be safe)
    passed_frozen_tier0_test=True
    allGood_return_code=False

    f = open("run_"+q+"/diff-root-"+q+"."+inputFormat+".log", 'r')
    for line in f.readlines():

        if "WARNING" in line: # Catches container addition/subtractions
            logging.error(line)
            passed_frozen_tier0_test=False
        if "leaves differ" in line: # Catches changes in branches
            logging.error(line)
            passed_frozen_tier0_test=False
        if "INFO all good." in line    :
            allGood_return_code = True

    f.close()

    _Test = passed_frozen_tier0_test and allGood_return_code
    if _Test:
        logging.info("Passed!")
    else:
        logging.error("Your tag breaks the frozen tier0 policy in test "+q+". See run_"+q+"/diff-root-"+q+"."+inputFormat+".log file for more information.")

    return _Test

############### Run A Very Simple Test 
def RunTest(q,qTestsToRun,TestName,SearchString,MeasurementUnit,FieldNumber,Threshold,CleanRunHeadDir,UniqID):

    logging.info("-----------------------------------------------------")
    logging.info("Running "+q+" "+TestName+" Test"                      )

    test_dir = CleanRunHeadDir+"/clean_run_"+q+"_"+UniqID
    
    _Test=True 
    for step in qTestsToRun[q]:

        # Memory leak test is not very reliable for nEvents < 50 in AODtoTAG so skipping it...
        if "AODtoTAG" in step and "Memory Leak" in TestName:
            logging.info("Skipping Memory Leak Test for AODtoTAG step...")
            continue

        cmd = "grep \""+SearchString+"\" " + test_dir + "/log."+str(step)


        try:
            ref = int(subprocess.Popen(['/bin/bash', '-c',cmd], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0].split()[FieldNumber])
        except:
            logging.error("No data available in "+ test_dir + "/log."+str(step)+" . Job failed.")
            return  

        cmd = "grep \""+SearchString+"\" run_"+q+"/log."+str(step)

        try:
            test = int(subprocess.Popen(['/bin/bash', '-c',cmd], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0].split()[FieldNumber])
        except:
            logging.error("No data available in run_"+q+"/log."+str(step)+" . Job failed.")
            return
 

        if float(ref) != 0 : 
            factor = float(test) / float(ref)

            # Error if the factor increases (very bad things)
            # Warning if the factor decreases (should be an understood feature)
            if  factor > float(1+Threshold) :
                logging.error(SearchString+" in the "+str(step)+" step with(out) your tag is "+str(test)+" ("+str(ref)+") "+str(MeasurementUnit))
                logging.error("Your tag changes "+SearchString+" by a factor "+str(factor)                                                      )
                logging.error("Is this an expected outcome of your tag(s)?"                                                                     )
                _Test=False
                logging.error(step+" : "+TestName                       )
                logging.error("ref  "+str(ref)+" "+str(MeasurementUnit) )
                logging.error("test "+str(test)+" "+str(MeasurementUnit))
            if factor < float(1-Threshold) :
                logging.warning(SearchString+" in the "+str(step)+" step with(out) your tag is "+str(test)+" ("+str(ref)+") "+str(MeasurementUnit))
                logging.warning("Your tag changes "+SearchString+" by a factor "+str(factor)                                                      )
                logging.warning("Is this an expected outcome of your tag(s)?"                                                                     )
                _Test=True
                logging.warning(step+" : "+TestName                       )
                logging.warning("ref  "+str(ref)+" "+str(MeasurementUnit) )
                logging.warning("test "+str(test)+" "+str(MeasurementUnit))

    if _Test:
        logging.info("Passed!")
    else :
        logging.error("Failed!" )
  
    return _Test

############### Run a WARNING helper function
def warnings_count(file_name):
    warnings=[]
    with open(file_name, "r") as file:
        lines= file.readlines()
        for line in lines:
            if "WARNING" in line:
                if not "| WARNING |" in line:
                    warnings.append(line)
    return warnings


############### Run a WARNINGs TEST
def RunWARNINGSTest(q,qTestsToRun,CleanRunHeadDir,UniqID):
    logging.info("-----------------------------------------------------")
    logging.info("Running "+q+" WARNINGS Test"                      )
    logging.info("")

    test_dir = str(CleanRunHeadDir+"/clean_run_"+q+"_"+UniqID)
    
    _Test=True 
    for step in qTestsToRun[q]:


        ref_file=str(test_dir + "/log."+str(step))
        test_file = str("run_"+q+"/log."+str(step))
        warnings_ref = warnings_count(ref_file)
        warnings_test  = warnings_count (test_file)

        wr=[]
        for w in warnings_ref:
            wr.append(w[9:])
        wt=[]
        for w in warnings_test:
            wt.append(w[9:])

        wn = list(set(wt)-set(wr))
        wo = list(set(wr)-set(wt))


        if (len(warnings_test) > len(warnings_ref)):
            logging.error("Test log file "+test_file+" has "+str(len(warnings_test) - len(warnings_ref))+" more warning(s) than the reference log file "+ref_file)
            logging.error("Please remove the new warning message(s):")
            for w in wn:
                logging.error(w)
            _Test=False

        elif (len(warnings_test) < len(warnings_ref)):
            logging.warning("Test log file "+test_file+" has "+str(len(warnings_ref) - len(warnings_test))+" less warnings than the reference log file "+ref_file)
            logging.warning("The reduction of unnecessary WARNINGs is much appreciated. Is it expected?")
            logging.warning("The following warning messages have been removed:")
            for w in wo:
                logging.warning(w)
            _Test=True
        else :
            logging.info("Test log file "+test_file+" has the same number of warnings as the reference log file "+ref_file)
            _Test=True


        logging.info("")
            
    if _Test:
        logging.info("Passed!")
    else :
        logging.error("Failed!" )
  
    return _Test

##########################################################################
def RunHistTest(q,CleanRunHeadDir,UniqID):
    logging.info("-----------------------------------------------------" )
    logging.info("Running "+q+" HIST Comparison Test"                    )

    ref_file   = CleanRunHeadDir+"/clean_run_"+q+"_"+UniqID+"/myHIST.root"
    test_file = "./run_"+q+"/myHIST.root"

    ref_outfile   = CleanRunHeadDir+"/clean_run_"+q+"_"+UniqID+"/ref."+q+".HIST.chk.log"
    test_outfile = "./run_"+q+"/test."+q+".HIST.chk.log"

    ref_cmd  = "root_lsr_rank.py "+  ref_file +" --hash >& "+ref_outfile
    test_cmd = "root_lsr_rank.py "+ test_file +" --hash >& "+test_outfile

    ref_out  = subprocess.Popen(['/bin/bash', '-c',ref_cmd], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0]
    test_out = subprocess.Popen(['/bin/bash', '-c',test_cmd], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0]
    

    comparison_command = "diff "+ref_outfile+" "+test_outfile+" >& run_"+q+"/diff."+q+".HIST.log"                                                    
    output,error = subprocess.Popen(['/bin/bash', '-c', comparison_command], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()                                                   

    f = open("run_"+q+"/diff."+q+".HIST.log", 'r')
    logging.info("The following are changes you've made to the HIST file: ")
    for line in f.readlines():                                                                                                                                                               
        logging.info(line)
    f.close() 
    logging.info("-----------------------------------------------------" )
    pass





##########################################################################
def main():
    from optparse import OptionParser

    parser=OptionParser(usage="\n ./RunUpgradeTests.py \n")

    parser.add_option("-e",
                      "--extra",
                      type="string",
                      dest="extraArgs",
                      default="",
                      help="define additional args to pass e.g. --preExec 'r2e':'from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup=\"MC_pp_v5\"' ")
    parser.add_option("-f",
                      "--fast",
                      action="store_true",
                      dest="fast_flag",
                      default=False,
                      help="""fast option will run all q tests simultaneously,
                              such that it will run faster if you have 4 cpu core slots on which to run. Be
                              warned! Only recommended when running on a high performance machine, not
                              lxplus""")
    parser.add_option("-c",
                      "--cleanDir",
                      type="string",
                      dest="cleanDir",
                      default="/tmp/",
                      help="specify the head directory for running the clean Tier0 tests. The default is /tmp/${USER}")
    parser.add_option("-r",
                      "--ref",
                      type="string",
                      dest="ref",
                      default=None,
                      help="define a particular reference release")
    parser.add_option("-v",
                      "--val",
                      type="string",
                      dest="val",
                      default=None,
                      help="define a particular validation release")
    parser.add_option("-s",
                      "--sim",
                      action="store_true",
                      dest="sim_flag",
                      default=False,
                      help="sim will run the Sim_tf.py test")
    parser.add_option("-p",
                      "--patched",
                      action="store_true",
                      dest="patched_flag",
                      default=False,
                      help="""patched option will run q-tests just on your
                              patched version of packages. Be warned! File output comparisons will only be
                              performed against pre-defined reference files stored in the directory
                              /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests and performance comparison
                              tests will not be run.""")

    parser.add_option("-i",
                      "--ignoreComparisons",
                      action="store_true",
                      dest="nocheck_flag",
                      default=False,
                      help="""No output comparisons will be run. For CI tests while no fixed references are available.""")
    

    parser.add_option("-n",
                      "--no-setup",
                      action="store_true",
                      dest="ci_flag",
                      default=False,
                      help="no-setup will not setup athena - only for CI tests!")

    parser.add_option("-u",
                      "--updated-test",
                      action="store_true",
                      dest="updatedtest_flag",
                      default=False,
                      help="Run newest development version tests")

    
    parser.add_option("-o",
                      "--old-test",
                      action="store_true",
                      dest="oldtest_flag",
                      default=False,
                      help="Run older version tests")
    
    parser.add_option("-I",
                      "--sim-input-file",
                      dest="siminputfile_flag",
                      default="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/ttbar_muplusjets-pythia6-7000.evgen.pool.root",
                      help="Input file for simulation tests")


    (options,args)=parser.parse_args()

    extraArg = ""
    if options.extraArgs == "MC_pp_v5":
        extraArg = "--preExec 'r2e':'from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup=\"MC_pp_v5\"' "
    else:
        extraArg = options.extraArgs

    RunSim          = options.sim_flag
    RunFast         = options.fast_flag
    RunPatchedOnly  = options.patched_flag
    CleanRunHeadDir = options.cleanDir  
    ciMode          = options.ci_flag
    NoCheck         = options.nocheck_flag
    RunUpdated      = options.updatedtest_flag
    RunOld          = options.oldtest_flag

#        tct_ESD = "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/tct/"+latest_nightly+"/"+release+"/"+platform+"/offline/Tier0ChainTests/"+q+"/myESD.pool.root"          

########### Are we running in CI
    if ciMode:
        logging.info("")
        logging.info("You're running with no-setup. This is suggested to be used only in CI tests.")
        logging.info("This mode assumes athena is setup w/ necessary patches and only runs patched tests.")
        logging.info("If you don't know what this mode does, you shouldn't be using it.")
        logging.info("")
        RunPatchedOnly = True
        NoCheck = True

    if RunUpdated:
        logging.info("")
        logging.info("You are running with the Updated tests, which are for development purposes and may be subject to change!")
        logging.info("This mode currently assumes you have a local geometry checked out, and conforming to the latest changes in the master branch")
        logging.info("This mode assumes athena is setup w/ necessary patches and only runs patched tests.")
        logging.info("If you don't know what this mode does, you shouldn't be using it.")
        logging.info("")
        RunPatchedOnly = True
        NoCheck = True

########### Does the clean run head directory exist?
    if str(CleanRunHeadDir) == "/tmp/":
        myUser = os.environ['USER']
        CleanRunHeadDir = "/tmp/"+str(myUser)

    if os.path.exists(CleanRunHeadDir):
        logging.info("")
        if RunPatchedOnly:
            logging.info("You are running in patched only mode whereby only q-tests against your build are being run.")
            if not NoCheck:               
                logging.info("In this mode ESD and AOD outputs are compared with pre-defined reference files found in the directory")
                logging.info("/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests")
            else:
                logging.info("You are running in ignoreComparisons mode, so no checks will be performed on output files.") 
            if not os.path.exists('/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests'):
                logging.info("")
                logging.error("Exit. Patched only mode can only be run on nodes with access to /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests")
                sys.exit(-1)            
        else:
            logging.info("The head directory for the output of the clean Tier0 q-tests will be "+CleanRunHeadDir)
        logging.info("")
    else:
        logging.error("")
        logging.error("Exit. Please specify a directory that exists for the argument of the -c or --cleanDir option")
        logging.error("")
        logging.error("RunTier0Tests.py  --cleanDir <ExistingDirectory>")
        logging.error("")
        sys.exit(-1)            


########### Is an ATLAS release setup?
    if 'AtlasPatchVersion' not in os.environ and 'AtlasArea' not in os.environ and 'AtlasBaseDir' not in os.environ and 'AtlasVersion' not in os.environ:
        logging.error("Exit. Please setup the an ATLAS release")
        sys.exit(-1)
    else:
        if 'AtlasPatchVersion' not in os.environ and 'AtlasArea' not in os.environ and 'AtlasBaseDir' in os.environ and 'AtlasVersion' not in os.environ:
            logging.warning("Please be aware that you are running a release which seems to not be a Tier0 release, where in general q-tests are not guaranteed to work.")

########### Define which q-tests to run
        sTestTag = 's3551'
        rTestTag = 'r11851'
        if RunOld:
         sTestTag = 's3547'
         rTestTag = 'r11838'   

        qTestsToRun = {}
        if RunUpdated:
            if RunSim:
               qTestsToRun = {
                'sim_updated':['EVNTtoHITS']
               }
            else:
                qTestsToRun = {
                    'sim_updated':['EVNTtoHITS'],
                    'reco_updated':['HITtoRDO','RAWtoESD','ESDtoAOD']
                }
        elif RunSim:
            qTestsToRun = {
            sTestTag:['EVNTtoHITS']
            }
        else:
            qTestsToRun = {
            rTestTag:[ 'HITtoRDO','RAWtoESD','ESDtoAOD']
            }          
            
        
########### Get release info
        if not (options.ref and options.val):
            mysetup = GetReleaseSetup(ciMode)
            cleanSetup = mysetup
        mypwd   = pwd()

########### List the packages in the local InstallArea                                                                                                                                                                                                                           
        if options.ref and options.val:
            cleanSetup = options.ref
            mysetup = options.val
            logging.info("WARNING: You have specified a dedicated release as reference %s and as validation %s release, Your local setup area will not be considered!!!" %(cleanSetup, mysetup))
            logging.info("this option is mainly designed for comparing release versions!!")
        else:
            list_patch_packages(ciMode)

########### Get unique name for the clean run directory
        UniqName = str(uuid.uuid4())

########### Define and run jobs
        mythreads={}
#        mysetup=mysetup+",builds"
        logging.info("------------------ Run Athena q-test jobs---------------"                )

        sim_input_file = options.siminputfile_flag

        if RunFast:
            for qtest in qTestsToRun:
                q=str(qtest)

                def mycleanqtest():
                    if RunSim:
                        RunCleanSTest(q,sim_input_file,mypwd,cleanSetup,extraArg,CleanRunHeadDir,UniqName)
                    else:   
                        RunCleanQTest(q,mypwd,cleanSetup,extraArg,CleanRunHeadDir,UniqName)
                    pass

                def mypatchedqtest():
                    if RunSim:
                        RunPatchedSTest(q,sim_input_file,mypwd,cleanSetup,extraArg)
                    else:
                        RunPatchedQTest(q,mypwd,mysetup,extraArg)
                    pass
            
                mythreads[q+"_clean"]   = threading.Thread(target=mycleanqtest)
                mythreads[q+"_patched"] = threading.Thread(target=mypatchedqtest)
                mythreads[q+"_clean"].start()
                mythreads[q+"_patched"].start()

            for thread in mythreads:
                mythreads[thread].join()


        elif RunUpdated:
            #sorting reverse-alphabetically to make sure 'sim' runs before 'reco' (better way to enforce this?)
            for qtest in sorted(qTestsToRun, reverse=True):
                q=str(qtest)
                def mypatchedqtest():
                    if "s" in q:
                     RunPatchedSTest(q,sim_input_file,mypwd,cleanSetup,extraArg, nosetup=ciMode)
                    else:
                     RunPatchedQTest(q,mypwd,mysetup,extraArg,nosetup=ciMode)
                mythreads[q+"_patched"] = threading.Thread(target=mypatchedqtest)
                mythreads[q+"_patched"].start()
                mythreads[q+"_patched"].join()

        elif RunPatchedOnly:

            for qtest in qTestsToRun:
                q=str(qtest)

                def mypatchedqtest():
                    if RunSim:
                        RunPatchedSTest(q,sim_input_file,mypwd,cleanSetup,extraArg, nosetup=ciMode)
                    else:
                        RunPatchedQTest(q,mypwd,mysetup,extraArg,nosetup=ciMode)
                    pass
            
                mythreads[q+"_patched"] = threading.Thread(target=mypatchedqtest)
                mythreads[q+"_patched"].start()

            for thread in mythreads:
                mythreads[thread].join()

        else :
            for qtest in qTestsToRun:
                q=str(qtest)
                def mycleanqtest():
                    if RunSim:
                        RunCleanSTest(q,sim_input_file,mypwd,cleanSetup,extraArg,CleanRunHeadDir,UniqName)
                    else:   
                        RunCleanQTest(q,mypwd,cleanSetup,extraArg,CleanRunHeadDir,UniqName)
                    pass
                
                def mypatchedqtest():
                    if RunSim:
                        RunPatchedSTest(q,sim_input_file,mypwd,cleanSetup,extraArg)
                    else:   
                        RunPatchedQTest(q,mypwd,mysetup,extraArg)
                    pass

                mythreads[q+"_clean"]   = threading.Thread(target=mycleanqtest)
                mythreads[q+"_patched"] = threading.Thread(target=mypatchedqtest)

                mythreads[q+"_clean"].start()
                mythreads[q+"_patched"].start()
                mythreads[q+"_clean"].join()
                mythreads[q+"_patched"].join()

#Run post-processing tests

        All_Tests_Passed = True

        for qtest in qTestsToRun:                                       
            q=str(qtest)
            logging.info("-----------------------------------------------------"    )
            logging.info("----------- Post-processing of "+q+" Test -----------"    )

            # HAZ: Open question -- is there a cleaner way to do this?
            # HAZ: adding a decorator to `logging` would be nicest (require 0 errors)...
            if not QTestsFailedOrPassed(q,qTestsToRun,CleanRunHeadDir,UniqName,RunPatchedOnly):
                All_Tests_Passed = False
                continue

            if NoCheck:
                logging.info("-----------------------------------------------------"    )
                logging.info("-- Running in ignoreComparisons mode, no tests run --"    )
                continue
                

            if RunSim:
                if not RunFrozenTier0PolicyTest(q,"HITS",10,CleanRunHeadDir,UniqName,RunPatchedOnly):
                    All_Tests_Passed = False
            else:
                if not RunFrozenTier0PolicyTest(q,"ESD",10,CleanRunHeadDir,UniqName,RunPatchedOnly):
                    All_Tests_Passed = False

                if not RunFrozenTier0PolicyTest(q,"AOD",10,CleanRunHeadDir,UniqName,RunPatchedOnly):
                    All_Tests_Passed = False

                if not RunFrozenTier0PolicyTest(q,"RDO",10,CleanRunHeadDir,UniqName):
                    All_Tests_Passed = False

            if RunPatchedOnly: continue  # Performance checks against static references not possible
    
            
            if not RunTest(q,qTestsToRun,"CPU Time"       ,"evtloop_time"    ,"msec/event"  ,4,0.4,CleanRunHeadDir,UniqName):
                All_Tests_Passed = False

            if not RunTest(q,qTestsToRun,"Physical Memory","VmRSS"           ,"kBytes"      ,4,0.2,CleanRunHeadDir,UniqName):
                All_Tests_Passed = False

            if not RunTest(q,qTestsToRun,"Virtual Memory" ,"VmSize"          ,"kBytes"      ,4,0.2,CleanRunHeadDir,UniqName):
                All_Tests_Passed = False

            if not RunTest(q,qTestsToRun,"Memory Leak"    ,"leakperevt_evt11","kBytes/event",7,0.05,CleanRunHeadDir,UniqName):
                All_Tests_Passed = False
            
            if not RunWARNINGSTest(q,qTestsToRun,CleanRunHeadDir,UniqName):
                All_Tests_Passed = False

#           RunHistTest(q,CleanRunHeadDir,UniqName)

        logging.info("-----------------------------------------------------"    )
        logging.info("---------------------- Summary ----------------------"    )
        if All_Tests_Passed:
            logging.info("ALL TESTS: PASSED (0)")
        else:
            logging.error("ALL TESTS: FAILED (-1)")
            sys.exit(-1)

if __name__ == '__main__':
        main()
