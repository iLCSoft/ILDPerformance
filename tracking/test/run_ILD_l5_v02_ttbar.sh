#!/bin/bash
#
#==============================================================
# Running shell script in parallel over multiple cores
#==============================================================

echo "############################################################################"
echo "                             System information"
echo "                             =================="

echo "Host:"
hostname -f

echo "CPU(s):"
cat /proc/cpuinfo | grep "model name" | cut -b 14-

echo "RAM:"
cat /proc/meminfo | grep "MemTotal" | cut -b 10-

echo "Swap:"
cat /proc/meminfo | grep "SwapTotal" | cut -b 11-

echo "############################################################################"
echo "Group:      ${GROUP}"

echo "Hosttype:   ${HOSTTYPE}"

echo "JobID:      ${JOB_ID}"

echo "JobName:    ${JOB_NAME}"

echo "Job_Script: ${JOB_SCRIPT}"

echo "User:       ${LOGNAME}"

echo "Queue:      ${QUEUE}"

echo "Shell:      ${SHELL}"

echo "TMP:        ${TMP}"

echo "TMPDIR:     ${TMPDIR}"

echo "User:       ${USER}"

echo "Working_DIR:${PWD}"

echo "############################################################################"
echo
echo "############################################################################"
echo

ILDMODEL=ILD_l5_v02
ILCSOFTVER=v01-19-05

. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc49_sl6/${ILCSOFTVER}/init_ilcsoft.sh

#INFILE=/nfs/dust/ilc/user/voutsina/GenFiles/E250-TDR_ws.Pe2e2h_mumu.Gwhizard-1_95.eL.pR.I108009.001.stdhep
#INFILE=/nfs/dust/ilc/group/ild/dbd-data/250/higgs/E250-TDR_ws.Pe2e2h_mumu.Gwhizard-1_95.eL.pR.I108009.001.stdhep
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
    --steeringFile DDSim/ddsim_steer.py \
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

Marlin ProductionSteeringFiles/MarlinStdReco_ILD_l5_o1_v02.xml \
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

mkdir -p ~/www/ILDPerformance_${ILCSOFTVER}

cp trkEff_${ILDMODEL}_${ILCSOFTVER}.png ~/www/ILDPerformance_${ILCSOFTVER}
cp trkEff_v02_${ILDMODEL}_${ILCSOFTVER}.png ~/www/ILDPerformance_${ILCSOFTVER}
