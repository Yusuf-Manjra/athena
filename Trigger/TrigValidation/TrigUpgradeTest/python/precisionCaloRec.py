#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from egammaRec.egammaRecFlags import jobproperties
from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
jobproperties.egammaRecFlags.print_JobProperties("full")

def precisionCaloRecoSequence(RoIs):
    egammaTopoClusterCopier = AlgFactory( egammaAlgsConf.egammaTopoClusterCopier,
                                          name = 'egammaTopoClusterCopier' ,
                                          #InputTopoCollection=jobproperties.egammaRecFlags.inputTopoClusterCollection(),
                                          InputTopoCollection= "caloclusters",
                                          OutputTopoCollection=jobproperties.egammaRecFlags.egammaTopoClusterCollection(),
                                          #OutputTopoCollection="ClustersName",
                                          OutputTopoCollectionShallow="tmp_"+jobproperties.egammaRecFlags.egammaTopoClusterCollection(),
                                          doAdd = False )

    from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
    (precisionRecoSequence, caloclusters) = HLTFSTopoRecoSequence(RoIs)

    algo = egammaTopoClusterCopier()
    algo.InputTopoCollection = caloclusters
    precisionRecoSequence += algo
    sequenceOut = algo.OutputTopoCollection

    return (precisionRecoSequence, sequenceOut)
