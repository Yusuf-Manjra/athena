#!/bin/sh
#
# art-description: evgen to reco
# art-type: grid

function findinput(){
  if [ ! -f $1 ]; then
    echo $2
  else
    echo $1
  fi
}

# INPUTS and backups
MY_EVGEN='MC15.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.13TeV.EVNT.pool.root'
BK_EVGEN='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FullChainTests/MC15.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.13TeV.EVNT.pool.root'

#
export JODIR=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest;export JOBOPTSEARCHPATH=$JODIR/share/DSID304xxx:$JOBOPTSEARCHPATH; Generate_tf.py --ecmEnergy 13000. --runNumber 304628 --firstEvent 1 --maxEvents 10 --randomSeed 54298752 --jobConfig MC15JobOptions/MC15.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.py --outputEVNTFile ${MY_EVGEN} --postExec 'TestHepMC.EffFailThreshold=0.50'


# check EVGEN output OK
MY_EVGEN=$(findinput MY_EVGEN BK_EVGEN)

#
Sim_tf.py --conditionsTag "default:OFLCOND-RUN12-SDR-19" --DataRunNumber 222525 --DBRelease "current" --geometryVersion "default:ATLAS-R2-2015-03-01-00_VALIDATION" --physicsList FTFP_BERT --postInclude "default:PyJobTransforms/UseFrontier.py" --preInclude "EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py" --runNumber=304628 --simulator MC12G4 --truthStrategy MC15aPlus --inputEVNTFile ${MY_EVGEN} --outputHITSFile MC15.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.13TeV.HITS.pool.root --maxEvents 10

#
Reco_tf.py --autoConfiguration everything --conditionsTag "default:OFLCOND-MC15c-SDR-09" --digiSteeringConf='StandardSignalOnlyTruth' --geometryVersion "default:ATLAS-R2-2015-03-01-00" --ignorePatterns Py:TrigConf2COOLLib.py --numberOfCavernBkg 0 --numberOfHighPtMinBias 0.12268057 --numberOfLowPtMinBias 39.8773194 --pileupFinalBunch 6 --postExec "all:CfgMgr.MessageSvc().setError+=[\"HepMcParticleLink\"]" "ESDtoAOD:fixedAttrib=[s if \"CONTAINER_SPLITLEVEL = \'99\'\" not in s else \"\" for s in svcMgr.AthenaPoolCnvSvc.PoolAttributes];svcMgr.AthenaPoolCnvSvc.PoolAttributes=fixedAttrib" --postInclude "default:RecJobTransforms/UseFrontier.py" --preExec "all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)" "RAWtoESD:from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False" "ESDtoAOD:TriggerFlags.AODEDMSet=\"AODSLIM\"" --preInclude "HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrainsMC15_2015_25ns_Config1.py,RunDependentSimData/configLumi_run284500_v2.py" "RDOtoRDOTrigger:RecExPers/RecoOutputMetadataList_jobOptions.py" --runNumber 304628 --steering "doRDO_TRIG" --triggerConfig "RDOtoRDOTrigger=MCRECO:DBF:TRIGGERDBMC:2046,20,56" --inputHighPtMinbiasHitsFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FullChainTests/HITS.05608152._002335.pool.root.1 --inputLowPtMinbiasHitsFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FullChainTests/HITS.05608147._000125.pool.root.1 --digiSeedOffset1 1 --digiSeedOffset2 2 --inputHITSFile MC15.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.13TeV.HITS.pool.root --outputAODFile MC15.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.13TeV.AOD.pool.root --jobNumber 1 --maxEvents 10


