#/bin/bash
export MU2E_BASE_RELEASE
OFFLINE="/cvmfs/mu2e.opensciencegrid.org/Offline/v7_5_4/SLF7/prof/Offline"
setup mu2e
source $OFFLINE/setup.sh
setup mu2egrid
setup mu2etools
setup gridexport
setup dhtools

MAINDIR=`pwd`
PROD=0
USECODE=1
MAKEFLC=0
#STAGE=stage1
STAGE=minedep_filter
#STAGE=concat
#STAGE=resampler
#STAGE=digi
#STAGE=reco

if [ "$STAGE" == "dsvacuum_analyzer" ]; then
    INLIST=submissionLists/beams1_0619_dsregion_filelist.txt
    INFCL=Mu2eANL/Analyses/fcl/analyzeFirstStepMC.fcl
    TAG=

elif [ "$STAGE" == "stage1" ]; then
    INFCL=Mu2eANL/JobConfig/cosmic/cosmic_s1_general.fcl
    TAG=190909133703

elif [ "$STAGE" == "minedep_filter" ]; then
    INLIST=Mu2eANL/JobConfig/FileLists/cry-rs1-1019.txt
    INFCL=Mu2eANL/JobConfig/cosmic/filterGapEvents.fcl
    TAG=191106224859

elif [ "$STAGE" == "concat" ]; then
    INLIST=Mu2eANL/JobConfig/FileLists/cry0919_minedep-filter.round7.txt
    INFCL=Mu2eANL/JobConfig/cosmic/concat.fcl
    TAG=191019181515

elif [ "$STAGE" == "resampler" ]; then
    INFCL=Mu2eANL/JobConfig/cosmic/dsstops_resampler.fcl
#    TAG=191025001731
    TAG=191102230332 #upstream

elif [ "$STAGE" == "digi" ]; then
#    INLIST=Mu2eANL/JobConfig/FileLists/cry0919_s1-resampled-dsvacuum.round8.txt
    INLIST=Mu2eANL/JobConfig/FileLists/cry0919_s1-resampled-dsvacuum.upstream.round8.txt
    INFCL=JobConfig/primary/CRY-offspill.fcl
#    TAG=191028220434
    TAG=191106104034 # upstream

elif [ "$STAGE" == "reco" ]; then
    INLIST=Mu2eANL/JobConfig/FileLists/cry0919_digi.round8.upstream.txt
    INFCL=JobConfig/reco/mcdigis_CRY.fcl
    TAG=191106213751
else
    echo "Not a valid stage"
    exit
fi

DSCONF=cry-${STAGE}-10-5
SETUPFN=${MU2E_BASE_RELEASE}/setup.sh
WFPROJ=cry_rs1_1019_g4_10_5
#/pnfs/mu2e/resilient/users/oksuzian/gridexport/tmp.48Ufqzb95I/Code.tar.bz
CODE=/pnfs/mu2e/resilient/users/oksuzian/gridexport/tmp.gCkGshOuDY/Code.tar.bz

OUTDIR=${DSCONF}_${TAG}

if [ "$MAKEFLC" -eq 1 ]
then
TAG=`date +"%y%m%d%H%M%S"`
OUTDIR=${DSCONF}_${TAG}
OUTPNFS=/pnfs/mu2e/scratch/users/oksuzian/workflow/${OUTDIR}/
if [ "$PROD" -eq 1 ]; then
    OUTPNFS=/pnfs/mu2e/persistent/users/oksuzian/workflow/${OUTDIR}/
fi

# Generate fcl files
mkdir backup/$OUTDIR
echo "Generating fcl files in: " backup/$OUTDIR
if [ "$STAGE" == "dsvacuum_analyzer" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=50 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "stage1" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --njobs=2000 --events-per-job=5000000 --run-number=2700 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "minedep_filter" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=371 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "resampler" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --njobs=10000 --events-per-job=300000 --run=2701 --embed ${MAINDIR}/${INFCL}  --aux=1:physics.filters.dsResample.fileNames:${MAINDIR}/Mu2eANL/JobConfig/FileLists/cry0919_minedep-filter.upstream.round8.txt)
elif [ "$STAGE" == "digi" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=10 --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "reco" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=10 --embed ${MAINDIR}/${INFCL})
elif  [[ "$STAGE" == "concat" ]]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=18  --embed ${MAINDIR}/${INFCL})
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
(find $OUTPNFS -name cnf*.fcl) | split -l 10000 -d - backup/$OUTDIR/fcllist.
exit
fi

# Submit jobs on grid
SF=backup/${OUTDIR}/fcllist.00
#SF=/mu2e/data/users/oksuzian/projects/cry_rs1019/fcllist.01
BN=`basename $SF | cut -d. -f 1`
JN="${BN}_${TAG}"
LN=logs/submit_${JN}.log

RESOURCE="--disk=20GB --memory=1750MB"
if [ "$STAGE" == "stage1" ] || [ "$STAGE" == "resampler" ]; then
    RESOURCE="--disk=20GB --memory=3500MB"
elif [ "$STAGE" == "minedep_filter" ]; then
    RESOURCE="--disk=100GB --memory=3500MB"
fi

SETUP="--setup=$OFFLINE/setup.sh"
if [ "$USECODE" -eq 1 ]; then
    SETUP="--code=$CODE"
fi

SETPROD=""
if [ "$PROD" -eq 1 ]; then
    SETPROD="--role=Production"
fi

#   --dsowner=oksuzian --role=Production --OS=SL7 ${RESOURCE} --expected-lifetime=23h 
#   --resource-provides=usage_model=OFFSITE --site=BNL,Caltech,FNAL,FermiGrid,Nebraska,Omaha,SU-OG,Wisconsin,UCSD,NotreDame"
#   --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC"
command="mu2eprodsys --clustername="${JN}" --fcllist=$SF --wfproject=$WFPROJ --dsconf=$DSCONF \
   --dsowner=oksuzian --OS=SL7 ${RESOURCE} --expected-lifetime=23h ${SETUP} ${SETPROD} \
   --resource-provides=usage_model=DEDICATED,OPPORTUNISTIC,OFFSITE --site=BNL,Caltech,FNAL,FermiGrid,Nebraska,Omaha,SU-OG,Wisconsin,UCSD,NotreDame"

echo "Submitting: " $command
echo `$command` > $LN 2>&1
RC=$?

JID=`grep -oP "cluster\s+\K\w+" $LN`
echo "RC=$RC JID=$JID"
if [ $RC -eq 0 ]; then
  DD=`dirname $SF`
fi


