# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnalysisAlgorithmsConfig.ConfigBlock import ConfigBlock


class PileupReweightingBlock (ConfigBlock):
    """the ConfigBlock for pileup reweighting"""

    def __init__ (self) :
        super (PileupReweightingBlock, self).__init__ ('Event')
        self.addOption ('campaign', None, type=None)
        self.addOption ('files', None, type=None)
        self.addOption ('useDefaultConfig', False, type=bool)
        self.addOption ('userLumicalcFiles', None, type=None)
        self.addOption ('userPileupConfigs', None, type=None)


    def makeAlgs (self, config) :

        from Campaigns.Utils import Campaign

        try:
            from AthenaCommon.Logging import logging
        except ImportError:
            import logging
        log = logging.getLogger('makePileupAnalysisSequence')

        # TODO: support per-campaign config

        toolConfigFiles = []
        toolLumicalcFiles = []
        campaign = self.campaign
        if self.files is not None and (campaign is None or campaign is Campaign.Unknown or self.userPileupConfigs is None):
            if campaign is None or campaign is Campaign.Unknown:
                from Campaigns.Utils import getMCCampaign
                campaign = getMCCampaign(self.files)
                if campaign:
                    log.info(f'Autoconfiguring PRW with campaign: {campaign}')
                else:
                    log.info('Campaign could not be determined.')

        if campaign:
            if self.userPileupConfigs is None:
                if config.dataType() == 'data':
                    log.info('Data needs no configuration files')
                else:
                    from PileupReweighting.AutoconfigurePRW import getConfigurationFiles
                    toolConfigFiles = getConfigurationFiles(campaign=campaign, files=self.files, useDefaultConfig=self.useDefaultConfig,
                                                            data_type=config.dataType())
                    log.info('Setting PRW configuration based on input files')

                    if toolConfigFiles:
                        log.info(f'Using PRW configuration: {", ".join(toolConfigFiles)}')
            else:
                log.info('Using user provided PRW configuration')

        if self.userPileupConfigs is not None:
            toolConfigFiles = self.userPileupConfigs[:]

        if self.userLumicalcFiles is not None:
            log.info('Using user-provided lumicalc files')
            toolLumicalcFiles = self.userLumicalcFiles[:]
        else:
            from PileupReweighting.AutoconfigurePRW import getLumicalcFiles
            toolLumicalcFiles = getLumicalcFiles(campaign)

        # Set up the only algorithm of the sequence:
        alg = config.createAlgorithm( 'CP::PileupReweightingAlg', 'PileupReweightingAlg' )
        config.addPrivateTool( 'pileupReweightingTool', 'CP::PileupReweightingTool' )
        alg.pileupReweightingTool.ConfigFiles = toolConfigFiles
        if not toolConfigFiles and config.dataType() != "data":
            log.info("No PRW config files provided. Disabling reweighting")
            # Setting the weight decoration to the empty string disables the reweighting
            alg.pileupWeightDecoration = ""
        alg.pileupReweightingTool.LumiCalcFiles = toolLumicalcFiles



class GeneratorAnalysisBlock (ConfigBlock):
    """the ConfigBlock for generator algorithms"""

    def __init__ (self) :
        super (GeneratorAnalysisBlock, self).__init__ ('Generator')
        self.addOption ('saveCutBookkeepers', False, type=bool)
        self.addOption ('runNumber', 0, type=int)
        self.addOption ('cutBookkeepersSystematics', False, type=bool)

    def makeAlgs (self, config) :

        if config.dataType() not in ["mc", "afii"] :
            raise ValueError ("invalid data type: " + config.dataType())

        if self.saveCutBookkeepers and not self.runNumber:
            raise ValueError ("invalid run number: " + 0)

        # Set up the CutBookkeepers algorithm:
        if self.saveCutBookkeepers:
          alg = config.createAlgorithm('CP::AsgCutBookkeeperAlg', 'CutBookkeeperAlg')
          alg.runNumber = self.runNumber
          alg.enableSystematics = self.cutBookkeepersSystematics
          config.addPrivateTool( 'truthWeightTool', 'PMGTools::PMGTruthWeightTool' )

        # Set up the weights algorithm:
        alg = config.createAlgorithm( 'CP::PMGTruthWeightAlg', 'PMGTruthWeightAlg' )
        config.addPrivateTool( 'truthWeightTool', 'PMGTools::PMGTruthWeightTool' )
        alg.decoration = 'generatorWeight_%SYS%'
        config.addOutputVar ('EventInfo', 'generatorWeight_%SYS%', 'generatorWeight', isEventLevel=True)



class PtEtaSelectionBlock (ConfigBlock):
    """the ConfigBlock for a pt-eta selection"""

    def __init__ (self, containerName, selectionName) :
        groupName = containerName
        if selectionName != '' :
            groupName += '.' + selectionName
        super (PtEtaSelectionBlock, self).__init__ (groupName)
        self.containerName = containerName
        self.selectionName = selectionName
        self.addOption ('postfix', '', type=str)
        self.addOption ('minPt', None, type=float,
                        duplicateAction='skip')
        self.addOption ('maxEta', None, type=float,
                        duplicateAction='skip')
        self.addOption ('selectionDecoration', 'selectPtEta', type=str)

    def makeAlgs (self, config) :

        postfix = self.postfix
        if postfix != '' and postfix[0] != '_' :
            postfix = '_' + postfix

        alg = config.createAlgorithm( 'CP::AsgSelectionAlg', 'PtEtaSelectionAlg' + self.containerName + postfix )
        config.addPrivateTool( 'selectionTool', 'CP::AsgPtEtaSelectionTool' )
        bits = 0
        if self.minPt is not None :
            alg.selectionTool.minPt = self.minPt
            bits += 1
        if self.maxEta is not None :
            alg.selectionTool.maxEta = self.maxEta
            bits += 1
        if not self.selectionDecoration.find (',as_bits') :
            bits = 1
        alg.selectionDecoration = self.selectionDecoration
        alg.particles = config.readName (self.containerName)
        alg.preselection = config.getPreselection (self.containerName, '')
        config.addSelection (self.containerName, self.selectionName, alg.selectionDecoration,
                             bits=bits)



