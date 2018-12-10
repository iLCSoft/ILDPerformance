<?xml version="1.0" encoding="us-ascii"?>
<!-- ?xml-stylesheet type="text/xsl" href="http://ilcsoft.desy.de/marlin/marlin.xsl"? -->
<!-- ?xml-stylesheet type="text/xsl" href="marlin.xsl"? -->

<!-- Loading shared library : /Users/fgaede/marlin/mymarlin/lib/libmymarlin.0.1.0.dylib (libmymarlin.dylib)-->

<!--##########################################
    #                                        #
    #     Example steering file for marlin   #
    #     writes an LCTuple with PandoraPFOs #
    #     and TOF parameters                 #
    ##########################################-->


<marlin xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://ilcsoft.desy.de/marlin/marlin.xsd">
 <execute>
  <processor name="MyAIDAProcessor"/>
  <processor name="TOFEstimators0ps"/>
  <processor name="TOFEstimators10ps"/>
  <processor name="TOFEstimators50ps"/> 
  <processor name="MyLCTuple"/>  
 </execute>

 <global>
  <parameter name="LCIOInputFiles">    
    
  </parameter>
 

  <!-- limit the number of processed records (run+evt): -->  
  <parameter name="MaxRecordNumber" value="0" />  
  <parameter name="SkipNEvents" value="0"/>  
  <parameter name="SupressCheck" value="false" />  
  <parameter name="Verbosity" options="DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT">ERROR  </parameter> 
  <parameter name="RandomSeed" value="1234567890" />
 </global>



 <processor name="MyAIDAProcessor" type="AIDAProcessor">
  <parameter name="FileName" type="string">lctuplettbar </parameter> 
  <parameter name="FileType" type="string">root </parameter>
 </processor>
 <!-- lctuplePi lctuplep lctupleK lctuplee1 lctuplee2-->



 <processor name="MyLCTuple" type="LCTuple">
 <!--LCTuple creates a ROOT TTRee with a column wise ntuple from LCIO collections ....-->

 <!--Name of the ReconstructedParticle collection-->
  <parameter name="RecoParticleCollection" type="string" lcioInType="ReconstructedParticle"> PandoraPFOs </parameter>
  <parameter name="LCRelationWithPFOCollections" type="string" lcioInType="LCRelation"> RecoMCTruthLink </parameter>
  <!--Names of LCRelation collections - need parallel prefix names in RelPrefixes-->
  <parameter name="LCRelationCollections" type="StringVec" lcioInType="LCRelation">
    RecoMCTruthLink 
   </parameter>
  
  <!-- Names of prefixes for variables from LCRelation collections - needs to be parallel to LCRelationCollections (one prefix per collection)-->
  <parameter name="LCRelationPrefixes" type="StringVec">  
    r2m  
  
  </parameter>
  <!--Name of the MCParticle collection-->
  <parameter name="MCParticleCollection" type="string" lcioInType="MCParticle"> MCParticle</parameter>


 <!--########## add TOF estimators for 100, 200 and 300 ps resolution  ###################################### -->

 
  <!--Definition of PID branches added to the ReconstructedParticle branches - for every algorithm:
      Algorithm:Name branchPrefix Parameter0 branch0  Parameter1 branch1  Parameter2 branch2 ...-->
  <parameter name="PIDBranchDefinition" type="StringVec">
    Algorithm:TOFEstimators0ps tof0
    TOFFirstHit TOFClosestHits TOFClosestHitsError TOFFlightLength TOFLastTrkHit TOFLastTrkHitFlightLength
    fh          ch             che                 len             th            thl
    Algorithm:TOFEstimators10ps tof10
    TOFFirstHit TOFClosestHits TOFClosestHitsError TOFFlightLength TOFLastTrkHit TOFLastTrkHitFlightLength
    fh          ch             che                 len             th            thl
    Algorithm:TOFEstimators50ps tof50
    TOFFirstHit TOFClosestHits TOFClosestHitsError TOFFlightLength TOFLastTrkHit TOFLastTrkHitFlightLength
    fh          ch             che                 len             th            thl
  </parameter>
  <!-- Algorithm:TOFEstimators100ps tof100
  TOFFirstHit TOFClosestHits TOFClosestHitsError TOFFlightLength TOFLastTrkHit TOFLastTrkHitFlightLength
  fh          ch             che                 len             th            thl
  Algorithm:TOFEstimators200ps tof200
  TOFFirstHit TOFClosestHits TOFClosestHitsError TOFFlightLength TOFLastTrkHit TOFLastTrkHitFlightLength
  fh          ch             che                 len             th            thl
  Algorithm:TOFEstimators300ps tof300
  TOFFirstHit TOFClosestHits TOFClosestHitsError TOFFlightLength TOFLastTrkHit TOFLastTrkHitFlightLength
  fh          ch             che                 len             th            thl -->
  
  <!-- can also add other PID algorithms, eg: 
    Algorithm:dEdxPID dedx
    electronLikelihood muonLikelihood  pionLikelihood kaonLikelihood protonLikelihood hadronLikelihood
    lhe                lhmu            lhpi           lhk            lhp              lhhad

   -->





  <!--verbosity level of this processor ("DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT")-->
  <!-- <parameter name="Verbosity" type="string">DEBUG </parameter> -->
</processor>

 <processor name="TOFEstimators0ps" type="TOFEstimators">
    <!--TOFEstimators compute some estimators for the time of flight from calorimeter hits-->
    <!--Use only calorimeter hits up to MaxLayerNumber in TOF estimators-->
    <parameter name="MaxLayerNumber" type="int">10 </parameter>
    <!--Name of the ReconstructedParticle collection-->
    <parameter name="ReconstructedParticleCollection" type="string" lcioInType="ReconstructedParticle">PandoraPFOs </parameter>
    <!--Assumed time resolution per hit in ps-->
    <parameter name="TimeResolution" type="float">0 </parameter>
  </processor>
  <processor name="TOFEstimators10ps" type="TOFEstimators">
    <!--TOFEstimators compute some estimators for the time of flight from calorimeter hits-->
    <!--Use only calorimeter hits up to MaxLayerNumber in TOF estimators-->
    <parameter name="MaxLayerNumber" type="int">10 </parameter>
    <!--Name of the ReconstructedParticle collection-->
    <parameter name="ReconstructedParticleCollection" type="string" lcioInType="ReconstructedParticle">PandoraPFOs </parameter>
    <!--Assumed time resolution per hit in ps-->
    <parameter name="TimeResolution" type="float">10 </parameter>
  </processor>
  <processor name="TOFEstimators50ps" type="TOFEstimators">
    <!--TOFEstimators compute some estimators for the time of flight from calorimeter hits-->
    <!--Use only calorimeter hits up to MaxLayerNumber in TOF estimators-->
    <parameter name="MaxLayerNumber" type="int">10 </parameter>
    <!--Name of the ReconstructedParticle collection-->
    <parameter name="ReconstructedParticleCollection" type="string" lcioInType="ReconstructedParticle">PandoraPFOs </parameter>
    <!--Assumed time resolution per hit in ps-->
    <parameter name="TimeResolution" type="float">50 </parameter>
    </processor>

</marlin>
