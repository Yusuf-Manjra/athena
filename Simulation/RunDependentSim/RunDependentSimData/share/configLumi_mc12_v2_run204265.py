####################
## File configLumi_204265.py: autogenerated configuration file from command
##/cvmfs/atlas-nightlies.cern.ch/repo/sw/nightlies/x86_64-slc6-gcc47-opt/devval/rel_1/AtlasSimulation/rel_1/InstallArea/share/bin/RunDepTaskMaker.py --externalDict={0.0: 5.0, 1.0: 5.0, 2.0: 5.0, 3.0: 5.0, 4.0: 5.0, 5.0: 5.0, 6.0: 5.0, 7.0: 18.0, 8.0: 47.0, 9.0: 103.0, 10.0: 147.0, 11.0: 205.0, 12.0: 248.0, 13.0: 276.0, 14.0: 285.0, 15.0: 303.0, 16.0: 303.0, 17.0: 303.0, 18.0: 303.0, 19.0: 293.0, 20.0: 282.0, 21.0: 275.0, 22.0: 249.0, 23.0: 236.0, 24.0: 210.0, 25.0: 185.0, 26.0: 156.0, 27.0: 141.0, 28.0: 105.0, 29.0: 88.0, 30.0: 65.0, 31.0: 51.0, 32.0: 31.0, 33.0: 18.0, 34.0: 16.0, 35.0: 9.0, 36.0: 6.0, 37.0: 2.0, 38.0: 2.0, 39.0: 2.0, 40.0: 2.0} --lumimethod=EXTERNAL --nMC=1 --trigger=L1_MBTS_2 --outfile=configLumi_204265.py run204265.xml
## Created on Thu Mar 27 22:29:51 2014
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
   {'run':204265, 'lb':134, 'starttstamp':1338613997, 'dt':0.000, 'evts':_evts(5), 'mu':0.00001, 'force_new':False},
   {'run':204265, 'lb':135, 'starttstamp':1338614007, 'dt':0.000, 'evts':_evts(5), 'mu':1.000, 'force_new':False},
   {'run':204265, 'lb':136, 'starttstamp':1338614068, 'dt':0.000, 'evts':_evts(5), 'mu':2.000, 'force_new':False},
   {'run':204265, 'lb':137, 'starttstamp':1338614129, 'dt':0.000, 'evts':_evts(5), 'mu':3.000, 'force_new':False},
   {'run':204265, 'lb':138, 'starttstamp':1338614140, 'dt':0.000, 'evts':_evts(5), 'mu':4.000, 'force_new':False},
   {'run':204265, 'lb':139, 'starttstamp':1338614201, 'dt':0.000, 'evts':_evts(5), 'mu':5.000, 'force_new':False},
   {'run':204265, 'lb':140, 'starttstamp':1338614227, 'dt':0.000, 'evts':_evts(5), 'mu':6.000, 'force_new':False},
   {'run':204265, 'lb':141, 'starttstamp':1338614288, 'dt':0.000, 'evts':_evts(18), 'mu':7.000, 'force_new':False},
   {'run':204265, 'lb':142, 'starttstamp':1338614349, 'dt':0.000, 'evts':_evts(47), 'mu':8.000, 'force_new':False},
   {'run':204265, 'lb':143, 'starttstamp':1338614410, 'dt':0.000, 'evts':_evts(103), 'mu':9.000, 'force_new':False},
   {'run':204265, 'lb':144, 'starttstamp':1338614437, 'dt':0.000, 'evts':_evts(147), 'mu':10.000, 'force_new':False},
   {'run':204265, 'lb':145, 'starttstamp':1338614497, 'dt':0.000, 'evts':_evts(205), 'mu':11.000, 'force_new':False},
   {'run':204265, 'lb':146, 'starttstamp':1338614558, 'dt':0.000, 'evts':_evts(248), 'mu':12.000, 'force_new':False},
   {'run':204265, 'lb':147, 'starttstamp':1338614619, 'dt':0.000, 'evts':_evts(276), 'mu':13.000, 'force_new':False},
   {'run':204265, 'lb':148, 'starttstamp':1338614680, 'dt':0.000, 'evts':_evts(285), 'mu':14.000, 'force_new':False},
   {'run':204265, 'lb':149, 'starttstamp':1338614741, 'dt':0.000, 'evts':_evts(303), 'mu':15.000, 'force_new':False},
   {'run':204265, 'lb':150, 'starttstamp':1338614802, 'dt':0.000, 'evts':_evts(303), 'mu':16.000, 'force_new':False},
   {'run':204265, 'lb':151, 'starttstamp':1338614863, 'dt':0.000, 'evts':_evts(303), 'mu':17.000, 'force_new':False},
   {'run':204265, 'lb':152, 'starttstamp':1338614874, 'dt':0.000, 'evts':_evts(303), 'mu':18.000, 'force_new':False},
   {'run':204265, 'lb':153, 'starttstamp':1338614935, 'dt':0.000, 'evts':_evts(293), 'mu':19.000, 'force_new':False},
   {'run':204265, 'lb':154, 'starttstamp':1338614996, 'dt':0.000, 'evts':_evts(282), 'mu':20.000, 'force_new':False},
   {'run':204265, 'lb':155, 'starttstamp':1338615057, 'dt':0.000, 'evts':_evts(275), 'mu':21.000, 'force_new':False},
   {'run':204265, 'lb':156, 'starttstamp':1338615118, 'dt':0.000, 'evts':_evts(249), 'mu':22.000, 'force_new':False},
   {'run':204265, 'lb':157, 'starttstamp':1338615179, 'dt':0.000, 'evts':_evts(236), 'mu':23.000, 'force_new':False},
   {'run':204265, 'lb':158, 'starttstamp':1338615241, 'dt':0.000, 'evts':_evts(210), 'mu':24.000, 'force_new':False},
   {'run':204265, 'lb':159, 'starttstamp':1338615302, 'dt':0.000, 'evts':_evts(185), 'mu':25.000, 'force_new':False},
   {'run':204265, 'lb':160, 'starttstamp':1338615362, 'dt':0.000, 'evts':_evts(156), 'mu':26.000, 'force_new':False},
   {'run':204265, 'lb':161, 'starttstamp':1338615423, 'dt':0.000, 'evts':_evts(141), 'mu':27.000, 'force_new':False},
   {'run':204265, 'lb':162, 'starttstamp':1338615486, 'dt':0.000, 'evts':_evts(105), 'mu':28.000, 'force_new':False},
   {'run':204265, 'lb':163, 'starttstamp':1338615545, 'dt':0.000, 'evts':_evts(88), 'mu':29.000, 'force_new':False},
   {'run':204265, 'lb':164, 'starttstamp':1338615606, 'dt':0.000, 'evts':_evts(65), 'mu':30.000, 'force_new':False},
   {'run':204265, 'lb':165, 'starttstamp':1338615667, 'dt':0.000, 'evts':_evts(51), 'mu':31.000, 'force_new':False},
   {'run':204265, 'lb':166, 'starttstamp':1338615728, 'dt':0.000, 'evts':_evts(31), 'mu':32.000, 'force_new':False},
   {'run':204265, 'lb':167, 'starttstamp':1338615790, 'dt':0.000, 'evts':_evts(18), 'mu':33.000, 'force_new':False},
   {'run':204265, 'lb':168, 'starttstamp':1338615851, 'dt':0.000, 'evts':_evts(16), 'mu':34.000, 'force_new':False},
   {'run':204265, 'lb':169, 'starttstamp':1338615911, 'dt':0.000, 'evts':_evts(9), 'mu':35.000, 'force_new':False},
   {'run':204265, 'lb':170, 'starttstamp':1338615972, 'dt':0.000, 'evts':_evts(6), 'mu':36.000, 'force_new':False},
   {'run':204265, 'lb':171, 'starttstamp':1338616033, 'dt':0.000, 'evts':_evts(2), 'mu':37.000, 'force_new':False},
   {'run':204265, 'lb':172, 'starttstamp':1338616094, 'dt':0.000, 'evts':_evts(2), 'mu':38.000, 'force_new':False},
   {'run':204265, 'lb':173, 'starttstamp':1338616151, 'dt':0.000, 'evts':_evts(2), 'mu':39.000, 'force_new':False},
   {'run':204265, 'lb':174, 'starttstamp':1338616211, 'dt':0.000, 'evts':_evts(2), 'mu':40.000, 'force_new':False},
#--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker
