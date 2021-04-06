# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""Make chain dicts for testing jet hypo config modules"""

from __future__ import print_function

from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import (
    SingleJetGroup,
    MultiJetGroup)

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp
from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName

from TrigHLTJetHypo.hypoToolDisplay import hypoToolDisplay

import sys

from pprint import pprint

# from TrigHLTJetHypo.xmlConfig import hypotool_from_chaindict
from TrigHLTJetHypo.hypoConfigBuilder import hypotool_from_chaindict

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG
logger = logging.getLogger( __name__)

logger.setLevel(DEBUG)

chains = [
    ChainProp(name='HLT_j0_dijetSEP80j12etSEP700djmassSEPdjdphi260_L1J20.log',
              l1SeedThresholds=['FSNOSEED'],groups=MultiJetGroup),

    ChainProp(name='HLT_j0_dijetSEP70j12etSEP1000djmassSEPdjdphi200SEP400djdeta_L1J20',
              l1SeedThresholds=['FSNOSEED'],groups=MultiJetGroup),
    
    ChainProp(name='HLT_j85_050momemfrac100_L1J20', groups=SingleJetGroup),

    ChainProp(name='HLT_j420_subresjesgscIS_ftf_L1J100',groups=SingleJetGroup),

    ChainProp(name='HLT_j260_320eta490_L1J75_31ETA49',
              groups=SingleJetGroup),

    ChainProp(name='HLT_j80_j60_L1J15',
              l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),

    ChainProp(name='HLT_2j80_3j60_L1J15',
              l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),

    ChainProp(name='HLT_j0_aggSEP1000ht_L1J20', groups=SingleJetGroup),


     ChainProp(name='HLT_j70_0eta320_j50_0eta490_j0_dijetSEP70j12etSEP1000djmassSEPdjdphi200SEP400djdeta__L1MJJ-500-NFF',

               l1SeedThresholds=['FSNOSEED']*3,
               groups=MultiJetGroup),

    ChainProp(name='HLT_10j40_L1J15',
              l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),

    ChainProp(name='HLT_j0_fbdjshared_L1J20', groups=SingleJetGroup),
        
    ChainProp(name='HLT_j40_j0_aggSEP50htSEP10etSEP0eta320_L1J20',
              l1SeedThresholds=['FSNOSEED']*2,
              groups=MultiJetGroup),

    ChainProp(name='HLT_j0_fbdjnosharedSEP10etSEP20etSEP34massSEP50fbet_L1J20',
              groups=SingleJetGroup),

    ChainProp(name='HLT_j85_ftf_maskSEP300ceta210SEP300nphi10_L1J20',
                  groups=SingleJetGroup),

    ChainProp(name='HLT_j45_pf_ftf_preselj20_L1J15', groups=SingleJetGroup),
    
    ChainProp(name='HLT_j85_ftf_maskSEP300ceta210SEP300nphi10_L1J20',
              groups=SingleJetGroup),
        
    ChainProp(name='HLT_j0_dijetSEP80j12etSEP0j12eta240SEP700djmass_L1J20', groups=SingleJetGroup),
    
]

def testChainDictMaker(idict):

    if idict >= 0:
        chain_props = [chains[idict]]
    else:
        chain_props = chains

    result = []
    for cp in chain_props:
        logger.debug(cp)
        chain_dict = dictFromChainName(cp)
        result.append((cp.name, chain_dict))

    return result


def list_chains():
    for i, c in enumerate(chains):
        logger.debug('%2d ' + c.name, i)


if __name__ == '__main__':
    import argparse
    import os

    parser = argparse.ArgumentParser()

    parser.add_argument(
        '-i',
        '--iprop',
        help='index of chainProp to run, -1: do all',
        type=int)

    parser.add_argument('-d', '--dot', help='write out a dot file',
                        action='store_true')

    parser.add_argument(
        '--dotdir',
        help='specify directory to which dot file is to be written [$HOME]',
        default=os.environ['HOME'])

   
    args = parser.parse_args()

    if args.iprop is None:
        list_chains()
        sys.exit()

    iprop = args.iprop
    dicts = testChainDictMaker(iprop)
    for d in dicts:
        pprint(d)

    do_dot = args.dot
    if args.dotdir:
        do_dot = True
        
    for cn, d in dicts:
        hypo_tool = hypotool_from_chaindict(d)
        hypoToolDisplay(hypo_tool,
                        do_dot=do_dot,
                        dotdir=args.dotdir)

        
