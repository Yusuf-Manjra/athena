# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags

from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )


def MenuPrescaleConfig(triggerConfigHLT):

    L1Prescales = {}
    HLTPrescales = {}
    Prescales = PrescaleClass()

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    menu_name = ConfigFlags.Trigger.triggerMenuSetup

    ## Do some aliasing here
    if menu_name == 'Physics_default': 
        menu_name = 'LS2_v1'
    elif menu_name == 'MC_loose_default': 
        menu_name = 'LS2_v1'
    elif menu_name == 'MC_tight_default': 
        menu_name = 'LS2_v1'

    log.info('Menu name: %s', menu_name)

    if menu_name.startswith('LS2_v1'):
        from TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1 import setupMenu
        setupMenu()
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        elif 'Primary_prescale' in menu_name:
            enableChains(TriggerFlags, Prescales.HLTPrescales_primary_prescale, ['Primary:L1Muon','Primary:Legacy','Primary:PhaseI','Primary:CostAndRate'])
            L1Prescales = Prescales.L1Prescales_trigvalid_prescale
            HLTPrescales = Prescales.HLTPrescales_primary_prescale
        elif 'TriggerValidation_prescale' in menu_name:
            disableChains(TriggerFlags, Prescales.HLTPrescales_trigvalid_prescale, ["PS:Online"])
            L1Prescales = Prescales.L1Prescales_trigvalid_prescale
            HLTPrescales = Prescales.HLTPrescales_trigvalid_prescale
        elif 'BulkMCProd_prescale' in menu_name:
            disableChains(TriggerFlags, Prescales.HLTPrescales_bulkmcprod_prescale, ["PS:Online"])
            L1Prescales = Prescales.L1Prescales_bulkmcprod_prescale
            HLTPrescales = Prescales.HLTPrescales_bulkmcprod_prescale
        elif 'CPSampleProd_prescale' in menu_name:
            disableChains(TriggerFlags, Prescales.HLTPrescales_cpsampleprod_prescale, ["PS:Online"])
            L1Prescales = Prescales.L1Prescales_cpsampleprod_prescale
            HLTPrescales = Prescales.HLTPrescales_cpsampleprod_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales        

    elif menu_name.startswith('Physics_pp_run3_v1'):
        from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import setupMenu
        setupMenu()
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales        

    elif menu_name.startswith('PhysicsP1_pp_run3_v1'):
        from TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_pp_run3_v1 import setupMenu
        setupMenu()
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales        

    elif menu_name.startswith('MC_pp_run3_v1'):
        from TriggerMenuMT.HLTMenuConfig.Menu.MC_pp_run3_v1 import setupMenu
        setupMenu()
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales        

    elif menu_name.startswith('PhysicsP1_HI_run3_v1'):
        from TriggerMenuMT.HLTMenuConfig.Menu.PhysicsP1_HI_run3_v1 import setupMenu
        setupMenu()
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales        

    elif menu_name.startswith('Dev_HI_run3_v1'):
        from TriggerMenuMT.HLTMenuConfig.Menu.Dev_HI_run3_v1 import setupMenu
        setupMenu()
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales        

    elif menu_name.startswith('LS2_emu_v1'):
        from TriggerMenuMT.HLTMenuConfig.Menu.LS2_v1 import setupMenu
        setupMenu()
        if 'tight_mc_prescale' in menu_name:
            L1Prescales = Prescales.L1Prescales_tight_mc_prescale
            HLTPrescales = Prescales.HLTPrescales_tight_mc_prescale
        else:
            L1Prescales = Prescales.L1Prescales
            HLTPrescales = Prescales.HLTPrescales        

    elif menu_name.startswith('Cosmic_run3_v1'):
        from TriggerMenuMT.HLTMenuConfig.Menu.Cosmic_run3_v1 import setupMenu
        setupMenu()
        L1Prescales = Prescales.L1Prescales
        HLTPrescales = Prescales.HLTPrescales
            
    else:
        log.fatal('Menu with name %s is not known in this version of TriggerMenu! ', menu_name)
        return

    return (L1Prescales, HLTPrescales)

def disableChains(flags, type_prescales, type_groups):
    signatures = []
    slice_props = [prop for prop in dir(flags) if prop.endswith("Slice")]
    for slice_prop in slice_props:
        slice = getattr(flags, slice_prop)
        if slice.signatures():
            signatures.extend(slice.signatures())
        else:
            log.debug('SKIPPING %s', slice_prop)

    chain_disable_list=[]

    for chain in signatures:
        toKeep = True
        for group in type_groups:
            if group in chain.groups:
                toKeep = False
        if not toKeep:
            chain_disable_list.append(chain.name)

    type_prescales.update(zip(chain_disable_list,len(chain_disable_list)*[ [-1] ]))

def enableChains(flags, type_prescales, type_groups):
    signatures = []
    slice_props = [prop for prop in dir(flags) if prop.endswith("Slice")]
    for slice_prop in slice_props:
        slice = getattr(flags, slice_prop)
        if slice.signatures():
            signatures.extend(slice.signatures())
        else:
            log.debug('SKIPPING %s', slice_prop)

    chain_disable_list=[]

    for chain in signatures:
        toKeep = False
        for group in type_groups:
            if group in chain.groups:
                toKeep = True
        if not toKeep:
            chain_disable_list.append(chain.name)

    type_prescales.update(zip(chain_disable_list,len(chain_disable_list)*[ [-1] ]))

def applyHLTPrescale(triggerPythonConfig, HLTPrescale, signaturesOverwritten):
    for item, prescales in HLTPrescale.items():
        if item not in triggerPythonConfig.dicts().keys():
            if signaturesOverwritten:
                log.warning('Attempt to set prescales for nonexisting chain: %s', item)
                continue
            else:
                log.error('Attempt to set prescales for nonexisting chain: %s', item)
                continue
        n = len(prescales)
        hltchain = triggerPythonConfig.dicts()[item]
        if n > 0:
            hltchain['prescale'] = str(prescales[0])
        log.info('Applied HLTPS to the item %s: PS %s', item, hltchain['prescale'])
       
class PrescaleClass(object):
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {}
    L1Prescales = dict([(ctpid,1) for ctpid in L1Prescales])  # setting all L1 prescales to 1 # NOT DOING ANYTHING ATM

    #   Signature name   | [ HLTprescale ]
    #   - Chains only need adding if have a Prescale value different from 1 (default)
    #----------------------------------------------------------
    HLTPrescales = {}

    L1Prescales_primary_prescale  = {}
    HLTPrescales_primary_prescale = {}

    L1Prescales_trigvalid_prescale  = {}
    HLTPrescales_trigvalid_prescale = {}

    L1Prescales_bulkmcprod_prescale  = {}
    HLTPrescales_bulkmcprod_prescale = {}

    L1Prescales_cpsampleprod_prescale  = {}
    HLTPrescales_cpsampleprod_prescale = {}

    chain_list=[]
