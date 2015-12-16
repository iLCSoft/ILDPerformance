#ifndef Pi0Tree_h
#define Pi0Tree_h 1

#include <marlin/Processor.h>

#include <EVENT/LCCollection.h>
#include <EVENT/Track.h>
#include <EVENT/LCRelation.h>
#include <EVENT/MCParticle.h>
#include <IMPL/LCCollectionVec.h>
#include "UTIL/LCRelationNavigator.h"
#include "UTIL/LCIterator.h"
#include "UTIL/Operators.h"
#include <UTIL/BitField64.h>
#include <UTIL/ILDConf.h>
#include <UTIL/BitSet32.h>

// STUFF needed for GEAR
#include <marlin/Global.h>
#include <gear/GEAR.h>
#include <gear/VXDParameters.h>
#include <gear/VXDLayerLayout.h>
#include "gear/BField.h"

#include <TTree.h>

#include <iomanip>

using namespace lcio ;
using namespace marlin ;
using namespace std ;

/** Pi0Tree processor
 * checks correctness of found pi0
 * authors: Jenny List
*/

class Pi0Tree : public Processor {
  
 public:
 

  virtual Processor*  newProcessor() { return new Pi0Tree ; }
  
  
  Pi0Tree() ;
  
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init();

  /** Called for every run.
   */
  virtual void processRunHeader( LCRunHeader* run ) ;
  
  /** Called for every event - the working horse.
   */
  virtual void processEvent( LCEvent * evt ) ; 
  
  
  virtual void check( LCEvent * evt ) ; 
  
  
  /** Called after data processing for clean up.
   */
  virtual void end() ;

  


 protected:

  /** Input collection name.
   */
  std::string _trueToReco ;
  std::string _recoToTrue ;
  std::string _mcParticleCollectionName ;
  std::string _gammaGammaParticleCollectionName ;

  int nEvt ;

  float _bField ;


 private:

  // declaration of trees
  TTree *pi0Tree ;
  int    nMCPi0;
    
  vector<double> trueE;
  vector<double> trueP;
  vector<double> truePt ;
  vector<double> trueTheta ;
  vector<double> truePhi ;
  vector<int>    truePDG;
  vector<int>    trueMother;
  vector<double> daughtersSeen;  // found photons?  
  vector<double> isSeen;  // both photons point to PFOs ?
  vector<double> isSeenAsPhotons;  // both photons point to photon PFOs?
  vector<double> weightToPhotons;  // sum of weigths

  int    nRecoPi0;
  vector<int> nTruePhotons;      
  vector<int> nTrueMeson;      
  vector<int> isTrue;        // 1= one photon from true meson 2= both photons from true meson
  vector<double> recoE;         // energy of gammagamma candidate (after constrained fit)
  vector<double> pfoE;          // energy of the two photons
  vector<double> trueEofSeen;    // energy of the true meson
  vector<double> recoMass;
  vector<double> pfoMass;
  vector<double> trueMassofSeen;
  vector<double> truePDGofSeen;
  
  
} ;


#endif



