#!/bin/sh
#
# art-description: ART Monitoring Tool for gamma Validation
#
# art-type: grid
# art-input: mc16_13TeV.423001.ParticleGun_single_photon_egammaET.recon.RDO.e3566_s3113_r9388
# art-input-nfiles: 40
# art-cores: 4
# art-include: 21.0/Athena
# art-include: master/Athena
# art-output: *.hist.root
# art-output: *.txt
# art-output: *.png

echo "ArtProcess: $ArtProcess"

case $ArtProcess in
    
    "start")
	echo "Starting"
	echo "List of files = " ${ArtInFile}
	;;

    "end")
	echo "Ending"
	
	echo "Merging AODs"
        echo "Unsetting ATHENA_NUM_PROC=${ATHENA_NUM_PROC} and ATHENA_PROC_NUMBER=${ATHENA_PROC_NUMBER}"
        unset  ATHENA_NUM_PROC
        unset  ATHENA_PROC_NUMBER

	AODMerge_tf.py --inputAODFile=art_core_*/Nightly_AOD_gamma.pool.root --outputAOD_MRGFile=Nightly_AOD_gamma.pool.root
	
	echo  "art-result: $? AODMerge"

	set +e

	checkFile.py Nightly_AOD_gamma.pool.root > checkFile_Nightly_gamma.txt 
	checkxAOD.py Nightly_AOD_gamma.pool.root > checkxAOD_Nightly_gamma.txt 

	echo  "art-result: $? checks_files"

	get_files -jo egamma_art_checker_joboptions.py
	athena -c "particleType='gamma'" egamma_art_checker_joboptions.py

	echo  "art-result: $? athena_job"

	EgammaARTmonitoring_plotsMaker.py /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/egammaValidation/Baseline_Files/rootFiles/Baseline-monitoring_gamma.hist.root Nightly-monitoring_gamma.hist.root gamma

	echo  "art-result: $? final_comparison"

	;;

    *)
	echo "Test $ArtProcess"

	mkdir "art_core_${ArtProcess}"
	cd "art_core_${ArtProcess}"

	IFS=',' read -r -a file <<< "${ArtInFile}"
        file=${file[${ArtProcess}]}
	x="../$file"
		
	echo "Unsetting ATHENA_NUM_PROC=${ATHENA_NUM_PROC} and ATHENA_PROC_NUMBER=${ATHENA_PROC_NUMBER}"
	unset  ATHENA_NUM_PROC
	unset  ATHENA_PROC_NUMBER
	
	Reco_tf.py --inputRDOFile=$x --outputAODFile=Nightly_AOD_gamma.pool.root --maxEvents=5 --autoConfiguration="everything" --preExec="rec.doTrigger=False; rec.doMuon=False; rec.doTau=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; "
	
	echo  "art-result: $? reconstruction"

	;;
esac
