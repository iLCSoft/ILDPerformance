#!/bin/bash

# Python3 is needed -> Specify available command version
PYTHON3="python3.4"

dir="$( cd "$( dirname "${BASH_SOURCE[0]}"  )" && pwd  )" # Directory of this script
config_reader="${dir}/../reading_config/ReadConfig.py"
config_dir="${dir}/../../config"

# Get configurated options
ilcsoft_ver=$(${PYTHON3} ${config_reader} --conf_file Setup.cnf --format single_value --key Environment --variables ILCSoftVersion)
ildconf_ver=$(${PYTHON3} ${config_reader} --conf_file Setup.cnf --format single_value --key Environment --variables ILDConfigVersion)
detector_model=$(${PYTHON3} ${config_reader} --conf_file Setup.cnf --format single_value --key Experiment --variables DetectorModel)
lctuple_output_base="${dir}/../../$(${PYTHON3} ${config_reader} --conf_file Setup.cnf --format single_value --key Environment --variables OutputBase)"

# Input/output paths
run_base="${lctuple_output_base}/SingleParticle_rv${ilcsoft_ver}_sv${ildconf_ver}_mILD_${detector_model}" 
run_input_base="${run_base}/lctuple/"
run_output_dir="${run_base}/plots"
run_output_base="${run_output_dir}/SP"

if [[ ! -d ${run_output_dir} ]]; then
  mkdir --parents ${run_output_dir}
fi

root -l -b -q ${dir}'/draw_tof_singleparticle.cpp("'${config_dir}'","'${run_input_base}'","'${run_output_base}'")' > ${run_output_dir}/plotting.log 2>&1