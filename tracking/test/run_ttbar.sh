#!/bin/bash
#
#==============================================================
# Running shell script in parallel over multiple cores
#==============================================================
#YES NO
RUNGEN="YES"
RUNSIM="YES"
RUNREC="NO"
RUNANA="NO"

ILDMODEL_SIM=ILD_l5_v02
ILDMODEL_REC=ILD_l5_o1_v02
ILCSOFTVER=v02-00-01

. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc49_sl6/${ILCSOFTVER}/init_ilcsoft.sh
#. /cvmfs/ilc.desy.de/sw/x86_64_gcc49_sl6/${ILCSOFTVER}/init_ilcsoft.sh

INFILE=/nfs/dust/ilc/group/ild/dbd-data/500/6f/E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.stdhep

#==================================================
#SIMULATION
if [ $RUNSIM == "YES" ]
then
  echo "running sim"

NSkip=0
for i in {0..9}
do
echo "NSkip ${NSkip}"

ddsim \
    --inputFiles $INFILE  \
    --outputFile ${ILDMODEL_SIM}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_${i}_SIM.slcio \
    --compactFile $lcgeo_DIR/ILD/compact/${ILDMODEL_SIM}/${ILDMODEL_SIM}.xml \
    --steeringFile ddsim_steer.py \
    --numberOfEvents 100  \
    --skipNEvents ${NSkip} &

NSkip=`expr $NSkip + 100`

done
wait

mv ${ILDMODEL_SIM}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_*_SIM.slcio Results/SimFiles
fi

#=================================================
# RECONSTRUCTION
if [ $RUNREC == "YES" ]
then
  echo "running rec"

for i in {0..9}
do

Marlin MarlinStdReco.xml \
    --constant..DetectorModel=${ILDMODEL_REC} \
    --global.LCIOInputFiles=Results/SimFiles/${ILDMODEL_SIM}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_${i}_SIM.slcio \
    --constant.RunBeamCalReco=false \
    --constant.lcgeo_DIR=$lcgeo_DIR \
    --constant.OutputBaseName=${ILDMODEL_REC}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_${i} \
    --MyRecoMCTruthLinker.UsingParticleGun=false \
> RECO_${ILDMODEL_REC}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_${i}.out &

done
wait

# move all to folder RecoFiles

mv ${ILDMODEL_REC}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_*_REC.slcio Results/RecoFiles

# cleanup 
rm ${ILDMODEL_REC}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_*_DST.slcio
rm ${ILDMODEL_REC}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_*_AIDA.root
rm ${ILDMODEL_REC}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_*_PfoAnalysis.root
fi

#=================================================
# diagnostics
if [ $RUNANA == "YES" ]
then
  echo "running ana"

Marlin Diagnostics_ttbar.xml \
    --constant.lcgeo_DIR=$lcgeo_DIR \
    --constant.DetectorModel=${ILDMODEL_REC} \
    --constant.ILCSoftVersion=${ILCSOFTVER} \
    --MyAIDAProcessor.FileName=analysis_${ILDMODEL_REC}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05 \
    --MyDiagnostics.PhysSampleOn=true \
    --MyDiagnostics.CosThetaCut=0.99 \
    --MyDiagnostics.PCut=0.0 \
    --MyDiagnostics.PTCut=0.0 \
    --MyDiagnostics.DistFromIP=10.0 \
    --MyDiagnostics.RequiredPurity=0.74 \
    --MyDiagnostics.RequiredSiHits=4 \
> DIAG_${ILDMODEL_REC}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.out


# move all to folder Analysis
rm ../Results/Analysis/analysis_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.root
cp  analysis_${ILDMODEL_REC}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.root ../Results/Analysis/analysis_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.root
mv  analysis_${ILDMODEL_REC}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.root ../Results/Analysis


# move log files to folder logFiles

#mv *.out logFiles
#mv *.err logFiles
#mv *.log logFiles


#==================================================
# generate monitor plots

cd ../macros
rm trkEff_momentum_ttbar.png trkEff_pt_ttbar.png trkEff_theta_ttbar.png

root -b -q EfficiencyL5.C

OUTPUTPATH=~/www/ILDPerformance_${ILCSOFTVER}_${ILDMODEL_REC}
mkdir -p ${OUTPUTPATH}

cp trkEff_momentum_ttbar.png ${OUTPUTPATH}/trkEff_momentum_ttbar_${ILDMODEL_REC}_${ILCSOFTVER}.png
cp trkEff_pt_ttbar.png ${OUTPUTPATH}/trkEff_pt_ttbar_${ILDMODEL_REC}_${ILCSOFTVER}.png
cp trkEff_theta_ttbar.png ${OUTPUTPATH}/trkEff_theta_ttbar_${ILDMODEL_REC}_${ILCSOFTVER}.png

fi