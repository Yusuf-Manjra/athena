##############################
#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
#############################
##############################################################################
#
#  Main alignment script which sets up all tools and algorithms to run global
#  or local chi2 alignment using analytical derivatives.
#
##############################################################################
#
# The Following options can be specified seperately in a file that include this one
#
# General Alignemnt Options and their defaults
newInDetAlignAlg_Options = {
    "outputLevel"           : INFO                   #  output level for log messages for all tools and algs
    ,"trackCollection"      : "SelectedMuonsRefit1"  #  track collection to process
    ,"d0significanceCut"    : -1                     #  Cut on d0 significance
    ,"CorrectD0"            : False                  #  whether to use D0 constraint
    ,"CorrectZ0"            : False                  #  whether to use Z0 constraint
    ,"readConstantsFromPool": False                  #  whether to read initial alignment constants from pool file
    ,"readSilicon"          : True                   #  whether to read initial Si alignment constants from pool file
    ,"readTRT"              : True                   #  whether to read initial TRT alignment constnats from pool file
    ,"readTRTL3"            : False                  #  whether to read initial TRT L3 alignment constnats from pool file
    ,"inputPoolFiles"       : ["IDalignment_nominal.pool.root"]  #  pool files to read the constants from
    ,"writeConstantsToPool" : True                   #  whether to write final alignment constants to pool file
    ,"writeSilicon"         : True                   #  whether to write final Si constants to pool file
    ,"writeTRT"             : True                   #  whether to write final TRT constants to pool file
    ,"writeTRTL3"           : False                  #  whether to write final TRT L3 constants to pool file
    ,"writeIBLDistDB"       : True                   #  whether to write /Indet/IBLDist to db file
    ,"writeDynamicDB"       : True                   #  whether to write new dynamic folder sheme or old static
    ,"outputPoolFile"       : "alignment_output.pool.root"  #  pool file to write the final constants
    ,"outputCoolFile"       : "mycool2.db"           #  cool file to write the final bowing
    ,"tagSi"                : "IndetAlign_test"      #  DB tag to use for final Si constants
    ,"tagTRT"               : "TRTAlign_test"        #  DB tag to use for final TRT constnats
    ,"tagBow"               : "IndetIBLDist"         #  DB tag to use for final Bowing constants
    ,"runAccumulate"        : True                   #  whether to run the event/track processing
    ,"runSolving"           : True                   #  whether to run the solving
    ,"runLocal"             : False                  #  whether to runlocal or global Chi2 method
    ,"residualType"         : 0                      #  type of residual to use in local method 0 - hit-only (biased), 1 - unbiased
    ,"usePixelErrors"       : True                   #  whether to use real Pixel errors (only for local method)
    ,"useSCTErrors"         : True                   #  whether to use real SCT errors (only for local method)
    ,"useTRTErrors"         : True                   #  whether to use real TRT errors (only for local method)
    ,"inputMatrixFiles"     : [ "matrix.bin" ]       #  list of matrix files when solving only
    ,"inputVectorFiles"     : [ "vector.bin" ]       #  list of vector files when solving only
    ,"inputTFiles"          : ["AlignmentTFile.root"]# list of the Alignment TFiles, used only if WriteTFile is True
    ,"solvingOption"        : 3                      #  which global solver to run 0-none, 1-Lapack, 2-Eigen, 6-ROOT, 7-CLHEP
    ,"solveLocal"           : False                  #  whether to run local solving
    ,"writeMatrixFile"      : True                   #  whether to write matrix to file before solving
    ,"writeMatrixFileTxt"   : True                   #  whether to write matrix to text file
    ,"WriteTFile"           : True                   #  use ROOT files for store the matrix
    ,"TFileName"            :"AlignmentTFile.root"   #  Output TFile name, used only if WriteTFile is True
    ,"PathBinName"          :"./"                    #  path for the output AlignmentTFile
    ,"runDiagonalization"   : True                   #  whether to run diagonalization (Lapack, CLHEP)
    ,"eigenvalueCut"        : 0.                     #  minimal size of eigenvalue for diagonalization
    ,"ModCut"               : 0                      #  nomber of modes to cut for diagonalization
    ,"useSparse"            : False                  #  whether to use sparse or symmetric matrix representation
    ,"softModeCut"          : 0.                     #  soft-mode-cut for all DoF
    ,"minHits"              : 100                    #  minimal number of hits in module to do the solving
    ,"scaleMatrix"          : True                   #  whethwe to normalize the matrix to number of hits before solving (Lapack)
    ,"calculateFullCovariance" : True                #  whether to calculate the full covariance matrix of the alignment parameters
    ,"refitTracks"          : True                   #  whether to refit tracks before processing
    ,"runOutlier"           : True                   #  whether to refit tracks with outlier removal
    ,"particleNumber"       : 3                      #  particle hypothesis to use in the refit
    ,"useTrackSelector"     : True                   #  whether to use track selector
    ,"writeAlignNtuple"     : False                  #  whether to write an alignment specific ntuple
    ,"alignNtupleName"      : "newIDalign.root"      #  name of the ntuple file
    ,"writeDerivatives"     : False                  #  whether to store dr/da in the ntuple
    ,"writeHitmap"          : True                   #  whether to store the hitmap
    ,"writeHitmapTxt"       : True                   #  whether to store the hitmap in a text file
    ,"writeEigenMat"        : False                  #  whether to write the eigenspectrum/eigenvectors into files
    ,"writeEigenMatTxt"     : False                  #  whether to write the eigenspectrum/eigenvectors into text files
    ,"inputHitmapFiles"     : [ "hitmap.bin" ]       #  list of hitmap files when solving only
    ,"readHitmaps"          : True                   #  whether to use the hitmap information when solve only
    ,"doMonitoring"         : True                   #  whether to run the monitoring scripts
    ,"monitoringName"       : ""                     # apply a monitoring name
    ,"Cosmics"              : False                  #  whether to use special cosmic track selection
    ,"useTRT"               : True                   #  whether to use TRT for track selection
    ,"useOldPreProcessor"   : False                  #  temporary option, new preProcessor with beam-spot constraint is
                                                     #  available only from tag TrkAlignGenTools-01-07-08 which didn't
                                                     #  make it into 15.6.9, once in the release, old preProcessor will
                                                     #  be removed
    ,"doBSConstraint"       : False                  #  run with beam-spot constraint
    ,"doPVConstraint"       : False                  #  run with primary vertex constraint
    ,"doFullVertex"         : False                  #  run with the full GX vertex constraint
    ,"doBSTrackSelection"   : False                  #  run BS constraint only for tracks which pass tighter track selection
    ,"doBSAssociatedToPVSelection"  : False                  #  run BS constraint only for tracks associated to primary vertex
    ,"beamspotScalingFactor"    : 1.                     #  factor to scale the size of the beam spot
    ,"PtCutForBSConstraint"     : 0.                     #  Max Pt Cut for the BS constraint in order to not clash with the IP constraints
    ,"MinPtForConstrainedProvider" : 0.                     #  Max Pt Cut for the BS constraint in order to not clash with the IP constraints
    ,"eoverpmapconstraint"      : ""                 #  sagitta bias map for momentum constraint(could be E/P or Z->mumu method )
    ,"eoverpmapconstraintmap"   : "h_deltasagitta_computed_map"                                 #  sagitta bias map for momentum constraint(could be E/P or Z->mumu method )
    ,"z0mapconstraint"          : ""                 #  z0 bias map file for IP constraint
    ,"z0mapconstraintmap"       : "z0CorrectionVsEtaPhi"                                 #  z0 bias map name for IP constraint
    ,"d0mapconstraint"          : ""                #  filename for d0 bias map for IP constraint
    ,"d0mapconstraintmap"       : "d0CorrectionVsEtaPhi"                     #  d0 bias map for IP constraint
    ,"CorrectPt"             : False                 #
    ,"CorrectD0"             : False                 #
    ,"CorrectZ0"             : False                 #
    ,"PtCut"                 : 10000                 #  Pt cut for the reconstruction [MeV]


    ,"doSiHitQualSel"       : True                   #  perform Si hit (pix+sct) quality selection with InDetAlinHitQualSelTool
    ,"AlignIBLbutNotPixel"  : False	             # Set to True to align the IBL but not the Old Pixel (L11).
    ,"AlignPixelbutNotIBL"  : False		     # Set to True to align the Old Pixel but not the IBL (L11).

    ,"Remove_Pixel_Tx"      : False		# Set to True to remove Old Pixel's Tx Dof from the alignment Matrix (L11).
    ,"Remove_Pixel_Ty"      : False		# Set to True to remove Old Pixel's Ty Dof from the alignment Matrix (L11).
    ,"Remove_Pixel_Tz"      : False		# Set to True to remove Old Pixel's Tz Dof from the alignment Matrix (L11).
    ,"Remove_Pixel_Rx"      : False		# Set to True to remove Old Pixel's Rx Dof from the alignment Matrix (L11).
    ,"Remove_Pixel_Ry"      : False		# Set to True to remove Old Pixel's Ry Dof from the alignment Matrix (L11).
    ,"Remove_Pixel_Rz"      : False		# Set to True to remove Old Pixel's Rz Dof from the alignment Matrix (L11).

    ,"Remove_IBL_Tx"        : False		# Set to True to remove IBL's Tx Dof from the alignment Matrix (L11).
    ,"Remove_IBL_Ty"        : False		# Set to True to remove IBL's Ty Dof from the alignment Matrix (L11).
    ,"Remove_IBL_Tz"        : False		# Set to True to remove IBL's Tz Dof from the alignment Matrix (L11).
    ,"Remove_IBL_Rx"        : False		# Set to True to remove IBL's Rx Dof from the alignment Matrix (L11).
    ,"Remove_IBL_Ry"        : False		# Set to True to remove IBL's Ry Dof from the alignment Matrix (L11).
    ,"Remove_IBL_Rz"        : False		# Set to True to remove IBL's Rz Dof from the alignment Matrix (L11).

    ,"FixMomentum"          : False             # Set to True to remove the QoverP from the track parameters when deriving the residuals.
    ,"LumiblockSelection"   : False				     #	whether to select events from a list of lumiblocks.
    ,"LumiblockList"	    : range(400,420,1) 		     #	The list of LBs you want to select

}

print (" == InDetAlign_setup == InDetAlign_Setup STARTED == ")
##############################################################################
## If the variables are defined use thier values.
## If not defined the defaults given above are used
for var in newInDetAlignAlg_Options:
    if var in dir():
        newInDetAlignAlg_Options[var] = eval(var)

print (" == InDetAlign_setup == set up for cosmics ? ", newInDetAlignAlg_Options["Cosmics"])

if newInDetAlignAlg_Options["Cosmics"]:
    # If Cosmics, switch off Beamspot / Vtx constraint
    newInDetAlignAlg_Options["doBSConstraint"] = False
    newInDetAlignAlg_Options["doPVConstraint"] = False
    newInDetAlignAlg_Options["doFullVertex"] = False
    newInDetAlignAlg_Options["doBSTrackSelection"] = False
    newInDetAlignAlg_Options["doBSAssociatedToPVSelection"] = False

if newInDetAlignAlg_Options["doBSConstraint"]:
    newInDetAlignAlg_Options["doBSTrackSelection"] = True

print (" <InDetAlign_setup> setup check for BS and Pvx constraints:")
print (" == doBSConstraint ?: ",newInDetAlignAlg_Options["doBSConstraint"])
print (" == doPVConstraint ?: ",newInDetAlignAlg_Options["doPVConstraint"])
print (" == doFullVertex   ?: ",newInDetAlignAlg_Options["doFullVertex"])
print (" == doBSTrackSelec ?: ",newInDetAlignAlg_Options["doBSTrackSelection"])
print (" == doBSAssoc2PV   ?: ",newInDetAlignAlg_Options["doBSAssociatedToPVSelection"])


