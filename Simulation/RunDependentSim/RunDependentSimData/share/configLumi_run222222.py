####################
## File configLumi_run219114.py: autogenerated configuration file from command
##/cvmfs/atlas-nightlies.cern.ch/repo/sw/nightlies/x86_64-slc6-gcc47-opt/devval/rel_1/AtlasSimulation/rel_1/InstallArea/share/bin/RunDepTaskMaker.py --externalDict={0.0:24.0,1.0:31.0, 2.0:38.0,3.0:45.0, 4.0:52.0,5.0:59.0, 6.0:65.0, 7.0:72.0, 8.0:79.0, 9.0:86.0, 10.0:93.0, 11.0:99.0, 12.0:105.0, 13.0:112.0, 14.0:119.0, 15.0:126.0, 16.0:133.0, 17.0:141.0, 18.0:147.0, 19.0:154.0, 20.0:161.0, 21.0:161.0, 22.0:161.0, 23.0:161.0, 24.0:161.0, 25.0:161.0, 26.0:161.0, 27.0:161.0, 28.0:161.0, 29.0:161.0, 30.0:161.0, 31.0:161.0, 32.0:161.0, 33.0:161.0, 34.0:161.0, 35.0:161.0, 36.0:161.0, 37.0:161.0, 38.0:161.0, 39.0:161.0} --lumimethod=EXTERNAL --nMC=1 --trigger=L1_MBTS_2 --outfile=configLumi_run219114.py run219114.xml
## Created on Tue May 20 18:26:16 2014
####################
#Run-dependent digi job configuration file.
#RunDependentSimData/OverrideRunLBLumiDigitConfig.py

# WARNING timestamps are from run 219114 + 15552000 to put them outside run-1 period! 

