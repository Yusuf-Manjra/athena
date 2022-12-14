# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
keys = [
    #xAOD::CaloClusterTrigAuxContainer_v1
    'HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex',

    #xAOD::TrigEMClusterAuxContainer_v2
    'HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
