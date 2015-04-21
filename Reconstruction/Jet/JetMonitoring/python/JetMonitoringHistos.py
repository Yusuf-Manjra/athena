# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetMonitoring.JetHistoTools import jhm, selectionAndHistos
from JetMonitoring.JetMonitoringConf import JetAttributeHisto, HistoDefinitionTool, JetMonitoringTool, JetKinematicHistos, JetContainerHistoFiller
from AthenaCommon.AppMgr import ToolSvc
from JetRec.JetRecFlags import jetFlags

def commonMonitoringTool(container, refcontainer=""):
    filler = JetContainerHistoFiller(container+"HistoFiller",JetContainer = container)

    # Give a list of predefined tools from jhm or a combination of such tools
    filler.HistoTools = [
        # build a special kinematics histo tool plotting 2D hists :
        JetKinematicHistos("kinematics",PlotOccupancy=True, PlotAveragePt=True, PlotAverageE=True, PlotNJet=True) ,

        
        # Draw a set of histo for a particular jet selection :
        selectionAndHistos( "leadingjet" , [ "basickinematics", ] ),
        selectionAndHistos( "subleadingjet" , [ "basickinematics"] ),

        jhm.Width,

        # distances between 2 leading jets.
        jhm.leadingjetrel,
        ]

    if "Topo" in container:
        filler.HistoTools += [

            # jet states
            jhm.basickinematics_emscale,
            jhm.basickinematics_constscale,


            # calo variables
            jhm.NegativeE,
            jhm.EMFrac,
            jhm.HECFrac,
            jhm.Timing,
            jhm.LArQuality,

            # calo quality variables
            jhm.AverageLArQF,
            jhm.HECQuality,
            jhm.FracSamplingMax,
            jhm.FracSamplingMaxIndex,
            jhm.N90Constituents,
            jhm.CentroidR,
            jhm.OotFracClusters5,
            jhm.OotFracClusters10,

            # energy per sampling
            jhm.PreSamplerB,
            jhm.EMB1,
            jhm.EMB2,
            jhm.EMB3,
            jhm.PreSamplerE,
            jhm.EME1,
            jhm.EME2,
            jhm.EME3,
            jhm.HEC0,
            jhm.HEC1,   
            jhm.HEC2,
            jhm.HEC3,
            jhm.FCAL0,
            jhm.FCAL1,
            jhm.FCAL2, 
            jhm.TileBar0,
            jhm.TileBar1,
            jhm.TileBar2,
            jhm.TileExt0,
            jhm.TileExt1,
            jhm.TileExt2,
            jhm.TileGap1,
            jhm.TileGap2,
            jhm.TileGap3,
            
            ]
        if jetFlags.useTracks:
            filler.HistoTools += [
                # track variables
                jhm.tool("JVF[0]"),
                jhm.SumPtTrkPt1000,
                jhm.GhostTrackCount, ]
            
        
        if refcontainer:
            # efficiency
            filler.HistoTools += [jhm.effresponse,]                                                          
            filler.HistoTools['effresponse'].RefContainer = refcontainer

    #filler.OutputLevel =2 
    return filler



if jetFlags.useTracks == False:
    athenaMonTool = JetMonitoringTool(HistoTools = [
        
        commonMonitoringTool( "AntiKt4LCTopoJets" ), # if truth is present, we could add : , "AntiKt4TruthJets" ,
        commonMonitoringTool( "AntiKt4EMTopoJets" ),
        commonMonitoringTool( "AntiKt10LCTopoJets" ),   
        #commonMonitoringTool( "AntiKt3PV0TrackJets" ),
        ],
                                      IntervalType = 6,) # 6 is 'Interval_t::run' interval
else:
     athenaMonTool = JetMonitoringTool(HistoTools = [
         
         commonMonitoringTool( "AntiKt4LCTopoJets" ), # if truth is present, we could add : , "AntiKt4TruthJets" ,
         commonMonitoringTool( "AntiKt4EMTopoJets" ),
         commonMonitoringTool( "AntiKt10LCTopoJets" ),   
         commonMonitoringTool( "AntiKt3PV0TrackJets" ),
         ],
                                       IntervalType = 6,) # 6 is 'Interval_t::run' interval


ToolSvc += athenaMonTool

def athenaMonitoringTool():
    return athenaMonTool

