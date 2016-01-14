PolarAngles=('10' '20' '40' '85')
Mom=( '1' '3' '5' '10' '15' '25' '50' '100' )


for j in {0..7}

do

for i in {0..3}

do

# GENERATION - particle gun

python lcio_particle_gun.py ${Mom[j]} ${PolarAngles[i]}  mcparticles_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio 13 -1.

#SIMULATION

#python ddsim.py $lcgeo_DIR/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml mcparticles_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio SIM_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio 
ddsim --inputFiles=mcparticles_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio  --outputFile=SIM_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio --compactFile $lcgeo_DIR/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml --steeringFile=$ILCSOFT/ILDConfig/HEAD/StandardConfig/lcgeo_current/bbudsc_3evt_ddsim.steer --numberOfEvents=1000

# RECONSTRUCTION

Marlin $ILCSOFT/ILDConfig/HEAD/StandardConfig/lcgeo_current/bbudsc_3evt_stdreco_dd4hep.xml \
    --global.GearXMLFile=$ILCSOFT/ILDConfig/HEAD/StandardConfig/lcgeo_current/gear_ILD_o1_v05_dd4hep.xml \
    --global.LCIOInputFiles=SIM_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio \
    --InitDD4hep.DD4hepXMLFile=$lcgeo_DIR/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml \
    --MyLCIOOutputProcessor.LCIOOutputFile=RECO_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0 \
> RECO_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out 

# REFITTING

Marlin run_refit_aidaTT.xml \
    --global.GearXMLFile=$ILCSOFT/ILDConfig/HEAD/StandardConfig/lcgeo_current/gear_ILD_o1_v05_dd4hep.xml \
    --global.LCIOInputFiles=RECO_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio \
    --InitDD4hep.DD4hepXMLFile=$lcgeo_DIR/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml \
    --MyLCIOOutputProcessor.LCIOOutputFile=REFIT_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0 \
> Refit_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out 



# diagnostics
# Choose which file you want to run diagnostics

INFILE=REFIT_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio
#INFILE=RECO_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio

Marlin Diagnostics.xml \
    --global.LCIOInputFiles=$INFILE \
    --global.GearXMLFile=$ILCSOFT/ILDConfig/HEAD/StandardConfig/lcgeo_current/gear_ILD_o1_v05_dd4hep.xml \
    --MyAIDAProcessor.FileName=analysis_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]} \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0 \
> DIAG_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out

mv  analysis_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.root ../Results/Analysis

done

done

#==========YOUR ROOT MACRO==============#

root ../macros/Plots.C