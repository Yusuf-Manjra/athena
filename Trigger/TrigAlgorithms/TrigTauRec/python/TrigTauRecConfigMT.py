# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigTauRec.TrigTauRecConf import TrigTauRecMergedMT

class TrigTauRecMerged_TauCaloOnly (TrigTauRecMergedMT) :

        def __init__(self, name = "TrigTauRecMerged_TauCaloOnly"):
            super( TrigTauRecMerged_TauCaloOnly , self ).__init__( name )

            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            taualgs.setPrefix("TrigTauCaloOnly_")
            
            
            # Only include tools needed for calo pre-selection
            
            # Set seedcalo energy scale (Full RoI)
            tools.append(taualgs.getJetSeedBuilder())
            # Set LC energy scale (0.2 cone) and intermediate axis (corrected for vertex: useless at trigger)
            tools.append(taualgs.getTauAxis())
            # Calibrate to TES
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy', caloOnly=True))
            # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
            tools.append(taualgs.getCellVariables(cellConeSize=0.2))

            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'
                pass

            self.Tools = tools

            ## add beam type flag
            from AthenaCommon.BeamFlags import jobproperties
            self.BeamType = jobproperties.Beam.beamType()


class TrigTauRecMerged_TauCaloOnlyMVA (TrigTauRecMergedMT) :

        def __init__(self, name = "TrigTauRecMerged_TauCaloOnlyMVA"):
            super( TrigTauRecMerged_TauCaloOnlyMVA , self ).__init__( name )

            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            taualgs.setPrefix("TrigTauCaloOnlyMVA_")


            # Only include tools needed for calo pre-selection

            # Set seedcalo energy scale (Full RoI)
            tools.append(taualgs.getJetSeedBuilder())
            # Set LC energy scale (0.2 cone) and intermediate axis (corrected for vertex: useless at trigger)
            tools.append(taualgs.getTauAxis())
            # Calibrate to TES
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy', caloOnly=True))
            # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
            tools.append(taualgs.getCellVariables(cellConeSize=0.2))
            # Compute MVA TES (ATR-17649), stores MVA TES as default tau pt()
            tools.append(taualgs.getMvaTESVariableDecorator())
            tools.append(taualgs.getMvaTESEvaluator())

            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'
                pass

            self.Tools = tools

            ## add beam type flag
            from AthenaCommon.BeamFlags import jobproperties
            self.BeamType = jobproperties.Beam.beamType()



class TrigTauRecMerged_TauPrecisionMVA (TrigTauRecMergedMT) :

        def __init__(self, name = "TrigTauRecMerged_TauPrecisionMVA", doMVATES=False, doTrackBDT=False, doRNN=False):
        
            super( TrigTauRecMerged_TauPrecisionMVA , self ).__init__( name )

            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            # using same prefix as in TauPrecision sequence should be safe if tools with different configurations have different names
            # e.g. TauTrackFinder in 2016 using dz0=2mm instead of 1mm in 2017
            taualgs.setPrefix("TrigTau_")


            # Include full set of tools
            # Set seedcalo energy scale (Full RoI), will keep the line below commented out for now
            #tools.append(taualgs.getJetSeedBuilder())
            # Associate RoI vertex or Beamspot to tau - don't use TJVA
            #Comment this tool for now
            #tools.append(taualgs.getTauVertexFinder(doUseTJVA=False)) #don't use TJVA by default
            # Set LC energy scale (0.2 cone) and intermediate axis (corrected for vertex: useless at trigger)       
            #Comment this tool for now
            #tools.append(taualgs.getTauAxis())
            
            # for now, use 'doMVATES=False' to detect tracktwoEF, instead of introducing new flag
            if not doMVATES:
                # Count tracks with deltaZ0 cut of 1mm for tracktwoEF           
                tools.append(taualgs.getTauTrackFinder(applyZ0cut=True, maxDeltaZ0=1))
            else:
                # tightened to 0.75 mm for tracktwoMVA (until the track BDT can be used)
                tools.append(taualgs.getTauTrackFinder(applyZ0cut=True, maxDeltaZ0=0.75, prefix='TrigTauTightDZ_'))            

            if doTrackBDT:                
                # BDT track classification
                tools.append(taualgs.getTauTrackClassifier())

            # Calibrate to calo TES
            tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=True, correctAxis=False, postfix='_onlyEnergy'))

            if doMVATES:
                # Compute MVA TES (ATR-17649), stores MVA TES as default tau pt()
                tools.append(taualgs.getMvaTESVariableDecorator())
                tools.append(taualgs.getMvaTESEvaluator())

            # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
            tools.append(taualgs.getCellVariables(cellConeSize=0.2))
            # tools.append(taualgs.getElectronVetoVars())
            # Lifetime variables
            tools.append(taualgs.getTauVertexVariables())
            # Variables combining tracking and calorimeter information
            tools.append(taualgs.getTauCommonCalcVars())
            # Cluster-based sub-structure, with dRMax also
            tools.append(taualgs.getTauSubstructure())
            # tools.append(taualgs.getEnergyCalibrationLC(correctEnergy=False, correctAxis=True, postfix='_onlyAxis'))
            tools.append(taualgs.getPileUpCorrection())

            if doRNN:
                # RNN tau ID
                tools.append(taualgs.getTauJetRNNEvaluator(NetworkFile0P="rnnid_config_0p_v3.json",
                                                           NetworkFile1P="rnnid_config_1p_v3.json",
                                                           NetworkFile3P="rnnid_config_mp_v3.json",
                                                           MaxTracks=10, 
                                                           MaxClusters=6,
                                                           MaxClusterDR=1.0))
                # flattened RNN score and WP
                tools.append(taualgs.getTauWPDecoratorJetRNN())


            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'
                pass

            self.Tools = tools

            ## add beam type flag
            from AthenaCommon.BeamFlags import jobproperties
            self.BeamType = jobproperties.Beam.beamType()
