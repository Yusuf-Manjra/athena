# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Core configuration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Local
from MuonConfig.MuonSegmentFindingConfig import MuonSegmentFindingCfg
from MuonConfig.MuonTrackBuildingConfig import MuonTrackBuildingCfg


def MuonReconstructionCfg(flags):
    # https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py
    result=ComponentAccumulator()
    result.merge( MuonSegmentFindingCfg(flags) )
    result.merge( MuonTrackBuildingCfg(flags) )
    return result
    
if __name__=="__main__":
    # To run this, do e.g. 
    # python -m MuonConfig.MuonReconstructionConfig --run --threads=1
    from MuonConfig.MuonConfigUtils import SetupMuonStandaloneArguments, SetupMuonStandaloneConfigFlags, SetupMuonStandaloneOutput, SetupMuonStandaloneCA

    args = SetupMuonStandaloneArguments()
    ConfigFlags = SetupMuonStandaloneConfigFlags(args)
    cfg = SetupMuonStandaloneCA(args,ConfigFlags)
          
    # Run the actual test.
    acc = MuonReconstructionCfg(ConfigFlags)
    cfg.merge(acc)
    
    if args.threads>1 and args.forceclone:
        from AthenaCommon.Logging import log
        log.info('Forcing track building cardinality to be equal to '+str(args.threads))
        # We want to force the algorithms to run in parallel (eventually the algorithm will be marked as cloneable in the source code)
        from GaudiHive.GaudiHiveConf import AlgResourcePool
        cfg.addService(AlgResourcePool( OverrideUnClonable=True ) )
        track_builder = acc.getPrimary()
        track_builder.Cardinality=args.threads
            
    # This is a temporary fix - it should go someplace central as it replaces the functionality of addInputRename from here:
    # https://gitlab.cern.ch/atlas/athena/blob/master/Control/SGComps/python/AddressRemappingSvc.py
    from SGComps.SGCompsConf import AddressRemappingSvc, ProxyProviderSvc
    pps = ProxyProviderSvc()
    ars=AddressRemappingSvc()
    pps.ProviderNames += [ 'AddressRemappingSvc' ]
    ars.TypeKeyRenameMaps += [ '%s#%s->%s' % ("TrackCollection", "MuonSpectrometerTracks", "MuonSpectrometerTracks_old") ]
    
    cfg.addService(pps)
    cfg.addService(ars)

    # This is a temporary fix! Should be private!
    Muon__MuonEDMHelperSvc=CompFactory.Muon__MuonEDMHelperSvc
    muon_edm_helper_svc = Muon__MuonEDMHelperSvc("MuonEDMHelperSvc")
    cfg.addService( muon_edm_helper_svc )

    itemsToRecord = ["Trk::SegmentCollection#MuonSegments", "Trk::SegmentCollection#NCB_MuonSegments"]
    itemsToRecord += ["TrackCollection#MuonSpectrometerTracks"] 
    SetupMuonStandaloneOutput(cfg, ConfigFlags, itemsToRecord)
    
    cfg.printConfig(withDetails = True, summariseProps = True)
              
    f=open("MuonReconstruction.pkl","w")
    cfg.store(f)
    f.close()
    
    if args.run:
        cfg.run(20)