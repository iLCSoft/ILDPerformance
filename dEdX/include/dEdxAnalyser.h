#ifndef dEdxAnalyser_H
#define dEdxAnalyser_H 1

#include <marlin/MarlinConfig.h>
#include "marlin/Processor.h"
#include "lcio.h"
#include <string>
#include <vector>
#include <array>
#include "UTIL/ILDConf.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TMath.h"
#include "TF1.h"
#include "TF2.h"
#include "TCanvas.h"
#include "TImage.h"

#ifdef MARLIN_AIDA //AIDA
#include <marlin/AIDAProcessor.h>
#endif

using namespace lcio;
using namespace marlin;


/** dEdxAnalyser Processor <br>
 *  The dEdxAnalyser processor loops through tracks to collect their dE/dx, fills various histograms and calculates dE/dx resolution and separation power.
 *  Everything is stored in an AIDA-generated root-file.
 *  Various plots can be automatically produced in the current working directory.
 *
 *  The required collections are Tracks, a TrackMCTruthLink and the corresponding MCParticles.
 *  If tracks attached to PFOs should be used, also a PFO collection is required.
 *  The number of particle species to be investigated and their properties are hard-coded and must be set:
 *  _nPart is the number of species, set in this header, while the PDG number and the names of the particles (for labelling the histograms) are set in the init().
 *  default: _nPart = 5; species: electrons, muons, pions, kaon, protons.
 *
 *  First, in processEvent(), the dE/dx values of each track is filled in the corresponding histogram, resulting in Bethe-Bloch curves.
 *  The correct PDG is received from the MCParticle linked to the track.
 *  In end(), these histograms are then fitted with FitSlices, and the resulting mean and sigma values for each momentum bin are used to calculate resolution and separation power.
 *  The separation power is calculated for each combination of the _nPart species.
 *  All histograms, including the auto-generated fit results, are stored via AIDA in a root file.
 *  Finally, many of the histograms can be plotted and saved as images to the current working directory.
 *
 *  @param _plotStuff - Set to true to switch on automatic image generation of many histograms in the current working directory.
 *    bool, default: false.
 *  @param _fileFormat - Select the file extension to be used for the automatically generated images.
 *    See https://root.cern.ch/doc/master/classTPad.html#ad2fc5f449e4cb5480b9fb05fda3a8307 for options.
 *    string, default: .png
 *
 *  @param _usePFOTracks - Set to true to use tracks attached to charged PFO, instead of all tracks in the track collection. This should be used with events which are more busy than single particle files.
 *    bool, default: false.
 *
 *  You can make a track selection via the optional parameters, e. g. to reduce 'contamination' from mis-linked Tracks and MCParticles.
 *  @param _useOneTrack - Set true if from every event only the first track should be used.
 *    bool, default: false.
 *  @param _cutdEdx - Set true if particles with a very off dE/dx value (hard-coded) should be ignored.
 *    bool, default: false.
 *  @param _cutTrackPurity - Set true if particles should be ignored, where the MCParticle is connected to more than one track.
 *    bool, default: false.
 *  @param _cutD0 - Tracks with a d0 larger than the given value should be ignored. Set to 0 to accept all particles.
 *    double, default: 0.
 *  @param _cutZ0 - Tracks with a z0 larger than the given value should be ignored. Set to 0 to accept all particles.
 *    double, default: 0.
 *  @param _cutMomMin - Tracks with a momentum smaller than the given value should be ignored. Set to 0 to accept all particles.
 *    double, default: 0.
 *  @param _cutMomMax - Tracks with a momentum larger than the given value should be ignored. Set to 0 to accept all particles.
 *    double, default: 0.
 *
 *  For calibration with beam test results, so-called fiducial electrons are selected, which ought to be similar to beam-test conditions, and their properties stored in separate histograms.
 *  @param _fidMomMin - Fiducial minimum absolut momentum / GeV.
 *    double, default: 3.
 *  @param _fidMomMax - Fiducial maximum absolut momentum / GeV.
 *    double, default: 10.
 *  @param _fidLamMin - Fiducial minimum absolut lambda / deg.
 *    double, default: 1.
 *  @param _fidLamMax - Fiducial maximum absolut lambda / deg.
 *    double, default: 10.
 *  @param _fidNHitsMin - Fiducial minimum number of hits.
 *    int, default: 200.
 *
 *  For some of the histograms, the axis binning can be adapted, which can be useful in case of very high or low statistics.
 *  @param _nBinsX - Number of bins of the momentum axis in histograms.
 *    int, default: 100.
 *  @param _nBinsY - Number of bins of the dE/dx axis in histograms.
 *    int, default: 300.
 *  @param _minBinX - log_10(momentum/GeV) of the lower end of the momentum axis in histograms.
 *    double, default: -1 (= 0.1 GeV).
 *  @param _maxBinX - log_10(momentum/GeV) of the upper end of the momentum axis in histograms.
 *    double, default: 2 (= 100 GeV).
 *  @param _minBinY - Lower end of the dE/dx axis in GeV/mm in histograms.
 *    double, default: 0.
 *  @param _maxBinY - Upper end of the dE/dx axis in GeV/mm in histograms.
 *    double, default: 1e-6.
 *
 *  You can run this Analyser over any selection of tracks, e. g. only pion and kaon tracks.
 *  In that case, only the pion and kaon resolution as well as only the pion-kaon separation would give sensible plots, but all other plots would be generated empty alongside.
 *  The Analyser has mostly been used to extract the dE/dx performance from single-particle random-momentum files, but also to check e. g. 6f-ttbar events.
 *
 *  Note that the hit energy histograms will only be filled, if the track hits are available, which is usually only the case for REC-files.
 *  The hit number histograms are unaffected by this.
 *
 *  @author U. Einhaus, DESY
 *  @version $1.1$
 */


