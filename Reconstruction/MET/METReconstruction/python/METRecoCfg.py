# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

#################################################################################
# Define some default values


defaultSelection = {
    'Ele':'Medium',
    'Gamma':'Tight',
}

defaultAuthor = {
    'Ele':17,
    'Gamma':20,
}

defaultInputKey = {
   'Ele'      :'Electrons',
   'Gamma'    :'Photons',
   'Tau'      :'TauJets',
   'Jet'      :'AntiKt4EMTopoJets',
   'Muon'     :'Muons',
   'SoftTrk'  :'InDetTrackParticles',
   'SoftClus' :'CaloCalTopoClusters',
   'SoftPFlow':'JetETMissNeutralParticleFlowObjects',
   'PrimaryVx':'PrimaryVertices',
   'Truth'    :'TruthEvents',
   'Calo'     :'AllCalo',
   'LCOCSoftClus':'LCOriginTopoClusters',
   'EMOCSoftClus':'EMOriginTopoClusters',
   }

defaultOutputKey = {
    'Ele'      :'RefEle',
    'Gamma'    :'RefGamma',
    'Tau'      :'RefTau',
    'Jet'      :'RefJet',
    'Muon'     :'Muons',
    'SoftTrk'  :'SoftTrk',
    'SoftClus' :'SoftClus',
    'SoftPFlow':'SoftPFlow',
    'Total'    :'Final',
    'Truth'    :'Truth',
    'Calo'     :'Calo'
    }

prefix = 'METRecoConfig:   '

#################################################################################
# Configuration of builders

class BuildConfig:
    def __init__(self,objType='',outputKey='',inputKey=''):
        self.objType = objType
        self.outputKey = outputKey
        self.inputKey = inputKey

def getBuilder(config,suffix,doTracks,doCells,doTriggerMET,doOriginCorrClus):
    tool = None
    # Construct tool and set defaults for case-specific configuration
    if config.objType == 'SoftTrk':
        tool = CfgMgr.met__METSoftTermsTool('MET_SoftTrkTool_'+suffix)
        tool.InputComposition = 'Tracks'
    if config.objType.endswith('SoftClus'):
        tool = CfgMgr.met__METSoftTermsTool('MET_SoftClusTool_'+suffix)
        tool.InputComposition = 'Clusters'
    if config.objType == 'SoftPFlow':
        tool = CfgMgr.met__METSoftTermsTool('MET_SoftPFlowTool_'+suffix)
        tool.InputComposition = 'PFlow'
        pfotool = CfgMgr.CP__RetrievePFOTool('MET_PFOTool_'+suffix)
        tool.PFOTool = pfotool
    if suffix == 'Truth':
        tool = CfgMgr.met__METTruthTool('MET_TruthTool_'+config.objType)
        tool.InputComposition = config.objType
        config.inputKey = defaultInputKey['Truth']
        config.outputKey = config.objType
    if suffix == 'Calo':
        from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
        CaloNoiseCondAlg ('totalNoise')
        tool = CfgMgr.met__METCaloRegionsTool('MET_CaloRegionsTool')
        if doCells:
            tool.UseCells     = True
            tool.DoTriggerMET = doTriggerMET
            config.inputKey   = defaultInputKey['Calo'] 
        else:
            tool.UseCells     = False                   
            tool.DoTriggerMET = False
            config.inputKey   = defaultInputKey['SoftClus']
        config.outputKey = config.objType

    # set input/output key names
    if config.inputKey == '':
        tool.InputCollection = defaultInputKey[config.objType]
        config.inputKey = tool.InputCollection
    else:
        tool.InputCollection = config.inputKey
    if not suffix=='Calo':
        if config.outputKey == '':
            tool.MissingETKey = defaultOutputKey[config.objType]
            config.outputKey = tool.MissingETKey
        else:
            tool.MissingETKey = config.outputKey
    return tool

#################################################################################
# Configuration of refiners

