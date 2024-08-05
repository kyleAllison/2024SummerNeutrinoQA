#!/bin/bash

# Use: CreateOnlineQAPDF.sh /path/to/OnlineQA.root 

pdfMakerDirectory=/afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/OfflineQA/pdfMaker/OnlineQA/
pdfDropDir=/eos/experiment/na61/data/OnlineProduction/2023-k+C-OfflineQA/OnlineQAPDFs/

if [[ $# -gt 1 ]]
then
    echo "Incorrect usage! Usage: ./CreateQAPDF onlineQA.root"
    exit 1
fi

onlineQAFile=$1

# Go to the code dir and compile the code
cp $onlineQAFile $pdfMakerDirectory
cd $pdfMakerDirectory
make clean && make

# Make the images from the root file and then use latex to compile the pdf
$pdfMakerDirectory"/ProcessPlots" $onlineQAFile
pdflatex OnlineQASlides.tex

qaPDFName="OnlineQA-"`date +"%Y-%m-%d"`"-"`date +"%H.%M.%S"`".pdf"
mv OnlineQASlides.pdf $pdfDropDir/$qaPDFName

# Clean up.
rm *.aux *.log *.nav *.out *.snm *.toc *.root

exit 0
