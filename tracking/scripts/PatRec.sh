#======YOUR ANALYSIS PROCESSOR=====#


INFILE=/scratch/voutsi/testarea/test_soft/SW_190314/Analysis/OrderedinZ.slcio

#======================================================================

Marlin Diagnostics.xml \
    --global.LCIOInputFiles=$INFILE \
    --global.GearXMLFile=GearOutput.xml \
    --MyAIDAProcessor.FileName=ildperf_mod \
    --global.MaxRecordNumber=0 \
    --global.SkipNEvents=0

#======================================================================


#==========YOUR ROOT MACRO==============#

root ../macros/PatRec.C






