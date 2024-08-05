#!/bin/bash

#Clean out stored chunks.

find /afs/cern.ch/user/n/na61qa/workDirectory/2024NeutrinoRun/EOSDropDirectory/ChunkStorage/ -maxdepth 1 -name "*.pteraw" -delete
