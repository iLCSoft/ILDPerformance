#!/bin/bash

# ----------------------------- FLAGS FOR DIFFERENT STEPS ----------------------- #
RUN_DDSIM=true
RUN_PANDORA=true
USE_PRERECONSTRUCTED=false
RUN_ANALYSIS=true
RUN_PLOTTING=true
RUN_ON_ILCDIRAC=false

# ----------------------------- GET INPUT PARAMETERS ---------------------------- #

# Default parameters
N_EVENTS=10
DETECTOR_MODEL=ILD_l5_o1_v02

ILCSOFT_VERSION="local"
ILDCONFIG_VERSION="local"

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
  --no-ddsim)
    RUN_DDSIM=false
    shift # past argument=value
  ;;
  --no-pandora)
    RUN_PANDORA=false
    shift # past argument=value
  ;;
  --no-analysis)
    RUN_ANALYSIS=false
    shift # past argument=value
  ;;
  --no-plotting)
    RUN_PLOTTING=false
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
  --useReconstructedFilesAt=*)
    USE_PRERECONSTRUCTED=true
    RECO_FILE_DIR="${i#*=}"
    echo "No sim or reco - will search for local reconstructed files at ${RECO_FILE_DIR} ."
    echo "They must follow ILC naming conventions or will be ignored!"
    shift # past argument=value
  ;;
  -h|--help)
    echo ""
    echo "In ./run_complete_analysis.sh: executed w/ asking for help:"
    echo ""
    echo "Script to run complete simulation, reconstruction and analysis of 2nu4q events."
    echo "Usage: ./run_complete_analysis.sh [-N=number_of_events] [--DetectorModel=detector]"
    echo "                                  [--no-{ddsim,pandora,analysis,plotting}]"
    echo "                                  [--UseILCDirac ilcsoft_version ildconfig_version]"
    echo "                                  [--useReconstructedFilesAt=DSTFilesDirectory]"
    echo ""
    echo "The --no-... flags suppress the ... part of the process."
    echo "The --useReconstructedFilesAt=... flag allows working with already reconstructed files."
    echo ""
    echo "When using ILCDIRAC the versions of iLCsoft and ILDConfig have to be supplied."
    echo "They must be versions known to the grid."
    echo ""
    echo "Further input information can be set in the input_config.sh script."
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

# No Sim or Reco when already reconstructed files used
if $USE_PRERECONSTRUCTED; then
  RUN_DDSIM=false
  RUN_PANDORA=false
  RUN_ON_ILCDIRAC=false
  ILDCONFIG_VERSION="none_UsingDSTFiles"
fi

WORKING_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

STEERING_DIRECTORY=${WORKING_DIR}/../processor/steering_files
TEMPLATE=${STEERING_DIRECTORY}/template.xml

LOG_DIRECTORY=${WORKING_DIR}/../log
OUTPUT_DIRECTORY=${WORKING_DIR}/../output

source ${WORKING_DIR}/get_output_subdir.sh
SUBDIR=$( echo_subdir_base ${DETECTOR_MODEL} ${ILCSOFT_VERSION} ${ILDCONFIG_VERSION} )
OUTPUT_BASE=${OUTPUT_DIRECTORY}/${SUBDIR}
# If not already here create detector model specific output directory
if [[ ! -d ${OUTPUT_BASE} ]] ; then
	mkdir ${OUTPUT_BASE}
fi

N_STATES=3
FINAL_STATES=("vvxyyx" "vvxxxx" "vvyyyy")
FILES=($(./../input_config.sh filepaths))
CROSS_SECTIONS=($(./../input_config.sh cross-sections))

# If needed and not already here load ILDConfig from GitHub
if ! $RUN_ON_ILCDIRAC && ($RUN_DDSIM || $RUN_PANDORA); then
	if [[ ! -d ${WORKING_DIR}/ILDConfig ]] ; then
		echo "Get ILDConfig:"
		git clone https://github.com/iLCSoft/ILDConfig.git
		echo ""
	fi
fi

PIDs=() #Process IDs keep track of my background processes

