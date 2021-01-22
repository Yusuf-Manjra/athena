# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Run this file in order to print out the empty slots

def print_available():
    from TriggerMenu.l1.Lvl1Flags import Lvl1Flags
    defineMenu()
    available = []
    for i in range(512):
      if i==463: continue #reserved for L1_RD2_BGRP14, L1_RD3_BGRP15 now assigned to 510 for partition 3 ATR-17737
      if i>=509 and i<=511: continue #reserved for CALREQ
      if not i in Lvl1Flags.CtpIdMap().values(): available.append(i)
    available.sort()
    print "There are %d available CTP IDs:"%len(available),",".join(map(str,available))
    print "IDs >= 472 go in partition 2, IDs >= 492 go in partition 3"

def defineMenu():
    """
    Defines the following LvlFlags:
    
    thresholds   .... list of all threshold names in the menu (includes forced thresholds)
    items        .... list of all L1 item names in the menu
    CtpIdMap     .... map of item names to CTP IDs
    """
    
    from TriggerMenu.l1.Lvl1Flags import Lvl1Flags

    Lvl1Flags.CTPVersion = 4 # new CTP

    Lvl1Flags.BunchGroupPartitioning = [1, 15, 15] # partition 1: 1-10, partition 2: empty (was 14), partition 3: 15 (note that BGRP0 is used by all items)
    Lvl1Flags.BunchGroupNames = ['BCRVeto', 'Paired', 'CalReq', 'Empty', 'IsolatedUnpaired', 'NonIsolatedUnpaired', 'EmptyAfterPaired', 'InTrain', 'AbortGapNotCalReq', 'VdM', 'ALFA', 'EmptyBeforePaired','EmptyAndPaired']
    Lvl1Flags.BunchGroupNames += ['NotUsed'] * len(Lvl1Flags.BunchGroupNames())

    Lvl1Flags.MenuPartitioning = [0, 472, 492] # partition 1: ctpid 0-471, partition 2: ctpid 472-491, partition 3: ctpid 492-511

    Lvl1Flags.RemapThresholdsAsListed = True

    Lvl1Flags.thresholds = [
        #------------------------------------
        #------------------------------------
        #      !!!!!!!IMPORTANT!!!!!!!!!
        # Please make sure to use the right
        # order to get the mapping right!!!
        #------------------------------------
        #------------------------------------

        # Note that the forced thresholds (FTHR) are not used in the menu but are needed for input monitoring
        # They can always be replaced with something that is needed for the menu


        #-------------------------
        # SLOT 7 / CON 0,1 (EM1,2)
        #-------------------------
        
        # 16 x EM
        'EM3', 'EM7', 'EM8VH',
        'EM10VH', 'EM12', 'EM15', 'EM15VH',
        'EM15VHI',
        'EM18VHI', 'EM20VH', 'EM20VHI', 'EM22VH', 'EM22VHI', 'EM24VHI', 
        # ATR-19437
        # 4 x new EM1
        'eEM7', 'eEM8VH', 'eEM10VH', 'eEM15VH', 
        # 6 x new EM1
        'eEM15VHI', 'eEM18VHI', 'eEM20VH', 'eEM20VHI', 'eEM22', 'eEM22VH',  'eEM22VHI',  'eEM24VHI',  'eEM24VHIM', 
        # 5 x new EM1
        'eEM3', 'eEM8', 'eEM10', 'eEM12', 'eEM15', 'eEM20',
        # 1 x ZB/
        'ZB_EM15',

        #--------------------------
        # SLOT 7 / CON 2,3 (TAU1,2)
        #--------------------------

        # 16 x TAU
        'HA8', 'HA12', 'HA12IM', 'HA20', 'HA20IM', 'HA25IM', 'HA40', 'HA60', 'HA100',
        # ATR-19437
        # 3 x new TAU1
        'eTAU12', 'eTAU12IL', 'eTAU20', 
        # 5 x new TAU2
        'eTAU20IT', 'eTAU25IT', 'eTAU40', 'eTAU60', 'eTAU100', 

        #----------------------
        # SLOT 8 / CON 0 (JET1)
        #----------------------
        # 10 x 3-bit JET (can have multiplicity 4 or more)

        # 8 x JETs and central jets
        'J12', 'J12.0ETA25', 'J15', 'J15.0ETA25','J20', 'J25','J25.0ETA23','J30',

        #----------------------
        # SLOT 8 / CON 1 (JET2)
        #----------------------
        # 15 x 2-bit JET (can have maximum multiplicity of 3) (SLOT 8, CON 1)

        # 3 x Central Jet
        'J35.0ETA23','J40.0ETA25', #'J20.28ETA31',

        # 6 Jets # replace J400 with J45.0ETA20 - ATR-19309
        'J40', 'J50', 'J75', 'J85', 'J100', 'J45.0ETA20', 'J120', 'J400',
        # 'J40', 'J50', 'J75', 'J85', 'J100', 'J120', 'J400',


        # 6 x FJ              
        'J15.31ETA49', 'J20.31ETA49', 'J30.31ETA49', 'J50.31ETA49', 'J75.31ETA49', #'JJ15.23ETA49', 'J100.31ETA49', # ATR-19437
        # 8 x new JET1
        'jJ12', 'jJ12.0ETA25', 'jJ15', 'jJ15.0ETA25', 'jJ20', 'jJ25', 'jJ25.0ETA23', 'jJ30', 
        # 9 x new JET2
        'jJ35.0ETA23', 'jJ40.0ETA25', 
        'jJ40', 'jJ50', 'jJ85', 'jJ100',
        'jJ15.31ETA49', 'jJ20.31ETA49', 'jJ75.31ETA49', 

        #large R jets from gFEX
        'gLJ100', 'gLJ120', 'gLJ140', 'gLJ160',

        'jLJ100', 'jLJ120', 'jLJ140', 'jLJ160',

        #---------------------
        # SLOT 8 / CON 2 (EN1)
        #---------------------
        # 24 x 1-bit thresholds
        
        # 8 x TE
        #'TE3', 'TE5', 'TE10', 'TE15', 'TE20', 'TE25', 'TE30', 'TE40',
        #'TE5', 'TE30', 'TE40', 'TE45', 'TE50', 'TE55', 'TE60', 'TE65',
        #'TE5', 'TE10', 'TE20', 'TE30', 'TE40', 'TE50', 'TE60', 'TE70',
        'TE5', 'TE10', 'TE20', 'TE50',
        #'TE2', 'TE3', 'TE4', 'TE5', 'TE10', 'TE20', 'TE30', 'TE70',

        # 8 x XE (for standard XE)
        #'XE35', 'XE40', 'XE45', 'XE50', 'XE55', 'XE60', 'XE70', 'XE80',
        # 8 x XE (for restricted ETA XE: check ATR-13234 for any further change)
        #'XE10', 'XE20', 'XE25', 'XE30', 'XE65', 'XE75', 'XE150', 'XE300',

        'XE30', 'XE35', 'XE40', 'XE45', 'XE50',
        'XE55', 'XE60', 'XE300',
        # ATR-19437
        # 8 x new XE
        'gXERHO20', 'gXERHO30', 'gXERHO35', 'gXERHO40',   'gXERHO45', 'gXERHO50', 'gXEPUFIT20',  'gXEPUFIT50', 
        'gXE50',
        'jXE50',
        'jXENC50','gXENC70','gXEPUFIT60','jXERHO80','gXEJWOJ110',
        
        # 8 x XS
        'XS20', 'XS30',


        #---------------------
        # SLOT 8 / CON 3 (EN2)
        #---------------------
        # 16 x 1-bit thresholds

        # 8 x weighted sum ET
#        'RXE35', 'RXE40', 'RXE45', 'RXE50', 'RXE55', 'RXE60', 'RXE70', 'RXE80', # FTHR

        # ATR-22426 - only three new restricted eta XE
        'XE40.0ETA24','XE50.0ETA24','XE60.0ETA24',

        # 8 x restricted eta range
        #'TE3.0ETA24', 'TE5.0ETA24','TE10.0ETA24', 'TE15.0ETA24', 'TE20.0ETA24', 'TE25.0ETA24', 'TE30.0ETA24', 'TE40.0ETA24',
        #'TE5.0ETA24', 'TE30.0ETA24','TE40.0ETA24', 'TE45.0ETA24', 'TE50.0ETA24', 'TE55.0ETA24', 'TE60.0ETA24', 'TE65.0ETA24',
        #'TE5.0ETA24', 'TE10.0ETA24', 'TE15.0ETA24', 'TE20.0ETA24', 'TE25.0ETA24', 'TE30.0ETA24', 'TE40.0ETA24', 'TE70.0ETA24',
        'TE5.24ETA49', 'TE10.24ETA49', 'TE15.24ETA49', 'TE20.24ETA49', 'TE25.24ETA49', 'TE30.24ETA49', 'TE40.24ETA49', 'TE70.24ETA49',

        #------------------------
        # SLOT 9 / CON 0 (MUCTPi)
        #------------------------
        
        # 6 x MU
        'MU4', 'MU6', 'MU10', 'MU11', 'MU20', 'MU21',


        #------------------------
        # SLOT 9 / CON 1 (CTPCal)
        #------------------------

        # 3 x 1-bit BCM
        'BCM_AtoC', 'BCM_CtoA', 'BCM_Wide',
        # 1 x 3-bit BCM
        'BCM_Comb',

        # 8 x DBM
        #'DBM0', 'DBM1', 'DBM2', 'DBM3', 'DBM4', 'DBM5', 'DBM6', 'DBM7', # FTHR

        # 2  x BPTX
       'BPTX0','BPTX1',

        # 6 x LUCID 
        'LUCID_A', 'LUCID_C',
        #'LUCID_Coinc_AC', # FTHR
        #'LUCID_COMM',
        #'LUCID_05', 'LUCID_06', # FHTR

        # 3 x ZDC
        'ZDC_A', 'ZDC_C',
        'ZDC_AND', # FTHR

        # 3 x CALREQ
        'CAL0','CAL1','CAL2',

        #-----------------------------
        # SLOT 9 / CON 2,3 (NIM1,NIM2)
        #-----------------------------

        # 2 x MBTS
        'MBTS_A', 'MBTS_C',

        # 32 x MBTSSI (all FTHR)
        # NOTE: THESE ARE OUT OF ORDER FOR A REASON! Do not change!
        # The order defines the mapping - see ATR-17870.
        'MBTS_A0', 'MBTS_A1', 'MBTS_A2', 'MBTS_A3', 'MBTS_A4', 'MBTS_A5', 'MBTS_A6', 'MBTS_A7',
        'MBTS_A8', 'MBTS_A10', 'MBTS_A12', 'MBTS_A14', 'MBTS_A9', 'MBTS_A11','MBTS_A13', 'MBTS_A15',
        'MBTS_C0', 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 'MBTS_C4', 'MBTS_C5', 'MBTS_C6', 'MBTS_C7',
        'MBTS_C8', 'MBTS_C10', 'MBTS_C12', 'MBTS_C14', 'MBTS_C11', 'MBTS_C13','MBTS_C9','MBTS_C15',

        # L1A for CTP monitoring itself
        'NIML1A',

        # LHCF
        'NIMLHCF',

        # TGC
        'NIMTGC',
        
        # RPC
        'NIMRPC',
        
        # TRT
        'NIMTRT',

        # AFP
        'AFP_NSC', 'AFP_NSA',
        'AFP_FSA_SIT', 'AFP_FSA_TOF', 'AFP_FSA_LOG',
        'AFP_FSC_SIT', 'AFP_FSC_LOG', 'AFP_FSC_TOF',

        #-------------------------------------------------------------------
        #L1 TOPO inputs 
        #-------------------------------------------------------------------
        
        #HT
#        'HT190-AJ15all.ETA21',
        'HT190-J15s5.ETA21',
#        'HT150-AJ20all.ETA31',
        'HT150-J20s5.ETA31',

        

        #ZH 
        #'10MINDPHI-AJ20s2-XE50', 
        '10MINDPHI-J20s2-XE50', '10MINDPHI-J20s2-XE30', 
        #'10MINDPHI-J20ab-XE50', '10MINDPHI-CJ20ab-XE50', 
        #Jpsi T&P
        #'1INVM5-EMs1-EMs6', 
        '1INVM5-EM7s1-EMs6', '1INVM5-EM12s1-EMs6',        

        #W T&P
        #'05MINDPHI-AJj10s6-XE0',
        #'10MINDPHI-AJj10s6-XE0',
        #'15MINDPHI-AJj10s6-XE0',
        #'05MINDPHI-EM12s6-XE0',
        #'15MINDPHI-EM12s6-XE0',
        #'05MINDPHI-EM15s6-XE0',
        #'15MINDPHI-EM15s6-XE0',
        #'05RATIO-XE0-HT0-AJj15all.ETA49',
        #'90RATIO2-XE0-HT0-AJj15all.ETA49',
        #'250RATIO2-XE0-HT0-AJj15all.ETA49',
        #'HT20-AJj15all.ETA49',
        #'NOT-02MATCH-EM10s1-AJj15all.ETA49',
        #'25MT-EM12s6-XE0',
        #'35MT-EM12s6-XE0',
        #'35MT-EM15s6-XE0',
        #'10MINDPHI-AJj15s2-XE0',
        #'20MINDPHI-AJjs6-XE0',
        #'20MINDPHI-AJj15s2-XE0',
        #'10MINDPHI-EM6s1-XE0',
        #'20MINDPHI-EM9s6-XE0',
        #'20MINDPHI-EM6s1-XE0',
        #'05RATIO-XE0-HT0-AJj15all.ETA49',
        #'08RATIO-XE0-HT0-AJj0all.ETA49',
        #'40RATIO2-XE0-HT0-AJj15all.ETA49',
        #'90RATIO2-XE0-HT0-AJj0all.ETA49',
        #'HT20-AJj0all.ETA49',
        #'NOT-02MATCH-EM9s1-AJj15all.ETA49',
        #'05RATIO-XE0-SUM0-EM9s1-HT0-AJj15all.ETA49',
        #'20MT-EM6s6-XE0',
        #'30MT-EM6s6-XE0',
        #'40MT-EM6s6-XE0',
        
        # central muon
        'MULT-CMU4ab',
        'MULT-CMU6ab',
        
        #B-jet
        #'0DR04-MU4ab-CJ15ab', '0DR04-MU4ab-CJ20ab', '0DR04-MU4ab-CJ30ab', '0DR04-MU6ab-CJ20ab', '0DR04-MU6ab-CJ25ab',
        #B-physics
        '0DR03-EM7ab-CJ15ab',
        #'2DR15-2MU6ab', 

        # L1 thresholds for L1Topo streamers
#SX        '2INVM999-CMU4ab-MU4ab',
#SX        '2INVM999-2CMU4ab',
#SX        '2INVM999-MU6ab-MU4ab',
#SX        '2INVM999-ONEBARREL-MU6ab-MU4ab',
#SX        '2INVM999-CMU6ab-CMU4ab',
#SX        '4INVM8-CMU4ab-MU4ab',
#SX        '4INVM8-2CMU4ab',
#SX        '4INVM8-MU6ab-MU4ab',
#SX        '4INVM8-ONEBARREL-MU6ab-MU4ab',
#SX        '4INVM8-CMU6ab-CMU4ab',        
        #'2DR99-2MU4ab',
        '5DETA99-5DPHI99-MU6ab-MU4ab',
        '5DETA99-5DPHI99-2MU6ab',
        #'0DR10-MU10ab-MU6ab',
        '0DR15-2MU6ab',
#        '0DETA04-0DPHI03-EM8abi-MU10ab',
        '0DETA04-EM8abi-MU10ab',
        '0DPHI03-EM8abi-MU10ab',
#        '0DETA04-0DPHI03-EM15abi-MUab',
        '0DETA04-EM15abi-MUab',
        '0DPHI03-EM15abi-MUab',
        #'900INVM9999-AJ30s6-AJ20s6',
        #'800INVM9999-AJ30s6-AJ20s6',
        '700INVM9999-AJ30s6-AJ20s6',
        #'500INVM9999-AJ30s6-AJ20s6',
        #'400INVM9999-AJ30s6-AJ20s6',
        #'350INVM9999-AJ30s6-AJ20s6',
        '300INVM9999-AJ30s6-AJ20s6',
        #'200INVM9999-AJ30s6-AJ20s6',
        #'100INVM9999-AJ30s6-AJ20s6',
        #'600INVM9999-J30s6-AJ20s6',
        '500INVM9999-J30s6-AJ20s6',
        #'400INVM9999-J30s6-AJ20s6',
        #'200INVM9999-J30s6-AJ20s6',
        #'63DETA127-FJ20s1-FJ20s2',
        '0DETA20-J50s1-Js2',
        '27DPHI32-EMs1-EMs6',
        #'350INVM9999-J30s6-J20s6',
        #'300INVM9999-J30s6-J20s6',
        #'250INVM9999-J30s6-J20s6',
        #'200INVM9999-J30s6-J20s6',
#        'HT150-AJj15all.ETA49',
        #'0MATCH-4AJ20.ETA31-4AJj15.ETA31',
        '100RATIO-0MATCH-TAU30si2-EMall',
        'NOT-0MATCH-TAU30si1-EMall',
        '2DISAMB-TAU12abi-J25ab',
        #'1DISAMB-EM15his2-TAU12abi-J25ab',
        #'DISAMB-0DR28-EM15his2-TAU12abi',
        '2DISAMB-J25ab-0DR28-EM15his2-TAU12abi',
        #'1DISAMB-TAU20abi-TAU12abi-J25ab',
        '0DR25-TAU20abi-TAU12abi',
        '0DR28-TAU20abi-TAU12abi',
        #'0DETA20-0DPHI20-TAU20abi-TAU12abi',
        #'1DISAMB-J25ab-0DR25-TAU20abi-TAU12abi',
        '2DISAMB-J25ab-0DR25-TAU20abi-TAU12abi',
        '2DISAMB-J25ab-0DR28-TAU20abi-TAU12abi',
        'DISAMB-30INVM-EM20his2-TAU12ab',
        '400INVM9999-AJ30s6.ETA31-AJ20s6.31ETA49',
#        'LAR-EM20shi1',
#        'LAR-J100s1',
#        'ZEE-EM20shi2',
#        'FTK-EM20s1',
#        'FTK-J100s1',
#        'FTK-MU10s1',

        #'2INVM9-2MU6ab',
        #'7INVM15-2MU4ab', 
        #'2INVM8-ONEBARREL-MU6ab-MU4ab', 
        #'0DR24-2CMU4ab', 
        #'0DR22-2MU6ab',
        #'0DR34-2MU4ab', 
        #'0DR24-2MU4ab', 
        #'0DR24-CMU4ab-MU4ab',  
        #'2INVM8-CMU4ab-MU4ab',
        #'0DR15-2MU4ab',  
        #'0DR15-MU6ab-MU4ab',  
        #'0DR22-MU6ab-MU4ab', 
        #'8INVM15-MU6ab-MU4ab',  
        #'8INVM15-2MU6ab', 
        '0INVM9-EM7ab-EMab',
        #'2INVM8-2MU4ab',  # ATR-15197 '2INVM9-2MU4ab',
        #'2INVM8-MU6ab-MU4ab', # ATR-15197 '2INVM9-MU6ab-MU4ab',
        #'2INVM9-2MU4ab',
        #'2INVM9-MU6ab-MU4ab',
#        'KF-XE40-AJall',
#        'KF-XE50-AJall',
#        'KF-XE55-AJall',
#        'KF-XE60-AJall',
#        'KF-XE65-AJall',
#        'KF-XE75-AJall',
        '400INVM9999-0DPHI20-J30s6-AJ20s6',
        '400INVM9999-0DPHI22-J30s6-AJ20s6',
        '400INVM9999-0DPHI24-J30s6-AJ20s6',
        '400INVM9999-0DPHI26-J30s6-AJ20s6',
        #'400INVM9999-0DPHI20-AJ30s6-AJ20s6',
        #'400INVM9999-0DPHI22-AJ30s6-AJ20s6',
        #'400INVM9999-0DPHI24-AJ30s6-AJ20s6',
        #'400INVM9999-0DPHI26-AJ30s6-AJ20s6',

        'LATE-MU10s1',
        'SC111-CJ15ab.ETA26',
#        'SC85-CJ15ab.ETA26',
        
        # ALFA (the replication is needed to build all the combinations in the CTPCore)
        'ALFA_B7R1L', 'ALFA_B7R1U', 'ALFA_A7R1L', 'ALFA_A7R1U', 'ALFA_A7L1L', 'ALFA_A7L1U', 'ALFA_B7L1L', 'ALFA_B7L1U',
        'ALFA2_B7R1L', 'ALFA2_B7R1U', 'ALFA2_A7R1L', 'ALFA2_A7R1U', 'ALFA2_A7L1L', 'ALFA2_A7L1U', 'ALFA2_B7L1L', 'ALFA2_B7L1U',
        'ALFA3_B7R1L', 'ALFA3_B7R1U', 'ALFA3_A7R1L', 'ALFA3_A7R1U', 'ALFA3_A7L1L', 'ALFA3_A7L1U', 'ALFA3_B7L1L', 'ALFA3_B7L1U',
        'ALFA4_B7R1L', 'ALFA4_B7R1U', 'ALFA4_A7R1L', 'ALFA4_A7R1U', 'ALFA4_A7L1L', 'ALFA4_A7L1U', 'ALFA4_B7L1L', 'ALFA4_B7L1U',
#ATR-13743      
        'ALFA_B7R1L_OD', 'ALFA_B7R1U_OD', 'ALFA_A7R1L_OD', 'ALFA_A7R1U_OD', 'ALFA_A7L1L_OD', 'ALFA_A7L1U_OD', 'ALFA_B7L1L_OD', 'ALFA_B7L1U_OD',

        #ATR-19355
        '0INVM10-3MU4ab',

        #ATR-18815
        '0INVM10-0DR15-EM8abi-MU10ab',
        '0INVM10-0DR15-EM12abi-MU6ab',

        #ATR-18824
        '60INVM9999-25DPHI32-EM15abhi-FJj15s623ETA49',
        '60INVM9999-04DPHI32-EM15abhi-FJj15s623ETA49',
        #ATR-19302
#        '0INVM70-27DPHI32-EM10his1-EM10his6',
        '0INVM70-27DPHI32-EM12his1-EM12his6',

        #ATR-19720
        '8INVM15-0DR22-2MU6ab',
        '2INVM9-2DR15-2MU6ab',
        '2INVM9-0DR15-MU6ab-MU4ab',
        '8INVM15-0DR22-MU6ab-MU4ab',
        '2INVM9-0DR15-2MU4ab',

        #ATR-19510
        '5DETA99-5DPHI99-2MU4ab',
            
        #ATR-17320
        'CEP-CJ60s6',
        'CEP-CJ50s6',
       
        #ATR-20174
        '8INVM15-2CMU4ab'
        ]

    
    Lvl1Flags.items = [

        
        # single EM

        'L1_EM3','L1_EM7','L1_EM12', 'L1_EM8VH',
#        'L1_EM8I',
        'L1_EM10VH', 'L1_EM15',
        'L1_EM15VHI',
        'L1_EM15VH', 'L1_EM18VHI', 'L1_EM20VH', 'L1_EM20VHI', 'L1_EM22VHI', 'L1_EM22VH','L1_EM24VHI', # see Jira ATR-10334'L1_EM22VHLIL', #'L1_EM50V',
        'L1_EM3_EMPTY', 'L1_EM7_EMPTY', 'L1_EM7_UNPAIRED_ISO', 'L1_EM7_FIRSTEMPTY',
        'L1_EM20VH_FIRSTEMPTY',
        #'L1_EM15_BGRP7',
        'L1_eEM3', 'L1_eEM12', 'L1_eEM15',
        'L1_eEM22',
        'L1_eEM22VH',
        'L1_eEM22VHI',
        'L1_eEM20',
        'L1_eEM20VH',
        'L1_eEM20VHI',
        'L1_eEM24VHI',
        'L1_eEM24VHIM',
        'L1_eEM7',
        'L1_2eEM7',
        'L1_2eEM8',
        'L1_2eEM10',
        'L1_2eEM12',

        # see savannah https://savannah.cern.ch/bugs/?103935

        # combined EM -jet
#        'L1_JJ15.23ETA49', # 4x4 window
        # single MU
        "L1_MU4", "L1_MU6", "L1_MU10", "L1_MU11", "L1_MU20", "L1_MU21",
        
        "L1_MU4_EMPTY", "L1_MU6_EMPTY", "L1_MU4_FIRSTEMPTY", "L1_MU11_EMPTY",
        'L1_MU4_UNPAIRED_ISO',

        
        # multi lepton
        "L1_2EM3", "L1_2EM7",   "L1_2EM15", "L1_2EM15VH", "L1_2EM15VHI",   
	"L1_2EM20VH",  "L1_EM20VH_3EM10VH", 
        "L1_EM7_MU10",
        "L1_2MU4", "L1_2MU6", "L1_2MU10", "L1_2MU20_OVERLAY",  "L1_MU11_2MU6", "L1_3MU4", "L1_MU6_2MU4", "L1_3MU6",  "L1_4MU4", "L1_MU6_3MU4", "L1_2MU6_3MU4", "L1_2MU11", "L1_MU11_2MU10",
          "L1_2EM8VH_MU10", "L1_EM15VH_MU10",  "L1_EM7_MU20", "L1_EM8VH_MU20",
          'L1_2eEM15VHI', 'L1_2eEM20VH', 'L1_2eEM8VH_MU10',
          'L1_eEM15VH_MU10',
          'L1_eEM20VH_3eEM10VH',
          'L1_eEM7_MU20',

        # single tau
        "L1_TAU12", "L1_TAU12IM",   "L1_TAU20IM",
        "L1_TAU40", "L1_TAU60", "L1_TAU100", "L1_TAU8", "L1_TAU8_EMPTY", 
        "L1_TAU8_UNPAIRED_ISO","L1_TAU8_FIRSTEMPTY",
        'L1_eTAU12', 'L1_eTAU100',

        # multi tau
        "L1_TAU20IM_2TAU12IM",   "L1_TAU60_2TAU40", "L1_2TAU8",
        'L1_eTAU60_2eTAU40',

        # combined tau - lepton
        "L1_EM15VHI_2TAU12IM",          
        "L1_EM15VHI_2TAU12IM_J25_3J12",   
	"L1_EM15VHI_2TAU12IM_4J12",
        "L1_EM15VHI_TAU40_2TAU12",
        "L1_MU10_TAU12IM",  
        "L1_MU10_TAU12IM_J25_2J12",
        "L1_MU10_TAU12IM_3J12", 
#        "L1_MU10_TAU12_J25_2J12", 
#        "L1_MU10_TAU20",
        "L1_MU10_TAU20IM",
        "L1_MU11_TAU20IM",
        "L1_MU10_TAU20IM_J25_2J20",
        'L1_MU10_eTAU20IT',

        # combined tau - jet
#        "L1_TAU25IT_2TAU12IT_2J25_3J12", 

        "L1_TAU20IM_2TAU12IM_J25_2J20_3J12",
        "L1_TAU20IM_2TAU12IM_4J12.0ETA25",
        "L1_TAU25IM_2TAU20IM_2J25_3J20", 
        'L1_eTAU20IT_2jJ20_gXERHO45',
        'L1_eTAU20IT_2eTAU12IL_4jJ12.0ETA25',
        'L1_eTAU25IT_2eTAU20IT_2jJ25_3jJ20',



        # combined tau - xe
        "L1_TAU20IM_2J20_XE45", 
        "L1_TAU20IM_2J20_XE50", 
#        "L1_TAU20_2J20_XE45", 
#        "L1_TAU20IM_2J20_XE50",   

        #"L1_EM15HI_2TAU12I_XE35", 
        "L1_EM15VHI_2TAU12IM_XE35", 
        'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20',


        "L1_MU10_TAU12IM_XE35",  
#        "L1_MU10_TAU12IM_XE40",  

        "L1_TAU20IM_2TAU12IM_XE35", 
        "L1_TAU40_2TAU12IM_XE40",  
        'L1_eTAU40_2eTAU12IL_gXERHO40',



        # combined em - jet
        #'L1_EM15VH_J15.23ETA49',
#        'L1_EM15VH_JJ15.23ETA49',
#        'L1_EM18VHI_JJ15.23ETA49',
#        'L1_EM20VHI_JJ15.23ETA49',
        'L1_EM18VHI_3J20',
        'L1_EM20VH_3J20',
        #'L1_EM18VH_3J20',
        'L1_eEM18VHI_3jJ20',

        # combined em / mu - tau - jet / XE
        'L1_eEM15VHI_2eTAU12IL_4jJ12',
        'L1_eEM15VHI_2eTAU12IL_jJ25_3jJ12',
        'L1_eEM15VHI_2eTAU12IL_gXERHO35',
        'L1_eEM20VHI_eTAU20IT_2eTAU20_jJ25_3jJ20',
        'L1_MU10_eTAU12IL_3jJ12',
        'L1_MU10_eTAU12IL_jJ25_2jJ12',
        'L1_MU10_eTAU12IL_gXERHO35',
        
        

        # combined mu - jet
        'L1_MU6_J20', 'L1_MU6_J40', 'L1_MU6_J75',
#ATR-13743        'L1_MU4_J30',
        #'L1_MU4_J50',
        'L1_MU10_2J20','L1_MU10_3J20', 'L1_MU10_2J15_J20',
        'L1_MU20_J40',
        'L1_MU20_XE30',
        'L1_MU10_2jJ15_jJ20',
        'L1_MU10_2jJ20',
        'L1_MU20_jJ40',
        'L1_MU20_gXERHO30',
        'L1_MU20_J50',

        # single jet
        "L1_J15","L1_J20","L1_J25", "L1_J30", "L1_J40", "L1_J50" ,"L1_J75","L1_J85", "L1_J100", "L1_J120", "L1_J400",
        "L1_J20.31ETA49", "L1_J30.31ETA49", "L1_J50.31ETA49", "L1_J75.31ETA49", #"L1_J100.31ETA49",
        #'L1_J15.23ETA49',
         'L1_J15.31ETA49',
        #'L1_J40.0ETA25', 
        "L1_J12_EMPTY","L1_J12_FIRSTEMPTY", "L1_J12_UNPAIRED_ISO", "L1_J12_UNPAIRED_NONISO", "L1_J12_ABORTGAPNOTCALIB", 
        "L1_J15.31ETA49_UNPAIRED_ISO",
        "L1_J30_EMPTY", "L1_J30_FIRSTEMPTY", "L1_J30.31ETA49_EMPTY", "L1_J30.31ETA49_UNPAIRED_ISO",
        "L1_J30.31ETA49_UNPAIRED_NONISO",
        "L1_J50_UNPAIRED_ISO", "L1_J50_UNPAIRED_NONISO", "L1_J50_ABORTGAPNOTCALIB",         
        'L1_J100_FIRSTEMPTY',
        #'L1_J100.31ETA49_FIRSTEMPTY',
        'L1_jJ100',
        #'L1_jJ75.31ETA49',
       
        
        # multi jet

	"L1_J45.0ETA20_3J15.0ETA25",
	"L1_J50_2J40.0ETA25_3J15.0ETA25",
           "L1_3J50", "L1_4J15", "L1_4J20",
        "L1_3J15.0ETA25_XE40",
        
        #"L1_3J75",
        #"L1_4J30",
        "L1_6J15",
        "L1_J85_3J30",
        'L1_3jJ50',
        'L1_4jJ15',
        'L1_jJ25.0ETA23_2jJ15.31ETA49',
        'L1_jJ40.0ETA25_2jJ25_jJ20.31ETA49',
        'L1_jJ75.31ETA49',
        #ATR-20408: gFEX
        'L1_gLJ100',
        'L1_gLJ120',
        'L1_gLJ140',
        'L1_gLJ160',
        'L1_jLJ100',
        'L1_jLJ120',
        'L1_jLJ140',
        'L1_jLJ160',
        'L1_jJ85_3jJ30',
       # multi jet forward
        "L1_J25.0ETA23_2J15.31ETA49",
        "L1_J40.0ETA25_2J15.31ETA49",
        "L1_J40.0ETA25_2J25_J20.31ETA49",
        
        # multi jet central
        "L1_3J25.0ETA23",
        "L1_3J35.0ETA23",
        "L1_4J15.0ETA25",
        "L1_5J15.0ETA25", 
        'L1_3jJ15.0ETA25_gXERHO40',
        'L1_3jJ35.0ETA23',
        'L1_4jJ15.0ETA25',
        # combined jet
        "L1_2J15_XE55", "L1_J40_XE50",

        #'L1_2J40_XE45',
        "L1_2J50_XE40", "L1_J40_XE60",
        'L1_2jJ50_gXERHO40', #'L1_3jJ15.0ETA25_gXERHO40',
        

        # XE
        "L1_XE35", "L1_XE40", "L1_XE45", "L1_XE50", 
        "L1_XE55", "L1_XE60",
        'L1_XE30', 'L1_XE300',
        'L1_gXERHO50',
        'L1_gXEPUFIT50',
        'L1_gXERHO20',
        'L1_gXEPUFIT20',
        'L1_gXE50',
        'L1_jXE50',
        'L1_jXENC50',
        'L1_gXENC70',
        'L1_gXEPUFIT60',
        'L1_jXERHO80',
        'L1_gXEJWOJ110',
        'L1_XE40.0ETA24',
        'L1_XE50.0ETA24',
        'L1_XE60.0ETA24',
        
        #XS
        'L1_EM12_XS20', 'L1_EM15_XS30',
        
        # TE
        'L1_TE5',
        'L1_TE10',
        'L1_TE20',
        'L1_TE50',
        #"L1_TE3",                                                        
        #                        "L1_TE45",                "L1_TE55",        "L1_TE60",        "L1_TE65",
        #      "L1_TE60", "L1_TE70",
        #"L1_TE2",        "L1_TE3",   "L1_TE4",                                "L1_TE70",

        #"L1_TE3.0ETA24", "L1_TE5.0ETA24", "L1_TE10.0ETA24", "L1_TE15.0ETA24", "L1_TE20.0ETA24", "L1_TE25.0ETA24", "L1_TE30.0ETA24", "L1_TE40.0ETA24", 
        #"L1_TE5.0ETA24", "L1_TE30.0ETA24", "L1_TE40.0ETA24", "L1_TE45.0ETA24", "L1_TE50.0ETA24", "L1_TE55.0ETA24", "L1_TE60.0ETA24", "L1_TE65.0ETA24", 
        #"L1_TE5.0ETA24", "L1_TE10.0ETA24", "L1_TE15.0ETA24", "L1_TE20.0ETA24", "L1_TE25.0ETA24", "L1_TE30.0ETA24", "L1_TE40.0ETA24","L1_TE70.0ETA24",
        #"L1_VTE2",        "L1_VTE3",   "L1_VTE4",  "L1_VTE5",        "L1_VTE10",

        #VTE + MBTS
        #"L1_MBTS_1_VTE2", "L1_MBTS_1_VTE3", "L1_MBTS_1_VTE4", "L1_MBTS_1_VTE5", "L1_MBTS_1_VTE10", "L1_MBTS_2_VTE2", "L1_MBTS_2_VTE3", "L1_MBTS_2_VTE4", "L1_MBTS_2_VTE5", "L1_MBTS_2_VTE10", 
        
        #TE + MU

        # MBTS
        # beam splashes
        
        #Overlay
#        "L1_MBTS_1_OVERLAY","L1_TE20_OVERLAY",
        
        #BeamSpot
#        "L1_MBTS_1_VTE70",

        #BGRP9

        #BGRP11

        # RNDM
        'L1_RD0_FILLED', 'L1_RD0_UNPAIRED_ISO',  'L1_RD0_EMPTY',
        "L1_RD1_EMPTY",
#        'L1_RD0_FIRSTEMPTY', 
        'L1_RD0_ABORTGAPNOTCALIB',
        'L1_RD2_FILLED',

        #LUCID
#        'L1_LUCID_A_C_EMPTY', 'L1_LUCID_A_C_UNPAIRED_ISO', 'L1_LUCID_A_C_UNPAIRED_NONISO',
        #'L1_LUCID_COMM_EMPTY', 'L1_LUCID_COMM_UNPAIRED_ISO', 

        

        # ZDC
         # FTHR

        # VDM
         #"L1_BGRP7",

        # TRT
        # "L1_TRT",

        # TGC
        "L1_TGC_BURST",
#        "L1_TGC_BURST_EMPTY",

        # LHCF
    
        #CALREQ
        "L1_CALREQ2",

        #ZB
        "L1_ZB",

        # BPTX
        
        # BCM
         #'L1_BCM_AC_CA_UNPAIRED_ISO',
        'L1_BCM_AC_CA_BGRP0', 'L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO',
        'L1_BCM_AC_UNPAIRED_ISO','L1_BCM_CA_UNPAIRED_ISO',
        'L1_BCM_AC_UNPAIRED_NONISO','L1_BCM_CA_UNPAIRED_NONISO',
        'L1_BCM_AC_ABORTGAPNOTCALIB', 'L1_BCM_CA_ABORTGAPNOTCALIB','L1_BCM_Wide_ABORTGAPNOTCALIB',
        'L1_BCM_AC_CALIB', 'L1_BCM_CA_CALIB','L1_BCM_Wide_CALIB',

        #AFP
#         'L1_AFP_FSC', 
#        'L1_AFP_C_ANY', 'L1_AFP_C_ANY_UNPAIRED_ISO', 'L1_AFP_C_ANY_UNPAIRED_NONISO', 'L1_AFP_C_ANY_EMPTY', 'L1_AFP_C_ANY_FIRSTEMPTY', 'L1_AFP_C_AND',

#	'L1_EM7_AFP_A_AND_C','L1_EM12_AFP_A_AND_C',


        #--------------------------------
        # TOPO items
        #--------------------------------

        # B-jet
        #'L1_BTAG-MU4J15', 'L1_BTAG-MU6J20',
        #'L1_BTAG-MU4J20',
        #'L1_3J20_BTAG-MU4J30', 'L1_3J20_BTAG-MU6J25',

        # ZH
            #'L1_J40_DPHI-J20s2XE30',

        #Higgsino
        'L1_2MU4_J40_XE50',
        'L1_MU4_J30_XE40_DPHI-J20s2XE30','L1_MU4_J50_XE50_DPHI-J20s2XE30',
        'L1_MU4_XE60',

        # HT
#        'L1_HT190-J15.ETA21',
        'L1_HT190-J15s5.ETA21',
#        'L1_HT150-J20.ETA31',
        'L1_HT150-J20s5.ETA31',   

        "L1_HT150-J20s5.ETA31_MJJ-400-CF",
        
        # Jpsi T&P
        'L1_JPSI-1M5-EM7', 'L1_JPSI-1M5-EM12',    
#            
        
        # W T&P
#        'L1_W-05DPHI-JXE-0',
#        'L1_W-10DPHI-JXE-0',
#        'L1_W-15DPHI-JXE-0',
#        'L1_W-05DPHI-EMXE-1',
        #'L1_W-15DPHI-EMXE-1',
#        'L1_W-05DPHI-EM15XE-1',
        #'L1_W-15DPHI-EM15XE-1',
        #'L1_W-90RO2-XEHT-0',
        #'L1_W-250RO2-XEHT-0',
        #'L1_W-HT20-JJ15.ETA49',
        #'L1_W-NOMATCH',
        #'L1_W-NOMATCH_W-05RO-XEEMHT',
#        'L1_EM12_W-MT35',
        # Old
        #'L1_W-10DPHI-JXE-0',
        #'L1_W-20DPHI-JXE-1',
        #'L1_W-20DPHI-JXE-0',
        #'L1_W-20DPHI-EMXE-1',
        #'L1_W-20DPHI-EMXE-0',
        #
        #'L1_W-08RO-XEHT-1',
        #'L1_W-40RO2-XEHT-0',
        #'L1_W-90RO2-XEHT-1',
        #'L1_W-HT20-JJ0.ETA49',
        #'L1_W-NOMATCH',
        #'L1_W-NOMATCH_W-05RO-XEEMHT',
        #'L1_EM3_W-MT20',
        #'L1_EM3_W-MT40',

        #'L1_DY-DR-2MU4',
        #'L1_DY-BOX-MU6MU4',
        'L1_DY-BOX-2MU6',
        'L1_LFV-MU6',
        'L1_LFV-MU11',
        'L1_LFV-EM8I',
        'L1_LFV-EM15I',
        'L1_DPHI-J20s2XE50',
        'L1_J40_XE50_DPHI-J20s2XE50',
#        'L1_DPHI-J20XE50',
#        'L1_DPHI-CJ20XE50',
        'L1_2MU4_J20_XE30_DPHI-J20s2XE30',
        'L1_MJJ-700',
#        'L1_MJJ-800',
#        'L1_MJJ-400',
        #'L1_MJJ-350',
        'L1_MJJ-300',
#        'L1_MJJ-200',
#        'L1_MJJ-100',
#        'L1_MJJ-600-NFF',
        'L1_MJJ-500-NFF',
#        'L1_MJJ-400-NFF',
#        'L1_MJJ-200-NFF',
#        'L1_63DETA-J30J15',
        'L1_J50_DETA20-J50J',
        'L1_DPHI-2EM3',
#        'L1_DPHI-2EM3_VTE5.24ETA49_ALFA_EINE',
#        'L1_HT150-JJ15.ETA49',
#        'L1_HT150-JJ15.ETA49_MJJ-400',
#        'L1_J4-MATCH',
        'L1_LLP-RO',
        'L1_LLP-NOMATCH',
#        'L1_TAU12I-J25',    
#        'L1_EM15TAU12I-J25',
        'L1_DR-EM15TAU12I-J25',    
#        'L1_TAU20ITAU12I-J25',
        'L1_DR-TAU20ITAU12I',    
        'L1_DR-TAU20ITAU12I-J25',

        'L1_30M-EM20ITAU12',
        'L1_MJJ-400-CF',
    
#        'L1_LAR-EM',
#        'L1_LAR-J',
#        'L1_LAR-ZEE',
#        'L1_LAR-ZEE_LAR-EM',
#        'L1_FTK-EM',
#        'L1_FTK-J',
#        'L1_FTK-MU',
#        'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4',
#        'L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4',
#        'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6',
#        'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6',
#        'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO',

        #ATR-19720
        'L1_BPH-8M15-0DR22-2MU6',
        'L1_BPH-2M9-2DR15-2MU6',
        'L1_BPH-2M9-0DR15-MU6MU4',
        'L1_BPH-8M15-0DR22-MU6MU4-BO',
        'L1_BPH-2M9-0DR15-2MU4',

        'L1_BPH-0M9-EM7-EM5',        
        'L1_BPH-0DR3-EM7J15',
        'L1_BPH-0M9-EM7-EM5_MU6',
        'L1_BPH-0DR3-EM7J15_MU6',
        'L1_BPH-0M9-EM7-EM5_2MU4',
        'L1_BPH-0DR3-EM7J15_2MU4',

        # combined L1Topo for VBF
        
        # INVM + DPHI 
        'L1_MJJ-400-NFF-0DPHI20',
        'L1_MJJ-400-NFF-0DPHI22',
        'L1_MJJ-400-NFF-0DPHI24',
        'L1_MJJ-400-NFF-0DPHI26',
#        'L1_MJJ-400-0DPHI20',
#        'L1_MJJ-400-0DPHI22',
#        'L1_MJJ-400-0DPHI24',
#        'L1_MJJ-400-0DPHI26',
   
        ### ATR-15062
        'L1_EM18VHI_MJJ-300',

        'L1_LATE-MU10_XE50','L1_LATE-MU10_XE40', 'L1_LATE-MU10_J50',

        'L1_TAU60_DR-TAU20ITAU12I',

        'L1_SC111-CJ15',
#        'L1_SC85-CJ15',
        
        ##     # Partition 1
        ##     'L1_CALREQ0_P1', 'L1_CALREQ1_P1', 'L1_CALREQ2_P1',

        #ALFA
#        'L1_ALFA_SDIFF5','L1_ALFA_SDIFF6','L1_ALFA_SDIFF7','L1_ALFA_SDIFF8',
#        'L1_MBTS_1_A_ALFA_C','L1_MBTS_1_C_ALFA_A','L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO','L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO',
#        'L1_MBTS_2_A_ALFA_C','L1_MBTS_2_C_ALFA_A','L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO','L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO',
#        'L1_LUCID_A_ALFA_C','L1_LUCID_C_ALFA_A','L1_LUCID_A_ALFA_C_UNPAIRED_ISO','L1_LUCID_C_ALFA_A_UNPAIRED_ISO',
#
#        'L1_ALFA_ELASTIC_UNPAIRED_ISO', 'L1_ALFA_ANTI_ELASTIC_UNPAIRED_ISO',        
#
#        #'L1_ZDC_A_ALFA_C','L1_ZDC_C_ALFA_A','L1_ZDC_ALFA',
#        'L1_EM3_ALFA_ANY',
#        'L1_EM3_ALFA_ANY_UNPAIRED_ISO',
#
#        'L1_EM3_ALFA_EINE',
#        'L1_J12_ALFA_ANY','L1_J12_ALFA_ANY_UNPAIRED_ISO',
#        'L1_TE5_ALFA_ANY','L1_TE5_ALFA_ANY_UNPAIRED_ISO',
#         'L1_TE5_ALFA_EINE',
#        'L1_TRT_ALFA_ANY','L1_TRT_ALFA_ANY_UNPAIRED_ISO',
#        'L1_TRT_ALFA_EINE',
#        #'L1_LHCF_ALFA_ANY_A',
#        #'L1_LHCF_ALFA_ANY_C',
##        'L1_LHCF_ALFA_ANY_A_UNPAIRED_ISO','L1_LHCF_ALFA_ANY_C_UNPAIRED_ISO',
#        'L1_ALFA_BGT','L1_ALFA_BGT_UNPAIRED_ISO',
#        'L1_ALFA_ANY_A_EMPTY','L1_ALFA_ANY_C_EMPTY',
#
#         'L1_AFP_C_MBTS_A',
#         'L1_AFP_C_ZDC_C',
#         'L1_AFP_C_J12',
#         'L1_AFP_C_EM3',
#         'L1_AFP_C_TE5',
#         'L1_AFP_C_ALFA_C',
#         'L1_AFP_C_ALFA_A',
#         'L1_AFP_C_ANY_MBTS_A',
#         'L1_AFP_C_MU4',

        # Items for beta*=90m run
#        'L1_J12_ALFA_EINE',
#        'L1_2EM3_ALFA_EINE',
#        'L1_MU4_ALFA_ANY',
#        'L1_MU4_ALFA_ANY_UNPAIRED_ISO',
#        'L1_MU4_ALFA_ANY_PAIRED_UNPAIRED_ISO',
#        'L1_MU4_ALFA_EINE',


        #ATR-18815
        'L1_LFV-EM8I-MU11',
        'L1_LFV-EM12I-MU6',

        #ATR-19355
        'L1_BPH-0M10-3MU4',

        #ATR-18824
        'L1_ZAFB-04DPHI-EM15I',
        'L1_ZAFB-25DPHI-EM15I',
        'L1_ZAFB-25DPHI-EM18I',

        #ATR-19302: 
#        'L1_DPHI-M70-2EM10I',
        'L1_DPHI-M70-2EM12I',

        #ATR-19510
        'L1_DY-BOX-2MU4',
                
        #ATR-17320
        'L1_CEP-CJ60',
        'L1_CEP-CJ50' ,

        #ATR-20174
        'L1_BPH-8M15-2MU4-BO'

        ]
    

