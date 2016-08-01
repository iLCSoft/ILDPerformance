#ifndef validatePilotProcessor_h
#define validatePilotProcessor_h 1
#include "marlin/Processor.h"

#include "UTIL/CellIDDecoder.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/SimTrackerHit.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/TrackerHit.h"
#include "valStruct.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace marlin ;



class validatePilotProcessor : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new validatePilotProcessor ; }
  
  validatePilotProcessor() ;
  
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

  int _simCols;
  int _recCols;

  std::string _outfile;

  std::map < std::string , std::vector < std::pair < std::string, std::pair<int, int> > > > _indxCode;

  CellIDDecoder <SimCalorimeterHit> * _SimCalorimeterHitDecoder;
  CellIDDecoder <SimTrackerHit> *     _SimTrackerHitDecoder;
  CellIDDecoder <CalorimeterHit> *    _CalorimeterHitDecoder;
  CellIDDecoder <TrackerHit> *        _TrackerHitDecoder;

  std::map < std::string , validatePilotProcessor_maxMin > _allranges;


};


#endif



