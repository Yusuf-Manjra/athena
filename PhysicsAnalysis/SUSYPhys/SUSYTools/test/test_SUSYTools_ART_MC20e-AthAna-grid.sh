#!/bin/sh

# art-description: SUSYTools ART test - share/minimalExampleJobOptions_mc.py
# art-type: grid
# art-include: master/AthAnalysis
# art-output: monitoring.mc20e.root
# art-output: dcube

# Create empty pool file
art.py createpoolfile

echo "Running SUSYTools test: 'share/minimalExampleJobOptions_mc.py -c \'MCCampaign=\"mc20e\"\' '"
athena SUSYTools/minimalExampleJobOptions_mc.py --evtMax=2000 -c 'MCCampaign="mc20e"'
echo  "art-result: $? TEST"

echo "Running DCube post-processing"

tName="mc20e"
dcubeRef=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/References/monitoring.${tName}.root
dcubeXml=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/xml/dcube_config_master.xml

/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube ${tName} monitoring.${tName}.root ${dcubeXml} ${dcubeRef}

echo "art-result: $? DCUBE"

echo "Done."
