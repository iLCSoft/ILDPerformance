#!/bin/bash

# --- Setup Environment ---
if [ -z "${KEY4HEP_STACK:-}" ]; then
    source /cvmfs/sw-nightlies.hsf.org/key4hep/setup.sh
fi

# Enable strict error tracking for production/pipeline safety
set -euo pipefail

#==============================================================
# Running shell script in parallel over multiple cores
#==============================================================

ILDMODELRECO=ILD_FCCee_v01 # ILD_l5_o1_v02
ILDMODELSIM=ILD_FCCee_v01 # ILD_l5_v02
ILCSOFTVER=key4hep_night

ILDCONFIGDIR=$codeDir/ILDConfig/StandardConfig/production
ILDRECO=${ILDCONFIGDIR}/ILDReconstruction.py
export PYTHONPATH=${ILDCONFIGDIR}:${PYTHONPATH}

# Set compact file root directory depending on detector model
if [[ "${ILDMODELRECO}" == ILD_FCCee_v01 ||
      "${ILDMODELRECO}" == ILD_FCCee_v02 ]]; then
    COMPACTFILEDIR=$K4GEO/FCCee/ILD_FCCee/compact
else
    COMPACTFILEDIR=$lcgeo_DIR/ILD/compact
fi

PolarAngles=('10' '20' '40' '85')
Mom=('1' '3' '5' '10' '15' '25' '50' '100' '200')

# --- DEBUG MODE ---
# Set to 'true' to run only one angle/momentum combination and stop after DDDiagnostics
DEBUG=true
if [[ "${DEBUG}" == "true" ]]; then
	echo ">>> DEBUG MODE: running single angle=${PolarAngles[3]}, momentum=${Mom[6]} only <<<"
	PolarAngles=("${PolarAngles[3]}")
	Mom=("${Mom[6]}")
fi
# ------------------

OUTPUTPATH=../Results/MonitorPlots
LOGFILEPATH=logFiles
TESTDIR=$(pwd)
#==================================================
# GENERATION - particle gun
for i in "${!PolarAngles[@]}"; do

	for j in "${!Mom[@]}"; do

		python lcio_particle_gun.py ${Mom[j]} ${PolarAngles[i]} Results/GenFiles/mcparticles_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio 13 -1. &

	done

done
wait

# mv mcparticles_MuonsAngle_*_Mom_*.slcio Results/GenFiles

#==================================================
# SIMULATION
for i in "${!PolarAngles[@]}"; do

	for j in "${!Mom[@]}"; do

		ddsim \
			--inputFiles Results/GenFiles/mcparticles_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio \
			--outputFile Results/SimFiles/${ILDMODELSIM}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}_SIM.slcio \
			--compactFile ${COMPACTFILEDIR}/${ILDMODELSIM}/${ILDMODELSIM}.xml \
			--steeringFile ${ILDCONFIGDIR}/ddsim_steer.py \
			--numberOfEvents -1 &

	done
	wait
done
wait

# mv ${ILDMODELSIM}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_SIM.slcio Results/SimFiles

