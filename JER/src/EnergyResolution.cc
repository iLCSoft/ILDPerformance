#include "EnergyResolution.h"
#include <iostream>

//#ifdef MARLIN_USE_AIDA
//#endif

using namespace lcio ;
using namespace marlin ;
using namespace std ;

EnergyResolution aEnergyResolution ;


EnergyResolution::EnergyResolution() : Processor("EnergyResolution") {
  

  // modify processor description
  _description = "EnergyResolution studies" ;
  

  // register steering parameters: name, description, class-variable, default value

  
  registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
                            "PFOs" , 
                            "particle flow objects"  ,
                            _pandoraPFOs ,
                            std::string("PandoraPFOs") ) ;

  registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
                            "Jets" , 
                            "Jets"  ,
                            _jets ,
                            std::string("RefinedJets") ) ;

  registerInputCollection( LCIO::MCPARTICLE,
			   "MCParticle" , 
			   "Monte Carlo particles collection"  ,
			   _mcparticles,
			   std::string("MCParticlesSkimmed") ) ;

  registerInputCollection( LCIO::LCRELATION,
                           "RecoMCTruthLinkName" , 
                           "Name of the RecoMCTruthLink output collection"  ,
                           _recoToTrue,
                           std::string("RecoMCTruthLink") ) ;

}


void EnergyResolution::init() { 

  message<DEBUG>( "     EnergyResolution::init()  " ) ;

  // usually a good idea to
  printParameters() ;

  _nRun = 0 ;
  _nEvt = 0 ;
}

void EnergyResolution::processRunHeader( LCRunHeader* run) { 

  _nRun++ ;

} 

void EnergyResolution::processEvent( LCEvent * evt ) { 

  if( isFirstEvent() ) { 

    t1 = new TTree("t1","t1") ;
    t1->Branch("recoEnergy",&recoEnergy) ;
    t1->Branch("PFOs",&PFOs,"PFOs/I") ;
    
    //hist_mcp  = new TH1F( "hist_mcp", "Pt distributions of charged mc particles", 20 , 0., 2.0 ) ;
    //hist_rp  = new TH1F( "hist_rp", "Pt distribution of charged reconstructed particles",  20 , 0., 2.0 ) ;    
    deltaE = new TH1F("deltaE","deltaE",50,-10,10);
  }

  recoEnergy.clear();

  LCCollection *Jets = evt->getCollection( _jets ) ;
  int NoOfJets = Jets->getNumberOfElements();

  for (int i = 0 ; i < NoOfJets ; i++ ){

    ReconstructedParticle *jet = dynamic_cast<ReconstructedParticle*>( Jets->getElementAt( i ) ) ;

    double jetEnergy = jet->getEnergy();
    deltaE->Fill( jetEnergy - 45.0 ) ;
  }


  LCCollection* PandoraPFO = evt->getCollection( _pandoraPFOs ) ;
  PFOs = PandoraPFO->getNumberOfElements();

  std::cout << " I have #PFOs " << PFOs << std::endl ;


  LCCollection* montecarlo = evt->getCollection( _mcparticles );
  LCCollection* recoToMcp = evt->getCollection( _recoToTrue ) ;
  LCRelationNavigator navRtT( recoToMcp ) ;

  for (int i=0; i<montecarlo->getNumberOfElements();i++){
    MCParticle *mcp = dynamic_cast<MCParticle*>( montecarlo->getElementAt( i ) ) ;

    const EVENT::LCObjectVec& recovec = navRtT.getRelatedFromObjects(mcp);
    FloatVec testFromWgt = navRtT.getRelatedFromWeights(mcp);
    //std::cout << " Reco - truth relation weight " << testFromWgt.size() << std::endl;

    for (int jj=0;jj<testFromWgt.size();jj++){
      //std::cout << " Reco - truth relation weight " << testToWgt[jj] << std::endl;
      int RECOPDG = ((ReconstructedParticle*)recovec[jj])->getType();
      //std::cout << " related monte carlo particle " << mcp->getPDG() << " and PFO " << RECOPDG << " with weight " << testFromWgt[jj] << std::endl ; 
      
      recoEnergy.push_back(((ReconstructedParticle*)recovec[jj])->getEnergy());
      double trueEnergy = mcp->getEnergy();

    }
  }

  t1->Fill();

}



void EnergyResolution::check( LCEvent * evt ) { 
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void EnergyResolution::end(){ 

  deltaE->Draw();
  deltaE->Fit("gaus");

}