class RefConfig:
    def __init__(self,myType='',outputKey=''):
        self.type = myType
        self.outputKey = outputKey

def getRefiner(config,suffix,trkseltool=None,trkvxtool=None,trkisotool=None,caloisotool=None):
    tool = None

    if config.type == 'TrackFilter':
        tool = CfgMgr.met__METTrackFilterTool('MET_TrackFilterTool_'+suffix)
        tool.InputPVKey = defaultInputKey['PrimaryVx']
        tool.TrackSelectorTool=trkseltool
        tool.TrackVxAssocTool=trkvxtool
        #
        tool.UseIsolationTools = False #True
        tool.TrackIsolationTool = trkisotool
        tool.CaloIsolationTool = caloisotool
        from METReconstruction.METRecoFlags import metFlags
        tool.DoPVSel = metFlags.UseTracks()
        tool.DoVxSep = metFlags.UseTracks()
    tool.MissingETKey = config.outputKey
    return tool

#################################################################################
# Region tools are a special case of refiners

def getRegions(config,suffix):
    if suffix == 'Truth':
        config.outputKey = config.objType
    tool = CfgMgr.met__METRegionsTool('MET_'+config.outputKey+'Regions_'+suffix)
    tool.InputMETContainer = 'MET_'+suffix
    tool.InputMETMap = 'METMap_'+suffix
    tool.InputMETKey = config.outputKey
    tool.RegionValues = [ 1.5, 3.2, 10 ]
    return tool

#################################################################################
# Top level MET configuration

class METConfig:
    def outputCollection(self):
        return 'MET_'+self.suffix
    #
    def outputMap(self):
        return 'METMap_'+self.suffix
    #
    def setupBuilders(self,buildconfigs):
        print prefix, 'Setting up builders for MET config '+self.suffix
        for config in buildconfigs:
            if config.objType in self.builders:
                print prefix, 'Config '+self.suffix+' already contains a builder of type '+config.objType
                raise LookupError
            else:
                builder = getBuilder(config,self.suffix,self.doTracks,self.doCells,
                                     self.doTriggerMET,self.doOriginCorrClus)
                self.builders[config.objType] = builder
                self.buildlist.append(builder)
                print prefix, '  Added '+config.objType+' tool named '+builder.name()
    #
    def setupRefiners(self,refconfigs):
        print prefix, 'Setting up refiners for MET config '+self.suffix
        for config in refconfigs:
            # need to enforce this?
            if config.type in self.refiners:
                print 'Config '+self.suffix+' already contains a refiner of type '+config.type
                raise LookupError
            else:
                refiner = getRefiner(config=config,suffix=self.suffix,
                                     trkseltool=self.trkseltool,trkvxtool=self.trkvxtool,
                                     trkisotool=self.trkisotool,caloisotool=self.caloisotool)
                self.refiners[config.type] = refiner
                self.reflist.append(refiner)
                print prefix, '  Added '+config.type+' tool named '+refiner.name()
    #
    def setupRegions(self,buildconfigs):
        print prefix, 'Setting up regions for MET config '+self.suffix
        for config in buildconfigs:
            if config.objType in self.regions:
                print prefix, 'Config '+self.suffix+' already contains a region tool of type '+config.objType
                raise LookupError
            else:
                regions = getRegions(config,self.suffix)
                self.regions[config.objType] = regions
                self.reglist.append(regions)
                print prefix, '  Added '+config.objType+' region tool named '+regions.name()
    #
    def __init__(self,suffix,buildconfigs=[],refconfigs=[],
                 doTracks=False,doSum=False,doRegions=False,
                 doCells=False,doTriggerMET=True,duplicateWarning=True,
                 doOriginCorrClus=False):
        print prefix, 'Creating MET config \''+suffix+'\''
        self.suffix = suffix
        self.doSum = doSum
        self.doTracks = doTracks
        self.doRegions = doRegions
        self.doCells = doCells,
        self.doOriginCorrClus = doOriginCorrClus
        self.doTriggerMET = doTriggerMET
        self.duplicateWarning = duplicateWarning
        #
        self.builders = {}
        self.buildlist = [] # need an ordered list
        #
        self.refiners = {}
        self.reflist = [] # need an ordered list
        #
        self.regions = {}
        self.reglist = [] # need an ordered list
        if doRegions:
            self.setupRegions(buildconfigs)
        #
        self.trkseltool=CfgMgr.InDet__InDetTrackSelectionTool("IDTrkSel_MET",
                                                              CutLevel="TightPrimary",
                                                              maxZ0SinTheta=3,
                                                              maxD0=2,
                                                              minPt=500)
        #
        self.trkvxtool=CfgMgr.CP__TrackVertexAssociationTool("TrackVertexAssociationTool_MET", WorkingPoint="Nominal")
        #self.trkvxtool=CfgMgr.CP__TightTrackVertexAssociationTool("TightTrackVertexAssociationTool_MET", dzSinTheta_cut=3, doPV=False)
        #
        self.trkisotool = CfgMgr.xAOD__TrackIsolationTool("TrackIsolationTool_MET")
        self.trkisotool.TrackSelectionTool = self.trkseltool # As configured above
        #
        from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool, Rec__ParticleCaloCellAssociationTool            
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        CaloExtensionTool= Trk__ParticleCaloExtensionTool(Extrapolator = AtlasExtrapolator())
        CaloCellAssocTool =  Rec__ParticleCaloCellAssociationTool(ParticleCaloExtensionTool = CaloExtensionTool)
        self.caloisotool = CfgMgr.xAOD__CaloIsolationTool("CaloIsolationTool_MET",
                                                          saveOnlyRequestedCorrections=True,
                                                          addCaloExtensionDecoration=False,
                                                          ParticleCaloExtensionTool = CaloExtensionTool,
                                                          ParticleCaloCellAssociationTool = CaloCellAssocTool)

        self.setupBuilders(buildconfigs)
        self.setupRefiners(refconfigs)

# Set up a top-level tool with mostly defaults
def getMETRecoTool(topconfig):
    recoTool = CfgMgr.met__METRecoTool('MET_RecoTool_'+topconfig.suffix,
                                       METBuilders = topconfig.buildlist,
                                       METRefiners = topconfig.reflist,
                                       METContainer = topconfig.outputCollection(),
                                       METComponentMap = topconfig.outputMap(),
                                       WarnIfDuplicate = topconfig.duplicateWarning,
                                       TimingDetail=0)
    if topconfig.doSum:
        recoTool.METFinalName = defaultOutputKey['Total']    

    return recoTool

# Set up a METRecoTool that builds MET regions
def getRegionRecoTool(topconfig):
    regTool = CfgMgr.met__METRecoTool('MET_RegionTool_'+topconfig.suffix,
                                       METBuilders = [],
                                       METRefiners = topconfig.reglist,
                                       METContainer = topconfig.outputCollection()+'Regions',
                                       METComponentMap = topconfig.outputMap()+'Regions',
                                       WarnIfDuplicate = topconfig.duplicateWarning)
    return regTool

# Allow user to configure reco tools directly or get more default configurations
def getMETRecoAlg(algName='METReconstruction',configs={},tools=[]):

    recoTools = []
    recoTools += tools
    print 'Setting up MET Reconstruction'
    for key,conf in configs.iteritems():
        print prefix, 'Generate METRecoTool for MET_'+key
        recotool = getMETRecoTool(conf)
	print recotool
        recoTools.append(recotool)
        #metFlags.METRecoTools()[key] = recotool
        if conf.doRegions:
            regiontool = getRegionRecoTool(conf)
            recoTools.append(regiontool)
	    print "Added region tool"
    for tool in recoTools:
        print prefix, 'Added METRecoTool \''+tool.name()+'\' to alg '+algName

    recoAlg = CfgMgr.met__METRecoAlg(name=algName,
                                     RecoTools=recoTools)
    return recoAlg
