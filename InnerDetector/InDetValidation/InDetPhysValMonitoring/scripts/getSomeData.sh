#!/usr/bin/bash
setupATLAS
localSetupRucioClients
voms-proxy-init -voms atlas
pushd /tmp/$USER
#rucio download --nrandom 1 valid3:valid3.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.ESD.e3099_s2578_r6603_tid05297574_00
rucio download valid3:ESD.05297574._000080.pool.root.1
rm $TestArea/InnerDetector/InDetValidation/InDetPhysValMonitoring/run/AOD.pool.root
ln -s $PWD/`ls */*` $TestArea/InnerDetector/InDetValidation/InDetPhysValMonitoring/run/AOD.pool.root
rm $TestArea/../run/AOD.pool.root
ln -s $PWD/`ls */*` $TestArea/../run/AOD.pool.root
rm $TestArea/InnerDetector/InDetValidation/InDetPhysValMonitoring/run/*.xml
popd


