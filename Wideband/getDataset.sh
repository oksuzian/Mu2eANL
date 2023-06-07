#!/bin/bash

source /cvmfs/mu2e.opensciencegrid.org/setupmu2e-art.sh
setup mu2efiletools
setup dhtools
samListLocations --defname $1
#mu2eDatasetFileList $1
