#setting the environment

unset PYTHONPATH
export PYTHONPATH=$ILCSOFT/DD4hep/HEAD/python:$ILCSOFT/DD4hep/HEAD/DDCore/python:$ILCSOFT/lcio/HEAD/src/python:/afs/desy.de/project/ilcsoft/sw/x86_64_gcc44_sl6/root/5.34.10/lib

cd $DD4HEP
. ./bin/thisdd4hep.sh

cd $ILCSOFT/lcgeo/HEAD
. ./bin/thislcgeo.sh

#seting the environment for the studied detector model - in this case the Simple_ILD
cd $DD4HEP/examples/SimpleDetector
. ./bin/thisSimpleDetector.sh

cd $ILCSOFT/ILDPerformance/trunk/tracking/NewSim

set -A Momenta 1.0  3.0 5.0 10.0 15.0 25.0 50.0 100.0

set -A PolarAngles 85 40 20 10

for j in {1..8}

do

for i in {1..4}

do

# GENERATION - particle gun

python lcio_particle_gun.py ${Momenta[j]} ${PolarAngles[i]}_PixelSIT  mcparticles_${Momenta[j]}_${PolarAngles[i]}_PixelSIT.slcio

#SIMULATION

python runNewSim.py $ILCSOFT/DD4hep/HEAD/examples/SimpleDetector/compact/Simple_ILD.xml mcparticles_${Momenta[j]}_${PolarAngles[i]}_PixelSIT.slcio SIM_${Momenta[j]}_${PolarAngles[i]}_PixelSIT.slcio 
#python runNewSim.py  $ILCSOFT/lcgeo/HEAD/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml mcparticles_${Momenta[j]}_${PolarAngles[i]}_PixelSIT_${PolarAngles[i]}_PixelSIT.slcio SIM_${Momenta[j]}_${PolarAngles[i]}_PixelSIT.slcio 

# RECONSTRUCTION

Marlin run_sitracking.xml \
    --global.GearXMLFile=gear_SimpleILD_DD4hep.xml \
    --global.LCIOInputFiles=SIM_${Momenta[j]}_${PolarAngles[i]}_PixelSIT.slcio \
    --MyLCIOOutputProcessor.LCIOOutputFile=RECO_${Momenta[j]}_${PolarAngles[i]}_PixelSIT.slcio \
    --global.MaxRecordNumber=10 \
    --global.SkipNEvents=0


# aida refitting starts
# currently commented out, until aida is fully tested and debugged

#cd $ILCSOFT/aidaTT/HEAD/build/examples 

#./lcio_tracks $ILCSOFT/DD4hep/HEAD/examples/SimpleDetector/compact/Simple_ILD.xml $ILCSOFT/lcgeo/HEAD/example/RECO_${Momenta[j]}_${PolarAngles[i]}_PixelSIT.slcio  Aida_Refitted_${Momenta[j]}_${PolarAngles[i]}_PixelSIT.slcio

#mv Aida_Refitted_${Momenta[j]}_${PolarAngles[i]}_PixelSIT.slcio Results/PixelSIT



# diagnostics

#INFILE=Results/PixelSIT/Aida_Refitted_${Momenta[j]}_${PolarAngles[i]}_PixelSIT.slcio
INFILE=$ILCSOFT/lcgeo/HEAD/example/RECO_${Momenta[j]}_${PolarAngles[i]}_PixelSIT.slcio

Marlin Diagnostics.xml \
    --global.LCIOInputFiles=$INFILE \
    --global.GearXMLFile=$ILCSOFT/lcgeo/HEAD/example/gear_SimpleILD_DD4hep.xml \
    --MyAIDAProcessor.FileName=anl_${Momenta[j]}_${PolarAngles[i]}_PixelSIT \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0

done

done