#!/bin/bash

#Exit without submitting Module Sequence. This disables
#OfflineQA. Comment this line out to turn back on.
#exit 0

#Transfer arguments.
inputFile=$1
inputDirectory=$2

# Temporary storage directory on EOS.
tmpDirectory=/eos/experiment/na61/data/OnlineProduction/2024-pLBNF-OfflineQA/ChunkStorage/

runNumber=$(echo $inputFile | sed 's/run-0\([0-9]\{5\}\)x.*/\1/')
runNumber=${runNumber##*/}

# Where to place the new run log table
runLogDropDir=/afs/cern.ch/user/n/na61qa/workDirectory/2024NeutrinoRun/NativeReconstruction/Managers/

# Read in 10 lines of the RunLogTable. If the run number is present, don't request 
# the bookkeeping
#runLines=$(cat $runLogDropDir/RunLogTableNew.txt | tail -n 10)
runLines=$(cat $runLogDropDir/RunLogTableNew.txt)
runExists=0
for word in $runLines; do
    if [[ $word =~ ^-?[0-9]+$ ]]; then
	if [ "$word" -eq "$runNumber" ]; then
	    runExists=1
	fi
    fi
done;

# get the latest run log table, and copy it to the manager folder
if [ "$runExists" -eq "0" ]; then
    echo "Querying bookkeeping"
    curl -k "https://bookkeeping.cern.ch:/api/v1/runlogtable?min_run_number=${runNumber}" > $runLogDropDir/RunLogTableNew.txt
fi

#Copy file immediately from CTA into temporary EOS space.
echo '[INFO] Copying file to temporary storage: '$inputDirectory/$inputFile'.'
xrdcp $inputDirectory/$inputFile $tmpDirectory

if [ $? -ne 0 ]
then
  echo "[ERROR] xrdcp of file "$inputDirectory/$inputFile" failed!"
  exit 1
fi

#Move to submit directory and submit QA job.
cd /afs/cern.ch/user/n/na61qa/workDirectory/2024NeutrinoRun/submit/
echo "Submitting file $inputFile in directory $inputDirectory ."
./condor_submit.sh $inputFile $tmpDirectory

exit 0
