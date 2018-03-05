#ifndef StepLength_h
#define StepLength_h 1

#include <marlin/Processor.h>
#include <marlin/Global.h>

#include <EVENT/LCCollection.h>
#include <EVENT/SimCalorimeterHit.h>

#include "TROOT.h"
#include <TTree.h>
#include <vector>
#include "lcio.h"
#include <string>
#include <iostream>
#include <algorithm>

using namespace lcio ;
using namespace marlin ;
using namespace std ;

/** StepLength processor
*/

class StepLength : public Processor {
  
 public:
 

  virtual Processor*  newProcessor() { return new StepLength ; }
  
  
  StepLength() ;
  StepLength(const StepLength&) = delete;
  StepLength& operator=(const StepLength&) = delete;

  
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

  /** Initialise the ROOT TTRee.
   */
  void initTree(void) ;

  /** Initialise the all the vectors.
   */
  void clearVec(void) ;

  /** Initialise the all the parameters for marlin.
   */
  void initParameters(void) ;

 protected:

  /** Input collection name.
   */
  StringVec   _simCaloHitCollectionNames = {};

  int nEvt = 0;

 private:

  TTree *StepTree = NULL;
  vector<int> PDG  = {};
  vector<float> stepLength  = {};

} ;


#endif
