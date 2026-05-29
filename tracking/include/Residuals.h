#ifndef Residuals_h
#define Residuals_h 1

#include "EVENT/LCIO.h"
#include "EVENT/Track.h"
#include "lcio.h"
#include "marlin/Processor.h"
#include <EVENT/LCCollection.h>
#include <EVENT/LCFloatVec.h>
#include <EVENT/LCRelation.h>
#include <EVENT/MCParticle.h>
#include <EVENT/ParticleID.h>
#include <EVENT/ReconstructedParticle.h>
#include <EVENT/TrackerHit.h>
#include <EVENT/TrackerHitPlane.h>
#include <EVENT/Vertex.h>
#include <IMPL/TrackStateImpl.h>
#include <UTIL/LCRelationNavigator.h>
#include <marlin/AIDAProcessor.h>
#include <string>
#include <vector>

#include "UTIL/ILDConf.h"
#include "UTIL/LCTrackerConf.h"
#include <TMath.h>

// ----- include for verbosity dependend logging ---------
#include "marlin/Exceptions.h"
#include "marlin/VerbosityLevels.h"
#include <marlin/Global.h>

#include "lcio.h"
#include <TCanvas.h>
#include <TF1.h>
#include <TGraphAsymmErrors.h>
#include <TH1F.h>
#include <TMath.h>
#include <TRotation.h>
#include <TStyle.h>
#include <TTree.h>
#include <TVector3.h>
#include <iostream>
#include <string>
#include <vector>

#include "MarlinTrk/Factory.h"
#include "MarlinTrk/IMarlinTrack.h"
#include "MarlinTrk/IMarlinTrkSystem.h"
#include "MarlinTrk/MarlinTrkUtils.h"

#include "aidaTT/ConstantSolenoidBField.hh"
#include "aidaTT/DD4hepGeometry.hh"
#include "aidaTT/GBLInterface.hh"
#include "aidaTT/IBField.hh"
#include "aidaTT/LCIOPersistency.hh"
#include "aidaTT/Vector3D.hh"
#include "aidaTT/Vector5.hh"
#include "aidaTT/analyticalPropagation.hh"
#include "aidaTT/fitResults.hh"
#include "aidaTT/simplifiedPropagation.hh"
#include "aidaTT/utilities.hh"

#include "DD4hep/DD4hepUnits.h"

using namespace lcio;
using namespace marlin;
using namespace std;
namespace MarlinTrk {
class IMarlinTrkSystem;
}
// using namespace aida;

class Residuals : public Processor {

public:
  virtual Processor* newProcessor() { return new Residuals; }

  Residuals();
  Residuals(const Residuals&) = delete;
  Residuals& operator=(const Residuals&) = delete;

  /** Called at the begin of the job before anything is read.
   * Use to initialize the processor, e.g. book histograms.
   */
  virtual void init();

  /** Called for every run.
   */
  virtual void processRunHeader(LCRunHeader* run);

  /** Called for every event - the working horse.
   */
  virtual void processEvent(LCEvent* evt);

  virtual void check(LCEvent* evt);

  /** Called after data processing for clean up.
   */
  virtual void end();

private:
  /** Input collection name.
   */
  std::string _trks = "";

  MarlinTrk::IMarlinTrkSystem* _trkSystem = NULL;

  TTree* ResiduTree = NULL;
  vector<double> TrackHitResidualsU = {};
  vector<double> TrackHitResidualsV = {};
  vector<int> DetLayer = {};
  vector<int> SubDet = {};

  int _nRun = 0;
  int _nEvt = 0;
  bool _MSOn = false;
  bool _ElossOn = false;
  bool _SmoothOn = false;
  bool _sortRad = false;
  float _bField = 0.0;

  std::map<long, const aidaTT::ISurface*> surfMap = {};
  std::string _trkSystemName = "";
};

#endif
