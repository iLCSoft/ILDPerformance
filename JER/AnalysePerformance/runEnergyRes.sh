#======MARLIN=====#

INFILE=/nfs/dust/ilc/user/voutsina/testarea/ILDConfig/dijet_reco.slcio

echo $INFILE

#=================================================

Marlin pfoanalysis.xml \
    --global.LCIOInputFiles=$INFILE \
    --global.GearXMLFile=GearOutput.xml \
    --global.MaxRecordNumber=1000 \
    --global.SkipNEvents=0 


root Jer.C