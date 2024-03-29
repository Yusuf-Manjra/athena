# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# AnaAlgorithm import(s):
from AnalysisAlgorithmsConfig.ConfigBlock import ConfigBlock


class OverlapAnalysisConfig (ConfigBlock):
    """the ConfigBlock for the MET configuration"""

    def __init__ (self, configName) :
        super (OverlapAnalysisConfig, self).__init__ (configName)
        self.addOption ('inputLabel', '', type=str)
        self.addOption ('outputLabel', 'passesOR', type=str)
        self.addOption ('linkOverlapObjects', False, type=bool)
        self.addOption ('doEleEleOR', False, type=bool)
        self.addOption ('enableUserPriority', False, type=bool)
        self.addOption ('bJetLabel', '', type=str)
        self.addOption ('boostedLeptons', False, type=bool)
        self.addOption ('postfix', '', type=str)
        self.addOption ('jets', "", type=str)
        self.addOption ('fatJets', "", type=str)
        self.addOption ('electrons', "", type=str)
        self.addOption ('muons', "", type=str)
        self.addOption ('photons', "", type=str)
        self.addOption ('taus', "", type=str)


    def makeAlgs (self, config) :

        postfix = self.postfix

        # For now we have to decorate our selections on the objects in
        # separate algorithms beforehand, so that the overlap
        # algorithm can read them.  This should probably be changed at
        # some point.  For one, this seems like an unnecessary
        # complication in the configuration.  For another, it requires
        # that all selection systematics applied so far have dedicated
        # shallow copies for all objects and systematics, i.e. be
        # kinematic systematics.  While that is technically the case
        # right now, I'd prefer if I didn't force that.

        electrons = None
        if self.electrons != "" :
            alg = config.createAlgorithm( 'CP::AsgSelectionAlg','ORElectronsSelectAlg' + postfix )
            electrons, alg.preselection = config.readNameAndSelection (self.electrons)
            alg.particles = electrons
            alg.selectionDecoration = self.inputLabel + ',as_char'
            config.addOutputVar (self.electrons.split('.')[0], self.outputLabel + '_%SYS%', 'select_or')

        photons = None
        if self.photons != "" :
            alg = config.createAlgorithm( 'CP::AsgSelectionAlg','ORPhotonsSelectAlg' + postfix )
            photons, alg.preselection = config.readNameAndSelection (self.photons)
            alg.particles = photons
            alg.selectionDecoration = self.inputLabel + ',as_char'
            config.addOutputVar (self.photons.split('.')[0], self.outputLabel + '_%SYS%', 'select_or')

        muons = None
        if self.muons != "" :
            alg = config.createAlgorithm( 'CP::AsgSelectionAlg','ORMuonsSelectAlg' + postfix )
            muons, alg.preselection = config.readNameAndSelection (self.muons)
            alg.particles = muons
            alg.selectionDecoration = self.inputLabel + ',as_char'
            config.addOutputVar (self.muons.split('.')[0], self.outputLabel + '_%SYS%', 'select_or')

        taus = None
        if self.taus != "" :
            alg = config.createAlgorithm( 'CP::AsgSelectionAlg','ORTausSelectAlg' + postfix )
            taus, alg.preselection = config.readNameAndSelection (self.taus)
            alg.particles = taus
            alg.selectionDecoration = self.inputLabel + ',as_char'
            config.addOutputVar (self.taus.split('.')[0], self.outputLabel + '_%SYS%', 'select_or')

        jets = None
        if self.jets != "" :
            alg = config.createAlgorithm( 'CP::AsgSelectionAlg','ORJetsSelectAlg' + postfix )
            jets, alg.preselection = config.readNameAndSelection (self.jets)
            alg.particles = jets
            alg.selectionDecoration = self.inputLabel + ',as_char'
            config.addOutputVar (self.jets.split('.')[0], self.outputLabel + '_%SYS%', 'select_or')

        fatJets = None
        if self.fatJets != "" :
            alg = config.createAlgorithm( 'CP::AsgSelectionAlg','ORFatJetsSelectAlg' + postfix )
            fatJets, alg.preselection = config.readNameAndSelection (self.fatJets)
            alg.particles = fatJets
            alg.selectionDecoration = self.inputLabel + ',as_char'
            config.addOutputVar (self.fatJets.split('.')[0], self.outputLabel + '_%SYS%', 'select_or')


        # Create the overlap removal algorithm:
        alg = config.createAlgorithm( 'CP::OverlapRemovalAlg', 'OverlapRemovalAlg' + postfix )
        alg.OutputLabel = self.outputLabel
        if electrons :
            alg.electrons = electrons
            alg.electronsDecoration = self.outputLabel + '_%SYS%,as_char'
            config.addSelection (electrons, self.outputLabel, alg.electronsDecoration, bits=1)
        if muons :
            alg.muons = muons
            alg.muonsDecoration = self.outputLabel + '_%SYS%,as_char'
            config.addSelection (muons, self.outputLabel, alg.muonsDecoration, bits=1)
        if taus :
            alg.taus = taus
            alg.tausDecoration = self.outputLabel + '_%SYS%,as_char'
            config.addSelection (taus, self.outputLabel, alg.tausDecoration, bits=1)
        if jets :
            alg.jets = jets
            alg.jetsDecoration = self.outputLabel + '_%SYS%,as_char'
            config.addSelection (jets, self.outputLabel, alg.jetsDecoration, bits=1)
        if photons :
            alg.photons = photons
            alg.photonsDecoration = self.outputLabel + '_%SYS%,as_char'
            config.addSelection (photons, self.outputLabel, alg.photonsDecoration, bits=1)
        if fatJets :
            alg.fatJets = fatJets
            alg.fatJetsDecoration = self.outputLabel + '_%SYS%,as_char'
            config.addSelection (fatJets, self.outputLabel, alg.fatJetsDecoration, bits=1)

        # Create its main tool, and set its basic properties:
        config.addPrivateTool( 'overlapTool', 'ORUtils::OverlapRemovalTool' )
        alg.overlapTool.InputLabel = self.inputLabel
        alg.overlapTool.OutputLabel = self.outputLabel

        # By default the OverlapRemovalTool would flag objects that need to be
        # suppressed, with a "true" value. But since the analysis algorithms expect
        # the opposite behaviour from selection flags, we need to tell the tool
        # explicitly to use the "true" flag on objects that pass the overlap
        # removal.
        alg.overlapTool.OutputPassValue = True

        # Set up the electron-electron overlap removal, if requested.
        if electrons and self.doEleEleOR:
            config.addPrivateTool( 'overlapTool.EleEleORT',
                                   'ORUtils::EleEleOverlapTool' )
            alg.overlapTool.EleEleORT.InputLabel = self.inputLabel
            alg.overlapTool.EleEleORT.OutputLabel = self.outputLabel
            alg.overlapTool.EleEleORT.LinkOverlapObjects = self.linkOverlapObjects
            alg.overlapTool.EleEleORT.OutputPassValue = True

        # Set up the electron-muon overlap removal.
        if electrons and muons:
            config.addPrivateTool( 'overlapTool.EleMuORT',
                                   'ORUtils::EleMuSharedTrkOverlapTool' )
            alg.overlapTool.EleMuORT.InputLabel = self.inputLabel
            alg.overlapTool.EleMuORT.OutputLabel = self.outputLabel
            alg.overlapTool.EleMuORT.LinkOverlapObjects = self.linkOverlapObjects
            alg.overlapTool.EleMuORT.OutputPassValue = True

        # Set up the electron-(narrow-)jet overlap removal.
        if electrons and jets:
            config.addPrivateTool( 'overlapTool.EleJetORT',
                                   'ORUtils::EleJetOverlapTool' )
            alg.overlapTool.EleJetORT.InputLabel = self.inputLabel
            alg.overlapTool.EleJetORT.OutputLabel = self.outputLabel
            alg.overlapTool.EleJetORT.LinkOverlapObjects = self.linkOverlapObjects
            alg.overlapTool.EleJetORT.BJetLabel = self.bJetLabel
            alg.overlapTool.EleJetORT.UseSlidingDR = self.boostedLeptons
            alg.overlapTool.EleJetORT.EnableUserPriority = self.enableUserPriority
            alg.overlapTool.EleJetORT.OutputPassValue = True

        # Set up the muon-(narrow-)jet overlap removal.
        if muons and jets:
            config.addPrivateTool( 'overlapTool.MuJetORT',
                                   'ORUtils::MuJetOverlapTool' )
            alg.overlapTool.MuJetORT.InputLabel = self.inputLabel
            alg.overlapTool.MuJetORT.OutputLabel = self.outputLabel
            alg.overlapTool.MuJetORT.LinkOverlapObjects = self.linkOverlapObjects
            alg.overlapTool.MuJetORT.BJetLabel = self.bJetLabel
            alg.overlapTool.MuJetORT.UseSlidingDR = self.boostedLeptons
            alg.overlapTool.MuJetORT.EnableUserPriority = self.enableUserPriority
            alg.overlapTool.MuJetORT.OutputPassValue = True

        # Set up the tau-electron overlap removal.
        if taus and electrons:
            config.addPrivateTool( 'overlapTool.TauEleORT',
                                   'ORUtils::DeltaROverlapTool' )
            alg.overlapTool.TauEleORT.InputLabel = self.inputLabel
            alg.overlapTool.TauEleORT.OutputLabel = self.outputLabel
            alg.overlapTool.TauEleORT.LinkOverlapObjects = self.linkOverlapObjects
            alg.overlapTool.TauEleORT.DR = 0.2
            alg.overlapTool.TauEleORT.OutputPassValue = True

        # Set up the tau-muon overlap removal.
        if taus and muons:
            config.addPrivateTool( 'overlapTool.TauMuORT',
                                   'ORUtils::DeltaROverlapTool' )
            alg.overlapTool.TauMuORT.InputLabel = self.inputLabel
            alg.overlapTool.TauMuORT.OutputLabel = self.outputLabel
            alg.overlapTool.TauMuORT.LinkOverlapObjects = self.linkOverlapObjects
            alg.overlapTool.TauMuORT.DR = 0.2
            alg.overlapTool.TauMuORT.OutputPassValue = True

        # Set up the tau-(narrow-)jet overlap removal.
        if taus and jets:
            config.addPrivateTool( 'overlapTool.TauJetORT',
                                   'ORUtils::DeltaROverlapTool' )
            alg.overlapTool.TauJetORT.InputLabel = self.inputLabel
            alg.overlapTool.TauJetORT.OutputLabel = self.outputLabel
            alg.overlapTool.TauJetORT.LinkOverlapObjects = self.linkOverlapObjects
            alg.overlapTool.TauJetORT.DR = 0.2
            alg.overlapTool.TauJetORT.EnableUserPriority = self.enableUserPriority
            alg.overlapTool.TauJetORT.OutputPassValue = True

        # Set up the photon-electron overlap removal.
        if photons and electrons:
            config.addPrivateTool( 'overlapTool.PhoEleORT',
                                   'ORUtils::DeltaROverlapTool' )
            alg.overlapTool.PhoEleORT.InputLabel = self.inputLabel
            alg.overlapTool.PhoEleORT.OutputLabel = self.outputLabel
            alg.overlapTool.PhoEleORT.LinkOverlapObjects = self.linkOverlapObjects
            alg.overlapTool.PhoEleORT.OutputPassValue = True

        # Set up the photon-muon overlap removal.
        if photons and muons:
            config.addPrivateTool( 'overlapTool.PhoMuORT',
                                   'ORUtils::DeltaROverlapTool' )
            alg.overlapTool.PhoMuORT.InputLabel = self.inputLabel
            alg.overlapTool.PhoMuORT.OutputLabel = self.outputLabel
            alg.overlapTool.PhoMuORT.LinkOverlapObjects = self.linkOverlapObjects
            alg.overlapTool.PhoMuORT.OutputPassValue = True

        # Set up the photon-(narrow-)jet overlap removal.
        if photons and jets:
            config.addPrivateTool( 'overlapTool.PhoJetORT',
                                   'ORUtils::DeltaROverlapTool' )
            alg.overlapTool.PhoJetORT.InputLabel = self.inputLabel
            alg.overlapTool.PhoJetORT.OutputLabel = self.outputLabel
            alg.overlapTool.PhoJetORT.LinkOverlapObjects = self.linkOverlapObjects
            alg.overlapTool.PhoJetORT.EnableUserPriority = self.enableUserPriority
            alg.overlapTool.PhoJetORT.OutputPassValue = True

        # Set up the electron-fat-jet overlap removal.
        if electrons and fatJets:
            config.addPrivateTool( 'overlapTool.EleFatJetORT',
                            'ORUtils::DeltaROverlapTool' )
            alg.overlapTool.EleFatJetORT.InputLabel = self.inputLabel
            alg.overlapTool.EleFatJetORT.OutputLabel = self.outputLabel
            alg.overlapTool.EleFatJetORT.LinkOverlapObjects = self.linkOverlapObjects
            alg.overlapTool.EleFatJetORT.DR = 1.0
            alg.overlapTool.EleFatJetORT.OutputPassValue = True

        # Set up the (narrow-)jet-fat-jet overlap removal.
        if jets and fatJets:
            config.addPrivateTool( 'overlapTool.JetFatJetORT',
                                   'ORUtils::DeltaROverlapTool' )
            alg.overlapTool.JetFatJetORT.InputLabel = self.inputLabel
            alg.overlapTool.JetFatJetORT.OutputLabel = self.outputLabel
            alg.overlapTool.JetFatJetORT.LinkOverlapObjects = self.linkOverlapObjects
            alg.overlapTool.JetFatJetORT.DR = 1.0
            alg.overlapTool.JetFatJetORT.OutputPassValue = True



