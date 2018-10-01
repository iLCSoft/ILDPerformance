#include "WWZZProcessor.h"

/* Functions that read in LCCollections and return
  vector containing the Particles in their corresponding
  LCIO format */

std::vector<MCParticle*> WWZZProcessor::checkMCFinalState(LCCollection* AllMC) {
  std::vector<MCParticle*> FS_all;
  int nMC = AllMC->getNumberOfElements();
  for( int i = 0; i < nMC; i++ ){
    MCParticle* MC = dynamic_cast< MCParticle* >( AllMC->getElementAt(i) );
    FS_all.push_back(MC);
  }
  return(FS_all);
}


std::vector<ReconstructedParticle*> WWZZProcessor::checkPOFinalState(LCCollection* AllPOs) {
  std::vector<ReconstructedParticle*> FS;
  int npo;
  try{
    npo = AllPOs->getNumberOfElements();
  } catch(...) {
    npo = 0;
  }
  if ( npo != 0 ){
    for( int i = 0; i < npo; i++ ){
      ReconstructedParticle* po= dynamic_cast< ReconstructedParticle* >( AllPOs->getElementAt(i) );
      FS.push_back(po);
    }
  }
  return(FS);
}