if (len(newInDetAlignAlg_Options["eoverpmapconstraint"])>0):
    newInDetAlignAlg_Options["CorrectPt"] = True
    print (" <InDetAlign_setup> file with momentum constraint map: %s" %newInDetAlignAlg_Options["eoverpmapconstraint"])
    print (" <InDetAlign_setup> histogram momentum constraint map: %s" %newInDetAlignAlg_Options["eoverpmapconstraintmap"])
else:
    print (" <InDetAlign_setup> NO momentum constraint")

if (len(newInDetAlignAlg_Options["d0mapconstraint"])>0):
    newInDetAlignAlg_Options["CorrectD0"] = True
    print (" <InDetAlign_setup> file with d0 constraint map: %s" %newInDetAlignAlg_Options["d0mapconstraint"])
    print (" <InDetAlign_setup> histogram d0 constraint map: %s" %newInDetAlignAlg_Options["d0mapconstraintmap"])
else:
    print (" <InDetAlign_setup> NO d0 constraint")

if (len(newInDetAlignAlg_Options["z0mapconstraint"])>0):
    newInDetAlignAlg_Options["CorrectZ0"] = True
    print (" <InDetAlign_setup> file with z0 constraint map: %s" %newInDetAlignAlg_Options["z0mapconstraint"])
    print (" <InDetAlign_setup> histogram z0 constraint map: %s" %newInDetAlignAlg_Options["z0mapconstraintmap"])
else:
    print (" <InDetAlign_setup> NO z0 constraint")


## ===================================================================
## Preprocessor Check ##
if newInDetAlignAlg_Options["doBSConstraint"] or newInDetAlignAlg_Options["doPVConstraint"] or newInDetAlignAlg_Options["doBSTrackSelection"] or newInDetAlignAlg_Options["doBSAssociatedToPVSelection"]:
    print (" == InDetAlign_setup ==  Switching off the OldPreprocessor")
    newInDetAlignAlg_Options["useOldPreProcessor"] = False

if newInDetAlignAlg_Options["useOldPreProcessor"]:
    print (" == InDetAlign_setup == WARNING: The BeamSpot constraint and PV Constraint are disabled")

## ===================================================================
## ===================================================================
## ==                                                               ==
## ==               Settings for the alignment                      ==
## ==                                                               ==
print (' == InDetAlign_setup == runAccumulate ?  %r' %newInDetAlignAlg_Options["runAccumulate"])
print (' == InDetAlign_setup == runSolving ?     %r' %newInDetAlignAlg_Options["runSolving"])
if not newInDetAlignAlg_Options["runAccumulate"] and not newInDetAlignAlg_Options["runSolving"]:
    print (' == InDetAlign_setup == ERROR: Both event processing and solving switched off. Nothing to be done.')
    exit(10)

# if we're running full local method we have to adjust some parameters
# if we're running Global, we need to refit tracks to get matrices from the fitter
if newInDetAlignAlg_Options["runLocal"]:
    newInDetAlignAlg_Options["solveLocal"] = True
    newInDetAlignAlg_Options["solvingOption"] = 0
    newInDetAlignAlg_Options["useSparse"] = True
else :
    newInDetAlignAlg_Options["refitTracks"] = True
    # residual type different that hit-only (biased)
    # doesn't mane sense for global chi2 method
    newInDetAlignAlg_Options["residualType"] = 0

# if we're not solving ...
if not newInDetAlignAlg_Options["runSolving"]:
    # don't run global
    newInDetAlignAlg_Options["solvingOption"] = 0
    # don't run local
    newInDetAlignAlg_Options["solveLocal"] = False
    # don't update constants
    updateConstants = False

# if we're not writing the alignment ntuple we also don't store the derivatives
if not newInDetAlignAlg_Options["writeAlignNtuple"]:
    newInDetAlignAlg_Options["writeDerivatives"] = False

## ===================================================================
## ===================================================================
## ==                                                               ==
## ==       Below is the detailed setup of the alignment job        ==
## ==                                                               ==

ToolSvc = Service('ToolSvc')

#######################
# global chi2 alignment
#######################