#==================================================
# RECONSTRUCTION
cd "${ILDCONFIGDIR}"
for i in "${!PolarAngles[@]}"; do

	for j in "${!Mom[@]}"; do

		EXTRA_FLAGS=()
		if [[ "${ILDMODELRECO}" == "ILD_FCCee_v01" ||
			"${ILDMODELRECO}" == "ILD_FCCee_v02" ]]; then
			EXTRA_FLAGS=(--trackMerge --doHLR)
		fi

        k4run ${ILDRECO} \
			--detectorModel ${ILDMODELRECO} \
			--inputFiles ${TESTDIR}/Results/SimFiles/${ILDMODELSIM}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}_SIM.slcio \
			--noBeamCalReco \
			--outputFileBase ${TESTDIR}/Results/RecoFiles/${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]} \
			--lcioOutput only \
			--usingParticleGun \
                        "${EXTRA_FLAGS[@]}" \
			-n -1 \
		>${TESTDIR}/${LOGFILEPATH}/RECO_${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out &

		#		Marlin MarlinStdReco.xml \
		#			--constant..DetectorModel=ILD_l5_o1_v02 \
		#			--global.LCIOInputFiles=Results/SimFiles/${ILDMODEL}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}_SIM.slcio \
		#			--constant.RunBeamCalReco=false \
		#			--constant.lcgeo_DIR=$lcgeo_DIR \
		#			--constant.OutputBaseName=${ILDMODEL}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]} \
		#			--MyRecoMCTruthLinker.UsingParticleGun=true \
		#			>${LOGFILEPATH}/RECO_${ILDMODEL}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out &

	done
	wait
done
wait
cd "${TESTDIR}"

# move all to folder RecoFiles
# mv ${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_REC.slcio Results/RecoFiles

# cleanup
rm ${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_DST.slcio
rm ${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_AIDA.root
rm ${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_*_Mom_*_PfoAnalysis.root

#==================================================
# start Diagnostics
for i in "${!PolarAngles[@]}"; do

	for j in "${!Mom[@]}"; do

		# diagnostics

		INFILE=Results/RecoFiles/${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}_REC.slcio

		# default steering file
		STEERING_FILE="DDDiagnostics.xml"

		# override for FCCee models
		if [[ "${ILDMODELRECO}" == ILD_FCCee_v01 ||
			"${ILDMODELRECO}" == ILD_FCCee_v02 ]]; then
			STEERING_FILE="DDDiagnostics_FCCee.xml"
		fi

		Marlin "${STEERING_FILE}" \
			--global.LCIOInputFiles=$INFILE \
			--InitDD4hep.DD4hepXMLFile=${COMPACTFILEDIR}/${ILDMODELRECO}/${ILDMODELRECO}.xml \
			--MyAIDAProcessor.FileName=analysis_${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]} \
			--MyDiagnostics.FillBigTTree=true \
			--MyDiagnostics.PhysSampleOn=false \
			>${LOGFILEPATH}/DIAG_${ILDMODELRECO}_${ILCSOFTVER}_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out &

	done
	wait
done
wait

# --- DEBUG MODE early exit ---
if [[ "${DEBUG}" == "true" ]]; then
	echo ">>> DEBUG MODE: stopping after DDDiagnostics. Skipping analysis file copies and ROOT macros. <<<"
	exit 0
fi
# ----------------------------

# copy output by removing the "${ILCSOFTVER}"
for i in "${!PolarAngles[@]}"; do

	for j in "${!Mom[@]}"; do

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
root -b -q "PResolutionL5.C(\"${ILDMODELRECO}\")"
root -b -q "meanL5.C(\"${ILDMODELRECO}\")"
root -b -q "sigmaL5.C(\"${ILDMODELRECO}\")"


cp IPResolution_${ILDMODELRECO}.png ${OUTPUTPATH}/IPResolution_${ILDMODELRECO}_${ILCSOFTVER}.png
cp D0_fit_${ILDMODELRECO}.pdf ${OUTPUTPATH}/D0_fit_${ILDMODELRECO}_${ILCSOFTVER}.pdf
cp PResolution_${ILDMODELRECO}.png ${OUTPUTPATH}/PResolution_${ILDMODELRECO}_${ILCSOFTVER}.png
cp PR_fit_${ILDMODELRECO}.pdf ${OUTPUTPATH}/PR_fit_${ILDMODELRECO}_${ILCSOFTVER}.pdf
cp pull_mean_${ILDMODELRECO}.png ${OUTPUTPATH}/pull_mean_${ILDMODELRECO}_${ILCSOFTVER}.png
cp pull_sigma_${ILDMODELRECO}.png ${OUTPUTPATH}/pull_sigma_${ILDMODELRECO}_${ILCSOFTVER}.png
