#!/bin/bash

# art-description: DirectIOART Athena Reco_tf.py inputFile:RDO protocol=DAVS
# art-type: grid
# art-output: *.pool.root
# art-include: master/Athena
# art-athena-mt: 8

set -e

unset ATHENA_CORE_NUMBER; Reco_tf.py --AMI q443 --steering "doRDO_TRIG" "doTRIGtoALL" --inputRDOFile davs://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc16_13TeV/90/96/RDO.11373415._000001.pool.root.1 --outputRDO_TRIGFile art.pool.root --maxEvents 25

echo "art-result: $? DirectIOART_Athena_RecoTF_inputRDO_protocol_DAVS"
