# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetMonitoring.JetHistoTools import jhm, selectionAndHistos
from JetMonitoring.JetMonitoringConf import JetAttributeHisto, HistoDefinitionTool, JetMonitoringTool, JetKinematicHistos, JetContainerHistoFiller
from AthenaCommon.AppMgr import ToolSvc

def commonMonitoringTool(container, refcontainer=""):
    filler = JetContainerHistoFiller(container+"HistoFiller",JetContainer = container)

    # Give a list of predefined tools from jhm or a combination of such tools
    filler.HistoTools = [
        # build a special tool without 2D hists :
        JetKinematicHistos("kinematics",PlotOccupancy=True, PlotAveragePt=True, PlotNJet=True) ,

        
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

            # track variables
            jhm.tool("JVF[0]"),
            jhm.SumPtTrkPt1000,
            jhm.GhostTrackCount,

            # calo variables
            jhm.NegativeE,
            jhm.EMFrac,
            jhm.HECFrac,
            jhm.Timing,
            jhm.LArQuality,

            jhm.EMB2,
            jhm.EMB3,            
            jhm.EME2,
            jhm.EME3,
            jhm.HEC2,
            jhm.HEC3,
            jhm.FCAL0,
            jhm.FCAL1,
            jhm.TileBar0,
            jhm.TileBar1,
            jhm.TileExt0,
            jhm.TileExt1,
            
            ]
        
        if refcontainer:
            # efficiency
            filler.HistoTools += [jhm.effresponse,]                                                          
            filler.HistoTools['effresponse'].RefContainer = refcontainer

    #filler.OutputLevel =2 
    return filler



athenaMonTool = JetMonitoringTool(HistoTools = [

    commonMonitoringTool( "AntiKt4LCTopoJets" ), # if truth is present, we could add : , "AntiKt4TruthJets" ,
    commonMonitoringTool( "AntiKt4EMTopoJets" ),
    commonMonitoringTool( "AntiKt10LCTopoJets" ),   
    commonMonitoringTool( "AntiKt3PV0TrackJets" ),
    
     # == 'run'
    ],
                                  IntervalType = 6,) # 6 is 'Interval_t::run' interval

ToolSvc += athenaMonTool

def athenaMonitoringTool():
    return athenaMonTool