class dEdxAnalyser : public Processor {
  
 public:
  
  virtual Processor*  newProcessor() { return new dEdxAnalyser ; }
  
  
  dEdxAnalyser();

  virtual ~dEdxAnalyser() = default;

  dEdxAnalyser(const dEdxAnalyser&) = delete;

  dEdxAnalyser& operator=(const dEdxAnalyser&) = delete;

  
  virtual void init();
  
  virtual void processRunHeader( LCRunHeader* run );
  
  virtual void processEvent( LCEvent * evt );
  
  virtual void check( LCEvent * evt );
  
  virtual void end();

  virtual void PlotTH1( TCanvas* can, TH1* hist );
  virtual void PlotTH2( TCanvas* can, TH2* hist );


 protected:

  std::string _description = "";
  int _nRun{};
  int _nEvt{};

  // geometry
  float _TPC_inner = 0;
  float _TPC_outer = 0;
  double _bField{};
  std::vector<double> _MCmomVec{};

  // collection names
  std::string _trackColName{};
  std::string _tracktruthLinkName{};
  std::string _MCParColName{};
  std::string _PFOColName{};

  // optional parameters: binning, track selection, fiducial parameters and plotting
  int _nBinsX=0, _nBinsY=0;
  double _minBinX=0, _maxBinX=0, _minBinX2=0;
  double _minBinY=0, _maxBinY=0;

  bool _usePFOTracks=false;
  bool _useOneTrack=false;
  bool _useHitCol=false;
  bool _cutdEdx=false;
  bool _cutTrackPurity=false;
  bool _cutTrackPurityMom=false;
  double _cutD0=0;
  double _cutZ0=0;
  double _cutMomMin=0;
  double _cutMomMax=0;

  double _fidMomMin=0;
  double _fidMomMax=0;
  double _fidLamMin=0;
  double _fidLamMax=0;
  int _fidNHitsMin=0;

  bool _plotStuff=false;
  bool _useLCTPCStyle=false;
  std::string _fileFormat = ".png";

  // number of particle species to be separated
  static const int _nPart = 5;

  // pdg numbers and masses of particles, to be specified in ::init() (along with corresponding strings for names and descriptions)
  std::vector<int> _PDG{};
  std::vector<double> _Masses{};

  // Bethe-Bloch histograms (including one for "other particles" and one for "all particles")
  TH2* _BBHist[_nPart+2];

