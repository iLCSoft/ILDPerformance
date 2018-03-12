#include "marlin/Processor.h"
#include "lcio.h"
#include <iostream>
#include <fstream>
#include <set>

#include <string>
#include <vector>
#include <algorithm> // for std::find
#include <iterator> // for std::begin, std::end

//ROOT
#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"

//LCIO
#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h> 
#include <EVENT/ReconstructedParticle.h>

//----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"

using namespace lcio ;
using namespace marlin ;

