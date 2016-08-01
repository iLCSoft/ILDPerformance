#!/bin/bash                                                                                                                                                                                                                     
initscript=/home/ilc/jeans/setup_ilcsoft_kek_v01-17-09.sh
unset MARLIN_DLL
source $initscript
export MARLIN_DLL=../lib/libvalidateSim.so:${MARLIN_DLL}

infile=mu-_10GeV_isotropic.slcio

parfile=parameter_ranges.txt

cat > pilotjob.xml <<EOF
<marlin>
  <execute>
    <processor name="myPilot"/>
  </execute>

  <global>
    <parameter name="LCIOInputFiles">
$infile
    </parameter>
    <parameter name="MaxRecordNumber" value="1000"/>
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
    <processor name="myValidate"/>
  </execute>

  <global>
    <parameter name="LCIOInputFiles">
$infile
    </parameter>
    <parameter name="MaxRecordNumber" value="1000"/>
    <parameter name="SkipNEvents" value="0"/>
    <parameter name="SupressCheck" value="false"/>
    <parameter name="Verbosity" options="DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT"> MESSAGE </parameter>
  </global>

  <processor name="myValidate" type="validateAutoProcessor">
     <parameter name="inputParamFilename"> $parfile </parameter>
  </processor>

</marlin>
EOF

Marlin autovalidate.xml

exit 0