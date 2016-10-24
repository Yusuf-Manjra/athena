#!/usr/bin/env zsh
# Script to generate the trigger menu XML files
#

if [ $# -lt 1 ]; then
    echo "Syntax: XMLDumperFromAthena.sh MENU [DEST]"
    exit 1
fi

menu=$1
dest=$2
if [ -z "$dest" ]; then
    dest=$PWD
fi

# Get absolute path name
dest=`cd $dest; pwd`

jo=TriggerMenuXML/runHLT_forXMLgeneration.py
sqliteFile=TriggerMenuSQLiteFile.sqlite
uploadLog=TriggerMenuUpload.log
doDBUpload="false"

if [[ "$NICOS_ATLAS_RELEASE" != "" ]]; then
    if [[ "$NICOS_ATLAS_ALT_RELEASE" != "None" ]]; then   # MIG nightlies
        release=$NICOS_ATLAS_ALT_RELEASE
    else
        release=$NICOS_ATLAS_RELEASE
    fi
    if [[ "$AtlasProject" != "AtlasP1HLT" && "$AtlasProject" != "AtlasCAFHLT" && "$AtlasProject" != "AtlasTestHLT" ]]; then
        #doDBUpload="true"     # Temporarily disabled due to sqlite slowness (Savannah #102038)
        # we upload the default menu
	    menuForUpload=`python -c "from TriggerJobOpts import TriggerFlags;print TriggerFlags.triggerMenuSetup().get_Value()" | tail -1`
    fi
else
    release=`python -c "from TriggerJobOpts import TriggerFlags; print(TriggerFlags.menuVersion().get_Value())" | tail -1`
fi

if [[ -e MenusKeys.txt ]]; then
    echo "XMLDumperFromAthena: Removing old MenusKeys.txt "
    rm -f MenusKeys.txt
fi

# Setup TriggerTool
if [ ${doDBUpload} = "true" ]; then
    source /afs/cern.ch/sw/lcg/external/Java/bin/setup.sh
    get_files -data -remove -symlink TriggerTool.jar
    export _JAVA_OPTIONS="-Xms256m -Xmx1048m"
fi

# Temporary run directroy and cleanup traps in case of termination
rundir=`mktemp -t -d tmxml.${menu}.XXXXXXXXXX`
TRAPINT() {
    rm -rf $rundir
    return 130 # 128+SIGINT
}
TRAPTERM() {
    rm -rf $rundir
    return 143 # 128+SIGTERM
}


## menu generation starts here
if [ -n "${AtlasTrigger_PLATFORM}" ]; then   # CMAKE
    platform=${AtlasTrigger_PLATFORM}
else  # CMT
    platform=${CMTCONFIG}
fi
echo "XMLDumperFromAthena: *** Building menu: ${menu} for ${release} ***"
logfiletopo=topo${menu}.log
logfile=${menu}.log
cd $rundir

# L1Topo config file
generateL1TopoMenu.py $menu >&! $logfiletopo
cp L1Topoconfig_*.xml ${dest}

# L1 + HLT config file
if [ -z "$TMXML_BUILD_TEST" ]; then
    athena.py -l DEBUG -c "TriggerMenuSetup='$menu'" $jo >&! $logfile
    athena_exit=$?
else
    # Set the above env var to fake the output files (for fast build system test)
    echo $menu > outputLVL1config.xml
    echo $menu > outputHLTconfig.xml
    echo $menu > $logfile
    athena_exit=0
fi

cp $logfile $logfiletopo ${dest}
if [ $athena_exit -eq 0 ]; then
    echo "XMLDumperFromAthena: *** $menu DONE | Exit code: $athena_exit | Log: $dest/$logfile ***"
else
    echo "XMLDumperFromAthena: *** $menu FAILED | Exit code: $athena_exit | Log: $dest/$logfile ***"
fi


#upload default menu into to the SQLite file
#doDBUpload is True if we are in NICOS and not in the P1HLT cache
if [[ ( ${doDBUpload} = "true" ) && ( ${menu} = ${menuForUpload} ) && ( -s outputHLTconfig.xml ) ]]; then
    echo "XMLDumperFromAthena: Uploading the xml files to the DB"
    rm -f $sqliteFile
    cmd=(java -jar TriggerTool.jar -up -release $release -l1_menu outputLVL1config.xml -hlt_menu outputHLTconfig.xml -name $menu -offlineconfig -dbConn "sqlite_file:$sqliteFile" --outputLevel FINER --output $uploadLog)
    echo "XMLDumperFromAthena: Upload command: java $cmd"
    ${cmd}
    cp $uploadLog $dest/TriggerMenuUpload.log
    echo "Copying sqlite file $sqliteFile to $dest/TriggerMenuSQLiteFile_${release}.sqlite"
    chmod 777 $sqliteFile
    cp $sqliteFile ${dest}/TriggerMenuSQLiteFile_${release}.sqlite
    
    if [[ -e MenusKeys.txt ]]; then
        echo "Copying MenuKeys.txt to ${dest}/MenusKeys_${release}.txt"
        cp MenusKeys.txt ${dest}/MenusKeys_${release}.txt
    else
        echo "Error : MenuKeys.txt does not exist, likely a DB upload problem "
    fi
fi

if [[ -e outputLVL1config.xml ]]; then
    cp outputLVL1config.xml ${dest}/LVL1config_${menu}_${release}.xml
    cp outputHLTconfig.xml ${dest}/HLTconfig_${menu}_${release}.xml
fi

#this gives some more sensitivity to hidden problems
grep --colour ERROR ${dest}/$logfile
grep --colour -A 100 "Shortened traceback" ${dest}/$logfile

rm -rf $rundir

# Do not return real athena exit code as we want to pretend everything was fine

