# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from ..Base.L1MenuFlags import L1MenuFlags
from ..Base.MenuConfObj import TopoMenuDef


def defineInputsMenu():
    
    ctpinBoards = odict() # Ctpin/Slot9 (CTPCAL, NIM1, NIM2)
    topoBoards = odict()  # Topo1, Topo2, Topo3
    muctpiBoard = odict() # MuCTPi
    alfaBoard = odict() # ALFA


    #-----------------------------------
    # SLOT 9 / CON 1 (CTPCal, NIM1,NIM2)
    # https://twiki.cern.ch/twiki/bin/view/Atlas/LevelOneCentralTriggerSetup#CTPIN_Slot_9
    #-----------------------------------
    ctpinBoards["Ctpin9"] = odict()
    ctpinBoards["Ctpin9"]["connectors"] = []
    ctpinBoards["Ctpin9"]["connectors"] += [
        {
            "name" : "CTPCAL",
            "format" : "multiplicity",
            "nbitsDefault" : 1,
            "type" : "ctpin",
            "legacy" : False,
            "thresholds" : [
                'BCM_AtoC', 'BCM_CtoA', 'BCM_Wide', # 3 x 1-bit BCM
                ('BCM_Comb',3), # 1x 3-bit BCM
                (None,11),
                'BPTX0','BPTX1', # 2x BPTX
                'LUCID_A', 'LUCID_C', # 2x LUCID 
                (None,4),
                'ZDC_A', 'ZDC_C', 'ZDC_AND', # 3x ZDC
                'CAL0','CAL1','CAL2', # 3 x CALREQ
            ]
        },
        {
            "name" : "NIM1",
            "format" : "multiplicity",
            "nbitsDefault" : 1,
            "type" : "ctpin",
            "legacy" : False,
            "thresholds" : [
                'MBTS_A0', 'MBTS_A1', 'MBTS_A2', 'MBTS_A3', 'MBTS_A4'  , 'MBTS_A5', 'MBTS_A6', 'MBTS_A7', 'MBTS_A8', 'MBTS_A10', 'MBTS_A12', 'MBTS_A14',
                'MBTS_A9', 'MBTS_A11', 'MBTS_A13', 'MBTS_A15', # 16x MBTSSI 
                ('MBTS_A',3),         # 1x MBTS_A
                'NIML1A',             # L1A for CTP monitoring itself
                'NIMLHCF',            # LHCF
                'AFP_NSA', 'AFP_FSA', 'AFP_FSA_TOF_T0', 'AFP_FSA_TOF_T1', 'AFP_FSA_TOF_T2', 'AFP_FSA_TOF_T3'   # 2xAFP
            ]
        },
        {
            "name" : "NIM2",
            "format" : "multiplicity",
            "nbitsDefault" : 1,
            "type" : "ctpin",
            "legacy" : False,
            "thresholds" : [
                'MBTS_C0', 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 'MBTS_C4', 'MBTS_C5', 'MBTS_C6', 'MBTS_C7', 'MBTS_C8', 'MBTS_C10', 'MBTS_C12', 'MBTS_C14', 
                'MBTS_C11', 'MBTS_C13', 'MBTS_C9', 'MBTS_C15', # 16x MBTSSI 
                ('MBTS_C',3), # 1x MBTS_C
                'NIMTGC',     # TGC
                'NIMRPC',     # RPC
                'NIMTRT',     # TRT
                'AFP_NSC', 'AFP_FSC', 'AFP_FSC_TOF_T0', 'AFP_FSC_TOF_T1', 'AFP_FSC_TOF_T2', 'AFP_FSC_TOF_T3'   # 2xAFP
            ]
        }
    ]


    #
    # new topo board for multiplicities
    #
    topoBoards["Topo1"] = odict([("connectors",[])])
    topoBoards["Topo1"]["connectors"].append({ # first optical connector
        "name" : "Topo1Opt0",
        "format" : "multiplicity",
        "nbitsDefault" : 2,
        "type" : "optical",
        "legacy" : False,
        "thresholds" : [  # Topo1A: eFex EM, eFex TAU, gJ, gLJ 
            ('eEM3',3), ('eEM5',3), ('eEM8',3), ('eEM8L',3), 
            'eEM8M', 'eEM10L', 'eEM15', 'eEM15L', 'eEM15M',
            'eEM20', 'eEM20L', 'eEM20M', 
            'eEM22', 'eEM22L', 'eEM22M', 'eEM22T',
        ]
    })

    topoBoards["Topo1"]["connectors"].append({ # second optical connector
        "name" : "Topo1Opt1",
        "format" : "multiplicity",
        "nbitsDefault" : 2,
        "type" : "optical",
        "fpga" : 0,
        "legacy" : False,
        "thresholds" : [ # Topo1A: eFex EM, eFex TAU, gJ, gLJ
             ('eTAU8',3), ('eTAU12',3), 'eTAU12L', 'eTAU12M', 'eTAU20',
             'eTAU25', 'eTAU30HM', 'eTAU40', 'eTAU60', 'eTAU100',
        ]
    })

    topoBoards["Topo1"]["connectors"].append({ # third optical connector
        "name" : "Topo1Opt2",
        "format" : "multiplicity",
        "nbitsDefault" : 2,
        "type" : "optical",
        "fpga" : 1,
        "legacy" : False,
        "thresholds" : [ # Topo1B: jFex small-R jet, jFex large-R jet, combined eFex/jFex TAU, gFex+jFex EX, gFex+jFex SumET, jFex TAU
            'jLJ80', 'jLJ100', 'jLJ140', 'jLJ160', 

            'jEM15', 'jEM15M', 'jEM18M', 

            ('gXERHO30',1),  ('gXERHO50',1),
            ('gXEPUFIT30',1),  ('gXEPUFIT50',1),  
            ('gXE30',1), ('gXE40',1), ('gXE50',1),
            ('gTE50',1),
        ]
    })

    topoBoards["Topo1"]["connectors"].append({ # fourth optical connector
        "name" : "Topo1Opt3",
        "format" : "multiplicity",
        "nbitsDefault" : 2,
        "type" : "optical",
        "fpga" : 1,
        "legacy" : False,
        "thresholds" : [ # Topo1B: jFex small-R jet, jFex large-R jet, combined eFex/jFex TAU, gFex+jFex EX, gFex+jFex SumET, jFex TAU
            ('jTAU12',3), ('cTAU12M',3), 'jTAU12M', 'cTAU20M', 'cTAU25M', 

            ('jJ12',3),  ('jJ12p0ETA25',3), ('jJ15',3),  ('jJ15p0ETA25',3), 'jJ15p31ETA49', 
            ('jJ20',3),  'jJ20p31ETA49', ('jJ25',3),  ('jJ25p0ETA23',3),  ('jJ30',3), 'jJ30p31ETA49',
            'jJ35p0ETA23', 'jJ40', 'jJ40p0ETA25', 'jJ45p0ETA20', 'jJ50', 'jJ50p31ETA49', 'jJ75', 'jJ75p31ETA49',
            'jJ85', 'jJ100', 'jJ120', 'jJ400',

            ('jXE30',1), ('jXE35',1), ('jXE40',1), ('jXE50',1), ('jXE55',1), ('jXE300',1),
            # test thresholds
            ('jXEC50',1), ('jXEPerf50',1),
            ('jTE100',1), ('jTEC100',1), ('jTEFWD100',1), ('jTEFWDA100',1), ('jTEFWDC100',1)
        ]
    })

    topoBoards["Topo2"] = odict()
    topoBoards["Topo2"]["connectors"] = []
    topoBoards["Topo2"]["connectors"].append({
        "name" : "Topo2El",
        "format" : "topological",
        "type" : "electrical",
        "legacy" : False,
        "algorithmGroups" : [
            {
                "fpga" : 0,
                "clock" : 0,
                "algorithms" : [
                    TopoMenuDef( '8INVM15-20DR99-C-2MU4ab',                    outputbits = 0 ), #BLS, test
                    TopoMenuDef( '0INVM16-20DR99-2MU4ab',                      outputbits = 1 ), #BLS, test
                    TopoMenuDef( '0INVM16-15DR99-2MU4ab',                      outputbits = 2 ), #BLS, test
                    TopoMenuDef( '8INVM15-20DR99-2MU4ab',                      outputbits = 3 ), #BLS, test
                    TopoMenuDef( '8INVM15-15DR99-2MU4ab',                      outputbits = 4 ), #BLS, test
                    TopoMenuDef( '0INVM10-3MU4ab',                             outputbits = 5 ), #BLS
                    TopoMenuDef( 'LATE-MU10s1',                                outputbits = 6 ),
                    TopoMenuDef( 'MULT-CMU4ab',                                outputbits = (7,8), outputlines = ['MULT-CMU4ab[0]', 'MULT-CMU4ab[1]'] ), # BLS, 2bits  # TODO: needed?
                    TopoMenuDef( 'MULT-CMU6ab',                                outputbits = (9,10), outputlines = ['MULT-CMU6ab[0]', 'MULT-CMU6ab[1]'] ), # BLS, 2bits # TODO: needed?
                    TopoMenuDef( '0DR04-MU4ab-CjJ15ab',                        outputbits = 11 ), #Bjet, TODO: not a primary
                    TopoMenuDef( '0DR04-MU6ab-CjJ20ab',                        outputbits = 12 ), #Bjet, TODO: not a primary
                    TopoMenuDef( '0INVM10C-3MU4ab',                            outputbits = 13 ), #BLS
                    TopoMenuDef( '2INVM9-0DR15-C-MU6ab-MU4ab',                 outputbits = 14 ), #BLS 
                ]
            },            

            {
                "fpga" : 0,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( '2DISAMB-jJ25ab-0DR25-eTAU20abi-eTAU12abi',   outputbits = 0 ),
                    TopoMenuDef( '2DISAMB-jJ25ab-0DR28-eTAU20abi-eTAU12abi',   outputbits = 1 ),
                    TopoMenuDef( '0DR25-eTAU20abi-eTAU12abi',                  outputbits = 2 ),
                    TopoMenuDef( '0DR28-eTAU20abi-eTAU12abi',                  outputbits = 3 ), 
                ]
            },
            
            {
                "fpga" : 1,
                "clock" : 0,
                "algorithms" : [
                    TopoMenuDef( 'KF-jXE-AjJall',                  outputbits = (0,5), outputlines = [ 'KF-jXE40-AjJall',
                                                                                                       'KF-jXE50-AjJall',
                                                                                                       'KF-jXE55-AjJall',
                                                                                                       'KF-jXE60-AjJall',
                                                                                                       'KF-jXE65-AjJall',
                                                                                                       'KF-jXE75-AjJall'] ),
                    TopoMenuDef( 'ZAFB_DPHI',                      outputbits = (6,7), outputlines = [ '60INVM-04DPHI32-eEM15abm-FjJj15s623ETA49',
                                                                                                       '60INVM-25DPHI32-eEM15abm-FjJj15s623ETA49'] ),
                    TopoMenuDef( 'CEP_CjJ',                         outputbits = (8,9), outputlines = [ 'CEP-CjJ50s6',
                                                                                                       'CEP-CjJ60s6'] ),
                    TopoMenuDef( 'INVM_eEMs6',                     outputbits = (10,11), outputlines = [ '1INVM5-eEM7s1-eEMs6',
                                                                                                         '1INVM5-eEM12s1-eEMs6'] ),
                    TopoMenuDef( '27DPHI32-eEMs1-eEMs6',           outputbits = 12 ),
                    TopoMenuDef( '0INVM70-27DPHI32-eEM12s1-eEM12s6',           outputbits = 13 ),
                ]
            },
            
            {
                "fpga" : 1,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( 'jINVM_DPHI_NFF',                             outputbits = (0,3), outputlines = ['400INVM-0DPHI26-jJ30s6-AjJ20s6',
                                                                                                                  '400INVM-0DPHI24-jJ30s6-AjJ20s6',
                                                                                                                  '400INVM-0DPHI22-jJ30s6-AjJ20s6',
                                                                                                                  '400INVM-0DPHI20-jJ30s6-AjJ20s6'] ),
                    TopoMenuDef( 'jINVM',                                       outputbits = (4,7), outputlines = ['300INVM-AjJ30s6-AjJ20s6',
                                                                                                                  '400INVM-AjJ30s6-AjJ20s6',
                                                                                                                  '500INVM-AjJ30s6-AjJ20s6',
                                                                                                                  '700INVM-AjJ30s6-AjJ20s6'] ),
                    TopoMenuDef( 'jINVM_DPHI',                                  outputbits = (8,11), outputlines = ['400INVM-0DPHI26-AjJ30s6-AjJ20s6',
                                                                                                                   '400INVM-0DPHI24-AjJ30s6-AjJ20s6',
                                                                                                                   '400INVM-0DPHI22-AjJ30s6-AjJ20s6',
                                                                                                                   '400INVM-0DPHI20-AjJ30s6-AjJ20s6'] ),
                ]
            }
        ]
    })

    topoBoards["Topo3"] = odict()
    topoBoards["Topo3"]["connectors"] = []
    topoBoards["Topo3"]["connectors"].append({
        "name" : "Topo3El",
        "format" : "topological",
        "type" : "electrical",
        "legacy" : False,
        "algorithmGroups" : [
            {
                "fpga" : 0,
                "clock" : 0,
                "algorithms" : [
                    TopoMenuDef( 'HT190-jJ15s5pETA21',                       outputbits = 0 ),
                    TopoMenuDef( '0DR03-eEM7ab-CjJ15ab',                     outputbits = 1 ),
                    TopoMenuDef( 'jINVM_NFF',                                outputbits = (2,5), outputlines = ['300INVM-jJ30s6-AjJ20s6',
                                                                                                                '400INVM-jJ30s6-AjJ20s6',
                                                                                                                '500INVM-jJ30s6-AjJ20s6',
                                                                                                                '700INVM-jJ30s6-AjJ20s6',] ), # TODO: needed? 
                    TopoMenuDef( 'HT150-jJ20s5pETA31',                       outputbits = 6 ),
                    TopoMenuDef( '400INVM-AjJ30s6pETA31-AjJ20s6p31ETA49',    outputbits = 7 ),
                    TopoMenuDef( 'SC111-CjJ15abpETA26',                      outputbits = 8 ),
                    TopoMenuDef( '0DETA20-jJ50s1-jJs2',                      outputbits = 9 ),
                    TopoMenuDef( '0INVM9-eEM7ab-eEMab',                      outputbits = 10 ),
                ]
            },

            {
                "fpga" : 0,
                "clock" : 1,
                "algorithms" : [
                    # EMPTY SO FAR...
                ]
            },

            {
                "fpga" : 1,
                "clock" : 0,
                "algorithms" : [
                    TopoMenuDef( '100RATIO-0MATCH-eTAU30si2-eEMall',     outputbits = 0 ),
                    TopoMenuDef( 'NOT-0MATCH-eTAU30si1-eEMall',          outputbits = 1 ), 
                    TopoMenuDef( '0INVM70-27DPHI32-eEM10sm1-eEM10sm6',   outputbits = 2 ),
                    TopoMenuDef( '0INVM70-27DPHI32-eEM12sm1-eEM12sm6',   outputbits = 3 ),
                    TopoMenuDef( 'ZEE-eEM20sm2',                         outputbits = 4 ), 
                    TopoMenuDef( '10DR99-2MU6ab',                        outputbits = 5 ), #Msonly Narrow scan triggers
                    TopoMenuDef( '0DR12C-2MU4ab',                        outputbits = 6 ), #BLS
                ]
            },

            {
                "fpga" : 1,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( '8INVM15-0DR22-MU6ab-MU4ab',            outputbits = 0 ),  #BLS
                    TopoMenuDef( '2INVM9-0DR15-MU6ab-MU4ab',             outputbits = 1 ),  #BLS 
                    TopoMenuDef( '2INVM9-0DR15-2MU4ab',                  outputbits = 2 ),  #BLS 
                    TopoMenuDef( '2INVM9-2DR15-2MU6ab',                  outputbits = 3 ),  #BLS
                    TopoMenuDef( '5DETA99-5DPHI99-2MU4ab',               outputbits = 4 ),  #Low-mass DY
                    TopoMenuDef( '5DETA99-5DPHI99-MU6ab-MU4ab',          outputbits = 5 ),  #Low-mass DY
                    TopoMenuDef( '5DETA99-5DPHI99-2MU6ab',               outputbits = 6 ),  #Low-mass DY
                    TopoMenuDef( '0INVM10-0DR15-eEM8abl-MU10ab',         outputbits = 7 ), #LFV
                    TopoMenuDef( '0INVM10-0DR15-eEM12abl-MU6ab',         outputbits = 8 ), #LFV
                    TopoMenuDef( '8INVM15-2CMU4ab',                      outputbits = 9 ), #BLS, TODO: needed?
                    TopoMenuDef( '0DR15-2MU6ab',                         outputbits = 10 ), #LFV
                    TopoMenuDef( '8INVM15-0DR22-2MU6ab',                 outputbits = 11 ), #BLS
                ]
            }
        ]
    })

    muctpiBoard["MuCTPi"] = odict() 
    muctpiBoard["MuCTPi"]["connectors"] = []
    muctpiBoard["MuCTPi"]["connectors"].append({
        "name" : "MuCTPiOpt0",
        "format" : "multiplicity",
        "nbitsDefault" : 2,
        "type" : "optical",
        "legacy" : False,
        "thresholds" : [
            # exactly the first 4 thresholds must be defined with 3 bits, all others use 2 bits automatically
            ('MU3V',3), ('MU5VF',3), ('MU8F',3), ('MU8VF',3), 'MU14FCH', 'MU14FCHR',
            # Run3 test/backup
            'MU3VF', 'MU8FC', 'MU15VFCH', 'MU10BOM', 'MU20FC',
        ]

    })

    muctpiBoard["MuCTPi"]["connectors"].append({
        "name" : "MuCTPiEl",
        "format" : "topological",
        "type" : "electrical",
        "legacy" : False,
        "algorithmGroups" : [
            {
                "fpga" : 0,
                "clock" : 0,
                "algorithms" : []
            },
            {
                "fpga" : 0,
                "clock" : 1,
                "algorithms" : [
                    TopoMenuDef( "MUCTP-0DR15-2MU6ab",            outputbits = 0 ),
                ]
            },
            {
                "fpga" : 1,
                "clock" : 0,
                "algorithms" : []
            },
            {
                "fpga" : 1,
                "clock" : 1,
                "algorithms" : []
            }
        ]
    })


    alfaBoard["AlfaCtpin"] = odict()
    alfaBoard["AlfaCtpin"]["connectors"] = []
    alfaBoard["AlfaCtpin"]["connectors"].append({
        "name" : "AlfaCtpin",
        "format" : "simple",
        "nbitsDefault" : 1,
        "type" : "electrical",
        "legacy" : False,
        "signalGroups" : [
            {
                "clock" : 0,
                "signals" : [
                    (None,2), "ALFA_B7R1L", "ALFA_A7R1L", "ALFA_A7L1L", "ALFA_B7L1L",
                    (None,2), "ALFA2_B7R1L", "ALFA2_A7R1L", "ALFA2_A7L1L", "ALFA2_B7L1L",
                    (None,2), "ALFA3_B7R1L", "ALFA3_A7R1L", "ALFA3_A7L1L", "ALFA3_B7L1L",
                    (None,2), "ALFA4_B7R1L", "ALFA4_A7R1L", "ALFA4_A7L1L", "ALFA4_B7L1L",
                    (None,2), "ALFA_B7R1L_OD", "ALFA_A7R1L_OD", "ALFA_A7L1L_OD", "ALFA_B7L1L_OD"
                ]
            },
            {
                "clock" : 1,
                "signals" : [
                    (None,2), "ALFA_B7R1U", "ALFA_A7R1U", "ALFA_A7L1U", "ALFA_B7L1U",
                    (None,2), "ALFA2_B7R1U", "ALFA2_A7R1U", "ALFA2_A7L1U", "ALFA2_B7L1U",
                    (None,2), "ALFA3_B7R1U", "ALFA3_A7R1U", "ALFA3_A7L1U", "ALFA3_B7L1U",
                    (None,2), "ALFA4_B7R1U", "ALFA4_A7R1U", "ALFA4_A7L1U", "ALFA4_B7L1U",
                    (None,2), "ALFA_B7R1U_OD", "ALFA_A7R1U_OD", "ALFA_A7L1U_OD", "ALFA_B7L1U_OD"
                ]
            }
        ]
    })


    L1MenuFlags.boards().clear()

    L1MenuFlags.boards().update( topoBoards )   # Topo1, Topo2, Topo3

    L1MenuFlags.boards().update( muctpiBoard )  # MuCTPi

    L1MenuFlags.boards().update( ctpinBoards )  # CTPIN/Slot9 NIM1, NIM2, CALREQ

    L1MenuFlags.boards().update( alfaBoard )  # ALFA

    #----------------------------------------------

    def remapThresholds():
        # remap thresholds. TODO: add checks in case the remap does not fulfill HW constraints?
        for boardName, boardDef in L1MenuFlags.boards().items():
            if "connectors" in boardDef:
                for c in boardDef["connectors"]:
                    if "thresholds" in c:
                        thresholdsToRemove = []
                        for thrIndex, thrName in enumerate(c["thresholds"]):
                            nBits = 0
                            if type(thrName)==tuple:
                                (thrName,nBits) = thrName
                            if thrName in L1MenuFlags.ThresholdMap():
                                if (L1MenuFlags.ThresholdMap()[thrName] != ''):
                                    if nBits > 0:
                                        c["thresholds"][thrIndex] = (L1MenuFlags.ThresholdMap()[thrName],nBits)
                                    else:
                                        c["thresholds"][thrIndex] = L1MenuFlags.ThresholdMap()[thrName]
                                else:
                                    thresholdsToRemove.append(thrIndex) 
                        for i in reversed(thresholdsToRemove):
                            del c["thresholds"][i]
          
    #----------------------------------------------

    remapThresholds()
