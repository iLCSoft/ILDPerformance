#!/bin/bash

# ----------------------------- GET INPUT PARAMETERS ---------------------------- #
# Default parameters
N_EVENTS=10
DETECTOR_MODEL=ILD_l5_o1_v02

RUN_ON_ILCDIRAC=false
ILCSOFT_VERSION=""
ILDCONFIG_VERSION=""

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
    --UseILCDirac)
    RUN_ON_ILCDIRAC=true
    shift
    ILCSOFT_VERSION=$1
    shift
    ILDCONFIG_VERSION=$1
    shift
    ;;
    -h|--help|-H)
    echo ""
    echo "In ./run_performance_check.sh: executed w/ asking for help:"
    echo ""
    echo "Script to run all necessary scripts to create WW/ZZ+nunu performance plots."
    echo "Usage: ./run_performance_check.sh: [-N=number_of_events] [--DetectorModel=detector] [--UseILCDirac ilcsoft_version ildconfig_version]"
    echo ""
    echo "Further input information can be set in the input_config.sh script."
    echo "When using ILCDIRAC the versions of iLCsoft and ILDConfig have to be supplied. They must be versions known to the grid."
    echo ""
    echo "Exiting now."
    exit
    shift # past argument=value
    ;;
    *)
          # unknown option
    shift
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
chmod u+x ${WORKING_DIR}/scripts/initialize_ilcdirac.sh

# Initialize ILCDirac and check if version names are good
source ${WORKING_DIR}/scripts/initialize_ilcdirac.sh

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
if $RUN_ON_ILCDIRAC; then
  ./run_complete_analysis.sh -N=${N_EVENTS} --DetectorModel=${DETECTOR_MODEL} --UseILCDirac ${ILCSOFT_VERSION} ${ILDCONFIG_VERSION}
else
  ./run_complete_analysis.sh -N=${N_EVENTS} --DetectorModel=${DETECTOR_MODEL}
fi
cd ${WORKING_DIR}
