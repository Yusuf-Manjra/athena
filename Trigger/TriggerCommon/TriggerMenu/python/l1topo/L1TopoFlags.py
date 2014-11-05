# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
L1Topo specific flags
"""

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenu.menu.CommonSliceHelper import AllowedList
from AthenaCommon.Logging import logging

log = logging.getLogger('TriggerMenu.L1TopoFlags.py')

_flags = [] 

class algos(JobProperty):
    """Names of algos enabled in the selected L1Topo menu"""
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]

_flags.append(algos)  

class L1TopoNew(JobPropertyContainer):
    """ L1Topo flags used in menu generation """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(L1TopoNew)

for flag in _flags:
    TriggerFlags.L1TopoNew.add_JobProperty(flag)
del _flags

# make an alias
L1TopoFlags = TriggerFlags.L1TopoNew

#  LocalWords:  AllowedList


            
