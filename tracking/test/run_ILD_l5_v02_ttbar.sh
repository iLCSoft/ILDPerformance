#!/bin/bash
#
#==============================================================
# Running shell script in parallel over multiple cores
#==============================================================

ILDMODEL=ILD_l5_v02
ILCSOFTVER=v01-19-05

. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc49_sl6/${ILCSOFTVER}/init_ilcsoft.sh

INFILE=/nfs/dust/ilc/group/ild/dbd-data/500/6f/E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.stdhep

#==================================================
#SIMULATION
NSkip=0
for i in {0..9}
do
echo "NSkip ${NSkip}"

ddsim \
    --inputFiles $INFILE  \
    --outputFile ${ILDMODEL}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_${i}_SIM.slcio \
    --compactFile $lcgeo_DIR/ILD/compact/${ILDMODEL}/${ILDMODEL}.xml \
    --steeringFile ddsim_steer.py \
    --numberOfEvents 100  \
    --skipNEvents ${NSkip} &

NSkip=`expr $NSkip + 100`

done
wait

mv ${ILDMODEL}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_*_SIM.slcio Results/SimFiles

#=================================================
# RECONSTRUCTION
for i in {0..9}
do

Marlin MarlinStdReco.xml \
    --constant..DetectorModel=ILD_l5_o1_v02 \
    --global.LCIOInputFiles=Results/SimFiles/${ILDMODEL}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_${i}_SIM.slcio \
    --constant.lcgeo_DIR=$lcgeo_DIR \
    --constant.OutputBaseName=${ILDMODEL}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_${i} \
    --MyRecoMCTruthLinker.UsingParticleGun=false \
> RECO_${ILDMODEL}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_${i}.out &

done
wait

# move all to folder RecoFiles

mv ${ILDMODEL}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_*_REC.slcio Results/RecoFiles

# cleanup 
rm ${ILDMODEL}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_*_DST.slcio
rm ${ILDMODEL}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_*_AIDA.root
rm ${ILDMODEL}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05_*_PfoAnalysis.root

#=================================================
# diagnostics

Marlin Diagnostics_ILD_l5_v02_ttbar.xml \
    --constant.lcgeo_DIR=$lcgeo_DIR \
    --MyAIDAProcessor.FileName=analysis_${ILDMODEL}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05 \
    --MyDiagnostics.PhysSampleOn=true \
> DIAG_${ILDMODEL}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.out


# move all to folder Analysis

mv  analysis_${ILDMODEL}_${ILCSOFTVER}_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.root ../Results/Analysis


# move log files to folder logFiles

#mv *.out logFiles
#mv *.err logFiles
#mv *.log logFiles


#==================================================
# generate monitor plots

cd ../macros
root -b -q EfficiencyL5.C

OUTPUTPATH=~/www/ILDPerformance_${ILCSOFTVER}
mkdir -p ${OUTPUTPATH}

cp trkEff_pt_ttbar_${ILDMODEL}_${ILCSOFTVER}.png ${OUTPUTPATH}
cp trkEff_theta_ttbar_${ILDMODEL}_${ILCSOFTVER}.png ${OUTPUTPATH}
