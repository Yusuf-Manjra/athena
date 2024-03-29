# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createTrigCaloConfigFlags(): 
    tccf=AthConfigFlags()

    # Enable cell timing cut
    tccf.addFlag('Trigger.Calo.TopoCluster.doTimeCut', False)
    tccf.addFlag('Trigger.Calo.TopoCluster.extendTimeCut', False)
    tccf.addFlag('Trigger.Calo.TopoCluster.useUpperLimitForTimeCut', False)
    tccf.addFlag('Trigger.Calo.TopoCluster.timeCutUpperLimit', 20.0)

    return tccf
