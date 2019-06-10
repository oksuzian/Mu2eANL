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
OUTPNFS=/pnfs/mu2e/scratch/users/bbarton/workflow/${OUTDIR}/
SF=pnfs_fcl/${OUTDIR}/fcllist.00
BN=`basename $SF | cut -d. -f 1`
JN="${BN}_${TAG}"
LN=logs/submit_${JN}.log
SETUPFN=./Offline/setup.sh
INLIST=/mu2e/app/users/oksuzian/Offline_v7_2_0_CRYMT/cry_filelist.txt
MERGE=10
CODE=/pnfs/mu2e/resilient/users/bbarton/gridexport/tmp.OiWE4lHVkR/Code.tar.bz

REPROCDIR=19374430.fcllist_190604094535
INPATH=/pnfs/mu2e/scratch/users/bbarton/workflow/CRY_MT2/outstage/${REPROCDIR}/00/


mkdir $OUTPNFS
mkdir pnfs_fcl/$OUTDIR

submit_job () {

    #Save enviroment variable
    printenv > pnfs_fcl/$OUTDIR/vars.txt 2>&1

    RESOURCE="--disk=20GB --memory=5000MB"
#    RESOURCE="--disk=10GB --memory=12000MB"
    command="mu2eprodsys --clustername="${JN}" --fcllist=$SF --wfproject=$WFPROJ --dsconf=$DSCONF \
      --dsowner=bbarton --OS=SL6 ${RESOURCE} --expected-lifetime=23h --code=$CODE \
      --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC"
    echo "Submitting: " $command
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
    #INFCL=Offline/JobConfig/cosmic/cosmic_s3_general_cry.fcl
    INFCL=Offline/JobConfig/primary/CRY-offspill.fcl ##Updated fcl file as of 6/9/19
#   INLIST=/mu2e/app/users/oksuzian/Offline_cryAdjustableBox/filelist_cry2_uniq.txt
#   INLIST=/mu2e/app/users/oksuzian/Offline_cryAdjustableBox/filelist_cry1_concat.txt
    INLIST=/mu2e/app/users/bbarton/submissionLists/cry2_digiList.txt
    MERGE=500 #10 for CRY1 because they were already concatenated
elif [ "$DSCONF" == "reco" ]; then
    INFCL=Offline/JobConfig/reco/mcdigis_primary.fcl  #####This is useful for a purely cosmic CRY sample
    #INFCL=Offline/JobConfig/reco/mcdigis_CRY.fcl  ##This is useful for a mixed cry sample
#    INLIST=/mu2e/app/users/oksuzian/Offline_cryAdjustableBox/cry_filelist_trkana_cry1_miss2.txt
    INLIST=/mu2e/app/users/bbarton/submissionLists/cry1_RecoList.txt
    MERGE=5

elif [ "$DSCONF" == "rerun" ]; then
    #Locate directories of failed jobs
    findResults=( $(find ${INPATH} -mindepth 1 -maxdepth 1 -type d '!' -exec sh -c 'ls -1 "{}"|egrep -i -q "*.art"' ';' -print) )

    #For each directory of a failed job, search the log file for the fcl file that needs to be reprocessed and write it to a fcllist file
    for res in ${findResults[@]}
    do
	grep 'origFCL=' ${res}/log.* | cut -c9-110 >> /mu2e/app/users/bbarton/recovery_${REPROCDIR}.fcllist
    done

    SF=/mu2e/app/users/bbarton/recovery_${REPROCDIR}.fcllist

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