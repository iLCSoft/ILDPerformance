#ifndef validateAutoProcessor_h
#define validateAutoProcessor_h 1
#include "marlin/Processor.h"
#include "valStruct.h"
#include "UTIL/CellIDDecoder.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/SimTrackerHit.h"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace marlin ;


class validateAutoProcessor : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new validateAutoProcessor ; }
  
  validateAutoProcessor() ;
  
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

  std::string _infile;


  std::string _inColl;
  float _maxR;
  float _maxZ;
  float _minZ;
  float _maxE;
  bool  _isEndcap;

  std::vector < std::string > cellindices;

  //  std::map < std::string, std::pair<int, int> > _indxCode;

  CellIDDecoder <SimCalorimeterHit> * _SimCalorimeterHitDecoder;
  CellIDDecoder <SimTrackerHit>     * _SimTrackerHitDecoder;

  void fillSpecificHistos() {}

  std::map < std::string , validatePilotProcessor_maxMin > allranges;


  TFile* _fout;

  TH2F* _hAll_overallposZR;
  TH2F* _hAll_Log_overallposZR;
  std::map < std::string, TH2F* > _h_overallposZR;
  std::map < std::string, TH2F* > _h_Log_overallposZR;


  std::map < std::string, TH1F* > _hSim_HitEn;
  std::map < std::string, TH1F* > _hSim_HitTime;
  std::map < std::string, std::pair < TH2F* , TH2F* > > _hSim_posXY;
  //  std::map < std::string, std::pair < TH2F* , TH2F* > > _hSim_posZX;
  //  std::map < std::string, std::pair < TH2F* , TH2F* > > _hSim_posZY;
  std::map < std::string, std::pair < TH2F* , TH2F* > > _hSim_posZR;

  std::map < std::string, std::vector < std::pair < TH2F* , TH2F* > > > _hSim_index_posX;
  std::map < std::string, std::vector < std::pair < TH2F* , TH2F* > > > _hSim_index_posY;
  std::map < std::string, std::vector < std::pair < TH2F* , TH2F* > > > _hSim_index_posZ;
  std::map < std::string, std::vector < std::pair < TH2F* , TH2F* > > > _hSim_index_posR;
  std::map < std::string, std::vector < std::pair < TH2F* , TH2F* > > > _hSim_index_posPhi;


};


#endif



