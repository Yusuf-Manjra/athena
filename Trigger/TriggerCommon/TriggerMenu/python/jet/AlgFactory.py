# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Provide a  look up tables that connects logical algortihm names to
factory functions and arguments.

The table is used to determine the function to used to return an Algorithm
proxy object.

The factory functions are static methods of the _AlgFactory class.
Static methods: methods of a class that make no use of instance variables.

"""

try:
    from AthenaCommon.SystemOfUnits import GeV
except ImportError:
    GeV = 1000.


from eta_string_conversions import eta_string_to_strings


class Alg(object):
    """Proxy class for ATLAS python configuration class (APCC). Can
    by converted to an APCC using an instantiator."""

    def __init__(self, factory, args, kargs, manual_attrs={}):
        """Hold the information needed to construct a string that can be
        passed to eval() to instantiate a python object to control an
        ATLAS Athena algorithm.
            
        This decouples assemblying the information to instantiate
        the python object from thre instantiation.
        
        This indirection allows the configuration code to be run in a mode
        where no such objects are created, which allows, e.g.,
        for development to be done when if instantiation is not possible,
        or for debug purposes
        where the creation string can be examined.

        The following information is stored:
        - the factory function (usally the class name) 
        used to obtain an instance of the Athena Alg python class

        - arguments which can be passed to the construtor as
        *list or **kwds

        - "manual_attributes " which are set post instantiation using
        the dot operator (ie a.sttr_name = attr_value). The attribute
        values themselves will be passed to eval before setting them in
        the newly instantiated object."""

        # factory function used to instantiate the APCC. Usually the class
        # name of the APCC
        self.factory = factory

        # arguments and keyword arguments for the factory function
        n_args = []
        for a in args:
            try:
                n_args.append(int(a))
            except ValueError:
                try:
                    n_args.append(float(a))
                except ValueError:
                    n_args.append(a)

        self.args = tuple(n_args)

        self.kargs = []
        for k in sorted(kargs.keys()):
            self.kargs.append('%s=%s' % (k, kargs[k]))

        self.manual_attrs = manual_attrs

    def getName(self):
        return self.factory

    def __repr__(self):
        return self.factory

    def __str__(self):
        text = [self.factory]
        text.extend(['arg: %s' % a for a in self.args])
        text.extend(['karg: %s' % k for k in self.kargs])
        text.extend(['manual attr: %s' % k for k in self.manual_attrs])
        return '\n'.join(text)

    def asString(self):
        args = list(self.args)
        args.extend(self.kargs)
        args = [str(a) for a in args]
        args = ', '.join(args)
        s = '%s(%s)' % (self.factory, args)
        return s


def _get_energy_density_radius():
    """Provide a common source for the energy density akt radius"""
    return 0.4


class AlgFactory(object):
    def __init__(self, chain_config):
        
        #chain_config contains "Algorithm-ready" menu data
        self.chain_config = chain_config
        self.menu_data = chain_config.menu_data
        self.cluster_params = self.menu_data.cluster_params
        
        # cluster_calib is a string that will be used to label
        # the PseudoJetGetter which is used by the offline software.
        # The name of the PSG will be '%sTopo' % cluster_calib.
        # The possible names are in JetContainerInfo.h (maybe in the JetOnput
        # enum?). And thus limited to a finite set of values. "LC" and "EM"
        # are strings in this set.

        self.fex_params = self.menu_data.fex_params
        self.last_fex_params = self.menu_data.last_fex_params
        self.recluster_params = self.menu_data.recluster_params
        self.hypo_params = self.menu_data.hypo_params

        # remove '.' characters from strings used to be Algorithm names
        # as names containing '.' are not accepted by Gaudi
        self.chain_name_esc = self.chain_config.chain_name.replace('.', '_')
        self.seed = self.chain_config.seed.replace('.', '_')
 
    def tt_maker(self):
        name = "'TrigL1BSTowerMakerForJets'"
        return [Alg('TrigL1BSTowerMaker', (), {'name': name})]

    def fullscan_roi(self):
        return [Alg('DummyAlgo', ('"RoiCreator"',), {})]

    def jetrec_triggertowers(self):
        """Instantiate a python object for TrigHLTJetRec that will
        use TriggerTower objcts as as input."""

        merge_param_str = str(self.fex_params.merge_param).zfill(2)
    
        factory = 'TrigHLTJetRecFromTriggerTower'
        # add factory to instance label to facliltate log file searches
        # name = '"%s_%s"' %(factory, self.fex_params.fex_label)

        # kwds = {
        #    'name': name,  # instance label
        #    'merge_param': "'%s'" % merge_param_str,
        #    'jet_calib': "'%s'" % self.fex_params.jet_calib,
        #    'cluster_calib': self.cluster_calib,
        #    'output_collection_label': "'%s'" % (
        #    self.fex_params.fex_label)
        #}

        
        name = '"%s"' % factory

        kwds = {
            'name': name,  # instance label
            'merge_param': "'04'",
            'jet_calib': "'nojcalib'",
            'cluster_calib': "'EM'",
            'output_collection_label': "'tcollection'",
            'ptMinCut': 20.0  # Note: 20 MeV
        }

        return [Alg(factory, (), kwds)]

    def jetrec_clusters(self):
        """Instantiate a python object for TrigHLTJetRec that will
        use xAOD::CaloCluster s as input."""

        merge_param_str = str(self.fex_params.merge_param).zfill(2)
    
        factory = 'TrigHLTJetRecFromCluster'
        # add factory to instance label to facliltate log file searches
        name = '"%s_%s"' %(factory, self.fex_params.fex_label)

        kwds = {
            'name': name,  # instance label
            'merge_param': "'%s'" % merge_param_str,
            'jet_calib': "'%s'" % self.fex_params.jet_calib,
            'cluster_calib': '"%s"' % self.fex_params.cluster_calib_fex,
            'output_collection_label': "'%s'" % (
            self.fex_params.fex_label)
        }
        return [Alg(factory, (), kwds)]


    def jetrec_recluster(self):
        """Instantiate a python object for TrigHLTJetRec that will
        use xAOD::Jet s as input."""

        merge_param_str = str(self.recluster_params.merge_param).zfill(2)
    
        factory = 'TrigHLTJetRecFromJet'
        # add factory to instance label to facliltate log file searches
        name = '"%s_%s"' %(factory, self.recluster_params.fex_label)

        kwds = {
            'name': name,  # instance label
            'merge_param': "'%s'" % merge_param_str,
            'jet_calib': "'%s'" % self.recluster_params.jet_calib,
            'cluster_calib': "'%s'" % self.fex_params.cluster_calib_fex,
            'output_collection_label': "'%s'" % (
                self.recluster_params.fex_label),
            # ptMinCut and etaMaxCut are cuts applied to the
            # input jets before the reclustering jet finding is done.
            'ptMinCut': self.recluster_params.ptMinCut * GeV,
            'etaMaxCut': self.recluster_params.etaMaxCut,
        }

        return [Alg(factory, (), kwds)]
    
    #HI
    def hijetrec_hic(self):
        """Instantiate a python object for TrigHLTHIJetRec that will
       	use xAOD::CaloCluster s as input."""

        merge_param_str = str(self.fex_params.merge_param).zfill(2)

        factory = 'TrigHLTHIJetRecFromHICluster'
        # add factory to instance label to facliltate log file searches
        name = '"%s_%s"' %(factory, self.fex_params.fex_label)

        kwds = {
            'name': name,  # instance label
            'merge_param': "'%s'" % merge_param_str,
            'jet_calib': "'%s'" % self.fex_params.jet_calib,
            #'cluster_calib': self.fex_params.cluster_calib_fex,
            'output_collection_label': "'%s'" % (
            self.fex_params.fex_label)
        }

        return [Alg(factory, (), kwds)]        

    # def jr_hypo_single(self):
    #    """
    #    Skype discussion RC/P Sherwood
    #    [21/01/15, 18:44:50] Ricardo Goncalo:
    #    hypo parameters: ET, eta min, eta max

    #     recoAlg:   'a4', 'a10', 'a10r'
    #    dataType:  'TT', 'tc', 'cc'
    #    calib:     'had', 'lcw', 'em'
    #    jetCalob:  'jes', 'sub', 'subjes', 'nocalib'
    #    scan:      'FS','PS'

    #    j65_btight_split_3j65_L13J25.0ETA22 ->
    #                    EFJetHypo_65000_0eta320_a4_tc_em_subjes_FS
    #    """
        
    #    assert len(self.hypo_params.jet_attributes) == 1
    #    ja = self.hypo_params.jet_attributes[0]

    #    # the hypo instance name is constructed from the
    #    # last jet fex run.
    #    name_extension = '_'.join([str(e) for  e in
    #                               (int(ja.threshold * GeV),
    #                                ja.eta_range,
    #                                self.last_fex_params.fex_alg_name,
    #                                self.last_fex_params.data_type,
    #                                self.cluster_params.cluster_calib,
    #                                self.last_fex_params.jet_calib,
    #                                self.menu_data.scan_type)])
        
    #    name = '"EFJetHypo_j%s"' % name_extension

    #    etaMin = str(ja.eta_min)
    #    etaMax = str(ja.eta_max)
    #    kargs = {}
    
    #    args = [name, str(GeV * ja.threshold), etaMin, etaMax]
    
    #    return [Alg('EFJetHypo', args, kargs)]

    def jr_hypo(self):
        """
        Skype discussion R Goncalo/P Sherwood
        [21/01/15, 18:44:50] Ricardo Goncalo:
        hypo parameters: ET, eta min, eta max

        recoAlg:   'a4', 'a10', 'a10r'
        dataType:  'TT', 'tc', 'cc', 'ion'
        calib:     'had', 'lcw', 'em'
        jetCalob:  'jes', 'sub', 'subjes', 'nocalib'
        scan:      'FS','PS'

        j65_btight_split_3j65_L13J25.0ETA22 ->
                        EFJetHypo_65000_0eta320_a4_tc_em_subjes_FS
        """

        
        # assert len(self.hypo_params.jet_attributes) > 1
        
        ja = self.hypo_params.jet_attributes_tostring()

        # the hypo instance name is constructed from the
        # last jet fex run.
        name_extension = '_'.join([str(e) for  e in
                                   (ja,
                                    self.last_fex_params.fex_alg_name,
                                    self.last_fex_params.data_type,
                                    self.fex_params.cluster_calib,
                                    self.last_fex_params.jet_calib,
                                    self.menu_data.scan_type)])
        
        name = '"EFCentFullScanMultiJetHypo_%s"' % name_extension
        hypo = self.menu_data.hypo_params

        etaMin = hypo.jet_attributes[0].eta_min
        etaMax = hypo.jet_attributes[0].eta_max
    
        kargs = {'multiplicity': len(hypo.jet_attributes),
                 'ef_thrs': [j.threshold * GeV for j in hypo.jet_attributes],
                 'etaMin': etaMin,
                 'etaMax': etaMax,
             }

        # the class name EFCentFullScanMultiJetHypo is a misnomer:
        # it sets up a C++ algorithm that is neither necessarily
        # central, not multi-jet.
        # It will now be used for single and multjets, and where
        # any eta range can be set.
        return [Alg(
            'EFCentFullScanMultiJetHypo',
            (name,),
            kargs)]


    def hlt_hypo(self):
        """ method to replace jr_hypo

        Skype discussion R Goncalo/P Sherwood
        [21/01/15, 18:44:50] Ricardo Goncalo:
        hypo parameters: ET, eta min, eta max

        recoAlg:   'a4', 'a10', 'a10r'
        dataType:  'TT', 'tc', 'cc'
        calib:     'had', 'lcw', 'em'
        jetCalob:  'jes', 'sub', 'subjes', 'nocalib'
        scan:      'FS','PS'

        j65_btight_split_3j65_L13J25.0ETA22 ->
                        EFJetHypo_65000_0eta320_a4_tc_em_subjes_FS
        """

        
        # assert len(self.hypo_params.jet_attributes) > 1
        
        ja = self.hypo_params.jet_attributes_tostring()

        cleaningAlg = self.hypo_params.cleaner
        matchingAlg = self.hypo_params.matcher

        # the hypo instance name is constructed from the
        # last jet fex run.

        name_extension = '_'.join([str(e) for  e in
                                   (ja,
                                    self.last_fex_params.fex_alg_name,
                                    self.last_fex_params.data_type,
                                    self.fex_params.cluster_calib,
                                    self.last_fex_params.jet_calib,
                                    self.menu_data.scan_type,
                                    cleaningAlg,
                                    matchingAlg
                                )])
        
        name = '"TrigHLTJetHypo_%s"' % name_extension
        hypo = self.menu_data.hypo_params

        eta_mins = [ja.eta_min for ja in hypo.jet_attributes]
        eta_maxs = [ja.eta_max for ja in hypo.jet_attributes]
        EtThresholds = [ja.threshold * GeV for ja in hypo.jet_attributes]
        
        kargs = {
            'name': name,
            'eta_mins': eta_mins,
            'eta_maxs': eta_maxs,
            'EtThresholds': EtThresholds,
            'cleaningAlg': '"%s"' % cleaningAlg, 
            'matchingAlg': '"%s"' % matchingAlg,
        }

        return [Alg('TrigHLTJetHypo',(), kargs)]

    def hlt_hypo_tla(self):

        name = '"TrigHLTJetHypo_%s"' % self.hypo_params.tla_string
        hypo = self.menu_data.hypo_params

        eta_min = -2.8
        eta_max = 2.8
        eta_mins = [eta_min]
        eta_maxs = [eta_max]
        ystar_mins = [0.]
        ystar_maxs = [min(0.5*abs(eta_max-eta_min), 1.2)]
        mass_mins = [float(hypo.mass_min) * GeV]
        mass_maxs = [float(hypo.mass_max) * GeV]
        indices = [int(hypo.indexlo), int(hypo.indexhi)]

        kargs = {
            'name': name,
            'eta_mins': eta_mins,
            'eta_maxs': eta_maxs,
            'ystar_mins': ystar_mins,
            'ystar_maxs': ystar_maxs,
            'mass_mins': mass_mins,
            'mass_maxs': mass_maxs,
            'cleaningAlg': '"noCleaning"', 
            'matchingAlg': '"selectedJets"',
            'jetvec_indices': indices,
        }

        return [Alg('TrigHLTJetHypo',(), kargs)]


    def hlt_hypo_test1(self):
        return self._hlt_hypo(cleaningAlg='noCleaning',
                              matchingAlg='orderedCollections')


    def hlt_hypo_test2(self):
        return self._hlt_hypo(cleaningAlg='noCleaning',
                              matchingAlg='maximumBipartite')


    def hlt_hypo_test3(self):
        return self._hlt_hypo(cleaningAlg='basicCleaning',
                              matchingAlg='orderedCollections')


    def ht_hypo(self):
        """set up a HT hypo"""
    
        eta_range = self.hypo_params.eta_range
        name_extension = '_'.join([str(e) for  e in (
            self.hypo_params.attributes_to_string(),
            self.last_fex_params.fex_alg_name,
            self.last_fex_params.data_type,
            self.fex_params.cluster_calib_fex,
            self.last_fex_params.jet_calib,
            self.menu_data.scan_type)])

        name = '"EFHTHypo_%s"' % name_extension

        eta_min, eta_max = eta_string_to_strings(eta_range)

        args = ()
    
        kargs = {'name': name,
                 'eta_min': eta_min,
                 'eta_max': eta_max,
                 'HT_cut': self.hypo_params.ht_threshold * GeV,
                 'Et_cut': self.hypo_params.jet_et_threshold * GeV
                 }
                 
        return [Alg('EFHT', args, kargs)]


    def superRoIMaker(self):
        factory = 'SeededAlgo'

        params = {'UseRoiSizes':False,
                  'EtaHalfWidth':0.5,
                  'PhiHalfWidth':0.5}

        name = '"SeededAlgo_%s"' % self.seed

        options = {'name': name,
                   'UseRoiSizes': False,
                   'EtaHalfWidth': 0.5,
                   'PhiHalfWidth': 0.5}

        return [Alg(factory, (), options)]

    def cellMaker_superPS_topo(self):
        """assign a name which identifies the fex sequence and
        the python class to be instantiated. The cell maker
        python class used here differs from the full scan case due
        to the absence of a read-out optimization present in the
        full scan case."""
        
        class_name = 'TrigCaloCellMaker_jet_super'
        instance_name = '"TriggerCaloCellMaker_PS"'

        return [Alg(class_name,
                   (instance_name,
                    'doNoise=0',
                    'AbsE=True'),
                   {})]

    def cellMaker_fullcalo_topo(self):
        # assign a name which identifies the fex sequence and
        # the python class to be instantiated.

        class_name = 'TrigCaloCellMaker_jet_fullcalo'
        instance_name = '"TriggerCaloCellMaker_FS"'

        return [Alg(class_name,
                   (instance_name,
                    'doNoise=0',
                    'AbsE=True',
                    'doPers=True'),
                   {})]

    def topoClusterMaker(self):
        
        # assign a name which identifies the sequence the Algorithm
        # will be placed in and the python class to be instantiated.

        class_name = 'TrigCaloClusterMaker_topo'
        instance_name = '"TrigCaloClusterMaker_topo"'

        return [Alg(class_name,
                   (instance_name,),
                   {'doMoments': True,
                    'doLC': True})]
    #HI
    def hiEventShapeMaker(self):

        class_name = 'TrigHIEventShapeMaker_hijet'
        instance_name = '"%s"' % class_name
        return [Alg(class_name,
                   (instance_name,),
                   {})]


    #HI
    def hiCombinedTowerMaker(self):

        class_name = 'TrigCaloTowerMaker_hijet'
        instance_name = '"%s_%s"' % (class_name,
                                     self.cluster_params.cluster_label)
        return [Alg(class_name,
                   (instance_name,),{})]


    #HI
    def hiClusterMaker(self):

        class_name = 'TrigHIClusterMaker_hijet'
        instance_name = '"%s_%s"' % (class_name,
                                     self.cluster_params.cluster_label)
        return [Alg(class_name,
                   (instance_name,),{})]                
        
    def roiDiagnostics(self):
        factory = 'TrigHLTRoIDiagnostics'

        instance_name = "'TrigHLTRoIDiagnostics_%s'" % self.chain_name_esc
        return [Alg(factory, (), {'name': instance_name})]

    def jetRecDiagnostics(self):
        factory = 'TrigHLTJetDiagnostics_named'
        # the instance name will be completed by with the name of rthe
        # sequence being monitored at the time the sequences are built
        # see SequenceTree.py. The same applies to 'chain_name',
        # which is a misnomer - it is really a histogram file label.

        instance_name = "'TrigHLTJetDiagnostics_%s'" % self.chain_name_esc

        kwds = {'name': instance_name,
                'chain_name': "'%s'" % self.chain_name_esc}
        return [Alg(factory, (), kwds)]

    def jetHypoDiagnostics(self):
        # label the output histograms with the hain name for hypo diagnostics
        
        factory = 'TrigHLTHypoDiagnostics_named'

        kwds = {'name': '"TrigHLTHypoDiagnostics_%s"' % self.chain_name_esc,
                'chain_name': '"%s"' % self.chain_name_esc}
        # kwds = {
        #    'name': '"TrigHLTHypoDiagnostics_%s"' % self.hypo_sequence_alias,
        #    'chain_name': '"%s"' % self.hypo_sequence_alias}

        return [Alg(factory, (), kwds)]


    def clusterDiagnostics(self):
        # the instance name will be completed by with the name of rthe
        # sequence being monitored at the time the sequences are built
        # see SequenceTree.py. The same applies to 'chain_name',
        # which is a misnomer - it is really a histogram file label.
        factory = 'TrigHLTClusterDiagnostics_named'


        kwds = {'name': '"TrigHLTClusterDiagnostics_%s"' % self.chain_name_esc,
                'chain_name': '"%s"' % self.chain_name_esc}
        return [Alg(factory, (), kwds)]


    def jetRecDebug(self):
        factory = 'TrigHLTJetDebug'
        name = '"TrigHLTJetRecDebug_%s"' % (self.fex_params.fex_label)
        ed_merge_param = _get_energy_density_radius()

        kwds = {'name': name,
                'chain_name': '"%s"' % self.chain_name_esc,
                'cluster_calib': self.fex_params.cluster_calib_fex,
                'ed_merge_param': ed_merge_param,
            }
        return [Alg(factory, (), kwds)]


    def cellDiagnostics(self):
        # the instance name will be completed by with the name of rthe
        # sequence being monitored at the time the sequences are built
        # see SequenceTree.py. The same applies to 'chain_name',
        # which is a misnomer - it is really a histogram file label.

        factory = 'TrigHLTCellDiagnostics_named'

        kwds = {'name': '"TrigHLTCellDiagnostics_%s"' % self.chain_name_esc,
                'chain_name': '"%s"' % self.chain_name_esc}

        return [Alg(factory, (), kwds)]


    def energyDensityAlg(self):

        factory = 'TrigHLTEnergyDensity'

        # assign a name which identifies the fex sequence and
        # the python class to be instantiated.
        ed_merge_param = _get_energy_density_radius()


        name = '"%s_%s%s"' % (
            factory, 
            str(int(10 * ed_merge_param)).zfill(2),
            self.fex_params.cluster_calib,
        )

        # we do not carry the energy density calculation merge_param
        # so hard wire it here (to be fixed). This is not the fex
        # merge param!
        kwds = {'name':  name,
                'cluster_calib': '"%s"' % self.fex_params.cluster_calib_fex,
                'ed_merge_param': ed_merge_param
            }
    
        return [Alg(factory, (), kwds)]


    def dataScoutingAlg1(self, manual_attrs):
        factory = 'ScoutingStreamWriter'
        manual_attrs = manual_attrs
          
        return [Alg(factory, ("'JetDataScouting'",), {}, manual_attrs)]
          
          
    def dataScoutingAlg2(self, manual_attrs):
        factory = 'ScoutingStreamWriter'
        manual_attrs = manual_attrs

        return [Alg(factory, ("'JetDataScouting'",), {}, manual_attrs)]
              

    def dataScouting_TrigHLTJetTLASelector(self,
                                           object_name,
                                           jetCollectionName,
                                           jetPtThreshold,
                                           maxNJets):
        factory = 'TrigHLTJetDSSelector'
        
        kwds = {'name': object_name,
                'jetCollectionName': jetCollectionName,
                'jetPtThreshold': jetPtThreshold,
                'maxNJets': maxNJets}
        
        return [Alg(factory, (), kwds)]


    def getDataScoutingAlgs(self):
        """Provide the arguments to instantiate data scouting Alg
        object. These arguments arr not provided by the ChainConfig
        object, from which other Alg objects get their parmeters"""

        jetPtThreshold = 20.* GeV
        maxNJets = -1
        name_frag = ""
        if (maxNJets<0) : 
           name_frag = str(int(jetPtThreshold))+"_AllJets" 
        else :
           name_frag = '%d_%dJets' % (int(jetPtThreshold), maxNJets)
        object_name = '"TrigHLTJetDSSelector_%s"' % name_frag
        jetCollectionName = '"TrigHLTJetDSSelectorCollection"'

        selector_alg =  self.dataScouting_TrigHLTJetTLASelector(
            object_name,
            jetCollectionName,
            jetPtThreshold,
            maxNJets)
        
        manual_attrs ={
            'CollectionTypeName':
            str(['xAOD::JetContainer#TrigHLTJetDSSelectorCollection',
                 'xAOD::JetTrigAuxContainer#TrigHLTJetDSSelectorCollectionAux'])}
        
        writer_alg = self.dataScoutingAlg1(manual_attrs)

        algs = []
        algs.extend(selector_alg)
        algs.extend(writer_alg)
        return algs


    def getDataScoutingAlgs1(self):
        return self.getDataScoutingAlgs()


    def getDataScoutingAlgs2(self):
        return self.getDataScoutingAlgs()


# old code....
# NEEDS UPDATING FOR ETA RANGE HANDLING
# def jr_hypo_testCleaning(params):
# 
#     assert len(params.hypo_data) == 1
#     hd = params.hypo_data[0]
# 
#     if hd.eta_region == 'j':
#         factory = 'EFJetHypo_doBasicCleaning'
#         arg0 = '"EFJetHypo_doBasicCleaning_%s"' % hd.sig
#     elif hd.eta_region == 'fj':
#         factory = 'EFFwdJetHypo_doBasicCleaning'
#         arg0 = '"EFFwdJetHypo_doBasicCleaning_%s"' % hd.sig
#     else:
#         assert False
# 
#     return Alg(factory, (arg0, str(GeV * hd.threshold)), {})
# 
#
