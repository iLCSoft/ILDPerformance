#!/bin/bash

# Standard run script that sets up environment and then runs whatever it was given.
# Purpose: This script can easily be given to HTCondor with any command

if ! [[ $# > 0 ]]; then 
  echo "usage: ./standard_bash_run.sh command [arguments]"
  exit
fi

command_with_arguments=$@

echo "Running command: ${command_with_arguments}"
eval "${command_with_arguments}" # Execute command