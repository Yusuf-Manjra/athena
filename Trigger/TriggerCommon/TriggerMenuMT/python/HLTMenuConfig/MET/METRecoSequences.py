#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#


from .AlgInputConfig import AlgInputConfig, InputConfigRegistry
from AthenaCommon.Logging import logging

from ..Menu.MenuComponents import RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags

import copy

log = logging.getLogger(__name__)


def jetRecoDictForMET(**recoDict):
    """ Get a jet reco dict that's usable for the MET slice """
    from ..Jet.JetRecoConfiguration import interpretJetCalibDefault
    from ..Jet.JetRecoConfiguration import recoKeys as jetRecoKeys
    from ..Menu.SignatureDicts import JetChainParts_Default

    jrd = {
        k: recoDict.get(k, JetChainParts_Default[k])
        for k in jetRecoKeys
        if not k == "cleaning"
    }
    jrd.update({"cleaning": "noCleaning"})
    # Rename the cluster calibration
    try:
        jrd["clusterCalib"] = recoDict["calib"]
    except KeyError:
        pass
    # Fill constitMod
    jrd["constitMod"] = recoDict.get("constitmod", "")
    # We only use em calibration for PFOs
    if jrd["constitType"] == "pf":
        jrd["clusterCalib"] = "em"
    # Interpret jet calibration
    if jrd["jetCalib"] == "default":
        jrd["jetCalib"] = interpretJetCalibDefault(jrd)
    return jrd


default_inputs = InputConfigRegistry()


class CellInputConfig(AlgInputConfig):
    def __init__(self):
        super().__init__(produces=["Cells"], step=0)

    def dependencies(self, recoDict):
        # Cells have no input dependencies
        return []

    def create_sequence(self, inputs, RoIs, recoDict):
        from ..CommonSequences.CaloSequences import cellRecoSequence

        cellSeq, cellName = RecoFragmentsPool.retrieve(
            cellRecoSequence, flags=None, RoIs=RoIs
        )
        return cellSeq, {"Cells": cellName}

    def create_accumulator(self, flags, inputs, RoIs, recoDict):
        from TrigCaloRec.TrigCaloRecConfig import hltCaloCellMakerCfg

        comp_name = "HLTCaloCellMaker_FS"
        acc = hltCaloCellMakerCfg(flags, name=comp_name, roisKey=RoIs)
        return acc, {"Cells": acc.getEventAlgo(comp_name).CellsName}


default_inputs.add_input(CellInputConfig())


class ClusterInputConfig(AlgInputConfig):
    def __init__(self):
        super().__init__(produces=["Clusters"], step=0)

    def dependencies(self, recoDict):
        # Clusters have no input dependencies
        return []

    def create_sequence(self, inputs, RoIs, recoDict):
        from ..CommonSequences.CaloSequences import (
            caloClusterRecoSequence,
            LCCaloClusterRecoSequence,
        )
        from ..Jet.JetRecoConfiguration import defineJetConstit
        from JetRecConfig.JetRecConfig import getConstitModAlg_nojetdef

        calib = recoDict["calib"]
        if calib == "em":
            tcSeq, clusterName = RecoFragmentsPool.retrieve(
                caloClusterRecoSequence, flags=None, RoIs=RoIs
            )
        elif calib == "lcw":
            tcSeq, clusterName = RecoFragmentsPool.retrieve(
                LCCaloClusterRecoSequence, flags=None, RoIs=RoIs
            )
        else:
            raise ValueError(f"Invalid value for cluster calibration: {calib}")
        sequences = [tcSeq]
        if recoDict.get("constitmod"):
            # Force the datatype to topoclusters
            recoDict = copy.copy(recoDict)
            recoDict["constitType"] = "tc"
            jetRecoDict = jetRecoDictForMET(**recoDict)
            constit = defineJetConstit(jetRecoDict, clustersKey=clusterName)
            # we pass the context argument to make sure the properties inside our JetConstit are tuned according to trkopt
            constit_mod_seq = getConstitModAlg_nojetdef(constit,context=jetRecoDict.get("trkopt","default"))
            sequences.append(constit_mod_seq)
            # Update the name to the modified container name
            clusterName = constit.containername

        return [tcSeq], {"Clusters": clusterName}

    def create_accumulator(self, flags, inputs, RoIs, recoDict):
        from ..CommonSequences.CaloConfig import CaloClusterCfg
        from ..CommonSequences.FullScanDefs import em_clusters, lc_clusters
        from ..Jet.JetRecoConfiguration import defineJetConstit
        from JetRecConfig.JetRecConfig import getConstitModAlg_nojetdef

        if recoDict["calib"] == "em":
            doLC = False
            clustername = em_clusters
        elif recoDict["calib"] == "lcw":
            doLC = True
            clustername = lc_clusters
        else:
            raise ValueError(
                f"Invalid value for cluster calibration: {recoDict['calib']}"
            )

        acc = CaloClusterCfg(flags, doLCCalib=doLC)

        if recoDict.get("constitmod"):
            # Force the datatype to topoclusters
            recoDict = copy.copy(recoDict)
            recoDict["constitType"] = "tc"
            jetRecoDict = jetRecoDictForMET(**recoDict)
            constit = defineJetConstit(jetRecoDict, clustersKey=clustername)
            for a in getConstitModAlg_nojetdef(constit, context=jetRecoDict.get("trkopt","default")):                
                acc.addEventAlgo(a)
            clustername = constit.containername

        return acc, {"Clusters": clustername}


