#!/bin/sh
#
# art-description: Athena runs topoclustering from an ESD file
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 4

art.py createpoolfile

athena --threads=1 eflowRec/run_ESDStandardReco.py
echo "art-result: $?"
