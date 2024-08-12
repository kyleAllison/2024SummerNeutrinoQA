Welcome to OfflineQA! This framework was developed by Brant Rumberger
for the 2022 IonRun NA61 data taking campaign. It was built using tools
and contributions from many NA61 students and collaborators.

This QA framework runs an instance of SHINE Offline on every chunk of
data that is sent to CTA. It also send the trackMatchDump.root file for permanent storage on eos, 
to save time when the data set is being calibrated. The Module Sequence and configuration files
used are located in the directory
/afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/OfflineQA/NativeReconstruction

Running of this Module Sequence can be done locally by executing the
script
/afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/OfflineQA/NativeReconstruction/CopyFromCTAAndRunSHINE.sh -i [inputFile.pteraw] -d [Full directory path (CTA, EOS, etc)] -g [global key] 

If running in this local mode, the output ROOT files will appear in
the /afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/OfflineQA/NativeReconstruction/ directory.

This Module Sequence can be sent to run as an HTCondor job by running the script
/afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/OfflineQA/scripts/SubmitQAJob.sh [inputFile.pteraw] [Full directory path (CTA, EOS, etc)] 

The output will appear instead in the direcrory
/afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/EOSDropDirectory/. The SHINE logs will appear
in the directory /afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/log.bz2/. The HTCondor logs
will appear in the directory
/afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/work/[JobSubmissionTime].

In the Module Sequence, QA modules are run (currently TPCClusterQA,
TPCVertexQA, and TDAQQA). These modules produce histograms that can be
combined later and written to a QA PDF.

To create a QA PDF, call the script
/afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/OfflineQA/scripts/CreateQAPDF.sh. This will create a QA
PDF using all of the ROOT files currently in EOSDropDirectory. Be
careful here! To remove the ROOT files in EOSDropDirectory, run the
script /afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/OfflineQA/scripts/CleanROOTFiles.sh.

To clean the logs, call the command
/afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/OfflineQA/scripts/CleanLogs.sh.

As files are copied from the DAQ to CTA, they sit in a ssd for ~20 minutes, and can be copied
over to eos for reconstruction.

DAQ should run the scripts/SubmitQAJobs.sh for each chunk recorded. This submits one job
per chunk, and all of the relevant vars are set at the top of the condor_submit.sh script.

For debugging, uncomment the "exit 0" at the beginning of the scripts/SubmitQAJobs.sh to not have thousands of 
failing jobs during data taking.

NOTE: Don't leave large files from tests in the
.../SHINEReconstruction-master_slc7/ directory. These will be copied
with _every_ job submitted to the .../work/ directory, and this will
fill up the home directory (which has a 10 GB quota! FIXME!).

SubmitQAJob-ExistingChunk.sh: For testing job submission/rec on one chunk in chunk storage.

CleanChunkStorage.sh: Removes all chunks in storage, be careful about this! You might want to keep some.

CleanLogStorage.sh: Cleans logs, keeps last 2 and first 100 for currently running jobs/dirs.

CleanRootFiles.sh: Cleans up the output qa.root files

WipeAndRestart.sh: Calls all clean scripts.

RunDailyQA.sh: Call the CreateQAPDF.sh script.

CreatQAPDF.sh: hadds each detectors qa.root files, has a list of those files at the top of the script.
	       So, you would have to add "TOFF" for example. Then, calls ProcessPlots.cc for each
	       detectors haddedQA.root file. After the ProcessPlots.cc code is done, it calls
	       pdflatex OfflineQASlide.tex

ProcessPlots.cc: This takes the haddedQA.root file, and takes the hists and converts to .png 
		 This is where hist options like "LogX", "Normalize", ... can be set for 
		 specific hists, by adding the desired hist name to the options. 

OfflineQASlides.tex: This takes all of the .png files in the plots folder, and makes a pdf
		     out of them. Each plot has to be called by name.


If everything is workin perfectly, then the daily QA can be run with just:

RunDailyQA.sh followed by WipeAndRestart.sh

lxplus supports scheduled cron jobs. They can be added with the command:
acrontab -e
and adding a line for the task. Example is for 7 am CERN time:
0 7 * * * lxplus.cern.ch /afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/OfflineQA/scripts/CreateQAPDF.sh &>>/afs/cern.ch/user/n/na61qa/workDirectory/2023SummerNeutrinoRun/OfflineQA/scripts/test.log

Have fun!!
-Brant, November 13th 2022
-Modified July 6th 2023 by Andras + Kyle
