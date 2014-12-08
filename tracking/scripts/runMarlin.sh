#unset MARLIN_DLL
#. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc41_sl5/v01-17-06/init_ilcsoft.sh

set -A PolarAngles 10 20 40 85
set -A y_dir 0.173 0.342 0.643 0.996
set -A z_dir 0.985 0.94  0.766 0.087 
set -A Mom 1 3 5 10 15 25 50 100

for i in {1..4}

do

for j in {1..8}

do



#======MARLIN=====#
#======MARLIN=====#

INFILE=../Results/SimFiles/MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio

echo $INFILE

#=================================================

Marlin stdreco_tracking.xml \
    --global.LCIOInputFiles=$INFILE \
    --global.GearXMLFile=GearOutput.xml \
    --MyLCIOOutputProcessor.LCIOOutputFile=Marlin_Reco_FullTrks_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0 

mv Marlin_Reco_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio ../Results/RecoFiles



done

done