################################
# use InDetTrackFitter as defined in the ID reconstruction
# it is a Global Chi2 fitter and only needed when run accumulation
if newInDetAlignAlg_Options["runAccumulate"]:

    if not newInDetAlignAlg_Options["runLocal"]:
        InDetTrackFitter.FillDerivativeMatrix = True

    trackFitter = InDetTrackFitter
    if newInDetAlignAlg_Options["refitTracks"]:
        if newInDetAlignAlg_Options["particleNumber"] == 0:
            trackFitter.GetMaterialFromTrack = False
        else:
            trackFitter.GetMaterialFromTrack = True

################################
# create AlignModuleTool
# this will be most likely overwritten with detector specific alignModuleTool
# during geometry setup but it is a reasonable default
from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__AlignModuleTool
alignModuleTool = Trk__AlignModuleTool( name = "AlignModuleTool",
                                        OutputLevel = newInDetAlignAlg_Options["outputLevel"]
                                    )
ToolSvc += alignModuleTool
print (alignModuleTool)

################################
#
# include geometry setup
#
include("InDetPerformanceMonitoring/InDetAlign_Geometry_Setup.py")

################################
# make GlobalChi2AlignTool
from TrkGlobalChi2AlignTools.TrkGlobalChi2AlignToolsConf import Trk__GlobalChi2AlignTool
globalChi2AlignTool = Trk__GlobalChi2AlignTool(
    name = 'GlobalChi2AlignTool',
    AlignModuleTool = alignModuleTool,
    StoreLocalDerivOnly = (newInDetAlignAlg_Options["solveLocal"] and newInDetAlignAlg_Options["solvingOption"]==0), # If we're running local later, we want to store ony local derivs
    SecondDerivativeCut = 0, # this is just for the moment :(
    OutputLevel = newInDetAlignAlg_Options["outputLevel"]
)
ToolSvc += globalChi2AlignTool

# setup MatrixTool
matrixTool = globalChi2AlignTool.MatrixTool
matrixTool.OutputLevel      = newInDetAlignAlg_Options["outputLevel"]
matrixTool.AlignModuleTool  = alignModuleTool
matrixTool.UseSparse        = newInDetAlignAlg_Options["useSparse"]
matrixTool.SolveOption      = newInDetAlignAlg_Options["solvingOption"]
matrixTool.MinNumHitsPerModule  = newInDetAlignAlg_Options["minHits"]
matrixTool.Diagonalize      = newInDetAlignAlg_Options["runDiagonalization"]
matrixTool.EigenvalueThreshold  = newInDetAlignAlg_Options["eigenvalueCut"]
matrixTool.RunLocalMethod       = newInDetAlignAlg_Options["solveLocal"]
matrixTool.WriteMat         = newInDetAlignAlg_Options["writeMatrixFile"]
matrixTool.WriteMatTxt      = newInDetAlignAlg_Options["writeMatrixFileTxt"]
matrixTool.WriteEigenMat    = newInDetAlignAlg_Options["writeEigenMat"]
matrixTool.WriteEigenMatTxt     = newInDetAlignAlg_Options["writeEigenMatTxt"]
matrixTool.ModCut       = newInDetAlignAlg_Options["ModCut"]
matrixTool.InputMatrixFiles     = newInDetAlignAlg_Options["inputMatrixFiles"]
matrixTool.InputVectorFiles     = newInDetAlignAlg_Options["inputVectorFiles"]
matrixTool.WriteHitmap      = newInDetAlignAlg_Options["writeHitmap"]
matrixTool.WriteHitmapTxt   = newInDetAlignAlg_Options["writeHitmapTxt"]
matrixTool.ReadHitmaps      = newInDetAlignAlg_Options["readHitmaps"]
matrixTool.InputHitmapFiles     = newInDetAlignAlg_Options["inputHitmapFiles"]
matrixTool.SoftEigenmodeCut     = newInDetAlignAlg_Options["softModeCut"]
matrixTool.ScaleMatrix      = newInDetAlignAlg_Options["scaleMatrix"]
matrixTool.PathBinName      = newInDetAlignAlg_Options["PathBinName"]

matrixTool.AlignIBLbutNotPixel = newInDetAlignAlg_Options["AlignIBLbutNotPixel"]
matrixTool.AlignPixelbutNotIBL = newInDetAlignAlg_Options["AlignPixelbutNotIBL"]

if newInDetAlignAlg_Options["WriteTFile"]:
    matrixTool.WriteTFile   = True
    matrixTool.ReadTFile    = True
    matrixTool.WriteMat     = False
    matrixTool.WriteHitmap  = False
    matrixTool.TFileName        = newInDetAlignAlg_Options["TFileName"]
    matrixTool.InputTFiles      = newInDetAlignAlg_Options["inputTFiles"]

