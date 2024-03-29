#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
"""Functionality core of the Generate_tf transform"""

##==============================================================
## Basic configuration
##==============================================================

## Create sequences for generators, clean-up algs, filters and analyses
## and import standard framework objects with standard local scope names
from __future__ import print_function
from __future__ import division

from future import standard_library
standard_library.install_aliases()

import ast
import os, re, string, subprocess
import AthenaCommon.AlgSequence as acas
import AthenaCommon.AppMgr as acam
from AthenaCommon.AthenaCommonFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AthenaCommonFlags import jobproperties

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
acam.athMasterSeq += xAODMaker__EventInfoCnvAlg(xAODKey="TMPEvtInfo")

theApp = acam.theApp
acam.athMasterSeq += acas.AlgSequence("EvgenGenSeq")
genSeq = acam.athMasterSeq.EvgenGenSeq
acam.athMasterSeq += acas.AlgSequence("EvgenFixSeq")
fixSeq = acam.athMasterSeq.EvgenFixSeq
acam.athMasterSeq += acas.AlgSequence("EvgenPreFilterSeq")
prefiltSeq = acam.athMasterSeq.EvgenPreFilterSeq
acam.athMasterSeq += acas.AlgSequence("EvgenTestSeq")
testSeq = acam.athMasterSeq.EvgenTestSeq
## NOTE: LogicalExpressionFilter is an algorithm, not a sequence
from EvgenProdTools.LogicalExpressionFilter import LogicalExpressionFilter
acam.athMasterSeq += LogicalExpressionFilter("EvgenFilterSeq")
filtSeq = acam.athMasterSeq.EvgenFilterSeq
topSeq = acas.AlgSequence()
anaSeq = topSeq
topSeq += acas.AlgSequence("EvgenPostSeq")
postSeq = topSeq.EvgenPostSeq
##==============================================================
## Configure standard Athena services
##==============================================================

## Special setup for event generation
include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")
include("PartPropSvc/PartPropSvc.py")

## Run performance monitoring (memory logging)
from PerfMonComps.PerfMonFlags import jobproperties as perfmonjp
perfmonjp.PerfMonFlags.doMonitoring = True
perfmonjp.PerfMonFlags.doSemiDetailedMonitoring = True

from RngComps.RngCompsConf import AthRNGSvc
svcMgr += AthRNGSvc()

## Jobs should stop if an include fails.
jobproperties.AthenaCommonFlags.AllowIgnoreConfigError = False

## Set up a standard logger
from AthenaCommon.Logging import logging
evgenLog = logging.getLogger('Gen_tf')

##==============================================================
## Run arg handling
##==============================================================

## Announce arg checking
evgenLog.debug("****************** CHECKING EVENT GENERATION ARGS *****************")
evgenLog.debug(str(runArgs))
evgenLog.info ("****************** CHECKING EVENT GENERATION ARGS *****************")
if hasattr(runArgs, "runNumber"):
   evgenLog.warning("##########################################################################" )         
   evgenLog.warning("runNumber - no longer a valid argument, do not use it ! " )         
   evgenLog.warning("##########################################################################")

if hasattr(runArgs, "inputGenConfFile"):
   raise RuntimeError("inputGenConfFile is invalid !! Gridpacks and config. files/links to be put into DSID directory ")

# TODO: Allow generation without writing an output file (if outputEVNTFile is None)?
if not hasattr(runArgs, "ecmEnergy"):
    raise RuntimeError("No center of mass energy provided.")
else:
    evgenLog.info(' ecmEnergy = ' + str(runArgs.ecmEnergy) )
##==============================================================
## Configure standard Athena and evgen services
##==============================================================

## Announce start of job configuration
evgenLog.debug("****************** CONFIGURING MATRIX ELEMENT GENERATION *****************")
evgenLog.info("****************** CONFIGURING MATRIX ELEMENT GENERATION *****************")

## Functions for operating on generator names
## NOTE: evgenConfig, topSeq, svcMgr, theApp, etc. should NOT be explicitly re-imported in JOs
from EvgenJobTransforms.EvgenConfig import evgenConfig
from EvgenJobTransforms.EvgenConfig import gens_known, gen_lhef, gens_lhef, gen_sortkey, gens_testhepmc, gens_notune, gen_require_steering

## Configure the event counting (AFTER all filters)
# TODO: Rewrite in Python?
from EvgenProdTools.EvgenProdToolsConf import CountHepMC
if (runArgs.firstEvent <= 0):
   evgenLog.warning("Run argument firstEvent should be > 0")