#CTP IDs are taken from this mapping. Every L1 item needs a unique ctpid.
# Run this file as python python/l1menu/Menu_MC_pp_v7.py to print out available IDs
# 463-464 are reserved for L1_RD2_BGRP14 and L1_RD3_BGRP15 (in MC_Physics_pp_v7)
# 509-511 are reserved for CALREQ
    
    Lvl1Flags.CtpIdMap = {
 
        'L1_EM3' : 0,
        'L1_EM7' : 1,
        'L1_EM12' : 2,
        'L1_EM8VH' : 3,
        'L1_EM10VH' : 4,
        #'L1_EM13VH' : 5,
        'L1_EM15' : 6,
        'L1_EM15VH' : 7,
        'L1_EM18VHI' : 8,
        'L1_EM20VH' : 9,
        'L1_EM20VHI' : 10,
        'L1_EM22VHI' : 11,
        'L1_EM22VH' : 295,
        'L1_EM3_EMPTY' : 12,
        'L1_EM7_EMPTY' : 13,
        'L1_eEM3': 212, 
        'L1_eEM8': 213, 
        'L1_eEM10': 214, 
        'L1_eEM15': 215, 
        'L1_eEM20': 217,
        'L1_eEM7': 195,
        'L1_2eEM7': 221,         
        'L1_2eEM8': 211,         
        'L1_2eEM10': 226,         
        'L1_2eEM12': 222,         
        'L1_eEM22' : 25,
        'L1_2eEM15VHI' : 42,
        'L1_2eEM20VH' : 49,
        'L1_2eEM8VH_MU10' : 50,
        'L1_eEM15VHI_2eTAU12IL_4jJ12' : 59,
        'L1_eEM15VHI_2eTAU12IL_jJ25_3jJ12' : 68,
        'L1_eEM15VHI_2eTAU12IL_gXERHO35' : 73,
        'L1_eEM15VH_MU10' : 82,
        'L1_eEM18VHI_3jJ20' : 86,
        'L1_eEM20VHI_eTAU20IT_2eTAU20_jJ25_3jJ20' :  87,
        'L1_eEM20VH_3eEM10VH' : 91,
        'L1_eEM22VHI' : 107,
        'L1_eEM12' : 180,
        'L1_eEM22VH' : 137,
        'L1_eEM24VHI' : 102,
        'L1_eEM24VHIM' : 72,
        'L1_eEM20VHI' : 178, 
        'L1_eEM20VH' : 186, 
        'L1_eEM7_MU20' : 110,
        'L1_MU4' : 14,
        'L1_MU6' : 15,
        'L1_MU10' : 16,
        'L1_MU11' : 256,
        'L1_MU20' : 18,
        'L1_MU21' : 17,
        'L1_MU4_EMPTY' : 19,
        'L1_MU6_EMPTY' : 66,
        'L1_MU4_FIRSTEMPTY' : 20,
        'L1_MU11_EMPTY' : 21,
        'L1_MU4_UNPAIRED_ISO' : 22,
        'L1_2EM3' : 23,
        'L1_2EM7' : 24,
        #'L1_2EM13VH' : 26,
        'L1_2EM15' : 27,
        'L1_2EM15VH' : 28,
	#'L1_2EM18VH':398,
	'L1_2EM20VH':400,
	#'L1_EM18VH_3EM8VH':406,
	#'L1_EM18VH_2EM10VH_3EM7':408,
        'L1_EM20VH_3EM10VH':431, 
        'L1_2MU4' : 32,
        'L1_2MU6' : 33,
        'L1_2MU10' : 34,
        'L1_2MU11' : 183,
        'L1_2MU20_OVERLAY' : 35,
        'L1_MU11_2MU6' : 37,
        'L1_MU11_2MU10' : 185,
        'L1_3MU4' : 38,
        'L1_MU6_2MU4' : 39,
        'L1_3MU6' : 40,
        'L1_4MU4' : 46,
        'L1_4J15.0ETA25' : 41,
        'L1_2EM8VH_MU10' : 43,
        'L1_EM15VH_MU10' : 44,
        'L1_EM7_MU20' : 483,
        'L1_EM8VH_MU20' : 484,
        'L1_TAU12' : 45,
        'L1_TAU12IM' : 47,
        'L1_TAU20IM' : 51,
        'L1_TAU40' : 54,
        'L1_TAU60' : 55,
        'L1_TAU100' : 52,
        'L1_TAU8' : 56,
        'L1_TAU8_EMPTY' : 57,
        'L1_TAU20IM_2TAU12IM' : 58,
        'L1_eTAU12' : 29,
        'L1_eTAU100' : 179,
        'L1_2TAU8' : 370,
        'L1_EM15VHI_2TAU12IM' : 60,
        'L1_EM15VHI_2TAU12IM_J25_3J12' : 61,
        'L1_EM15VHI_2TAU12IM_4J12' : 5,
        'L1_EM20VHI_TAU20IM_2TAU20_J25_3J20' : 69,
        'L1_EM15VHI_TAU40_2TAU12' : 62,
        'L1_MU10_TAU12IM' : 63,
        'L1_MU10_TAU12IM_J25_2J12' : 64,
        'L1_MU10_TAU12IM_3J12' : 482,
        'L1_EM7_MU10':65,
        'L1_TAU30_EMPTY':341,
        'L1_MU10_TAU20IM' : 67,
        'L1_MU11_TAU20IM' : 430,
        'L1_MU10_TAU20IM_J25_2J20' : 377,


        'L1_TAU20IM_2TAU12IM_J25_2J20_3J12' : 70,
        'L1_TAU25IM_2TAU20IM_2J25_3J20' : 398,
        'L1_TAU20IM_2TAU12IM_4J12.0ETA25' : 317,
        'L1_TAU20IM_2J20_XE45' : 74,
        'L1_TAU20IM_2J20_XE50' : 48,
        'L1_TAU30_UNPAIRED_ISO' :343,
        'L1_TAU60_DR-TAU20ITAU12I' : 76,
        'L1_EM15VHI_2TAU12IM_XE35' : 78,
        'L1_MU10_TAU12IM_XE35' : 81,
#        'L1_MU10_TAU12IM_XE40' : 82,
        'L1_TAU20IM_2TAU12IM_XE35' : 83,
        'L1_TAU40_2TAU12IM_XE40' : 429,
#        'L1_EM15VH_JJ15.23ETA49' : 86,
#        'L1_EM18VHI_JJ15.23ETA49' : 316,
#        'L1_EM20VHI_JJ15.23ETA49' : 326,
        'L1_MU6_J20' : 88,
        'L1_MU6_J40' : 89,
        'L1_MU6_J75' : 90,
        'L1_MU20_J40'  : 428,
        'L1_MU20_XE30' : 433,
        'L1_MU10_2jJ15_jJ20' : 133,
        'L1_MU10_2jJ20' : 136,
        'L1_MU10_eTAU12IL_3jJ12' : 138,
        'L1_MU10_eTAU12IL_jJ25_2jJ12' : 139,
        'L1_MU10_eTAU12IL_gXERHO35' : 143,
        'L1_MU10_eTAU20IT' : 152,
        'L1_MU20_jJ40' : 157,
        'L1_MU20_gXERHO30' : 158,
        'L1_eTAU20IT_2jJ20_gXERHO45' : 159,
        'L1_eTAU20IT_2eTAU12IL_4jJ12.0ETA25' : 160,
        'L1_eTAU25IT_2eTAU20IT_2jJ25_3jJ20' : 162,
        'L1_eTAU40_2eTAU12IL_gXERHO40' : 164,
        'L1_eTAU60_2eTAU40' : 165,
        'L1_J15' : 92,
        'L1_J20' : 93,
        'L1_J25' : 94,
        'L1_J30' : 95,
        
        'L1_J40' : 96,
        'L1_J50' : 97,
        'L1_J75' : 98,
        'L1_J85' : 99,
        'L1_J100' : 100,
        'L1_J120' : 101,
        'L1_J400' : 197,
        'L1_jJ100' : 36,
        'L1_2jJ50_gXERHO40' : 166,
        'L1_3jJ15.0ETA25_gXERHO40' : 167,
        'L1_3jJ35.0ETA23' : 168,
        'L1_3jJ50' : 169,
        'L1_4jJ15' : 170,
        'L1_4jJ15.0ETA25' : 171,
        'L1_jJ25.0ETA23_2jJ15.31ETA49' : 125,
        'L1_jJ40.0ETA25_2jJ25_jJ20.31ETA49' : 126,
        'L1_jJ75.31ETA49' : 127,
        #ATR-20408: gFEX
        'L1_gLJ100' : 77,
        'L1_gLJ120' : 79,
        'L1_gLJ140' : 129,
        'L1_gLJ160' : 227,
        'L1_jLJ100' : 228,
        'L1_jLJ120' : 238,
        'L1_jLJ140' : 241,
        'L1_jLJ160' : 242,
        'L1_jJ85_3jJ30' : 128,
        'L1_J20.31ETA49' : 103,
        'L1_J30.31ETA49' : 104,
        'L1_J50.31ETA49' : 105,
        'L1_J75.31ETA49' : 106,
        #'L1_J100.31ETA49' : 107,
#        'L1_JJ15.23ETA49' : 107,
#        'L1_J40.0ETA25' : 108,
        
        'L1_J15.31ETA49' : 109,
        'L1_J12_EMPTY' : 111,
        'L1_J12_FIRSTEMPTY' : 112,
        'L1_J12_UNPAIRED_ISO' : 113,
        'L1_J12_UNPAIRED_NONISO' : 114,
        'L1_J12_ABORTGAPNOTCALIB' : 115,
        'L1_J15.31ETA49_UNPAIRED_ISO' : 75,
        'L1_J30_EMPTY' : 116,
        'L1_J30_FIRSTEMPTY' : 117,
        'L1_J30.31ETA49_EMPTY' : 118,
        'L1_J30.31ETA49_UNPAIRED_ISO' : 119,
        'L1_J30.31ETA49_UNPAIRED_NONISO' : 120,
        'L1_J50_UNPAIRED_ISO' : 121,
        'L1_J50_UNPAIRED_NONISO' : 122,
        'L1_J50_ABORTGAPNOTCALIB' : 123,

        'L1_3J50' : 130,
        'L1_4J15' : 131,
        'L1_4J20' : 132,
        'L1_3J15.0ETA25_XE40' : 184,
        'L1_J45.0ETA20_3J15.0ETA25' : 374,
        'L1_J50_2J40.0ETA25_3J15.0ETA25' : 375,
 
#        'L1_3J75' : 133,
#        'L1_4J30' : 134,
        'L1_6J15' : 135,
        'L1_J85_3J30' : 480,

       
        'L1_5J15.0ETA25' : 140,
        'L1_2J15_XE55' : 141,
        'L1_J40_XE50' : 142,
        #'L1_J75_XE40' : 143,

        #'L1_XE25': 82,
        'L1_XE30': 85,
        'L1_XE300': 187,
        
        'L1_XE35' : 144,
        'L1_XE40' : 145,
        'L1_XE45' : 146,
        'L1_XE50' : 147,
        'L1_XE55' : 148,
        'L1_XE60' : 149,
        'L1_XE40.0ETA24' : 252,
        'L1_XE50.0ETA24' : 253,
        'L1_XE60.0ETA24' : 254,
 
        
        'L1_gXERHO20' : 177,
        'L1_gXERHO50' : 30,
        'L1_gXEPUFIT20' : 174,
        'L1_gXEPUFIT50' : 31,
        'L1_jXENC50' : 243,
        'L1_gXENC70' : 244,
        'L1_gXEPUFIT60' : 246,
        'L1_jXERHO80' : 247,
        'L1_gXEJWOJ110' : 248,
        'L1_gXE50' : 218,
        'L1_jXE50' : 219,
        'L1_EM12_XS20' : 154,
        'L1_EM15_XS30' : 155,

        'L1_TE5' : 53,
        'L1_TE10' : 150,
        'L1_TE20' : 151,
        'L1_TE50' : 188,

#        'L1_MBTS_1_VTE2' : 160,
        'L1_BCM_Wide_UNPAIRED_NONISO' : 161,
#        'L1_BCM_AC_CA_UNPAIRED_ISO' : 162,
        'L1_BCM_AC_UNPAIRED_ISO' : 163,

        'L1_EM18VHI_3J20' : 172,
        #'L1_EM18VH_3J20' : 295,
        'L1_EM20VH_3J20' : 26,
        'L1_MU10_3J20' : 173,

        'L1_2J50_XE40' : 175,
        'L1_J40_XE60' : 176,
        'L1_MU10_2J20' : 278,
        'L1_MU20_J50' : 198, 
        'L1_TAU60_2TAU40' : 458,
        'L1_MU10_2J15_J20' : 255,
        
        'L1_BPH-0M9-EM7-EM5' : 80,
        'L1_BPH-0DR3-EM7J15' : 84,
        'L1_BPH-0M9-EM7-EM5_MU6' : 124,
        'L1_BPH-0DR3-EM7J15_MU6' : 134,
        'L1_BPH-0M9-EM7-EM5_2MU4' : 153,
        'L1_BPH-0DR3-EM7J15_2MU4' : 156,

        'L1_J25.0ETA23_2J15.31ETA49': 335 , 
        'L1_J40.0ETA25_2J15.31ETA49' : 181,
        'L1_J40.0ETA25_2J25_J20.31ETA49' : 182,


         ## noid(ea)
        "L1_HT150-J20s5.ETA31_MJJ-400-CF": 273,
        
        ## the ctp ids are a mess.. no 7 consequetive ids are free.. but these here are:
#        'L1_AFP_C_MBTS_A':183,
#        'L1_AFP_C_ZDC_C':189,
#        'L1_AFP_C_J12':190,
#        'L1_AFP_C_EM3':191,
#        'L1_AFP_C_TE5':192,
#        'L1_AFP_C_ALFA_C':193,
#        'L1_AFP_C_ALFA_A':170,
#        'L1_AFP_C_ANY_MBTS_A':174,
#        'L1_AFP_C_MU4':379,   
 
#	'L1_EM7_AFP_A_AND_C': 435,
#	'L1_EM12_AFP_A_AND_C': 455,     

#        'L1_EM8I' : 183,
#        
        'L1_RD0_FILLED' : 200,
        'L1_RD0_UNPAIRED_ISO' : 201,
        'L1_RD0_EMPTY' : 202,
        'L1_RD1_EMPTY' : 204,
        'L1_RD2_FILLED' : 239,
#        'L1_RD0_FIRSTEMPTY' : 209,
#        'L1_LUCID_A_C_EMPTY' : 215,
#        'L1_LUCID_A_C_UNPAIRED_ISO' : 216,
#        'L1_LUCID_A_C_UNPAIRED_NONISO' : 217,
        'L1_TGC_BURST' : 220,
#        'L1_TGC_BURST_EMPTY' : 184,
        'L1_BCM_AC_CA_BGRP0' : 223,
        'L1_BCM_Wide_EMPTY' : 224,
        'L1_BCM_Wide_UNPAIRED_ISO' : 225,
        
      
        'L1_BCM_CA_UNPAIRED_ISO' : 229,
        'L1_BCM_AC_UNPAIRED_NONISO' : 230,
        'L1_BCM_CA_UNPAIRED_NONISO' : 231,
        'L1_BCM_AC_ABORTGAPNOTCALIB' : 232,
        'L1_BCM_CA_ABORTGAPNOTCALIB' : 233,
        'L1_BCM_Wide_ABORTGAPNOTCALIB' : 234,
        'L1_BCM_AC_CALIB' : 235,
        'L1_BCM_CA_CALIB' : 236,
        'L1_BCM_Wide_CALIB' : 237,
#        'L1_BTAG-MU4J15' : 238,
        'L1_ZB' : 240,
#        'L1_BTAG-MU6J20' : 243,
#        'L1_BTAG-MU4J20' : 245,
#        'L1_J40_DPHI-Js2XE50' : 250,
        #'L1_J40_DPHI-J20s2XE30' : 257,
        'L1_J40_XE50_DPHI-J20s2XE50': 245,
        'L1_2MU4_J20_XE30_DPHI-J20s2XE30' : 271,

        'L1_2MU4_J40_XE50' : 449,
        'L1_MU4_J30_XE40_DPHI-J20s2XE30' : 452,
        'L1_MU4_J50_XE50_DPHI-J20s2XE30' : 453,
        'L1_MU4_XE60' : 454,
        
#        'L1_HT190-J15.ETA21' : 258,
        'L1_HT190-J15s5.ETA21' : 259,
#        'L1_HT150-J20.ETA31' : 260,
        'L1_HT150-J20s5.ETA31' : 261,
        'L1_JPSI-1M5-EM7' : 263,
        'L1_JPSI-1M5-EM12' : 264,
#        
#        
#        
#        
#        
#        'L1_W-05DPHI-JXE-0' : 271,
#        'L1_W-10DPHI-JXE-0' : 272,
#        'L1_W-15DPHI-JXE-0' : 273,
        #'L1_W-15DPHI-EMXE-1' : 271,
        #'L1_W-15DPHI-EM15XE-1' : 272,
#        'L1_W-05DPHI-EMXE-1' : 276,
#        
#        'L1_W-90RO2-XEHT-0' : 279,
#        'L1_W-250RO2-XEHT-0' : 280,
#        'L1_W-HT20-JJ15.ETA49' : 281,
#        'L1_W-NOMATCH' : 282,
        #'L1_W-NOMATCH_W-05RO-XEEMHT' : 283,

        'L1_MJJ-400-NFF-0DPHI20' : 266,
        'L1_MJJ-400-NFF-0DPHI22' : 267,
        'L1_MJJ-400-NFF-0DPHI24' : 268,
        'L1_MJJ-400-NFF-0DPHI26' : 269,
#        'L1_MJJ-400-0DPHI20' : 270,
#        'L1_MJJ-400-0DPHI22' : 272,
#        'L1_MJJ-400-0DPHI24' : 274,
#        'L1_MJJ-400-0DPHI26' : 276,




#        'L1_BPH-2M9-2MU6_BPH-2DR15-2MU6' : 300,
#        'L1_BPH-8M15-2MU6_BPH-0DR22-2MU6'  : 301,
#        'L1_BPH-8M15-MU6MU4_BPH-0DR22-MU6MU4-BO' : 488,

        ### ATR-14350 - these should be replaced in 2017 with items 296 and 298
#        'L1_BPH-2M9-2MU4_BPH-0DR15-2MU4' : 426,
#        'L1_BPH-2M9-MU6MU4_BPH-0DR15-MU6MU4' : 427,

        #'L1_DY-DR-2MU4' : 315,
        #'L1_DY-BOX-MU6MU4' : 317,
        'L1_DY-BOX-2MU6' : 318,
        'L1_LFV-MU6' : 332,
        'L1_LFV-MU11' : 364,
        'L1_LFV-EM8I' : 320,
        'L1_LFV-EM15I' : 321,
        'L1_DPHI-J20s2XE50' : 323,
#        'L1_DPHI-J20XE50' : 324,
#        'L1_DPHI-CJ20XE50' : 325,
        'L1_MJJ-700' : 216,
#        'L1_MJJ-800' : 327,
#        'L1_MJJ-400' : 329,
        #'L1_MJJ-350' : 330,
        'L1_MJJ-300' : 327,
#        'L1_MJJ-200' : 332,
#        'L1_MJJ-100' : 333,
#        'L1_MJJ-600-NFF' : 127,
        'L1_MJJ-500-NFF' : 108,
#        'L1_MJJ-400-NFF' : 129,
#        'L1_MJJ-200-NFF' : 133,
#        'L1_63DETA-J30J15' : 257,
        'L1_J50_DETA20-J50J' : 275,
        'L1_DPHI-2EM3' : 288, 
#        'L1_HT150-JJ15.ETA49' : 334,
#        'L1_J4-MATCH' : 336,
        'L1_LLP-RO' : 338,
        'L1_LLP-NOMATCH' : 339,
#        'L1_TAU12I-J25' : 341,
#        'L1_EM15TAU12I-J25' : 345,
        'L1_DR-EM15TAU12I-J25' : 346,
#        'L1_TAU20ITAU12I-J25' : 347,
        'L1_DR-TAU20ITAU12I' : 348,
        'L1_DR-TAU20ITAU12I-J25' : 350,


        'L1_30M-EM20ITAU12' : 387,
        'L1_MJJ-400-CF': 397,

#        'L1_LAR-EM' : 351,
#        'L1_LAR-J' : 352,
#        'L1_FTK-EM' : 48,
#        'L1_FTK-J' : 77,
#        'L1_FTK-MU' : 79,
#        'L1_LAR-ZEE' : 71,
#        'L1_LAR-ZEE_LAR-EM' : 285,
        'L1_LATE-MU10_XE50': 354,
        'L1_LATE-MU10_XE40': 466,
        'L1_LATE-MU10_J50' : 355,
        'L1_SC111-CJ15' : 356,
#        'L1_SC85-CJ15' : 357,

        'L1_EM7_FIRSTEMPTY': 371, 
        'L1_RD0_ABORTGAPNOTCALIB':372,
        'L1_3J25.0ETA23' : 373,
        'L1_3J35.0ETA23' : 425,
#        'L1_TE4' : 374,
##         'L1_VTE3' : 375,
##         'L1_VTE4' : 48,
##         'L1_VTE5' : 376,
##         'L1_VTE10' : 409,
        #'L1_XS40' : 377,
        #'L1_XS50' : 378,
        #'L1_XS60' : 379,
        'L1_HT150-JJ15.ETA49_MJJ-400' : 416,

        ### ATR-15062
        'L1_EM18VHI_MJJ-300' : 385,
        
#ATR-13743        'L1_MU4_J30' : 386,
#        'L1_MU4_J50' : 387,

        
        'L1_TAU8_UNPAIRED_ISO': 388,
        'L1_EM7_UNPAIRED_ISO': 389,
        'L1_TAU8_FIRSTEMPTY': 391,
        'L1_EM24VHI'           : 392,
        "L1_EM15VHI"    :423,
        "L1_2EM15VHI"    :342,
        "L1_MU6_3MU4"  :403,
        "L1_2MU6_3MU4" :404,
        #
        #'L1_TE2'       :407,
        #'L1_TE60'      :408,
        #'L1_TE50.0ETA24':409,
#        'L1_VTE2':410, 
        'L1_EM20VH_FIRSTEMPTY':411, 
        'L1_J100_FIRSTEMPTY':414, 
        #'L1_J100.31ETA49_FIRSTEMPTY':415, 
        #'L1_TE60.0ETA24':416, 
#        'L1_MBTS_1_VTE4':417, 
#        'L1_MBTS_1_VTE3':418,
        
##         'L1_MBTS_1_VTE5':77, 
##         'L1_MBTS_1_VTE10':79,
##         'L1_MBTS_2_VTE2':80, 
##         'L1_MBTS_2_VTE3':108,
##         'L1_MBTS_2_VTE4':124, 
##         'L1_MBTS_2_VTE5':127,
##         'L1_MBTS_2_VTE10':129,  

        
#        'L1_MBTS_1_VTE70' : 477,

        # 3 x ZDC

        
        #Overlay
#        'L1_MBTS_1_OVERLAY' : 445,
#        'L1_TE20_OVERLAY' : 455,
        



        
        'L1_CALREQ2' : 511,  # never use 509-511 for anything else than CALREQ triggers
        
        #ATR-19355
        'L1_BPH-0M10-3MU4' : 189,

        #ATR-18815
        'L1_LFV-EM8I-MU11' : 190,
        'L1_LFV-EM12I-MU6' : 191,

        #ATR-18824
        'L1_ZAFB-04DPHI-EM15I' : 192,
        'L1_ZAFB-25DPHI-EM15I' : 193,
        'L1_ZAFB-25DPHI-EM18I' : 194,

        #ATR-19302: 
#        'L1_DPHI-M70-2EM10I' : 195,
        'L1_DPHI-M70-2EM12I' : 196,

        #ATR-19720
        'L1_BPH-8M15-0DR22-2MU6' : 205,
        'L1_BPH-2M9-2DR15-2MU6' : 206,
        'L1_BPH-2M9-0DR15-MU6MU4' : 207,
        'L1_BPH-8M15-0DR22-MU6MU4-BO' : 208,
        'L1_BPH-2M9-0DR15-2MU4' : 209,

        #ATR-19510
        'L1_DY-BOX-2MU4' : 199,
        
        #ATR-17320
        'L1_CEP-CJ60' : 203,
        'L1_CEP-CJ50' : 210,
      
        #ATR-20174
        'L1_BPH-8M15-2MU4-BO' : 71

}


    Lvl1Flags.prescales = {}

if __name__ == "__main__": print_available()
