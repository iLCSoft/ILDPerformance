#ifndef DDDiagnostics_h
#define DDDiagnostics_h 1

#include <marlin/Processor.h>
#include <marlin/Global.h>
#include <marlinutil/HelixClass.h>

#include <EVENT/LCCollection.h>
#include <EVENT/TrackerHit.h>
#include <EVENT/SimTrackerHit.h>
#include <EVENT/Track.h>
#include <EVENT/LCRelation.h>
#include <EVENT/MCParticle.h>
#include "UTIL/LCRelationNavigator.h"
#include "UTIL/LCIterator.h"
#include "UTIL/Operators.h"
#include <UTIL/BitField64.h>
#include "UTIL/LCTrackerConf.h"
#include <UTIL/ILDConf.h>
#include <UTIL/BitSet32.h>

#include "DD4hep/Detector.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/DetType.h"
#include "DDRec/DetectorData.h"
#include "DD4hep/DetectorSelector.h"

#include "TROOT.h"
#include <TTree.h>
#include <TVector3.h>
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
#include <algorithm>

using namespace lcio ;
using namespace marlin ;
using namespace std ;

/** DDDiagnostics processor
 * feels a ROOT tree with track - mcparticle info
 * which can be used in order to produce resolution plots
 * creates canvases with finding efficiency - pulls
 * authors: Yorgos Voutsinas, Alonso Guerrero Llorente
 * update: to get DD4hep geometry information
*/

class DDDiagnostics : public Processor {
  
 public:
 

  virtual Processor*  newProcessor() { return new DDDiagnostics ; }
  
  
  DDDiagnostics() ;
  
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

  /** Initialise the ROOT TH1F and so on ...
   */
  void initHist(void) ;

  /** Finalise the ROOT, and fill the TCanvas.
   */
  void fillCanvas(void) ;

  /** Initialise the all the vectors.
   */
  void clearVec(void) ;

  /** Initialise the all the parameters for marlin.
   */
  void initParameters(void) ;

 protected:

  /** Input collection name.
   */
  std::string _trueToReco ;
  std::string _recoToTrue ;
  std::string _mcParticleCollectionName ;
  StringVec   _simTrkHitCollectionNames ;
  std::string _trackColName ;
  std::string _sitSpacePoints ;
  std::string _sitTrkHitRelations ;
  std::string _vxdTrackerHits ;
  std::string _vxdTrkHitRelations ;
  std::string _BCALParticleCollectionName ;
  std::string _BCALClusterCollectionName ;
  std::string _pandoraPFOs ;

  int nEvt ;
  int OutSitBkgHit ;
  int OutSitPhysHit ;
  int OutVxdBkgHit ;
  int OutVxdPhysHit ;

  int ghostCounter ;

  float _bField ;

  bool _trkEffOn ; 
  bool _siTrkEffOn ;
  bool _physSampleOn ;
  double _pCut ;
  double _ptCut ;
  double _originCut ;
  double _cosTheta ;
  double _minPurity ;
  int _minSiHits ;
  bool _reqInnVXDHit ;
  bool _fillBigTTree ;

 private:

  // declaration of trees
  TTree *EvalTree ;
  vector<int> foundTrk;
  vector<int> TrackSiHits;
  vector<int> SiHitsSiTrk ;
  vector<int> MarlinTrkHits ;
  vector<int> VXDHits ;
  vector<int> SITHits ;
  vector<int> FTDHits ;
  vector<int> TPCHits ;
  vector<float> foundTrkChi2OverNdof ;
  vector<float> CluChi2OverNdof ;
  vector<float> SiTrkChi2OverNdof ;
  vector<float> PtReco;
  vector<float> CosThetaReco;
  vector<float> SiTrksPt;
  vector<float> PtMCP;
  vector<float> CosThetaMCP;
  vector<float> Wgt;
  vector<float> InvWgt;
  vector<float> trueD0;
  vector<float> trueZ0;
  vector<float> trueOmega;
  vector<float> truePhi;
  vector<float> trueTanLambda;
  vector<float> recoD0;
  vector<float> recoZ0;
  vector<float> recoD0Error;
  vector<float> recoZ0Error;
  vector<float> recoOmega;
  vector<float> recoPhi;
  vector<float> recoTanLambda;
  vector<float> recoD0error;
  vector<float> recoZ0error;
  vector<float> recoOmegaError;
  vector<float> recoPhiError;
  vector<float> recoTanLambdaError;
  vector<float> siTrksCosTheta;

  TTree *GhostTree ;
  vector<float> ghostPt;
  vector<float> ghostCosTheta;
  vector<float> ghostTrkChi2OverNdof ;
  vector<float> ghostWgt;
  vector<int>   ghost_hits;
  vector<int>   generatorStatus;
  vector<int>   DecayedInTracker;
  vector<float> doubleCountingPt;
  vector<float> doubleCountingCosTheta;
  vector<float> BadTrksD0 ;
  vector<float> BadTrksZ0 ;


  int MarlinTracks;
  int SeedTracks;
  int SiliconTracks;
  int BCalParts;
  int BCalCls;
  int pfos ;

  TH1F *OmegaPull; 
  TH1F *PhiPull; 
  TH1F *TanLambdaPull; 
  TH1F *d0pull; 
  TH1F *z0pull; 

  TH1F *OmegaResidual; 
  TH1F *PhiResidual; 
  TH1F *TanLambdaResidual; 
  TH1F *d0Residual; 
  TH1F *z0Residual; 

  static const int nBins = 13 ;

  TH1F *hist_pt_t ;
  TH1F *hist_pt_f ;
  TH1F *hist_p_t ;
  TH1F *hist_p_f ;
  TH1F *hist_th_t ;
  TH1F *hist_th_f ;
  TH1F *hist_thm_t ;
  TH1F *hist_thm_f ;

  TH1F *hist_pt_allMarTrk ;
  TH1F *hist_pt_fakeMarTrk ;
  TH1F *hist_p_allMarTrk ;
  TH1F *hist_p_fakeMarTrk ;
  TH1F *hist_th_allMarTrk ;
  TH1F *hist_th_fakeMarTrk ;
  TH1F *hist_thm_allMarTrk ;
  TH1F *hist_thm_fakeMarTrk ;

  TCanvas *pulls;
  TCanvas *residuals;
  TCanvas *eff;
  TCanvas *Rfake;
  TCanvas *effPM;
  TCanvas *RfakePM;

  TF1 *myfunc ;

  TGraphAsymmErrors *gpt;
  TGraphAsymmErrors *gp;
  TGraphAsymmErrors *gth;
  TGraphAsymmErrors *gthm;

  TGraphAsymmErrors *gptfake;
  TGraphAsymmErrors *gpfake;
  TGraphAsymmErrors *gthfake;
  TGraphAsymmErrors *gthmfake;

  double PI;
  double TWOPI;

} ;


#endif
