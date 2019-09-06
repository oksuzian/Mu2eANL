#/bin/bash
export MU2E_BASE_RELEASE
setup mu2e
source $MU2E_BASE_RELEASE/setup.sh
setup mu2egrid
setup mu2etools
setup gridexport
setup dhtools


MAINDIR=`pwd`
MAKEFLC=1
STAGE=stage1
#STAGE=reco
#STAGE=planes_resampler
#STAGE=dsvacuum_resampler
#STAGE=planes_filter

if [ "$STAGE" == "planes_analyzer" ]; then
    INLIST=submissionLists/beams1_0619_dsregion_filelist.txt
    INFCL=fcl/planes_analyzer.fcl
    TAG=190803205659

elif [ "$STAGE" == "stage1" ]; then
    INFCL=Mu2eANL/JobConfig/cosmic/cosmic_s1_general_cryMT_DSVaccum.fcl
    TAG=190807230524

elif [ "$STAGE" == "planes_filter" ]; then
    INLIST=prestage/beams1_0619.dsregion.txt
    INFCL=fcl/planes_filter.fcl
    TAG=190807230524
elif [ "$STAGE" == "planes_resampler" ]; then
    INFCL=fcl/planes_resampler.fcl
    TAG=190808075558
elif [ "$STAGE" == "dsvacuum_resampler" ]; then
    INFCL=fcl/dsvacuum_resampler.fcl
    TAG=190809124212
elif [ "$STAGE" == "digi" ]; then
    INLIST=prestage/kl_digi_noG4.txt
    INFCL=Offline/JobConfig/primary/CRY-offspill.fcl
    TAG=190813162936
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
CODE=/pnfs/mu2e/resilient/users/oksuzian/gridexport/tmp.4RuQJ4MEaD/Code.tar.bz

OUTDIR=${DSCONF}_${TAG}

if [ "$MAKEFLC" -eq 1 ]
then
TAG=`date +"%y%m%d%H%M%S"`
OUTDIR=${DSCONF}_${TAG}
OUTPNFS=/pnfs/mu2e/scratch/users/oksuzian/workflow/${OUTDIR}/

# Generate fcl files
mkdir backup/$OUTDIR
echo "Generating fcl files in: " backup/$OUTDIR
if [ "$STAGE" == "planes_analyzer" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=50 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "stage1" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --njobs=10 --events-per-job=100000 --run-number=2700 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "planes_filter" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=50 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "planes_resampler" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --njobs=1000 --events-per-job=187903 --run=2705 --embed ${MAINDIR}/${INFCL} --aux=1:physics.filters.flashResample.fileNames:${MAINDIR}/prestage/planes_resampler_noG4filt.txt)
elif [ "$STAGE" == "dsvacuum_resampler" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --njobs=1000 --events-per-job=66836 --run=2705 --embed ${MAINDIR}/${INFCL}  --aux=1:physics.filters.dsResample.fileNames:${MAINDIR}/prestage/dsvacuum_resampler_noG4.txt)
elif [ "$STAGE" == "digi" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=2 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "reco" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=2 --embed ${MAINDIR}/${INFCL})
elif  [[ "$STAGE" == "concat"* ]]; then
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
if [ "$STAGE" == "mix" ]; then
    RESOURCE="--disk=20GB --memory=8GB"
fi

command="mu2eprodsys --clustername="${JN}" --fcllist=$SF --wfproject=$WFPROJ --dsconf=$DSCONF \
   --dsowner=oksuzian --OS=SL7 ${RESOURCE} --expected-lifetime=23h --code=$CODE \
   --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC"
echo "Submitting: " $command
echo `$command` > $LN 2>&1
RC=$?

JID=`grep -oP "cluster\s+\K\w+" $LN`
echo "RC=$RC JID=$JID"
if [ $RC -eq 0 ]; then
  DD=`dirname $SF`
fi