default_inputs.add_input(ClusterInputConfig())


class EMClusterInputConfig(AlgInputConfig):
    """Input config that forces the clusters produced to be at the EM scale

    We have this so that we can force PFOs to be produced at the EM scale,
    however a better solution would probably be to add 'em' to the trigger name
    or to change the 'calib' default to be "default" and then have the algorithm
    choose.
    """

    def __init__(self):
        super().__init__(produces=["EMClusters"], step=0)

    def dependencies(self, recoDict):
        # Clusters have no input dependencies
        return []

    def create_sequence(self, inputs, RoIs, recoDict):
        from ..CommonSequences.CaloSequences import caloClusterRecoSequence

        tcSeq, clusterName = RecoFragmentsPool.retrieve(
            caloClusterRecoSequence, flags=None, RoIs=RoIs
        )
        return [tcSeq], {"EMClusters": clusterName}

    def create_accumulator(self, flags, inputs, RoIs, recoDict):
        from ..CommonSequences.CaloConfig import CaloClusterCfg
        from ..CommonSequences.FullScanDefs import em_clusters

        return CaloClusterCfg(flags, doLCCalib=False), {"EMClusters": em_clusters}


default_inputs.add_input(EMClusterInputConfig())


class TrackingInputConfig(AlgInputConfig):
    def __init__(self):
        from JetRecConfig.StandardJetContext import jetContextDic
        super().__init__(
            produces=copy.copy(jetContextDic['trackKeys']),
            step=1,
        )

    def dependencies(self, recoDict):
        # Tracks have no input dependencies
        return []

    def create_sequence(self, inputs, RoIs, recoDict):
        from ..Jet.JetTrackingConfig import JetTrackingSequence

        trkSeq, trkColls = RecoFragmentsPool.retrieve(
            JetTrackingSequence, flags=None, trkopt="ftf", RoIs=RoIs
        )
        return [trkSeq], trkColls

    def create_accumulator(self, flags, inputs, RoIs, recoDict):
        from ..Jet.JetTrackingConfig import JetTrackingCfg

        return JetTrackingCfg(flags, "ftf", RoIs)


default_inputs.add_input(TrackingInputConfig())


