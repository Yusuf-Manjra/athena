#!/bin/bash

echo 'Testing duplicate key upload'
if [ $# -ge 1 ]; then
   type=$1
   echo "Trying to upload Menu generated with tests with "${type}"_menu and "${type}"_rerun in name" 
else
   type=""
fi

#setup the TT
export _JAVA_OPTIONS="-Xms512m -Xmx1048m"
export DBConn="TRIGGERDBATN"

source $TDAQ_RELEASE_BASE/tdaq/$TDAQ_VERSION/installed/setup.sh $TDAQ_VERSION
export PATH=$PATH:$TDAQ_JAVA_HOME/bin

export TNS_ADMIN=/afs/cern.ch/atlas/offline/external/oracle/latest/admin

##get the right pattern to load Lvl1 xml file
if [ "$type" == "HLT_LS1V1" ]; then
  stump="LS1_v1"
elif [ "$type" == "HLT_physicsV5" ]; then
  stump="Physics_pp_v5"
elif [ "$type" == "HLT_physicsV6" ]; then
  stump="Physics_pp_v6"
elif [ "$type" == "HLT_physicsV7" ]; then
  stump="Physics_pp_v7"
else 
  stump=""
fi

#get the L1 file (common) cosmic and IB the same
get_files -xmls -copy LVL1config_"${stump}".xml
l1menu=`find .  -name LVL1config_${stump}.xml` 

#get the L1 Topo configuration

get_files -xmls -copy L1Topoconfig_"${stump}".xml 
l1topo=`find .  -name L1Topoconfig_${stump}.xml`

#prepare files for first key: l2 and ef menus are the same (full menu)
hltmenu1=`find ../"${type}"_menu/ -name outputHLTconfig_\*.xml`


# copy the setup files to the local directory to have tests independent of each other
cp ../"${type}"_menu/ef_Default_setup.txt ../"${type}"_menu/ef_Default_setup_setup.txt .
ConvertHLTSetup_txt2xml.py ef_Default_setup.txt ef_Default_setup_setup.txt > convertHLT1
hlt__setup1=ef_Default_setup.xml

# get dtd file for L1 menu
get_files -xmls LVL1config.dtd


#upload the first key
echo "upload the first key"

cmd1="/afs/cern.ch/user/a/attrgcnf/public/TriggerTool/cmake/run_TriggerTool_MenuExperts.sh -up -release 'AthenaP1' --l1_menu $l1menu --topo_menu $l1topo -hlt $hltmenu1 --hlt_setup $hlt__setup1 --name 'AthenaP1test' -l INFO --dbConn $DBConn -w_n 60 -w_t 60 "


echo $cmd1
eval $cmd1 &> uploadSMK1.log


if [ ! -f MenusKeys.txt ]
then
    echo '... ERROR Upload of key 1 failed'
    echo 'In ./uploadSMK1.log:'
    grep "Can't obtain write lock" uploadSMK1.log
    grep "SEVERE" uploadSMK1.log
    exit 1
fi

mv MenusKeys.txt MenusKeys1.txt

#prepare files for second key: l2 and ef menus are the same (full menu)
hltmenu2=`find ../"${type}"_rerun/ -name outputHLTconfig_\*.xml`

cp ../"${type}"_rerun/ef_Default_setup.txt ef_Default_setup_rerun.txt 
cp ../"${type}"_rerun/ef_Default_setup_setup.txt ef_Default_setup_setup_rerun.txt

ConvertHLTSetup_txt2xml.py ef_Default_setup_rerun.txt ef_Default_setup_setup_rerun.txt > convertHLT2
hlt__setup2=ef_Default_setup_rerun.xml


#upload the second key
echo "upload the second key"

cmd2="/afs/cern.ch/user/a/attrgcnf/public/TriggerTool/cmake/run_TriggerTool_MenuExperts.sh -up -release 'AthenaP1' --l1_menu $l1menu --topo_menu $l1topo -hlt $hltmenu2 --hlt_setup $hlt__setup2 --name 'AthenaP1test' -l INFO --dbConn $DBConn -w_n 60 -w_t 60 "

echo $cmd2 "&> uploadSMK2.log"
eval $cmd2 &> uploadSMK2.log



if [ ! -f MenusKeys.txt ]
then
    echo '... ERROR Upload of key 2 failed'
    echo 'In ./uploadSMK2.log:'
    grep "Can't obtain write lock" uploadSMK2.log
    grep "SEVERE" uploadSMK2.log
    exit 1
fi

mv MenusKeys.txt MenusKeys2.txt

smk1=`grep SM MenusKeys1.txt | cut -f8 -d" "| cut -f1 -d":"`
smk2=`grep SM MenusKeys2.txt | cut -f8 -d" "| cut -f1 -d":"`

smkDiffFile=diff_smk_${smk1}_${smk2}.xml

echo "diff key 1 vs key 2"
#java -jar TriggerTool.jar -diff -smk1 $smk1 -smk2 $smk2 -name "AthenaP1test" -dbConn $DBConn -xml $smkDiffFile -w_n 50 -w_t 60
cmd3="/afs/cern.ch/user/a/attrgcnf/public/TriggerTool/cmake/run_TriggerTool_MenuExperts.sh -diff -smk1 $smk1 -smk2 $smk2 -name 'AthenaP1test' -dbConn $DBConn -xml diff_smk_${smk1}_${smk2}.xml -w_n 50 -w_t 60"
echo $cmd3 "&> uploadSMK3.log"
eval $cmd3 &> uploadSMK3.log


### # to be reworked:
### echo "checking Setups"
### 
### sort $hlt__setup1 | grep -v DataFlowConfig > ef_1
### sort $hlt__setup2 | grep -v DataFlowConfig > ef_2
### 
### echo "diffing HLT setups , first pass , second pass "
### 
### diff ef_1 ef_2 > ef_diff.txt
### ef_diff_status=$?
### 
### echo "diff status " $ef_diff_status


if [ $smk1 != $smk2 ]
then
    echo "... ERROR Non identical keys found. See $smkDiffFile"
    echo "... $smkDiffFile ..."
    cat $smkDiffFile
    exit 1
else
    echo 'Identical keys found: PASS'
    exit 0
fi
