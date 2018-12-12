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

dir="$( cd "$( dirname "${BASH_SOURCE[0]}"  )" && pwd  )" # Directory of this script

run_script="${dir}/creating_lctuples/run_singleparticle_lctuple.sh"
plotting_script="${dir}/plotting/run_singleparticle_plotting.sh"

chmod u+x ${run_script} ${plotting_script}

filler="*------------------------------------------------------------------------*"

echo "Running Time-Of-Flight script for single particle events."
echo
echo "${filler}"
echo "Creating LCTuple root files."
echo

if $RUN_ON_CONDOR; then
  ${run_script}
else 
  ${run_script} --no-bird
fi

echo
echo "${filler}"
echo "Running plotting."

${plotting_script}

echo
echo "${filler}"
echo "Done with single particle events."
echo "${filler}"
