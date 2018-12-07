#!/bin/bash

for i in "$@"
do
case $i in
    --InputBase=*)
    INPUT_BASE="${i#*=}"
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

LUMINOSITY=($(./../input_config.sh luminosity))
POLARISATIONS=($(./../input_config.sh polarisations))

root -l -b -q '../plotting/create_mjjmjjplots.cc("'$LUMINOSITY'","'${POLARISATIONS[0]}'","'${POLARISATIONS[1]}'","'${INPUT_BASE}'")'