class PFOInputConfig(AlgInputConfig):
    def __init__(self):
        super().__init__(produces=["PFOPrefix", "cPFOs", "nPFOs"])

    def _input_clusters(self, recoDict):
        # Only take the config that forces 'EMClusters' if the calibration
        # isn't set to 'em'. If it's set to em then we want to make sure that
        # we only create those sequences once
        return "Clusters" if recoDict["calib"] == "em" else "EMClusters"

    def dependencies(self, recoDict):
        return [self._input_clusters(recoDict), "Tracks", "Vertices", "TVA", "Cells"]

    def create_sequence(self, inputs, RoIs, recoDict):
        from eflowRec.PFHLTSequence import PFHLTSequence
        from ..Jet.JetRecoConfiguration import defineJetConstit
        from JetRecConfig.JetRecConfig import getConstitModAlg_nojetdef

        pfSeq, pfoPrefix = RecoFragmentsPool.retrieve(
            PFHLTSequence,
            flags=None,
            clustersin=inputs[self._input_clusters(recoDict)],
            tracktype="ftf",
            cellsin=inputs["Cells"],
        )
        # The jet constituent modifier sequence here is to apply the correct weights
        # and decorate the PV matching decoration. If we've specified constituent
        # modifiers those are also applied.
        recoDict = copy.copy(recoDict)
        # Force the jet data type to the correct thing
        recoDict["constitType"] = "pf"
        jetRecoDict = jetRecoDictForMET(trkopt="ftf", **recoDict)
        constit = defineJetConstit(jetRecoDict, pfoPrefix=pfoPrefix)
        # we pass the context argument to make sure the properties inside our JetConstit are tuned according to trkop
        constit_mod_seq = getConstitModAlg_nojetdef(constit,context=jetRecoDict.get("trkopt","default"))
        # Update the PFO prefix
        pfoPrefix = constit.containername
        if pfoPrefix.endswith("ParticleFlowObjects"):
            pfoPrefix = pfoPrefix[:-19]
        allSeqs = [pfSeq]
        if constit_mod_seq:
            allSeqs.append(constit_mod_seq)
        return (
            allSeqs,
            {
                "PFOPrefix": pfoPrefix,
                "cPFOs": pfoPrefix + "ChargedParticleFlowObjects",
                "nPFOs": pfoPrefix + "NeutralParticleFlowObjects",
            },
        )

    def create_accumulator(self, flags, inputs, RoIs, recoDict):
        from eflowRec.PFHLTConfig import PFCfg
        from ..Jet.JetRecoConfiguration import defineJetConstit
        from JetRecConfig.JetRecConfig import getConstitModAlg_nojetdef

        acc = PFCfg(
            flags,
            tracktype="ftf",
            clustersin=inputs[self._input_clusters(recoDict)],
            calclustersin="",
            tracksin=inputs["Tracks"],
            verticesin=inputs["Vertices"],
            cellsin=inputs["Cells"],
        )
        # The jet constituent modifier sequence here is to apply the correct weights
        # and decorate the PV matching decoration. If we've specified constituent
        # modifiers those are also applied.
        recoDict = copy.copy(recoDict)
        # Force the jet data type to the correct thing
        recoDict["constitType"] = "pf"
        jetRecoDict=jetRecoDictForMET(trkopt="ftf", **recoDict)
        constit = defineJetConstit(jetRecoDict, pfoPrefix="HLT_ftf")
        # we pass the context argument to make sure the properties inside our JetConstit are tuned according to trkop        
        acc.addEventAlgo( getConstitModAlg_nojetdef(constit,context=jetRecoDict.get("trkopt","default")) ) # WARNING getConstitModAlg_nojetdef could return None, however this won't happen for PFlow
        # Update the PFO prefix
        pfoPrefix = constit.containername
        if pfoPrefix.endswith("ParticleFlowObjects"):
            pfoPrefix = pfoPrefix[:-19]

        return (
            acc,
            {
                "PFOPrefix": pfoPrefix,
                "cPFOs": pfoPrefix + "ChargedParticleFlowObjects",
                "nPFOs": pfoPrefix + "NeutralParticleFlowObjects",
            },
        )


default_inputs.add_input(PFOInputConfig())


