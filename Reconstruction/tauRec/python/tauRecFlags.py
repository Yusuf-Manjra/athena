# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file tauRecFlags.py
#
#@brief define some tauRec flags 
################################################################################

#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units


class Enabled(JobProperty):
    """ if all tau algorithm to be enabled
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doTauRec(JobProperty):
    """ if TauRec to be enabled
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    def get_Value(self):
        return self.statusOn and self.StoredValue and jobproperties.tauRecFlags.Enabled()

class tauRecSeedMaxEta(JobProperty):
    """ max allowed abs_eta of jet seed
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=2.5
    
class doRunTauDiscriminant(JobProperty):
    """ switch for TauDiscriminant running
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class useVertexBasedConvFinder(JobProperty):
    """ switch for PhotonConversionVertex.cxx/h conversion veto
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class useNewPIDBasedConvFinder(JobProperty):
    """ switch for TauConversionTagger.cxx/h conversion veto
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doPanTau(JobProperty):
    """ if pantau should run after tauRec
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doPi0(JobProperty):
    """ switch on Pi0 Finder
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class pi0EtCuts(JobProperty):
    """ Set |eta| dependent Et requirement for pi0 tag
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[2800.*Units.MeV,2700.*Units.MeV,2500.*Units.MeV,2900.*Units.MeV,2700.*Units.MeV]

class pi0MVACuts_1prong(JobProperty):
    """ Set |eta| dependent MVA requirement for pi0 tag for 1-prong taus
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[0.45,0.37,0.39,0.40,0.38]

class pi0MVACuts_mprong(JobProperty):
    """ Set |eta| dependent MVA requirement for pi0 tag for multi-prong taus
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[0.73,0.69,0.58,0.69,0.67]

class shotPtCut_1Photon(JobProperty):
    """ Set |eta| dependent pt requirement for counting one photon in a shot
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[430.*Units.MeV,300.*Units.MeV,9999999.*Units.MeV,330.*Units.MeV,350.*Units.MeV]

class shotPtCut_2Photons(JobProperty):
    """ Set |eta| dependent pt requirement for counting two photons in a shot
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[10000.*Units.MeV,10000.*Units.MeV,9999999.*Units.MeV,10000.*Units.MeV,10000.*Units.MeV]

class useOldVertexFitterAPI(JobProperty):
    """ use the old (AOD-style) API of the AdaptiveVertexFitter.
    The AdaptiveVertexFitter is used for finding the tau decay vertex (aka. secondary vertex) and called in TauVertexVariables.
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


# Defines a sub-container for the algorithm switches
class tauRecFlags(JobPropertyContainer):
    """ tau information """

# add the tau flags container to the top container 
jobproperties.add_Container(tauRecFlags)

# I want always the following flags in the Rec container  
_list_tau=[Enabled,doTauRec,tauRecSeedMaxEta,doRunTauDiscriminant,useVertexBasedConvFinder,useNewPIDBasedConvFinder,doPanTau,doPi0,pi0EtCuts,pi0MVACuts_1prong,pi0MVACuts_mprong,shotPtCut_1Photon,shotPtCut_2Photons,useOldVertexFitterAPI]
for j in _list_tau: 
    jobproperties.tauRecFlags.add_JobProperty(j)
del _list_tau



#################################################
#### AOD flags ##################################
#                                               #
# only for re-running tau reco on AODs          #
# !not for normal reconstruction!               #
#################################################

class doUpdate(JobProperty):
    """ update the tau containers (if running in AODmode)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class tauRecAODFlags(JobPropertyContainer):
    """ tau information if re-running on AODs """

# add the tau flags container to the top container 
jobproperties.add_Container(tauRecAODFlags)

# I want always the following flags in the Rec container  
_list_tau=[doUpdate]
for j in _list_tau: 
    jobproperties.tauRecAODFlags.add_JobProperty(j)
del _list_tau


tauFlags=jobproperties.tauRecFlags
#=======================================================================
    
