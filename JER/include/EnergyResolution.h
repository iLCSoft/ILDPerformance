#ifndef EnergyResolution_h
#define EnergyResolution_h 1

#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <vector>
#include <marlin/AIDAProcessor.h>
#include "EVENT/LCIO.h"
#include <EVENT/LCCollection.h>
#include <EVENT/ReconstructedParticle.h>
#include <EVENT/ParticleID.h>
#include <EVENT/LCFloatVec.h>
#include <EVENT/LCRelation.h>
#include <UTIL/LCRelationNavigator.h>
#include "EVENT/Track.h"
#include <EVENT/MCParticle.h>
#include <EVENT/Vertex.h>
#include <EVENT/LCRelation.h>

#include <TMath.h>

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"
#include "marlin/Exceptions.h"

#include <TTree.h>
#include <TMath.h>
#include <TH1F.h>
#include <TF1.h>
#include <TGraphAsymmErrors.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <vector>
#include "lcio.h"
#include <string>
#include <iostream>
#include <TVector3.h>
#include <TRotation.h>

using namespace lcio ;
using namespace marlin ;
using namespace std;
//using namespace aida;

class EnergyResolution : public Processor {
  
 public:



  virtual Processor*  newProcessor() { return new EnergyResolution ; }
  
  
  EnergyResolution() ;
  
  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init() ;
  
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
  std::string _pandoraPFOs ;
  std::string _mcparticles ;
  std::string _recoToTrue ;
  std::string _jets ;


  int _nRun ;
  int _nEvt ;

  TTree *t1;
  int PFOs ;
  std::vector <double> recoEnergy ; 

  TH1F *deltaE;

} ;

#endif



