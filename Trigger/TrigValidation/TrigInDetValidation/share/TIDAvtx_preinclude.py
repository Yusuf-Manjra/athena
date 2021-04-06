
from AthenaCommon.Logging import logging 
log = logging.getLogger("TrigInDetValidation")

log.info( "preinclude: TIDAvtx_preinclude.py" ) 

from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

getInDetTrigConfig("jet")._adaptiveVertex = True

log.info( "Setting adaptiveVertex: "+str(getInDetTrigConfig("jet").adaptiveVertex) )

