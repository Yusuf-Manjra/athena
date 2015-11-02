#!/bin/bash

if [ $# -ne 3 ]
then
    echo " Syntax $0 <Run> <LB> <FileName>"
    echo " Run/LB = IoV start for UPD4 tag"
    echo " FileName = full list of HVPathologies (first line /LAR/HVPathologiesOfl/Pathologies, then fields  barrel_ec pos_neg FT Slot channel HVModule HVLine pathology)"
    exit
fi

runStart=$1
lbStart=$2
inputTextFile=$3

if [ -f larhvpathology.db ]
then
  /bin/rm larhvpathology.db
fi

if [ -f write.log ]
then
  /bin/rm write.log
fi

if [ -f read.log ]
then
  /bin/rm read.log
fi

if [ -f newList.txt ]
then
  /bin/rm newList.txt
fi

echo " Produce HV pathology DB from input file " $inputTextFile
echo " IoV start to use for UPD4 tag " ${runStart} ${lbStart}

upd1TagName="LARHVPathologiesOflPathologies-UPD1-00"
upd4TagName="LARHVPathologiesOflPathologies-UPD4-01"

echo " Run athena to produce sqlite file"

athena.py -c "InputFile=\"${inputTextFile}\";tagName=\"${upd1TagName}\"" LArCondUtils/LArHVPathologyDbWrite.py > write.log 2>&1

if [ $? -ne 0 ];  then
    echo " Athena reported an error! Please check write.log!"
    exit    
fi

if grep -q ERROR write.log
      then    
      echo " An error occured ! Please check write.log!"
      exit    
fi


echo " Run athena to test sqlite file reading"

athena.py  LArCondUtils/LArHVPathologyDbRead.py > read.log 2>&1

if [ $? -ne 0 ];  then
    echo " Athena reported an error! Please check read.log!"
    exit    
fi

if grep -q ERROR read.log
      then    
      echo " An error occured ! Please check read.log!"
      exit    
fi

grep "Got pathology for cell ID:" read.log > newList.txt

echo " List of pathologies in new sqlite file can be found in newList.txt"


AtlCoolCopy.exe "sqlite://;schema=larhvpathology.db;dbname=COMP200" "sqlite://;schema=larhvpathology.db;dbname=COMP200" -f /LAR/HVPathologiesOfl/Pathologies -of /LAR/HVPathologiesOfl/Pathologies -t ${upd1TagName} -ot ${upd4TagName} -rls ${runStart} ${lbStart} -truncate

echo " Sqlite file produced, to merge it to Oracle execute: "
echo " /afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py larhvpathology.db COMP200 ATLAS_COOLWRITE ATLAS_COOLOFL_LAR_W <password>"

exit
