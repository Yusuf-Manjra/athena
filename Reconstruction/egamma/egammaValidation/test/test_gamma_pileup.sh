#!/bin/sh
#
# art-description: ART Monitoring Tool for gamma Validation
#
# art-type: grid
# art-input: mc16_13TeV.423001.ParticleGun_single_photon_egammaET.recon.RDO.e3566_s3113_r10470
# art-input-nfiles: 8
# art-cores: 4
# art-include: 21.0/Athena
# art-include: master/Athena
# art-output: *.hist.root
# art-output: *.txt
# art-output: *.png
# art-output: log.*
# art-output: dcube

echo "ArtProcess: $ArtProcess"

case $ArtProcess in
    
    "start")
	echo "Starting"
	echo "List of files = " ${ArtInFile}
	;;

    "end")
	echo "Ending"
	
	echo "Merging AODs"
        echo "Unsetting ATHENA_NUM_PROC=${ATHENA_NUM_PROC}"
        unset  ATHENA_NUM_PROC

        AODMerge_tf.py --inputAODFile=art_core_*/Nightly_AOD_gamma.pool.root --outputAOD_MRGFile=Nightly_AOD_gamma.pool.root --preExec "from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doTrigger=False" --postInclude "all:egammaValidation/egammaArtCaloCalPostInclude.py" 

	echo  "art-result: $? AODMerge"

	set +e

	checkFile.py Nightly_AOD_gamma.pool.root > checkFile_Nightly_gamma.txt 

	echo  "art-result: $? checks_files"

	get_files -jo egamma_art_checker_joboptions.py
	athena -c "particleType='gamma'" egamma_art_checker_joboptions.py

	echo  "art-result: $? athena_job"

	EgammaARTmonitoring_plotsMaker.py /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/rootFiles/Baseline-monitoring_gamma_pileup.hist.root Nightly-monitoring_gamma.hist.root gamma

	echo  "art-result: $? final_comparison"

	$ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py -p -x dcube -c /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/DCube_Config/gamma.xml -r /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Nightly_Files/Nightly-monitoring_gamma_pileup.hist.root  Nightly-monitoring_gamma.hist.root
	echo  "art-result: $? plot"

	;;

    *)
	echo "Test $ArtProcess"

	mkdir "art_core_${ArtProcess}"
	cd "art_core_${ArtProcess}"

	IFS=',' read -r -a file <<< "${ArtInFile}"
        file=${file[${ArtProcess}]}
	x="../$file"
		
	echo "Unsetting ATHENA_NUM_PROC=${ATHENA_NUM_PROC}"
	unset  ATHENA_NUM_PROC
	
	Reco_tf.py --inputRDOFile=$x --outputAODFile=Nightly_AOD_gamma.pool.root --maxEvents=2000 --autoConfiguration="everything" --preExec="from ParticleBuilderOptions.AODFlags import AODFlags; AODFlags.ThinGeantTruth.set_Value_and_Lock(False);AODFlags.egammaTrackSlimmer.set_Value_and_Lock(False);AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False);AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False); rec.doTrigger=False; rec.doMuon=False ; rec.doTau=False ; rec.doBTagging=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; recAlgs.doTrigger=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; "  --postInclude "ESDtoAOD:egammaValidation/egammaArtCaloCalPostInclude.py" "POOLMergeAthenaMPAOD0:egammaValidation/egammaArtCaloCalPostInclude.py" 
	
	echo  "art-result: $? reconstruction"

	;;
esac
