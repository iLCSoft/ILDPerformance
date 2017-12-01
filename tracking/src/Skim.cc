#include "Skim.h"
#include <iostream>

#include <cstdlib>

// LCIO 
#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>
// #include <EVENT/LCRelation.h>
#include <IMPL/LCCollectionVec.h>

#include "TLorentzVector.h"
#include <math.h>

#ifdef MARLIN_USE_AIDA
#include <marlin/AIDAProcessor.h>
#include <AIDA/AIDA.h>
#endif


using namespace lcio ;
using namespace marlin ;



Skim aSkim;


struct MCPKeep :  public LCIntExtension<MCPKeep> {} ;

typedef std::map< MCParticle* , int > MCPMap ;

typedef std::map< MCParticle* , double > MCPMapDouble ;

typedef std::map< MCParticle* , MCParticle* >  Remap_as_you_go;

Skim::Skim() : Processor("Skim") {
  
  // modify processor description
  _description = "skim the MCParticle collection to MC charged particle with 40<theta<140 (vertex barrel)" ;
  
  
  registerInputCollection( LCIO::MCPARTICLE,
			   "MCParticleCollectionName" , 
			   "Name of the MCParticle input collection"  ,
			   _mcpColName ,
			   std::string("MCParticle") ) ;
  
  registerOutputCollection( LCIO::MCPARTICLE,
			    "MCParticleSkimmedCollectionName" , 
			    "Name of the skimmed MCParticle  output collection"  ,
			    _mcpSkimColName ,
			    std::string("MCParticleSkimmed") ) ;

  registerProcessorParameter("ThetaMin",
                             "Minimum polar angle (in deg) to accept MC in skim",
                             _thetaMin,
                             double(40.));

  registerProcessorParameter("ThetaMax",
                             "Maximum polar angle (in deg) to accept MC in skim",
                             _thetaMax,
                             double(140.));
  
}


void Skim::init() { 
  
  // usually a good idea to
  printParameters<MESSAGE>() ;
     
  _nRun = 0 ;
  _nEvt = 0 ;
}


void Skim::processRunHeader( LCRunHeader* run) { 

  streamlog_out( DEBUG4 ) << " processRun "  << run->getRunNumber()
			  << std::endl ;
  _nRun++ ;
} 


void Skim::processEvent( LCEvent * evt ) { 
  
  _nEvt ++ ;
  
  
  streamlog_out( DEBUG4 ) << " processEvent "  <<  evt->getEventNumber() 
			  << "  - " << evt->getRunNumber() 
			  << std::endl ; 
  
  
  LCCollectionVec* skimVec = new LCCollectionVec( LCIO::MCPARTICLE )  ;
  skimVec->setSubset(true) ; 

  //make the skim here

  LCCollection* mcpCol = evt->getCollection( _mcpColName) ;
  if( mcpCol != NULL ){


    int mcpN = mcpCol->getNumberOfElements();
    
    for(int i=0; i< mcpN ; i++){
      MCParticle* mcp = dynamic_cast<MCParticle*>( mcpCol->getElementAt(i) );
      
      bool is_stable = mcp->getDaughters().size() == 0; 
      bool is_charged = mcp->getCharge()!=0.;

      const double* mcpMom = mcp->getMomentum(); 
      double e = mcp->getEnergy();
      TLorentzVector tlv_mcp;
      tlv_mcp.SetPxPyPzE(mcpMom[0],mcpMom[1],mcpMom[2],e); 
      double theta = tlv_mcp.Theta()*180./acos(-1.);
      // double theta_min = 40.;
      // double theta_max = 140.;
      // bool is_barrel = theta>=theta_min && theta<=theta_max;
      bool is_barrel = theta>=_thetaMin && theta<=_thetaMax;


      if (is_stable && is_charged && is_barrel) {    
	if( mcp->ext<MCPKeep>() == true )  continue ;   //particle already in skim
	keepMCParticle( mcp, false ) ;
      }

    }//end loop on MC


    //not done in one loop in case parents are also wanted

    streamlog_out( DEBUG4 ) << " All found, add to skimmed list " << std::endl;
  
    for(int i=0; i< mcpN ; i++){    

      MCParticle* mcp = dynamic_cast<MCParticle*> ( mcpCol->getElementAt( i ) ) ;      
      if( mcp->ext<MCPKeep>() == true )  skimVec->addElement( mcp ) ;
      
    }    
  
    // skim finished

    evt->addCollection(  skimVec , _mcpSkimColName ) ;


  }//end MC coll not empty

  
}




void Skim::check( LCEvent * evt ) { 
  streamlog_out(DEBUG4) << " Skim::check event "
			<< evt->getEventNumber() <<std::endl;
}




void Skim::end(){ 
  
  streamlog_out(DEBUG4) << " processed " << _nEvt << " events in " << _nRun << " runs "
  << std::endl ;
  
}










void  Skim::keepMCParticle( MCParticle* mcp, bool save_parents ){
  
  mcp->ext<MCPKeep>() = true;
  

  if (save_parents) {
    const MCParticleVec& parents = mcp->getParents() ;    
    streamlog_out( DEBUG4 ) << " keepMCParticle keep particle with pdg : " << mcp->getPDG() << std::endl ;
  
    for( MCParticleVec::const_iterator pIt = parents.begin() ;
	 pIt != parents.end() ; ++pIt ){
      if(  (*pIt )->ext<MCPKeep>() != true  ) { 
	keepMCParticle(  *pIt ) ;
      }
    }
  }//end save_parents

}


