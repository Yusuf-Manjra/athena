# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

if __name__=="__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc21_13p6TeV/ESDFiles/mc21_13p6TeV.421450.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep_fct.recon.ESD.e8445_e8447_s3822_r13565/ESD.28877240._000046.pool.root.1"]
    #Use latest MC21 tag to pick up latest muon folders apparently needed
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC21-SDR-RUN3-10"
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(ConfigFlags))
    
    from xAODTruthCnv.xAODTruthCnvConfig import GEN_AOD2xAODCfg
    acc.merge(GEN_AOD2xAODCfg(ConfigFlags))

    from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedReconstructionCfg
    acc.merge(MuonCombinedReconstructionCfg(ConfigFlags))

    acc.run(100)

