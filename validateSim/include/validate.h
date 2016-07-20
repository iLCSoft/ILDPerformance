#ifndef validateProcessor_h
#define validateProcessor_h 1
#include "marlin/Processor.h"

#include "UTIL/CellIDDecoder.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/SimTrackerHit.h"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace marlin ;


class validateProcessor : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new validateProcessor ; }
  
  validateProcessor() ;
  
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

  std::string _inColl;
  float _maxR;
  float _maxZ;
  float _minZ;
  float _maxE;
  bool  _isEndcap;

  std::map < std::string, std::pair<int, int> > _indxCode;
  
  CellIDDecoder <SimCalorimeterHit> * _SimCalorimeterHitDecoder;
  CellIDDecoder <SimTrackerHit> * _SimTrackerHitDecoder;

  void fillSpecificHistos() {}


  TFile* _fout;

  TH1F* _hSim_HitEn;
  TH1F* _hSim_HitTime;
  TH2F* _hSim_posXY[2];
  TH2F* _hSim_posXZ[2];
  TH2F* _hSim_posYZ[2];
  TH2F* _hSim_posRZ[2];
  std::vector < TH2F* >  _hSim_posX_index[2];
  std::vector < TH2F* >  _hSim_posY_index[2];
  std::vector < TH2F* >  _hSim_posZ_index[2];
  std::vector < TH2F* >  _hSim_posR_index[2];
  std::vector < TH2F* >_hSim_posPhi_index[2];


};


#endif