  // Gaussian fit results: each TObjArray has 4 histograms (amplitude, mean, sigma and chi2)
  TObjArray* _FitHist[_nPart];
  TObjArray* _FitLambda[_nPart];
  TObjArray* _FitNHit[_nPart];
  TObjArray* _FitBoth[_nPart];
  TObjArray* _FitBoth_1GeV[_nPart];
  TObjArray* _FitLambdaFull[_nPart+1];

  // Storage for names of histograms to have a handle on fit results that are 'free floating' in memory, since TH3::FitSlicesZ has no direct transfer
  std::vector< std::string > _NormBothHistNames[_nPart];
  std::vector< std::string > _NormBothHistNames_1GeV[_nPart];

  // dE/dx resolution over momentum histograms
  TH1* _FitHist_Res[_nPart];  // resolution from fit results
  TH1* _FitHist_ResSum[_nPart];  // resolution from fit results summed up above 1 GeV
  TH1* _ResNormHist[_nPart];  // resolution for all entries of a species above .3 GeV with normalisation
  TH1* _ResNormHist_1GeV[_nPart];  // resolution for all entries of a species above 1 GeV with normalisation
  TH1* _ResElectrons{};  // resolution for all electron entries above .3 GeV, no normalisation
  TH1* _ResNorm_FiducialElectrons{};  // resolution for all fiducial electrons with normalisation

  // Resolution dependence on different parameters, fit histograms
  TH2* _NormLambdaHist[_nPart];  // relative dEdx vs. lambda of track, for tracks with p>1GeV, nHits>=200
  TH1* _FitLambda_Res[_nPart];  // resolution vs. lambda from fit results
  TH2* _NormNHitHist[_nPart];  // normalised dEdx vs. number of hits of track, for tracks with |lambda|<20deg
  TH1* _FitNHit_Res[_nPart];  // resolution vs. number of hits from fit results
  TH3* _NormBothHist[_nPart];  // relative dEdx vs. lambda of track and vs. number of hits
  TH2* _FitBoth_Res[_nPart];  // resolution vs. lambda and vs. number of hits from fit results
  TH3* _NormBothHist_1GeV[_nPart];  // relative dEdx vs. lambda of track and vs. number of hits, for tracks with p>1GeV
  TH2* _FitBoth_Res_1GeV[_nPart];  // resolution vs. lambda and vs. number of hits from fit results, for tracks with p>1GeV
  TH2* _NormLambdaFullHist[_nPart+1];  // relative dEdx vs. lambda of track, for tracks with p>1GeV
  TH1* _FitLambdaFull_Res[_nPart+1];  // resolution vs. lambda from fit results

  // Vector array of all momentum, dE/dx, dE/dx error, tan lambda and number of hits values for each track
  std::vector< std::array<double,6> > _dEdxVec[_nPart];

  // Separation power between all particle combinations
  TH1* _SPHist[_nPart*(_nPart-1)/2];

  // MC particle multiplicity according to track2mctruth link
  TH1* _MCmult{};

  // Cross check: Does the PDG from PFO and MCParticle match?
  TH2* _PDGCheck{};
  TH2* _PDGCheck_weight{};
  TH2* _PDGCheck_eff{};
  TH2* _PDGCheck_pur{};

  // Contamination analyser histogram with given cuts
  TH1* _CutAnaHist{};
  TH1* _NTracksHist{};
  TH1* _NTracksUsedHist{};
  TH1* _NTracksMomHist{};
  TH1* _TrackMomHist{};
  TH1* _dEdxOutlierAbundance{};
  TH1* _dEdxOutlierProportion{};

  // dE/dx error display
  TH2* _dEdxErrorCorrelation{};

  // Hit charge spectrum
  TH1* _HitEnergySpectrum[_nPart+2];
  TH1* _HitNumberSpectrum[_nPart+2];
  TH1* _HitEnergySpectrum_at1[_nPart+2];
  TH1* _HitEnergySpectrum_at25[_nPart+2];
  TH2* _HEHist[_nPart+2];

  // Landau fit results: each TObjArray has 4 histograms (amplitude, MPV, sigma and chi2)
  TObjArray* _HEFitHist[_nPart];

  // handles on number of hits, stored in the vector SubdetectorHitNumbers at a certain index
  const int _TPCID = ILDDetID::TPC;
  const int _TPCindex = _TPCID*2-2;

} ;

#endif
