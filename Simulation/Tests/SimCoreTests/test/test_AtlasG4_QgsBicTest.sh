#!/bin/sh
#
# art-description: Run simulation outside ISF, with alternative QGS_BIC physics list, reading in SU3 SUSY events, using 2015 geometry and conditions
# art-type: grid

AtlasG4_tf.py --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/susy_SU3-herwigpp-7000.evgen.pool.root' --outputHITSFile 'test.HITS.pool.root' --maxEvents '5' --skipEvents '0' --randomSeed '42' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'QGSP_BIC' --postInclude 'PyJobTransforms/UseFrontier.py'

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
