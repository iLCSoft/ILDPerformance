#!/bin/bash

if [[  $# != 4 ]]  ; then
  echo "usage: ./link_local_files.sh final_state detector_model file_dir output_subdir"
  exit
fi

# Get input parameters
final_state=${1}
detector_model=${2}
file_dir=${3}
output_subdir=${4}

# Convert final state to ILD conventions
if   [[ ${final_state} == "vvxyyx" ]]; then
  ILD_final_state="vvxyyx"
elif [[ ${final_state} == "vvxxxx" ]]; then
  ILD_final_state="xxxxvv"
elif [[ ${final_state} == "vvyyyy" ]]; then
  ILD_final_state="yyyyvv"
else 
  (>&2 echo "ERROR in link_local_files.sh: Unknown final state ${final_state} ! Exiting.")
  exit
fi

# Case insensitive search for DST files in the given directory
files=$(find ${file_dir} -iname "*rv${RECONSTRUCTION_SOFTWARE_VERSION}*sv${SIMULATION_SOFTWARE_VERSION}*m${detector_model}*E1000*P${ILD_final_state}*eL.pR*DST*.slcio" -print)
if [[ files == "" ]]; then
  (>&2 echo "ERROR in link_local_files.sh: No ${final_state} found in ${file_dir} ! Exiting.")
  exit
fi

echo "Creating symbolic links for local files of ${final_state}."
ln -s ${files} ${output_subdir}