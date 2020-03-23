# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

#todo? add in the explanatory text from previous implementation

def createSimConfigFlags():
    scf=AthConfigFlags()

    scf.addFlag("Sim.ISFRun",False)
    scf.addFlag("Sim.ISF.HITSMergingRequired", True)
    scf.addFlag("Sim.ParticleID",False)
    scf.addFlag("Sim.CalibrationRun", "DeadLAr") # "LAr", "Tile", "LAr+Tile", "DeadLAr", "Off"

    scf.addFlag("Sim.CavernBG",False) #"Write" , "Read" , "Signal" , "WriteWorld" , "SignalWorld"
    scf.addFlag("Sim.ReadTR",False)
    scf.addFlag("Sim.WorldRRange", False) #12500. #int or float
    scf.addFlag("Sim.WorldZRange", False) #22031. #int or float

    # the G4 offset. It was never changed, so no need to peek in file
    scf.addFlag("Sim.SimBarcodeOffset", 200000)

    #for forward region
    scf.addFlag('Sim.TwissFileBeam1',False)
    scf.addFlag('Sim.TwissFileBeam2',False)
    scf.addFlag('Sim.TwissEnergy',8000000.)
    scf.addFlag('Sim.TwissFileBeta',550.)
    scf.addFlag('Sim.TwissFileNomReal',False) #'nominal','real' #default to one of these?!
    scf.addFlag('Sim.TwissFileVersion','v01')

    #for G4AtlasAlg
    #in simflags
    scf.addFlag('Sim.ReleaseGeoModel',True)
    scf.addFlag('Sim.RecordFlux',False)
    scf.addFlag('Sim.TruthStrategy','MC12') #todo - needs to have some extra functionality!
    scf.addFlag('Sim.G4Commands',['/run/verbose 2'])
    #in atlasflags
    scf.addFlag('Sim.FlagAbortedEvents',False)
    scf.addFlag('Sim.KillAbortedEvents',True)

    # Do full simulation + digitisation + reconstruction chain
    scf.addFlag("Sim.DoFullChain", False)

    scf.addFlag("Sim.G4Version", "geant4.10.1.patch03.atlas02")
    scf.addFlag("Sim.PhysicsList", "FTFP_BERT_ATL")
    scf.addFlag("Sim.NeutronTimeCut", 150.) # Sets the value for the neutron out of time cut in G4
    scf.addFlag("Sim.NeutronEnergyCut", -1.) # Sets the value for the neutron energy cut in G4
    scf.addFlag("Sim.ApplyEMCuts", False) # Turns on the G4 option to apply cuts for EM physics

    #For G4AtlasToolsConfig
    scf.addFlag('Sim.RecordStepInfo',False) 
    scf.addFlag('Sim.StoppedParticleFile', False) 
    scf.addFlag('Sim.BeamPipeSimMode', 'Normal')  ## ['Normal', 'FastSim', 'EGammaRangeCuts', 'EGammaPRangeCuts']
    scf.addFlag('Sim.LArParameterization', 2)  ## 0 = No frozen showers, 1 = Frozen Showers, 2 = DeadMaterial Frozen Showers

    #For BeameffectsAlg
    scf.addFlag('Sim.Vertex.Source', 'CondDB' ) #'CondDB', 'VertexOverrideEventFile.txt', 'VertexOverride.txt',"LongBeamspot"

    #for G4UserActions
    scf.addFlag('Sim.Layout','ATLAS-R2-2015-03-01-00')
    scf.addFlag('Sim.NRRThreshold', False)
    scf.addFlag('Sim.NRRWeight', False)
    scf.addFlag('Sim.PRRThreshold', False)
    scf.addFlag('Sim.PRRWeight', False)

    # For G4FieldConfigNew
    scf.addFlag('Sim.G4Stepper', 'AtlasRK4')
    scf.addFlag('Sim.G4EquationOfMotion', '')

    scf.addFlag('Sim.UsingGeant4', True)
    return scf

