include.block("HIRecExample/hip_flagsESD.py")

# put here HeavyIon-proton specific ESD configuration
if rec.doESD :
     print "Applying HIP ESD flags mods"

     from RecExConfig.RecFlags import rec
     rec.doAlfa.set_Value_and_Lock(False)

     from InDetRecExample.InDetJobProperties import InDetFlags
     InDetFlags.doLowMuRunSetup.set_Value_and_Lock(True)
     InDetFlags.doSplitVertexFindingForMonitoring.set_Value_and_Lock(False)
     InDetFlags.writeRDOs.set_Value_and_Lock(True)

     from CaloTools.CaloNoiseFlags import jobproperties
     jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(0)

     from CaloRec.CaloCellFlags import jobproperties
     jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False)
