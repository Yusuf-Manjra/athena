##############################################################
#
# Job options file to configure:
#        Digitization
#        LVL1 Simulation
#        ByteStream conversion
#
# Author: Davide Costanzo
# Contacts:
#      Inner Detector:   Davide Costanzo
#      LArCalorimeter:   Guillaume Unal
#      TileCalorimter:   Sasha Solodkov
#      MuonSpectrometer: Daniela Rebuzzi, Ketevi Assamagam
#      LVL1 Simulation:  Tadashi Maeno
#      ByteStream:       Hong Ma
#
#==============================================================
#

# Set up PileupMergeSvc used by subdetectors
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon import CfgGetter

# Set up ComTimeRec for cosmics digitization
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType == "cosmics" :
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    from CommissionRec.CommissionRecConf import ComTimeRec
    comTimeRec = ComTimeRec("ComTimeRec")
    topSequence += comTimeRec

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from Digitization.DigitizationFlags import digitizationFlags
if 'doFastPixelDigi' in digitizationFlags.experimentalDigi() or 'doFastSCT_Digi' in digitizationFlags.experimentalDigi() or 'doFastTRT_Digi' in digitizationFlags.experimentalDigi():
    job += CfgGetter.getAlgorithm("FastPileUpToolsAlg", tryDefaultConfigurable=True)
elif 'doSplitDigi' in digitizationFlags.experimentalDigi():
    job += CfgGetter.getAlgorithm("SplitPileUpToolsAlg", tryDefaultConfigurable=True)
else: ## The above two options are just there for back-compatibility
    job += CfgGetter.getAlgorithm(digitizationFlags.digiSteeringConf.get_Value(), tryDefaultConfigurable=True)


# MC Truth info
#if DetFlags.Truth_on():
#    include( "Digitization/TruthDigitization.py" )

# Forward Detectors
#if DetFlags.Forward_on():
#    include( "Digitization/FwdDetDigitization.py" )

# Inner Detector
if DetFlags.ID_on():
    include( "Digitization/InDetDigitization.py" )

# Calorimeters
if DetFlags.Calo_on():
    include( "Digitization/CaloDigitization.py" )

# Muon Digitization
if DetFlags.Muon_on():
    include( "Digitization/MuonDigitization.py" )

# LVL1 trigger simulation
#if DetFlags.digitize.LVL1_on():
include( "Digitization/LVL1Digitization.py" )