if not newInDetAlignAlg_Options["runLocal"]:
    # we want to set 'reasonable' defaults for global L3 solving
    if (
            ("sctAlignmentLevel" in dir()       and sctAlignmentLevel==3) or
            ("sctAlignmentLevelBarrel" in dir()     and sctAlignmentLevelBarrel==3) or
            ("sctAlignmentLevelEndcaps" in dir()    and sctAlignmentLevelEndcaps==3) or
            ("pixelAlignmentLevel" in dir()     and pixelAlignmentLevel==3) or
            ("pixelAlignmentLevelBarrel" in dir()   and pixelAlignmentLevelBarrel==3) or
            ("pixelAlignmentLevelEndcaps" in dir()  and pixelAlignmentLevelEndcaps==3)
        ):
        matrixTool.WriteEigenMat = False
        matrixTool.UseSparse = True
        matrixTool.SolveOption = 2 # run Eigen for L3 by default
        matrixTool.WriteMatTxt = False
        matrixTool.WriteHitmapTxt = False
        matrixTool.CalculateFullCovariance = False
    # but the defaults can still be overwritten if explicitely requested
    if 'useSparse' in dir():
        matrixTool.UseSparse            = newInDetAlignAlg_Options["useSparse"]
    if 'solvingOption' in dir():
        matrixTool.SolveOption          = newInDetAlignAlg_Options["solvingOption"]
    if 'writeMatrixFileTxt' in dir():
        matrixTool.WriteMatTxt          = newInDetAlignAlg_Options["writeMatrixFileTxt"]
    if 'writeHitmapTxt' in dir():
        matrixTool.WriteHitmapTxt       = newInDetAlignAlg_Options["writeMatrixFileTxt"]
    if 'writeEigenMat' in dir():
        matrixTool.WriteEigenMat        = newInDetAlignAlg_Options["writeEigenMat"]
    if 'writeEigenMatTxt' in dir():
        matrixTool.WriteEigenMatTxt         = newInDetAlignAlg_Options["writeEigenMatTxt"]
    if 'calculateFullCovariance' in dir():
        matrixTool.CalculateFullCovariance  = newInDetAlignAlg_Options["calculateFullCovariance"]

print (matrixTool)
print (globalChi2AlignTool)

#################################################
##
## Hit quality selection tool
##
from InDetAlignGenTools.InDetAlignGenToolsConf import InDetAlignHitQualSelTool
myHitQualSelTool = InDetAlignHitQualSelTool(
    name = "AlignSiHitQualSelTool",
    OutputLevel = INFO,
    RejectOutliers = True,
    RejectEdgeChannels = False,
    #AcceptIBLHits = True,
    #AcceptPixelHits = True,
    #AcceptSCTHits = True,
    )
ToolSvc += myHitQualSelTool
print (myHitQualSelTool)

##################################################
##
##  Setup all tools needed for event processing
##
if newInDetAlignAlg_Options["runAccumulate"]:
    ###############################
    # New InDetTrackSelectionTool
    if newInDetAlignAlg_Options["useTrackSelector"]:
        minNTRTHits  = -1
        maxEtaForTRTHitCuts = 0
        maxTRTEtaAcceptance = 1e+16
        if newInDetAlignAlg_Options["useTRT"]:
            print (" == InDetAlign_setup == InDetAlign_setup : setting TRT Cuts")
            minNTRTHits = 0
            maxEtaForTRTHitCuts = 1.9 # was 1.9
            maxTRTEtaAcceptance = 0

        from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
        if not newInDetAlignAlg_Options["Cosmics"] : # collision events
            print (' == InDetAlign_setup == InDetAlign_setup : setting InDetTrackSelectionToolAlignTracks')
            trackSelectorNew = InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectionToolAlignTracks",
                                                              UseTrkTrackTools = True,
                                                              OutputLevel       = newInDetAlignAlg_Options["outputLevel"],
                                                              minPt = float(newInDetAlignAlg_Options["PtCut"]),
                                                              #maxPt = 5000., # in MeV
                                                              maxD0 = 500,
                                                              maxZ0 = 500,
                                                              #minNPixelHits = 1,
                                                              minNPixelHitsPhysical = 2,
                                                              #minNSiHits = 5,
                                                              minNSiHitsPhysical = 10,
                                                              minNSctHits = 5,
                                                              minNSctHitsPhysical = 7,
                                                              minNTrtHits  = minNTRTHits,
                                                              maxEtaForTrtHitCuts = maxEtaForTRTHitCuts,
                                                              maxTrtEtaAcceptance = maxTRTEtaAcceptance,
                                                              CutLevel = "TightPrimary",
                                                              TrackSummaryTool    = InDetTrackSummaryTool,
                                                              Extrapolator        = InDetExtrapolator)
            if newInDetAlignAlg_Options["d0significanceCut"] > 0:
                trackSelectorNew.maxD0overSigmaD0 = 3
        else:
            print (' == InDetAlign_setup == InDetAlign_setup : setting InDetTrackSelectionToolAlignTracks for cosmics')
            trackSelectorNew= InDet__InDetTrackSelectionTool(name         = "InDetTrackSelectionToolAlignTracks",
                                                             UseTrkTrackTools = True,
                                                             minPt = float(newInDetAlignAlg_Options["PtCut"]),
                                                             maxD0 = 9999.,
                                                             maxZ0 = 9999.,
                                                             minNPixelHits = 1,
                                                             minNSctHits = 8,
                                                             minNSiHitsModTop = 2,
                                                             minNSiHitsModBottom = 2,
                                                             minNTrtHits = minNTRTHits,
                                                             maxEtaForTrtHitCuts = maxEtaForTRTHitCuts,
                                                             maxTrtEtaAcceptance = maxTRTEtaAcceptance,
                                                             CutLevel = "Loose",
                                                             TrackSummaryTool    = InDetTrackSummaryTool,
                                                             Extrapolator        = InDetExtrapolator)
        ToolSvc+=trackSelectorNew
        print (trackSelectorNew)

    ###############################
    # create AlignTrackPreProcessor
    if newInDetAlignAlg_Options["useOldPreProcessor"] :
        print (" == InDetAlign_setup == WARNING using old preprocessor") 
        from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__AlignTrackPreProcessor
        # with hit quality selection tool
        preProcessor = Trk__AlignTrackPreProcessor(
            name = "AlignTrackPreProcessor",
            OutputLevel = newInDetAlignAlg_Options["outputLevel"],
            RefitTracks = newInDetAlignAlg_Options["refitTracks"],
            TrackFitterTool = trackFitter,
            SLTrackFitterTool = trackFitter,
            UseSingleFitter = True,
            ParticleHypothesis = newInDetAlignAlg_Options["particleNumber"],
            RunOutlierRemoval = newInDetAlignAlg_Options["runOutlier"],
            HitQualityTool = myHitQualSelTool
        )
        if newInDetAlignAlg_Options["doSiHitQualSel"]:
            preProcessor.SelectHits = newInDetAlignAlg_Options["doSiHitQualSel"]

        if newInDetAlignAlg_Options["FixMomentum"]:
            preProcessor.FixMomentum = newInDetAlignAlg_Options["FixMomentum"]
            print (" == InDetAlign_setup == OldProcessor -> Using FixMomentum = ", newInDetAlignAlg_Options["FixMomentum"])

        # don't store matrices when running local
        if newInDetAlignAlg_Options["runLocal"]:
            preProcessor.StoreFitMatricesAfterRefit = False

        if newInDetAlignAlg_Options["useTrackSelector"]:
            preProcessor.SelectTracks = True
            preProcessor.TrackSelectorTool = trackSelectorNew
    else :
        print (" == InDetAlign_setup == Using new preProcessor") 
        from InDetRecExample import TrackingCommon
        from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__BeamspotVertexPreProcessor
        print (' == InDetAlign_setup == setting BeamspotVertexPreProcessor ')
        preProcessor = Trk__BeamspotVertexPreProcessor("BeamspotVertexPreProcessor",
                                                       OutputLevel           = newInDetAlignAlg_Options["outputLevel"],
                                                       RefitTracks           = newInDetAlignAlg_Options["refitTracks"],
                                                       TrackFitter           = trackFitter,
                                                       AlignModuleTool       = alignModuleTool,
                                                       TrackToVertexIPEstimator = TrackingCommon.getTrackToVertexIPEstimator(), 
                                                       UseSingleFitter       = True,
                                                       ParticleNumber        = newInDetAlignAlg_Options["particleNumber"],
                                                       RunOutlierRemoval     = newInDetAlignAlg_Options["runOutlier"],
                                                       DoTrackSelection      = False, # accept the tracks as provided by the IDPerfMon
                                                       DoBSConstraint        = newInDetAlignAlg_Options["doBSConstraint"],
                                                       #maxPt                = newInDetAlignAlg_Options["PtCutForBSConstraint"],
                                                       DoPVConstraint        = newInDetAlignAlg_Options["doPVConstraint"],
                                                       DoAssociatedToPVSelection = newInDetAlignAlg_Options["doBSAssociatedToPVSelection"],
                                                       BeamspotScalingFactor     = newInDetAlignAlg_Options["beamspotScalingFactor"],
                                                       DoBSTrackSelection        = newInDetAlignAlg_Options["doBSTrackSelection"],
                                                       BSConstraintTrackSelector = trackSelectorNew
        )

        #if newInDetAlignAlg_Options["doSiHitQualSel"]:        
        #    preProcessor.SelectHits = newInDetAlignAlg_Options["doSiHitQualSel"]

        if newInDetAlignAlg_Options["FixMomentum"]:
            preProcessor.FixMomentum = newInDetAlignAlg_Options["FixMomentum"]
            print (' == InDetAlign_setup ==  NewProcessor -> Using FixMomentum = ', newInDetAlignAlg_Options["FixMomentum"])

        # don't store matrices when running local
        if newInDetAlignAlg_Options["runLocal"]:
            preProcessor.StoreFitMatrices = False

        if newInDetAlignAlg_Options["useTrackSelector"]:
            preProcessor.TrackSelector = trackSelectorNew

    ToolSvc += preProcessor
    print (" == InDetAlign_setup.py == preProcessor properties follow (Trk__BeamspotVertexPreProcessor):")
    print (preProcessor)

    ################################
    # create AlignResidualCalculator
    from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__AlignResidualCalculator
    alignResidualCalculator = Trk__AlignResidualCalculator( name = "AlignResidualCalculator",
                                                            OutputLevel = newInDetAlignAlg_Options["outputLevel"],
                                                            ResidualType = newInDetAlignAlg_Options["residualType"],
                                                            IncludeScatterers = False
                                                        )
    ToolSvc += alignResidualCalculator
    print (alignResidualCalculator)

    ################################
    # create AlignTrackCreator
    from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__AlignTrackCreator
    alignTrackCreator = Trk__AlignTrackCreator( name = "AlignTrackCreator",
                                                OutputLevel = newInDetAlignAlg_Options["outputLevel"],
                                                AlignModuleTool = alignModuleTool,
                                                ResidualCalculator = alignResidualCalculator,
                                                IncludeScatterers = False,
                                                RemoveATSOSNotInAlignModule = True,
                                                WriteEventList = False,
                                                RequireOverlap = False
                                            )
    ToolSvc += alignTrackCreator
    print (alignTrackCreator)

    ################################
    # create AnalyticalDerivCalcTool
    from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__AnalyticalDerivCalcTool
    derivCalcTool = Trk__AnalyticalDerivCalcTool( name = "AnalyticalDerivCalcTool",
                                                  OutputLevel = newInDetAlignAlg_Options["outputLevel"],
                                                  StoreDerivatives = newInDetAlignAlg_Options["writeDerivatives"],
                                                  AlignModuleTool = alignModuleTool
                                              )
    # some extra settings when running local
    if newInDetAlignAlg_Options["runLocal"]:
        derivCalcTool.UseLocalSetting = True
        derivCalcTool.UseIntrinsicPixelError = not newInDetAlignAlg_Options["usePixelErrors"]
        derivCalcTool.UseIntrinsicSCTError = not newInDetAlignAlg_Options["useSCTErrors"]
        derivCalcTool.UseIntrinsicTRTError = not newInDetAlignAlg_Options["useTRTErrors"]
    else:
        derivCalcTool.UseLocalSetting = False
    ToolSvc += derivCalcTool
    print (derivCalcTool)

    ################################
    # create AlignTrackDresser that uses AnalyticalDerivCalcTool
    from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__AlignTrackDresser
    alignTrackDresser = Trk__AlignTrackDresser( name = "AlignTrackDresser",
                                                OutputLevel = newInDetAlignAlg_Options["outputLevel"],
                                                DerivCalcTool = derivCalcTool
                                            )
    ToolSvc += alignTrackDresser
    print (alignTrackDresser)

    ################################
    # Ntuple filling tool
    ntupleTool = ""
    if newInDetAlignAlg_Options["writeAlignNtuple"]:
        from InDetAlignNtupleTools.InDetAlignNtupleToolsConf import InDet__SimpleIDNtupleTool
        ntupleTool = InDet__SimpleIDNtupleTool( name = "SimpleIDNtupleTool",
                                                OutputLevel = INFO,
                                                TrackParticleCreatorTool = InDetParticleCreatorTool,
                                                AlignModuleTool = alignModuleTool,
                                                StoreDerivatives = newInDetAlignAlg_Options["writeDerivatives"]
                                            )
        ToolSvc += ntupleTool
        print (ntupleTool)
        
    ###############################
    ###############################
    # Track Collection Provider
    if newInDetAlignAlg_Options["Cosmics"]:
        newInDetAlignAlg_Options["trackCollection"] = "CombinedInDetTracks"

    print (" == CorrectD0 ? %r" %newInDetAlignAlg_Options["CorrectD0"])
    print (" == CorrectZ0 ? %r" %newInDetAlignAlg_Options["CorrectZ0"])
    print (" == CorrectPt ? %r" %newInDetAlignAlg_Options["CorrectPt"])

    if (newInDetAlignAlg_Options["CorrectPt"] or newInDetAlignAlg_Options["CorrectD0"] or newInDetAlignAlg_Options["CorrectZ0"]):
        # this is the case when some constraint will be applied to the track fit 
        newInDetAlignAlg_Options["trackCollection"] = "SelectedMuonsRefit1"
        from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__ConstrainedTrackProvider
        trackCollectionProvider=Trk__ConstrainedTrackProvider("TrackCollectionProvider",
                                                              OutputLevel = newInDetAlignAlg_Options["outputLevel"],
                                                              doTrackSelection = False, 
                                                              InputTracksCollection= newInDetAlignAlg_Options["trackCollection"],
                                                              MinPt = newInDetAlignAlg_Options["MinPtForConstrainedProvider"],
                                                              MaxPt = 120,
                                                              MomentumConstraintFileName = newInDetAlignAlg_Options["eoverpmapconstraint"],
                                                              MomentumConstraintHistName = newInDetAlignAlg_Options["eoverpmapconstraintmap"],
                                                              CorrectMomentum = newInDetAlignAlg_Options["CorrectPt"],
                                                              DeltaScaling = 1,
                                                              ScalePMapToGeV = True,
                                                              ReduceConstraintUncertainty = 1., 
                                                              CorrectZ0 = newInDetAlignAlg_Options["CorrectZ0"],
                                                              z0ConstraintFileName =  newInDetAlignAlg_Options["z0mapconstraint"],
                                                              z0ConstraintHistName =  newInDetAlignAlg_Options["z0mapconstraintmap"],
                                                              CorrectD0 = newInDetAlignAlg_Options["CorrectD0"],
                                                              d0ConstraintFileName =  newInDetAlignAlg_Options["d0mapconstraint"],
                                                              d0ConstraintHistName =  newInDetAlignAlg_Options["d0mapconstraintmap"],
                                                              UseConstraintError =  False,
                                                              UseConstrainedTrkOnly= True,
                                                              TrackFitter = trackFitter   
                                                          )
    else:
        newInDetAlignAlg_Options["trackCollection"] = "SelectedMuonsRefit1"
        from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__TrackCollectionProvider
        trackCollectionProvider = Trk__TrackCollectionProvider("TrackCollectionProvider2",
                                                               OutputLevel = newInDetAlignAlg_Options["outputLevel"],
                                                               InputTrkCol = newInDetAlignAlg_Options["trackCollection"]
                                                           )
    print (trackCollectionProvider)
    ToolSvc += trackCollectionProvider

    ################################
    ################################
    # build GlobalAlign algorithm
    #
    from TrkAlignGenAlgs.TrkAlignGenAlgsConf import Trk__AlignAlg
    InDetGlobalAlign =  Trk__AlignAlg( name = "InDetGlobalAlign",
                                       OutputLevel = newInDetAlignAlg_Options["outputLevel"],
                                       TrackCollectionProvider = trackCollectionProvider,
                                       AlignTrackCreator = alignTrackCreator,
                                       AlignTrackDresser = alignTrackDresser,
                                       AlignTrackPreProcessor = preProcessor,
                                       GeometryManagerTool = trkAlignGeoManagerTool,
                                       FillNtupleTool = ntupleTool,
                                       WriteNtuple = newInDetAlignAlg_Options["writeAlignNtuple"],
                                       FileName = newInDetAlignAlg_Options["alignNtupleName"],
                                       AlignTool = globalChi2AlignTool,
                                       AlignDBTool = trkAlignDBTool
                                   )

    topSequence += InDetGlobalAlign
    print (InDetGlobalAlign)

