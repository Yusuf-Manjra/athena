####################
## MC12 test file
## File configLumi_run195847.py: autogenerated configuration file from command
##/afs/cern.ch/atlas/software/releases/17.0.6/AtlasSimulation/17.0.6/InstallArea/share/bin/RunDepTaskMaker.py --externalDict={0.0: 2.0, 1.0: 2.0, 2.0: 2.0, 3.0: 2.0, 4.0: 2.0, 5.0: 2.0, 6.0: 2.0, 7.0: 11.0, 8.0: 33.0, 9.0: 55.0, 10.0: 78.0, 11.0: 100.0, 12.0: 122.0, 13.0: 144.0, 14.0: 166.0, 15.0: 189.0, 16.0: 211.0, 17.0: 220.0, 18.0: 220.0, 19.0: 222.0, 20.0: 222.0, 21.0: 222.0, 22.0: 222.0, 23.0: 222.0, 24.0: 222.0, 25.0: 222.0, 26.0: 222.0, 27.0: 222.0, 28.0: 220.0, 29.0: 220.0, 30.0: 200.0, 31.0: 178.0, 32.0: 155.0, 33.0: 133.0, 34.0: 111.0, 35.0: 89.0, 36.0: 67.0, 37.0: 44.0, 38.0: 22.0} --lumimethod=EXTERNAL --nMC=1 --trigger=L1_MBTS_2 --outfile=configLumi_run195847.py run195847.xml
## Created on Tue Feb  7 04:11:21 2012
####################
#Run-dependent digi job configuration file.
#RunDependentSimData/OverrideRunLBLumiDigitConfig.py

