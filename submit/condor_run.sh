#!/bin/bash

input_file=${1}
shift 1
input_directory=${1}
shift 1
reco_drop_dir=${1}
shift 1
log_drop_dir=${1}
shift 1
SHINE_DIR=${1}
shift 1
GLOBAL_KEY=${1}
shift 1
trackMatchDumpDropDir=${1}

echo $input_file
echo $input_directory
echo $reco_drop_dir
echo $log_drop_dir
echo $SHINE_DIR
echo $GLOBAL_KEY
echo $trackMatchDumpDropDir

#Vertex fit type = pp or pA
VERTEX_FIT_TYPE=pp

function initShine {
    #if ! source "$SHINE_DIR/scripts/env/lxplus_32bit_slc6.sh"; then exit 1; fi
    if ! source $SHINE_DIR/scripts/env/lxplus.sh x86_64-el9-gcc12-opt ; then exit 1; fi
    eval $($SHINE_DIR/bin/shine-offline-config --env-sh)	
}

timestamp_start=$(date +%Y%m%d_%H%M%S)

#remove the .raw
outputFileName=${input_file%.pteraw}

#Duplicate output and save it to dedicated logfile
logfile=output.log
exec &> >(tee $logfile)

#Save small, compressed version of the log 
function save_log
{
    if [ ! -f $logfile ]
    then
	echo "No log"
	exit
    fi
    
    lines=$(wc -l $logfile | cut -d' ' -f1)
    #If log has less than 5000 lines, then save entire file
    if [ $lines -lt 5000 ]
    then
	cp $logfile $outputFileName.output.log
    else
	#Otherwise, save first lines from the beginning until message about reading second event
	second_event_line=$(grep -ne "EventFileReader\.cc.*run\ =.*event\ =" $logfile | head -n2 | tail -n1 | cut -d':' -f1)
	head -n $second_event_line $logfile > $outputFileName.output.log
	#... and save last 500 lines
	echo -ne "\n\n================== TAIL =======================\n\n" >> $outputFileName.output.log
	tail -n 500 $logfile >> $outputFileName.output.log
    fi
    bzip2 -f -k $outputFileName.output.log
    cp $outputFileName.output.log.bz2 $log_drop_dir
    echo "Log saved"
}

#####################
# Preparing chain
#####################

echo
echo Unpacking chain...
if ! tar -xzvf ./Stage.tar.gz; then
    EXITCODE=$?
    echo Unable to unpack chain! exiting...
    exit $EXITCODE
fi

cd ./Prod

####################
# Init Shine
####################

echo
echo "Configuring Shine... "
initShine

shoe_file=`echo $input_file | sed -e 's/shoe/calib/g'`
#log_file=${shoe_file}.stdout.log

echo
echo Starting reconstruction...

./CopyFromCTAAndRunSHINE.sh -i $input_file -d $input_directory -g $GLOBAL_KEY &> $logfile ;

EXITCODE=$?

#Comment to not save logs!
echo Copying Logs...
save_log

echo SHINE exit code: $EXITCODE

if [ $EXITCODE -eq 0 ]; then
    echo Moving output...

    echo 'Copying files: '
    ls *Residuals.root
    ls *QA.root
    ls *shoe.root
    #ls *krCalibration.root
    ls *trackMatchDump.root
    #ls *krCalibration.root
    echo 'To directory:'
    echo $reco_drop_dir
    
    #cp -f *Residuals.root $reco_drop_dir
    #cp -f *QA.root $reco_drop_dir
    #cp -f *shoe.root $reco_drop_dir
    #cp -f *vDrift.root $reco_drop_dir
    #cp -f *trackMatchDump.root $reco_drop_dir

    cp -f *Residuals.root ${reco_drop_dir}
    cp -f *QA.root ${reco_drop_dir}
    cp -f *shoe.root ${reco_drop_dir}
    cp -f *vDrift.root ${reco_drop_dir}
    cp -f *trackMatchDump.root ${reco_drop_dir}
    cp -f *trackMatchDump.root ${trackMatchDumpDropDir}
    cp -f *DelayWireChamberReconstruction.root ${reco_drop_dir}
    
    #cp -f *krCalibration.root $reco_drop_dir
fi

exit $EXITCODE

cd -

timestamp_end=$(date +%Y%m%d_%H%M%S)
echo "TIME: ""$timestamp_start"" -- ""$timestamp_end"

exit $EXITCODE
