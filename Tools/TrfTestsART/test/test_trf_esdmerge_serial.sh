#!/bin/bash
#
# art-description: ESDMerge_tf.py serial
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena

ESDMerge_tf.py \
    --inputESDFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/DESDM_MCP.26614755._001203.pool.root.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/DESDM_MCP.26614755._001208.pool.root.1 \
    --postInclude="all:RecJobTransforms/UseFrontier.py" \
    --autoConfiguration="everything" \
    --conditionsTag="all:CONDBR2-BLKPA-RUN2-08" \
    --geometryVersion="all:ATLAS-R2-2016-01-00-01" \
    --runNumber="358031" \
    --outputESD_MRGFile="DESDM_MCP.pool.root" \
    --AMITag="p4756" 

echo "art-result: $? ESDMerge_tf_serial"
