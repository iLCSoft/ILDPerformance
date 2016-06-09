#ifndef Residuals_h
#define Residuals_h 1

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
#include "EVENT/Track.h"
#include <EVENT/MCParticle.h>
#include <EVENT/Vertex.h>
#include <EVENT/LCRelation.h>
#include <EVENT/TrackerHit.h>
#include <EVENT/TrackerHitPlane.h>
#include <UTIL/LCRelationNavigator.h>
#include <IMPL/TrackStateImpl.h>

#include "UTIL/ILDConf.h"
#include <TMath.h>

// ----- include for verbosity dependend logging ---------
#include "marlin/VerbosityLevels.h"
#include "marlin/Exceptions.h"
#include <marlin/Global.h>

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

#include "MarlinTrk/MarlinAidaTTTrack.h"
#include "MarlinTrk/MarlinKalTestTrack.h"
#include "MarlinTrk/MarlinAidaTT.h"
#include "MarlinTrk/IMarlinTrkSystem.h"
#include "MarlinTrk/IMarlinTrack.h"
#include "MarlinTrk/Factory.h"
#include "MarlinTrk/MarlinTrkUtils.h"


#include "aidaTT/IBField.hh"
#include "aidaTT/ConstantSolenoidBField.hh"
#include "aidaTT/analyticalPropagation.hh"
#include "aidaTT/simplifiedPropagation.hh"
#include "aidaTT/GBLInterface.hh"
#include "aidaTT/fitResults.hh"
#include "aidaTT/Vector5.hh"
#include "aidaTT/utilities.hh"
#include "aidaTT/LCIOPersistency.hh"
#include "aidaTT/Vector3D.hh"
#include "aidaTT/DD4hepGeometry.hh"

#include "DD4hep/DD4hepUnits.h"


using namespace lcio ;
using namespace marlin ;
using namespace std;
namespace MarlinTrk{
  class IMarlinTrkSystem ;
}
//using namespace aida;

class Residuals : public Processor {
  
 public:



  virtual Processor*  newProcessor() { return new Residuals ; }
  
  
  Residuals() ;
  
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
  
  private:

  /** Input collection name.
   */
  std::string _trks ;

  MarlinTrk::IMarlinTrkSystem* _trkSystem;

  TTree *ResiduTree;
  vector<double> TrackHitResidualsU ;
  vector<double> TrackHitResidualsV ;
  vector<int> DetLayer ;
  vector<int> SubDet ;

  int _nRun ;
  int _nEvt ;
  bool _MSOn, _ElossOn, _SmoothOn, _sortRad ;
  float _bField;

  std::map< long, const aidaTT::ISurface* > surfMap ;
  std::string _trkSystemName ;

} ;

#endif



