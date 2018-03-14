#!/bin/bash

# ----------------------------- GET INPUT PARAMETERS ---------------------------- #
# Default parameters
N_EVENTS=10
DETECTOR_MODEL=ILD_l5_o1_v02

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
	-h|--help|-H)
	echo ""
	echo "In ./run_performance_check.sh: executed w/ asking for help:"
	echo ""
	echo "Script to run all necessary scripts to create WW/ZZ+nunu performance plots."
	echo "Usage: ./run_performance_check.sh: [-N=number_of_events] [--DetectorModel=detector]"
	echo ""
	echo "Further input information can be set in the input_config.sh script."
	echo ""
	echo "Exiting now."
	exit
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

# Directory of this script
WORKING_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Make all scripts executable so that the shebang is not ignored
echo "Making scripts executable."
chmod u+x ${WORKING_DIR}/input_config.sh
chmod u+x ${WORKING_DIR}/scripts/run_complete_analysis.sh
chmod u+x ${WORKING_DIR}/scripts/run_plotting.sh
chmod u+x ${WORKING_DIR}/scripts/run_sim_and_rec.sh

# Make sure all necessary directories are here
if [[ ! -d ${WORKING_DIR}/log ]] ; then
	mkdir ${WORKING_DIR}/log
fi
if [[ ! -d ${WORKING_DIR}/output ]] ; then
	mkdir ${WORKING_DIR}/output
fi

echo "Starting main script."
echo ""
cd ${WORKING_DIR}/scripts
./run_complete_analysis.sh -N=${N_EVENTS} --DetectorModel=${DETECTOR_MODEL}
cd ${WORKING_DIR}

