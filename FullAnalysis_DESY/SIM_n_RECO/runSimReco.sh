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
#
# # qsub ./runMarlinReco.sh mokka.slcio reco_dbd_trk_ 100
#


#=================================================

#SET THE DESIRED ILCSOFT HEAD VERSION
unset MARLIN_DLL
. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc44_sl6/HEAD-2016-01-18/init_ilcsoft.sh


#for i in {1..10}
#do

INFILE=/afs/desy.de/user/v/voutsina/public/w20618_01.stdhep
COMPACTFILE=$lcgeo_DIR/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml
GEARFILE=$ILCSOFT/ILDConfig/HEAD/StandardConfig/lcgeo_current/GearOutput.xml

#==================================================
#SIMULATION

ddsim --inputFiles=$INFILE  --outputFile=$1.slcio --compactFile=$COMPACTFILE --steeringFile=$ILCSOFT/ILDConfig/HEAD/StandardConfig/lcgeo_current/bbudsc_3evt_ddsim.steer --numberOfEvents=$2  --skipNEvents=$3

mv $1.slcio ../SimFiles
#=================================================
# RECONSTRUCTION

cp $ILCSOFT/ILDConfig/HEAD/StandardConfig/lcgeo_current/Pandora* .

Marlin  $ILCSOFT/ILDConfig/HEAD/StandardConfig/lcgeo_current/bbudsc_3evt_stdreco_dd4hep.xml \
    --global.LCIOInputFiles=../SimFiles/$1.slcio \
    --global.GearXMLFile=$GEARFILE \
    --InitDD4hep.DD4hepXMLFile=$COMPACTFILE \
    --MyLCIOOutputProcessor.LCIOOutputFile=reco_$1.slcio \
    --global.MaxRecordNumber=$2 \
    --global.SkipNEvents=0 
    > reco_$1.out 2>&1 

mv reco_$1.slcio ../RecoFiles

#=================================================


Marlin lctuple.xml \
    --global.GearXMLFile=$GEARFILE \
    --global.LCIOInputFiles=../RecoFiles/reco_$1.slcio \
    --MyAIDAProcessor.FileName=lc_$1 \
    --global.MaxRecordNumber=$2 \
    --global.SkipNEvents=0 \
> lctuple_$1.out 

mv lc_$1.root ../RootFiles

cd ../RootFiles
hadd lctuple.root *root

#done



