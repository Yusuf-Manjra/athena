# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# this is based on MuonConfigFlags as a guide

import unittest
from AthenaConfiguration.AthConfigFlags import AthConfigFlags


def createEgammaConfigFlags():
    egcf = AthConfigFlags()

    # enable/disable the full egamma
    egcf.addFlag("Egamma.enabled", True)

    # do standard cluster-based egamma algorithm
    egcf.addFlag("Egamma.doCaloSeeded",
                 lambda prevFlags: prevFlags.Detector.EnableCalo and
                 prevFlags.Output.doESD)

    # do forward egamma
    egcf.addFlag("Egamma.doForwardSeeded",
                 lambda prevFlags: prevFlags.Detector.EnableCalo)

    # do egamma truth association when running on MC
    egcf.addFlag("Egamma.doTruthAssociation",
                 lambda prevFlags: prevFlags.Input.isMC)

    # run the GSF refitting /egamma Tracking
    egcf.addFlag("Egamma.doGSF", lambda prevFlags: prevFlags.Detector.EnableID)

    # build photon conversion vertices
    egcf.addFlag("Egamma.doConversionBuilding",
                 lambda prevFlags: prevFlags.Detector.EnableID)

    # Do e/gamma track thinning (Although we call the alg slimming...)
    egcf.addFlag("Egamma.doTrackThinning",
                 lambda prevFlags: prevFlags.Output.doWriteAOD)

    # The cluster corrections/calib
    egcf.addFlag("Egamma.Calib.ClusterCorrectionVersion",
                 'v12phiflip_noecorrnogap')
    egcf.addFlag("Egamma.Calib.SuperClusterCorrectionVersion",
                 'v12phiflip_supercluster')
    egcf.addFlag("Egamma.Calib.MVAVersion", 'egammaMVACalib/offline/v7')

    ##################################################
    # The keys: should update to use file peeking info
    # Also not sure what's the best way to do this, might want to revisit
    # one idea is to make the keys have tuples with type, name, etc
    ##################################################

    egcf.addFlag("Egamma.Keys.Input.CaloCells", 'AllCalo')
    egcf.addFlag("Egamma.Keys.Input.TopoClusters",
                 'CaloTopoClusters')  # input topoclusters
    egcf.addFlag("Egamma.Keys.Input.TruthParticles", 'TruthParticles')
    egcf.addFlag("Egamma.Keys.Input.TruthEvents", 'TruthEvents')
    egcf.addFlag("Egamma.Keys.Input.TrackParticles",
                 'InDetTrackParticles')  # input to GSF

    # the topoclusters selected for egamma from the input topoclusters
    egcf.addFlag("Egamma.Keys.Internal.EgammaTopoClusters",
                 'egammaTopoClusters')
    egcf.addFlag("Egamma.Keys.Internal.EgammaRecs", 'egammaRecCollection')
    egcf.addFlag("Egamma.Keys.Internal.PhotonSuperRecs",
                 'PhotonSuperRecCollection')
    egcf.addFlag("Egamma.Keys.Internal.ElectronSuperRecs",
                 'ElectronSuperRecCollection')

    # These are the clusters that are used to determine
    # which cells to write out to AOD
    egcf.addFlag("Egamma.Keys.Output.EgammaLargeClusters", 'egamma711Clusters')
    egcf.addFlag("Egamma.Keys.Output.EgammaLargeClustersSuppESD", '')
    # don't define SuppAOD because the whole container is suppressed

    egcf.addFlag("Egamma.Keys.Output.ConversionVertices",
                 'GSFConversionVertices')
    egcf.addFlag("Egamma.Keys.Output.ConversionVerticesSuppESD",
                 '-vxTrackAtVertex')
    egcf.addFlag("Egamma.Keys.Output.ConversionVerticesSuppAOD",
                 '-vxTrackAtVertex')

    egcf.addFlag("Egamma.Keys.Output.CaloClusters", 'egammaClusters')
    egcf.addFlag("Egamma.Keys.Output.CaloClustersSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.CaloClustersSuppAOD", '')

    egcf.addFlag("Egamma.Keys.Output.TopoSeededClusters",
                 'egammaTopoSeededClusters')
    egcf.addFlag("Egamma.Keys.Output.TopoSeededClustersSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.TopoSeededClustersSuppAOD", '-CellLink')

    egcf.addFlag("Egamma.Keys.Output.Electrons", 'Electrons')
    egcf.addFlag("Egamma.Keys.Output.ElectronsSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.ElectronsSuppAOD",
                 "-e033.-e011.-e333.-e335.-e337.-e377."
                 "-EgammaCovarianceMatrix."
                 "-isEMLHLoose.-isEMLHTight.-isEMLHMedium."
                 "-isEMLoose.-isEMMedium.-isEMTight")

    egcf.addFlag("Egamma.Keys.Input.ForwardTopoClusters",
                 'CaloCalTopoClusters')
    egcf.addFlag("Egamma.Keys.Output.ForwardElectrons", 'ForwardElectrons')
    egcf.addFlag("Egamma.Keys.Output.ForwardElectronsSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.ForwardElectronsSuppAOD",
                 '-isEMTight.-isEMMedium.-isEMLoose')

    egcf.addFlag("Egamma.Keys.Output.ForwardClusters",
                 'ForwardElectronClusters')
    egcf.addFlag("Egamma.Keys.Output.ForwardClustersSuppESD", '-SisterCluster')
    egcf.addFlag("Egamma.Keys.Output.ForwardClustersSuppAOD",
                 '-SisterCluster')

    # These are the clusters that are used to determine
    # which cells to write out to AOD
    egcf.addFlag("Egamma.Keys.Output.EgammaLargeFWDClusters",
                 'egamma66FWDClusters')
    egcf.addFlag("Egamma.Keys.Output.EgammaLargeFWDClustersSuppESD", '')
    # don't define SuppAOD because the whole container is suppressed

    egcf.addFlag("Egamma.Keys.Output.Photons", 'Photons')
    egcf.addFlag("Egamma.Keys.Output.PhotonsSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.PhotonsSuppAOD",
                 '-e033.-e011.-e333.-e335.-e337.-e377.-isEMLoose.-isEMTight')

    egcf.addFlag("Egamma.Keys.Output.GSFTrackParticles", 'GSFTrackParticles')
    egcf.addFlag("Egamma.Keys.Output.GSFTrackParticlesSuppESD",
                 "-caloExtension.-cellAssociation."
                 "-perigeeExtrapEta.-perigeeExtrapPhi")
    egcf.addFlag("Egamma.Keys.Output.GSFTrackParticlesSuppAOD",
                 '-caloExtension.-cellAssociation.'
                 '-perigeeExtrapEta.-perigeeExtrapPhi')

    # not xAOD
    egcf.addFlag("Egamma.Keys.Output.GSFTracks", 'GSFTracks')

    egcf.addFlag("Egamma.Keys.Output.TruthParticles", 'egammaTruthParticles')
    egcf.addFlag("Egamma.Keys.Output.TruthParticlesSuppESD", '-caloExtension')
    egcf.addFlag("Egamma.Keys.Output.TruthParticlesSuppAOD", '-caloExtension')

    return egcf


# self test


class TestEgammaConfigFlags(unittest.TestCase):

    def runTest(self):
        flags = createEgammaConfigFlags()
        self.assertEqual(flags.Egamma.Keys.Output.Photons, "Photons")
        self.assertEqual(flags._get("Egamma.Keys.Output.Photons"), "Photons")


if __name__ == "__main__":

    unittest.main()