#We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
   {'run':195847, 'lb':1, 'starttstamp':1328039085, 'dt':0.000, 'evts':_evts(2), 'mu':0.000, 'force_new':False},
   {'run':195847, 'lb':2, 'starttstamp':1328039152, 'dt':0.000, 'evts':_evts(2), 'mu':1.000, 'force_new':False},
   {'run':195847, 'lb':3, 'starttstamp':1328039213, 'dt':0.000, 'evts':_evts(2), 'mu':2.000, 'force_new':False},
   {'run':195847, 'lb':4, 'starttstamp':1328039274, 'dt':0.000, 'evts':_evts(2), 'mu':3.000, 'force_new':False},
   {'run':195847, 'lb':5, 'starttstamp':1328039335, 'dt':0.000, 'evts':_evts(2), 'mu':4.000, 'force_new':False},
   {'run':195847, 'lb':6, 'starttstamp':1328039396, 'dt':0.000, 'evts':_evts(2), 'mu':5.000, 'force_new':False},
   {'run':195847, 'lb':7, 'starttstamp':1328039457, 'dt':0.000, 'evts':_evts(2), 'mu':6.000, 'force_new':False},
   {'run':195847, 'lb':8, 'starttstamp':1328039518, 'dt':0.000, 'evts':_evts(11), 'mu':7.000, 'force_new':False},
   {'run':195847, 'lb':9, 'starttstamp':1328039579, 'dt':0.000, 'evts':_evts(33), 'mu':8.000, 'force_new':False},
   {'run':195847, 'lb':10, 'starttstamp':1328039640, 'dt':0.000, 'evts':_evts(55), 'mu':9.000, 'force_new':False},
   {'run':195847, 'lb':11, 'starttstamp':1328039701, 'dt':0.000, 'evts':_evts(78), 'mu':10.000, 'force_new':False},
   {'run':195847, 'lb':12, 'starttstamp':1328039762, 'dt':0.000, 'evts':_evts(100), 'mu':11.000, 'force_new':False},
   {'run':195847, 'lb':13, 'starttstamp':1328039823, 'dt':0.000, 'evts':_evts(122), 'mu':12.000, 'force_new':False},
   {'run':195847, 'lb':14, 'starttstamp':1328039884, 'dt':0.000, 'evts':_evts(144), 'mu':13.000, 'force_new':False},
   {'run':195847, 'lb':15, 'starttstamp':1328039945, 'dt':0.000, 'evts':_evts(166), 'mu':14.000, 'force_new':False},
   {'run':195847, 'lb':16, 'starttstamp':1328040006, 'dt':0.000, 'evts':_evts(189), 'mu':15.000, 'force_new':False},
   {'run':195847, 'lb':17, 'starttstamp':1328040067, 'dt':0.000, 'evts':_evts(211), 'mu':16.000, 'force_new':False},
   {'run':195847, 'lb':18, 'starttstamp':1328040128, 'dt':0.000, 'evts':_evts(220), 'mu':17.000, 'force_new':False},
   {'run':195847, 'lb':19, 'starttstamp':1328040189, 'dt':0.000, 'evts':_evts(220), 'mu':18.000, 'force_new':False},
   {'run':195847, 'lb':20, 'starttstamp':1328040250, 'dt':0.000, 'evts':_evts(222), 'mu':19.000, 'force_new':False},
   {'run':195847, 'lb':21, 'starttstamp':1328040311, 'dt':0.000, 'evts':_evts(222), 'mu':20.000, 'force_new':False},
   {'run':195847, 'lb':22, 'starttstamp':1328040372, 'dt':0.000, 'evts':_evts(222), 'mu':21.000, 'force_new':False},
   {'run':195847, 'lb':23, 'starttstamp':1328040433, 'dt':0.000, 'evts':_evts(222), 'mu':22.000, 'force_new':False},
   {'run':195847, 'lb':24, 'starttstamp':1328040494, 'dt':0.000, 'evts':_evts(222), 'mu':23.000, 'force_new':False},
   {'run':195847, 'lb':25, 'starttstamp':1328040555, 'dt':0.000, 'evts':_evts(222), 'mu':24.000, 'force_new':False},
   {'run':195847, 'lb':26, 'starttstamp':1328040616, 'dt':0.000, 'evts':_evts(222), 'mu':25.000, 'force_new':False},
   {'run':195847, 'lb':27, 'starttstamp':1328040677, 'dt':0.000, 'evts':_evts(222), 'mu':26.000, 'force_new':False},
   {'run':195847, 'lb':28, 'starttstamp':1328040738, 'dt':0.000, 'evts':_evts(222), 'mu':27.000, 'force_new':False},
   {'run':195847, 'lb':29, 'starttstamp':1328040799, 'dt':0.000, 'evts':_evts(220), 'mu':28.000, 'force_new':False},
   {'run':195847, 'lb':30, 'starttstamp':1328040860, 'dt':0.000, 'evts':_evts(220), 'mu':29.000, 'force_new':False},
   {'run':195847, 'lb':31, 'starttstamp':1328040921, 'dt':0.000, 'evts':_evts(200), 'mu':30.000, 'force_new':False},
   {'run':195847, 'lb':32, 'starttstamp':1328040982, 'dt':0.000, 'evts':_evts(178), 'mu':31.000, 'force_new':False},
   {'run':195847, 'lb':33, 'starttstamp':1328041043, 'dt':0.000, 'evts':_evts(155), 'mu':32.000, 'force_new':False},
   {'run':195847, 'lb':34, 'starttstamp':1328041104, 'dt':0.000, 'evts':_evts(133), 'mu':33.000, 'force_new':False},
   {'run':195847, 'lb':35, 'starttstamp':1328041165, 'dt':0.000, 'evts':_evts(111), 'mu':34.000, 'force_new':False},
   {'run':195847, 'lb':36, 'starttstamp':1328041226, 'dt':0.000, 'evts':_evts(89), 'mu':35.000, 'force_new':False},
   {'run':195847, 'lb':37, 'starttstamp':1328041287, 'dt':0.000, 'evts':_evts(67), 'mu':36.000, 'force_new':False},
   {'run':195847, 'lb':38, 'starttstamp':1328041348, 'dt':0.000, 'evts':_evts(44), 'mu':37.000, 'force_new':False},
   {'run':195847, 'lb':39, 'starttstamp':1328041409, 'dt':0.000, 'evts':_evts(22), 'mu':38.000, 'force_new':False},
#--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker
