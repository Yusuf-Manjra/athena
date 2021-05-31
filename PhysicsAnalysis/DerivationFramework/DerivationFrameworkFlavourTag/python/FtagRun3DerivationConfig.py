# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import findAllAlgorithms
from AthenaCommon.AthenaCommonFlags import jobproperties as jps

from GaudiKernel.Configurable import WARNING

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from AthenaConfiguration.ComponentFactory import CompFactory

# for backward compatability
def FtagJetCollection(jetcol, seq, OutputLevel=WARNING):
    FtagJetCollections([jetcol], seq, OutputLevel)

# this should be able to tag a few collections
def FtagJetCollections(jetcols, seq, OutputLevel=WARNING):

    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    taggerlist = ['IP2D', 'IP3D', 'SV1', 'SoftMu']

    setupCondDb(cfgFlags, taggerlist)

    acc = ComponentAccumulator()

    if 'AntiKt4EMTopoJets' in jetcols:
        acc.merge(RenameInputContainerEmTopoHacksCfg('oldAODVersion'))

    for jetcol in jetcols:
        acc.merge(getFtagComponent(cfgFlags, jetcol, taggerlist, OutputLevel))

    Configurable.configurableRun3Behavior=0
    algs = findAllAlgorithms(acc.getSequence("AthAlgSeq"))
    for alg in algs:

        seq += conf2toConfigurable(alg)

    acc.wasMerged()


# this returns a component accumulator, which is merged across jet
# collections in FtagJetCollections above
def getFtagComponent(cfgFlags, jetcol, taggerlist, OutputLevel=WARNING):

    from BTagging.JetParticleAssociationAlgConfig import JetParticleAssociationAlgCfg
    from BTagging.JetBTaggingAlgConfig import JetBTaggingAlgCfg
    from BTagging.JetSecVertexingAlgConfig import JetSecVertexingAlgCfg
    from BTagging.JetSecVtxFindingAlgConfig import JetSecVtxFindingAlgCfg
    from BTagging.BTagTrackAugmenterAlgConfig import BTagTrackAugmenterAlgCfg
    from BTagging.BTagHighLevelAugmenterAlgConfig import BTagHighLevelAugmenterAlgCfg
    from BTagging.HighLevelBTagAlgConfig import HighLevelBTagAlgCfg

    jetcol_name_without_Jets = jetcol.replace('Jets','')
    BTaggingCollection = cfgFlags.BTagging.OutputFiles.Prefix + jetcol_name_without_Jets

    kwargs = {}
    kwargs['Release'] = '22'

    cfgFlags.Input.Files = jps.AthenaCommonFlags.FilesInput.get_Value()

    acc = ComponentAccumulator()

    acc.merge(JetParticleAssociationAlgCfg(cfgFlags, jetcol_name_without_Jets, "InDetTrackParticles", 'BTagTrackToJetAssociator', **kwargs))

    SecVertexingAndAssociators = {'JetFitter':'BTagTrackToJetAssociator','SV1':'BTagTrackToJetAssociator'}
    for k, v in SecVertexingAndAssociators.items():

        acc.merge(JetSecVtxFindingAlgCfg(cfgFlags, jetcol_name_without_Jets, "PrimaryVertices", k, v))

        acc.merge(JetSecVertexingAlgCfg(cfgFlags, BTaggingCollection, jetcol_name_without_Jets, "PrimaryVertices", k, v))


    acc.merge( JetBTaggingAlgCfg(cfgFlags, BTaggingCollection = BTaggingCollection, JetCollection = jetcol_name_without_Jets, PrimaryVertexCollectionName="PrimaryVertices", TaggerList = taggerlist, SVandAssoc = SecVertexingAndAssociators) )


    postTagDL2JetToTrainingMap={
        'AntiKt4EMPFlow': [
            'BTagging/201903/rnnip/antikt4empflow/network.json',
            'BTagging/201903/dl1r/antikt4empflow/network.json',
            'BTagging/201903/dl1/antikt4empflow/network.json',
            'BTagging/20210517/dipsLoose/antikt4empflow/network.json',
            'BTagging/20210517/dips/antikt4empflow/network.json',
            'BTagging/20210519r22/dl1r/antikt4empflow/network.json',
            'BTagging/20210528r22/dl1d/antikt4empflow/network.json',
        ],
        'AntiKt4EMTopo': [
            'BTagging/201903/rnnip/antikt4empflow/network.json',
            'BTagging/201903/dl1r/antikt4empflow/network.json',
            'BTagging/201903/dl1/antikt4empflow/network.json',
            'BTagging/20210517/dipsLoose/antikt4empflow/network.json',
            'BTagging/20210517/dips/antikt4empflow/network.json',
            'BTagging/20210519r22/dl1r/antikt4empflow/network.json',
            'BTagging/20210528r22/dl1d/antikt4empflow/network.json',
        ],
        'AntiKtVR30Rmax4Rmin02Track': [
            'BTagging/201903/rnnip/antiktvr30rmax4rmin02track/network.json',
            'BTagging/201903/dl1r/antiktvr30rmax4rmin02track/network.json',
            'BTagging/201903/dl1/antiktvr30rmax4rmin02track/network.json',
            'BTagging/20210517/dipsLoose/antikt4empflow/network.json',
            'BTagging/20210517/dips/antikt4empflow/network.json',
            'BTagging/20210519r22/dl1r/antikt4empflow/network.json',
            'BTagging/20210528r22/dl1d/antikt4empflow/network.json',
        ]
    }

    acc.merge(BTagTrackAugmenterAlgCfg(cfgFlags))

    acc.merge(BTagHighLevelAugmenterAlgCfg(cfgFlags, JetCollection=jetcol_name_without_Jets, BTagCollection=BTaggingCollection, Associator='BTagTrackToJetAssociator'))

    for jsonfile in postTagDL2JetToTrainingMap[jetcol_name_without_Jets]:
        acc.merge(HighLevelBTagAlgCfg(cfgFlags, BTaggingCollection=BTaggingCollection, TrackCollection='InDetTrackParticles', NNFile=jsonfile) )

    return acc

