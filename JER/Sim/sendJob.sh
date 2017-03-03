#!/bin/zsh
########################################################
# This script is to generate kaon0L and electron samples 
# for checking the calorimeter performance
########################################################

ilcsoft=HEAD-2017-03-02
detector=ILD_o1_v05
nevents=1000

for particle (kaon0L e-)
do
    
    for energy (5 10 20 30 40 50 60 70 80 90 100)
    do
	for randomSeed (`seq 1 1 10`)
	do
	    
	    ./changeXML.sh run_ddsim.sub run_ddsim_${particle}_${energy}GeV_${randomSeed}.sub ddsim_steering.py ddsim_steering_${particle}_${energy}GeV_${randomSeed}.py $particle $energy $nevents $randomSeed $ilcsoft $detector

	    qsub run_ddsim_${particle}_${energy}GeV_${randomSeed}.sub
	
	done
    done
done