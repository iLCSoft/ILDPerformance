#ifndef validateAutoProcessor_h
#define validateAutoProcessor_h 1
#include "marlin/Processor.h"
#include "valStruct.h"
#include "UTIL/CellIDDecoder.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/SimTrackerHit.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/TrackerHit.h"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace marlin ;


class validateAutoProcessor : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new validateAutoProcessor ; }
  
  validateAutoProcessor(const validateAutoProcessor&) = delete ;
  validateAutoProcessor& operator=(const validateAutoProcessor& ) = delete ;

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

  StringVec _infiles{};
  std::string _outfile{};

  float _maxR{};
  float _maxZ{};
  float _minZ{};
  float _maxE{};
  bool  _isEndcap{};

  std::vector < std::string > cellindices{};

  CellIDDecoder <SimCalorimeterHit> * _SimCalorimeterHitDecoder{};
  CellIDDecoder <SimTrackerHit>     * _SimTrackerHitDecoder{};
  CellIDDecoder <CalorimeterHit>    * _CalorimeterHitDecoder{};
  CellIDDecoder <TrackerHit>        * _TrackerHitDecoder{};

  std::map < std::string , validatePilotProcessor_maxMin > _allranges{};


  TFile* _fout{};

  TH2F* _hAll_overallposZR{};
  TH2F* _hAll_Log_overallposZR{};
  std::map < std::string, TH2F* > _h_overallposZR{};
  std::map < std::string, TH2F* > _h_Log_overallposZR{};

  std::map < std::string, TH1F* > _h_HitEn{};
  std::map < std::string, TH1F* > _h_HitTime{};
  std::map < std::string, std::pair < TH2F* , TH2F* > > _h_posXY{};
  std::map < std::string, std::pair < TH2F* , TH2F* > > _h_posZR{};

  std::map < std::string, std::vector < std::pair < TH2F* , TH2F* > > > _h_index_posX{};
  std::map < std::string, std::vector < std::pair < TH2F* , TH2F* > > > _h_index_posY{};
  std::map < std::string, std::vector < std::pair < TH2F* , TH2F* > > > _h_index_posZ{};
  std::map < std::string, std::vector < std::pair < TH2F* , TH2F* > > > _h_index_posR{};
  std::map < std::string, std::vector < std::pair < TH2F* , TH2F* > > > _h_index_posPhi{};


};


#endif



