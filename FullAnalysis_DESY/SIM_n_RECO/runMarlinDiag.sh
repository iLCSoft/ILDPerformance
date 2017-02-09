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
###  force to run on desy hamburg side
#$ -l site="hh"


#=================================================
#
# qsub ./runMarlin.sh SIM01-13-06-pre00_ILD_O1_v03_4f_ww_h_100evt_00 0
#


#=================================================

unset MARLIN_DLL
. /cvmfs/ilc.desy.de/sw/x86_64_gcc49_sl6/v01-19-01/init_ilcsoft.sh


INFILE=../RecoFiles/$1.slcio

Marlin ./Diagnostics.xml \
    --global.LCIOInputFiles=$INFILE \
    --MyAIDAProcessor.FileName=diag_$1 \
    > diag_$1.out 2>&1 

#=================================================

mv diag_$1.root ../Analysis




