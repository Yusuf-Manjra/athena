# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaConfiguration.AutoConfigFlags import DetDescrInfo, getDefaultDetectors
# This module is based upon Control/AthenaCommon/python/DetFlags.py
# Only some flags have been migrated. A full list of what the old
# DetFlags provided is given for reference below:
# detectors : ID = bpipe pixel SCT TRT BCM DBM
#             ITk = bpipe ITkPixel ITkStrip BCMPrime (in future maybe PLR)
#             Forward = Lucid ZDC ALFA AFP FwdRegion
#             LAr = em HEC FCal
#             Calo = em HEC FCal Tile
#             Muon = MDT CSC TGC RPC sTGC Micromegas
#             Truth
#             BField
#             FTK
# tasks:
#   geometry : setup the geometry
#   dcs : DCS information is available
#   simulate : simulate
#   simulateLVL1 : LVL1 simulation
#   detdescr : setup detector description (for anything which is not geant)
#   pileup   : collect hits from physics and min bias events
#   digitize : hit -> RDO (raw data objects)
#   makeRIO  : RDO -> RIO (Reconstruction Input Objects)
#   writeBS  : write RDO byte stream
#   readRDOBS : read RDO from byte stream
#   readRDOPool : read RDO from pool
#   readRIOBS : read RIO directly from BS
#   writeRDOPool : write RDO in pool
#   readRIOPool  : read RIO from pool
#   writeRIOPool : write RIO in pool
#   overlay : overlay setup


# all detectors - used in helper functions
allDetectors = [
    'Bpipe', 'Cavern',
    'BCM', 'DBM', 'Pixel', 'SCT', 'TRT',
    'BCMPrime', 'ITkPixel', 'ITkStrip', 'HGTD',
    'LAr', 'Tile',
    'CSC', 'MDT', 'RPC', 'TGC', 'sTGC', 'MM',
    'Lucid', 'ZDC', 'ALFA', 'AFP', 'FwdRegion',
]
# all detector groups - used in helper functions
allGroups = {
    'ID': ['BCM', 'DBM', 'Pixel', 'SCT', 'TRT'],
    'ITk': ['BCMPrime', 'ITkPixel', 'ITkStrip'],
    'Calo': ['LAr', 'Tile', 'MBTS'],
    'Muon': ['CSC', 'MDT', 'RPC', 'TGC', 'sTGC', 'MM'],
    'Forward': ['Lucid', 'ZDC', 'ALFA', 'AFP', 'FwdRegion'],
}


