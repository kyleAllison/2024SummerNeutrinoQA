#!/bin/bash

# Use: ./CreateQAPDF.sh 

#Number of most-recent files to hadd. 
#numberOfFiles=1000
numberOfFiles=2500
#numberOfFiles=10

if [[ $# -gt 1 ]]
then
    echo "Incorrect usage! Usage: ./CreateQAPDF [optional run number, properly zero-padded]"
    exit 1
fi

runNumber=$1

EOSDropDirectory='/afs/cern.ch/user/n/na61qa/workDirectory/2024NeutrinoRun/EOSDropDirectory'
pdfMakerDirectory='/afs/cern.ch/user/n/na61qa/workDirectory/2024NeutrinoRun/pdfMaker'

# hadd seemes to need the shine env set for alma9. Set env here for acrontab.
initShineScriptDirectory='/afs/cern.ch/user/n/na61qa/workDirectory/2024NeutrinoRun/'
source $initShineScriptDirectory/initShine.sh

cd $EOSDropDirectory

#List of QA rootfile template names.
qaFiles=("BPDQA" "PSDQA" "MRPCQA" "VDQA" "TPCClusterQA" "TPCVertexQA" "TDAQQA" "GRCClusterQA" "vDrift" "TOFFQA" "DetectorCorrelationQA")
#qaFiles=("TDAQQA")

#Compile plot processor.
cd $pdfMakerDirectory
make clean && make

for qaName in "${qaFiles[@]}"
do
    echo `pwd`

    matchString=''
    if [[ ${#runNumber} -ne 0 ]]
    then
	matchString='run-'$runNumber'x*.'$qaName'.root'
    else
	matchString='*'$qaName'.root'
    fi

    #Get QA histogram files.
    cd $EOSDropDirectory
    ls -rt $EOSDropDirectory'/'$matchString | tail -n $numberOfFiles | sort > haddList$qaName.txt

    #hadd files.
    hadd -k -f $EOSDropDirectory'/'$qaName.root `cat haddList$qaName.txt`
    
    #Export plots to PNG files.
    cd $pdfMakerDirectory
    $pdfMakerDirectory"/ProcessPlots" $EOSDropDirectory"/"$qaName".root"
    #root -b -q 'ProcessPlots.C("'$EOSDropDirectory'/'$qaName'.root")'
    #Remove hadd'ed files and lists of files.
    
    cd $EOSDropDirectory
    #rm $qaName.root
    rm haddList$qaName.txt
done

#Generate PDF.
runString=''
if [[ ${#runNumber} -ne 0 ]]
then
    runString='run-'$runNumber'-'
fi

cd $pdfMakerDirectory
pdflatex OfflineQASlides.tex
qaPDFName="OfflineQA-"$runString`date +"%Y-%m-%d"`"-"`date +"%H.%M.%S"`".pdf"
mv OfflineQASlides.pdf $EOSDropDirectory'/QAPDFs/General/'$qaPDFName

#Clean up.
rm *.aux *.log *.nav *.out *.snm *.toc

exit 0
