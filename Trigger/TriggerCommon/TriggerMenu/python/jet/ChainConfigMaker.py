# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Reformat input dictionary. Dictionary structure at
https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TriggerCommon/ \
TriggerMenu/trunk/python/menu/SignatureDicts.py#L144
"""

import re
from fexparams_factory import fexparams_factory
from hypo_factory import hypo_factory
from clusterparams_factory import clusterparams_factory
from MenuData import MenuData
from ChainConfig import ChainConfig
from eta_string_conversions import eta_string_to_floats

class JetAttributes(object):
    """Per jet attributes. Used by  hypo algorithms."""

    def __init__(self, threshold, eta_range):
        self.threshold = threshold
        self.eta_range = eta_range  # string like '0eta320'
        # eta_min, eta_max are floats
        self.eta_min, self.eta_max = eta_string_to_floats(eta_range)

    def __str__(self):
        return 'thresh: %s eta_min: %s eta_max: %s' % (str(self.threshold),
                                                       str(self.eta_min),
                                                       str(self.eta_max))


class ChainConfigMaker(object):
    """Process input dictionary to create a ChainConfig object.
    A Chainfig obeject carries "algorithm-ready"" arguments (derived
    from the incoming dictionary) arranged by algorithm type.

    
    ChainConfigMaker.__init__ method acquires the incoming dictionary from
    the menu code, performs consistancy checks (eg the chainParts of the input
    dictionary are highly redundant. By affirmation of the menu code author
    many entries in the chain part dictionary will have the same values.
    If this is not the case, there is an error, or the chainParts has acquired
    new, currently unknown, semantics), and transforms its values into values
    directly usable by the Algoritm classes. These variables are attributes
    of this class.

    When the  function method (__call__) is invoked, these atrributes
    are transferred to a lighter weight ChainConfig object."""

    reco_alg_re = re.compile(r'^a\d+$')
    recluster_alg_re = re.compile(r'^a\d+r$')

    def __init__(self, d):
            
        self.err_hdr = '%s.process_part: ' % self.__class__.__name__
        self.n_parts = 0

        self.data_type = ''
        self.scan_type = ''
        self.multi_eta = len(set([cp['etaRange'] for cp in d['chainParts']])) > 1
        self.jet_attributes = []
        self.run_rtt_diags = d['run_rtt_diags']
        self.chain_name = d['chainName']
        self.seed = d['L1item']
        [self.process_part(p) for p in d['chainParts']]

    def process_part(self, part):
        """Process chain parts. If there is more than one chain part,
        the fex data must always be the same: multiple chain parts
        convey information about different thresholds in for the
        jet hypo"""

        self._check_part(part)

        # ----- test chain parameter - if present, sill be  be in the 'extra' field
        extra = part['extra']
        if extra.startswith('test'):
            self.check_and_set('test_flag', extra)
        else:
            self.check_and_set('test_flag', '')

        # -------------- cluster parameters -------------

        # the following dictionary translates the incoming value of
        # the calibration string to arguments relevant for
        # python class instantiation. The first is used by TrigCaloClusterMaker
        # while the second affects how JetRecTool is set up by
        # TrigHLTJetRec.

        # calib = {
        #    'em': {'cluster_dolc': False, 'do_jes': False},
        #    'lcw': {'cluster_dolc': True, 'do_jes': False},
        #    'had': {'cluster_dolc': False, 'do_jes': True}
        # }

        # calib used to (prior to 1/12/2014) refer to cluster
        # and jet calibration. Now it only determines the
        # cluster calibration.
        cluster_do_lcs = {
            'em':  False,
            'lcw': True,
            'had': False,
        }

        cluster_do_lc = cluster_do_lcs.get(part['calib'])
        
        self.check_and_set('cluster_calib', 'lcw' if
                           cluster_do_lc else 'em')

        if cluster_do_lc is None:
            msg = '%s Unknown cluster calibration %s, possible values: %s' % (
                err_hdr, p['calib'], str(cluster_do_lcs.keys())) 
            raise RuntimeError(msg)
            
        self.check_and_set('cluster_do_lc', cluster_do_lc)

        cluster_calib = 'lcw' if self.cluster_do_lc else 'em'

        # set up an identifier for the clutering algorithm
        cluster_label = reduce(lambda x, y: x + y,
                               (part['dataType'],
                                self.cluster_calib,
                                part["scan"]))
        self.check_and_set('cluster_label', cluster_label)


        # the input data changes meaning depending on the chain,
        # all in the interest of having chain names that are alledgedly
        # easy for analysts to understand...
        # 13/01/2015... jet reclustering changes the meaning ofthe
        # dictionary contents. Straighten this out here to allow
        # systematic use of the dictionary.
        
        match = self.recluster_alg_re.search(part['recoAlg'])
        if match:
            # reclustering has been requested - fix dict:
            self.check_and_set('do_recluster', True)

            # ------- recluster parameters ----------
            # - the Et cut on input jets is hardwired.
            # - the eta cut on input jets is hardwired.
            # - the algo is calculated from the input dictionary value

            self.check_and_set('recl_fex_name', 'antikt')
            self.check_and_set('recl_ptMinCut', 15.)
            self.check_and_set('recl_etaMaxCut', 2.0)
            self.check_and_set('recl_merge_param', part['recoAlg'][1:-1])
            self.check_and_set('recl_jet_calib', 'nojcalib')
            self.check_and_set('recl_fex_alg_name', part["recoAlg"])

            fex_label = reduce(lambda x, y: x + y,
                           (part["recoAlg"],
                            '_',
                            part["dataType"],
                            cluster_calib,
                            part["jetCalib"],
                            part["scan"]))

            self.check_and_set('recl_fex_label', fex_label)
            # - input jets are made from akt4
            part['recoAlg'] = 'a4'
        else:
            self.check_and_set('do_recluster', False)

        jet_calib = part['jetCalib']
        self.check_and_set('jet_calib', jet_calib)
        
        # find the type of data the clustering will act on: topoclusters
        # or trigger towers are the possibilities for now.
        data_type = part['dataType']
        self.check_and_set('data_type', data_type)

        # -------------- fex parameters ----------------

        reco_alg = part['recoAlg']
        m = self.reco_alg_re.search(reco_alg)
        if not m:
            msg = '%s unknown reco alg: %s' % (self.err_hdr, reco_alg)
            raise RuntimeError(msg)

        merge_param = int(reco_alg[1:])
        self.check_and_set('fex_name', 'antikt')
        self.check_and_set('merge_param', merge_param)
        self.check_and_set('dataType', part['dataType'])
        self.check_and_set('fex_alg_name', part["recoAlg"])
        # generate a string that will be used to label the fex
        # instance and associated entities.
        fex_label = reduce(lambda x, y: x + y,
                           (self.fex_alg_name,
                            self.data_type,
                            self.cluster_calib,
                            part["jetCalib"],
                            part["scan"]))

        self.check_and_set('fex_label', fex_label)
        


        # ------- data scouting parameters ----------
        
        ds = part.get('dataScouting', '')
        ds1 = 'ds1' if 'ds1' in ds  else ''
        ds2 = 'ds2' if 'ds2' in ds  else ''

        if ds1 and ds2:
            msg = '%s both "ds1" and "ds2" present in data scouting' \
            'string: %s' % (self.err_hdr, ds)
            raise RuntimeError(msg)

        self.check_and_set('data_scouting', ds1 + ds2)

        # --------  check scan type consistency ----------------
        scan_type = part['scan']
        scan_types = ('', 'PS', 'FS')
        if scan_type not in scan_types:
            msg = '%s unknown scanType %s allowed: %s' % (self.err_hdr,
                                                          scan_type,
                                                          str(scan_types))
            raise RuntimeError(msg)
        
        # -----------------------------------------

        if scan_type != 'FS' and self.data_type == 'TT':
            msg = '%s: scanType %s set for Trigger tower scan' % (
                self.err_hdr, scan_type)
            raise RuntimeError(msg)

        self.check_and_set('scan_type', scan_type)

        # check whether to run the hypo
        run_hypo =  'perf' not in part['addInfo'] and not self.data_scouting
        self.check_and_set('run_hypo', run_hypo)

        # --------  hypo parameters ----------------
        hypo_type = {('j', ''): 'standard',
                     ('j', 'test1'): 'single_region',
                     ('j', 'test2'): 'maximum_bipartite',
                     ('j', 'test3'): 'single_region_cleaning',
                     ('ht', ''):'ht'}.get((part['trigType'],
                                           self.test_flag), None)

        if self.multi_eta: hypo_type = 'maximum_bipartite'

        if not hypo_type:
            msg = '%s: cannot determine hypo type '\
                  'from trigger type: %s test flag: %s' % (
                self.err_hdr, part['trigType'], self.test_flag)
            raise RuntimeError(msg)

        self.check_and_set('hypo_type', hypo_type)

        hypo_setup_fn = {
            'standard': self._setup_standard_hypo,
            'single_region': self._setup_standard_hypo,
            'maximum_bipartite': self._setup_standard_hypo,
            'single_region_cleaning': self._setup_standard_cleaning_hypo,
            'ht': self._setup_ht_hypo}.get(hypo_type, None)

        if hypo_setup_fn is None:
            msg = '%s: unknown hypo type (JetDef bug) %s' % (
                self.err_hdr,
                str(self.hypo_type))
            raise RuntimeError(msg)

        hypo_setup_fn(part)

            
        self.n_parts += 1

    def check_and_set(self, attr, val):
        """Set the value of attribute attr to val if the first chain part
        is being processed. For subsequent chain parts, ensure their
        attributevalues is the same (chain parts have much redundant
        information)."""

        if self.n_parts == 0:
            setattr(self, attr, val)

        if self.n_parts:
            my_val = getattr(self, attr)
            if my_val != val:
                msg = '%s attribute clash %s %s %s' % (self.err_hdr,
                                                       attr,
                                                       my_val,
                                                       val)
                raise RuntimeError(msg)

    def __call__(self):

        # obtain objects which state which the
        # algorithm to use, and the corresponding algorithm dependent
        # parameters

        cluster_args = {'do_lc': self.cluster_do_lc,
                        'cluster_calib': self.cluster_calib,
                        'label': self.cluster_label}
        
        cluster_params = clusterparams_factory(cluster_args)

        fex_args = {'merge_param': self.merge_param,
                    'jet_calib': self.jet_calib,
                    'fex_label': self.fex_label,
                    'data_type': self.data_type,
                    'fex_alg_name': self.fex_alg_name,  # for labelling
                }

        fex_params = fexparams_factory(self.fex_name, fex_args)

        # various fexes maybe present in the same chain (a4, a10r eg)
        # the last set of fex parameters is used to name the hypo instance
        last_fex_params = fex_params  

        recluster_params = None
        if self.do_recluster:
            recl_args = dict(fex_args)
            recl_args.update({'ptMinCut': self.recl_ptMinCut,
                              'etaMaxCut': self.recl_etaMaxCut,
                              'fex_label': self.recl_fex_label,
                              'merge_param': self.recl_merge_param,
                              'jet_calib': self.recl_jet_calib,
                              'fex_alg_name': self.recl_fex_alg_name,
                          })

            recluster_params = fexparams_factory(
                'jetrec_recluster', recl_args)

            # overwrite last_fex_params
            # this is used to name the hypo instance
            last_fex_params = recluster_params  

        if self.hypo_type in ('standard', 'single_region',
                              'maximum_bipartite', 'single_region_cleaning'):
            hypo_args = {
                'chain_name': self.chain_name,
                'jet_attributes': self.jet_attributes,
                'cleaning': self.cleaning,
                'isCaloFullScan': self.scan_type == 'FS',
                'triggertower': self.data_type == 'TT',
            }

        elif self.hypo_type == 'ht':
            hypo_args = {
                'chain_name': self.chain_name,
                'eta_range': self.eta_range,
                'ht_threshold': self.ht_threshold,
                'jet_et_threshold': self.jet_et_threshold}
        else:
            msg = '%s unknown hypo_type' % (self.err_hdr, self.hypo_type)
            raise RuntimeError(msg)

        hypo_params = hypo_factory(self.hypo_type, hypo_args)

        menu_data = MenuData(self.scan_type,
                             self.data_type,
                             fex_params=fex_params,
                             hypo_params=hypo_params,
                             cluster_params=cluster_params,
                             recluster_params=recluster_params,
                             last_fex_params= last_fex_params
                         )


        return ChainConfig(chain_name=self.chain_name,
                           seed=self.seed,
                           run_hypo=self.run_hypo,
                           run_rtt_diags=self.run_rtt_diags,
                           hypo_type = self.hypo_type,
                           data_scouting=self.data_scouting,
                           menu_data=menu_data,)

    def _check_part(self, part):
        """Check chain part for errors"""

        # Attempting to do partial scan with pileup subtraction
        # produces nonsense results
        if part['scan'] == 'PS' and 'sub' in part['jetCalib']:
            msg = '%s partial scan is incompatible with pileup subtraction' %(
                self.err_hdr, )
            raise RuntimeError(msg)


            hypo_setup_fn = {'standard': self.setup_standard_hypo,
                             'ht': self.setup_ht_hypo}.get(hypo_type, None)

    def _setup_standard_hypo(self, part):
        mult = int(part['multiplicity'])
        threshold = int(part['threshold'])
        eta_range = part['etaRange']

        self.jet_attributes.extend(
            [(JetAttributes(threshold, eta_range)) for i in range(mult)])
        self.cleaning = False

    def _setup_standard_cleaning_hypo(self, part):
        self._setup_standard_hypo(part)
        self.cleaning = True

    def _setup_ht_hypo(self, part):

        eta_range = part['etaRange']
        self.check_and_set('eta_range', eta_range)
        ht_threshold = int(part['threshold'])
        self.check_and_set('ht_threshold',  ht_threshold)

        # set the default cuts on the jets contributing to the Et
        # sum to be 30 if not specified. Otherwise the expected form is
        #j\d+

        try:
            jet_et_threshold = 30. if not part['extra'] else \
                               float(part['extra'][1:])
        except:
            m = '%s unrecognized value for HT jet cut %' % (
                self.err_hdr,
                str(part['extra'][1:]))

            raise RuntimeError(m)
            
        self.check_and_set('jet_et_threshold', jet_et_threshold)
            
