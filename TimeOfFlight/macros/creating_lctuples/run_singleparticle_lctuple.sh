#!/bin/bash

# Read input parameters
RUN_ON_CONDOR=true

for i in "$@" 
do
  case $i in
    --no-bird)
    RUN_ON_CONDOR=false
    ;;
    *)
    # Unknown option
    ;;
  esac
done

# Python3 is needed -> Specify available command version
PYTHON3="python3.4"

# Directory environment (where to find stuff)
dir="$( cd "$( dirname "${BASH_SOURCE[0]}"  )" && pwd  )" # Directory of this script
config_reader="${dir}/../reading_config/ReadConfig.py"
scripts_dir="${dir}/../../scripts"
template_file="${scripts_dir}/template_lctuple_steering_singleparticle.py"

submit_file_dir="${dir}/../submitting_bird_jobs"
submit_file_path="${submit_file_dir}/standard_htcondor_steering.submit"
bird_output_dir="${dir}/../../BIRD_output"
if [[ ! -d ${bird_output_dir} ]]; then
  mkdir --parents ${bird_output_dir}
fi

# Get configurated options
input_path=$(${PYTHON3} ${config_reader} --conf_file Setup.cnf --format single_value --key SingleParticle --variables InputDirectory)
ilcsoft_ver=$(${PYTHON3} ${config_reader} --conf_file Setup.cnf --format single_value --key Environment --variables ILCSoftVersion)
ildconf_ver=$(${PYTHON3} ${config_reader} --conf_file Setup.cnf --format single_value --key Environment --variables ILDConfigVersion)
detector_model=$(${PYTHON3} ${config_reader} --conf_file Setup.cnf --format single_value --key Experiment --variables DetectorModel)
output_base="${dir}/../../$(${PYTHON3} ${config_reader} --conf_file Setup.cnf --format single_value --key Environment --variables OutputBase)"

time_resolutions="$(${PYTHON3} ${config_reader} --conf_file TimeResolutions.cnf --format list_of_keys)"

# Directory structures derived from configuration
run_base="SingleParticle_rv${ilcsoft_ver}_sv${ildconf_ver}_mILD_${detector_model}"
run_steering_dir="${scripts_dir}/${run_base}"
run_output_dir="${output_base}/${run_base}/lctuple"

if [[ ! -d ${run_steering_dir} ]]; then
  mkdir --parents ${run_steering_dir}
fi
if [[ ! -d ${run_output_dir} ]]; then
  mkdir --parents ${run_output_dir}
fi

# Loop to run lctuple for each particle type
for ptype in $(${PYTHON3} ${config_reader} --conf_file ParticleTypes.cnf --format list_of_variables --variables pdgID); do
  echo "Starting to run single particle jobs for particle type: ${ptype}"
  {
    # Uses file convention for single particle files
    files=$(find ${input_path} -iname "*rv${ilcsoft_ver}*sv${ildconf_ver}*mILD_${detector_model}*calib*pm${ptype}_prnd*e0.p0*rec*.slcio" -print)
    
    type_output_base="${run_output_dir}/${ptype}"
    
    if $RUN_ON_CONDOR; then
      echo "Will run jobs on BIRD cluster."
      type_steering_dir="${run_steering_dir}/${ptype}"
      type_output_dir="${type_output_base}"
      
      if [[ ! -d ${type_steering_dir} ]]; then
        mkdir --parents ${type_steering_dir}
      fi
      if [[ ! -d ${type_output_dir} ]]; then
        mkdir --parents ${type_output_dir}
      fi

      # Send Marlin job to HTCondor for each individual input file
      file_index=0
      for file in ${files[@]}; do
        {
          steering_path="${type_steering_dir}/${file_index}.xml"
          lctuple_output_base="${type_output_dir}/${file_index}"

          ${PYTHON3} ${dir}/SetSteeringFile.py --steering_template ${template_file} --input_files ${file} --output_base ${lctuple_output_base} --resolutions ${time_resolutions} --result_path ${steering_path}
          
          # Submit job and wait for it
          cd ${submit_file_dir}
          condor_job_output=$(condor_submit ${submit_file_path} arguments="source ${ILCSOFT}/init_ilcsoft.sh \&\& Marlin ${steering_path}")
          job_ID="${condor_job_output##* }"
          job_log_path=$(ls ${bird_output_dir}/${job_ID}*.log)
          wait_output=$(condor_wait ${job_log_path}) # Write into variable to suppress spammy output
          cd -
        } > ${type_steering_dir}/${file_index}.log 2>&1 & 
        file_index=$((file_index + 1))
        sleep 0.1 # to avoid machine spamming and scheduler overload
      done
      wait
      
      # Combine results of individual lctuples
      echo "All individual lctuples done, combining them."
      hadd -k -f ${type_output_base}_lctuple.root ${type_output_dir}/*.root
      rm -r ${type_output_dir}
    else
      # When running on local machine (Not recommended)
      echo "Will run jobs on local machine."
      steering_path="${run_steering_dir}/${ptype}_all.xml"
      lctuple_output_base="${type_output_base}_lctuple"

      ${PYTHON3} ${dir}/SetSteeringFile.py --steering_template ${template_file} --input_files ${files} --output_base ${lctuple_output_base} --resolutions ${time_resolutions} --result_path ${steering_path}
      Marlin ${steering_path}
    fi
  } > ${run_steering_dir}/${ptype}.log 2>&1 &
  sleep 10 # to avoid machine spamming and scheduler overload
done
wait
echo "Done with all particle types!"
