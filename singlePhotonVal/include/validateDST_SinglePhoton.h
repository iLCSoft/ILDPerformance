#ifndef validateDST_SinglePhotonProcessor_h
#define validateDST_SinglePhotonProcessor_h 1
#include "marlin/Processor.h"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"

using namespace marlin ;


class validateDST_SinglePhotonProcessor : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new validateDST_SinglePhotonProcessor ; }

  validateDST_SinglePhotonProcessor( const validateDST_SinglePhotonProcessor&) = delete ;
  validateDST_SinglePhotonProcessor& operator=(const validateDST_SinglePhotonProcessor& ) = delete;

  validateDST_SinglePhotonProcessor() ;
  
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
  TFile* _fout = nullptr ;

  TH1F* _h_allMC_en = nullptr ;
  TH1F* _h_allMC_phi = nullptr ;
  TH1F* _h_allMC_costh = nullptr ;
  TH2F* _h_allMC_endcapxy = nullptr ;
  TH2F* _h_mcDecPt = nullptr ;

  std::map < int, std::map < int, TH1F* > > enHistos{};
  std::map < int, std::map < int, TH2F* > > enHistos2d{};

  enum{ivar_npfo=0, ivar_npfo22, ivar_npfoCosth, ivar_enCosth, ivar_totenCosth, ivar_barenPhi, ivar_bartotenPhi, 
       ivar_posendEnX, ivar_posendEnY, ivar_negendEnX, ivar_negendEnY};

};


#endif



