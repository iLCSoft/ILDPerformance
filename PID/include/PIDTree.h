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


#include <TTree.h>


/** PIDTree processor
 * fills a ROOT tree with mcparticle - reco / PID info
 * which can be used in order to produce hermeticity plots
 * authors: Jenny List
*/

class PIDTree : public marlin::Processor {
  
 public:
 
  PIDTree(const PIDTree&) = delete ;
  PIDTree& operator=(const PIDTree&) = delete ; 

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
  std::string _trueToReco {};
  std::string _trueToTrack {};
  std::string _recoToTrue {};
  std::string _mcParticleCollectionName {};
  std::string _trackColName {};
  std::string _BCALParticleCollectionName {};
  std::string _BCALClusterCollectionName {};
  std::string _pandoraPFOs {};

  int nEvt = 0;

  float _bField = 0.;


 private:

  // declaration of trees
  TTree *hermTree = nullptr ;
  int    nMCParticles = 0;
  
  std::vector<double> trueP{};
  std::vector<double> trueMass{};
  std::vector<double> truePt {};
  std::vector<double> trueTheta {};
  std::vector<double> truePhi {};
  std::vector<double> trueCharge{};
  std::vector<int>    truePDG{};
  std::vector<int>    trueMother{};
  std::vector<double> isTrue{};  // store max weight of reco -> truth
  std::vector<double> isSeen{};  // store max weight of truth -> reco
  std::vector<double> isSeenDEdx {};  // store max weight of truth -> track
  std::vector<double> seenP{};
  std::vector<double> seenPt {};
  std::vector<double> seenTheta {};
  std::vector<double> seenPhi {};
  std::vector<double> seenDEdx {};
  std::vector<double> seenDEdxErr {};
  std::vector<double> seenCharge {};
  std::vector<int> basicPDG{};
  std::vector<int> dEdxPDG{};
  std::vector<int> showerPDG{};
  std::vector<int> likeliPDG{};
  std::vector<int> lowmomPDG{};

  std::vector<float> dedxdist_el{};
  std::vector<float> dedxdist_mu{};
  std::vector<float> dedxdist_pi{};
  std::vector<float> dedxdist_ka{};
  std::vector<float> dedxdist_pr{};

  // individual likelihoods for each hypothesis
  std::vector<float> LiPDG_el{};
  std::vector<float> LiPDG_mu{};
  std::vector<float> LiPDG_pi{};
  std::vector<float> LiPDG_ka{};
  std::vector<float> LiPDG_pr{};
  std::vector<float> dedxPDG_el{};
  std::vector<float> dedxPDG_mu{};
  std::vector<float> dedxPDG_pi{};
  std::vector<float> dedxPDG_ka{};
  std::vector<float> dedxPDG_pr{};

} ;


#endif



