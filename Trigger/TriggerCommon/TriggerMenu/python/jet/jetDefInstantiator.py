# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Module of factory functions to control the instantiation of Algorithms.

In some cases instantiation is done by a remote module.
In thsi case the factory function imports that module and retrieves the
instance."""

from exc2string import exc2string2

from TriggerJobOpts.TriggerFlags import TriggerFlags

from TrigGenericAlgs.TrigGenericAlgsConf import \
    PESA__DummyUnseededAllTEAlgo as DummyAlgo

from TrigGenericAlgs.TrigGenericAlgsConf import \
    PESA__SeededSuperRoiAllTEAlgo as SeededAlgo

from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX

from TrigCaloRec.TrigCaloRecConfig import (TrigCaloCellMaker_jet_fullcalo,
                                           TrigCaloCellMaker_jet_super,
                                           TrigCaloTowerMaker_jet,
                                           TrigCaloTopoTowerMaker_jet,
                                           TrigCaloClusterMaker_topo,
                                           TrigLArNoisyROAlgConfig)

from TrigHLTJetRec.TrigHLTJetRecConf import TrigHLTRoIDiagnostics

from TrigHLTJetRec.TrigHLTJetRecConfig import (TrigHLTJetDiagnostics_named,
                                               TrigHLTJetRecFromCluster,
                                               TrigHLTJetRecFromJet,
                                               TrigHLTClusterDiagnostics_named,
                                               TrigHLTCellDiagnostics_named,
                                               TrigHLTHypoDiagnostics_named,
                                               TrigHLTJetRec_param,
                                               # TrigHLTJetDebug,
                                               TrigHLTEnergyDensity,
                                               TrigHLTJetDSSelector,)

from TrigJetHypo.TrigJetHypoConfig import (EFJetHypo,
                                           # EFCentJetHypo,
                                           EFCentFullScanMultiJetHypo,
                                           # EFFwdJetHypo,
                                           EFJetHypoNoiseConfig,
                                           EFFwdJetHypo_doBasicCleaning,
                                           EFJetHypoNoiseConfig,
                                           EFJetHypo_doBasicCleaning)

# from TrigT2CaloJet.TrigT2CaloJetConfig import T2L1Unpacking_TT

from TrigDetCalib.TrigDetCalibConf import ScoutingStreamWriter

abomination_to_keep_config_weakvalue_dict_intact = []


class Instantiator(object):

    def __init__(self):
        self.cache = {}
        self.err_hdr = '%s()' % self.__class__.__name__

    def __call__(self, a):
        """__call__ takes the string returned by alg.asString for
        an object alg, and evals it to return an Athena Algorithm instance"""

        try:
            s = a.asString()  # convert alg to a string to be eval'd
        except Exception, e:
            m = '%s() call to asString failed for object %s\n%s ' % (err_hdr,
                                                                     str(s),
                                                                     str(e))
            raise RuntimeError(m)


        alg = self.cache.get(s)
        if alg:
            return alg

        try:
            alg = eval(s)
        except Exception, e:
            tb = exc2string2()
            m = '%s() Error instantiating  Algorithm: eval(%s) '\
                '%s\nTraceback: \n%s'
            m = m % (self.__class__.__name__, s, str(e), tb)

            raise RuntimeError(m)

        def manual_attr_add(k, v):

            try:
                val = eval(v)
            except Exception, e:
                m = '%s() Error running  eval: '\
                    'name %s value: eval(%s) \n%s' % (err_hdr, k, v, str(e))
                raise RuntimeError(m)

            try:
                alg.__setattr__(k, val)
            except Exception, e:
                m = '%s() Error inserting a new Algorithm attribute: '\
                    'name %s value: eval(%s) \n%s' % (err_hdr, k, v, str(e))
                raise RuntimeError(m)

        err_hdr = self.err_hdr
        [manual_attr_add(k, v) for k, v in a.manual_attrs.items()]

        self.cache[s] = alg
        # in 2012 it was required to maintain a reference to the
        # configuration objects due to the use of a weak referece
        # dictionary in a module far far away. Who knows what 2015 will bring..
        abomination_to_keep_config_weakvalue_dict_intact.append(alg)

        return alg
