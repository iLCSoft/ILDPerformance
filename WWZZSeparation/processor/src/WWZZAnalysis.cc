#include "WWZZProcessor.h"

void WWZZProcessor::analyseEvent(	
    LCCollection* colMC, LCCollection* colPFOs, LCCollection* colJets, 
    LCCollection* colIsoleps, EventInfo &info
    ) {

  // Read out the particle from the Collections into C++ vectors
  std::vector<MCParticle*> MCparticles=checkMCFinalState(colMC);
  std::vector<ReconstructedParticle*> PFOs=checkPOFinalState(colPFOs);
  std::vector<ReconstructedParticle*> jets=checkPOFinalState(colJets);
  std::vector<ReconstructedParticle*> isoleps=checkPOFinalState(colIsoleps);

  // Categorize Event on generator level
  findTrueEventType(MCparticles, info);

  // Calculate Parameters that I later make a cut on,
  // y_34 has to be done separate because its a parameter of the collection	
  info.observ.y_34= colJets->getParameters().getFloatVal("y_{n-1,n}");
  calculateCutParameters( PFOs, jets, info.observ );

  // nunu+4q FS does not contain isolated leptons
  info.observ.num_isolated_leptons = isoleps.size();
}       
