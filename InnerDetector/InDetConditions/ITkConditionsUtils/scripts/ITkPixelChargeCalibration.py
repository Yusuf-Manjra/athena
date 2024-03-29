#!/usr/bin/env python
#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
import logging
from argparse import ArgumentParser
from json import dumps
from pathlib import Path
from sys import exit

from PyCool import cool

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)

parser = ArgumentParser("ItkPixelChargeCalibration.py")
parser.add_argument("-i", "--identifiers", type=str, help="Identifiers dump", default="ITkPixelIdentifiers.dat")
parser.add_argument("--highThresholds", action='store_true', help="Enable high thresholds")
args = parser.parse_args()

logging.info("Identifiers file: %s", args.identifiers)
logging.info("High thresholds enabled: %s", args.highThresholds)

dbName = "OFLP200"
dbFolder = "/ITk/PixelChargeCalib"
dbTag = "PixelChargeCalib_LUTTest-00-00" 
dbFile = "ITkPixelChargeCalib.db"

runSince = 0
runUntil = 'inf'

logging.info("Database name: %s", dbName)
logging.info("Folder name: %s", dbFolder)
logging.info("Folder tag: %s", dbTag)
logging.info("Output file: %s", dbFile)
logging.info("Run range: %s - %s", runSince, runUntil)

dbFilePath = Path(dbFile)
if dbFilePath.exists():
    dbFilePath.unlink()

# get database service and open database
dbSvc = cool.DatabaseSvcFactory.databaseService()
# database accessed via physical name
dbString = f"sqlite://;schema={dbFile};dbname={dbName}"
try:
    db = dbSvc.createDatabase(dbString)
except Exception as e:
    logging.error("Problem creating database %s", e)
    exit(1)
logging.info("Created database %s", dbString)

# setup folder
spec = cool.RecordSpecification()
# define main field
spec.extend('data_array', cool.StorageType.String16M)
# folder meta-data - note for Athena this has a special meaning
desc = '<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type="71" clid="1238547719" /></addrHeader><typeName>CondAttrListCollection</typeName>'
# create the folder - multiversion version
# last argument is createParents - if true, automatically creates parent folders if needed
# note this will not work if the database already exists - delete ITkPixelChargeCalib.db first
folderSpec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION, spec)
folder = db.createFolder(dbFolder, folderSpec, desc, True)
# now fill in some data - create a record and fill it
data = cool.Record(spec)

# define IoVs
iovMin = runSince << 32 | 0
iovMax = cool.ValidityKeyMax if runUntil == 'inf' else runUntil << 32 | 0

logging.info("IoV range: %s - %s", iovMin, iovMax)


## Format:
# analog threshold value,noise for normal,long/ganged pixels
# Q0-Q16 charge calibration constants for normal and long/ganged pixels

# quad chip
if args.highThresholds:

    calibQuad = [
        [2000, 75, 2000, 75, 2080, 2957, 4000, 5163, 6824, 8700, 11285, 14709, 19600, 27500, 40400, 69500, 166400, 3000000, 999999999., 999999999.],
        [2000, 75, 2000, 75, 2080, 2957, 4000, 5163, 6824, 8700, 11285, 14709, 19600, 27500, 40400, 69500, 166400, 3000000, 999999999., 999999999.],
        [2000, 75, 2000, 75, 2080, 2957, 4000, 5163, 6824, 8700, 11285, 14709, 19600, 27500, 40400, 69500, 166400, 3000000, 999999999., 999999999.],
        [2000, 75, 2000, 75, 2080, 2957, 4000, 5163, 6824, 8700, 11285, 14709, 19600, 27500, 40400, 69500, 166400, 3000000, 999999999., 999999999.],
    ]
    # single chip 50x50
    calibSingle = [
        [2000, 75, 2000, 75, 2080, 2957, 4000, 5163, 6824, 8700, 11285, 14709, 19600, 27500, 40400, 69500, 166400, 3000000, 999999999., 999999999.],
    ]
    # single chip 25x150 (L0)
    calibSingleL0 = [
        [2000, 110, 2000, 110, 2080, 2957, 4000, 5163, 6824, 8700, 11285, 14709, 19600, 27500, 40400, 69500, 166400, 3000000, 999999999., 999999999.],
    ]

else:
    calibQuad = [
        [600, 75, 600, 75, 2080, 2957, 4000, 5163, 6824, 8700, 11285, 14709, 19600, 27500, 40400, 69500, 166400, 3000000, 999999999., 999999999.],
        [600, 75, 600, 75, 2080, 2957, 4000, 5163, 6824, 8700, 11285, 14709, 19600, 27500, 40400, 69500, 166400, 3000000, 999999999., 999999999.],
        [600, 75, 600, 75, 2080, 2957, 4000, 5163, 6824, 8700, 11285, 14709, 19600, 27500, 40400, 69500, 166400, 3000000, 999999999., 999999999.],
        [600, 75, 600, 75, 2080, 2957, 4000, 5163, 6824, 8700, 11285, 14709, 19600, 27500, 40400, 69500, 166400, 3000000, 999999999., 999999999.],
    ]
    # single chip 50x50
    calibSingle = [
        [600, 75, 600, 75, 2080, 2957, 4000, 5163, 6824, 8700, 11285, 14709, 19600, 27500, 40400, 69500, 166400, 3000000, 999999999., 999999999.],
    ]
    # single chip 25x150 (L0)
    calibSingleL0 = [
        [900, 110, 900, 110, 2080, 2957, 4000, 5163, 6824, 8700, 11285, 14709, 19600, 27500, 40400, 69500, 166400, 3000000, 999999999., 999999999.],
    ]

# read pixel identifiers
output = {}

with open(args.identifiers, "r") as identifiers:
    for line in identifiers:
        if line[0] == "#":
            continue
        ids = line.split()

        if ids[-1] == "4":
            output[ids[0]] = calibQuad
        elif ids[2] == "0" and ids[3] == "0":
            output[ids[0]] = calibSingleL0
        else:
            output[ids[0]] = calibSingle

data["data_array"] = dumps(output, separators=(',', ':'))
folder.storeObject(iovMin, iovMax, data, 0, dbTag)

# close the database
db.closeDatabase()
