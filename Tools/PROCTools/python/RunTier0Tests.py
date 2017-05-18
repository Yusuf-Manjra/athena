#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## RunTier0Tests.py - Brief description of the purpose of this script (Has to be in PROC tools)
# $Id$
import sys
import subprocess
import os
import threading
import time
import uuid
import logging

### Setup global logging
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s %(levelname)-8s %(message)s',
                    datefmt='%m-%d %H:%M',
                    filename='./RunTier0Tests.log',
                    filemode='w')
console = logging.StreamHandler()
console.setLevel(logging.INFO)
formatter = logging.Formatter('%(levelname)-8s %(message)s')
console.setFormatter(formatter)
logging.getLogger('').addHandler(console)

def RunCleanQTest(qtest,pwd,release,extraArg,CleanRunHeadDir,UniqID, doR2A=False):
    q=qtest
    if q == 'q431' and doR2A:
        extraArg += " --steering='doRAWtoALL'"
    if 'CMTPATH' in os.environ:
        if q == 'q431':
            extraArg += " --geometryVersion all:ATLAS-R2-2015-04-00-00 --conditionsTag all:CONDBR2-BLKPA-2016-11 "
        elif q == 'q221':
            extraArg += " --conditionsTag all:OFLCOND-RUN12-SDR-25 "

    logging.info("Running clean in rel "+release+" \"Reco_tf.py --AMI "+q+" "+extraArg+"\"")
    #Check if CleanRunHead directory exists if not exist with a warning 

    CleanDirName="clean_run_"+q+"_"+UniqID

    cmd = "mkdir -p "+CleanRunHeadDir+" ; cd "+CleanRunHeadDir+"; mkdir -p "+CleanDirName+" ; cd "+CleanDirName+" ; source $AtlasSetup/scripts/asetup.sh "+release+" --testarea `pwd` >& /dev/null ; Reco_tf.py --AMI="+q+" "+extraArg+" > "+q+".log 2>&1"
    subprocess.call(cmd,shell=True)
    logging.info("Finished clean \"Reco_tf.py --AMI "+q+"\"")
    pass

def RunPatchedQTest(qtest,pwd,release,theTestArea,extraArg, doR2A=False):
    q=qtest
    if q == 'q431' and doR2A:
        extraArg += " --steering='doRAWtoALL'"
    if 'CMTPATH' in os.environ:
        if q == 'q431':
            extraArg += " --geometryVersion all:ATLAS-R2-2015-04-00-00 --conditionsTag all:CONDBR2-BLKPA-2016-11 "
        elif q == 'q221':
            extraArg += " --conditionsTag all:OFLCOND-RUN12-SDR-25 "

    logging.info("Running patched in rel "+release+" \"Reco_tf.py --AMI "+q+" "+extraArg+"\"")

    if 'WorkDir_DIR' in os.environ:
        cmake_build_dir = (os.environ['WorkDir_DIR'])
        cmd = "cd "+pwd+"; source $AtlasSetup/scripts/asetup.sh "+release+" --testarea "+theTestArea+" >& /dev/null  ; source "+cmake_build_dir+"/setup.sh ; mkdir -p run_"+q+"; cd run_"+q+"; Reco_tf.py --AMI="+q+" "+extraArg+" > "+q+".log 2>&1"
        subprocess.call(cmd,shell=True)
    else :
        cmd = "cd "+pwd+"; source $AtlasSetup/scripts/asetup.sh "+release+" --testarea "+theTestArea+" >& /dev/null  ; mkdir -p run_"+q+"; cd run_"+q+"; Reco_tf.py --AMI="+q+" "+extraArg+" > "+q+".log 2>&1"
        subprocess.call(cmd,shell=True)

    logging.info("Finished patched \"Reco_tf.py --AMI "+q+"\"")
    pass

def pwd():
    Proc = subprocess.Popen('pwd', shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
    Out = (Proc.communicate()[0])[:-1]     
    return Out
    
def GetReleaseSetup():
    
    atlas_base_dir = os.environ['AtlasBaseDir']

    if 'AtlasPatchVersion' in os.environ:
        current_nightly = os.environ['AtlasPatchVersion']
    elif 'AtlasArea' in os.environ:
        current_nightly = os.environ['AtlasArea'].split('/')[-1]
    elif 'AtlasVersion' in os.environ:
        current_nightly = os.environ['AtlasVersion']
        
    if "rel" not in current_nightly:
        platform = os.environ['GEANT4'].split('/')[-1]
        setup="%s,%s"%(platform.replace("-", ","),current_nightly)
    else :
        latest_tag = "latest_copied_release"
        if atlas_base_dir.split('/')[1] == 'cvmfs':
            latest_tag = "latest"
            latest_nightly  = (os.environ['AtlasBaseDir'].split('rel')[:-1])[0]+latest_tag 
        elif atlas_base_dir.split('/')[1] == 'afs':  
            latest_nightly  = (os.environ['AtlasArea'].split('rel')[:-1])[0]+latest_tag 

        latest_nightly  = subprocess.Popen(['/bin/bash', '-c',"ls -l "+latest_nightly], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0].split()[-1]
    
        release  = os.environ['ATLAS_RELEASE_BASE']
        if 'afs' in release.split('/'):
            release = release.split('/')[-1]         
        elif 'cvmfs' in release.split('/'):
            release = release.split('/')[-2]         

        platform = os.environ['GEANT4'].split('/')[-1]

        if current_nightly != latest_nightly:
            logging.info("Please be aware that you are not testing your tags in the latest available nightly, which is "+latest_nightly )
        setup="%s,%s,%s"%(release,platform.replace("-", ","),current_nightly)

    logging.info("Your tags will be tested in environment "+setup)

    return setup

###############################
########### List patch packages
def list_patch_packages():
    if 'CMTPATH' in os.environ:
        if 'TestArea' in os.environ and os.access(os.environ['TestArea'], os.R_OK):
            logging.info("Patch packages in your InstallArea that will be tested are:\n")

            cmd = ['cmt', 'show', 'packages', os.environ['TestArea']]
            cmtProc = subprocess.Popen(cmd, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, bufsize = 1)
            cmtOut = cmtProc.communicate()[0]
            for line in cmtOut.split('\n'):
                try:
                    if line.strip() == '':
                        continue
                    (package, packageVersion, packagePath) = line.split()
                    logging.info('\t%s\n' % (packageVersion))
                except ValueError:
                    logging.info("Warning, unusual output from cmt: %s\n" % line )
    elif 'WorkDir_DIR' in os.environ :                 
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

def QTestsFailedOrPassed(q,qTestsToRun,CleanRunHeadDir,UniqID):
    logging.info("-----------------------------------------------------"  )
    logging.info("Did each step of the "+q+" test complete successfully?" )

    test_dir = CleanRunHeadDir+"/clean_run_"+q+"_"+UniqID

    _Test=True
    for step in qTestsToRun[q]:
        logging.info("")
        cmd = "grep \"successful run\" " + test_dir + "/log."+str(step)
        ref = subprocess.Popen(['/bin/bash', '-c',cmd], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0]

        if "successful run" in ref:
            logging.info(step+" Reference test successful")
        else :
            logging.info(step+" Reference test failed")
            _Test = False

        cmd = "grep \"successful run\" run_"+q+"/log."+str(step)
        test = subprocess.Popen(['/bin/bash', '-c',cmd], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0]

        if "successful run" in test:
            logging.info(step+" Patched test successful")
        else :
            logging.info(step+" Patched test failed")
            _Test = False

    logging.info("")       
    if _Test == True:
        logging.info("All "+q+" athena steps completed successfully")
    else :
        logging.info("One or more "+q+" Athena steps failed. Please investigate the cause.")
        sys.exit()
             

############### Run Frozen Tier0 Policy Test 
def RunFrozenTier0PolicyTest(q,inputFormat,maxEvents,CleanRunHeadDir,UniqID):
    logging.info("---------------------------------------------------------------------------------------" )
    logging.info("Running "+q+" Frozen Tier0 Policy Test on "+inputFormat+" for "+str(maxEvents)+" events" )

    clean_dir = CleanRunHeadDir+"/clean_run_"+q+"_"+UniqID

    comparison_command = "acmd.py diff-root "+clean_dir+"/my"+inputFormat+".pool.root run_"+q+"/my"+inputFormat+".pool.root --error-mode resilient --ignore-leaves  RecoTimingObj_p1_HITStoRDO_timings  RecoTimingObj_p1_RAWtoESD_mems  RecoTimingObj_p1_RAWtoESD_timings  RAWtoESD_mems  RAWtoESD_timings  ESDtoAOD_mems  ESDtoAOD_timings  HITStoRDO_mems  HITStoRDO_timings --entries "+str(maxEvents)+" > run_"+q+"/diff-root-"+q+"."+inputFormat+".log 2>&1"   
    output,error = subprocess.Popen(['/bin/bash', '-c', comparison_command], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

    passed_frozen_tier0_test=False

    f = open("run_"+q+"/diff-root-"+q+"."+inputFormat+".log", 'r')
    for line in f.readlines():

        if "WARNING" in line:
            logging.info(line)
        if "INFO all good." in line    :
            passed_frozen_tier0_test=True
    f.close()

    if passed_frozen_tier0_test:
        logging.info("Passed!")
    else:
        logging.info("Your tag breaks the frozen tier0 policy in test "+q+". See run_"+q+"/diff-root-"+q+"."+inputFormat+".log file for more information.")
    pass

############### Run A Very Simple Test 
def RunTest(q,qTestsToRun,TestName,SearchString,MeasurementUnit,FieldNumber,Threshold,CleanRunHeadDir,UniqID):

    logging.info("-----------------------------------------------------")
    logging.info("Running "+q+" "+TestName+" Test"                      )

    test_dir = CleanRunHeadDir+"/clean_run_"+q+"_"+UniqID
    
    _Test=True 
    for step in qTestsToRun[q]:

        cmd = "grep \""+SearchString+"\" " + test_dir + "/log."+str(step)


        try:
            ref = int(subprocess.Popen(['/bin/bash', '-c',cmd], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0].split()[FieldNumber])
        except:
            logging.info("No data available in "+ test_dir + "/log."+str(step)+" . Job failed.")
            return  

        cmd = "grep \""+SearchString+"\" run_"+q+"/log."+str(step)

        try:
            test = int(subprocess.Popen(['/bin/bash', '-c',cmd], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0].split()[FieldNumber])
        except:
            logging.info("No data available in run_"+q+"/log."+str(step)+" . Job failed.")
            return
 

        if float(ref) != 0 : 
            factor = float(test) / float(ref)

            if  factor > float(1+Threshold) or factor < float(1-Threshold) :
                logging.info(SearchString+" in the",step," step with(out) your tag is",test,"(",ref,") "+MeasurementUnit  )
                logging.info("Your tag changes "+SearchString+" by a factor "+str(factor)                                 )
                logging.info("Is this an expected outcome of your tag(s)?"                                                )
                _Test=False
    
                logging.info(step+" : "+TestName                       )
                logging.info("ref  "+str(ref)+" "+str(MeasurementUnit) )
                logging.info("test "+str(test)+" "+str(MeasurementUnit))

                

    if _Test:
        logging.info("Passed!")
    else :
        logging.info("Failed!" )
  
    pass    

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
            logging.error("Test log file "+test_file+" has "+str(len(warnings_ref) - len(warnings_test))+" less warnings than the reference log file "+ref_file)
            logging.error("The reduction of unnecessary WARNINGs is much appreciated. Is it expected?")
            logging.error("The following warning messages have been removed:")
            for w in wo:
                logging.error(w)
            _Test=True
        else :
            logging.info("Test log file "+test_file+" has the same number of warnings as the reference log file "+ref_file)
            _Test=True


        logging.info("")
            
    if _Test:
        logging.info("Passed!")
    else :
        logging.info("Failed!" )
  
    pass    
  
    pass    


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

    parser=OptionParser(usage="\n ./RunTier0Test.py \n")

    parser.add_option("-e","--extra"     ,type="string"       ,dest="extraArgs"        ,default=""    ,help="define additional args to pass e.g. --preExec 'r2e':'from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup=\"MC_pp_v5\"' ")
    parser.add_option("-a","--r2a"      ,action="store_true"       ,dest="r2a_flag"         ,default=False    ,help="r2a option will run q431 test in r2a mode")
    parser.add_option("-f","--fast"     ,action="store_true"       ,dest="fast_flag"        ,default=False    ,help="fast option will run all q tests simultaneously, such that it will run faster if you have 4 cpu core slots on which to run. Be warned! Only recommended when running on a high performance machine, not lxplus")
    parser.add_option("-c","--cleanDir"     ,type="string"       ,dest="cleanDir"        ,default="/tmp/"    ,help="specify the head directory for running the clean Tier0 tests. The default is /tmp/${USER}")
    parser.add_option("-r","--ref"     ,type="string"        ,dest="ref"   ,default=None    ,help="define a particular reference release")
    parser.add_option("-v","--val"     ,type="string"        ,dest="val"   ,default=None    ,help="define a particular validation release")

    (options,args)=parser.parse_args()

    extraArg = ""
    if options.extraArgs == "MC_pp_v5":
        extraArg = "--preExec 'r2e':'from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.triggerMenuSetup=\"MC_pp_v5\"' "
    else:
        extraArg = options.extraArgs

    RunFast = options.fast_flag
    CleanRunHeadDir=options.cleanDir
    r2aMode = options.r2a_flag

#        tct_ESD = "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/tct/"+latest_nightly+"/"+release+"/"+platform+"/offline/Tier0ChainTests/"+q+"/myESD.pool.root"                                                       
    
    
########### Does the clean run head directory exist?
    if str(CleanRunHeadDir) == "/tmp/":
        myUser = os.environ['USER']
        CleanRunHeadDir = "/tmp/"+str(myUser)

    if os.path.exists(CleanRunHeadDir):
        logging.info("")
        logging.info("The head directory for the output of the clean Tier0 q-tests will be "+CleanRunHeadDir)
        logging.info("")
    else:
        logging.info("")
        logging.info("Exit. Please specify a directory that exists for the argument of the -c or --cleanDir option")
        logging.info("")
        logging.info("RunTier0Tests.py  --cleanDir <ExistingDirectory>")
        logging.info("")
        sys.exit(0)            


########### Is an ATLAS release setup?
    if 'AtlasPatchVersion' not in os.environ and 'AtlasArea' not in os.environ and 'AtlasBaseDir' not in os.environ:
        logging.info("Exit. Please setup the an ATLAS release")
        sys.exit(0)    
    elif 'TestArea' not in os.environ :
        logging.info("Exit. The environment variable ${TESTAREA} is not defined."                                        )
        logging.info("Please re-setup the release with the argument \"here\" in the execution of the asetup command"     )
        logging.info("to specify the TestArea to be the directory from which you setup the release"                      )
        logging.info("E.g. "                                                                                             )
        logging.info("     asetup 20.7.X.Y-VAL,rel_5,AtlasProduction,here"                                               )
        logging.info(                                                                                                    )
        logging.info("or use the --testarea <TestArea> option of asetup to explicitly define the TestArea"               )
        logging.info("E.g. "                                                                                             )
        logging.info("     asetup 20.7.X.Y-VAL,rel_5,AtlasProduction --testarea `pwd`"                                   )
        logging.info(                                                                                                    )
        sys.exit(0)
    elif not os.path.exists(os.environ['TestArea']):
        logging.info("Exit. The path for your TestArea "+os.environ['TestArea']+" does not exist!."        )
    else:
        if 'AtlasPatchVersion' not in os.environ and 'AtlasArea' not in os.environ and 'AtlasBaseDir' in os.environ:
            logging.info("Please be aware that you are running in a base release rather than a Tier0 release, where in general q-tests are not guaranteed to work.")

            
        

########### Define which q-tests to run

        qTestsToRun = {}
        if r2aMode:
            qTestsToRun = { 
            'q221':[ 'HITtoRDO','RAWtoESD','ESDtoAOD','AODtoTAG'],
            'q431':[ 'RAWtoALL']
            }
        else:
            qTestsToRun = { 
            'q221':[ 'HITtoRDO','RAWtoESD','ESDtoAOD','AODtoTAG'],
            'q431':[ 'RAWtoESD','ESDtoAOD','AODtoTAG']
            }          

        
########### Get release info
        mysetup = GetReleaseSetup() 
        mypwd   = pwd()
        myTestArea = os.environ['TestArea']
        cleanSetup = mysetup

########### List the packages in the local InstallArea                                                                                                                                                                                                                           
        if options.ref and options.val:
            cleanSetup = options.ref
            mysetup = options.val
            logging.info("WARNING: You have specified a dedicated release as reference %s and as validation %s release, Your local testarea will not be considered!!!" %(cleanSetup, mysetup))
            logging.info("this option is mainly designed for comparing release versions!!")
        else:
            list_patch_packages()

########### Get unique name for the clean run directory
        UniqName = str(uuid.uuid4())

########### Define and run jobs
        mythreads={}
#        mysetup=mysetup+",builds"
        logging.info("------------------ Run Athena q-test jobs---------------"                )


        if RunFast:
            for qtest in qTestsToRun:
                q=str(qtest)

                def mycleanqtest():
                    RunCleanQTest(q,mypwd,cleanSetup,extraArg,CleanRunHeadDir,UniqName, doR2A=r2aMode)
                    pass
    
                def mypatchedqtest():
                    RunPatchedQTest(q,mypwd,mysetup,myTestArea,extraArg, doR2A=r2aMode)
                    pass
            
                mythreads[q+"_clean"]   = threading.Thread(target=mycleanqtest)
                mythreads[q+"_patched"] = threading.Thread(target=mypatchedqtest)
                mythreads[q+"_clean"].start()
                mythreads[q+"_patched"].start()

            for thread in mythreads:
                mythreads[thread].join()

        else :
            for qtest in qTestsToRun:
                q=str(qtest)

                def mycleanqtest():
                    RunCleanQTest(q,mypwd,cleanSetup,extraArg,CleanRunHeadDir,UniqName)
                    pass
                
                def mypatchedqtest():
                    RunPatchedQTest(q,mypwd,mysetup,myTestArea,extraArg)
                    pass

                mythreads[q+"_clean"]   = threading.Thread(target=mycleanqtest)
                mythreads[q+"_patched"] = threading.Thread(target=mypatchedqtest)

                mythreads[q+"_clean"].start()
                mythreads[q+"_patched"].start()
                mythreads[q+"_clean"].join()
                mythreads[q+"_patched"].join()

#Run post-processing tests

        for qtest in qTestsToRun:                                       
            q=str(qtest)
            logging.info("-----------------------------------------------------"    )
            logging.info("----------- Post-processing of "+q+" Test -----------"    )

            QTestsFailedOrPassed(q,qTestsToRun,CleanRunHeadDir,UniqName)

            if 'q221' in q: 
                RunFrozenTier0PolicyTest(q,"RDO",10,CleanRunHeadDir,UniqName)
            
            RunFrozenTier0PolicyTest(q,"ESD",10,CleanRunHeadDir,UniqName)

            RunFrozenTier0PolicyTest(q,"AOD",20,CleanRunHeadDir,UniqName)

            RunTest(q,qTestsToRun,"CPU Time"       ,"evtloop_time"    ,"sec/event"   ,4,0.4,CleanRunHeadDir,UniqName)

            RunTest(q,qTestsToRun,"Physical Memory","VmRSS"           ,"kBytes"      ,4,0.2,CleanRunHeadDir,UniqName)

            RunTest(q,qTestsToRun,"Virtual Memory" ,"VmSize"          ,"kBytes"      ,4,0.2,CleanRunHeadDir,UniqName)

            RunTest(q,qTestsToRun,"Memory Leak"    ,"leakperevt_evt11","kBytes/event",7,0.05,CleanRunHeadDir,UniqName)
            
            RunWARNINGSTest(q,qTestsToRun,CleanRunHeadDir,UniqName)

#           RunHistTest(q,CleanRunHeadDir,UniqName)

if __name__ == '__main__':
        main()

