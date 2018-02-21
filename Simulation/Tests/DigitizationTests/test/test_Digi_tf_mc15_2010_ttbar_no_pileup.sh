#!/bin/sh
#
# art-description: Run digitization of an MC15 ttbar sample with 2010 geometry and conditions, without pile-up
# art-type: grid


Digi_tf.py --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ttbar.ATLAS-R1-2010-02-00-00.HITS.pool.root --outputRDOFile mc15_2010_ttbar.RDO.pool.root --maxEvents 25 --skipEvents 0  --digiSeedOffset1=11 --digiSeedOffset2=22 --geometryVersion ATLAS-R1-2010-02-00-00 --conditionsTag OFLCOND-RUN12-SDR-22 --DataRunNumber 155697 --postInclude default:PyJobTransforms/UseFrontier.py

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references and add DCube tests
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