class MergedPFOInputConfig(AlgInputConfig):
    """ Input config for the PFOs merged into a single container """

    def __init__(self):
        super().__init__(produces=["MergedPFOs", "PFOPUCategory"])

    def dependencies(self, recoDict):
        return ["PFOPrefix", "cPFOs", "nPFOs"]

    def create_sequence(self, inputs, RoIs, recoDict):
        from TrigEFMissingET.TrigEFMissingETConf import HLT__MET__PFOPrepAlg

        # Alg generator for RecoFragmentsPool
        # Need to unpack dict as not hashable
        def getPFOPrepAlg(_, **inputs):
            return HLT__MET__PFOPrepAlg(
                f"{inputs['PFOPrefix']}METTrigPFOPrepAlg",
                InputNeutralKey=inputs["nPFOs"],
                InputChargedKey=inputs["cPFOs"],
                OutputKey=f"{inputs['PFOPrefix']}METTrigCombinedParticleFlowObjects",
                OutputCategoryKey="PUClassification",
            )

        prepAlg = RecoFragmentsPool.retrieve(getPFOPrepAlg, None, **inputs)
        return (
            [prepAlg],
            {
                "MergedPFOs": prepAlg.OutputKey,
                "PFOPUCategory": prepAlg.OutputCategoryKey,
            },
        )

    def create_accumulator(self, flags, inputs, RoIs, recoDict):
        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        from AthenaConfiguration.ComponentFactory import CompFactory

        acc = ComponentAccumulator()
        alg = CompFactory.getComp("HLT::MET::PFOPrepAlg")(
            f"{inputs['PFOPrefix']}METTrigPFOPrepAlg",
            InputNeutralKey=inputs["nPFOs"],
            InputChargedKey=inputs["cPFOs"],
            OutputKey=f"{inputs['PFOPrefix']}METTrigCombinedParticleFlowObjects",
            OutputCategoryKey="PUClassification",
        )
        acc.addEventAlgo(alg)
        return (
            acc,
            {
                "MergedPFOs": alg.OutputKey,
                "PFOPUCategory": alg.OutputCategoryKey,
            },
        )


default_inputs.add_input(MergedPFOInputConfig())


class CVFClusterInputConfig(AlgInputConfig):
    def __init__(self):
        super().__init__(produces=["CVF", "CVFPUCategory"])

    def dependencies(self, recoDict):
        return ["Clusters", "Tracks", "Vertices"]

    def create_sequence(self, inputs, RoIs, recoDict):
        from TrigEFMissingET.TrigEFMissingETConf import (
            HLT__MET__CVFAlg,
            HLT__MET__CVFPrepAlg,
            ApproximateTrackToLayerTool,
        )
        from InDetTrackSelectionTool.InDetTrackSelectionToolConf import (
            InDet__InDetTrackSelectionTool,
        )
        from TrackVertexAssociationTool.getTTVAToolForReco import getTTVAToolForReco

        trkopt = "ftf"
        # Alg generator for RecoFragmentsPool
        # Need to unpack dict as not hashable
        # We can only use ** once, so manually
        # extract the inputs
        def getCVFAlg(_, inputClusters, inputTracks, inputVertices, **recoDict):
            return HLT__MET__CVFAlg(
                f"{recoDict['calib']}{trkopt}ClusterCVFAlg",
                InputClusterKey=inputClusters,
                InputTrackKey=inputTracks,
                InputVertexKey=inputVertices,
                OutputCVFKey="CVF",
                TrackSelectionTool=InDet__InDetTrackSelectionTool(
                    CutLevel="TightPrimary"
                ),
                # Note: Currently (March 2021), this is configured to not use the TTVA decorations
                # provided by tracking CP. This will work with the current configured WP.
                #
                # If you need the decorations, you need to make sure to pass
                # this method the correct alg sequence to add to, since it needs
                # to schedule an algorithm to provide the information.
                TVATool=getTTVAToolForReco(
                    WorkingPoint="Custom",
                    d0_cut=2.0,
                    dzSinTheta_cut=2.0,
                    addDecoAlg=False,
                    TrackContName=inputTracks,
                    VertexContName=inputVertices,
                ),
                ExtensionTool=ApproximateTrackToLayerTool(),
            )

        cvfAlg = RecoFragmentsPool.retrieve(
            getCVFAlg,
            None,
            inputClusters=inputs["Clusters"],
            inputTracks=inputs["Tracks"],
            inputVertices=inputs["Vertices"],
            **recoDict,
        )

        def getCVFPrepAlg(_, inputClusters, inputCVFKey, **recoDict):
            return HLT__MET__CVFPrepAlg(
                f"{recoDict['calib']}{trkopt}ClusterCVFPrepAlg",
                InputClusterKey=inputClusters,
                InputCVFKey=inputCVFKey,
                OutputCategoryKey="PUClassification",
            )

        prepAlg = RecoFragmentsPool.retrieve(
            getCVFPrepAlg,
            None,
            inputClusters=inputs["Clusters"],
            inputCVFKey=cvfAlg.OutputCVFKey,
            **recoDict,
        )
        return (
            [cvfAlg, prepAlg],
            {"CVF": cvfAlg.OutputCVFKey, "CVFPUCategory": prepAlg.OutputCategoryKey},
        )


