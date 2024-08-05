#!/bin/bash

# Use: "submitAllFilesInDir.sh [directory]"
# Calls the condor_submit.sh script to submit a job for every file in the passed directory

# Get every file in the directory that ends with "pteraw"
fileDir=$1
listOfFiles=`ls ${fileDir}/*.pteraw`

# Only submit up to this number
nToSubmit=500
nSubmitted=0

# Submit a job for each file
for file in $listOfFiles
do
    # Get the file name without the file path
    fileName=${file##*/}
    #echo $fileName
    
    # Submit the job
    ./condor_submit.sh $fileName $fileDir

    let "nSubmitted=nSubmitted + 1"
    echo $nSubmitted
    if [ "$nSubmitted" -gt "$nToSubmit" ]; then
	exit 0
    fi
done
