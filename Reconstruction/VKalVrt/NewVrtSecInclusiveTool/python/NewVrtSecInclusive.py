# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Author: Vadim Kostyukhin vadim.kostyukhin@cern.ch
from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *
from AthenaCommon.Include  import Include, IncludeError, include
from AthenaCommon.Logging  import logging
from AthenaCommon.AppMgr   import ToolSvc
from AthenaCommon          import CfgMgr

from NewVrtSecInclusiveTool.NewVrtSecInclusiveToolConf import Rec__NewVrtSecInclusiveTool

# define the class
# Search for soft B-hadron vertices. 
#------------------------------------
class SoftBFinderTool( Rec__NewVrtSecInclusiveTool ):

    def __init__(self, name = 'SoftBFinderTool'  ):        

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'SoftBFinderTool::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="SVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator")
        ToolSvc += SVertexFitterTool
        #----------------------
        # Soft B-hadron vertex finder itself
        #
        Rec__NewVrtSecInclusiveTool.__init__( self, name = name,
                                             VertexFitterTool   = SVertexFitterTool,
					     CutBLayHits  = 1,
					     CutPixelHits = 3,
					     CutSiHits    = 8,
					     useVertexCleaning  = True,
					     MultiWithOneTrkVrt = True,
					     removeTrkMatSignif = -1.,    # No additional material rejection
					     AntiPileupSigRCut = 2.0,
					     TrkSigCut         = 2.0,
					     SelVrtSigCut      = 3.0,
					     v2tIniBDTCut      =-0.6,
					     v2tFinBDTCut      = 0.2,
					     CutPt             = 500,
					     MaxSVRadiusCut    = 50        # Inside Pixel BL volume
                                             )

##########################################################################################################
# define the class
class InclusiveBFinderTool( Rec__NewVrtSecInclusiveTool ):

    def __init__(self, name = 'SoftBFinderTool'  ):        

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'InclusiveBFinderTool::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="SVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator")
        ToolSvc += SVertexFitterTool
        #----------------------
        # Soft B-hadron vertex finder itself
        #
        Rec__NewVrtSecInclusiveTool.__init__( self, name = name,
                                             VertexFitterTool   = SVertexFitterTool,
					     CutBLayHits  = 0,
					     CutPixelHits = 2,
					     CutSiHits    = 8,
					     useVertexCleaning  = True,
					     MultiWithOneTrkVrt = True,
					     removeTrkMatSignif = -1.,    # No additional material rejection
					     AntiPileupSigRCut = 2.0,
					     TrkSigCut         = 2.0,
					     SelVrtSigCut      = 3.0,
					     v2tIniBDTCut      =-0.5,
					     v2tFinBDTCut      = 0.0,
					     CutPt             = 500
                                             )
##########################################################################################################
# define the class
class HighPtBFinderTool( Rec__NewVrtSecInclusiveTool ):

    def __init__(self, name = 'SoftBFinderTool'  ):        

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'InclusiveBFinderTool::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="SVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator")
        ToolSvc += SVertexFitterTool
        #----------------------
        # Soft B-hadron vertex finder itself
        #
        Rec__NewVrtSecInclusiveTool.__init__( self, name = name,
                                             VertexFitterTool   = SVertexFitterTool,
					     CutBLayHits  = 0,
					     CutPixelHits = 2,
					     CutSiHits    = 8,
					     useVertexCleaning  = True,
					     MultiWithOneTrkVrt = True,
					     removeTrkMatSignif = -1.,    # No additional material rejection
					     AntiPileupSigRCut = 2.0,
					     TrkSigCut         = 2.0,
					     SelVrtSigCut      = 3.0,
					     v2tIniBDTCut      =-0.6,
					     v2tFinBDTCut      = 0.2,
					     CutPt             = 1000
                                             )
##########################################################################################################
# define the class
class MaterialSVFinderTool( Rec__NewVrtSecInclusiveTool ):

    def __init__(self, name = 'MaterialSVFinderTool'  ):        

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'MaterialFinderTool::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="SVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator")
        ToolSvc += SVertexFitterTool
        #----------------------
        # Soft B-hadron vertex finder itself
        #
        Rec__NewVrtSecInclusiveTool.__init__( self, name = name,
                                             VertexFitterTool   = SVertexFitterTool,
					     CutBLayHits  = 0,
					     CutPixelHits = 1,
					     CutSiHits    = 8,
					     useVertexCleaning  = False,
					     MultiWithOneTrkVrt = False,
					     removeTrkMatSignif = -1.,    # No additional material rejection
					     AntiPileupSigRCut = 5.0,
					     TrkSigCut         = 5.0,
					     SelVrtSigCut      = 10.0,
					     v2tIniBDTCut      =-10.,     #Effectively remove MVA selection
					     v2tFinBDTCut      =-10.,     #Effectively remove MVA selection
					     VrtMassLimit      = 8000.,
					     Vrt2TrMassLimit   = 8000.,
					     CutPt             = 500.
                                             )
##########################################################################################################
# define the class
class DVFinderTool( Rec__NewVrtSecInclusiveTool ):

    def __init__(self, name = 'MaterialSVFinderTool'  ):        

        from __main__ import ToolSvc
        mlog = logging.getLogger( 'MaterialFinderTool::__init__ ' )
        mlog.info("entering")
        #----------------------
        # VKalVrt vertex fitter
        # 
        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
        SVertexFitterTool = Trk__TrkVKalVrtFitter(name="SVertexFitterTool",
                                                  Extrapolator="Trk::Extrapolator/AtlasExtrapolator")
        ToolSvc += SVertexFitterTool
        #----------------------
        # Soft B-hadron vertex finder itself
        #
        Rec__NewVrtSecInclusiveTool.__init__( self, name = name,
                                             VertexFitterTool   = SVertexFitterTool,
					     CutBLayHits  = 0,
					     CutPixelHits = 0,
					     CutSiHits    = 0,
					     useVertexCleaning  = False,
					     MultiWithOneTrkVrt = False,
					     removeTrkMatSignif = 2.,
					     AntiPileupSigRCut = 6.0,
					     TrkSigCut         = 6.0,
					     SelVrtSigCut      = 8.0,
					     v2tIniBDTCut      =-0.5,
					     v2tFinBDTCut      = 0.0,
					     VrtMassLimit      = 50000.,
					     Vrt2TrMassLimit   = 50000.,
					     CutPt             = 1000.
                                             )