default_inputs.add_input(CVFClusterInputConfig())


class JetInputConfig(AlgInputConfig):
    """Helper input config for jets

    Note that if the jets require JVT but are topo jets there is nothing in the
    recoDict (unless the calibration is gsc) to force tracking to run. Therefore
    for this config, you can add an extra key 'forceTracks' to the recoDict
    """

    def __init__(self):
        super().__init__(produces=["Jets", "JetDef"])

    def _input_clusters(self, recoDict):
        # Only take the config that forces 'EMClusters' if the calibration
        # isn't set to 'em'. If it's set to em then we want to make sure that
        # we only create those sequences once
        # NB: for the jets here this is hopefully temporary - we're only forcing
        # the clusters to be EM as we haven't added the lcw jets to the EDM
        return "Clusters" if recoDict["calib"] == "em" else "EMClusters"

    def _use_tracks(self, recoDict):
        """ Whether or not this reco configuration requires tracks """
        if recoDict.get("forceTracks", False):
            return True
        if recoDict["constitType"] == "pf":
            return True
        elif recoDict["constitType"] == "tc":
            return "gsc" in recoDict["jetCalib"]
        else:
            raise ValueError(f"Unexpected constitType {recoDict['constitType']}")

    def dependencies(self, recoDict):
        deps = [self._input_clusters(recoDict)]
        if self._use_tracks(recoDict):
            deps += ["Tracks", "Vertices"]
        return deps

    def create_sequence(self, inputs, RoIs, recoDict):
        from ..Jet.JetRecoSequences import jetRecoSequence, getTrkColls

        trkopt = "ftf" if self._use_tracks(recoDict) else "notrk"
        recoDict = {k: v for k, v in recoDict.items() if k != "forceTracks"}
        jetRecoDict = jetRecoDictForMET(trkopt=trkopt, **recoDict)
        # hard code to em (for now) - there are no LC jets in EDM
        jetRecoDict["clusterCalib"] = "em"

        # Extract the track collections part from our input dict
        trkcolls = {} if trkopt == "notrk" else getTrkColls(inputs)

        jetSeq, jetName, jetDef = RecoFragmentsPool.retrieve(
            jetRecoSequence,
            ConfigFlags,
            clustersKey=inputs[self._input_clusters(recoDict)],
            **trkcolls,
            **jetRecoDict,
        )
        return [jetSeq], {"Jets": jetName, "JetDef": jetDef}

    def create_accumulator(self, flags, inputs, RoIs, recoDict):
        from ..Jet.JetRecoConfig import JetRecoCfg
        from ..Jet.JetRecoSequences import getTrkColls

        trkopt = "ftf" if self._use_tracks(recoDict) else "notrk"
        recoDict = {k: v for k, v in recoDict.items() if k != "forceTracks"}
        jetRecoDict = jetRecoDictForMET(trkopt=trkopt, **recoDict)
        # hard code to em (for now) - there are no LC jets in EDM
        jetRecoDict["clusterCalib"] = "em"

        trkcolls = {} if trkopt == "notrk" else getTrkColls(inputs)

        acc, jetName, jetDef = JetRecoCfg(
            flags,
            clustersKey=inputs[self._input_clusters(recoDict)],
            trkcolls=trkcolls,
            **jetRecoDict,
        )
        return acc, {"Jets": jetName, "JetDef": jetDef}


default_inputs.add_input(JetInputConfig())
