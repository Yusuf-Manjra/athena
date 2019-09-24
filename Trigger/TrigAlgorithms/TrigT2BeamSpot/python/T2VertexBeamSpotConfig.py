# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
# $Id: T2VertexBeamSpotConfig.py 775662 2016-09-28 15:46:38Z masik $
from TrigT2BeamSpot.TrigT2BeamSpotConf import PESA__T2VertexBeamSpot
from TrigVertexFitter.TrigVertexFitterConf import TrigPrimaryVertexFitter
from AthenaCommon.AppMgr import ToolSvc

# TrigPrimaryVertexFitter: D. Emeliyanov (author)
primaryVertexFitter = TrigPrimaryVertexFitter(zVariance=3.0, CreateTrackLists=True)
ToolSvc += primaryVertexFitter

#Adding new monitoring tool
from TrigT2BeamSpot.T2VertexBeamSpotMonitoring import  T2VertexBeamSpotAlgMonitoring, T2VertexBeamSpotToolMonitoring
validation = T2VertexBeamSpotToolMonitoring()
bsAlgMonitoring = T2VertexBeamSpotAlgMonitoring()


#TODO: create an instance which can be called and adjusted
from TrigT2BeamSpot.TrigT2BeamSpotConf import PESA__T2VertexBeamSpotTool
InDetTrigMTBeamSpotTool = PESA__T2VertexBeamSpotTool( name = "TestBeamSpotTool",
                                                      MonTool = validation,
                                                      WeightClusterZ      = True,     # Use the track Z0 weighted cluster Z position as seed            
                                                      ReclusterSplit      = False,    # Recluster split track collections before vertex fitting
                                                      nSplitVertices      = 1,        # Turn on (>1) or off vertex splitting
                                                      TotalNTrackMin      = 4,        # Minimum number of tracks required in an event
                                                      TrackMinPt          = 0.5,      # Minimum track pT to be considered for vertexing
                                                      TrackSeedPt         = 0.7,      # Minimum track pT to be considered for seeding a vertex fit
                                                      TrackClusterDZ      = 0.35,      # Maximum distance between tracks considered as a cluster
                                                      TrackMaxZ0          = 200.0,    # Maximum track Z0 to be considered for vertexing
                                                      TrackMaxD0          = 10.0,     # Maximum track d0 to be considered for vertexing
                                                      TrackMaxZ0err       = 5.0,      # Maximum track Z0 error to be considered for vertexing
                                                      TrackMaxD0err       = 5.0,      # Maximum track d0 error to be considered for vertexing
                                                      TrackMinNDF         = 2.0,      # Minimum track NDF to be considered for vertexing
                                                      TrackMinQual        = 0.0,      # Minimum track chi^2/NDF to be considered for vertexing
                                                      TrackMaxQual        = 10.0,     # Maximum track chi^2/NDF to be considered for vertexing
                                                      TrackMinChi2Prob    = -10.0,    # Minimum track cumulative chi2 probability, from CLHEP/GenericFunctions/CumulativeChiSquare.hh 
                                                      TrackMinSiHits      = 7,        # Minimum # track silicon (PIX + SCT) hits to be considered for vertexing
                                                      TrackMinPIXHits     = 0,        # Minimum # track silicon (PIX + SCT) hits to be considered for vertexing
                                                      TrackMinSCTHits     = 0,        # Minimum # track silicon (PIX + SCT) hits to be considered for vertexing
                                                      TrackMinTRTHits     = -10,      # Minimum # track TRT hits to be considered for vertexing

                                                      VertexMinNTrk       = 2,        # Minimum # tracks in a cluster to be considered for vertexing
                                                      VertexMaxNTrk       = 100,      # Maximum # tracks in a cluster to be considered for vertexing (saves on time!)
                                                      VertexMaxXerr       = 1.,       # Maximum resulting X error on vertex fit for "good" vertices
                                                      VertexMaxYerr       = 1.,       # Maximum resulting Y error on vertex fit for "good" vertices
                                                      VertexMaxZerr       = 10.,      # Maximum resulting Z error on vertex fit for "good" vertices
                                                      VertexMinQual       = 0.0,      # Minimum resulting chi^2/NDF on vertex fit for "good" vertices
                                                      VertexMaxQual       = 100.0,    # Maximum resulting chi^2/NDF on vertex fit for "good" vertices
                                                      VertexMinChi2Prob   = -10.0,    # Minimum cumulative chi2 probability, from CLHEP/GenericFunctions/CumulativeChiSquare.hh      
                                                      VertexBCIDMinNTrk   = 10,       # Minimum # tracks in a vertex to be used for per-BCID monitoring
                                                      PrimaryVertexFitter = primaryVertexFitter )