def makeOverlapAnalysisConfig( seq,
                               inputLabel = None, outputLabel = None,
                               linkOverlapObjects = None,
                               doEleEleOR = None, electrons = None,
                               muons = None, jets = None, taus = None,
                               photons = None, fatJets = None,
                               enableUserPriority = None,
                               bJetLabel = None,
                               boostedLeptons = None,
                               postfix = None,
                               configName = 'OverlapRemoval'):
    """Function creating the overlap removal algorithm sequence

    The function sets up a multi-input/multi-output analysis algorithm sequnce,
    which needs to be used in a quite particular way. First off you need to set
    the arguments of this function correctly.

    Function keyword arguments:
      inputLabel -- Any possible label used to pick up the selected objects
                    with.  This should not be a label already used otherwise.
      outputLabel -- Decoration put on the output variables. Set to "true" for
                     objects passing the overlap removal.
      linkOverlapObjects -- Set up an element link between overlapping objects
      doEleEleOR -- Set up electron-electron overlap removal
      enableUserPriority -- If enabled, the Ele-, Mu-, Tau- and PhoJetOR tools
                            will respect the user priority in the inputLabel.
                            E.g. SUSYTools assigns all signal objects the
                            priority 2 and pre-selected jets the priority 1.
      bJetLabel -- Flag to select b-jets with. If left empty, no b-jets are used
                   in the overlap removal.
      boostedLeptons -- Set to True to enable boosted lepton overlap removal
    """

    config = OverlapAnalysisConfig (configName)
    config.setOptionValue ('inputLabel', inputLabel, noneAction='ignore')
    config.setOptionValue ('outputLabel', outputLabel, noneAction='ignore')
    config.setOptionValue ('linkOverlapObjects', linkOverlapObjects, noneAction='ignore')
    config.setOptionValue ('doEleEleOR', doEleEleOR, noneAction='ignore')
    config.setOptionValue ('electrons', electrons, noneAction='ignore')
    config.setOptionValue ('muons', muons, noneAction='ignore')
    config.setOptionValue ('jets', jets, noneAction='ignore')
    config.setOptionValue ('taus', taus, noneAction='ignore')
    config.setOptionValue ('photons', photons, noneAction='ignore')
    config.setOptionValue ('fatJets', fatJets, noneAction='ignore')
    config.setOptionValue ('enableUserPriority', enableUserPriority, noneAction='ignore')
    config.setOptionValue ('bJetLabel', bJetLabel, noneAction='ignore')
    config.setOptionValue ('boostedLeptons', boostedLeptons, noneAction='ignore')
    config.setOptionValue ('postfix', postfix, noneAction='ignore')
    seq.append (config)
