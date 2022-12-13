# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

if __name__=="__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc21_13p6TeV/AODFiles/mc21_13p6TeV.421450.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep_fct.merge.AOD.e8445_e8447_s3822_r13565_r13491/AOD.28775909._000037.pool.root.1"]
    # We have to set the production step, which PFFlow muon linking uses for autoconfiguration.
    from AthenaConfiguration.Enums import ProductionStep
    ConfigFlags.Common.ProductionStep=ProductionStep.Derivation
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(ConfigFlags))
    
    # Setup calorimeter geometry, which is needed for jet reconstruction
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(ConfigFlags))

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(ConfigFlags))

    from JetRecConfig.JetRecoSteering import JetRecoSteeringCfg
    acc.merge(JetRecoSteeringCfg(ConfigFlags))

    # We also need to build links between the newly
    # created jet constituents (GlobalFE)
    # and electrons,photons,muons and taus
    from eflowRec.PFCfg import PFGlobalFlowElementLinkingCfg
    acc.merge(PFGlobalFlowElementLinkingCfg(ConfigFlags))

    acc.run(100)

