#unset MARLIN_DLL
#. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc41_sl5/v01-17-06/init_ilcsoft.sh

##set -A PolarAngles 10 20 40 85
##set -A y_dir 0.173 0.342 0.643 0.996
##set -A z_dir 0.985 0.94  0.766 0.087 
##set -A Mom 1 3 5 10 15 25 50 100

# in bash it should read:
PolarAngles=('10' '20' '40' '85')
y_dir=('0.173' '0.342' '0.643' '0.996')
z_dir=('0.985' '0.94'  '0.766' '0.087') 
Mom=( '1' '3' '5' '10' '15' '25' '50' '100' )

for i in {0..3}

do

for j in {0..7}

do



#======MOKKA=====#
#======MOKKA=====#
rm -f sim.steer
cat >  sim.steer << EOF   

/Mokka/init/detectorModel ILD_o1_v05

/Mokka/init/registerPlugin TrackingOnlyPlugin 

/Mokka/init/mcRunNumber   20151124
/Mokka/init/randomSeed       420074712
/Mokka/init/userInitString TIMEOUT_TO_RELAX_TMP 120
/Mokka/init/userInitInt SLEEP_BEFORE_RETRY 5
/Mokka/init/user consult
/Mokka/init/dbPasswd consult
/Mokka/init/lcioFilename MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio
/Mokka/init/physicsListName QGSP_BERT
/Mokka/init/TPCCut 0.01 MeV
/Mokka/init/rangeCut 0.1 mm
/Mokka/init/lcioWriteCompleteHepEvt true
/Mokka/init/lcioWriteMode WRITE_NEW

/Mokka/init/lcioDetailedTRKHitMode SETCollection
/Mokka/init/lcioDetailedTRKHitMode SITCollection
/Mokka/init/lcioDetailedTRKHitMode VXDCollection
/Mokka/init/lcioDetailedTRKHitMode FTD_PIXELCollection
/Mokka/init/lcioDetailedTRKHitMode FTD_STRIPCollection
/Mokka/init/lcioDetailedTRKHitMode TPCCollection
/Mokka/init/lcioDetailedTRKHitMode TPCSpacePointCollection

/Mokka/init/lorentzTransformationAngle 0 mrad
/Mokka/init/BFactor 1
/Mokka/init/userDeltaIntersection 1e-5 mm
/Mokka/init/userDeltaOneStep 1e-4 mm
/Mokka/init/lcioDetailedShowerMode false
/Mokka/init/lcioWriteCompleteHepEvt true
/Mokka/init/BatchMode true
/Mokka/init/initialMacroFile muons

EOF
#=================================================

rm -rf muons
cat >  muons << EOF   

/generator/generator particleGun
/gun/position 0 0 0
/gun/direction 0.0 ${y_dir[i]} ${z_dir[i]} 
/gun/phiSmearing 180.0 deg
/gun/momentum ${Mom[j]}
/gun/particle mu-

/run/beamOn 100

EOF
#=================================================

Mokka -U sim.steer

mv MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio ../Results/SimFiles/

#======MARLIN=====#
#======MARLIN=====#

INFILE=../Results/SimFiles/MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio

echo $INFILE

#=================================================

Marlin stdreco_tracking.xml \
    --global.LCIOInputFiles=$INFILE \
    --global.GearXMLFile=GearOutput.xml \
    --MyLCIOOutputProcessor.LCIOOutputFile=Marlin_Reco_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0 

mv Marlin_Reco_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio ../Results/RecoFiles

#======YOUR ANALYSIS PROCESSOR=====#


INFILE=../Results/RecoFiles/Marlin_Reco_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio

#======================================================================

Marlin Diagnostics.xml \
    --global.LCIOInputFiles=$INFILE \
    --global.GearXMLFile=GearOutput.xml \
    --MyAIDAProcessor.FileName=analysis_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]} \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0
#    --MCProcessor.TracksMCTruthLinkCollectionName=MarlinTrkTracksMCTruthLink \
#    --MCProcessor.TracksMCTruthLinkCollectionName=MCTruthMarlinTrkTracksLink \

mv analysis_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.root ../Results/Analysis

#======================================================================
#======================================================================

done

done


#==========YOUR ROOT MACRO==============#

root ../macros/Plots.C


