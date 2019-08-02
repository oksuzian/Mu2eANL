#/bin/bash
#setup mu2e
#source Offline/setup.sh
setup mu2egrid
setup mu2etools
setup gridexport
setup dhtools

MAINDIR=`pwd`
MAKEFCL=0
STAGE=planes_resampler
TAG=1908011015

if [ "$STAGE" == "planes_filter" ]; then
    INLIST=submissionLists/beams1_0619_dsregion_filelist.txt
    INFCL=fcl/planes_filter.fcl
   # TAG=190727225743
elif [ "$STAGE" == "planes_resampler" ]; then
    INFCL=../Fcl/BeamSim_OutputProcessing/Yuri_fcl_Series/planes_resampler.fcl
  #  TAG=1908010855
elif [ "$STAGE" == "dsvacuum_resampler" ]; then
    INFCL=fcl/dsvacuum_resampler.fcl
   # TAG=1907271030
else
    echo "Not a valid stage"
    exit
fi

DSCONF=beam${STAGE}_0619
SETUPFN=./Offline/setup.sh
WFPROJ=beam_0619
CODE=/pnfs/mu2e/resilient/users/oksuzian/gridexport/tmp.Z7BNyFuPp0/Code.tar.bz
OUTDIR=${DSCONF}_${TAG}

if [ "$MAKEFCL" -eq 1 ]
then
#TAG=`date +"%y%m%d%H%M%S"`
OUTDIR=${DSCONF}_${TAG}
OUTPNFS=/pnfs/mu2e/scratch/users/bbarton/workflow/${OUTDIR}/

# Generate fcl files
mkdir Backup/$OUTDIR
echo "Generating fcl files in: " Backup/$OUTDIR
if [ "$STAGE" == "planes_filter" ]; then
    (cd Backup/$OUTDIR && generate_fcl --desc=sim --dsowner=bbarton --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=100 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "planes_resampler" ]; then
    #(cd Backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --njobs=10 --events-per-job=1604050 --run=2701 --embed ${MAINDIR}/${INFCL})
    (cd Backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --njobs=10 --events-per-job=1604050 --run=2701 --embed ${MAINDIR}/${INFCL} --aux=1:physics.filters.flashResample.fileNames:/mu2e/app/users/bbarton/Mu2eANL/Scripts/SubmissionLists/planes_resample_filtered.txt)
elif [ "$STAGE" == "dsvacuum_resampler" ]; then
    (cd Backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --njobs=10 --events-per-job=150000 --run=2705 --embed ${MAINDIR}/${INFCL})
elif  [[ "$STAGE" == "concat"* ]]; then
    (cd Backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=600  --embed ${MAINDIR}/${INFCL})
else
    echo "Can't generate fcl's. Not a valid stage"
    exit
fi

#Save enviroment variable
printenv > Backup/$OUTDIR/vars.txt 2>&1
# Copy fcl files to pnfs area
mkdir $OUTPNFS
(for dir in Backup/$OUTDIR/???; do ls $dir/*.fcl; done) | while read FF; do echo "Working on:" $FF; ifdh cp $FF $OUTPNFS; done
#Make  fcl lists
(for dir in $OUTPNFS; do ls $dir/*.fcl; done) | while read FF; do echo $FF; done | split -l 10000 -d - Backup/$OUTDIR/fcllist.
exit
fi

# Submit jobs on grid
SF=Backup/${OUTDIR}/fcllist.00
BN=`basename $SF | cut -d. -f 1`
JN="${BN}_${TAG}"
LN=Logs/submit_${JN}.log

RESOURCE="--disk=20GB --memory=1750MB"
if [ "$STAGE" == "mix" ]; then
    RESOURCE="--disk=20GB --memory=8GB"
fi

command="mu2eprodsys --clustername="${JN}" --fcllist=$SF --wfproject=$WFPROJ --dsconf=$DSCONF \
   --dsowner=bbarton --OS=SL7 ${RESOURCE} --expected-lifetime=23h --code=$CODE \
   --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC"
echo "Submitting: " $command
echo `$command` > $LN 2>&1
RC=$?

JID=`grep -oP "cluster\s+\K\w+" $LN`
echo "RC=$RC JID=$JID"
if [ $RC -eq 0 ]; then
  DD=`dirname $SF`
fi


