# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def defineMenu():

    import TriggerMenuMT.L1.Menu.Menu_MC_pp_v8 as mc_menu
    from TriggerMenuMT.L1.Base.L1MenuFlags import L1MenuFlags

    mc_menu.defineMenu()

    L1MenuFlags.ThresholdMap = {
        # Do not use for L1Topo decision threshold!
        'jXEPerf50' :'',
    }

    L1MenuFlags.ItemMap = {
        'L1_jXEPerf50':'', 
    } 

    #----------------------------------------------
    def remapItems():  
        itemsToRemove = []
        for itemIndex, itemName in enumerate(L1MenuFlags.items()):
            if (itemName in L1MenuFlags.ItemMap()):
                if (L1MenuFlags.ItemMap()[itemName] != ''):
                    L1MenuFlags.items()[itemIndex] = L1MenuFlags.ItemMap()[itemName]                                                
                else: 
                    itemsToRemove.append(itemIndex)

        for i in reversed(itemsToRemove):
            del L1MenuFlags.items()[i]
    #----------------------------------------------
                                           
    remapItems()

