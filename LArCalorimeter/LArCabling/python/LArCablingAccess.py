# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AthSequencer
from IOVDbSvc.CondDB import conddb
from LArRecUtils.LArRecUtilsConf import LArOnOffMappingAlg, LArFebRodMappingAlg, LArCalibLineMappingAlg

#FIXME, the folder are also set up in LArConditionsCommon/share/LArIdMap_*_jobOptions.py
 
def LArOnOffIdMapping():
    condSequence = AthSequencer("AthCondSeq")
    if hasattr(condSequence,"LArOnOffMappingAlg"):
        return #Already there....

    if conddb.isMC:
        dbname="LAR_OFL"
    else:
        dbname="LAR"

    folder="/LAR/Identifier/OnOffIdMap"
    conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    condSequence+=LArOnOffMappingAlg(ReadKey=folder)
    return

def LArOnOffIdMappingSC():
    condSequence = AthSequencer("AthCondSeq")
    folder="/LAR/IdentifierOfl/OnOffIdMap_SC"
    if hasattr(condSequence,"LArOnOffMappingAlg") and condSequence.LArOnOffMappingAlg.ReadKey==folder:
        return #Already there....

    #for the moment SC mapping is only in MC database
    #and with one tag
    #conddb.addFolder(dbname,folder,className="AthenaAttributeList",forceMC=True)
    conddb.addFolder("","<db>COOLOFL_LAR/OFLP200</db>"+folder,className="AthenaAttributeList",forceMC=True)
    conddb.addOverride(folder,"LARIdentifierOflOnOffIdMap_SC-000")
    condSequence+=LArOnOffMappingAlg("LArOnOffMappingAlgSC",ReadKey=folder, WriteKey="LArOnOffIdMapSC", isSuperCell=True)
    return



def LArFebRodMapping():
    condSequence = AthSequencer("AthCondSeq")
    if hasattr(condSequence,"LArFebRodMappingAlg"):
        return #Already there....

    if conddb.isMC:
        dbname="LAR_OFL"
    else:
        dbname="LAR"

    folder="/LAR/Identifier/FebRodMap"
    conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    condSequence+=LArFebRodMappingAlg(ReadKey=folder)
    return


def LArCalibIdMapping():
    condSequence = AthSequencer("AthCondSeq")
    if hasattr(condSequence,"LArCalibLineMappingAlg"):
        return #Already there....

    if conddb.isMC:
        dbname="LAR_OFL"
    else:
        dbname="LAR"

    folder="/LAR/Identifier/CalibIdMap"
    conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    condSequence+=LArCalibLineMappingAlg(ReadKey=folder)
    return

def LArCalibIdMappingSC():
    condSequence = AthSequencer("AthCondSeq")
    #temporarily disabled, until conditions will arrive to COOL
    #folder="/LAR/Identifier/CalibIdMap"
    #if hasattr(condSequence,"LArCalibLineMappingAlg") and condSequence.LArCalibLineMappingAlg.ReadKey==folder:
    #    return #Already there....

    #if conddb.isMC:
    #    dbname="LAR_OFL"
    #else:
    #    dbname="LAR"
    #conddb.addFolder(dbname,folder,className="AthenaAttributeList")
    # SC only in OFL database
    folder="/LAR/IdentifierOfl/CalibIdMap_SC"
    conddb.addFolder("","<db>sqlite://;schema=/afs/cern.ch/user/p/pavol/w0/public/LAr_Reco_SC_22/run/SCCalibMap.db;dbname=OFLP200</db>"+folder,className="AthenaAttributeList",forceMC=True)
    conddb.addOverride(folder,"LARIdentifierOflCalibIdMap_SC-000")
    condSequence+=LArCalibLineMappingAlg("LArCalibLineMappingAlgSC",ReadKey=folder, WriteKey="LArCalibIdMapSC",isSuperCell=True,MaxCL=16)
    return