##################################################
##
##  Setup solving from accumulated files
##
else:
    ################################
    # build GlobalAlign algorithm
    #
    from TrkAlignGenAlgs.TrkAlignGenAlgsConf import Trk__AlignAlg
    InDetGlobalSolve =  Trk__AlignAlg( name = "InDetGlobalSolve",
                                       OutputLevel = newInDetAlignAlg_Options["outputLevel"],
                                       GeometryManagerTool = trkAlignGeoManagerTool,
                                       AlignTool = globalChi2AlignTool,
                                       AlignDBTool = trkAlignDBTool,
                                       SolveOnly = True,
                                       WriteNtuple = False
                                   )

    topSequence += InDetGlobalSolve
    print (InDetGlobalSolve)

################################
# Write constants to Pool file
# the OutputConditionsAlg has to be set up at the end
# so that it runs last and picks up the latest version
# of constants from memory
if newInDetAlignAlg_Options["runSolving"] and newInDetAlignAlg_Options["writeConstantsToPool"]:
    objectList = []
    tagList = []

    if newInDetAlignAlg_Options["writeSilicon"]:
        if newInDetAlignAlg_Options["writeDynamicDB"]:
            objectList += ["CondAttrListCollection#/Indet/AlignL1/ID"]
            objectList += ["CondAttrListCollection#/Indet/AlignL2/PIX"]
            objectList += ["CondAttrListCollection#/Indet/AlignL2/SCT"]
            objectList += [ "AlignableTransformContainer#/Indet/AlignL3" ]
            tagList += [ "IndetL1Test", "IndetL2PIXTest", "IndetL2SCTTest",  newInDetAlignAlg_Options["tagSi"] ]
        else:
            objectList += [ "AlignableTransformContainer#/Indet/Align" ]
            tagList += [ newInDetAlignAlg_Options["tagSi"] ]
    if newInDetAlignAlg_Options["writeTRT"]:
        if newInDetAlignAlg_Options["writeDynamicDB"]:
            objectList += ["CondAttrListCollection#/TRT/AlignL1/TRT"]
            objectList += [ "AlignableTransformContainer#/TRT/AlignL2" ]
            tagList += [ "IndetL1TRTTest", newInDetAlignAlg_Options["tagTRT"] ]
        else:
            objectList += [ "AlignableTransformContainer#/TRT/Align" ]
            tagList += [ newInDetAlignAlg_Options["tagTRT"] ]
    if newInDetAlignAlg_Options["writeTRTL3"]:
        objectList += [ "TRTCond::StrawDxContainer#/TRT/Calib/DX"]
    if newInDetAlignAlg_Options["writeIBLDistDB"]:
        objectList += ["CondAttrListCollection#/Indet/IBLDist"]
        tagList    += [newInDetAlignAlg_Options["tagBow"]]


    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    myOCA = OutputConditionsAlg(outputFile = newInDetAlignAlg_Options["outputPoolFile"])
    myOCA.ObjectList = objectList
    myOCA.IOVTagList = tagList

################################
# build AlignTrack collection splitter algorithm
print (' == InDetAlign_setup == Setting up the AlignTrackCollSplitter ')
from TrkAlignGenAlgs.TrkAlignGenAlgsConf import Trk__AlignTrackCollSplitter
AlignTrackCollSplitter = Trk__AlignTrackCollSplitter(
    "AlignTrackCollSplitter",
    OutputLevel = newInDetAlignAlg_Options["outputLevel"]
)

topSequence += AlignTrackCollSplitter
print (AlignTrackCollSplitter)


################################
if newInDetAlignAlg_Options["LumiblockSelection"]:
	print (" == InDetAlign_setup ==         **************************** ")
	print ("                                **  Selecting LumiBlocks  ** ")
	print ("                                **************************** ")
	print ("                                **   List of LumiBlocks   ** ")
	print (str(newInDetAlignAlg_Options["LumiblockList"]).strip('[]'))
	print ("                                 **                      ** ")
	import InDetBeamSpotExample.FilterUtils as FilterUtils
	FilterUtils.filterSeq += FilterUtils.LBFilter(newInDetAlignAlg_Options["LumiblockList"])
else:
	print (' == InDetAlign_setup == ** LumiBlocks selection is NOT active ** ')
