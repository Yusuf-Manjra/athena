#!/bin/sh

# art-description: SUSYTools ART test - share/minimalExampleJobOptions.py
# art-type: grid
# art-include: master/AthAnalysis
# art-output: hist-Ath_mc21a_DAOD_PHYS.root
# art-output: dcube

# Create empty pool file
art.py createpoolfile

echo "Running SUSYTools test: 'share/athena SUSYTools/jobOptions.py - --testCampaign mc21a'"
athena SUSYTools/jobOptions.py --evtMax 2000 - --testCampaign mc21a
echo  "art-result: $? TEST"

echo "Running DCube post-processing"

tName="mc21a"
dcubeRef=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/References/hist-mc21a_DAOD_PHYS-rel22.root
dcubeXml=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/xml/dcube_config_master_rel22_Run3.xml

/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube ${tName} hist-Ath_${tName}_DAOD_PHYS.root ${dcubeXml} ${dcubeRef}

echo "art-result: $? DCUBE"

echo "Done."
