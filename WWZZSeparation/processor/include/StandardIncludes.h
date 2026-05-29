#include "lcio.h"
#include "marlin/Processor.h"
#include <fstream>
#include <iostream>
#include <set>

#include <algorithm> // for std::find
#include <iterator>  // for std::begin, std::end
#include <string>
#include <vector>

// ROOT
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"

// LCIO
#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>
#include <EVENT/ReconstructedParticle.h>

//----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"

using namespace lcio;
using namespace marlin;
