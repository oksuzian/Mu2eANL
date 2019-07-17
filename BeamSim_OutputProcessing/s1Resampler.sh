## Submit a grid job which resamples beam s1 ouput files by processing each input
## Ben Barton
## July, 2019


#!/bin/bash


setup mu2e
source Offline/setup.sh
setup mu2egrid
setup mu2etools
setup gridexport
setup dhtools


RESAMPLE_FACTOR=1 # The multiplicative factor to resample by
INLIST=  # The list of the output files from the K-long + neutron filter
INFCL=   # The base fcl to use to create job fcls
MERGE=27 # Combine all filtered files into a single job since they are each small
CODE= ######### Path to tarball

DSCONF=beams2-resample-neutrals
DSOWNER=bbarton
WFPROJ=beam_0619

TAG=`date + "%y%m%d%H%M"`
MAINDIR=`pwd`
OUTDIR=${DSCONF}_${TAG}
OUTPNFS=/pnfs/mu2e/scratch/users/bbarton/workflow/${OUTDIR}

OLD_SEEDS=/dev/null #Will be updated after each iteration


# Generate fcl files
mkdir backup/${OUTDIR}
touch backup/${OUTDIR}/old_seeds.txt
OLD_SEEDS=../old_seeds.txt

for iter in {0..${RESAMPLE_FACTOR}}
do
    mkdir backup/${OUTDIR}/${iter}
    (cd backup/$OUTDIR/${iter} && generate_fcl --desc=sim --dsowner=$DSOWNER --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=$MERGE --embed ${MAINDIR}/${INFCL} --old-seeds=$OLD_SEEDS)
    cat seeds.${OWNER}.sim.${DSCONF}.????.txt >> ${OLD_SEEDS}  # Append the seeds that were used to the master seeds file
done


# Copy fcl files to pnfs area
mkdir $OUTPNFS
(for dir in backup/$OUTDIR/*/???; do ls $dir/*.fcl; done) | while read FF; do echo "Working on:" $FF; ifdh cp $FF $OUTPNFS; done
#Make  fcl lists
(for dir in $OUTPNFS; do ls $dir/*.fcl; done) | while read FF; do echo $FF; done | split -l 10000 -d - backup/$OUTDIR/fcllist.


# Submit jobs on grid
SF=backup/${OUTDIR}/fcllist.00
BN=`basename $SF | cut -d. -f 1`
JN="${BN}_${TAG}"
LN=logs/submit_${JN}.log

RESOURCE="--disk=20GB --memory=1750MB"

COMMAND="mu2eprodsys --clustername="${JN}" --fcllist=$SF --wfproject=$WFPROJ --dsconf=$DSCONF \
   --dsowner=bbarton --OS=SL7 ${RESOURCE} --expected-lifetime=23h --code=$CODE \
   --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC"
echo "Submitting: " $COMMAND
echo `$command` > $LN 2>&1
RC=$?

JID=`grep -oP "cluster\s+\K\w+" $LN`
echo "RC=$RC JID=$JID"
if [ $RC -eq 0 ]; then
  DD=`dirname $SF`
fi



