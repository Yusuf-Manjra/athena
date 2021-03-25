#!/bin/sh
#
# art-description: Reco_tf runs on 2020 IDCosmic stream, without trigger.
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena
Reco_tf.py --athenaopts="--threads=8" --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data20_idcomm/data20_idcomm.00375727.physics_IDCosmic.merge.RAW._lb0876._SFO-ALL._0001.1 --maxEvents=10 --preExec="rec.doTrigger=False;rec.doBTagging=False;" --outputESDFile=myESD.pool.root --outputAODFile=myAOD.pool.root --conditionsTag=CONDBR2-BLKPA-RUN2-01 --geometryVersion=ATLAS-R2-2016-01-00-01