def createDetectorConfigFlags():
    dcf = AthConfigFlags()

    ## Detector.Geometry* flags (will) represent the default full geometry,
    ## autoconfigured from the geometry tag
    ## currently keeping the behaviour the same until all clients are identified
    dcf.addFlag('Detector.GeometryBpipe', False)

    # Inner Detector
    dcf.addFlag('Detector.GeometryBCM',   False)
    dcf.addFlag('Detector.GeometryDBM',   False)
    dcf.addFlag('Detector.GeometryPixel', False)
    dcf.addFlag('Detector.GeometrySCT',   False)
    dcf.addFlag('Detector.GeometryTRT',   False) # Set default according to prevFlags.GeoModel.Run?
    dcf.addFlag('Detector.GeometryID',    lambda prevFlags : (prevFlags.Detector.GeometryBCM or prevFlags.Detector.GeometryDBM or
                                                              prevFlags.Detector.GeometryPixel or prevFlags.Detector.GeometrySCT or
                                                              prevFlags.Detector.GeometryTRT))

    # Upgrade ITk Inner Tracker is a separate and parallel detector
    dcf.addFlag('Detector.GeometryBCMPrime',   False)
    dcf.addFlag('Detector.GeometryITkPixel',   False)
    dcf.addFlag('Detector.GeometryITkStrip',   False)
    dcf.addFlag('Detector.GeometryITk',    lambda prevFlags : (prevFlags.Detector.GeometryBCMPrime or prevFlags.Detector.GeometryITkPixel or prevFlags.Detector.GeometryITkStrip))

    dcf.addFlag('Detector.GeometryHGTD', False)

    # Calorimeters
    dcf.addFlag('Detector.GeometryLAr',   False) # Add separate em HEC and FCAL flags?
    dcf.addFlag('Detector.GeometryTile',  False)
    dcf.addFlag('Detector.GeometryMBTS',  True)  # for backwards compatibility for now
    dcf.addFlag('Detector.GeometryCalo', lambda prevFlags : (prevFlags.Detector.GeometryLAr or prevFlags.Detector.GeometryTile))

    # Muon Spectrometer
    dcf.addFlag('Detector.GeometryCSC', lambda prevFlags : DetDescrInfo(prevFlags.GeoModel.AtlasVersion)['Muon']['HasCSC'])
    dcf.addFlag('Detector.GeometryMDT',   False)
    dcf.addFlag('Detector.GeometryRPC',   False)
    dcf.addFlag('Detector.GeometryTGC',   False)
    dcf.addFlag('Detector.GeometrysTGC', lambda prevFlags : DetDescrInfo(prevFlags.GeoModel.AtlasVersion)['Muon']['HasSTGC'])
    dcf.addFlag('Detector.GeometryMM', lambda prevFlags : DetDescrInfo(prevFlags.GeoModel.AtlasVersion)['Muon']['HasMM'])
    dcf.addFlag('Detector.GeometryMuon',  lambda prevFlags : (prevFlags.Detector.GeometryCSC or prevFlags.Detector.GeometryMDT or
                                                              prevFlags.Detector.GeometryRPC or prevFlags.Detector.GeometryTGC or
                                                              prevFlags.Detector.GeometrysTGC or prevFlags.Detector.GeometryMM))

    # Forward detectors (disabled by default)
    dcf.addFlag('Detector.GeometryLucid',     False)
    dcf.addFlag('Detector.GeometryZDC',       False)
    dcf.addFlag('Detector.GeometryALFA',      False)
    dcf.addFlag('Detector.GeometryAFP',       False)
    dcf.addFlag('Detector.GeometryFwdRegion', False)
    dcf.addFlag('Detector.GeometryForward',   lambda prevFlags : (prevFlags.Detector.GeometryLucid or prevFlags.Detector.GeometryZDC or
                                                                  prevFlags.Detector.GeometryALFA or prevFlags.Detector.GeometryAFP or
                                                                  prevFlags.Detector.GeometryFwdRegion))

    # Cavern (disabled by default)
    dcf.addFlag('Detector.GeometryCavern',False)


    ## Detector.Enable* flags (currently) represent the default full geometry,
    ## autoconfigured from the geometry tag
    # Inner Detector
    dcf.addFlag('Detector.EnableBCM',   lambda prevFlags : 'BCM' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableDBM',   lambda prevFlags : 'DBM' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnablePixel', lambda prevFlags : 'Pixel' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableSCT',   lambda prevFlags : 'SCT' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableTRT',   lambda prevFlags : 'TRT' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableID',    lambda prevFlags : (prevFlags.Detector.EnableBCM or prevFlags.Detector.EnableDBM or
                                                            prevFlags.Detector.EnablePixel or prevFlags.Detector.EnableSCT or
                                                            prevFlags.Detector.EnableTRT))

    # Upgrade ITk Inner Tracker is a separate and parallel detector
    dcf.addFlag('Detector.EnableBCMPrime', lambda prevFlags : 'BCMPrime' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableITkPixel', lambda prevFlags : 'ITkPixel' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableITkStrip', lambda prevFlags : 'ITkStrip' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableITk',      lambda prevFlags : (prevFlags.Detector.EnableBCMPrime or
                                                               prevFlags.Detector.EnableITkPixel or
                                                               prevFlags.Detector.EnableITkStrip))

    dcf.addFlag('Detector.EnableHGTD', lambda prevFlags : 'HGTD' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))

    # Calorimeters
    dcf.addFlag('Detector.EnableLAr',    lambda prevFlags : 'LAr' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))  # Add separate em HEC and FCAL flags?
    dcf.addFlag('Detector.EnableTile',   lambda prevFlags : 'Tile' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableMBTS',   lambda prevFlags : (prevFlags.Detector.EnableLAr and 'MBTS' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion)))
    dcf.addFlag('Detector.EnableL1Calo', lambda prevFlags : (prevFlags.Detector.EnableLAr or prevFlags.Detector.EnableTile))
    dcf.addFlag('Detector.EnableCalo',   lambda prevFlags : (prevFlags.Detector.EnableLAr or prevFlags.Detector.EnableTile))

    # Muon Spectrometer
    dcf.addFlag('Detector.EnableCSC',  lambda prevFlags : 'CSC' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableMDT',  lambda prevFlags : 'MDT' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableRPC',  lambda prevFlags : 'RPC' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableTGC',  lambda prevFlags : 'TGC' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnablesTGC', lambda prevFlags : 'sTGC' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableMM',   lambda prevFlags : 'MM' in getDefaultDetectors(prevFlags.GeoModel.AtlasVersion))
    dcf.addFlag('Detector.EnableMuon', lambda prevFlags : (prevFlags.Detector.EnableCSC or prevFlags.Detector.EnableMDT or
                                                           prevFlags.Detector.EnableRPC or prevFlags.Detector.EnableTGC or
                                                           prevFlags.Detector.EnablesTGC or prevFlags.Detector.EnableMM))

    # Forward detectors (disabled by default)
    dcf.addFlag('Detector.EnableLucid',     False)
    dcf.addFlag('Detector.EnableZDC',       False)
    dcf.addFlag('Detector.EnableALFA',      False)
    dcf.addFlag('Detector.EnableAFP',       False)
    dcf.addFlag('Detector.EnableFwdRegion', False)
    dcf.addFlag('Detector.EnableForward',   lambda prevFlags : (prevFlags.Detector.EnableLucid or prevFlags.Detector.EnableZDC or
                                                                prevFlags.Detector.EnableALFA or prevFlags.Detector.EnableAFP or
                                                                prevFlags.Detector.EnableFwdRegion))


    # Reconstruction flags (ID and ITk only. Disabled by default)
    dcf.addFlag('Detector.RecoBCM',   False)
    dcf.addFlag('Detector.RecoIBL', lambda prevFlags : (prevFlags.Detector.RecoPixel and prevFlags.GeoModel.Run in ["RUN2", "RUN3"])) # TODO Review if a separate RecoIBL flag is really required here
    dcf.addFlag('Detector.RecoPixel', False)
    dcf.addFlag('Detector.RecoSCT',   False)
    dcf.addFlag('Detector.RecoTRT',   False)
    dcf.addFlag('Detector.RecoID',    lambda prevFlags : (prevFlags.Detector.RecoBCM or prevFlags.Detector.RecoIBL or
                                                          prevFlags.Detector.RecoPixel or prevFlags.Detector.RecoSCT or
                                                          prevFlags.Detector.RecoTRT))
    
#    dcf.addFlag('Detector.Reco',      lambda prevFlags : (prevFlags.Detector.RecoID or prevFlags.Detector.RecoCalo or
#                                                          prevFlags.Detector.RecoMuon))
    dcf.addFlag('Detector.RecoITkPixel', False)
    dcf.addFlag('Detector.RecoITkStrip',   False)
    dcf.addFlag('Detector.RecoBCMPrime',   False)
    dcf.addFlag('Detector.RecoITk',    lambda prevFlags : (prevFlags.Detector.RecoITkPixel or prevFlags.Detector.RecoITkStrip or prevFlags.Detector.RecoBCMPrime))

    return dcf


def _parseDetectorsList(flags, detectors):
    # setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('DetectorConfigFlags')
    # load flags
    flags._loadDynaFlags('Detector')
    # first check if we have groups
    groups = [d for d in detectors if d in allGroups.keys()]
    detectors = [d for d in detectors if d not in allGroups.keys()]
    for g in groups:
        log.debug("Evaluating detector group '%s'", g)
        if g == 'Forward':
            # forward detectors are special
            for d in allGroups[g]:
                log.debug("Appending detector '%s'", d)
                detectors.append(d)
        else:
            # in case of groups only enable defaults
            for d in allGroups[g]:
                if d in getDefaultDetectors(flags.GeoModel.AtlasVersion):
                    log.debug("Appending detector '%s'", d)
                    detectors.append(d)

    # check if valid detectors are required
    for d in detectors:
        if d not in allDetectors:
            log.warning("Unknown detector '%s'", d)

    return detectors


def setupDetectorsFromList(flags, detectors, toggle_geometry=False, validate_only=False):
    """Setup (toggle) detectors from a pre-defined list"""
    changed = False
    # setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('DetectorConfigFlags')
    # parse the list
    detectors = _parseDetectorsList(flags, detectors)

    # print summary
    if validate_only:
        log.info('Required detectors: %s', detectors)
    else:
        log.info('Setting detectors to: %s', detectors)

    # go through all of the detectors and check what should happen
    for d in allDetectors:
        status = d in detectors
        name = f'Detector.Enable{d}'
        if flags.hasFlag(name):
            if flags(name) != status:
                changed = True
                if validate_only:
                    log.warning("Flag '%s' should be %s but is set to %s", name, status, not status)
                else:
                    log.info("Toggling '%s' from %s to %s", name, not status, status)
                    flags._set(name, status)
        if toggle_geometry:
            name = f'Detector.Geometry{d}'
            if flags.hasFlag(name):
                if flags(name) != status:
                    changed = True
                    if validate_only:
                        log.warning("Flag '%s' should be %s but is set to %s", name, status, not status)
                    else:
                        log.info("Toggling '%s' from %s to %s", name, not status, status)
                        flags._set(name, status)

    return changed


def enableDetectors(flags, detectors, toggle_geometry=False):
    """Enable detectors from a list"""
    changed = False
    # setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('DetectorConfigFlags')
    # parse the list
    detectors = _parseDetectorsList(flags, detectors)

    # debugging
    log.info('Enabling detectors: %s', detectors)

    # go through all of the detectors and enable them if needed
    for d in detectors:
        name = f'Detector.Enable{d}'
        if flags.hasFlag(name):
            if flags(name) is not True:
                changed = True
                log.info("Enabling '%s'", name)
                flags._set(name, True)
        if toggle_geometry:
            name = f'Detector.Geometry{d}'
            if flags.hasFlag(name):
                if flags(name) is not True:
                    changed = True
                    log.info("Enabling '%s'", name)
                    flags._set(name, True)

    return changed


def disableDetectors(flags, detectors, toggle_geometry=False):
    """Disable detectors from a list"""
    changed = False
    # setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('DetectorConfigFlags')
    # parse the list
    detectors = _parseDetectorsList(flags, detectors)

    # debugging
    log.info('Disabling detectors: %s', detectors)

    # go through all of the detectors and disable them if needed
    for d in detectors:
        name = f'Detector.Enable{d}'
        if flags.hasFlag(name):
            if flags(name) is not False:
                changed = True
                log.info("Disabling '%s'", name)
                flags._set(name, False)
        if toggle_geometry:
            name = f'Detector.Geometry{d}'
            if flags.hasFlag(name):
                if flags(name) is not False:
                    changed = True
                    log.info("Disabling '%s'", name)
                    flags._set(name, False)

    return changed
