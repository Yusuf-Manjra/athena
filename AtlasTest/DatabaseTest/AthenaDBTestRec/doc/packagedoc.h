/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

in/**
@page AthenaDBTestRec_page AthenaDBTestRec package

@section AthenaDBTestRec_introductionAthenaDBTestRec Introduction

The AthenaDBTestRec package provides a number of tools to help study and 
evaluate the performance of the conditions database in the context of
reconstruction:
- TestCoolRecWriter.exe: A standalone applicaton to write entries to the COOL
 conditions database, according to a configuration file specifying a number
 of folders, data structure per folder and time granularity.
- TestCoolRecRead: An Athena algorithm which reads back conditions data within
Athena, configured using a joboption file specifed by TestCoolRecWriter.
- DetStoreDump: A simple algorithm to dump the contents of the StoreGate 
  detector store at the end of initialisation, and after the first event. This
  can be used to study the use of detector store objects under conditions 
  database control.

@section AthenaDBTestRec_TestCoolRecWriter 

This standalone application fills a COOL conditions database according to 
a folder structure specification given in a configuration file. It is designed
to mimic the structure of conditions data from one or several runs, with 
data being stored accordingly. The data is stored by run and then by folder
within a run, with no attempt to mimic the insertion time order or load pattern
generated by real ATLAS conditions data - the main purpose is to fill large
conditions databases which can then be used in readback tests.

The command syntax is:
<pre>
TestCoolRecWriter <coolDBconnection> <mode> <config_file> [<nrun> <run0> <nevent> <time0> <timerun>]
</pre>
where the parameters are as follows:
 -  coolDBconnection: specifies a database connection in the form 
technology:server:schema:dbinstance:username:password, where technology can
be any of those accepted by COOL (oracle, mysql or sqlite), server is a 
database server name (ignored for sqlite), schema is the schema name (or
database filename for SQLite), dbinstance the COOL database instance name
(8 characters maximum) and username and passowrd are obvious (ignored for
SQLite).
 - mode: 1 for filling the database, 2 for creating an Athena joboption file
to read back.
 - config_file: the name of a configuration file specifying the folder structure (see below)
 - nrun: number of runs to be simulated (default 1, 0 gives a single run entry valid for all IOVs)
 - run0: run number of first run (default 10000)
 - nevent: number of events per run (default 720000, for a 1hour run at 200 Hz rate).
 - time0: initial timestamp, in seconds (default 0)
 - timerun: time per run, in seconds (default 3600)

@section AthenaDBTestRec_ConfigurationFile Configuration File

The TestCoolRecWriter configuration file defines one or several database 
folders. Each line starts with a keyword (case senstive) followed by 
several parameters:
<pre>
folder <path> <nchan> <ptime> <ptype> <ncolumns> <size> <period>
multi <n> <path> <nchan> <ptime> <ptype> <ncolumns> <size> <period>
schema <schema_name>
poolfile <mode> <filestem>
</pre>
The (case-sensitive) commands have the following meanings:
- folder: defines a single COOL folder with the given name (<path>) and number 
of channels (<nchan>). The ptime parameter sets the time structure of the 
folder and
is discussed below. The ptype parameter sets the data type, and can be 'int',
'float', 'string, 'poolref' for data consisting of <ncolumns> of 
integers, floats,
strings, references to POOL objects or external tables read via the CoraCool 
interface. For strings, the <size> parameter
sets the string length (in bytes), and for POOL objects it sets the size
of the vector of floats in the payload data (so the size will be approximately
4 times this number, ignoring ROOT file compression). For 
CoraCool, the data consists of a set of <ncolumn> integers, with 
the <size> parameter defining how many sets are associated to each 
COOL channel. The <period> parameter 
defines how often the data will change within one run. For RUNC folders,
the period can be negative, in which case the data will be updated every 
-period runs, with IOVs set to span a number of runs. In all cases, the
actual payload data is filled in randomly.
- multi: defines a set of <n> folders with the same parameters. Folders are
named <path>_0, <path>_1 etc.
- schema allows separate schema to be used for different folders.
- poolfile specifies the mode for writing POOL files, and the stem of the POOL
file names.

The <ptime> variable can take the values DCSC, DCSP, RUNC or RUNP. DCSC models
DCS data indexed by timestamp,  which changes at a constant rate - new values 
being entered into
COOL regularly with time intervals given by the period parameter.  DCSP causes
the time intervals to be distibuted (randomly and independently for each 
channel) between
0 and 2*<period> (so the average amount of data is the same, but the IOVs for
each channel become decorrelated). RUNC models data indexed by run-event
number (typical of calibrations), which change <period> times per run, either
regularly (RUNC) or according to a random distribution (RUNP).

For inline COOL data, the first 2 ints or floats are set 
by TestCoolRecWriter to the run and channel number of the data, and the 
other attributes are set randomly. For strings, this information is stored at 
the beginning of the string in the form run,chan, and the rest of the string
is padded with random characters.

For conditions data stored in POOL files, the example class TestCoolRecPoolData
is used. This class consists of a vector of random floats (of length given 
by the folder <size> parameter discussed above), and subsiduary information
giving the conditons data folder name, run number and channel ID. This 
additional information allows TestCoolRecReader to check the integrity of the
data, i.e. that the right POOL file object was associated to the 
right interval of validity. The POOL objects are stored using a COOL
multichannel folder, using the CondMultChanCollection templated object. Various
options are available for the division of data into different POOL files, 
depending on the setting of the poolfile <mode> parameter (default 0):
- bit 0 controls whether each folder uses a separate POOL file.
- bit 1 controls whether new POOL files are opened for each run produced
POOL files are named according to the setting of the <filestem> parameter, as
<poolstem><x>_run.root where <x> is an internal index of the folder 
(starting at zero) and <run> is the run number. If the mode parameter is zero, 
the filename is simple <poolstem>.root. POOL files are created and registered 
in the local PoolFileCatalog.xml in the usual way. If a file with the same
name is already present and registered in the catalogue, data is appended to it.
@section AthenaDBTestRec_TestCoolRecRead 

TestCoolRecRead is an Athena algorithm for reading back data stored with
TestCoolRecWriter. The algorithm is configured using joboptions to set which
folders should be read, their type (inline data or POOL object references),
whether the data integrity in POOL files should be checked, and an optional
list of auxiliary files which will simply be read. The parameters are as 
follows:

- Folders:    List of COOL folders to be read
- FTypes:     List of types for each folder (0=inline data, 1=POOL reference)
- CheckMatch: Flag to check data integrity (in inline payloads and POOL files)
- AuxFiles:   List of filenames of auxiliary files to be read

The folders to be read and the corresponding database connection parameters
should also be specified to IOVDbSvc in the usual way. The algorithm registers
a callback on each conditions folder in order to count how many times the
folder data changes, and also accesses the data every event, noting any 
access errors. If the CheckMatch property is set, the run and channel number
of inline COOL data is checked to make sure it corresponds to what was 
written by TestCoolRecWriter. For POOL objects, this information is stored
inside the POOL object, allowing it to ensure that the right POOL file has 
been attached, and any errors are
noted. At the end of the job, a line is printed for each folder of the form:
<pre>
TestCoolRecRead      INFO Folder /REFCOND1/LAR/FOLDER_44 nAccess=10 nItems=320 nCallBack=1
</pre>
showing how many times the folder was accessed (once per event), how many items
were read (should be <nchan>*<nevent>) and how many callbacks were counted
(should be one for every time new data is read in). If there are errors,
these are noted in a separate ERROR printout per folder, e.g:
<pre>
TestCoolRecRead     ERROR Folder /TEST/POOL_0 had 120 data mismatches
TestCoolRecRead     ERROR Folder /TEST/POOL_0 read no data
</pre>
Finally, a single summary line is printed, either:
<pre>
TestCoolRecRead      INFO TestCoolRecReadSummary: All folders OK
</pre>
or if there are problems, a summary of the number of folders with errors
or mismatches (note the keyword 'BAD'):
<pre>
TestCoolRecRead      INFO TestCoolRecReadSummary: BAD (0,2,2,0) error, empty folders, msimatched folders, bad files 
</pre>
These lines can be checked for in automatic scripts to see if there were any 
problems.

The 'auxiliary files' feature was added to test the access to arbitrary 
files - each of the filenames given is opened in initialisation and read
(as a binary file). If there are any access errors, they are counted and also
printed n the summaries given above.

The TestCoolRecWriter can be used with <mode>=2 to generate a joboption 
fragment 'CoolReadConfig.py' containing the configuration-specific information
for both TestCoolRecRead and IOVDbSvc. This is included by the example 
joboption TestCoolRecExample.py, which can be used to read back the data 
just produced in TestCoolRecWriter without any further joboption file editing.

The TestCoolRecExample job options.py file:

@include TestCoolRecExample.py

@section AthenaDBTestRec_Reference Workloads

An initial reference workload has been defined in the file refc1301.txt in
the config directory. This defines 50 folders each containing 32 channels
of 150 byte string data (modelling the LAr calibration POOL references),
plus two folders each containing 1174 channels of MDT calibration data strings
(3-5k each, using string and CLOB data types). Note that this file was 
originally refcond1.txt, but changed to refc1301.txt to indicate a version
of the database associated with COOL1.3.0. Similarly, the database instance
below was REFCOND1, and is now REFC1301.

The script gen_refdb.sh in the script directory can be used to create a
COOL conditions database containing a single instance of all this data 
(using TestCoolRecWriter.exe), which
can be written in an Oracle or SQLite conditions database. The athena 
joboption file ReadRefDBExample.py in the share directory runs a complete
athena job to read back this data, using the file Read_refcond1.py to
actually define the folders used (Read_refcond1.py was produced using 
TestCoolRecWriter.exe with mode 2).

An instance of this database has been setup as REFC1301 on the account
ATLAS_COOL_GLOBAL on the CERN INTR database server. By default, 
ReadRefDBExample.py will read from this database instance. Note that the
read password is included in the standard authentication.xml file in 
the AtlasAuthentication package.

Further reference workloads are defined in the configuration files 
ATLR_CalibStream_1 etc, corresponding to the values discussed in the
note 'ATLAS conditions database data volumes and workloads'. These can be
written and read using special scripts WriteRun.py and ReadRun.py. From the
run directory:
<pre>
 ../scripts/WriteRun.py "sqlite:X:test1.db:REFC133" 123 ../config/ATLR_CalibStream_1.txt
</pre>
writes the data corresponding to reference workload ATLR_CalibStream_1.txt
for run 123 into the specified database (here a local SQLite file).
<pre>
../scripts/ReadRun.py "sqlite:X:test1.db:REFC133" 123 20 180  ../config/ATLR_CalibStream_1.txt
</pre>
reads it back, running a test job reading 20 events from run 123, with 180 
seconds timestamp difference between events. These scripts assume runs 
are 3600 seconds long, with runs beginning at time=run*3600.

Another script, ReadReal.py, has been designed to simulate the conditions
DB load produced by real reconstruction jobs. This script uses a configuration
file which can be produced by including the line:
<pre>
conddb.dumpFolderReadReal('config.py')
</pre>
in a reconstruction job. The file 'config.py' will then contain a definition
for the python variable FolderList (a copy of the setting of IOVDbSvc.Folders)
which will be used by the ReadReal.py script to configure the same set of
folders to be accessed. The ReadReal.py script is used from the run directory
as follows:
<pre>
../scripts/ReadReal.py config.py COMCOND-ES1C-000-00 91389 0
</pre>
where the configuration file, the global conditions tag and the run number
to be used are specified. The job reads all the conditions data that a
real reconstruction job processing that run and conditions tag would read 
(from the production COMP200 database instance),
with the exception of five LAR/CALO folders which cannot be read in correctly
without some calorimeter tools infrastructure being configured. The last 
parameter gives an optional time offset in seconds since the start of run - 
this affects timestamp of the DCS data which is being retrieved.
Two example
configuration files for this script are provided in the 'config' directory:
Tier0Cosmic1.py simulates the read load produced by the RecExCommission_ATN.py
reconstruction job in 14.2.23, and Tier0Cosmic2.py does the same but with
the addition of the /LAR/DCS/HV/BARREL/I8, /LAR/DCS/HV/BARREl/I16,
/SCT/DCS/HV and /SCT/DCS/MPS/LV folders, which are not yet read by 
standard reconstruction.

For each of these scripts, it should be sufficent to setup the release and then
do a source setup.sh for the AthenaDBTestRec package - a checkout and 
compilation of AthenaDBTestRec should not be needed. The relative pathnames
to the commands and configuration file shown above would of course have to 
be replaced by absolute pathnames in this case.



@section AthenaDBTestRec_DetStoreDump 

DetStoreDump is a very simple algorithm that calls the Storegate dump method 
on the detector store, at initialisation or at execute for the first event.
If put as the last algorithm in the sequence, it will show the state of the
detector store, what proxies are loaded, etc.

The DetStoreDump_jobOptions.py file:

@include DetStoreDump_jobOptions.py



The package requirements file:




*/
