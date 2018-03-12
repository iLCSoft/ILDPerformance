#!/bin/bash

# ----------------------------- FLAGS FOR DIFFERENT STEPS ----------------------- #
RUN_DDSIM=true
RUN_PANDORA=true
RUN_ANALYSIS=true
RUN_PLOTTING=true

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
	-h|--help)
	echo ""
	echo "In ./run_complete_analysis.sh: executed w/ asking for help:"
	echo ""
	echo "Script to run complete simulation, reconstruction and analysis of 2nu4q events."
	echo "Usage: ./run_complete_analysis.sh [-N=number_of_events] [--DetectorModel=detector]"
	echo "                                  [--no-{ddsim,pandora,analysis,plotting}]"
	echo ""
	echo "The --no-... flags suppress the ... part of the process."
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

WORKING_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

STEERING_DIRECTORY=${WORKING_DIR}/../processor/steering_files
OUTPUT_DIRECTORY=${WORKING_DIR}/../output/${DETECTOR_MODEL}
# If not already here create detector model specific output directory 
if [[ ! -d ${OUTPUT_DIRECTORY} ]] ; then
	mkdir ${OUTPUT_DIRECTORY}
fi

LOG_DIRECTORY=${WORKING_DIR}/../log

TEMPLATE=${STEERING_DIRECTORY}/template.xml

N_STATES=3
FINAL_STATES=("vvxyyx" "vvxxxx" "vvyyyy")
FILES=($(./../input_config.sh filepaths)) 
CROSS_SECTIONS=($(./../input_config.sh cross-sections))

PIDs=() #Process IDs keep track of my background processes

# If needed and not already here load ILDConfig from GitHub
if $RUN_DDSIM || $RUN_PANDORA; then
	if [[ ! -d ${WORKING_DIR}/ILDConfig ]] ; then
		echo "Get ILDConfig:"
		git clone https://github.com/iLCSoft/ILDConfig.git
		echo ""
	fi
fi



for (( i=0; i<$(( $N_STATES )); i++ )) do
	final_state=${FINAL_STATES[$i]}
	file=${FILES[$i]}
	cross_section=${CROSS_SECTIONS[$i]}

	echo "Starting final state: " ${final_state}
	echo "" 

	{ # Brackets allow each process to be run in parallel (until plotting they are independent)
	{

	#--------------------- RUN SIM AND RECO ------------------------#
	if $RUN_DDSIM && $RUN_PANDORA; then 
		./run_sim_and_rec.sh -N=${N_EVENTS} --DetectorModel=${DETECTOR_MODEL} --stdhepInput=${file} --processName=${final_state}	
	elif $RUN_DDSIM; then
		./run_sim_and_rec.sh -N=${N_EVENTS} --DetectorModel=${DETECTOR_MODEL} --stdhepInput=${file} --processName=${final_state} --no-pandora
	elif $RUN_PANDORA; then
		./run_sim_and_rec.sh -N=${N_EVENTS} --DetectorModel=${DETECTOR_MODEL} --stdhepInput=${file} --processName=${final_state} --no-ddsim
	fi
		
	# Clean up unneeded files
	rm ${OUTPUT_DIRECTORY}/${final_state}*.root 

	dst_output_file=${OUTPUT_DIRECTORY}/${final_state}_DST.slcio


	#--------------------- RUN ANALYSIS ----------------------------#
	tmp_steering_dir=${STEERING_DIRECTORY}/tmp_${final_state}
	steering_file=${tmp_steering_dir}/My_${final_state}.xml

	output_root_name=${OUTPUT_DIRECTORY}/${final_state}.root 

	if $RUN_ANALYSIS; then 
		echo "Starting analysis."
		echo "" 	

		if [[ ! -d ${tmp_steering_dir} ]] ; then
			mkdir ${tmp_steering_dir}
		fi
		cp ${TEMPLATE} ${steering_file}
		
		# Set the input and output file names in the steering file
		sed -i "116s\.*\ ${cross_section}\ " ${steering_file} 
		sed -i "113s\.*\ ${output_root_name} \  " ${steering_file}
		sed -i "12s\.*\ ${dst_output_file}\ " ${steering_file}
		
		cd ${tmp_steering_dir}
		
		# Start processor that runs analysis of reconstructed objects
		Marlin My_${final_state}.xml
		
		cd ${WORKING_DIR}
		# Clean up unneeded files
		rm -r ${tmp_steering_dir}
	fi

	rm ${OUTPUT_DIRECTORY}/${final_state}*.slcio 

	#Do logging for each final state
	} > ${LOG_DIRECTORY}/${final_state}.log 2>${LOG_DIRECTORY}/${final_state}.err	
	} & PIDs+=($!) # Add backround process ID to ID list so I can check if it's still running

done

# Go through list of background process IDs and if one is still running wait
for pid in "${PIDs[@]}"
do 
	wait $pid
done

echo "Simulation, reconstruction and Marlin processors finished."
echo ""

if $RUN_PLOTTING; then
	echo "Start plotting."

	./run_plotting.sh
fi
