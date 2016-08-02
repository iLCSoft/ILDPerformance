#!/bin/bash                                                                                                                                                                                                                     
initscript=/home/ilc/jeans/setup_ilcsoft_kek_v01-17-09.sh
unset MARLIN_DLL
source $initscript
export MARLIN_DLL=../lib/libvalidateSim.so:${MARLIN_DLL}

# infile=/home/ilc/jeans/reco_v1-17-09/REC_noBg_mokka_ILDv05_dbd_si_uds200_00.stdhep_sub0.slcio
infile=mu-_10GeV_isotropic.slcio

parfile=parameter_ranges

cat > pilotjob.xml <<EOF
<marlin>
  <execute>
    <processor name="myPilot"/>
  </execute>

  <global>
    <parameter name="LCIOInputFiles">
$infile
    </parameter>
    <parameter name="MaxRecordNumber" value="0"/>
    <parameter name="SkipNEvents" value="0"/>
    <parameter name="SupressCheck" value="false"/>
    <parameter name="Verbosity" options="DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT"> MESSAGE </parameter>
  </global>

  <processor name="myPilot" type="validationPreProcessor">
    <parameter name="outputParamFilename"> $parfile </parameter>
  </processor>

</marlin>
EOF

Marlin pilotjob.xml

cat > autovalidate.xml <<EOF
<marlin>
  <execute>
    <processor name="myValidateSIM"/>
    <processor name="myValidateREC"/>
  </execute>

  <global>
    <parameter name="LCIOInputFiles">
$infile
    </parameter>
    <parameter name="MaxRecordNumber" value="0"/>
    <parameter name="SkipNEvents" value="0"/>
    <parameter name="SupressCheck" value="false"/>
    <parameter name="Verbosity" options="DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT"> MESSAGE </parameter>
  </global>

  <processor name="myValidateSIM" type="validateAutoProcessor">
     <parameter name="inputParamFilenames"> 
parameter_ranges_SIMCALO.txt
parameter_ranges_SIMTRK.txt
     </parameter>
     <parameter name="outputFilename"> 
validate_SIM.root
     </parameter>
  </processor>

  <processor name="myValidateREC" type="validateAutoProcessor">
     <parameter name="inputParamFilenames"> 
parameter_ranges_CALO.txt
parameter_ranges_TRK.txt
     </parameter>
     <parameter name="outputFilename"> 
validate_REC.root
     </parameter>
  </processor>

</marlin>
EOF

Marlin autovalidate.xml

exit 0