svcMgr.EventSelector.FirstEvent = runArgs.firstEvent
theApp.EvtMax = -1

#evgenConfig.nEventsPerJob = 1
if not hasattr(postSeq, "CountHepMC"):
   postSeq += CountHepMC(InputEventInfo="TMPEvtInfo",
                         OutputEventInfo="EventInfo",
                         mcEventWeightsKey="")

postSeq.CountHepMC.FirstEvent = runArgs.firstEvent
postSeq.CountHepMC.CorrectHepMC = True
postSeq.CountHepMC.CorrectEventID = True


##==============================================================
## Pre- and main config parsing
##==============================================================

## Announce JO loading
evgenLog.debug("****************** LOADING PRE-INCLUDES AND JOB CONFIG *****************")
evgenLog.info("****************** LOADING PRE-INCLUDES AND JOB CONFIG *****************")

## Pre-include
if hasattr(runArgs, "preInclude"):
    for fragment in runArgs.preInclude:
        include(fragment)

## Pre-exec
if hasattr(runArgs, "preExec"):
    evgenLog.info("Transform pre-exec")
    for cmd in runArgs.preExec:
        evgenLog.info(cmd)
        exec(cmd)

def get_immediate_subdirectories(a_dir):
            return [name for name in os.listdir(a_dir)
                    if os.path.isdir(os.path.join(a_dir, name))]

        
# TODO: Explain!!!
def OutputTXTFile():
    outputTXTFile = None
    if hasattr(runArgs,"outputTXTFile"): outputTXTFile=runArgs.outputTXTFile
    return outputTXTFile

## Main job option include
## Only permit one jobConfig argument for evgen: does more than one _ever_ make sense?

if len(runArgs.jobConfig) != 1:
    evgenLog.info("runArgs.jobConfig = " +  runArgs.jobConfig)
    evgenLog.error("You must supply one and only one jobConfig file argument. It has to start from mc. and end with .py")
    sys.exit(1)

printfunc ("Using JOBOPTSEARCHPATH (as seen in skeleton) = '%s'" % (os.environ["JOBOPTSEARCHPATH"]))
FIRST_DIR = (os.environ['JOBOPTSEARCHPATH']).split(":")[0]

dsid_param = runArgs.jobConfig[0]
evgenLog.info("dsid_param = " + dsid_param)
dsid = os.path.basename(dsid_param)
evgenLog.info("dsid = " + dsid)
jofiles = [f for f in os.listdir(FIRST_DIR) if (f.startswith('mc') and f.endswith('.py'))]
## Only permit one JO file in each dsid folder
if len(jofiles) !=1:
    evgenLog.info("runArgs.jobConfig wrong " + runArgs.jobConfig)
    evgenLog.error("You must supply one and only one jobOption file in DSID directory. It has to start with mc. and end with .py")
    sys.exit(1)
jofile = jofiles[0]
joparts = (os.path.basename(jofile)).split(".")
## Perform some consistency checks if this appears to be an "official" production JO
officialJO = False
if joparts[0].startswith("mc") and all(c in string.digits for c in joparts[0][2:]):
    officialJO = True
    ## Check that there are exactly 4 name parts separated by '.': MCxx, DSID, physicsShort, .py
    if len(joparts) != 3:
        evgenLog.error(jofile + " name format is wrong: must be of the form MC<xx>.<physicsShort>.py: please rename.")
        sys.exit(1)

    ## Check the length limit on the physicsShort portion of the filename
    jo_physshortpart = joparts[1]
    if len(jo_physshortpart) > 50:
        evgenLog.error(jofile + " contains a physicsShort field of more than 50 characters: please rename.")
        sys.exit(1)
    ## There must be at least 2 physicsShort sub-parts separated by '_': gens, (tune)+PDF, and process
    jo_physshortparts = jo_physshortpart.split("_")
    if len(jo_physshortparts) < 2:
        evgenLog.error(jofile + " has too few physicsShort fields separated by '_': should contain <generators>(_<tune+PDF_if_available>)_<process>. Please rename.")
        sys.exit(1)
    ## NOTE: a further check on physicsShort consistency is done below, after fragment loading
    check_jofiles="/cvmfs/atlas.cern.ch/repo/sw/Generators/MC16JobOptions/scripts/check_jo_consistency.py"
    if os.path.exists(check_jofiles):
        evgenLog.info("Checking offical JO file name consistency")
        include(check_jofiles)
        check_naming(os.path.basename(jofile))
    else:
        evgenLog.error("check_jo_consistency.py not found")
        sys.exit(1)
