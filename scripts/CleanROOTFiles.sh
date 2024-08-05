#!/bin/bash

#Clean out root files.
find /afs/cern.ch/user/n/na61qa/workDirectory/2024NeutrinoRun/EOSDropDirectory/ -maxdepth 1 -name "*.root" -delete

#Clean out .png plots.
rm /afs/cern.ch/user/n/na61qa/workDirectory/2024NeutrinoRun/pdfMaker/plots/*.png
