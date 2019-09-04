#/bin/bash
setup mu2e
source Offline/setup.sh
setup mu2egrid
setup mu2etools
setup gridexport
setup dhtools

MAINDIR=`pwd`
SECTOR=Top

if [ "$SECTOR" == "Top" ]; then
    INFCL=Offline/CRVResponse/efficiencyCheck/CRV_Efficiency_check_CRTS.fcl
else
    echo "Not a valid stage"
    exit
fi

DSCONF=CRV_EffCheck${SECTOR}
SETUPFN=./Offline/setup.sh
WFPROJ=v703
CODE=/pnfs/mu2e/resilient/users/oksuzian/gridexport/tmp.92ddXDuxQr/Code.tar.bz
OUTDIR=${DSCONF}_${TAG}

TAG=`date +"%y%m%d%H%M%S"`
OUTDIR=${DSCONF}_${TAG}
OUTPNFS=/pnfs/mu2e/scratch/users/bbarton/workflow/${OUTDIR}/
# Generate fcl files
mkdir backup/$OUTDIR
echo "Generating fcl files in: " backup/$OUTDIR
(cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=bbarton --dsconf=$DSCONF --events-per-job=200000 --njobs=50 --run=2700 --embed ${MAINDIR}/${INFCL})
#Save enviroment variable
printenv > backup/$OUTDIR/vars.txt 2>&1
# Copy fcl files to pnfs area
mkdir $OUTPNFS
(for dir in backup/$OUTDIR/???; do ls $dir/*.fcl; done) | while read FF; do echo "Working on:" $FF; ifdh cp $FF $OUTPNFS; done
# Make  fcl lists
(for dir in $OUTPNFS; do ls $dir/*.fcl; done) | while read FF; do echo $FF; done | split -l 10000 -d - backup/$OUTDIR/fcllist.

# Submit jobs on grid
SF=backup/${OUTDIR}/fcllist.00
BN=`basename $SF | cut -d. -f 1`
JN="${BN}_${TAG}"
LN=logs/submit_${JN}.log
RESOURCE="--disk=5GB --memory=2100MB"
command="mu2eprodsys --clustername="${JN}" --fcllist=$SF --wfproject=$WFPROJ --dsconf=$DSCONF \
   --dsowner=bbarton --OS=SL6 ${RESOURCE} --expected-lifetime=8h --code=$CODE \
   --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC,OFFSITE \
   --site=BNL,Caltech,FNAL_GPGrid,FNAL,Omaha,SU-OG,Wisconsin,UCSD,Nebraska"
echo "Sumbitting: " $command
echo `$command` > $LN 2>&1
RC=$?

JID=`grep -oP "cluster\s+\K\w+" $LN`
echo "RC=$RC JID=$JID"
if [ $RC -eq 0 ]; then
  DD=`dirname $SF`
fi


