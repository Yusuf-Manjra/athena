# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define method to construct configured Tile cabling service"""

from AthenaConfiguration.ComponentFactory import CompFactory

def TileCablingSvcCfg(flags):
    """Return component accumulator with configured Tile cabling service

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """

    from AthenaCommon.Logging import logging
    msg = logging.getLogger('TileCablingSvc')

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge( TileGMCfg(flags) )

    tileCablingSvc = CompFactory.TileCablingSvc()

    geometry = flags.GeoModel.AtlasVersion
    runNumber = flags.Input.RunNumber[0]
    run = flags.GeoModel.Run
    if run == 'RUN1':
        if runNumber > 219651: 
            # Choose RUN2 cabling for old geometry tags starting from 26-MAR-2013
            tileCablingSvc.CablingType = 4
            msg.warning("Forcing RUN2 cabling for run %s with geometry %s", runNumber, geometry)

    elif run == 'RUN2':
        if (flags.Input.isMC and runNumber >= 310000) or runNumber >= 343000 or runNumber < 1:
            # Choose RUN2a cabling for R2 geometry tags starting from 31-Jan-2018
            tileCablingSvc.CablingType = 5
            msg.info("Forcing RUN2a (2018) cabling for run %s with geometry %s", runNumber, geometry)

        else:
            tileCablingSvc.CablingType = 4
            msg.info("Forcing RUN2 (2014-2017) cabling for run %s with geometry %s", runNumber, geometry)

    acc.addService(tileCablingSvc, primary = True)

    return acc


if __name__ == "__main__":

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    
    # Test setup
    log.setLevel(DEBUG)

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = ComponentAccumulator()
    acc.merge( TileCablingSvcCfg(ConfigFlags) )

    acc.printConfig(withDetails = True, summariseProps = True)
    print(acc.getService('TileCablingSvc'))
    acc.store( open('TileCablingSvc.pkl','wb') )

    print('All OK')
