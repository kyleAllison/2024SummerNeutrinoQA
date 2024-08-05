#!/bin/bash

#Run calibration on a series of chunks, copying the central files from CTA
#Good for testing, since jobs use runModuleSeq.sh anyway

function usage {
  echo """
  usage: $0 -i <filename> -d <directory>
    -i <filename> (required): The stripped filename of the new chunk.
    -d <directory> (required): The directory in which the new chunk is located.
    -g <globalKey> (required): The global for the module sequence.
  """
}

INPUT_FILENAME=""
DIRECTORY=""
GLOBAL_KEY=""

while getopts ":hi:d:g:" opt; do
  case $opt in
    h)
      usage
      exit 1
      ;;
    i)
      INPUT_FILENAME=$OPTARG
      ;;
    d)
      DIRECTORY=$OPTARG
      ;;
    g)
      GLOBAL_KEY=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

if [[ "$INPUT_FILENAME" == "" ]]  ; then echo "No input file name given!" 1>&2  ; usage; exit 1 ; fi
if [[ "$DIRECTORY" == "" ]]  ; then echo "No directory given!" 1>&2  ; usage; exit 1 ; fi

#Source SHINE.
source ./initShine.sh

BASE_NAME=${INPUT_FILENAME%.*}

echo '[INFO] Running SHINE Reconstruction & QA on file '$DIRECTORY/$INPUT_FILENAME'. Global key: '$GLOBAL_KEY
xrdcp $DIRECTORY/$INPUT_FILENAME ./

echo '[INFO] SHINE command:' runModuleSeq.sh -i "./"$INPUT_FILENAME -o $run$chunkNumber -b bootstrap.xml -k $GLOBAL_KEY -v pp -m on

#echo '[INFO] Dumping nominal drift velocities to text files.'
#cd DriftVelocityDump
#./DumpDriftVelocities.sh
#cd ..

#Run module sequence on copied chunk.
make
source runModuleSeq.sh -i "./"$INPUT_FILENAME -o $BASE_NAME -b bootstrap.xml -k $GLOBAL_KEY -v pp -m on

#Remove the file and clean up.
#rm  ./$INPUT_FILENAME
rm bootstrap.xml

#Remove chunk.
#rm $DIRECTORY/$INPUT_FILENAME
