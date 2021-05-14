#!/bin/sh
#
# art-description: Run reco with a latest (September 2020) MC setup with pileup, and then run phys val on the output AOD.
# art-output: log.*
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena

export TRF_ECHO=True;
Reco_tf.py --AMI=r11976 --outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV/valid1/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3227/HITS.12560240._000287.pool.root.1 --inputHighPtMinbiasHitsFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/HITS.10501933._000008.pool.root.1 --inputLowPtMinbiasHitsFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/HITS.10501925._000027.pool.root.1 --maxEvents=500 --jobNumber=1 --imf False --athenaopts "RDOtoRDOTrigger:--imf --threads=1 --concurrent-events=1"
RES=$?
echo "art-result: $RES Reco"
if [ $RES -ne 0 ];then
return $RES
fi

Reco_tf.py --inputAODFile=myAOD.pool.root --outputNTUP_PHYSVALFile physval.root  --validationFlags 'doExample,doMET,doPFlow,doTau,doEgamma,doBtag,doZee,doJet,doTopoCluster,doMuon,doTrigMinBias,doTrigIDtrk,doTrigBphys,doTrigMET,doTrigJet,doTrigTau, doTrigEgamma,doTrigMuon,doTrigBjet,doTrigHLTResult'
RES=$?
echo "art-result: $RES Reco"


