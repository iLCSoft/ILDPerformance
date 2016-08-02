#!/bin/zsh

#(make sure the right shell will be used)
#$ -S /bin/zsh
#
#execute the job from the current directory and not relative to your home directory
#$ -cwd
#
# sld5 for bird on NAF2, and sl5 on NAF1
#$ -l arch=amd64
#$ -l os=sld6
#
#(the cpu time for this job)
#$ -l h_rt=23:59:00
#
#(the maximum memory usage of this job)
#$ -l h_vmem=10G
#
#(stderr and stdout are merged together to stdout)
#$ -j y
#
#
###  force to run on desy hamburg side
#$ -l site="hh"


#=================================================

#SET THE DESIRED ILCSOFT RELEASE OR HEAD VERSION
unset MARLIN_DLL
. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc44_sl6/v01-17-09/init_ilcsoft.sh
#. /nfs/dust/ilc/user/voutsina/DBD/init_ilcsoft.sh
#. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc48_sl6/HEAD-2016-05-20/init_ilcsoft.sh

INFILE=/nfs/dust/ilc/user/voutsina/GenFiles/E250-TDR_ws.Pe2e2h_mumu.Gwhizard-1_95.eL.pR.I108009.001.stdhep

# set the ILDConfig version to be used. IMPORTANT: it should be consistent with the ilcsoft release
ILDCONFIG=${ILCSOFT}/ILDConfig/trunk/StandardConfig/current

cp ${ILDCONFIG}/* .
cp -rp ${ILDCONFIG}/* .

#==================================================
#SIMULATION

#======MOKKA=====#
rm -f $1.steer
cat >  $1.steer << EOF   

/Mokka/init/detectorModel ILD_o1_v05

/Mokka/init/mcRunNumber   20120516
/Mokka/init/randomSeed       420074712
/Mokka/init/startEventNumber $3
/Mokka/init/userInitString TIMEOUT_TO_RELAX_TMP 120
/Mokka/init/userInitInt SLEEP_BEFORE_RETRY 5
/Mokka/init/user consult
/Mokka/init/dbPasswd consult
/Mokka/init/lcioFilename  $1.slcio
/Mokka/init/physicsListName QGSP_BERT
/Mokka/init/TPCCut 0.01 MeV
/Mokka/init/rangeCut 0.1 mm
/Mokka/init/lcioWriteCompleteHepEvt true
/Mokka/init/lcioWriteMode WRITE_NEW
/Mokka/init/lorentzTransformationAngle 7 mrad
/Mokka/init/BFactor 1.0
/Mokka/init/MokkaGearFileName GearOutput.xml

/Mokka/init/lcioDetailedTRKHitMode SETCollection
/Mokka/init/lcioDetailedTRKHitMode SITCollection
/Mokka/init/lcioDetailedTRKHitMode VXDCollection
/Mokka/init/lcioDetailedTRKHitMode FTD_PIXELCollection
/Mokka/init/lcioDetailedTRKHitMode FTD_STRIPCollection
/Mokka/init/lcioDetailedTRKHitMode TPCCollection
/Mokka/init/lcioDetailedTRKHitMode TPCSpacePointCollection

/Mokka/init/userDeltaIntersection 1e-5 mm
/Mokka/init/userDeltaOneStep 1e-4 mm
/Mokka/init/lcioDetailedShowerMode false
/Mokka/init/lcioWriteCompleteHepEvt true
/Mokka/init/BatchMode true
/Mokka/init/initialMacroFile $1.g4macro

EOF
#=================================================

rm -rf $1.g4macro
cat >  $1.g4macro << EOF   

/run/verbose 0
/event/verbose 0
/tracking/verbose 0
/generator/generator $INFILE
/run/beamOn $2

EOF

Mokka -M ILD_o1_v05 $1.steer > $1.out 2>&1 

mv $1.slcio ../SimFiles/
#=================================================
# RECONSTRUCTION

Marlin  bbudsc_3evt_stdreco.xml \
    --global.LCIOInputFiles=../SimFiles/$1.slcio \
    --global.GearXMLFile=GearOutput.xml \
    --MyLCIOOutputProcessor.LCIOOutputFile=reco_hz_$1.slcio \
    --global.MaxRecordNumber=$2 \
    --global.SkipNEvents=0 \
    > reco_hz_$1.out 2>&1 

mv reco_hz_$1.slcio ../RecoFiles

#=================================================


Marlin lctuple.xml \
    --global.GearXMLFile=GearOutput.xml \
    --global.LCIOInputFiles=../RecoFiles/reco_hz_$1.slcio \
    --MyAIDAProcessor.FileName=lc_$1 \
    --global.MaxRecordNumber=$2 \
    --global.SkipNEvents=0 \
> lctuple_$1.out 

mv lc_$1.root ../MokkaMadeRootFiles

#cd ../RootFiles
#hadd lctuple.root *root

#done