for (( i=0; i<$(( $N_STATES )); i++ )) do
	final_state=${FINAL_STATES[$i]}
	file=${FILES[$i]}
  cross_section=${CROSS_SECTIONS[$i]}

	echo "Starting final state: " ${final_state}
	echo ""

	{ # Brackets allow each process to be run in parallel (until plotting they are independent)
	{

  #--------------------- RUN SIM AND RECO ------------------------#
  output_subdir=${OUTPUT_BASE}/${final_state}
  if [[ ! -d ${output_subdir} ]] ; then
    mkdir ${output_subdir}
  fi

  if $USE_PRERECONSTRUCTED; then
    # Use already reconstructed DST files -> Link them to the output directory
    ${WORKING_DIR}/link_local_files.sh ${final_state} ${DETECTOR_MODEL} ${RECO_FILE_DIR} ${output_subdir}
  elif $RUN_ON_ILCDIRAC; then
    cd ${output_subdir}
    if $RUN_DDSIM && $RUN_PANDORA; then
      # Run
      python ${WORKING_DIR}/ilcdirac_user_jobs/submit_jobs.py -N ${N_EVENTS} --DetectorModel ${DETECTOR_MODEL} --stdhepInput ${file} --processName ${final_state} --ILCSoftVer ${ILCSOFT_VERSION} --ILDConfigVer ${ILDCONFIG_VERSION}
    fi
    # Retrieve output
    dirac-repo-retrieve-jobs-output-data -r ${ILCSOFT_VERSION}_${ILDCONFIG_VERSION}_${final_state}_${DETECTOR_MODEL}.rep
    cd ${WORKING_DIR}
  else
    if $RUN_DDSIM && $RUN_PANDORA; then
      ./run_sim_and_rec.sh -N=${N_EVENTS} --DetectorModel=${DETECTOR_MODEL} --stdhepInput=${file} --processName=${final_state}
    elif $RUN_DDSIM; then
      ./run_sim_and_rec.sh -N=${N_EVENTS} --DetectorModel=${DETECTOR_MODEL} --stdhepInput=${file} --processName=${final_state} --no-pandora
    elif $RUN_PANDORA; then
      ./run_sim_and_rec.sh -N=${N_EVENTS} --DetectorModel=${DETECTOR_MODEL} --stdhepInput=${file} --processName=${final_state} --no-ddsim
    fi

    if $RUN_DDSIM || $RUN_PANDORA; then
      # Clean up unneeded files
      rm ${output_subdir}/${final_state}*.root
    fi
  fi
  
  # Find all DST files that were created
  cd ${output_subdir}
  dst_output_file="$( find $(pwd) -iname "*dst*.slcio" | tr "\n" " " )" # List of absolute paths, line breaks replaced by spaces
  cd ${WORKING_DIR}
  
	#--------------------- RUN ANALYSIS ----------------------------#
	tmp_steering_dir=${STEERING_DIRECTORY}/tmp_${final_state}
	steering_file=${tmp_steering_dir}/My_${final_state}.xml

	output_root_name=${output_subdir}/${final_state}.root

	if $RUN_ANALYSIS; then
		echo "Starting analysis."
		echo ""

		if [[ ! -d ${tmp_steering_dir} ]] ; then
			mkdir ${tmp_steering_dir}
		fi
		cp ${TEMPLATE} ${steering_file}

		# Set the input and output file names in the steering file
    sed -i "123s\.*\ ${cross_section}\ " ${steering_file}
    sed -i "120s\.*\ ${output_root_name} \  " ${steering_file}
    sed -i "13s\.*\ ${dst_output_file}\ " ${steering_file}

		cd ${tmp_steering_dir}

		# Start processor that runs analysis of reconstructed objects
		Marlin My_${final_state}.xml

		cd ${WORKING_DIR}
		# Clean up unneeded files
		rm -r ${tmp_steering_dir}
	fi

	#Do logging for each final state
	} > ${LOG_DIRECTORY}/${final_state}.log 2>${LOG_DIRECTORY}/${final_state}.err
	} & PIDs+=($!) # Add backround process ID to ID list so I can check if it's still running
done

echo "Waiting for sim+reco+analysis chain to finish."
echo ""

# Go through list of background process IDs and if one is still running wait
for pid in "${PIDs[@]}"
do
	wait $pid
done

echo "Simulation, reconstruction and Marlin processors finished."
echo ""

if $RUN_PLOTTING; then
	echo "Start plotting."
	./run_plotting.sh --InputBase=${OUTPUT_BASE}
fi

echo "Done!"
