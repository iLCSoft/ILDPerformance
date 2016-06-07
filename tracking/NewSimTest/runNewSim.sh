PolarAngles=('10' '20' '40' '85')
Mom=( '1' '3' '5' '10' '15' '25' '50' '100' )

# need to set the correct path to ILDConfig
ILDCONFIG=ILDConfig_path
#fg: need to copy all config files here

cp ${ILDCONFIG}/StandardConfig/lcgeo_current/* .
cp -rp ${ILDCONFIG}/StandardConfig/lcgeo_current/* .

for j in {0..7}

do

for i in {0..3}

do


# GENERATION - particle gun

python lcio_particle_gun.py ${Mom[j]} ${PolarAngles[i]}  mcparticles_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio 13 -1.

#SIMULATION
ddsim --inputFiles=mcparticles_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio  --outputFile=SIM_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio --compactFile $lcgeo_DIR/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml --steeringFile=${ILDCONFIG}/StandardConfig/lcgeo_current/ddsim_steer.py --numberOfEvents=1000

mv SIM_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio Results/SimFiles

# RECONSTRUCTION

Marlin ${ILDCONFIG}/StandardConfig/lcgeo_current/bbudsc_3evt_stdreco_dd4hep.xml \
    --global.GearXMLFile=${ILDCONFIG}/StandardConfig/lcgeo_current/GearOutput.xml \
    --global.LCIOInputFiles=Results/SimFiles/SIM_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio \
    --InitDD4hep.DD4hepXMLFile=$lcgeo_DIR/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml \
    --MyLCIOOutputProcessor.LCIOOutputFile=RECO_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0 \
> RECO_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out 

mv RECO_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio Results/RecoFiles

# REFITTING

Marlin run_refit_aidaTT.xml \
    --global.GearXMLFile=${ILDCONFIG}/StandardConfig/lcgeo_current/GearOutput.xml \
    --global.LCIOInputFiles=Results/RecoFiles/RECO_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio \
    --InitDD4hep.DD4hepXMLFile=$lcgeo_DIR/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml \
    --MyLCIOOutputProcessor.LCIOOutputFile=REFIT_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0 \
> Refit_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out 

mv REFIT_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio Results/RefitFiles


# diagnostics
# Choose which file you want to run diagnostics

#INFILE=Results/RefitFiles/REFIT_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio
INFILE=Results/RecoFiles/RECO_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.slcio

Marlin Diagnostics.xml \
    --global.LCIOInputFiles=$INFILE \
    --global.GearXMLFile=${ILDCONFIG}/StandardConfig/lcgeo_current/GearOutput.xml \
    --MyAIDAProcessor.FileName=analysis_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]} \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0 \
> DIAG_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.out

mv  analysis_MuonsAngle_${PolarAngles[i]}_Mom_${Mom[j]}.root ../Results/Analysis

done

done

#==========YOUR ROOT MACRO==============#

root ../macros/Plots.C
