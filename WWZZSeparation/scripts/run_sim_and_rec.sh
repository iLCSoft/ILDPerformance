#!/bin/bash

WORKING_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Set defaults for if no parameters given, only meant for code testing
N_EVENTS=10
DETECTOR_MODEL=ILD_l5_o1_v02

SIM_INPUT_FILES=/pnfs/desy.de/ilc/prod/ilc/mc-dbd/generated/1000-B1b_ws/6f/E1000-B1b_ws.Pxxxxvv.Gwhizard-1.95.eL.pR.I35911.001.stdhep

PROCESS_NAME=test_name

RUN_DDSIM=true
RUN_PANDORA=true

# ----------------------------- GET INPUT PARAMETERS ---------------------------- #

for i in "$@"
do
case $i in
    -N=*|--numberOfEvents=*)
    N_EVENTS="${i#*=}"
    shift # past argument=value
    ;;
    --DetectorModel=*)
    DETECTOR_MODEL="${i#*=}"
    shift # past argument=value
    ;;
    --stdhepInput=*)
    SIM_INPUT_FILES="${i#*=}"
    shift # past argument=value
    ;;
    --processName=*)
    PROCESS_NAME="${i#*=}"
    shift # past argument=value
    ;;
    --no-ddsim)
    RUN_DDSIM=false
    shift # past argument=value
    ;;
    --no-pandora)
    RUN_PANDORA=false
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

OUTPUT_DIR=${WORKING_DIR}/../output/local_ILDConfig_local_${DETECTOR_MODEL}

CONFIG_DIR=${WORKING_DIR}/ILDConfig/StandardConfig/production

SIM_OUTPUT_FILE=${OUTPUT_DIR}/${PROCESS_NAME}.slcio
SIM_STEERING=ddsim_steer.py

LCGEO_XML=$lcgeo_DIR/ILD/compact/${DETECTOR_MODEL}/${DETECTOR_MODEL}.xml
REC_STEERING_DIR=${CONFIG_DIR}/
REC_OUTPUT_BASE=${OUTPUT_DIR}/${PROCESS_NAME}


# ----------------------------- RUN SIMULATION ---------------------------------- #

cd ${REC_STEERING_DIR}

if $RUN_DDSIM; then
	ddsim --inputFiles ${SIM_INPUT_FILES} --outputFile ${SIM_OUTPUT_FILE} --compactFile ${LCGEO_XML} --steeringFile ${SIM_STEERING} --numberOfEvents ${N_EVENTS}
fi

wait

# ----------------------------- RUN RECONSTRUCTION ------------------------------ #

if $RUN_PANDORA; then
	Marlin MarlinStdReco.xml --constant.lcgeo_DIR=$lcgeo_DIR --constant.DetectorModel=${DETECTOR_MODEL} --constant.OutputBaseName=${REC_OUTPUT_BASE} --global.LCIOInputFiles=${SIM_OUTPUT_FILE}
fi

wait

cd ${WORKING_DIR}
