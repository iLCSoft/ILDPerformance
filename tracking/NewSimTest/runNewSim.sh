#set -A Momenta 1.0 3.0 5.0 10.0 15.0 25.0 50.0 100.0
#set -A PolarAngles 85 

PolarAngles=('10' '20' '40' '85')
Momenta=( '1' '3' '5' '10' '15' '25' '50' '100' )

for j in {0..7}

do

for i in {0..3}

do

# GENERATION - particle gun

python lcio_particle_gun.py ${Momenta[j]} ${PolarAngles[i]}  mcparticles_${Momenta[j]}_${PolarAngles[i]}.slcio 13 -1.

#SIMULATION

python ddsim.py $lcgeo_DIR/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml mcparticles_${Momenta[j]}_${PolarAngles[i]}.slcio SIM_${Momenta[j]}_${PolarAngles[i]}.slcio 

# RECONSTRUCTION

Marlin bbudsc_3evt_stdreco_dd4hep.xml \
    --global.GearXMLFile=gear_ILD_o1_v05_dd4hep.xml \
    --global.LCIOInputFiles=SIM_${Momenta[j]}_${PolarAngles[i]}.slcio \
    --InitDD4hep.DD4hepXMLFile=$lcgeo_DIR/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml \
    --MyLCIOOutputProcessor.LCIOOutputFile=RECO_${Momenta[j]}_${PolarAngles[i]}.slcio \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0


# For time being we run aida refitter during the reconstruction
# aida refitting starts
# Do you really want to use aida fitter?

#Marlin run_refit_aidaTT.xml \ 
#    --global.GearXMLFile=gear_ILD_o1_v05_dd4hep.xml \
#    --global.LCIOInputFiles=RECO_${Momenta[j]}_${PolarAngles[i]}.slcio \
#    --InitDD4hep.DD4hepXMLFile=/afs/desy.de/project/ilcsoft/sw/x86_64_gcc44_sl6/HEAD-2015-11-23/lcgeo/HEAD/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml \
#    --MyLCIOOutputProcessor.LCIOOutputFile=REFIT_${Momenta[j]}_${PolarAngles[i]}.slcio \
#    --global.MaxRecordNumber=1000 \
#    --global.SkipNEvents=0


# diagnostics
# Choose which file you want to run diagnostics

#INFILE=REFIT_${Momenta[j]}_${PolarAngles[i]}.slcio
INFILE=RECO_${Momenta[j]}_${PolarAngles[i]}.slcio

Marlin Diagnostics.xml \
    --global.LCIOInputFiles=$INFILE \
    --global.GearXMLFile=gear_ILD_o1_v05_dd4hep.xml \
    --MyAIDAProcessor.FileName=anl_${Momenta[j]}_${PolarAngles[i]} \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0

mv  anl_${Momenta[j]}_${PolarAngles[i]}.root ../Results/Analysis

done

done

#==========YOUR ROOT MACRO==============#

root ../macros/Plots.C