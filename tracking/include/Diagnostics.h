#ifndef Diagnostics_h
#define Diagnostics_h 1

#include <marlin/Processor.h>

#include <EVENT/LCCollection.h>
#include <EVENT/TrackerHit.h>
#include <EVENT/SimTrackerHit.h>
#include <EVENT/Track.h>
#include <EVENT/LCRelation.h>
#include <EVENT/MCParticle.h>
//#include "IMPL/TrackerHitImpl.h"
#include <IMPL/LCCollectionVec.h>
#include "UTIL/LCRelationNavigator.h"
#include "UTIL/LCIterator.h"
#include "UTIL/Operators.h"
#include <UTIL/BitField64.h>
#include <UTIL/ILDConf.h>
#include <UTIL/BitSet32.h>

// STUFF needed for GEAR
#include <marlin/Global.h>
#include <gear/GEAR.h>
#include <gear/VXDParameters.h>
#include <gear/VXDLayerLayout.h>
#include "gear/BField.h"

#include <marlinutil/HelixClass.h>

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
#include <algorithm>

using namespace lcio ;
using namespace marlin ;
using namespace std ;

/** Diagnostics processor
 * feels a ROOT tree with track - mcparticle info
 * which can be used in order to produce resolution plots
 * creates canvases with finding efficiency - pulls
 * authors: Yorgos Voutsinas, Alonso Guerrero Llorente
*/

class Diagnostics : public Processor {
  
 public:
 

  virtual Processor*  newProcessor() { return new Diagnostics ; }
  
  
  Diagnostics() ;
  
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

 private:

  // declaration of trees
  TTree *EvalTree ;
  vector<int> foundTrk;
  vector<int> TrackSiHits;
  vector<int> SiHitsSiTrk ;
  vector<int> MarlinTrkHits ;
  vector<float> BadTrksD0 ;
  vector<float> BadTrksZ0 ;
  vector<int> VXDHits ;
  vector<int> SITHits ;
  vector<int> ghost_hits;
  vector<float> foundTrkChi2OverNdof ;
  vector<float> ghostTrkChi2OverNdof ;
  vector<float> CluChi2OverNdof ;
  vector<float> SiTrkChi2OverNdof ;
  vector<float> PtReco;
  vector<float> GhostsPt;
  vector<float> SiTrksPt;
  vector<float> PtMCP;
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
  vector<float> ghostCosTheta;
  vector<float> siTrksCosTheta;


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

  TCanvas *pulls;
  TCanvas *residuals;
  TCanvas *eff;

  TF1 *myfunc ;

  TGraphAsymmErrors *gpt;


} ;


#endif



