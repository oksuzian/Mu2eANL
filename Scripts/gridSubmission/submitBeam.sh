#/bin/bash
setup mu2e
source Offline/setup.sh
setup mu2egrid
setup mu2etools
setup gridexport
setup dhtools

MAINDIR=`pwd`
MAKEFLC=0
STAGE=s2
#MIXFILES="V703RegConc"
MIXFILES="V703_07CB4"

if [ "$STAGE" == "s2" ]; then
#    INLIST=prestage/sim.mu2e.cd3-beam-g4s1-dsregion.051017.txt
#    INLIST=prestage/sim.oksuzian.mu2eII-beam.g4s1-dsregion.txt
    INLIST=submissionLists/beams1_0619_dsregion_filelist.txt
  #  INFCL=/mu2e/app/users/oksuzian/Offlinev7_0_3/prestage/beam_g4s2_crv.fcl
  #  INFCL=../oksuzian/Offlinev7_0_3/prestage/beam_g4s2_crv.fcl #Used for 7/5 jobs
    #INFCL=../oksuzian/Offline_cryAdjustableBox/beam_g4s2_crv.fcl #Used for
    INFCL=../oksuzian/Offline_cryAdjustableBox/beam_g4s2_kl_dsvacuum.fcl #7/10 jobs with
    TAG=1907101500
elif [ "$STAGE" == "s3" ]; then
    #INLIST=prestage/sim.mu2e.cd3-beam-g4s1-mubeam.051017.txt
    INLIST=submissionLists/beam_0619_stage3_fileList.txt
#    INLIST=prestage/sim.oksuzian.mu2eII-beam.g4s1-mubeam.txt
    INFCL=../oksuzian/Offlinev7_0_3/prestage/beam_g4s3.fcl
    TAG=1907081105
elif [ "$STAGE" == "s4dio" ]; then
    INFCL=prestage/beam_g4s4_dio.fcl
    TAG=180713204829
elif [ "$STAGE" == "s4neu" ]; then
    INFCL=prestage/beam_g4s4_neutron.fcl
    TAG=180713185147
elif [ "$STAGE" == "concats2" ];  then
    INFCL=prestage/concat.fcl
    INLIST=prestage/sim.mu2e.cd3-beam-g4s2-geomv07CB407noBpoly.txt
    TAG=181029133812
elif [ "$STAGE" == "concats3" ];  then
    INFCL=prestage/concat.fcl
    INLIST=prestage/sim.mu2e.cd3-beam-g4s3-geomv07CB407noBpoly.txt
    TAG=181029133840
elif [ "$STAGE" == "concatneu" ]; then
    INFCL=prestage/concat.fcl
    INLIST=prestage/sim.mu2e.cd3-beam-g4s4neu-geomv07HighCRVDRegConc.051017.txt
    TAG=180716184813
elif [ "$STAGE" == "concatdio" ]; then
    INFCL=prestage/concat.fcl
    INLIST=prestage/sim.mu2e.cd3-beam-g4s4dio-geomv07HighCRVDRegConc.051017.txt
    TAG=180716185105
elif [ "$STAGE" == "concatmu2eIIs1mubeam" ]; then
    INLIST=prestage/sim.oksuzian.mu2eII-beam-g4s1-mubeam.010118.txt
    INFCL=prestage/concat.fcl
    TAG=180317220915
elif [ "$STAGE" == "concatmu2eIIs1dsregion" ]; then
    INLIST=prestage/sim.oksuzian.mu2eII-beam-g4s1-dsregion.010118.txt
    INFCL=prestage/concat.fcl
    TAG=180317202623
elif [ "$STAGE" == "mix" ]; then
    INFCL=prestage/crvmix_full_4out4_${MIXFILES}.fcl
    FCL_TEMPLATE=prestage/crvmix_full_4out4.fcl
#    INFCL=prestage/crvmix_full.fcl
#    INBG="--aux=4:physics.filters.plane.fileNames:${MAINDIR}/prestage/plane_bgHits.txt \
#          --aux=4:physics.filters.flashoot.fileNames:${MAINDIR}/prestage/flash_bgHits.txt \
#          --aux=1:physics.filters.dio.fileNames:${MAINDIR}/prestage/dio_bgHits.txt \
#          --aux=1:physics.filters.neu.fileNames:${MAINDIR}/prestage/neu_bgHits.txt"

    INBG="--aux=4:physics.filters.plane.fileNames:${MAINDIR}/prestage/plane_bgHits${MIXFILES}.txt \
          --aux=4:physics.filters.flashoot.fileNames:${MAINDIR}/prestage/flash_bgHits${MIXFILES}.txt \
          --aux=1:physics.filters.dio.fileNames:${MAINDIR}/prestage/dio_bgHits${MIXFILES}.txt \
          --aux=1:physics.filters.neu.fileNames:${MAINDIR}/prestage/neu_bgHits${MIXFILES}.txt"
    
    tail -n +1 prestage/*${MIXFILES}.txt
    cp ${FCL_TEMPLATE} ${INFCL}
    # Append stages efficiencis to template fcl file
    cat prestage/eff_bgHits${MIXFILES}.txt >> ${INFCL}

#    TAG=180403201406 #x1 4-out-4
#    TAG=180403201827 #x1
#    TAG=180403203842 #x3 4-out-4 
#    TAG=180403204355 #x3 3-out-4 
#    TAG=180226213603 #x2
#    TAG=180226213645 #x3

#    TAG=180406104342 #x1 4-out-4
#    TAG=180616185457 # Lei
#    TAG=180406104305 #x2 4-out-4
#    TAG=180406103952 #x3 4-out-4

#    TAG=180717211037 # V703 - Ralf's thresholds
#    TAG=180717213657 # V703 - Ralf's thresholds. Regular concrete
#    TAG=180917095803 # V703 - Ralf's thresholds. Default

#    TAG=180917203441 # V703 - Regular concrete
#    TAG=180923214243 # V703 - Regular concrete, x2
#    TAG=180923214043 # V703 - Regular concrete, x3
#     TAG=181030171021 # V703 - 0.7 CB4 withour borated poly
#     TAG=181113185204 # V703 - 0.7 CB4 withour borated poly x2
#     TAG=190509213758 # V703 - 0.7 CB4 withour borated poly x1

#     TAG=190510171606 # V703 - 0.7 CB4 withour borated poly x0.5
#     TAG=190510171647 # V703 - 0.7 CB4 withour borated poly x1
#     TAG=190510171751 # V703 - 0.7 CB4 withour borated poly x1.5

#     TAG=190512145535 # V703 - 0.7 CB4 withour borated poly x0.5 no upstream
#     TAG=190512145637 # V703 - 0.7 CB4 withour borated poly x1   no upstream
#     TAG=190512145703 # V703 - 0.7 CB4 withour borated poly x1.5 no upstream
#     TAG=190512145828 # V703 - 0.7 CB4 withour borated poly x2.0 no upstream
#     TAG=190512145911 # V703 - 0.7 CB4 withour borated poly x3.0 no upstream
     TAG=190517142246 # V703 - 0.7 CB4 baseline x3.0 no upstream

#    TAG=180917212654 # V703 - Regular concrete, No bpoly
#    TAG=xx  # V703 - Regular concrete, No bpoly, x2
#    TAG=xx  # V703 - Regular concrete, No bpoly, x3
#    TAG=180918101112  # V703 - CB4 0.7%
#    TAG=180923213611  # V703 - CB4 0.7% x2
#    TAG=180923213822  # V703 - CB4 0.7% x3


# Mu2e-II
#    INFCL=prestage/crvmix_full.fcl
#    INBG="--aux=1:physics.filters.plane.fileNames:${MAINDIR}/prestage/plane_bgHits_Mu2eII.txt \
#          --aux=1:physics.filters.flashoot.fileNames:${MAINDIR}/prestage/flash_bgHits_Mu2eII.txt \
#          --aux=1:physics.filters.dio.fileNames:${MAINDIR}/prestage/dio_bgHits.txt \
#          --aux=1:physics.filters.neu.fileNames:${MAINDIR}/prestage/neu_bgHits.txt"
#    TAG=180227123633 #x1
#    TAG= #x2
#    TAG= #x3

else
    echo "Not a valid stage"
    exit
fi

DSCONF=beam${STAGE}_0619
#DSCONF=g4s1-dsregion
#DSCONF=g4s1-mubeam
SETUPFN=./Offline/setup.sh
WFPROJ=beam_0619
#CODE=/pnfs/mu2e/resilient/users/oksuzian/gridexport/tmp.OxITZI5J6F/Code.tar.bz
#v7_0_3 - high CRVD+0.7p Boron concrete
#CODE=/pnfs/mu2e/resilient/users/oksuzian/gridexport/tmp.5740HBjhXj/Code.tar.bz
#Updated trhesholds
#CODE=/pnfs/mu2e/resilient/users/oksuzian/gridexport/tmp.8yiOBjc6Uh/Code.tar.bz
#Deafault 0.7% without borated poly
CODE=/pnfs/mu2e/resilient/users/bbarton/gridexport/tmp.OxBAJQZB7p/Code.tar.bz
OUTDIR=${DSCONF}_${TAG}

if [ "$MAKEFLC" -eq 1 ]
then
#TAG=`date +"%y%m%d%H%M%S"`
OUTDIR=${DSCONF}_${TAG}
OUTPNFS=/pnfs/mu2e/scratch/users/bbarton/workflow/${OUTDIR}/

# Generate fcl files
mkdir backup/$OUTDIR
echo "Generating fcl files in: " backup/$OUTDIR
if [ "$STAGE" == "s2" ] || [ "$STAGE" == "s3" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=bbarton --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=100 --embed ${MAINDIR}/${INFCL})
elif  [[ "$STAGE" == "concat"* ]]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --inputs=${MAINDIR}/${INLIST} --merge=600  --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "s4dio" ] || [ "$STAGE" == "s4neu" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --events-per-job=200000 --njobs=500 --run=2700  --embed ${MAINDIR}/${INFCL})
elif [ "$STAGE" == "mix" ]; then
    (cd backup/$OUTDIR && generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --events-per-job=20 --njobs=50 --run=2700 ${INBG} --embed ${MAINDIR}/${INFCL})
    command="generate_fcl --desc=sim --dsowner=oksuzian --dsconf=$DSCONF --events-per-job=20 --njobs=50 --run=2700 ${INBG} --embed ${MAINDIR}/${INFCL}"
    echo ${command}

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


