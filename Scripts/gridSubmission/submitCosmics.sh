#/bin/bash
setup mu2e
source Offline/setup.sh
setup mu2egrid
setup mu2etools
setup gridexport
setup dhtools

DSCONF=digi
MAINDIR=`pwd`
WFPROJ=CRY_MT2
TAG=`date +"%y%m%d%H%M%S"`
OUTDIR=${DSCONF}_${TAG}
OUTPNFS=/pnfs/mu2e/scratch/users/oksuzian/workflow/${OUTDIR}/
SF=pnfs_fcl/${OUTDIR}/fcllist.00
BN=`basename $SF | cut -d. -f 1`
JN="${BN}_${TAG}"
LN=logs/submit_${JN}.log
SETUPFN=./Offline/setup.sh
INLIST=/mu2e/app/users/oksuzian/Offline_v7_2_0_CRYMT/cry_filelist.txt
MERGE=10
CODE=/pnfs/mu2e/resilient/users/oksuzian/gridexport/tmp.CYPjF7hIm0/Code.tar.bz

mkdir $OUTPNFS
mkdir pnfs_fcl/$OUTDIR

submit_job () {

    #Save enviroment variable
    printenv > pnfs_fcl/$OUTDIR/vars.txt 2>&1

    RESOURCE="--disk=20GB --memory=5000MB"
#    RESOURCE="--disk=10GB --memory=12000MB"
    command="mu2eprodsys --clustername="${JN}" --fcllist=$SF --wfproject=$WFPROJ --dsconf=$DSCONF \
      --dsowner=oksuzian --OS=SL6 ${RESOURCE} --expected-lifetime=23h --code=$CODE \
      --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC"
    echo "Sumbitting: " $command
    echo `$command` > $LN 2>&1
    RC=$?

    JID=`grep -oP "cluster\s+\K\w+" $LN`
    echo "RC=$RC JID=$JID"
    if [ $RC -eq 0 ]; then
	DD=`dirname $SF`
    fi

}


echo "Running stage: ${DSCONF}" 

if [ "$DSCONF" == "CRY" ]; then
    INFCL=Offline/JobConfig/cosmic/cosmic_s1_general_cryMT.fcl
elif [ "$DSCONF" == "DYB" ]; then
    INFCL=Offline/JobConfig/cosmic/cosmic_s1_general_dbay.fcl
elif [ "$DSCONF" == "digi" ]; then
    INFCL=Offline/JobConfig/cosmic/cosmic_s3_general_cry.fcl
#    INLIST=/mu2e/app/users/oksuzian/Offline_cryAdjustableBox/filelist_cry2_uniq.txt
    INLIST=/mu2e/app/users/oksuzian/Offline_cryAdjustableBox/filelist_cry1_concat.txt
    MERGE=10
elif [ "$DSCONF" == "reco" ]; then
    INFCL=Offline/JobConfig/reco/RecoMCDigisTrig_CRY.fcl
#    INLIST=/mu2e/app/users/oksuzian/Offline_cryAdjustableBox/cry_filelist_trkana_cry1_miss2.txt
    INLIST=/mu2e/app/users/oksuzian/Offline_cryAdjustableBox/filelist_cry2_digi.txt
    MERGE=5

elif [ "$DSCONF" == "rerun" ]; then
    SF=pnfs_fcl/digi_190202095834/fcllist.01
    submit_job
    exit 0
else
    echo "Unknown configuration"
    exit 0
fi

# Generate fcl files
mkdir fcl_files/$OUTDIR
echo "Generating fcl files in: " fcl_files/$OUTDIR
#(cd fcl_files/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --events-per-job=100000 --njobs=1000 --run=2705 --embed ${MAINDIR}/${INFCL})
(cd fcl_files/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${INLIST} --merge=${MERGE}  --embed ${MAINDIR}/${INFCL})
#(for dir in fcl_files/$OUTDIR/???; do ls $dir/*.fcl; done) | while read FF; do echo "Working on:" $FF; done
# Make  fcl lists
#(for dir in fcl_files/$OUTDIR/???; do ls $dir/*.fcl; done) | while read FF; do echo $FF; done | split -l 2000 -d - fcl_files/$OUTDIR/fcllist.
#echo "Start copying to pnfs"
#(find fcl_files/$OUTDIR/???/ -name "*.fcl") | while read FF; do echo $FF; done | split -l 100 -d - fcl_files/$OUTDIR/fcllist.

if [ "$DSCONF" == "digi" ] || [ "$DSCONF" == "reco" ] || [ "$DSCONF" == "trkana" ] || [ "$DSCONF" == "CRY" ] ; then
    # Copy fcl files to pnfs area
    (for dir in fcl_files/$OUTDIR/???; do ls $dir/*.fcl; done) | while read FF; do echo "Working on:" $FF; ifdh cp $FF $OUTPNFS; done
    #Make pnfs fcl lists
    (for dir in $OUTPNFS; do ls $dir/*.fcl; done) | while read FF; do echo $FF; done | split -l 10000 -d - pnfs_fcl/$OUTDIR/fcllist.
    submit_job
fi