class OutputThinningBlock (ConfigBlock):
    """the ConfigBlock for output thinning"""

    def __init__ (self, containerName, configName) :
        super (OutputThinningBlock, self).__init__ (containerName + '.' + configName)
        self.containerName = containerName
        self.addOption ('postfix', '', type=str)
        self.addOption ('selection', '', type=str)
        self.addOption ('selectionName', '', type=str)
        self.addOption ('outputName', None, type=str)

    def makeAlgs (self, config) :

        postfix = self.postfix
        if postfix != '' and postfix[0] != '_' :
            postfix = '_' + postfix

        selection = config.getFullSelection (self.containerName, self.selectionName)
        if selection == '' :
            selection = self.selection
        elif self.selection != '' :
            selection = selection + '&&' + self.selection

        if selection != '' :
            alg = config.createAlgorithm( 'CP::AsgUnionSelectionAlg', 'UnionSelectionAlg' + self.containerName + postfix)
            alg.preselection = selection
            alg.particles = config.readName (self.containerName)
            alg.selectionDecoration = 'outputSelect' + postfix

        alg = config.createAlgorithm( 'CP::AsgViewFromSelectionAlg', 'DeepCopyAlg' + self.containerName + postfix )
        alg.input = config.readName (self.containerName)
        if self.outputName is not None :
            alg.output = self.outputName + '_%SYS%'
            config.addOutputContainer (self.containerName, self.outputName)
        else :
            alg.output = config.copyName (self.containerName)
        if selection != '' :
            alg.selection = ['outputSelect' + postfix]
        else :
            alg.selection = []
        alg.deepCopy = False



def makePileupReweightingConfig( seq, campaign=None, files=None, useDefaultConfig=None, userLumicalcFiles=None, userPileupConfigs=None ):
    """Create a PRW analysis config

    Keyword arguments:
    """
    # TO DO: add explanation of the keyword arguments, left to experts

    config = PileupReweightingBlock ()
    config.setOptionValue ('campaign', campaign, noneAction='ignore')
    config.setOptionValue ('files', files, noneAction='ignore')
    config.setOptionValue ('useDefaultConfig', useDefaultConfig, noneAction='ignore')
    config.setOptionValue ('userLumicalcFiles', userLumicalcFiles, noneAction='ignore')
    config.setOptionValue ('userPileupConfigs', userPileupConfigs, noneAction='ignore')
    seq.append (config)



def makeGeneratorAnalysisConfig( seq,
                                 saveCutBookkeepers=None,
                                 runNumber=None,
                                 cutBookkeepersSystematics=None ):
    """Create a generator analysis algorithm sequence

    Keyword arguments:
      saveCutBookkeepers -- save cut bokkeepers information into output file
      runNumber -- MC run number
      cutBookkeepersSystematics -- store CutBookkeepers systematics
    """

    config = GeneratorAnalysisBlock ()
    config.setOptionValue ('saveCutBookkeepers', saveCutBookkeepers, noneAction='ignore')
    config.setOptionValue ('runNumber', runNumber, noneAction='ignore')
    config.setOptionValue ('cutBookkeepersSystematics', cutBookkeepersSystematics, noneAction='ignore')
    seq.append (config)



def makePtEtaSelectionConfig( seq, containerName,
                              *, postfix = None, minPt = None, maxEta = None,
                              selectionDecoration = None, selectionName = ''):
    """Create a pt-eta kinematic selection config

    Keyword arguments:
      containerName -- name of the container
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      minPt -- minimum pt value
      maxEta -- maximum eta value
      selectionDecoration -- the name of the decoration to set
      selectionName -- the name of the selection to append this to
    """

    config = PtEtaSelectionBlock (containerName, selectionName)
    config.setOptionValue ('postfix',postfix, noneAction='ignore')
    config.setOptionValue ('minPt',minPt, noneAction='ignore')
    config.setOptionValue ('maxEta',maxEta, noneAction='ignore')
    config.setOptionValue ('selectionDecoration',selectionDecoration, noneAction='ignore')
    seq.append (config)



def makeOutputThinningConfig( seq, containerName,
                              *, postfix = None, selection = None, selectionName = None, outputName = None, configName='Thinning'):
    """Create an output thinning config

    This will do a consistent selection of output containers (if there
    is a preselection or a selection specified) and then creates a set
    of view containers (or deep copies) based on that selection.

    Keyword arguments:
      containerName -- name of the container
      postfix -- a postfix to apply to decorations and algorithm
                 names.  this is mostly used/needed when using this
                 sequence with multiple working points to ensure all
                 names are unique.
      selection -- the name of an optional selection decoration to use
      outputName -- an optional name for the output container

    """

    config = OutputThinningBlock (containerName, configName)
    config.setOptionValue ('postfix', postfix, noneAction='ignore')
    config.setOptionValue ('selection', selection, noneAction='ignore')
    config.setOptionValue ('selectionName', selectionName, noneAction='ignore')
    config.setOptionValue ('outputName', outputName, noneAction='ignore')
    seq.append (config)
