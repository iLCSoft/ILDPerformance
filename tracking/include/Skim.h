#ifndef Skim_h
#define Skim_h 

#include "marlin/Processor.h"

#include <EVENT/MCParticle.h>
#include <IMPL/LCCollectionVec.h>
#include "lcio.h"



using namespace lcio ;


class Skim : public marlin::Processor {
  

public:
  
  virtual Processor*  newProcessor() { return new Skim ; }
  
  
  Skim() ;
  
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
  
  void keepMCParticle( MCParticle* mcp, bool save_parents=true ) ; 


  std::string _mcpColName ;
  std::string _mcpSkimColName;

  int _nRun ;
  int _nEvt ;

  double _thetaMin;
  double _thetaMax;

} ;



#endif
