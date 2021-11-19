#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigDecisionToolConfig.py
@brief Configures and returns the TrigDecisionTool (TDT) for use in Athena-MT. 
The TDT uses a Tool interface such that it can be used in either Athena or AnalysisBase releases.
The manages an MT-safe internal state, hence we should only have one instance of it configured in any job.
It is thus one of the few places where it OK to use a PublicTool in Athena-MT.

Obtain the configured public tool instance by calling getPrimary()
on the ComponentAccumulator returned by this function, or by calling
getPublicTool("TrigDecisionTool") on any downstream merged ComponentAccumulator

When running in AnalysisBase, the tdt.TrigConfigTool="TrigConf::xAODConfigTool" should be used 
in place of the TrigConf::xAODConfigSvc
'''

# List of all possible keys of the Run 3 navigation summary collection
# in order of verbosity. Want to take the most verbose which is available.
possible_keys = [
    "HLTNav_Summary",
    "HLTNav_Summary_OnlineSlimmed",
    "HLTNav_Summary_ESDSlimmed",
    "HLTNav_Summary_AODSlimmed",
    "HLTNav_Summary_DAODSlimmed"
    ]

def getRun3NavigationContainerFromInput(ConfigFlags):
    # What to return if we cannot look in the file
    default_key = "HLTNav_Summary_OnlineSlimmed" if ConfigFlags.Trigger.doOnlineNavigationCompactification else "HLTNav_Summary"
    to_return = default_key

    for key in possible_keys:
        if key in ConfigFlags.Input.Collections:
            to_return = key
            break

    from AthenaCommon.Logging import logging
    msg = logging.getLogger('getRun3NavigationContainerFromInput')
    msg.info("Returning {} as the Run 3 trigger navigation colletion to read in this job.".format(to_return))
    return to_return

def getTrigDecisionTool(ConfigFlags):
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaCommon.Logging import logging
    msg = logging.getLogger('getTrigDecisionTool')

    from TrigConfxAOD.TrigConfxAODConfig import getxAODConfigSvc
    acc = getxAODConfigSvc(ConfigFlags)
    cfgsvc = acc.getPrimary()

    tdt = CompFactory.Trig.TrigDecisionTool('TrigDecisionTool')
    tdt.TrigConfigSvc = cfgsvc
    use_run3_format = ConfigFlags.Trigger.EDMVersion == 3 # or ConfigFlags.Trigger.doEDMVersionConversion (to be added when this is working)
    tdt.NavigationFormat = "TrigComposite" if use_run3_format else "TriggerElement"
    tdt.HLTSummary = getRun3NavigationContainerFromInput(ConfigFlags)

    # This pre-loads libraries required to read the run 2 trigger navigation
    from TrigEDMConfig.TriggerEDM import EDMLibraries
    nav = CompFactory.HLT.Navigation('Navigation')
    nav.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]
    tdt.Navigation = nav

    acc.addPublicTool(nav)
    acc.addPublicTool(tdt, primary=True)

    msg.info("Configuring the TrigDecisionTool and xAODConfigSvc to use ConfigSource:{}, Run3NavigationFormat:{}, Run3NavigationSummaryCollection:{}".format(
        "InFileMetadata" if ConfigFlags.Trigger.InputContainsConfigMetadata else "ConditionsAndDetStore",
        str(use_run3_format),
        tdt.HLTSummary)
    )

    return acc
