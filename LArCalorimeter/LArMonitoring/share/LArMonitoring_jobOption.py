
include.block("LArMonitoring/LArMonitoring_jobOption.py")

#Create the set of flags
from AthenaMonitoring.DQMonFlags import DQMonFlags

#Add colltime algo to sequence

from LArMonitoring.LArCollisionTimeMonAlg import LArCollisionTimeMonConfigOld
topSequence +=LArCollisionTimeMonConfigOld(DQMonFlags)

from AthenaCommon.GlobalFlags import globalflags
if DQMonFlags.monManEnvironment() == 'tier0Raw' and globalflags.DataSource == 'data':
    from LArMonitoring.LArDigitMonAlg import LArDigitMonConfigOld
    topSequence +=LArDigitMonConfigOld(DQMonFlags, topSequence)

    from LArMonitoring.LArRODMonAlg import LArRODMonConfigOld
    topSequence +=LArRODMonConfigOld(DQMonFlags)

from LArMonitoring.LArFEBMonAlg import LArFEBMonConfigOld
topSequence +=LArFEBMonConfigOld(DQMonFlags)

#from LArMonitoring.LArCoverageAlg import LArCoverageConfigOld
#topSequence +=LArCoverageConfigOld(DQMonFlags)

#print topSequence