#We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
    {'run':222222, 'lb':1, 'starttstamp':1376009561, 'dt':0.000, 'evts':_evts(24), 'mu':0.000, 'force_new':False},
    {'run':222222, 'lb':2, 'starttstamp':1376009695, 'dt':0.000, 'evts':_evts(31), 'mu':1.000, 'force_new':False},
    {'run':222222, 'lb':3, 'starttstamp':1376009707, 'dt':0.000, 'evts':_evts(38), 'mu':2.000, 'force_new':False},
    {'run':222222, 'lb':4, 'starttstamp':1376009718, 'dt':0.000, 'evts':_evts(45), 'mu':3.000, 'force_new':False},
    {'run':222222, 'lb':5, 'starttstamp':1376009748, 'dt':0.000, 'evts':_evts(52), 'mu':4.000, 'force_new':False},
    {'run':222222, 'lb':6, 'starttstamp':1376009809, 'dt':0.000, 'evts':_evts(59), 'mu':5.000, 'force_new':False},
    {'run':222222, 'lb':7, 'starttstamp':1376009870, 'dt':0.000, 'evts':_evts(65), 'mu':6.000, 'force_new':False},
    {'run':222222, 'lb':8, 'starttstamp':1376009931, 'dt':0.000, 'evts':_evts(72), 'mu':7.000, 'force_new':False},
    {'run':222222, 'lb':9, 'starttstamp':1376009992, 'dt':0.000, 'evts':_evts(79), 'mu':8.000, 'force_new':False},
    {'run':222222, 'lb':10, 'starttstamp':1376010053, 'dt':0.000, 'evts':_evts(86), 'mu':9.000, 'force_new':False},
    {'run':222222, 'lb':11, 'starttstamp':1376010114, 'dt':0.000, 'evts':_evts(93), 'mu':10.000, 'force_new':False},
    {'run':222222, 'lb':12, 'starttstamp':1376010175, 'dt':0.000, 'evts':_evts(99), 'mu':11.000, 'force_new':False},
    {'run':222222, 'lb':13, 'starttstamp':1376010237, 'dt':0.000, 'evts':_evts(105), 'mu':12.000, 'force_new':False},
    {'run':222222, 'lb':14, 'starttstamp':1376010298, 'dt':0.000, 'evts':_evts(112), 'mu':13.000, 'force_new':False},
    {'run':222222, 'lb':15, 'starttstamp':1376010359, 'dt':0.000, 'evts':_evts(119), 'mu':14.000, 'force_new':False},
    {'run':222222, 'lb':16, 'starttstamp':1376010420, 'dt':0.000, 'evts':_evts(126), 'mu':15.000, 'force_new':False},
    {'run':222222, 'lb':17, 'starttstamp':1376010481, 'dt':0.000, 'evts':_evts(133), 'mu':16.000, 'force_new':False},
    {'run':222222, 'lb':18, 'starttstamp':1376010542, 'dt':0.000, 'evts':_evts(141), 'mu':17.000, 'force_new':False},
    {'run':222222, 'lb':19, 'starttstamp':1376010603, 'dt':0.000, 'evts':_evts(147), 'mu':18.000, 'force_new':False},
    {'run':222222, 'lb':20, 'starttstamp':1376010664, 'dt':0.000, 'evts':_evts(154), 'mu':19.000, 'force_new':False},
    {'run':222222, 'lb':21, 'starttstamp':1376010725, 'dt':0.000, 'evts':_evts(161), 'mu':20.000, 'force_new':False},
    {'run':222222, 'lb':22, 'starttstamp':1376010787, 'dt':0.000, 'evts':_evts(161), 'mu':21.000, 'force_new':False},
    {'run':222222, 'lb':23, 'starttstamp':1376010848, 'dt':0.000, 'evts':_evts(161), 'mu':22.000, 'force_new':False},
    {'run':222222, 'lb':24, 'starttstamp':1376010909, 'dt':0.000, 'evts':_evts(161), 'mu':23.000, 'force_new':False},
    {'run':222222, 'lb':25, 'starttstamp':1376010970, 'dt':0.000, 'evts':_evts(161), 'mu':24.000, 'force_new':False},
    {'run':222222, 'lb':26, 'starttstamp':1376011031, 'dt':0.000, 'evts':_evts(161), 'mu':25.000, 'force_new':False},
    {'run':222222, 'lb':27, 'starttstamp':1376011092, 'dt':0.000, 'evts':_evts(161), 'mu':26.000, 'force_new':False},
    {'run':222222, 'lb':28, 'starttstamp':1376011153, 'dt':0.000, 'evts':_evts(161), 'mu':27.000, 'force_new':False},
    {'run':222222, 'lb':29, 'starttstamp':1376011214, 'dt':0.000, 'evts':_evts(161), 'mu':28.000, 'force_new':False},
    {'run':222222, 'lb':30, 'starttstamp':1376011275, 'dt':0.000, 'evts':_evts(161), 'mu':29.000, 'force_new':False},
    {'run':222222, 'lb':31, 'starttstamp':1376011336, 'dt':0.000, 'evts':_evts(161), 'mu':30.000, 'force_new':False},
    {'run':222222, 'lb':32, 'starttstamp':1376011397, 'dt':0.000, 'evts':_evts(161), 'mu':31.000, 'force_new':False},
    {'run':222222, 'lb':33, 'starttstamp':1376011458, 'dt':0.000, 'evts':_evts(161), 'mu':32.000, 'force_new':False},
    {'run':222222, 'lb':34, 'starttstamp':1376011519, 'dt':0.000, 'evts':_evts(161), 'mu':33.000, 'force_new':False},
    {'run':222222, 'lb':35, 'starttstamp':1376011580, 'dt':0.000, 'evts':_evts(161), 'mu':34.000, 'force_new':False},
    {'run':222222, 'lb':36, 'starttstamp':1376011641, 'dt':0.000, 'evts':_evts(161), 'mu':35.000, 'force_new':False},
    {'run':222222, 'lb':37, 'starttstamp':1376011702, 'dt':0.000, 'evts':_evts(161), 'mu':36.000, 'force_new':False},
    {'run':222222, 'lb':38, 'starttstamp':1376011763, 'dt':0.000, 'evts':_evts(161), 'mu':37.000, 'force_new':False},
    {'run':222222, 'lb':39, 'starttstamp':1376011824, 'dt':0.000, 'evts':_evts(161), 'mu':38.000, 'force_new':False},
    {'run':222222, 'lb':40, 'starttstamp':1376011885, 'dt':0.000, 'evts':_evts(161), 'mu':39.000, 'force_new':False},
#--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker
