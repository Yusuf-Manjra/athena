#This file is to run topocluster + pflow reconstruction on an ESD file produced in the release 22 mc16 campaign

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PFlowTests/mc16_13TeV/mc16_13TeV.361021.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1W.recon.ESD.e3569_s3170_r12310_r12253_r12310/ESD.23850840._000295.pool.root.1"]

doDumpProperties=True

#Turn on calotopocluster algorithms, such that we rebuild CaloTopoCluster
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

#Turn pflow on
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(True)

#Disable reco of trigger, egamma, muons
from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)

#Disable linking of PFO/FE to and from other objects - this won't work because e.g electrons already have links to PFO/FE and
#we cannot overwrite those.
from eflowRec.eflowRecFlags import jobproperties
jobproperties.eflowRecFlags.usePFEGammaPFOAssoc.set_Value_and_Lock(False)
jobproperties.eflowRecFlags.usePFFlowElementAssoc.set_Value_and_Lock(False)

#Disable thinning, which would mess up links for e.g taus
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False)
AODFlags.ThinNegativeEnergyCaloClusters.set_Value_and_Lock(False)

#Do not write out TAG file
rec.doWriteTAG.set_Value_and_Lock(False)
#Disable truth reco
rec.doTruth.set_Value_and_Lock(False)
#Don't build AOD CaloCell container
rec.doAODCaloCells.set_Value_and_Lock(False)
#Turn off xAODRingSetConfWriter
rec.doCaloRinger.set_Value_and_Lock(False)

#Disable VertexCnvAlg
from InDetRecExample.InDetJobProperties import jobproperties
jobproperties.InDetJobProperties.doxAOD.set_Value_and_Lock(False)
#Disable AllExecutedEvents
rec.doFileMetaData.set_Value_and_Lock(False)

athenaCommonFlags.EvtMax=100
#Rerun jet finding because constituents (pflow,calocluster) were rebuilt
UserAlgs = ["eflowRec/jetAlgs.py"]
#Rerun taus because we rebuild MET - will get errors if do not rebuild taus
from tauRec.tauRecFlags import tauFlags
tauFlags.isStandalone.set_Value_and_Lock(True)
UserAlgs += ["tauRec/tauRec_jobOptions.py"]
#Rebuild MET from the rebuilt PFO etc
UserAlgs+=["METReconstruction/METReconstruction_jobOptions.py"]
include ("RecExCommon/RecExCommon_topOptions.py")