class T2VertexBeamSpot_Fex ( PESA__T2VertexBeamSpot ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_Fex", detail=1):
        super(T2VertexBeamSpot_Fex, self).__init__(name)
        self.attachVertices      = True     # Write the good vertices to StoreGate
        self.attachSplitVertices = False    # Write split vertices to StoreGate
        self.BeamSpotTool = InDetTrigMTBeamSpotTool    
        self.MonTool = bsAlgMonitoring
        
# Setup for relaxed cuts at 900 GeV LHC center-of-mass
class T2VertexBeamSpot_loose ( T2VertexBeamSpot_Fex ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_loose"):
        super(T2VertexBeamSpot_loose, self).__init__(name)
        self.TotalNTrackMin  = 2        # Minimum number of tracks required in an event
        self.TrackMinPt      = 0.5      # Minimum track pT to be considered for vertexing
        self.TrackSeedPt     = 0.7      # Minimum track pT to be considered for seeding a vertex fit
        self.TrackClusterDZ  = 10.0     # Maximum distance between tracks considered as a cluster
        self.TrackMinTRTHits = -10      # Minimum # track TRT hits to be considered for vertexing
        self.nSplitVertices  = 1        # Turn OFF vertex splitting
        self.vertexCollName  = "T2VertexBeamSpot_loose"


# Generic base classes for three differen types of chains:
# default:       never activate the output TE
# activateTE:    do activate the output TE on events that have a good vertex
# activateAllTE: indiscriminately active the output TE on all events

# Setup for not writing out any events
class T2VertexBeamSpot ( T2VertexBeamSpot_Fex ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot", detail=1):
        super(T2VertexBeamSpot, self).__init__(name, detail)
        self.activateTE     = False  # Activate the output TE if a good vertex is found
        self.activateAllTE  = False  # Activate the output TE for all events where this trigger is run
        self.nSplitVertices = 2      # Turn on vertex splitting
        self.vertexCollName = name

# Setup for writing out all events seen by the BeamSpot algorithm
class T2VertexBeamSpot_activeAllTE ( T2VertexBeamSpot_Fex ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeAllTE", detail=1):
        super(T2VertexBeamSpot_activeAllTE, self).__init__(name, detail)
        self.activateTE     = False  # Activate the output TE if a good vertex is found
        self.activateAllTE  = True   # Activate the output TE for all events where this trigger is run

# Setup for writing out events with a "good" vertex seen by the BeamSpot algorithm
class T2VertexBeamSpot_activeTE ( T2VertexBeamSpot_Fex ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeTE", detail=1):
        super(T2VertexBeamSpot_activeTE, self).__init__(name, detail)
        self.activateTE     = True    # Activate the output TE if a good vertex is found
        self.activateAllTE  = False   # Activate the output TE for active events where this trigger is run
        self.nSplitVertices = 1       # Turn OFF vertex splitting
        self.vertexCollName = name

# Setup for using FTF tracks
class T2VertexBeamSpot_FTF ( T2VertexBeamSpot ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_FTF"):
        super(T2VertexBeamSpot_FTF, self).__init__(name, detail=2)

# Setup for writing out all events seen by the BeamSpot algorithm with FTF tracks
class T2VertexBeamSpot_activeAllTE_FTF ( T2VertexBeamSpot_activeAllTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeAllTE_FTF"):
        super(T2VertexBeamSpot_activeAllTE_FTF, self).__init__(name, detail=0)

# Setup for writing out events with a "good" vertex seen by the BeamSpot algorithm with FTF tracks
class T2VertexBeamSpot_activeTE_FTF ( T2VertexBeamSpot_activeTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeTE_FTF"):
        super(T2VertexBeamSpot_activeTE_FTF, self).__init__(name, detail=0)

 # Setup for using FTK tracks
class T2VertexBeamSpot_FTK ( T2VertexBeamSpot ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_FTK"):
        super(T2VertexBeamSpot_FTK, self).__init__(name, detail=2)
 
# Setup for writing out all events seen by the BeamSpot algorithm with FTK tracks
class T2VertexBeamSpot_activeAllTE_FTK ( T2VertexBeamSpot_activeAllTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeAllTE_FTK"):
        super(T2VertexBeamSpot_activeAllTE_FTK, self).__init__(name, detail=0)
 
# Setup for writing out events with a "good" vertex seen by the BeamSpot algorithm with FTK tracks
class T2VertexBeamSpot_activeTE_FTK ( T2VertexBeamSpot_activeTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeTE_FTK"):
        super(T2VertexBeamSpot_activeTE_FTK, self).__init__(name, detail=0)

 # Setup for using FTKRefit tracks
class T2VertexBeamSpot_FTKRefit ( T2VertexBeamSpot ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_FTKRefit"):
        super(T2VertexBeamSpot_FTKRefit, self).__init__(name, detail=2)
 
# Setup for writing out all events seen by the BeamSpot algorithm with FTK tracks
class T2VertexBeamSpot_activeAllTE_FTKRefit ( T2VertexBeamSpot_activeAllTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeAllTE_FTKRefit"):
        super(T2VertexBeamSpot_activeAllTE_FTKRefit, self).__init__(name, detail=0)
 
# Setup for writing out events with a "good" vertex seen by the BeamSpot algorithm with FTK tracks
class T2VertexBeamSpot_activeTE_FTKRefit ( T2VertexBeamSpot_activeTE ) :
    __slots__ = []
    def __init__ (self, name="T2VertexBeamSpot_activeTE_FTKRefit"):
        super(T2VertexBeamSpot_activeTE_FTKRefit, self).__init__(name, detail=0)

