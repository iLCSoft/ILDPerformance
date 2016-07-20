#!/bin/bash

# script to simulate an event sample, and run the validation processors on the output
# you will need to adapt various file paths etc. to your system
# DJeans July 2016

# check inputs

if [[ $# -ne 7 ]]; then
    echo "call as: sim_and_check.sh runsim runcheck particle(PDG) energy(GeV) nevents"
    exit 1
fi

runsim=$1   # should I run the simulation?
runcheck=$2 # should I run the validation processor
particle=$3 # what particle (PDG)
energy=$4   # what energy particle
nevt=$5     # how many events

isotropic=true
simfile=${particle}_${energy}GeV_isotropic.slcio

dd4hep_xml_file=/path_to/compact/ILD_o1_v05/ILD_o1_v05.xml


initscript=$ILCSOFT/init_ilcsoft.sh

source $initscript

if [[ $runsim == 1 ]]; then  # run ddsim
    ddsim --steeringFile ./my_ddsim_steer.py --gun.isotrop True --gun.particle ${particle} --gun.energy ${energy}*GeV --numberOfEvents $nevt --outputFile $simfile
fi

if [[ $runcheck == 1 ]]; then   # run the validation
    unset MARLIN_DLL
    source $initscript
    export MARLIN_DLL=../lib/libvalidateSim.so:${MARLIN_DLL}
    if [[ ! -e ${simfile} ]]; then
	echo could not find simulation file $simfile
	exit 1
    fi

# write the steering file

    cat > tt.xml <<EOF

<marlin>
   
  <execute>
    <processor name="InitDD4hep"/>

    <processor name="MyVXDValidate"/>
    <processor name="MyFTDValidate"/>
    <processor name="MySITValidate"/>
    <processor name="MySETValidate"/>
    <processor name="MyTPCValidate"/>
    <processor name="MyBeamCalValidate"/>
    <processor name="MyLumiCalValidate"/>
    <processor name="MyLHCalValidate"/>
    <processor name="MyEcalBarrelValidate"/>
    <processor name="MyEcalEndcapsValidate"/>
    <processor name="MyEcalEndcapRingValidate"/>
    <processor name="MyHcalBarrelRegValidate"/>
    <processor name="MyHcalEndcapsValidate"/>
    <processor name="MyHcalEndcapRingValidate"/>
    <processor name="MyYokeBarrelValidate"/>
    <processor name="MyYokeEndcapsValidate"/>


  </execute>

  <global>
    <parameter name="LCIOInputFiles">
      ${simdir}/${simfile}
    </parameter>
    <parameter name="MaxRecordNumber" value="0"/>
    <parameter name="SkipNEvents" value="0"/>
    <parameter name="SupressCheck" value="false"/>
    <parameter name="Verbosity" options="DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT"> MESSAGE </parameter>
  </global>
  
  <processor name="InitDD4hep" type="InitializeDD4hep">
    <!--InitializeDD4hep reads a compact xml file and initializes the DD4hep::LCDD object-->
    <!--Name of the DD4hep compact xml file to load-->
    <parameter name="DD4hepXMLFile" type="string">
      $dd4hep_xml_file
    </parameter>
  </processor>

  <processor name="MyEcalBarrelDigi" type="RealisticCaloDigiSilicon">
    <parameter name="inputHitCollections"> EcalBarrelCollection </parameter>

    <parameter name="outputHitCollections"> EcalBarrelCollectionDigi </parameter>
    <parameter name="outputRelationCollections"> EcalBarrelRelationsSimDigi </parameter>

    <parameter name="threshold"> 0.5 </parameter>
    <parameter name="timingCut"> 1  </parameter>
    <parameter name="calibration_mip"> 1.5e-4  </parameter>
    <parameter name="CellIDLayerString"> layer </parameter>
  </processor>

  <processor name="MyEcalBarrelReco" type="RealisticCaloRecoSilicon">

    <parameter name="inputHitCollections"> EcalBarrelCollectionDigi </parameter>
    <parameter name="inputRelationCollections"> EcalBarrelRelationsSimDigi </parameter>

    <parameter name="outputHitCollections"> EcalBarrelCollectionRec </parameter>
    <parameter name="outputRelationCollections"> EcalBarrelRelationsSimRec </parameter>

    <parameter name="calibration_layergroups"> 20 11 </parameter>
    <parameter name="calibration_factorsMipGev"> 5.2e-3 1.4e-2 </parameter>
    <parameter name="CellIDLayerString"> layer </parameter>
  </processor>

  <processor name="MyEcalBarrelGapFiller" type="BruteForceEcalGapFiller">
    <parameter name="inputHitCollection"> EcalBarrelCollectionRec </parameter>
    <parameter name="outputHitCollection"> EcalBarrelCollectionGapHits </parameter>
    <parameter name="CellIDLayerString"> layer </parameter>
    <parameter name="CellIDModuleString"> module </parameter>
    <parameter name="CellIDStaveString"> stave </parameter>
    <parameter name="expectedInterModuleDistance"> 7.0 </parameter>
  </processor>





  <processor name="MyVXDValidate" type="validationProcessor">
    <parameter name="inputCollectionName">  VXDCollection  </parameter>
    <parameter name="eMax"> 0.0005 </parameter>
    <parameter name="rMax"> 80 </parameter>   
    <parameter name="zMax"> 160 </parameter>
  </processor>

  <processor name="MyFTDValidate" type="validationProcessor">
    <parameter name="inputCollectionName">  FTDCollection </parameter>
    <parameter name="eMax"> 0.0005 </parameter>
    <parameter name="rMax"> 400 </parameter>   
    <parameter name="zMax"> 1000 </parameter>
    <parameter name="inEndcap"> true </parameter>
 </processor>

  <processor name="MySITValidate" type="validationProcessor">
    <parameter name="inputCollectionName">  SITCollection </parameter>
    <parameter name="eMax"> 0.001 </parameter>
    <parameter name="rMax"> 400 </parameter>   
    <parameter name="zMax"> 1000 </parameter>
 </processor>

  <processor name="MySETValidate" type="validationProcessor">
    <parameter name="inputCollectionName">  SETCollection </parameter>
    <parameter name="eMax"> 0.001 </parameter>
    <parameter name="rMax"> 2200 </parameter>   
    <parameter name="zMax"> 2500 </parameter>
 </processor>


  <processor name="MyTPCValidate" type="validationProcessor">
    <parameter name="inputCollectionName">  TPCCollection </parameter>
    <parameter name="eMax"> 0.00001 </parameter>
    <parameter name="rMax"> 2000 </parameter>   
    <parameter name="zMax"> 3000 </parameter>
 </processor>


  <processor name="MyBeamCalValidate" type="validationProcessor">
     <parameter name="inputCollectionName">      BeamCalCollection </parameter>
    <parameter name="eMax"> 0.005 </parameter>
    <parameter name="rMax"> 300 </parameter>   
    <parameter name="zMax"> 4000 </parameter>
    <parameter name="zMin"> 3200 </parameter>
    <parameter name="inEndcap"> true </parameter>
 </processor>


  <processor name="MyLHCalValidate" type="validationProcessor">
    <parameter name="inputCollectionName"> LHCalCollection </parameter>
    <parameter name="eMax"> 0.005 </parameter>
    <parameter name="rMax"> 500 </parameter>   
    <parameter name="zMax"> 3500 </parameter>
    <parameter name="zMin"> 2200 </parameter>
    <parameter name="inEndcap"> true </parameter>
 </processor>


  <processor name="MyLumiCalValidate" type="validationProcessor">
    <parameter name="inputCollectionName"> LumiCalCollection </parameter>
    <parameter name="eMax"> 0.001 </parameter>
    <parameter name="rMax"> 300 </parameter>   
    <parameter name="zMax"> 3000 </parameter>
    <parameter name="zMin"> 2000 </parameter>
    <parameter name="inEndcap"> true </parameter>
 </processor>


  <processor name="MyEcalBarrelValidate" type="validationProcessor">
    <parameter name="inputCollectionName"> EcalBarrelCollection </parameter>
    <parameter name="eMax"> 0.005 </parameter>
    <parameter name="rMax"> 2500 </parameter>   
    <parameter name="zMax"> 3000 </parameter>
 </processor>


  <processor name="MyEcalEndcapsValidate" type="validationProcessor">
    <parameter name="inputCollectionName"> EcalEndcapsCollection </parameter>
    <parameter name="eMax"> 0.005 </parameter>
    <parameter name="rMax"> 2500 </parameter>   
    <parameter name="zMax"> 2800 </parameter>
    <parameter name="zMin"> 2200 </parameter>
    <parameter name="inEndcap"> true </parameter>
 </processor>


  <processor name="MyEcalEndcapRingValidate" type="validationProcessor">
    <parameter name="inputCollectionName"> EcalEndcapRingCollection </parameter>
    <parameter name="eMax"> 0.01 </parameter>
    <parameter name="rMax"> 600 </parameter>   
    <parameter name="zMax"> 2800 </parameter>
    <parameter name="zMin"> 2200 </parameter>
    <parameter name="inEndcap"> true </parameter>
 </processor>


  <processor name="MyHcalBarrelRegValidate" type="validationProcessor">
    <parameter name="inputCollectionName"> HcalBarrelRegCollection </parameter>
    <parameter name="eMax"> 0.01 </parameter>
    <parameter name="rMax"> 3800 </parameter>   
    <parameter name="zMax"> 3000 </parameter>
 </processor>


  <processor name="MyHcalEndcapsValidate" type="validationProcessor">
    <parameter name="inputCollectionName"> HcalEndcapsCollection </parameter>
    <parameter name="eMax"> 0.01 </parameter>
    <parameter name="rMax"> 3500 </parameter>   
    <parameter name="zMax"> 4500 </parameter>
    <parameter name="zMin"> 2200 </parameter>
    <parameter name="inEndcap"> true </parameter>
 </processor>


  <processor name="MyHcalEndcapRingValidate" type="validationProcessor">
    <parameter name="inputCollectionName"> HcalEndcapRingCollection </parameter>
    <parameter name="eMax"> 0.01 </parameter>
    <parameter name="rMax"> 3500 </parameter>   
    <parameter name="zMax"> 2500 </parameter>
    <parameter name="zMin"> 2200 </parameter>
    <parameter name="inEndcap"> true </parameter>
 </processor>


  <processor name="MyYokeBarrelValidate" type="validationProcessor">
    <parameter name="inputCollectionName"> YokeBarrelCollection </parameter>
    <parameter name="eMax"> 0.05 </parameter>
    <parameter name="rMax"> 9000 </parameter>   
    <parameter name="zMax"> 5000 </parameter>
 </processor>


  <processor name="MyYokeEndcapsValidate" type="validationProcessor">
    <parameter name="inputCollectionName"> YokeEndcapsCollection </parameter>
    <parameter name="eMax"> 0.05 </parameter>
    <parameter name="rMax"> 9000 </parameter>   
    <parameter name="zMax"> 7500 </parameter>
    <parameter name="zMin"> 3500 </parameter>
    <parameter name="inEndcap"> true </parameter>
 </processor>




</marlin>

EOF


# run it
    Marlin tt.xml

fi

exit 0