# this probably only has to happen once
def setupCondDb(cfgFlags, taggerlist):
    from AthenaCommon.AppMgr import athCondSeq
    CalibrationChannelAliases = ["AntiKt4EMPFlow->AntiKt4EMPFlow,AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo"]
    grades= cfgFlags.BTagging.Grades
    RNNIPConfig = {'rnnip':''}

    JetTagCalibCondAlg=CompFactory.Analysis.JetTagCalibCondAlg
    jettagcalibcondalg = "JetTagCalibCondAlg"
    readkeycalibpath = "/GLOBAL/BTagCalib/RUN12"
    connSchema = "GLOBAL_OFL"
    if not cfgFlags.Input.isMC:
        readkeycalibpath = readkeycalibpath.replace("/GLOBAL/BTagCalib","/GLOBAL/Onl/BTagCalib")
        connSchema = "GLOBAL"
    histoskey = "JetTagCalibHistosKey"
    from IOVDbSvc.CondDB import conddb

    conddb.addFolder(connSchema, readkeycalibpath, className='CondAttrListCollection')
    JetTagCalib = JetTagCalibCondAlg(jettagcalibcondalg, ReadKeyCalibPath=readkeycalibpath, HistosKey = histoskey, taggers = taggerlist,
        channelAliases = CalibrationChannelAliases, IP2D_TrackGradePartitions = grades, RNNIP_NetworkConfig = RNNIPConfig)

    athCondSeq+=conf2toConfigurable( JetTagCalib, indent="  " )



# Valerio's magic hacks for emtopo
def RenameInputContainerEmTopoHacksCfg(suffix):

    acc=ComponentAccumulator()

    #Delete BTagging container read from input ESD
    AddressRemappingSvc, ProxyProviderSvc=CompFactory.getComps("AddressRemappingSvc","ProxyProviderSvc",)
    AddressRemappingSvc = AddressRemappingSvc("AddressRemappingSvc")
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMTopoJets.BTagTrackToJetAssociator->AntiKt4EMTopoJets.BTagTrackToJetAssociator_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMTopoJets.JFVtx->AntiKt4EMTopoJets.JFVtx_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMTopoJets.SecVtx->AntiKt4EMTopoJets.SecVtx_' + suffix]

    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMTopoJets.btaggingLink->AntiKt4EMTopoJets.btaggingLink_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingContainer#BTagging_AntiKt4EMTopo->BTagging_AntiKt4EMTopo_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingAuxContainer#BTagging_AntiKt4EMTopoAux.->BTagging_AntiKt4EMTopo_' + suffix+"Aux."]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::VertexContainer#BTagging_AntiKt4EMTopoSecVtx->BTagging_AntiKt4EMTopoSecVtx_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::VertexAuxContainer#BTagging_AntiKt4EMTopoSecVtxAux.->BTagging_AntiKt4EMTopoSecVtx_' + suffix+"Aux."]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTagVertexContainer#BTagging_AntiKt4EMTopoJFVtx->BTagging_AntiKt4EMTopoJFVtx_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTagVertexAuxContainer#BTagging_AntiKt4EMTopoJFVtxAux.->BTagging_AntiKt4EMTopoJFVtx_' + suffix+"Aux."]
    acc.addService(AddressRemappingSvc)
    acc.addService(ProxyProviderSvc(ProviderNames = [ "AddressRemappingSvc" ]))
    return acc
