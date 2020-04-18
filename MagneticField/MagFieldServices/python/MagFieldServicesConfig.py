# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    
def MagneticFieldSvcCfg(flags, **kwargs):
    result=ComponentAccumulator()
    
    # initialise required conditions DB folders
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    
    if flags.Input.isMC:
        db='GLOBAL_OFL'
    else:
        db='GLOBAL'
        
    result.merge(addFolders(flags,['/GLOBAL/BField/Maps <noover/>'], detDb=db, className="CondAttrListCollection") )
        
    if not flags.Common.isOnline:
        result.merge(addFolders(flags, ['/EXT/DCS/MAGNETS/SENSORDATA'], detDb='DCS_OFL', className="CondAttrListCollection") )
            

    # AtlasFieldSvc - old one
    afsArgs = {
      "name": "AtlasFieldSvc",
    }
    if flags.Common.isOnline:
      afsArgs.update( UseDCS = False )
      afsArgs.update( UseSoleCurrent = 7730 )
      afsArgs.update( UseToroCurrent = 20400 )
    else:
      afsArgs.update( UseDCS = True )
    if 'UseDCS' in kwargs:
      afsArgs['UseDCS'] = kwargs['UseDCS']
    mag_field_svc = CompFactory.MagField.AtlasFieldSvc(**afsArgs)  
    result.addService(mag_field_svc, primary=True)

    # AtlasFieldMapCondAlg - for reading in map
    afmArgs = {
      "name": "AtlasFieldMapCondAlg",
    }
    afmArgs.update( UseMapsFromCOOL = True )
    mag_field_map_cond_alg = CompFactory.MagField.AtlasFieldMapCondAlg(**afmArgs) 
    result.addCondAlgo(mag_field_map_cond_alg)
    
    # AtlasFieldCacheCondAlg - for reading in current
    afcArgs = {
      "name": "AtlasFieldCacheCondAlg",
    }
    if flags.Common.isOnline:
      afcArgs.update( UseDCS = False )
      afcArgs.update( UseSoleCurrent = 7730 )
      afcArgs.update( UseToroCurrent = 20400 )
    else:
      afcArgs.update( UseDCS = True )
      afcArgs.update( UseNewBfieldCache = True )
    
    if 'UseDCS' in kwargs:
      afcArgs['UseDCS'] = kwargs['UseDCS']
    mag_field_cache_cond_alg = CompFactory.MagField.AtlasFieldCacheCondAlg(**afcArgs) 
    result.addCondAlgo(mag_field_cache_cond_alg)
    
    return result 
    
if __name__=="__main__":
    # To run this, do e.g. 
    # python ../athena/MagneticField/MagFieldServices/python/MagFieldServicesConfig.py
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import VERBOSE
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    log.setLevel(VERBOSE)
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Input.isMC = False
    ConfigFlags.lock()

    cfg=ComponentAccumulator()

    acc  = MagneticFieldSvcCfg(ConfigFlags)
    log.verbose(acc.getPrimary())    
    cfg.merge(acc)


          
    f=open("MagneticFieldSvc.pkl","wb")
    cfg.store(f)
    f.close()
