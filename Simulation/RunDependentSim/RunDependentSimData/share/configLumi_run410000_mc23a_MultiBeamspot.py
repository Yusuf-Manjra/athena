# Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration

# configLumi_run410000_mc23a_MultiBeamspot.py
# Default mu-profile for variable beam spot simulation without
# bunch-to-bunch variations in luminosity.

# We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
    {'run':410000, 'lb':1, 'starttstamp':1625000060, 'evts':_evts(1), 'mu':26.5, 'step': 0},
    {'run':410000, 'lb':2, 'starttstamp':1625000120, 'evts':_evts(1), 'mu':27.5, 'step': 0},
    {'run':410000, 'lb':3, 'starttstamp':1625000180, 'evts':_evts(1), 'mu':29.5, 'step': 0},
    {'run':410000, 'lb':4, 'starttstamp':1625000240, 'evts':_evts(1), 'mu':30.5, 'step': 0},
    {'run':410000, 'lb':5, 'starttstamp':1625000300, 'evts':_evts(1), 'mu':31.5, 'step': 0},
    {'run':410000, 'lb':6, 'starttstamp':1625000360, 'evts':_evts(1), 'mu':32.5, 'step': 0},
    {'run':410000, 'lb':7, 'starttstamp':1625000420, 'evts':_evts(1), 'mu':33.5, 'step': 0},
    {'run':410000, 'lb':8, 'starttstamp':1625000480, 'evts':_evts(2), 'mu':34.5, 'step': 0},
    {'run':410000, 'lb':9, 'starttstamp':1625000540, 'evts':_evts(3), 'mu':35.5, 'step': 0},
    {'run':410000, 'lb':10, 'starttstamp':1625000600, 'evts':_evts(4), 'mu':36.5, 'step': 0},
    {'run':410000, 'lb':11, 'starttstamp':1625000660, 'evts':_evts(6), 'mu':37.5, 'step': 0},
    {'run':410000, 'lb':12, 'starttstamp':1625000720, 'evts':_evts(10), 'mu':38.5, 'step': 0},
    {'run':410000, 'lb':13, 'starttstamp':1625000780, 'evts':_evts(12), 'mu':39.5, 'step': 0},
    {'run':410000, 'lb':14, 'starttstamp':1625000840, 'evts':_evts(15), 'mu':40.5, 'step': 0},
    {'run':410000, 'lb':15, 'starttstamp':1625000900, 'evts':_evts(16), 'mu':41.5, 'step': 0},
    {'run':410000, 'lb':16, 'starttstamp':1625000960, 'evts':_evts(19), 'mu':42.5, 'step': 0},
    {'run':410000, 'lb':17, 'starttstamp':1625001020, 'evts':_evts(21), 'mu':43.5, 'step': 0},
    {'run':410000, 'lb':18, 'starttstamp':1625001080, 'evts':_evts(23), 'mu':44.5, 'step': 0},
    {'run':410000, 'lb':19, 'starttstamp':1625001140, 'evts':_evts(25), 'mu':45.5, 'step': 0},
    {'run':410000, 'lb':20, 'starttstamp':1625001200, 'evts':_evts(22), 'mu':46.5, 'step': 0},
    {'run':410000, 'lb':21, 'starttstamp':1625001260, 'evts':_evts(20), 'mu':47.5, 'step': 0},
    {'run':410000, 'lb':22, 'starttstamp':1625001320, 'evts':_evts(18), 'mu':48.5, 'step': 0},
    {'run':410000, 'lb':23, 'starttstamp':1625001380, 'evts':_evts(14), 'mu':49.5, 'step': 0},
    {'run':410000, 'lb':24, 'starttstamp':1625001440, 'evts':_evts(12), 'mu':50.5, 'step': 0},
    {'run':410000, 'lb':25, 'starttstamp':1625001500, 'evts':_evts(8), 'mu':51.5, 'step': 0},
    {'run':410000, 'lb':26, 'starttstamp':1625001560, 'evts':_evts(7), 'mu':52.5, 'step': 0},
    {'run':410000, 'lb':27, 'starttstamp':1625001620, 'evts':_evts(5), 'mu':53.5, 'step': 0},
    {'run':410000, 'lb':28, 'starttstamp':1625001680, 'evts':_evts(3), 'mu':54.5, 'step': 0},
    {'run':410000, 'lb':29, 'starttstamp':1625001740, 'evts':_evts(3), 'mu':55.5, 'step': 0},
    {'run':410000, 'lb':30, 'starttstamp':1625001800, 'evts':_evts(2), 'mu':56.5, 'step': 0},
    {'run':410000, 'lb':31, 'starttstamp':1625001860, 'evts':_evts(1), 'mu':57.5, 'step': 0},
    {'run':410000, 'lb':32, 'starttstamp':1625001920, 'evts':_evts(1), 'mu':58.5, 'step': 0},
    {'run':410000, 'lb':33, 'starttstamp':1625001980, 'evts':_evts(1), 'mu':59.5, 'step': 0},
    {'run':410000, 'lb':1001, 'starttstamp':1625060060, 'evts':_evts(1), 'mu':26.5, 'step':1},
    {'run':410000, 'lb':1002, 'starttstamp':1625060120, 'evts':_evts(1), 'mu':27.5, 'step':1},
    {'run':410000, 'lb':1003, 'starttstamp':1625060180, 'evts':_evts(1), 'mu':29.5, 'step':1},
    {'run':410000, 'lb':1004, 'starttstamp':1625060240, 'evts':_evts(1), 'mu':30.5, 'step':1},
    {'run':410000, 'lb':1005, 'starttstamp':1625060300, 'evts':_evts(1), 'mu':31.5, 'step':1},
    {'run':410000, 'lb':1006, 'starttstamp':1625060360, 'evts':_evts(1), 'mu':32.5, 'step':1},
    {'run':410000, 'lb':1007, 'starttstamp':1625060420, 'evts':_evts(1), 'mu':33.5, 'step':1},
    {'run':410000, 'lb':1008, 'starttstamp':1625060480, 'evts':_evts(2), 'mu':34.5, 'step':1},
    {'run':410000, 'lb':1009, 'starttstamp':1625060540, 'evts':_evts(3), 'mu':35.5, 'step':1},
    {'run':410000, 'lb':1010, 'starttstamp':1625060600, 'evts':_evts(4), 'mu':36.5, 'step':1},
    {'run':410000, 'lb':1011, 'starttstamp':1625060660, 'evts':_evts(6), 'mu':37.5, 'step':1},
    {'run':410000, 'lb':1012, 'starttstamp':1625060720, 'evts':_evts(10), 'mu':38.5, 'step':1},
    {'run':410000, 'lb':1013, 'starttstamp':1625060780, 'evts':_evts(12), 'mu':39.5, 'step':1},
    {'run':410000, 'lb':1014, 'starttstamp':1625060840, 'evts':_evts(15), 'mu':40.5, 'step':1},
    {'run':410000, 'lb':1015, 'starttstamp':1625060900, 'evts':_evts(16), 'mu':41.5, 'step':1},
    {'run':410000, 'lb':1016, 'starttstamp':1625060960, 'evts':_evts(19), 'mu':42.5, 'step':1},
    {'run':410000, 'lb':1017, 'starttstamp':1625061020, 'evts':_evts(21), 'mu':43.5, 'step':1},
    {'run':410000, 'lb':1018, 'starttstamp':1625061080, 'evts':_evts(23), 'mu':44.5, 'step':1},
    {'run':410000, 'lb':1019, 'starttstamp':1625061140, 'evts':_evts(25), 'mu':45.5, 'step':1},
    {'run':410000, 'lb':1020, 'starttstamp':1625061200, 'evts':_evts(22), 'mu':46.5, 'step':1},
    {'run':410000, 'lb':1021, 'starttstamp':1625061260, 'evts':_evts(20), 'mu':47.5, 'step':1},
    {'run':410000, 'lb':1022, 'starttstamp':1625061320, 'evts':_evts(18), 'mu':48.5, 'step':1},
    {'run':410000, 'lb':1023, 'starttstamp':1625061380, 'evts':_evts(14), 'mu':49.5, 'step':1},
    {'run':410000, 'lb':1024, 'starttstamp':1625061440, 'evts':_evts(12), 'mu':50.5, 'step':1},
    {'run':410000, 'lb':1025, 'starttstamp':1625061500, 'evts':_evts(8), 'mu':51.5, 'step':1},
    {'run':410000, 'lb':1026, 'starttstamp':1625061560, 'evts':_evts(7), 'mu':52.5, 'step':1},
    {'run':410000, 'lb':1027, 'starttstamp':1625061620, 'evts':_evts(5), 'mu':53.5, 'step':1},
    {'run':410000, 'lb':1028, 'starttstamp':1625061680, 'evts':_evts(3), 'mu':54.5, 'step':1},
    {'run':410000, 'lb':1029, 'starttstamp':1625061740, 'evts':_evts(3), 'mu':55.5, 'step':1},
    {'run':410000, 'lb':1030, 'starttstamp':1625061800, 'evts':_evts(2), 'mu':56.5, 'step':1},
    {'run':410000, 'lb':1031, 'starttstamp':1625061860, 'evts':_evts(1), 'mu':57.5, 'step':1},
    {'run':410000, 'lb':1032, 'starttstamp':1625061920, 'evts':_evts(1), 'mu':58.5, 'step':1},
    {'run':410000, 'lb':1033, 'starttstamp':1625061980, 'evts':_evts(1), 'mu':59.5, 'step':1},
    {'run':410000, 'lb':2001, 'starttstamp':1625120060, 'evts':_evts(1), 'mu':26.5, 'step':2},
    {'run':410000, 'lb':2002, 'starttstamp':1625120120, 'evts':_evts(1), 'mu':27.5, 'step':2},
    {'run':410000, 'lb':2003, 'starttstamp':1625120180, 'evts':_evts(1), 'mu':29.5, 'step':2},
    {'run':410000, 'lb':2004, 'starttstamp':1625120240, 'evts':_evts(1), 'mu':30.5, 'step':2},
    {'run':410000, 'lb':2005, 'starttstamp':1625120300, 'evts':_evts(1), 'mu':31.5, 'step':2},
    {'run':410000, 'lb':2006, 'starttstamp':1625120360, 'evts':_evts(1), 'mu':32.5, 'step':2},
    {'run':410000, 'lb':2007, 'starttstamp':1625120420, 'evts':_evts(1), 'mu':33.5, 'step':2},
    {'run':410000, 'lb':2008, 'starttstamp':1625120480, 'evts':_evts(2), 'mu':34.5, 'step':2},
    {'run':410000, 'lb':2009, 'starttstamp':1625120540, 'evts':_evts(3), 'mu':35.5, 'step':2},
    {'run':410000, 'lb':2010, 'starttstamp':1625120600, 'evts':_evts(4), 'mu':36.5, 'step':2},
    {'run':410000, 'lb':2011, 'starttstamp':1625120660, 'evts':_evts(6), 'mu':37.5, 'step':2},
    {'run':410000, 'lb':2012, 'starttstamp':1625120720, 'evts':_evts(10), 'mu':38.5, 'step':2},
    {'run':410000, 'lb':2013, 'starttstamp':1625120780, 'evts':_evts(12), 'mu':39.5, 'step':2},
    {'run':410000, 'lb':2014, 'starttstamp':1625120840, 'evts':_evts(15), 'mu':40.5, 'step':2},
    {'run':410000, 'lb':2015, 'starttstamp':1625120900, 'evts':_evts(16), 'mu':41.5, 'step':2},
    {'run':410000, 'lb':2016, 'starttstamp':1625120960, 'evts':_evts(19), 'mu':42.5, 'step':2},
    {'run':410000, 'lb':2017, 'starttstamp':1625121020, 'evts':_evts(21), 'mu':43.5, 'step':2},
    {'run':410000, 'lb':2018, 'starttstamp':1625121080, 'evts':_evts(23), 'mu':44.5, 'step':2},
    {'run':410000, 'lb':2019, 'starttstamp':1625121140, 'evts':_evts(25), 'mu':45.5, 'step':2},
    {'run':410000, 'lb':2020, 'starttstamp':1625121200, 'evts':_evts(22), 'mu':46.5, 'step':2},
    {'run':410000, 'lb':2021, 'starttstamp':1625121260, 'evts':_evts(20), 'mu':47.5, 'step':2},
    {'run':410000, 'lb':2022, 'starttstamp':1625121320, 'evts':_evts(18), 'mu':48.5, 'step':2},
    {'run':410000, 'lb':2023, 'starttstamp':1625121380, 'evts':_evts(14), 'mu':49.5, 'step':2},
    {'run':410000, 'lb':2024, 'starttstamp':1625121440, 'evts':_evts(12), 'mu':50.5, 'step':2},
    {'run':410000, 'lb':2025, 'starttstamp':1625121500, 'evts':_evts(8), 'mu':51.5, 'step':2},
    {'run':410000, 'lb':2026, 'starttstamp':1625121560, 'evts':_evts(7), 'mu':52.5, 'step':2},
    {'run':410000, 'lb':2027, 'starttstamp':1625121620, 'evts':_evts(5), 'mu':53.5, 'step':2},
    {'run':410000, 'lb':2028, 'starttstamp':1625121680, 'evts':_evts(3), 'mu':54.5, 'step':2},
    {'run':410000, 'lb':2029, 'starttstamp':1625121740, 'evts':_evts(3), 'mu':55.5, 'step':2},
    {'run':410000, 'lb':2030, 'starttstamp':1625121800, 'evts':_evts(2), 'mu':56.5, 'step':2},
    {'run':410000, 'lb':2031, 'starttstamp':1625121860, 'evts':_evts(1), 'mu':57.5, 'step':2},
    {'run':410000, 'lb':2032, 'starttstamp':1625121920, 'evts':_evts(1), 'mu':58.5, 'step':2},
    {'run':410000, 'lb':2033, 'starttstamp':1625121980, 'evts':_evts(1), 'mu':59.5, 'step':2},
    {'run':410000, 'lb':3001, 'starttstamp':1625180060, 'evts':_evts(1), 'mu':14.5, 'step':3},
    {'run':410000, 'lb':3002, 'starttstamp':1625180120, 'evts':_evts(1), 'mu':15.5, 'step':3},
    {'run':410000, 'lb':3003, 'starttstamp':1625180180, 'evts':_evts(2), 'mu':16.5, 'step':3},
    {'run':410000, 'lb':3004, 'starttstamp':1625180240, 'evts':_evts(4), 'mu':17.5, 'step':3},
    {'run':410000, 'lb':3005, 'starttstamp':1625180300, 'evts':_evts(6), 'mu':18.5, 'step':3},
    {'run':410000, 'lb':3006, 'starttstamp':1625180360, 'evts':_evts(9), 'mu':19.5, 'step':3},
    {'run':410000, 'lb':3007, 'starttstamp':1625180420, 'evts':_evts(12), 'mu':20.5, 'step':3},
    {'run':410000, 'lb':3008, 'starttstamp':1625180480, 'evts':_evts(16), 'mu':21.5, 'step':3},
    {'run':410000, 'lb':3009, 'starttstamp':1625180540, 'evts':_evts(20), 'mu':22.5, 'step':3},
    {'run':410000, 'lb':3010, 'starttstamp':1625180600, 'evts':_evts(27), 'mu':23.5, 'step':3},
    {'run':410000, 'lb':3011, 'starttstamp':1625180660, 'evts':_evts(34), 'mu':24.5, 'step':3},
    {'run':410000, 'lb':3012, 'starttstamp':1625180720, 'evts':_evts(40), 'mu':25.5, 'step':3},
    {'run':410000, 'lb':3013, 'starttstamp':1625180780, 'evts':_evts(43), 'mu':26.5, 'step':3},
    {'run':410000, 'lb':3014, 'starttstamp':1625180840, 'evts':_evts(47), 'mu':27.5, 'step':3},
    {'run':410000, 'lb':3015, 'starttstamp':1625180900, 'evts':_evts(54), 'mu':28.5, 'step':3},
    {'run':410000, 'lb':3016, 'starttstamp':1625180960, 'evts':_evts(53), 'mu':29.5, 'step':3},
    {'run':410000, 'lb':3017, 'starttstamp':1625181020, 'evts':_evts(56), 'mu':30.5, 'step':3},
    {'run':410000, 'lb':3018, 'starttstamp':1625181080, 'evts':_evts(58), 'mu':31.5, 'step':3},
    {'run':410000, 'lb':3019, 'starttstamp':1625181140, 'evts':_evts(59), 'mu':32.5, 'step':3},
    {'run':410000, 'lb':3020, 'starttstamp':1625181200, 'evts':_evts(60), 'mu':33.5, 'step':3},
    {'run':410000, 'lb':3021, 'starttstamp':1625181260, 'evts':_evts(56), 'mu':34.5, 'step':3},
    {'run':410000, 'lb':3022, 'starttstamp':1625181320, 'evts':_evts(53), 'mu':35.5, 'step':3},
    {'run':410000, 'lb':3023, 'starttstamp':1625181380, 'evts':_evts(50), 'mu':36.5, 'step':3},
    {'run':410000, 'lb':3024, 'starttstamp':1625181440, 'evts':_evts(44), 'mu':37.5, 'step':3},
    {'run':410000, 'lb':3025, 'starttstamp':1625181500, 'evts':_evts(33), 'mu':38.5, 'step':3},
    {'run':410000, 'lb':3026, 'starttstamp':1625181560, 'evts':_evts(30), 'mu':39.5, 'step':3},
    {'run':410000, 'lb':3027, 'starttstamp':1625181620, 'evts':_evts(26), 'mu':40.5, 'step':3},
    {'run':410000, 'lb':3028, 'starttstamp':1625181680, 'evts':_evts(28), 'mu':41.5, 'step':3},
    {'run':410000, 'lb':3029, 'starttstamp':1625181740, 'evts':_evts(25), 'mu':42.5, 'step':3},
    {'run':410000, 'lb':3030, 'starttstamp':1625181800, 'evts':_evts(23), 'mu':43.5, 'step':3},
    {'run':410000, 'lb':3031, 'starttstamp':1625181860, 'evts':_evts(20), 'mu':44.5, 'step':3},
    {'run':410000, 'lb':3032, 'starttstamp':1625181920, 'evts':_evts(17), 'mu':45.5, 'step':3},
    {'run':410000, 'lb':3033, 'starttstamp':1625181980, 'evts':_evts(20), 'mu':46.5, 'step':3},
    {'run':410000, 'lb':3034, 'starttstamp':1625182040, 'evts':_evts(20), 'mu':47.5, 'step':3},
    {'run':410000, 'lb':3035, 'starttstamp':1625182100, 'evts':_evts(20), 'mu':48.5, 'step':3},
    {'run':410000, 'lb':3036, 'starttstamp':1625182160, 'evts':_evts(23), 'mu':49.5, 'step':3},
    {'run':410000, 'lb':3037, 'starttstamp':1625182220, 'evts':_evts(18), 'mu':50.5, 'step':3},
    {'run':410000, 'lb':3038, 'starttstamp':1625182280, 'evts':_evts(19), 'mu':51.5, 'step':3},
    {'run':410000, 'lb':3039, 'starttstamp':1625182340, 'evts':_evts(11), 'mu':52.5, 'step':3},
    {'run':410000, 'lb':3040, 'starttstamp':1625182400, 'evts':_evts(8), 'mu':53.5, 'step':3},
    {'run':410000, 'lb':3041, 'starttstamp':1625182460, 'evts':_evts(7), 'mu':54.5, 'step':3},
    {'run':410000, 'lb':3042, 'starttstamp':1625182520, 'evts':_evts(2), 'mu':55.5, 'step':3},
    {'run':410000, 'lb':3043, 'starttstamp':1625182580, 'evts':_evts(1), 'mu':56.5, 'step':3},
    {'run':410000, 'lb':3044, 'starttstamp':1625182640, 'evts':_evts(2), 'mu':57.5, 'step':3},
    {'run':410000, 'lb':3045, 'starttstamp':1625182700, 'evts':_evts(1), 'mu':60.5, 'step':3},
    {'run':410000, 'lb':3046, 'starttstamp':1625182760, 'evts':_evts(1), 'mu':61.5, 'step':3},
    #--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker
