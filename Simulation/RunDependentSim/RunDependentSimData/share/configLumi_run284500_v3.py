####################
## File configLumi_run219114.py: autogenerated configuration file from command
##/cvmfs/atlas-nightlies.cern.ch/repo/sw/nightlies/x86_64-slc6-gcc47-opt/devval/rel_1/AtlasSimulation/rel_1/InstallArea/share/bin/RunDepTaskMaker.py --externalDict={0.0:24.0,1.0:31.0, 2.0:38.0,3.0:45.0, 4.0:52.0,5.0:59.0, 6.0:65.0, 7.0:72.0, 8.0:79.0, 9.0:86.0, 10.0:93.0, 11.0:99.0, 12.0:105.0, 13.0:112.0, 14.0:119.0, 15.0:126.0, 16.0:133.0, 17.0:141.0, 18.0:147.0, 19.0:154.0, 20.0:161.0, 21.0:161.0, 22.0:161.0, 23.0:161.0, 24.0:161.0, 25.0:161.0, 26.0:161.0, 27.0:161.0, 28.0:161.0, 29.0:161.0, 30.0:161.0, 31.0:161.0, 32.0:161.0, 33.0:161.0, 34.0:161.0, 35.0:161.0, 36.0:161.0, 37.0:161.0, 38.0:161.0, 39.0:161.0} --lumimethod=EXTERNAL --nMC=1 --trigger=L1_MBTS_2 --outfile=configLumi_run219114.py run219114.xml
## Created on Tue May 20 18:26:16 2014
####################
#Run-dependent digi job configuration file.
#RunDependentSimData/OverrideRunLBLumiDigitConfig.py

# WARNING timestamps are derived ad hoc to put them outside run-1 or run-2 periods! 
# This is based on assumptions for beta*=60cm collisions.

#We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
     {'run':284500, 'lb':1, 'starttstamp':1446539185, 'dt':0.000, 'evts':_evts(22 ), 'mu':0.5, 'force_new':False},
     {'run':284500, 'lb':2, 'starttstamp':1446539245, 'dt':0.000, 'evts':_evts(22 ), 'mu':1.5, 'force_new':False},
     {'run':284500, 'lb':3, 'starttstamp':1446539305, 'dt':0.000, 'evts':_evts(22 ), 'mu':2.5, 'force_new':False},
     {'run':284500, 'lb':4, 'starttstamp':1446539365, 'dt':0.000, 'evts':_evts(22 ), 'mu':3.5, 'force_new':False},
     {'run':284500, 'lb':5, 'starttstamp':1446539425, 'dt':0.000, 'evts':_evts(22 ), 'mu':4.5, 'force_new':False},
     {'run':284500, 'lb':6, 'starttstamp':1446539485, 'dt':0.000, 'evts':_evts(22 ), 'mu':5.5, 'force_new':False},
     {'run':284500, 'lb':7, 'starttstamp':1446539545, 'dt':0.000, 'evts':_evts(22 ), 'mu':6.5, 'force_new':False},
     {'run':284500, 'lb':8, 'starttstamp':1446539605, 'dt':0.000, 'evts':_evts(38 ), 'mu':7.5, 'force_new':False},
     {'run':284500, 'lb':9, 'starttstamp':1446539665, 'dt':0.000, 'evts':_evts(169), 'mu':8.5, 'force_new':False},
    {'run':284500, 'lb':10, 'starttstamp':1446539725, 'dt':0.000, 'evts':_evts(472), 'mu':9.5, 'force_new':False},
    {'run':284500, 'lb':11, 'starttstamp':1446539785, 'dt':0.000, 'evts':_evts(729), 'mu':10.5, 'force_new':False},
    {'run':284500, 'lb':12, 'starttstamp':1446539845, 'dt':0.000, 'evts':_evts(788), 'mu':11.5, 'force_new':False},
    {'run':284500, 'lb':13, 'starttstamp':1446539905, 'dt':0.000, 'evts':_evts(804), 'mu':12.5, 'force_new':False},
    {'run':284500, 'lb':14, 'starttstamp':1446539965, 'dt':0.000, 'evts':_evts(559), 'mu':13.5, 'force_new':False},
    {'run':284500, 'lb':15, 'starttstamp':1446540025, 'dt':0.000, 'evts':_evts(386), 'mu':14.5, 'force_new':False},
    {'run':284500, 'lb':16, 'starttstamp':1446540085, 'dt':0.000, 'evts':_evts(218), 'mu':15.5, 'force_new':False},
    {'run':284500, 'lb':17, 'starttstamp':1446540145, 'dt':0.000, 'evts':_evts(108), 'mu':16.5, 'force_new':False},
    {'run':284500, 'lb':18, 'starttstamp':1446540205, 'dt':0.000, 'evts':_evts(68 ), 'mu':17.5, 'force_new':False},
    {'run':284500, 'lb':19, 'starttstamp':1446540265, 'dt':0.000, 'evts':_evts(39 ), 'mu':18.5, 'force_new':False},
    {'run':284500, 'lb':20, 'starttstamp':1446540325, 'dt':0.000, 'evts':_evts(28 ), 'mu':19.5, 'force_new':False},
    {'run':284500, 'lb':21, 'starttstamp':1446540385, 'dt':0.000, 'evts':_evts(22 ), 'mu':20.5, 'force_new':False},
    {'run':284500, 'lb':22, 'starttstamp':1446540445, 'dt':0.000, 'evts':_evts(22 ), 'mu':21.5, 'force_new':False},
    {'run':284500, 'lb':23, 'starttstamp':1446540505, 'dt':0.000, 'evts':_evts(22 ), 'mu':22.5, 'force_new':False},
    {'run':284500, 'lb':24, 'starttstamp':1446540565, 'dt':0.000, 'evts':_evts(22 ), 'mu':23.5, 'force_new':False},
    {'run':284500, 'lb':25, 'starttstamp':1446540625, 'dt':0.000, 'evts':_evts(22 ), 'mu':24.5, 'force_new':False},
    {'run':284500, 'lb':26, 'starttstamp':1446540685, 'dt':0.000, 'evts':_evts(22 ), 'mu':25.5, 'force_new':False},
    {'run':284500, 'lb':27, 'starttstamp':1446540745, 'dt':0.000, 'evts':_evts(22 ), 'mu':26.5, 'force_new':False},
    {'run':284500, 'lb':28, 'starttstamp':1446540805, 'dt':0.000, 'evts':_evts(22 ), 'mu':27.5, 'force_new':False},
    {'run':284500, 'lb':29, 'starttstamp':1446540865, 'dt':0.000, 'evts':_evts(22 ), 'mu':28.5, 'force_new':False},
    {'run':284500, 'lb':30, 'starttstamp':1446540925, 'dt':0.000, 'evts':_evts(22 ), 'mu':29.5, 'force_new':False},
    {'run':284500, 'lb':31, 'starttstamp':1446540985, 'dt':0.000, 'evts':_evts(22 ), 'mu':30.5, 'force_new':False},
    {'run':284500, 'lb':32, 'starttstamp':1446541045, 'dt':0.000, 'evts':_evts(22 ), 'mu':31.5, 'force_new':False},
    {'run':284500, 'lb':33, 'starttstamp':1446541105, 'dt':0.000, 'evts':_evts(22 ), 'mu':32.5, 'force_new':False},
    {'run':284500, 'lb':34, 'starttstamp':1446541165, 'dt':0.000, 'evts':_evts(22 ), 'mu':33.5, 'force_new':False},
    {'run':284500, 'lb':35, 'starttstamp':1446541225, 'dt':0.000, 'evts':_evts(22 ), 'mu':34.5, 'force_new':False},
    {'run':284500, 'lb':36, 'starttstamp':1446541285, 'dt':0.000, 'evts':_evts(22 ), 'mu':35.5, 'force_new':False},
    {'run':284500, 'lb':37, 'starttstamp':1446541345, 'dt':0.000, 'evts':_evts(22 ), 'mu':36.5, 'force_new':False},
    {'run':284500, 'lb':38, 'starttstamp':1446541405, 'dt':0.000, 'evts':_evts(22 ), 'mu':37.5, 'force_new':False},
    {'run':284500, 'lb':39, 'starttstamp':1446541465, 'dt':0.000, 'evts':_evts(22 ), 'mu':38.5, 'force_new':False},
    {'run':284500, 'lb':40, 'starttstamp':1446541525, 'dt':0.000, 'evts':_evts(22 ), 'mu':39.5, 'force_new':False}
    #--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker
