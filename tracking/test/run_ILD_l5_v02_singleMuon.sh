#!/bin/bash
#
#==============================================================
# Running shell script in parallel over multiple cores
#==============================================================

ILDMODELRECO=ILD_l5_o1_v02
ILDMODELSIM=ILD_l5_v02
ILCSOFTVER=v02-03-04

. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc131_el9/${ILCSOFTVER}/init_ilcsoft.sh

PolarAngles=('10' '20' '40' '85')
Mom=('1' '3' '5' '10' '15' '25' '50' '100' '200')

#==================================================
# GENERATION - particle gun
for i in {0..3}; do

	for j in {0..8}; do

		python lcio_particle_gun.py ${Mom[j]} ${PolarAngles[i]} mcparticles_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio 13 -1. &

	done

done
wait

mv mcparticles_MuonsAngle_*_Mom_*.slcio Results/GenFiles

#==================================================
# SIMULATION
for i in {0..3}; do

	for j in {0..8}; do

		ddsim \
			--inputFiles Results/GenFiles/mcparticles_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio \
			--outputFile ${ILDMODELSIM}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}_SIM.slcio \
			--compactFile $lcgeo_DIR/ILD/compact/${ILDMODELSIM}/${ILDMODELSIM}.xml \
			--steeringFile ddsim_steer.py \
			--numberOfEvents -1 &

	done
	wait
done
wait

mv ${ILDMODELSIM}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_SIM.slcio Results/SimFiles

#==================================================
# RECONSTRUCTION
for i in {0..3}; do

	for j in {0..8}; do

		Marlin MarlinStdReco.xml \
			--constant.DetectorModel=${ILDMODELRECO} \
			--global.LCIOInputFiles=Results/SimFiles/${ILDMODELSIM}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}_SIM.slcio \
			--constant.RunBeamCalReco=false \
			--constant.lcgeo_DIR=$lcgeo_DIR \
			--constant.OutputBaseName=${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]} \
			--MyRecoMCTruthLinker.UsingParticleGun=true \
			>RECO_${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out &

	done
	wait
done
wait

# move all to folder RecoFiles

mv ${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_REC.slcio Results/RecoFiles

# cleanup
rm ${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_DST.slcio
rm ${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_AIDA.root
rm ${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_PfoAnalysis.root

#==================================================
# start Diagnostics
for i in {0..3}; do

	for j in {0..8}; do

		# diagnostics

		INFILE=Results/RecoFiles/${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}_REC.slcio

		Marlin DDDiagnostics.xml \
			--global.LCIOInputFiles=$INFILE \
			--InitDD4hep.DD4hepXMLFile=$lcgeo_DIR/ILD/compact/${ILDMODELRECO}/${ILDMODELRECO}.xml \
			--MyAIDAProcessor.FileName=analysis_${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]} \
			--MyDiagnostics.FillBigTTree=true \
			--MyDiagnostics.PhysSampleOn=false \
			>DIAG_${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out &

	done
	wait
done
wait

# copy output by removing the "${ILCSOFTVER}"
for i in {0..3}; do

	for j in {0..8}; do

		cp analysis_${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.root ../Results/Analysis/analysis_${ILDMODELRECO}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.root

	done
done

# move all to folder Analysis

mv analysis_${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_*_Mom_*.root ../Results/Analysis

# move log files to folder logFiles

#mv *.out logFiles
#mv *.err logFiles
#mv *.log logFiles

#==================================================
# generate monitor plots

cd ../macros

root -b -q "D0ResolutionL5.C(\"${ILDMODELRECO}\")"
# root -b -q D0ResolutionL5.C
root -b -q PResolutionL5.C
root -b -q meanL5.C
root -b -q sigmaL5.C

OUTPUTPATH=../Results/MonitorPlots

cp IPResolution_${ILDMODELRECO}.png ${OUTPUTPATH}/IPResolution_${ILDMODELRECO}_${ILCSOFTVER}.png
cp D0_fit_${ILDMODELRECO}.pdf ${OUTPUTPATH}/D0_fit_${ILDMODELRECO}_${ILCSOFTVER}.pdf
cp PResolution_${ILDMODELRECO}.png ${OUTPUTPATH}/PResolution_${ILDMODELRECO}_${ILCSOFTVER}.png
cp PR_fit_${ILDMODELRECO}.pdf ${OUTPUTPATH}/PR_fit_${ILDMODELRECO}_${ILCSOFTVER}.pdf
cp pull_mean_${ILDMODELRECO}.png ${OUTPUTPATH}/pull_mean_${ILDMODELRECO}_${ILCSOFTVER}.png
cp pull_sigma_${ILDMODELRECO}.png ${OUTPUTPATH}/pull_sigma_${ILDMODELRECO}_${ILCSOFTVER}.png
