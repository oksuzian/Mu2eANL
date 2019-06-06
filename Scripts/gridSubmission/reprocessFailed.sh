#Locate failed jobs and reprocess them on grid
#Ben Barton

#USAGE: ./reprocessFailed.sh <directory name>

#/bin/bash

#Setup necessary environment
#setup mu2e
#source /mu2e/app/users/bbarton/Offline/setup.sh
setup mu2egrid
setup mu2etools
setup gridexport
setup dhtools

REPROCDIR=19374430.fcllist_190604094535
INPATH=/pnfs/mu2e/scratch/users/bbarton/workflow/CRY_MT2/outstage/${REPROCDIR}/00/

findResults=( $(find ${INPATH} -mindepth 1 -maxdepth 1 -type d '!' -exec sh -c 'ls -1 "{}"|egrep -i -q "*.art"' ';' -print) )

FCLLIST=/mu2e/app/users/bbarton/recovery_${REPROCDIR}.fcllist

for res in ${findResults[@]}
do
    grep 'origFCL=' ${res}/log.* | cut -c9-110 >> /mu2e/app/users/bbarton/recovery_${REPROCDIR}.fcllist
done


