<!--##########################################
    #     Steering file template for marlin  #
    #     writes an LCTuple with PandoraPFOs #
    #     and TOF parameters                 #
    ##########################################-->


<marlin xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://ilcsoft.desy.de/marlin/marlin.xsd">
 <execute>
  <processor name="MyAIDAProcessor"/>
  <processor name="MyLCTuple"/>  
 </execute>

 <global>
  <parameter name="LCIOInputFiles">    
    {INPUT_FILES}  
  </parameter>
  <!-- limit the number of processed records (run+evt): -->  
  <parameter name="MaxRecordNumber" value="0" />  
  <parameter name="SkipNEvents" value="0"/>  
  <parameter name="SupressCheck" value="false" />  
  <!-- <parameter name="GearXMLFile"></parameter>   -->
  <parameter name="Verbosity" options="DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT">ERROR  </parameter> 
  <parameter name="RandomSeed" value="1234567890" />
 </global>

  <processor name="MyAIDAProcessor" type="AIDAProcessor">
    <parameter name="FileName" type="string">
      {OUTPUT_FILE_BASE} 
    </parameter> 
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
    <!--Definition of PID branches added to the ReconstructedParticle branches - for every algorithm:
    Algorithm:Name branchPrefix Parameter0 branch0  Parameter1 branch1  Parameter2 branch2 ...-->
    <parameter name="PIDBranchDefinition" type="StringVec">
      {TOF_PID_BRANCHES}
    </parameter>
    
    <!--verbosity level of this processor ("DEBUG0-4,MESSAGE0-4,WARNING0-4,ERROR0-4,SILENT")-->
    <!-- <parameter name="Verbosity" type="string">DEBUG </parameter> -->
  </processor>
</marlin>
