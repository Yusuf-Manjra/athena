# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

def AddSUSYWeightsCfg(flags, pref = ""):
    """Add SUSY weights"""
 
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory

    acc = ComponentAccumulator()

    # Load all potential SUSY process IDs following Prospino conventions
    # https://twiki.cern.ch/twiki/bin/view/AtlasProtected/SUSYSignalUncertainties#Subprocess_IDs 
    listTools = []
    susyWeight = []
    for i in range(0, 225):
        if i==0: #flat sum of all processes (i.e. sum the weight no matter what)
            susyWeight.append( CompFactory.McEventWeight(name = pref+"SUSYWeight_ID"+"_"+str(i), UseTruthEvents = True))
        else: #add weight only to keeper associated to current process id
            susyWeight.append( CompFactory.SUSYIDWeight(name = pref+"SUSYWeight_ID"+"_"+str(i), SUSYProcID = i, UseTruthEvents = True))
        acc.addPublicTool(susyWeight[i], primary = True)
        listTools.append(susyWeight[i])

    sumOfWeightsAlg = CompFactory.SumOfWeightsAlg(name = pref+"SUSYSumWeightsAlg",
                                                  WeightTools = listTools)
    acc.addEventAlgo(sumOfWeightsAlg)
    return(acc) 
