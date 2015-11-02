# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Flag class for Pileup Overlay building

usage:
from OverlayCommonAlgs.OverlayFlags import OverlayFlags
overlayflags.EventIDTextFile= runArgs.inputTXT_EVENTIDFile[0]

"""

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

from AthenaCommon.Logging import logging

overlayflaglog = logging.getLogger('Overlay_Flags')

class EventIDTextFile(JobProperty):
    """Name of the Event ID Text file"""
    statusOn=True
    allowedTypes=['str']
    StoredValue = 'events.txt'

class doSignal(JobProperty):
    """Signal"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue = False
   
class doBkg(JobProperty):
    """Background"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue = False
 
## Definition and registration of the simulation flag container

class Overlay_Flags(JobPropertyContainer):
    """ The global Overlay flag/job property container.
    """
## Register and populate the Overlay_Flags container

jobproperties.add_Container(Overlay_Flags)
for jpname in dir():
    jp = eval(jpname)
    import inspect
    if inspect.isclass(jp):
        if issubclass(jp, JobProperty) and jp is not JobProperty:
            overlayflaglog.debug("Adding OverlayFlag '%s' to Overlay_Flags container" % jpname)
            jobproperties.Overlay_Flags.add_JobProperty(jp)

## Short-cut alias (standard naming)
overlayFlags = jobproperties.Overlay_Flags


  

