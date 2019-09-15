#/bin/bash
export MU2E_BASE_RELEASE
setup mu2e
source $MU2E_BASE_RELEASE/setup.sh
setup mu2egrid
setup mu2etools
setup gridexport
setup dhtools


MAINDIR=`pwd`
MAKEFLC=0
#STAGE=stage1
#STAGE=minedep_filter
STAGE=dsvacuum_resampler
#STAGE=digi
#STAGE=reco



if [ "$STAGE" == "dsvacuum_analyzer" ]; then
    INLIST=submissionLists/beams1_0619_dsregion_filelist.txt
    INFCL=Mu2eANL/Analyses/fcl/StepPointMC1stHitDumper_analyzer.fcl
    TAG=

elif [ "$STAGE" == "stage1" ]; then
    INFCL=Mu2eANL/JobConfig/cosmic/cosmic_s1_general_cryMT_DSVaccum.fcl
    TAG=190909133703

elif [ "$STAGE" == "minedep_filter" ]; then
    INLIST=Mu2eANL/JobConfig/FileLists/cry0919_1k_round2.txt
    INFCL=Mu2eANL/Filters/fcl/FilterMinEdepCRV_filter.fcl
    TAG=190911085549

elif [ "$STAGE" == "dsvacuum_resampler" ]; then
    INFCL=Mu2eANL/JobConfig/cosmic/dsvacuum_resampler.fcl
    TAG=190914203500

elif [ "$STAGE" == "digi" ]; then
    INLIST=Mu2eANL/JobConfig/FileLists/cry0919_cry0919_s1-resampled-dsvacuum.round2.txt
    INFCL=JobConfig/primary/CRY-offspill.fcl
    TAG=190913125527

elif [ "$STAGE" == "reco" ]; then
    INLIST=prestage/kl_reco_noG4.txt
    INFCL=Offline/JobConfig/reco/mcdigis_CRY.fcl
    TAG=190813224125
else
    echo "Not a valid stage"
    exit
fi

DSCONF=cry${STAGE}_0919
SETUPFN=${MU2E_BASE_RELEASE}/setup.sh
WFPROJ=cry_0919
CODE=/pnfs/mu2e/resilient/users/oksuzian/gridexport/tmp.GxB7O2hbiS/Code.tar.bz

OUTDIR=${DSCONF}_${TAG}

if [ "$MAKEFLC" -eq 1 ]
then
TAG=`date +"%y%m%d%H%M%S"`
OUTDIR=${DSCONF}_${TAG}
OUTPNFS=/pnfs/mu2e/scratch/users/oksuzian/workflow/${OUTDIR}/

# Generate fcl files
mkdir backup/$OUTDIR
echo "Generating fcl files in: " backup/$OUTDIR
if [ "$STAGE" == "dsvacuum_analyzer" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=50 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "stage1" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --njobs=2000 --events-per-job=5000000 --run-number=2700 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "minedep_filter" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=20 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "dsvacuum_resampler" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --njobs=500 --events-per-job=526000 --run=2701 --embed ${MAINDIR}/${INFCL}  --aux=1:physics.filters.dsResample.fileNames:${MAINDIR}/Mu2eANL/JobConfig/FileLists/cry0919_minedep-filter.nocat.1k_round2.txt)
elif [ "$STAGE" == "digi" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=5 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "reco" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=2 --embed ${MAINDIR}/${INFCL})
elif  [[ "$STAGE" == "concat" ]]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=600  --embed ${MAINDIR}/${INFCL})
else
    echo "Can't generate fcl's. Not a valid stage"
    exit
fi

#Save enviroment variable
printenv > backup/$OUTDIR/vars.txt 2>&1
# Copy fcl files to pnfs area
mkdir $OUTPNFS
(for dir in backup/$OUTDIR/???; do ls $dir/*.fcl; done) | while read FF; do echo "Working on:" $FF; ifdh cp $FF $OUTPNFS; done
#Make  fcl lists
(for dir in $OUTPNFS; do ls $dir/*.fcl; done) | while read FF; do echo $FF; done | split -l 10000 -d - backup/$OUTDIR/fcllist.
exit
fi

# Submit jobs on grid
SF=backup/${OUTDIR}/fcllist.00
BN=`basename $SF | cut -d. -f 1`
JN="${BN}_${TAG}"
LN=logs/submit_${JN}.log

RESOURCE="--disk=20GB --memory=1750MB"
if [ "$STAGE" == "stage1" ] || [ "$STAGE" == "dsvacuum_resampler" ]; then
    RESOURCE="--disk=20GB --memory=3500MB"
fi

#   --dsowner=oksuzian --OS=SL7 ${RESOURCE} --expected-lifetime=23h --code=$CODE \

command="mu2eprodsys --clustername="${JN}" --fcllist=$SF --wfproject=$WFPROJ --dsconf=$DSCONF \
   --dsowner=oksuzian --OS=SL7 ${RESOURCE} --expected-lifetime=23h --setup=/cvmfs/mu2e.opensciencegrid.org/Offline/v7_5_4/SLF7/prof/Offline/setup.sh \
   --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC"
echo "Submitting: " $command
echo `$command` > $LN 2>&1
RC=$?

JID=`grep -oP "cluster\s+\K\w+" $LN`
echo "RC=$RC JID=$JID"
if [ $RC -eq 0 ]; then
  DD=`dirname $SF`
fi


