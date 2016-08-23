# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: PhysVal_jobOptions.py 767153 2016-08-10 00:20:04Z mbaugh $

# Set up the reading of the input xAOD:
import getpass
FNAME = "AOD.pool.root"
#FNAME="root://eosatlas//eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_13TeV/0c/a1/RDO.07497163._000001.pool.root.1"
if (getpass.getuser())=="mbaugh":
  FNAME = "../rootfile_storage/ESD.ttbarB.pool.root"
  #FNAME = "../rootfile_storage/AOD.07275543._000001.pool.root.1"
  #FNAME = "../rootfile_storage/ESD.kshortBT_large.pool.root"
  #FNAME = "../rootfile_storage/ESD.05297574._000081.pool.root.1"
  #FNAME = "../rootfile_storage/ESD.06634780._000015.pool.root.1"
  print " Hello, Max"

include( "AthenaPython/iread_file.py" )

if (getpass.getuser())=="woodsn":
  print "Hi Natasha!"

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/InDetPhysValMonitoringPlotDefinitions.xml"
ServiceMgr.HistogramDefinitionSvc.DefinitionFormat="text/xml"

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValDecoratorAlg
decorators = InDetPhysValDecoratorAlg()
topSequence += decorators

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.FileKey = "M_output"
topSequence += monMan


'''
This doesn't work:
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectionTool",
                                                             TrackSummaryTool = InDetTrackSummaryTool,
                                                             Extrapolator = InDetExtrapolator)        
ToolSvc += InDetTrackSelectorTool
tool1.TrackSelectionTool=InDetTrackSelectorTool
tool1.onlyInsideOutTracks = True
tool1.TrackSelectionTool.CutLevel         = "Loose" 
tool1.TrackSelectionTool.UseTrkTrackTools = True
tool1.TrackSelectionTool.TrackSummaryTool = InDetTrackSummaryTool
tool1.TrackSelectionTool.Extrapolator     = InDetExtrapolator
'''
#this works:
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
InDetTrackSelectorTool = InDet__InDetTrackSelectionTool(name = "InDetTrackSelectorTool",
                                                        CutLevel = InDetPrimaryVertexingCuts.TrackCutLevel(),
                                                        minPt = InDetPrimaryVertexingCuts.minPT(),
                                                        maxD0 = InDetPrimaryVertexingCuts.IPd0Max(),
                                                        maxZ0 = InDetPrimaryVertexingCuts.z0Max(),
                                                        maxZ0SinTheta = InDetPrimaryVertexingCuts.IPz0Max(),
                                                        maxSigmaD0 = InDetPrimaryVertexingCuts.sigIPd0Max(),
                                                        maxSigmaZ0SinTheta = InDetPrimaryVertexingCuts.sigIPz0Max(),
                                                        # maxChiSqperNdf = InDetPrimaryVertexingCuts.fitChi2OnNdfMax(), # Seems not to be implemented?
                                                        maxAbsEta = InDetPrimaryVertexingCuts.etaMax(),
                                                        minNInnermostLayerHits = InDetPrimaryVertexingCuts.nHitInnermostLayer(),
                                                        minNPixelHits = InDetPrimaryVertexingCuts.nHitPix(),
                                                        maxNPixelHoles = InDetPrimaryVertexingCuts.nHolesPix(),
                                                        minNSctHits = InDetPrimaryVertexingCuts.nHitSct(),
                                                        minNTrtHits = InDetPrimaryVertexingCuts.nHitTrt(),
                                                        minNSiHits = InDetPrimaryVertexingCuts.nHitSi(),
                                                        TrackSummaryTool = InDetTrackSummaryTool,
                                                        Extrapolator = InDetExtrapolator)
'''
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
InDetTrackSelectorTool = InDet__InDetTrackSelectionTool("InDetTrackSelectorTool")
InDetTrackSelectorTool.minPt            = 400           # Mev
InDetTrackSelectorTool.maxD0            = 4             # mm
InDetTrackSelectorTool.maxZ0            = 1000           # mm
InDetTrackSelectorTool.minNSiHits       = 6            # Pixel + SCT
InDetTrackSelectorTool.maxNPixelHoles   = 1             # Pixel only
'''
ToolSvc += InDetTrackSelectorTool

#This section should control TTST  7-12-16                                                        
mode = "Back" #Set this to "Back" for backtracking
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import TrackTruthSelectionTool
truthSelection = TrackTruthSelectionTool()
if mode == "Back":
  truthSelection.requireDecayBeforePixel = False
  truthSelection.maxBarcode = -1

ToolSvc += truthSelection

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
tool1 = InDetPhysValMonitoringTool()
tool1.TruthSelectionTool = truthSelection
tool1.useTrackSelection = False
tool1.TrackSelectionTool=InDetTrackSelectorTool
tool1.FillTrackInJetPlots = True


ToolSvc += tool1
monMan.AthenaMonTools += [tool1]

from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool", Extrapolator = InDetExtrapolator, usePixel = True, useSCT= True, CountDeadModulesAfterLastHit = True)
ToolSvc += InDetHoleSearchTool
print InDetHoleSearchTool

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["M_output DATAFILE='M_output.root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = WARNING
ServiceMgr.MessageSvc.defaultLimit = 10000
theApp.EvtMax = -1
if (getpass.getuser())=="sroe":
  theApp.EvtMax = 1000
