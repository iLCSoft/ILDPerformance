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
# # qsub ./runMarlinReco.sh sim.slcio reco_dbd_trk_ 100
#


#=================================================

#SET THE DESIRED ILCSOFT RELEASE OR HEAD VERSION
unset MARLIN_DLL
. /cvmfs/ilc.desy.de/sw/x86_64_gcc49_sl6/v01-19-01/init_ilcsoft.sh

# set the ILDConfig version to be used. IMPORTANT: it should be consistent with the ilcsoft release
ILDCONFIG=${ILCSOFT}/ILDConfig/HEAD

#for i in {1..10}
#do

#INFILE=/nfs/dust/ilc/user/voutsina/GenFiles/E250-TDR_ws.Pe2e2h_mumu.Gwhizard-1_95.eL.pR.I108009.001.stdhep
#INFILE=/nfs/dust/ilc/group/ild/dbd-data/250/higgs/E250-TDR_ws.Pe2e2h_mumu.Gwhizard-1_95.eL.pR.I108009.001.stdhep
INFILE=/nfs/dust/ilc/group/ild/dbd-data/500/6f/E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.stdhep
COMPACTFILE=$lcgeo_DIR/ILD/compact/ILD_l1_v01/ILD_l1_v01.xml
GEARFILE=${ILDCONFIG}/StandardConfig/lcgeo_current/gear_ILD_l1_v01_dd4hep.xml

#==================================================
#SIMULATION

ddsim --inputFiles=$INFILE  --outputFile=$1.slcio --compactFile=$COMPACTFILE --steeringFile=${ILDCONFIG}/StandardConfig/lcgeo_current/ddsim_steer.py --numberOfEvents=$2  --skipNEvents=$3

mv $1.slcio ../SimFiles
#=================================================
# RECONSTRUCTION

cp ${ILDCONFIG}/StandardConfig/lcgeo_current/* .
cp -rp ${ILDCONFIG}/StandardConfig/lcgeo_current/* .
#fg: need to copy all config files here

Marlin  ${ILDCONFIG}/StandardConfig/lcgeo_current/bbudsc_3evt_stdreco_dd4hep.xml \
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



