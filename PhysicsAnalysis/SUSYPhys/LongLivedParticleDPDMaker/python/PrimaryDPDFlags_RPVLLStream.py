# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


##=============================================================================
## Name:        PrimaryDPDFlags_RPVLL_Stream
## Authors:     Nick Barlow (Cambridge), C. Ohm (Stockholm)
## Created:     November 2009
##
## Based on:    PrimaryDPDFlags_PhotonJetStream by Karsten Koeneke (DESY)
##
## Description: Here, all necessary job flags for the RPV/LL ESD-based DPD
##              are defined.
##
##=============================================================================

__doc__ = """Here, all necessary job flags for the RPV/LL SUSY  DESD are defined."""

__version__ = "0.0.1"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_RPVLLStream(JobPropertyContainer):
    """ The Primary RPV/LL DPD flag/job property  container."""

jobproperties.add_Container(PrimaryDPDFlags_RPVLLStream)

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

from LongLivedParticleDPDMaker import DiLepFlags        # noqa: F401
from LongLivedParticleDPDMaker import DVFlags           # noqa: F401
from LongLivedParticleDPDMaker import KinkedTrackFlags  # noqa: F401
from LongLivedParticleDPDMaker import EmergingFlags     # noqa: F401
from LongLivedParticleDPDMaker import StoppedFlags      # noqa: F401
from LongLivedParticleDPDMaker import VH_DVFlags        # noqa: F401
from LongLivedParticleDPDMaker import QuirksFlags       # noqa: F401
from LongLivedParticleDPDMaker import HipsFlags         # noqa: F401
from LongLivedParticleDPDMaker import HNLFlags          # noqa: F401
from LongLivedParticleDPDMaker import HVFlags           # noqa: F401
from LongLivedParticleDPDMaker import SmpCaloIdFlags    # noqa: F401
from LongLivedParticleDPDMaker import SmpMsFlags        # noqa: F401
from LongLivedParticleDPDMaker import VHFlags           # noqa: F401
from LongLivedParticleDPDMaker import TauFlags          # noqa: F401

#===============================================================
# Set specific properties for each of the analysis selections
#===============================================================

class doDiLep(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
primRPVLLDESDM.add_JobProperty(doDiLep)
    
class doDV(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
primRPVLLDESDM.add_JobProperty(doDV)

class doKinkedTrack(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
primRPVLLDESDM.add_JobProperty(doKinkedTrack)

class doEmerging(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
primRPVLLDESDM.add_JobProperty(doEmerging)
    
class doStopped(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
primRPVLLDESDM.add_JobProperty(doStopped)

class doVH_DV(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
primRPVLLDESDM.add_JobProperty(doVH_DV)

class doQuirks(JobProperty):
    statusOn=True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
primRPVLLDESDM.add_JobProperty(doQuirks)

class doHips(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
primRPVLLDESDM.add_JobProperty(doHips)

class doHnl(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
primRPVLLDESDM.add_JobProperty(doHnl)

class doHV(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
primRPVLLDESDM.add_JobProperty(doHV)

class doSmpCaloId(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
primRPVLLDESDM.add_JobProperty(doSmpCaloId)

class doSmpMs(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
primRPVLLDESDM.add_JobProperty(doSmpMs)

class doVH(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
primRPVLLDESDM.add_JobProperty(doVH)

class doTaus(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
primRPVLLDESDM.add_JobProperty(doTaus)


class prescaleFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    applyPrescale = False
    prescaleFactor = 1.0
primRPVLLDESDM.add_JobProperty(prescaleFlags)