## Include the JO fragment
include(jofile)
include("EvgenJobTransforms/LHEonly.py")

##==============================================================
## Config validation and propagation to services, generators, etc.
##==============================================================

## Announce start of JO checking
evgenLog.debug("****************** CHECKING EVGEN CONFIGURATION *****************")
evgenLog.info("****************** CHECKING EVGEN CONFIGURATION *****************")

## Print out options
for opt in str(evgenConfig).split(os.linesep):
    evgenLog.info(opt)
evgenLog.info(".transform =                   Gen_tf")      

## Sort and check generator name / JO name consistency
##
## Check that the common fragments are not obsolete:
if evgenConfig.obsolete:
    evgenLog.error("JOs or icludes are obsolete, please check them")
    sys.exit(1)
## Check that the generators list is not empty:
if not evgenConfig.generators:
    evgenLog.error("No entries in evgenConfig.generators: invalid configuration, please check your JO")
    sys.exit(1)
## Check for duplicates:
if len(evgenConfig.generators) > len(set(evgenConfig.generators)):
    evgenLog.error("Duplicate entries in evgenConfig.generators: invalid configuration, please check your JO")
    sys.exit(1)
## Sort the list of generator names into standard form
gennames = sorted(evgenConfig.generators, key=gen_sortkey)
## Check that the actual generators, tune, and main PDF are consistent with the JO name
if joparts[0].startswith("MC"): #< if this is an "official" JO
    genpart = jo_physshortparts[0]
    expectedgenpart = ''.join(gennames)
    ## We want to record that HERWIG was used in metadata, but in the JO naming we just use a "Herwig" label
    expectedgenpart = expectedgenpart.replace("HerwigJimmy", "Herwig")
    def _norm(s):
        # TODO: add EvtGen to this normalization for MC14?
        return s.replace("Photospp", "").replace("Photos", "").replace("TauolaPP", "").replace("Tauolapp", "").replace("Tauola", "")
    def _norm2(s):
        return s.replace("Py", "Pythia").replace("MG","MadGraph").replace("Ph","Powheg").replace("Hpp","Herwigpp").replace("H7","Herwig7").replace("Sh","Sherpa").replace("Ag","Alpgen").replace("EG","EvtGen").replace("PG","ParticleGun").replace("Gva","Geneva")
        
    def _short2(s):
         return s.replace("Pythia","Py").replace("MadGraph","MG").replace("Powheg","Ph").replace("Herwigpp","Hpp").replace("Herwig7","H7").replace("Sherpa","Sh").replace("Alpgen","Ag").replace("EvtGen","EG").replace("PG","ParticleGun").replace("Geneva","Gva")
     

    if genpart != _norm(expectedgenpart)  and _norm2(genpart) != _norm(expectedgenpart):
        evgenLog.error("Expected first part of JO name to be '%s' or '%s', but found '%s'" % (_norm(expectedgenpart), _norm(_short2(expectedgenpart)), genpart))
        evgenLog.error("gennames '%s' " %(expectedgenpart))
        sys.exit(1)

    del _norm
    ## Check if the tune/PDF part is needed, and if so whether it's present
    if not gens_notune(gennames) and len(jo_physshortparts) < 3:
        evgenLog.error(jofile + " with generators " + expectedgenpart +
                       " has too few physicsShort fields separated by '_'." +
                       " It should contain <generators>_<tune+PDF_<process>. Please rename.")
        sys.exit(1)

## Check the "--steering=afterburn" command line argument has been set if EvtGen is in the JO name
# Dont't have access to steering flag so check it's effect on output files
if gen_require_steering(gennames):
    if hasattr(runArgs, "outputEVNTFile") and not hasattr(runArgs, "outputEVNT_PreFile"):
        raise RuntimeError("'EvtGen' found in job options name, please set '--steering=afterburn'")


## Check that the evgenConfig.nEventsPerJob setting is acceptable
## nEventsPerJob defines the production event sizes and must be sufficiently "round"    
rounding = 0
if hasattr(runArgs,'inputGeneratorFile') and ',' in runArgs.inputGeneratorFile:   multiInput = runArgs.inputGeneratorFile.count(',')+1
else:
   multiInput = 0

# check if default nEventsPerJob used
if not evgenConfig.nEventsPerJob:
    evgenLog.info('#############################################################')
    evgenLog.info(' !!!! no nEventsPerJob set !!!  The default 10000 used. !!! ')
    evgenLog.info('#############################################################')
else:
    evgenLog.info(' nEventsPerJob set to ' + str(evgenConfig.nEventsPerJob)  )
    
if evgenConfig.minevents > 0 :
    raise RuntimeError("evgenConfig.minevents is obsolete and should be removed from the JOs")
   
if evgenConfig.nEventsPerJob < 1:
    raise RunTimeError("evgenConfig.nEventsPerJob must be at least 1")
else: 
    evgenLog.info("evgenConfig.nEventsPerJob = {}, but only {} (dummy) event(s) will be generated by Pythia8 for lhe-only production".format(evgenConfig.nEventsPerJob, postSeq.CountHepMC.RequestedOutput))
### NB: postSeq.CountHepMC.RequestedOutput is set to 1 in LHEonly.py

## Check that the keywords list is not empty:
if not evgenConfig.keywords:
    evgenLog.warning("No entries in evgenConfig.keywords: invalid configuration, please check your JO !!")    

## Check that the keywords are in the list of allowed words (and exit if processing an official JO)
if evgenConfig.keywords:
    ## Get the allowed keywords file from the JO package if possibe
    # TODO: Make the package name configurable
    kwfile = "evgenkeywords.txt"
    kwpath = None
    for p in os.environ["DATAPATH"].split(":"):
        kwpath = os.path.join(p, kwfile)
        if os.path.exists(kwpath):
            break
        kwpath = None
    ## Load the allowed keywords from the file
    allowed_keywords = []
    if kwpath:
        evgenLog.info("evgenkeywords = "+kwpath)
        kwf = open(kwpath, "r")
        for l in kwf:
            allowed_keywords += l.strip().lower().split()
        ## Check the JO keywords against the allowed ones
        evil_keywords = []
        for k in evgenConfig.keywords:
            if k.lower() not in allowed_keywords:
                evil_keywords.append(k)
        if evil_keywords:
            msg = "evgenConfig.keywords contains non-standard keywords: %s. " % ", ".join(evil_keywords)
            msg += "Please check the allowed keywords list and fix."
            evgenLog.error(msg)
            if officialJO:
                sys.exit(1)
    else:
        evgenLog.warning("Could not find evgenkeywords.txt file %s in $JOBOPTSEARCHPATH" % kwfile)

## Check that the categories list is not empty:
if not evgenConfig.categories:
    evgenLog.warning("No entries in evgenConfig.categories: invalid configuration, please check your JO !!")        

## Check that the L1 and L2 keywords pairs are in the list of allowed words pairs (and exit if processing an official JO)
if evgenConfig.categories:
    ## Get the allowed categories file from the JO package if possibe
    # TODO: Make the package name configurable
    lkwfile = "CategoryList.txt"
    lkwpath = None
    for p in os.environ["DATAPATH"].split(":"):
        lkwpath = os.path.join(p, lkwfile)
        if os.path.exists(lkwpath):
            break
        lkwpath = None
## Load the allowed categories names from the file
    allowed_cat = []
    if lkwpath:
        with open(lkwpath, 'r') as catlist:
            for line in catlist:
               allowed_list = ast.literal_eval(line)
               allowed_cat.append(allowed_list)

        ## Check the JO categories against the allowed ones
        bad_cat =[]
        it = iter(evgenConfig.categories)
        for x in it:
           l1 = x
           l2 = next(it)
           if "L1:" in l2 and "L2:" in l1:
               l1, l2 = l2, l1
           printfunc("first",l1,"second",l2)
           bad_cat.extend([l1, l2])
           for a1,a2 in allowed_cat:
               if l1.strip().lower()==a1.strip().lower() and l2.strip().lower()==a2.strip().lower():
                 bad_cat=[]
           if bad_cat:
               msg = "evgenConfig.categories contains non-standard category: %s. " % ", ".join(bad_cat)
               msg += "Please check the allowed categories list and fix."
               evgenLog.error(msg)
               if officialJO:
                   sys.exit(1)
    else:
        evgenLog.warning("Could not find CategoryList.txt file %s in DATAPATH" % lkwfile)

## Set the run numbers
dsid = os.path.basename(runArgs.jobConfig[0])
if not dsid.isdigit():
    dsid = "999999"
svcMgr.EventSelector.RunNumber = int(dsid)

## Include information about generators in metadata
gennamesvers=[]
for item in gennames:
       genera = item.upper()
       generat = genera+"VER"
       if (generat in os.environ):
           gennamesvers.append(item+"(v."+os.environ[generat]+")")
       else:
           gennamesvers.append(item)

import EventInfoMgt.EventInfoMgtInit
svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"hepmc_version":"HepMC" + str(os.environ['HEPMCVER'])})
svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"mc_channel_number":str(dsid)})
svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"lhefGenerator": '+'.join( filter( gen_lhef, gennames ) ) })
svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"generators": '+'.join(gennamesvers)})
svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"evgenProcess": evgenConfig.process})
svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"evgenTune": evgenConfig.tune})
if hasattr( evgenConfig, "hardPDF" ) : svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"hardPDF": evgenConfig.hardPDF})
if hasattr( evgenConfig, "softPDF" ) : svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"softPDF": evgenConfig.softPDF})
if hasattr( runArgs, "randomSeed") :  svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"randomSeed": str(runArgs.randomSeed)})
# Set AMITag in in-file metadata
from PyUtils import AMITagHelper
AMITagHelper.SetAMITag(runArgs=runArgs)

## Handle beam info
svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"beam_energy": str(int(runArgs.ecmEnergy*Units.GeV/2.0))})
svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"beam_type": 'collisions'})

## Propagate energy argument to the generators
# TODO: Standardise energy setting in the GenModule interface
include("EvgenJobTransforms/Generate_ecmenergies.py")

# Propagate DSID and seed to the generators
include("EvgenJobTransforms/Generate_dsid_ranseed.py")

## Propagate debug output level requirement to generators
if (hasattr( runArgs, "VERBOSE") and runArgs.VERBOSE ) or (hasattr( runArgs, "loglevel") and runArgs.loglevel == "DEBUG") or (hasattr( runArgs, "loglevel") and runArgs.loglevel == "VERBOSE"):
   include("EvgenJobTransforms/Generate_debug_level.py")

##=============================================================
## Check release number
##=============================================================
# Function to check blacklist (from Spyros'es logParser.py)
def checkBlackList(relFlavour,cache,generatorName) :
    isError = None
    with open('/cvmfs/atlas.cern.ch/repo/sw/Generators/MC16JobOptions/common/BlackList_caches.txt') as bfile:
        for line in bfile.readlines():
            if not line.strip():
                continue
            # Blacklisted release flavours
            badRelFlav=line.split(',')[0].strip()
            # Blacklisted caches
            badCache=line.split(',')[1].strip()
            # Blacklisted generators
            badGens=line.split(',')[2].strip()

            used_gens = ','.join(generatorName)
            #Match Generator and release type e.g. AtlasProduction, MCProd
            if relFlavour==badRelFlav and cache==badCache and re.search(badGens,used_gens) is not None:
                if badGens=="": badGens="all generators"
                isError=relFlavour+","+cache+" is blacklisted for " + badGens
                return isError  
    return isError

def checkPurpleList(relFlavour,cache,generatorName) :
    isError = None
    with open('/cvmfs/atlas.cern.ch/repo/sw/Generators/MC16JobOptions/common/PurpleList_generators.txt') as bfile:
        for line in bfile.readlines():
            if not line.strip():
                continue
            # Purple-listed release flavours  
            purpleRelFlav=line.split(',')[0].strip()
            # Purple-listed caches
            purpleCache=line.split(',')[1].strip()
            # Purple-listed generators
            purpleGens=line.split(',')[2].strip()
            # Purple-listed process
            purpleProcess=line.split(',')[3].strip()
    
            used_gens = ','.join(generatorName)
            #Match Generator and release type e.g. AtlasProduction, MCProd
            if relFlavour==purpleRelFlav and cache==purpleCache and re.search(purpleGens,used_gens) is not None:
                isError=relFlavour+","+cache+" is blacklisted for " + purpleGens + " if it uses " + purpleProcess 
                return isError
    return isError

## Announce start of JO checkingrelease nimber checking
evgenLog.debug("****************** CHECKING RELEASE IS NOT BLACKLISTED *****************")
rel = os.popen("echo $AtlasVersion").read()
rel = rel.strip()
errorBL = checkBlackList("AthGeneration",rel,gennames)
if (errorBL):
  if (hasattr( runArgs, "ignoreBlackList") and runArgs.ignoreBlackList): 
      evgenLog.warning("This run is blacklisted for this generator, please use a different one for production !! "+ errorBL )
  else:
      raise RuntimeError("This run is blacklisted for this generator, please use a different one !! "+ errorBL)   
      
