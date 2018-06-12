#!/bin/bash
#
#==============================================================
# Running shell script in parallel over multiple cores
#==============================================================
#YES NO
RUNGEN="NO"
RUNSIM="YES"
RUNREC="NO"
RUNANA="NO"

ILDMODEL_SIM=ILD_l5_v02
ILDMODEL_REC=ILD_l5_o1_v02
ILCSOFTVER=v02-00-01

. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc49_sl6/${ILCSOFTVER}/init_ilcsoft.sh
#. /cvmfs/ilc.desy.de/sw/x86_64_gcc49_sl6/${ILCSOFTVER}/init_ilcsoft.sh

PolarAngles=('10' '20' '40' '85')
Mom=( '1' '3' '5' '10' '15' '25' '50' '100' )


#==================================================
# GENERATION - particle gun
if [ $RUNGEN == "YES" ]
then
  echo "running gen"

for i in {0..3}
do

for j in {0..7}
do

python lcio_particle_gun.py ${Mom[j]} ${PolarAngles[i]}  mcparticles_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio 13 -1. &

done

done
wait

mv mcparticles_MuonsAngle_*_Mom_*.slcio Results/GenFiles
fi

#==================================================
# SIMULATION
if [ $RUNSIM == "YES" ]
then
  echo "running sim"

for i in {0..3}
do

for j in {0..7}
do

ddsim \
    --inputFiles Results/GenFiles/mcparticles_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio  \
    --outputFile ${ILDMODEL_SIM}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}_SIM.slcio \
    --compactFile $lcgeo_DIR/ILD/compact/${ILDMODEL_SIM}/${ILDMODEL_SIM}.xml \
    --steeringFile ddsim_steer.py \
    --numberOfEvents 5000 &

done
wait
done
wait

mv ${ILDMODEL_SIM}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_SIM.slcio Results/SimFiles
fi

#==================================================
# RECONSTRUCTION
if [ $RUNREC == "YES" ]
then
  echo "running rec"

for i in {0..3}
do

for j in {0..7}
do

Marlin MarlinStdReco.xml \
    --constant.DetectorModel=${ILDMODEL_REC} \
    --global.LCIOInputFiles=Results/SimFiles/${ILDMODEL_SIM}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}_SIM.slcio \
    --constant.RunBeamCalReco=false \
    --constant.lcgeo_DIR=$lcgeo_DIR \
    --constant.OutputBaseName=${ILDMODEL_REC}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]} \
    --MyRecoMCTruthLinker.UsingParticleGun=true \
> RECO_${ILDMODEL_REC}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out &

done
wait
done
wait


# move all to folder RecoFiles

mv ${ILDMODEL_REC}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_REC.slcio Results/RecoFiles

# cleanup 
rm ${ILDMODEL_REC}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_DST.slcio
rm ${ILDMODEL_REC}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_AIDA.root
rm ${ILDMODEL_REC}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_PfoAnalysis.root
fi

#==================================================
# start Diagnostics
if [ $RUNANA == "YES" ]
then
  echo "running ana"

for i in {0..3}
do

for j in {0..7}
do

# diagnostics

INFILE=Results/RecoFiles/${ILDMODEL_REC}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}_REC.slcio

Marlin DDDiagnostics.xml \
    --constant.lcgeo_DIR=$lcgeo_DIR \
    --constant.DetectorModel=${ILDMODEL_REC} \
    --constant.ILCSoftVersion=${ILCSOFTVER} \
    --global.LCIOInputFiles=$INFILE \
    --MyAIDAProcessor.FileName=analysis_${ILDMODEL_REC}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]} \
    --MyDiagnostics.FillBigTTree=true \
    --MyDiagnostics.PhysSampleOn=false \
    --MyDiagnostics.CosThetaCut=0.99 \
    --MyDiagnostics.PCut=0.0 \
    --MyDiagnostics.PTCut=0.0 \
    --MyDiagnostics.DistFromIP=10.0 \
    --MyDiagnostics.RequiredPurity=0.74 \
    --MyDiagnostics.RequiredSiHits=4 \
> DIAG_${ILDMODEL_REC}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out &

done
wait
done
wait

# copy output by removing the "${ILDMODEL_REC}" "${ILCSOFTVER}"
rm ../Results/Analysis/analysis_MuonsAngle_*_Mom_*.root

for i in {0..3}
do

for j in {0..7}
do
cp analysis_${ILDMODEL_REC}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.root ../Results/Analysis/analysis_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.root

done
done

# move all to folder Analysis

mv  analysis_${ILDMODEL_REC}_${ILCSOFTVER}_MuonsAngle_*_Mom_*.root ../Results/Analysis

# move log files to folder logFiles

#mv *.out logFiles
#mv *.err logFiles
#mv *.log logFiles



#==================================================
# generate monitor plots

cd ../macros
rm D0Resolution.png D0_fit.pdf Z0Resolution.png Z0_fit.pdf PResolution.png PR_fit.pdf pull_mean.png pull_sigma.png

root -b -q D0ResolutionL5.C
root -b -q Z0ResolutionL5.C
root -b -q PResolutionL5.C
root -b -q meanL5.C
root -b -q sigmaL5.C

OUTPUTPATH=~/www/ILDPerformance_${ILCSOFTVER}_${ILDMODEL_REC}
mkdir -p ${OUTPUTPATH}

mv D0Resolution.png ${OUTPUTPATH}/D0Resolution_${ILDMODEL_REC}_${ILCSOFTVER}.png
mv D0_fit.pdf ${OUTPUTPATH}/D0_fit_${ILDMODEL_REC}_${ILCSOFTVER}.pdf
mv Z0Resolution.png ${OUTPUTPATH}/Z0Resolution_${ILDMODEL_REC}_${ILCSOFTVER}.png
mv Z0_fit.pdf ${OUTPUTPATH}/Z0_fit_${ILDMODEL_REC}_${ILCSOFTVER}.pdf
mv PResolution.png ${OUTPUTPATH}/PResolution_${ILDMODEL_REC}_${ILCSOFTVER}.png
mv PR_fit.pdf ${OUTPUTPATH}/PR_fit_${ILDMODEL_REC}_${ILCSOFTVER}.pdf
mv pull_mean.png ${OUTPUTPATH}/pull_mean_${ILDMODEL_REC}_${ILCSOFTVER}.png
mv pull_sigma.png ${OUTPUTPATH}/pull_sigma_${ILDMODEL_REC}_${ILCSOFTVER}.png

fi