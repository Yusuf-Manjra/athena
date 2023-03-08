from TrigInDetConfig.ConfigSettings import getInDetTrigConfig; 
from AthenaCommon.SystemOfUnits import GeV; 
getInDetTrigConfig('tauCore')._pTmin = 1*GeV; 
getInDetTrigConfig('tauIso')._pTmin = 1*GeV; 
getInDetTrigConfig('tauIso')._Xi2max = 9; 
getInDetTrigConfig('bjet')._Xi2max = 9;
getInDetTrigConfig('jetSuper')._zedHalfWidth = 150.0;

from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
flags.Trigger.InDetTracking.RoiZedWidthDefault=0.0
