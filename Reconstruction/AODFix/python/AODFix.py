# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""@package AODFix 

This package is the only one meant to be used externally (mainly by
RecExCommon). Only the AODFix_* functions should be called.

When adding a new AODFix for a release family (where a release family
is defined by the first two numbers in a release, e.g., 20.1) that
does not yet have AODFix, the user needs to create a new
AODFix_rXXY.py file with an AODFix_rXXY class inheriting from the
AODFix_base class and overwrite the appropriate member
functions. Furthermore, the AODFix_Init() function below needs to set
_aodFixInstance approprately, as explained in the comments below.

When modifying the AODFix for a release family for which
AODFix_rXXY.py exists, most likely the changes would be restricted to
the AODFix_rXXY.py class. The only exception is if the release family
definition changes (e.g., add 17.3 to the 17.2 family).

"""

from AthenaCommon.Logging import logging

logAODFix = logging.getLogger( 'AODFix' )

from AODFix_base import AODFix_base
from AODFix_r191 import AODFix_r191
from AODFix_r201 import AODFix_r201

_aodFixInstance = AODFix_base()

def run_once(f):
    def wrapper(*args, **kwargs):
        if not wrapper.has_run:
            wrapper.has_run = True
            return f(*args, **kwargs)
    wrapper.has_run = False
    return wrapper

@run_once
def AODFix_Init():
    ''' This function is called to correctly setup AODFix.'''

    global _aodFixInstance

    from RecExConfig.RecFlags import rec

    ##################
    # exit if locked to false
    ##################
    if rec.doApplyAODFix.is_locked() and rec.doApplyAODFix.get_Value() == False:
        logAODFix.info("doApplyAODFix is locked to False; not applying anything")
        return


    ##################
    # deterimine in what release we are running (and fill some rec variables
    ##################

    import os
    curRelease=os.getenv("AtlasVersion") or ''

    logAODFix.debug("curRelease set to " + curRelease)
    
    
    runningInNightly = False
    if "rel_" in curRelease:
       runningInNightly = True
       # change rec.AtlasReleaseVersion to be more useful
       atlasBaseDir = os.getenv("AtlasBaseDir") or ''
       curRelease = os.path.basename(atlasBaseDir)
       logAODFix.debug("Running in nightly, curRelease set to " + curRelease)


    ##################
    # exit if not locked to true and running in a nightly
    ##################
    if runningInNightly and not rec.doApplyAODFix.is_locked():
        logAODFix.info("running in a nightly. AODFix not scheduled.")
        logAODFix.info("Please do manually 'rec.doApplyAODFix.set_Value_and_Lock(True)' to overwrite")
        return 
 
    ##################
    # determine athena with which input file was created and previous AODFix
    ##################
   
    # RDO doesn't have MetaData
    prevAODFix='none'
    prevRelease = ''
    if rec.readRDO():
        prevRelease = 'bs'
    else:

        from RecExConfig.InputFilePeeker import inputFileSummary

        try:
            AtlasReleaseVersionString=inputFileSummary['metadata']['/TagInfo']['AtlasRelease']
            rv = AtlasReleaseVersionString.split('-')
            if len(rv) > 1:
                prevRelease = rv[1]
        except Exception:
            logAODFix.warning("no AthenRelease found in input file, setting to <none> !!")
            prevRelease='none'


        ##################
        # determine which is the previous verion of the AODFix
        ##################
        try:
            prevAODFix=inputFileSummary['tag_info']['AODFixVersion']
            if prevAODFix.startswith("AODFix_"):
                prevAODFix=prevAODFix[7:]
                prevAODFix=prevAODFix.split("_")[0]
        except Exception:
            logAODFix.debug("no AODFixVersion found in input file, setting to <none>.")
            prevAODFix='none'

    logAODFix.info("Summary of MetaData for AODFix:")
    logAODFix.info(" Inputfile produced with Athena version <%s>." % prevRelease)
    logAODFix.info(" AODFix version <%s> was previously applied." % prevAODFix)

    ##################
    # determine which AODFix to run (if actually running--to be determined later)
    ##################

    doAODFix = False
    metadataOnly = False
    if rec.doESD():
        # running from RAW or ESD, don't apply it, but set metadata
        logAODFix.debug("detected doESD=True, *not* applying AODFix (but setting metadata)")
        doAODFix = True
        metadataOnly = True
    elif rec.readAOD():  # change in policy: no AODFix if reading ESD.
        doAODFix = True
        # check to see if it's a DAOD; do not run by default in DAODs
        from RecExConfig.InputFilePeeker import inputFileSummary
        try:
            streamNames = inputFileSummary['stream_names']
            if any(map(lambda x: "StreamDAOD" in x, streamNames)) and not rec.doApplyAODFix.is_locked():
                logAODFix.info("Running on a DAOD, so not scheduling AODFix")
                doAODFix = False
        except Exception:
            logAODFix.warning("Could not determine the stream name")

    elif rec.readESD() and rec.doApplyAODFix.is_locked():
        # only run on ESDs if forcing
        doAODFix = True

    #exit if not doing AODFix
    if not doAODFix:
        return  

    curReleaseSplit = curRelease.split('.')
    prevReleaseSplit = prevRelease.split('.')
    if len(curReleaseSplit) >= 2 and (metadataOnly or rec.doApplyAODFix.is_locked() or len(prevReleaseSplit) >= 2):
        ### If adding an AODFix for a release family that does not have it,
        ### please add it to the if-elif... statement below
        if (curReleaseSplit[0] == '19' and curReleaseSplit[1] == '1' and 
              (metadataOnly or rec.doApplyAODFix.is_locked() or 
               (prevReleaseSplit[0] == '19' and prevReleaseSplit[1] == '1'))):
            _aodFixInstance = AODFix_r191(prevAODFix, metadataOnly, rec.doApplyAODFix.is_locked())
        elif (curReleaseSplit[0] == '20' and curReleaseSplit[1] == '1' and 
              (metadataOnly or rec.doApplyAODFix.is_locked() or 
               (prevReleaseSplit[0] == '20' and prevReleaseSplit[1] == '1'))):
            _aodFixInstance = AODFix_r201(prevAODFix, metadataOnly, rec.doApplyAODFix.is_locked())
        else:
            logAODFix.info("No AODFix scheduled for this release.")

    # file produced by nightly ?
    elif prevRelease.startswith("rel_"):
        logAODFix.info("input file produced from a nightly, and no AODFix applied. Will *not* schedule AODFix automatically! Please do manually 'rec.doApplyAODFix.set_Value_and_Lock(True)' to overwrite")


# The interface routines
def AODFix_addMetaData():
    '''This adds metadata both when AODFix is run or when "fixed" reco is performed'''
    _aodFixInstance.addMetaData()

# inside RecExCommon_topOptions.py:
def AODFix_preInclude():
    _aodFixInstance.preInclude()

def AODFix_postAtlfast():
    _aodFixInstance.postAtlfast()

def AODFix_postTrigger():
    _aodFixInstance.postTrigger()

def AODFix_postSystemRec():
    _aodFixInstance.postSystemRec()

def AODFix_postCombinedRec():
    _aodFixInstance.postCombinedRec()

def AODFix_postHeavyIon():
    _aodFixInstance.postHeavyIon()

# inside SystemRec_config.py:
def AODFix_postCaloRec():
    _aodFixInstance.postCaloRec()

def AODFix_postInDetRec():
    _aodFixInstance.postInDetRec()

def AODFix_postMuonRec():
    _aodFixInstance.postMuonRec()

def AODFix_postForwardRec():
    _aodFixInstance.postForwardRec()

# inside CombinedRec_config.py:
def AODFix_postEgammaRec():
    _aodFixInstance.postEgammaRec()

def AODFix_postMuonCombinedRec():
    _aodFixInstance.postMuonCombinedRec()

def AODFix_postJetRec():
    _aodFixInstance.postJetRec()

# shoudl ideally capitalize tau, like the others
def AODFix_posttauRec():
    _aodFixInstance.postTauRec()

def AODFix_postMissingETRec():
    _aodFixInstance.postMissingETRec()
