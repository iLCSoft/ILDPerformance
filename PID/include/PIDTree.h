#ifndef PIDTree_h
#define PIDTree_h 1

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
#include "UTIL/LCTrackerConf.h"
#include <UTIL/BitSet32.h>

// STUFF needed for GEAR
#include <marlin/Global.h>
#include <gear/GEAR.h>
#include <gear/VXDParameters.h>
#include <gear/VXDLayerLayout.h>
#include "gear/BField.h"

#include <TTree.h>

using namespace lcio ;
using namespace marlin ;
using namespace std ;

/** PIDTree processor
 * fills a ROOT tree with mcparticle - reco / PID info
 * which can be used in order to produce hermeticity plots
 * authors: Jenny List
*/

class PIDTree : public Processor {
  
 public:
 

  virtual Processor*  newProcessor() { return new PIDTree ; }
  
  
  PIDTree() ;
  
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
  std::string _trueToTrack ;
  std::string _recoToTrue ;
  std::string _mcParticleCollectionName ;
  std::string _trackColName ;
  std::string _BCALParticleCollectionName ;
  std::string _BCALClusterCollectionName ;
  std::string _pandoraPFOs ;

  int nEvt ;

  float _bField ;


 private:

  // declaration of trees
  TTree *hermTree ;
  int    nMCParticles;
  
  vector<double> trueP;
  vector<double> trueMass;
  vector<double> truePt ;
  vector<double> trueTheta ;
  vector<double> truePhi ;
  vector<double> trueCharge;
  vector<int>    truePDG;
  vector<int>    trueMother;
  
  vector<double> isTrue;  // store max weight of reco -> truth
  vector<double> isSeen;  // store max weight of truth -> reco
  vector<double> isSeenDEdx ;  // store max weight of truth -> track
  vector<double> seenP;
  vector<double> seenPt ;
  vector<double> seenTheta ;
  vector<double> seenPhi ;
  vector<double> seenDEdx ;
  vector<double> seenDEdxErr ;
  vector<double> seenCharge ;
  vector<int> basicPDG;
  vector<int> dEdxPDG;
  vector<int> showerPDG;
  vector<int> likeliPDG;
  vector<int> lowmomPDG;
  
  // dEdz distance for each hypothesis
  vector<float> dedxdist_el;
  vector<float> dedxdist_mu;
  vector<float> dedxdist_pi;
  vector<float> dedxdist_ka;
  vector<float> dedxdist_pr;

  // individual likelihoods for each hypothesis
  vector<float> LiPDG_el;
  vector<float> LiPDG_mu;
  vector<float> LiPDG_pi;
  vector<float> LiPDG_ka;
  vector<float> LiPDG_pr;
  vector<float> dedxPDG_el;
  vector<float> dedxPDG_mu;
  vector<float> dedxPDG_pi;
  vector<float> dedxPDG_ka;
  vector<float> dedxPDG_pr;

} ;


#endif



