#!/bin/sh
#
# art-description: Run FastChain with MC+MC Overlay for MC16, ttbar, no trigger
# art-type: grid
# art-include: 21.3/Athena

# art-output: *.root                                                           
# art-output: log.*                                                            
# art-output: mem.summary.*                                                    
# art-output: mem.full.*                                                       
# art-output: runargs.*  

export inputHitsFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1
export inputRdoBkgFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/PileupPremixing/21.0/v1/RDO.merged-pileup.100events.pool.root
export outputRdoFile=MC_plus_MC.RDO.pool.root
export outputAodFile=MC_plus_MC.AOD.pool.root
export outputEsdFile=MC_plus_MC.ESD.pool.root

FastChain_tf.py \
    --inputHITSFile $inputHitsFile \
    --inputRDO_BKGFile $inputRdoBkgFile \
    --outputRDOFile $outputRdoFile \
    --maxEvents 20 \
    --skipEvents 10  \
    --digiSeedOffset1 511 \
    --digiSeedOffset2 727 \
    --conditionsTag OFLCOND-MC16-SDR-20 \
    --geometryVersion ATLAS-R2-2016-01-00-01 \
    --triggerConfig 'OverlayPool=NONE' \
    --postExec 'ServiceMgr.TGCcablingServerSvc.forcedUse=True' \
    --preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False);' \
    --postInclude 'EventOverlayJobTransforms/blockFoldersMC_OverlayPool.py' \
    --imf False


FastChain_tf.py \
   --inputRDOFile $outputRdoFile \
   --outputESDFile $outputEsdFile \
   --outputAODFile $outputAodFile \
   --maxEvents -1 \
   --skipEvents 0 \
   --autoConfiguration everything \
   --preExec 'rec.doTrigger=False;from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False);' 'RAWtoESD:from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False' \
  --postInclude 'r2e:EventOverlayJobTransforms/blockFoldersMC_RAWtoESD.py' 'e2a:EventOverlayJobTransforms/blockFoldersMC_ESDtoAOD.py' \
  --imf False