errorPL = checkPurpleList("AthGeneration",rel,gennames)
if (errorPL):
   evgenLog.warning("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")  
   evgenLog.warning("!!! WARNING  !!! "+ errorPL )
   evgenLog.warning("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")

## Add special config option (extended model info for BSM scenarios)
svcMgr.TagInfoMgr.ExtraTagValuePairs.update({"specialConfiguration": evgenConfig.specialConfig})


## Process random seed arg and pass to generators
#include("EvgenJobTransforms/Generate_randomseeds.py")


##==============================================================
## Handling of a post-include/exec args at the end of standard configuration
##==============================================================

if hasattr(runArgs, "postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

if hasattr(runArgs, "postExec"):
    evgenLog.info("Transform post-exec")
    for cmd in runArgs.postExec:
        evgenLog.info(cmd)
        exec(cmd)
        
##==============================================================
## Show the algorithm sequences and algs now that config is complete
##==============================================================
acas.dumpMasterSequence()


##==============================================================
## Input file arg handling
##==============================================================

## Announce start of input file handling
evgenLog.debug("****************** HANDLING EVGEN INPUT FILES *****************")
printfunc("****************** HANDLING EVGEN INPUT FILES *****************")
## Dat files
datFile = None
if "McAtNlo" in evgenConfig.generators and "Herwig" in evgenConfig.generators:
    datFile = "inparmMcAtNlo.dat"
elif "Alpgen" in evgenConfig.generators:
    datFile = "inparmAlpGen.dat"
elif "Protos" in evgenConfig.generators:
    datFile = "protos.dat"
elif "ProtosLHEF" in evgenConfig.generators:
    datFile = "protoslhef.dat"
elif "AcerMC" in evgenConfig.generators:
    datFile = "inparmAcerMC.dat"
elif "CompHep" in evgenConfig.generators:
    datFile = "inparmCompHep.dat"

## Events files
if "Alpgen" in evgenConfig.generators:
   eventsFile = "alpgen.unw_events"
elif "Protos" in evgenConfig.generators:
   eventsFile = "protos.events"
elif "ProtosLHEF" in evgenConfig.generators:
   eventsFile = "protoslhef.events"
elif "BeamHaloGenerator" in evgenConfig.generators:
   eventsFile = "beamhalogen.events"
elif "HepMCAscii" in evgenConfig.generators:
   eventsFile = "events.hepmc"
elif "ReadMcAscii" in evgenConfig.generators:
    eventsFile = "events.hepmc"
elif gens_lhef(evgenConfig.generators):
   #eventsFile = outputTXTFile
   eventsFile = "events.lhe"


## Helper functions for input file handling
def find_unique_file(pattern):
    "Return a matching file, provided it is unique"
    import glob
    files = glob.glob(pattern)
    ## Check that there is exactly 1 match
    if not files:
        raise RuntimeError("No '%s' file found" % pattern)
    elif len(files) > 1:
        raise RuntimeError("More than one '%s' file found" % pattern)
    return files[0]

# This function merges a list of input LHE file to make one outputFile.  The header is taken from the first
# file, but the number of events is updated to equal the total number of events in all the input files
def merge_lhe_files(listOfFiles,outputFile):
    if(os.path.exists(outputFile)):
      printfunc ("outputFile ",outputFile," already exists.  Will rename to ",outputFile,".OLD")
      os.rename(outputFile,outputFile+".OLD")
    output = open(outputFile,'w')
    holdHeader = ""
    nevents=0
    for file in listOfFiles:
       cmd = "grep /event "+file+" | wc -l"
       nevents+=int(subprocess.check_output(cmd,stderr=subprocess.STDOUT,shell=True))

    for file in listOfFiles:
       inHeader = True
       header = ""
       printfunc ("*** Starting file ",file)
       for line in open(file,"r"):
##        Reading first event signals that we are done with all the header information
##        Using this approach means the script will properly handle any metadata stored
##        at the beginning of the file.  Note:  aside from the number of events, no metadata
##        is updated after the first header is read (eg the random number seed recorded will be
##        that of the first file.
          if("<event" in line and inHeader):
             inHeader = False
             if(len(holdHeader)<1):
                holdHeader = header
                output.write(header)
             output.write(line)
##        each input file ends with "</LesHouchesEvents>".  We don't want to write this out until all
##        the files have been read.  The elif below writes out all the events.
          elif(not inHeader and not ("</LesHouchesEvents>" in line)):
              output.write(line)
          if(inHeader):
##           Format for storing number of events different in MG and Powheg
             if("nevents" in line):
##              MG5 format is "n = nevents"
                tmp = line.split("=")
                line = line.replace(tmp[0],str(nevents))
             elif("numevts" in line):
##              Powheg format is numevts n
                tmp = line.split(" ")
                nnn = str(nevents)
                line = line.replace(tmp[1],nnn)
             header+=line
    output.write("</LesHouchesEvents>\n")
    output.close()

def mk_symlink(srcfile, dstfile):
    "Make a symlink safely"
    if dstfile:
        if os.path.exists(dstfile) and not os.path.samefile(dstfile, srcfile):
            os.remove(dstfile)
        if not os.path.exists(dstfile):
            evgenLog.info("Symlinking %s to %s" % (srcfile, dstfile))
            os.symlink(srcfile, dstfile)
        else:
            evgenLog.debug("Symlinking: %s is already the same as %s" % (dstfile, srcfile))

## Find and symlink dat and event files, so they are available via the name expected by the generator
if eventsFile or datFile:
    if not hasattr(runArgs, "inputGeneratorFile") or runArgs.inputGeneratorFile == "NONE":
        raise RuntimeError("%s needs input file (argument inputGeneratorFile)" % runArgs.jobConfig)
    if evgenConfig.inputfilecheck and not re.search(evgenConfig.inputfilecheck, runArgs.inputGeneratorFile):
        raise RuntimeError("inputGeneratorFile=%s is incompatible with inputfilecheck '%s' in %s" %
                           (runArgs.inputGeneratorFile, evgenConfig.inputfilecheck, runArgs.jobConfig))
    if datFile:
      if ".tar" in os.path.basename(runArgs.inputGeneratorFile):
        inputroot = os.path.basename(runArgs.inputGeneratorFile).split(".tar.")[0]
      else:
        inputroot = os.path.basename(runArgs.inputGeneratorFile).split("._")[0]

      realDatFile = find_unique_file('*%s*.dat' % inputroot)
      mk_symlink(realDatFile, datFile)
    if eventsFile:
        myinputfiles = runArgs.inputGeneratorFile
        genInputFiles = myinputfiles.split(',')
        numberOfFiles = len(genInputFiles)
        # if there is a single file, make a symlink.  If multiple files, merge them into one output eventsFile
        if(numberOfFiles<2):
           if ".tar" in os.path.basename(runArgs.inputGeneratorFile):
             inputroot = os.path.basename(runArgs.inputGeneratorFile).split(".tar.")[0]
           else:
             inputroot = os.path.basename(runArgs.inputGeneratorFile).split("._")[0]

           if "events" in inputroot :
               inputroot = inputroot.replace(".events","")
           realEventsFile = find_unique_file('*%s.*ev*ts' % inputroot)
           mk_symlink(realEventsFile, eventsFile)
        else:
           allFiles = []
           for file in genInputFiles:
#             Since we can have multiple files from the same task, inputroot must include more of the filename
#             to make it unique
              if ".tar" in os.path.basename(runArgs.inputGeneratorFile):
                inputroot = os.path.basename(runArgs.inputGeneratorFile).split(".tar.")[0]
              else:
                input0 = os.path.basename(file).split("._")[0]
                input1 = (os.path.basename(file).split("._")[1]).split(".")[0]
                inputroot = input0+"._"+input1
              realEventsFile = find_unique_file('*%s.*ev*ts' % inputroot)
#             The only input format where merging is permitted is LHE
              with open(realEventsFile, 'r') as f:
                 first_line = f.readline()
                 if(not ("LesHouche" in first_line)):
                    raise RuntimeError("%s is NOT a LesHouche file" % realEventsFile)
                 allFiles.append(realEventsFile)
           merge_lhe_files(allFiles,eventsFile)

else:
    if hasattr(runArgs, "inputGeneratorFile") and runArgs.inputGeneratorFile != "NONE":
        raise RuntimeError("inputGeneratorFile arg specified for %s, but generators %s do not require an input file" %
                           (runArgs.jobConfig, str(gennames)))
#    if evgenConfig.inputfilecheck:
#        raise RuntimeError("evgenConfig.inputfilecheck specified in %s, but generators %s do not require an input file" %
#                           (runArgs.jobConfig, str(gennames)))

## Do the aux-file copying
if evgenConfig.auxfiles:
    from PyJobTransformsCore.trfutil import get_files
    get_files(evgenConfig.auxfiles, keepDir=False, errorIfNotFound=True)
    
##==============================================================
## Write out metadata for reporting to AMI
##==============================================================

def _checkattr(attr, required=False):
    if not hasattr(evgenConfig, attr) or not getattr(evgenConfig, attr):
        msg = "evgenConfig attribute '%s' not found." % attr
        if required:
            raise RuntimeError("Required " + msg)
        return False
    return True
# counting the number of events in LHE output
count_ev = 0
with open(eventsFile) as f:
    for line in f:
       count_ev += line.count('/event')
    
evgenLog.info('Requested output events = '+str(count_ev))
printfunc("MetaData: %s = %s" % ("Number of produced LHE events ", count_ev))

if _checkattr("description", required=True):
    msg = evgenConfig.description
    if _checkattr("notes"):
        msg += " " + evgenConfig.notes
    printfunc("MetaData: %s = %s" % ("physicsComment", msg))
if _checkattr("generators", required=True):
    printfunc ("MetaData: %s = %s" % ("generatorName", "+".join(gennamesvers)))    
if _checkattr("process"):
    printfunc ("MetaData: %s = %s" % ("physicsProcess", evgenConfig.process))
if _checkattr("tune"):
    printfunc ("MetaData: %s = %s" % ("generatorTune", evgenConfig.tune))
if _checkattr("hardPDF"):
    printfunc ("MetaData: %s = %s" % ("hardPDF", evgenConfig.hardPDF))
if _checkattr("softPDF"):
    printfunc ("MetaData: %s = %s" % ("softPDF", evgenConfig.softPDF))
if _checkattr("nEventsPerJob"):
    printfunc ("MetaData: %s = %s" % ("nEventsPerJob", evgenConfig.nEventsPerJob))
if _checkattr("keywords"):
    printfunc ("MetaData: %s = %s" % ("keywords", ", ".join(evgenConfig.keywords).lower() ))
if _checkattr("categories"):
    printfunc ( ", " + ", ".join(evgenConfig.categories))
else:
    printfunc (" ")

#if _checkattr("categories"):  # will be uncommented when categories included into metadata
#    printfunc "MetaData: %s = %s" % ("categories", ", ".join(evgenConfig.categories))
if _checkattr("specialConfig"):
    printfunc ("MetaData: %s = %s" % ("specialConfig", evgenConfig.specialConfig))
# TODO: Require that a contact / JO author is always set
if _checkattr("contact"):
    printfunc ("MetaData: %s = %s" % ("contactPhysicist", ", ".join(evgenConfig.contact)))
#if _checkattr( "randomSeed") :    # comment out for the time being
printfunc ("MetaData: %s = %s" % ("randomSeed", str(runArgs.randomSeed)))

    
    

# Output list of generator filters used
filterNames = [alg.getType() for alg in acas.iter_algseq(filtSeq)]
excludedNames = ['AthSequencer', 'PyAthena::Alg', 'TestHepMC']
filterNames = list(set(filterNames) - set(excludedNames))
printfunc ("MetaData: %s = %s" % ("genFilterNames", ", ".join(filterNames)))


##==============================================================
## Dump evgenConfig so it can be recycled in post-run actions
##==============================================================

from PyJobTransformsCore.runargs import RunArguments
runPars = RunArguments()
runPars.nEventsPerJob = evgenConfig.nEventsPerJob
runPars.maxeventsstrategy = evgenConfig.maxeventsstrategy
with open("config.pickle", "wb") as f:
    import pickle
    pickle.dump(runPars, f)


##==============================================================
## Get ready to run...
##==============================================================
 
evgenLog.debug("****************** STARTING EVENT GENERATION *****************")
printfunc ("****************** STARTING EVENT GENERATION *****************")
printfunc ("**************************************************************")
printfunc ("****************** PLEASE IGNORE THE LOG FROM PYTHIA ************")
printfunc ("****************** GENERATION OF ONE PYTHIA EVENT ***************")
printfunc ("******************** IS NEEDED TO MAKE *************")
printfunc ("****************** THE TRANSFORM WORK ***************************")
printfunc ("**************************************************************")

