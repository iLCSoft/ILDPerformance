#include "StepLength.h"
#include <GeometryUtil.h>

StepLength aStepLength ;

void StepLength::initTree(void) {
  streamlog_out(DEBUG4) << " initialise stepTree " << std::endl;

  StepTree = new TTree("StepLength","StepLength");
  StepTree->Branch("PDG",&PDG) ;
  StepTree->Branch("stepLength",&stepLength) ;

  streamlog_out(DEBUG4) << " initialise StepTree " << std::endl;

}


void StepLength::clearVec(void) {

  PDG.clear();  stepLength.clear();

}


void StepLength::initParameters(void) {

  // register steering parameters: name, description, class-variable, default value

  StringVec exampleSimHits ;
  exampleSimHits.push_back("HCalBarrelRPCHits") ;
  exampleSimHits.push_back("HcalBarrelRegCollection") ;
  
  registerInputCollections( LCIO::SIMCALORIMETERHIT,
			   "SimCalorimeterHitCollections" ,
			   "Names of the SimCalorimeterHits input collection"  ,
			   _simCaloHitCollectionNames ,
			    exampleSimHits ) ;
}


StepLength::StepLength() : Processor("StepLength") {

  // modify processor description
  _description = "StepLength does whatever it does ..." ;

  initParameters() ;

}


void StepLength::init() { 

  streamlog_out(DEBUG) << "   StepLength::init() called  "  << std::endl ;

  // usually a good idea to
  printParameters() ;
  nEvt = 0;

  gROOT->ProcessLine("#include <vector>");
  //TApplication theApp("tapp", 0, 0);

}

void StepLength::processRunHeader( LCRunHeader* run) { 
   streamlog_out(MESSAGE) << " start processing run "<<run->getRunNumber() << std::endl;
} 

void StepLength::processEvent( LCEvent * evt ) { 

  streamlog_out(MESSAGE) << " start processing event "<<evt->getEventNumber() << std::endl;

  if( isFirstEvent() ) { 
    streamlog_out(DEBUG4) << " This is the first event " << std::endl;

    initTree() ;

  }

  clearVec() ;

 
  for( unsigned i=0,iN=_simCaloHitCollectionNames.size() ; i<iN ; ++i){

    const LCCollection* col = 0 ;
    try{ col = evt->getCollection( _simCaloHitCollectionNames[i] ) ; } catch(DataNotAvailableException&) {}
    if( col ){
      streamlog_out(DEBUG4) << " We examine collection " << _simCaloHitCollectionNames[i] << " with " << col->getNumberOfElements() << " hits " << std::endl;
	
      for( int j = 0, jN = col->getNumberOfElements() ; j<jN ; ++j ) {
	SimCalorimeterHit* simHit = (SimCalorimeterHit*) col->getElementAt( j ) ;

	for( int k = 0, kN = simHit->getNMCContributions() ; k<kN ; ++k ) {
	  int NMCC_PDG = simHit->getPDGCont(k) ;
	  float NMCC_stepLength = simHit->getLengthCont(k) ;

	  PDG.push_back(NMCC_PDG);
	  stepLength.push_back(NMCC_stepLength);
	}

      }
      
    }
  }

    
  StepTree->Fill();

  nEvt++;
  
  cout << " event " << nEvt << std::endl ;
  
}



void StepLength::check( LCEvent * evt ) { 
   streamlog_out(DEBUG4) << " StepLength::check event "
			 << evt->getEventNumber() <<std::endl;
}


void StepLength::end(){ 
  streamlog_out(DEBUG4) << " StepLength::end() "<<std::endl;
}
