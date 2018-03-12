#!/bin/bash

DETECTOR_MODEL=ILD_l5_o1_v02

for i in "$@"
do
case $i in
    --DetectorModel=*)
    DETECTOR_MODEL="${i#*=}"
    shift # past argument=value
    ;;
    *)
          # unknown option
    ;;
esac
done

LUMINOSITY=($(./../input_config.sh luminosity))
POLARISATIONS=($(./../input_config.sh polarisations))

root -l -b -q '../plotting/create_mjjmjjplots.cc("'$LUMINOSITY'","'${POLARISATIONS[0]}'","'${POLARISATIONS[1]}'","'${DETECTOR_MODEL}'")'
