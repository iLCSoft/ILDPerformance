#include "dEdxAnalyser.h"
#include "marlin/Global.h"
#include "EVENT/LCCollection.h"
#include "IMPL/LCCollectionVec.h"

#include "IMPL/TrackImpl.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/ReconstructedParticleImpl.h"
#include "IMPL/ParticleIDImpl.h"
#include "EVENT/LCRelation.h"
#include "UTIL/LCRelationNavigator.h"
#include "UTIL/Operators.h"
#include "UTIL/PIDHandler.h"

#include <DD4hep/Detector.h>
#include <DDRec/DetectorData.h>
#include <DD4hep/DD4hepUnits.h>

// ROOT
#include "TStyle.h"
#include "TDirectory.h"
#include "TObjArray.h"

#include <math.h>
#include <iostream>

#include "HelixClass.h"

using namespace lcio;
using namespace marlin;


dEdxAnalyser adEdxAnalyser;

dEdxAnalyser::dEdxAnalyser() : Processor("dEdxAnalyser") {
  
  _description = "dEdxAnalyser: Looks into track dEdx values, calculates resolution and separation power, puts everything in histograms and plots them." ;
  
  registerInputCollection(LCIO::TRACK,
			   "TrackCollection",
			   "Name of input collection of reconstructed Tracks",
			   _trackColName,
			   std::string("MarlinTrkTracks"));

  registerInputCollection(LCIO::LCRELATION,
  			   "TrackMCTruthLink",
  			   "LCRelation from the reconstructed tracks to the MC particles",
  			   _tracktruthLinkName,
  			   std::string("MarlinTrkTracksMCTruthLink"));

  registerInputCollection(LCIO::MCPARTICLE,
  			   "MCParticleCollection",
  			   "MC Particles",
  			   _MCParColName,
  			   std::string("MCParticle"));

  registerInputCollection(LCIO::RECONSTRUCTEDPARTICLE,
               "PFOCollection",
               "Pandora PFOs",
               _PFOColName,
               std::string("PandoraPFOs"));


  registerProcessorParameter("nBinsX",
                 "Number of bins of the momentum axis in histograms. Default: 100.",
                 _nBinsX,
                 int(100));

  registerProcessorParameter("nBinsY",
                 "Number of bins of the dE/dx axis in histograms. Default: 300.",
                 _nBinsY,
                 int(300));

  registerProcessorParameter("minBinX",
                 "log_10(momentum/GeV) of the lower end of the momentum axis in histograms. Default: -1 (= 0.1 GeV).",
                 _minBinX,
                 double(-1));

  registerProcessorParameter("maxBinX",
                 "log_10(momentum/GeV) of the upper end of the momentum axis in histograms. Default: 2 (= 100 GeV).",
                 _maxBinX,
                 double(2));

  registerProcessorParameter("minBinY",
                 "Lower end of the dE/dx axis in GeV/mm in histograms. Default: 0 GeV/mm.",
                 _minBinY,
                 double(0));

  registerProcessorParameter("maxBinY",
                 "Upper end of the dE/dx axis in GeV/mm in histograms. Default: 1e-6 GeV/mm.",
                 _maxBinY,
                 double(1e-6));

  registerProcessorParameter("usePFOTracks",
                 "Set true if tracks from (charged) PFOs should be used as data basis instead of all tracks from the TrackCollection.",
                 _usePFOTracks,
                 bool(false));

  registerProcessorParameter("useOneTrack",
                 "Set true if from every event only the first track should be used.",
                 _useOneTrack,
                 bool(false));

  registerProcessorParameter("cutdEdx",
                 "Set true if particles with a very off dE/dx value should be ignored.",
                 _cutdEdx,
                 bool(false));

  registerProcessorParameter("cutTrackPurity",
                 "Set true if particles should be ignored, where the MCParticle is connected to more than one track.",
                 _cutTrackPurity,
                 bool(false));

  registerProcessorParameter("cutTrackPurityMom",
                 "Set true if particles should be ignored, where the MCParticle is connected to more than one track, but the track with the highest momentum is still used.",
                 _cutTrackPurityMom,
                 bool(false));

  registerProcessorParameter("cutD0",
                 "Tracks with a d0 larger than the given value should be ignored. Set to 0 to accept all particles.",
                 _cutD0,
                 double(0));

  registerProcessorParameter("cutZ0",
                 "Tracks with a z0 larger than the given value should be ignored. Set to 0 to accept all particles.",
                 _cutZ0,
                 double(0));

  registerProcessorParameter("cutMomMin",
                 "Tracks with a momentum smaller than the given value should be ignored. Set to 0 to accept all particles.",
                 _cutMomMin,
                 double(0));

  registerProcessorParameter("cutMomMax",
                 "Tracks with a momentum larger than the given value should be ignored. Set to 0 to accept all particles.",
                 _cutMomMax,
                 double(0));

  registerProcessorParameter("plotStuff",
                 "Set true to automatically plot various histograms as .png in the current working directory. Default: true.",
                 _plotStuff,
                 bool(false));

  registerProcessorParameter("useLCTPCStyle",
                 "Set true if you would like to use the LCTPCStyle.C from MarlinTPC/analysis/rootscripts as style for the plots. Default: false.",
                 _useLCTPCStyle,
                 bool(false));

  registerProcessorParameter("fileFormat",
                 "Specify the format of the automatically plotted images (if chosen). Selection depends on root TASImage capabilities, typically: [.gif, .png, .xpm, .jpg, .tiff, .xcf]. Default: .png.",
                 _fileFormat,
                 std::string(".png"));
  // .pdf, .ps, .eps, .svg, .tex, .C, .root, .xml,

}

void dEdxAnalyser::init() {

  // usually a good idea to
  printParameters();

  _nRun = -1;
  _nEvt = 0;

  // prepare geometry info
  dd4hep::Detector& theDet = dd4hep::Detector::getInstance();
  dd4hep::DetElement tpcDE = theDet.detector("TPC") ;
  const dd4hep::rec::FixedPadSizeTPCData* tpc = tpcDE.extension<dd4hep::rec::FixedPadSizeTPCData>() ;
  _TPC_inner = tpc->rMinReadout/dd4hep::mm;
  _TPC_outer = tpc->rMaxReadout/dd4hep::mm;
  double bfieldV[3] ;
  theDet.field().magneticField( { 0., 0., 0. }  , bfieldV  ) ;
  _bField = bfieldV[2]/dd4hep::tesla ;

  streamlog_out(MESSAGE) << "geometry prepared" << std::endl;

  // prepare bins and string content for histograms
  //const int nBinsX=100, nBinsY=300;
  //double minBinX=-1, maxBinX=2, minBinX2=-3;  // log10(min/max momentum / GeV)
  //double minBinY= 0, maxBinY=1e-6;  // for dE/dx values in GeV/mm
  //double histbinsY[nBinsY+1], histbinsX2[nBinsX+1];  //histbinsX[nBinsX+1]
  double* histbinsX = new double[_nBinsX+1];
  double* histbinsX2= new double[_nBinsX+1];
  double* histbinsY = new double[_nBinsY+1];
  double minBinX2 = _minBinX - 1;

  for (int i=0; i<_nBinsX+1; i++) histbinsX [i] = pow( 10, _minBinX + (_maxBinX-_minBinX)*i/(float)(_nBinsX) );
  for (int i=0; i<_nBinsX+1; i++) histbinsX2[i] = pow( 10, minBinX2 + (_maxBinX-minBinX2)*i/(float)(_nBinsX) );
  for (int i=0; i<_nBinsY+1; i++) histbinsY [i] = _minBinY + (_maxBinY-_minBinY)*i/(float)(_nBinsY);
  std::vector<std::string> Particles = {"Electrons","Muons","Pions","Kaons","Protons","Other","All"};                          // size: _nPart+2
  std::vector<std::string> particles = {"electrons","muons","pions","kaons","protons","all other particles","all particles"};  // size: _nPart+2
  std::vector<std::string> SP_part   = {"e","mu","pi","ka","p"};                                                               // size: _nPart
  _PDG = {11, 13, 211, 321, 2212, 10};  // size: _nPart+1

  AIDAProcessor::tree(this);
  streamlog_out(MESSAGE) << "this treeed" << std::endl;

  // create Bethe-Bloch histograms
  for (int i=0; i<_nPart+2; i++)
  {
	std::stringstream s1; s1 << "BB" << Particles[i];
	std::stringstream s2; s2 << "Bethe-Bloch histogram for " << particles[i];
	_BBHist[i] = new TH2D(s1.str().c_str(), s2.str().c_str(), _nBinsX,histbinsX,_nBinsY,histbinsY);
	_BBHist[i]->SetXTitle("momentum / GeV/c");
	_BBHist[i]->SetYTitle("dE/dx / GeV/mm");
  }

  // create space for fit results
  for (int i=0; i<_nPart; i++) _FitHist[i]      = new TObjArray();
  for (int i=0; i<_nPart; i++) _FitLambda[i]    = new TObjArray();
  for (int i=0; i<_nPart; i++) _FitNHit[i]      = new TObjArray();
  for (int i=0; i<_nPart; i++) _FitBoth[i]      = new TObjArray();
  for (int i=0; i<_nPart; i++) _FitBoth_1GeV[i] = new TObjArray();

  // create resolution and separation power histograms
  int sp_c = 0;
  for (int i=0; i<_nPart; i++)
  {
    std::stringstream s1; s1 << "BB" << Particles[i] << "_FitRes";
    std::stringstream s2; s2 << "dE/dx resolution for " << particles[i] << " from fit result";
    _FitHist_Res[i] = new TH1D(s1.str().c_str(), s2.str().c_str(), _nBinsX, histbinsX);
    _FitHist_Res[i]->SetXTitle("momentum / GeV/c");
    _FitHist_Res[i]->SetYTitle("dE/dx resolution");

    std::stringstream s3; s3 << "BB" << Particles[i] << "_FitResSum";
    std::stringstream s4; s4 << "dE/dx resolution distribution above 1 GeV for " << particles[i] << " from fit result";
    _FitHist_ResSum[i] = new TH1D(s3.str().c_str(), s4.str().c_str(), 100, 0, .2);
    _FitHist_ResSum[i]->SetXTitle("dE/dx resolution");
    _FitHist_ResSum[i]->SetYTitle("number of momentum bins with that resolution");

    std::stringstream s5; s5 << "Res" << Particles[i] << "_Norm";
    std::stringstream s6; s6 << "Relative dE/dx resolution distribution for " << particles[i] << " from all entries above .3 GeV";
    _ResNormHist[i] = new TH1D(s5.str().c_str(), s6.str().c_str(), 100, 0, 2);
    _ResNormHist[i]->SetXTitle("relative dE/dx");
    _ResNormHist[i]->SetYTitle("rel. abundance");

    std::stringstream s7; s7 << "Res" << Particles[i] << "_Norm_1GeV";
    std::stringstream s8; s8 << "Relative dE/dx resolution distribution for " << particles[i] << " from all entries above 1 GeV";
    _ResNormHist_1GeV[i] = new TH1D(s7.str().c_str(), s8.str().c_str(), 100, 0, 2);
    _ResNormHist_1GeV[i]->SetXTitle("relative dE/dx");
    _ResNormHist_1GeV[i]->SetYTitle("rel. abundance");

    // Resolution dependence on different parameters
    std::stringstream s9; s9 << "NormLambda" << Particles[i] << "";
    std::stringstream s10; s10 << "Relative dE/dx distribution over lambda with p>1 GeV and nHit>=200 for " << particles[i] << " from all entries";
    _NormLambdaHist[i] = new TH2D(s9.str().c_str(), s10.str().c_str(), 18, 0, 90, 100, 0, 2);
    _NormLambdaHist[i]->SetXTitle("|lambda| / deg");
    _NormLambdaHist[i]->SetYTitle("relative dE/dx");

    std::stringstream s11; s11 << "NormLambda" << Particles[i] << "_FitRes";
    std::stringstream s12; s12 << "dE/dx resolution for " << particles[i] << " over lambda from fit result";
    _FitLambda_Res[i] = new TH1D(s11.str().c_str(), s12.str().c_str(), 18, 0, 90);
    _FitLambda_Res[i]->SetXTitle("|lambda| / deg");
    _FitLambda_Res[i]->SetYTitle("dE/dx resolution");

    std::stringstream s13; s13 << "NormNHit" << Particles[i] << "";
    std::stringstream s14; s14 << "Relative dE/dx distribution over number of hits with p>1GeV and |lambda|<20deg for " << particles[i] << " from all entries";
    _NormNHitHist[i] = new TH2D(s13.str().c_str(), s14.str().c_str(), 110, 0.5, 220.5, 100, 0, 2);
    _NormNHitHist[i]->SetXTitle("number of hits");
    _NormNHitHist[i]->SetYTitle("relative dE/dx");

    std::stringstream s15; s15 << "NormNHit" << Particles[i] << "_FitRes";
    std::stringstream s16; s16 << "dE/dx resolution for " << particles[i] << " over number of hits from fit result";
    _FitNHit_Res[i] = new TH1D(s15.str().c_str(), s16.str().c_str(), 110, 0.5, 220.5);
    _FitNHit_Res[i]->SetXTitle("number of hits");
    _FitNHit_Res[i]->SetYTitle("dE/dx resolution");

    std::stringstream s20; s20 << "NormBoth" << Particles[i] << "";
    std::stringstream s21; s21 << "Relative dE/dx distribution over lambda and number of hits for " << particles[i] << " from all entries";
    _NormBothHist[i] = new TH3D(s20.str().c_str(), s21.str().c_str(), 18, 0, 90, 120, 0.5, 240.5, 100, 0, 2);
    _NormBothHist[i]->SetXTitle("|lambda| / deg");
    _NormBothHist[i]->SetYTitle("number of hits");
    _NormBothHist[i]->SetZTitle("relative dE/dx");

    std::string HistName = s20.str();
    _NormBothHistNames[i].push_back(HistName.append("_0"));
    _NormBothHistNames[i].push_back(HistName.replace(HistName.end()-1,HistName.end(),"1"));
    _NormBothHistNames[i].push_back(HistName.replace(HistName.end()-1,HistName.end(),"2"));
    _NormBothHistNames[i].push_back(HistName.replace(HistName.end()-1,HistName.end(),"chi2"));

    std::stringstream s22; s22 << "NormBoth" << Particles[i] << "_FitRes";
    std::stringstream s23; s23 << "dE/dx resolution for " << particles[i] << " over lambda and number of hits from fit result";
    _FitBoth_Res[i] = new TH2D(s22.str().c_str(), s23.str().c_str(), 18, 0, 90, 120, 0.5, 240.5);
    _FitBoth_Res[i]->SetXTitle("|lambda| / deg");
    _FitBoth_Res[i]->SetYTitle("number of hits");
    _FitBoth_Res[i]->SetZTitle("dE/dx resolution");

    std::stringstream s24; s24 << "NormBoth" << Particles[i] << "_1GeV";
    std::stringstream s25; s25 << "Relative dE/dx distribution over lambda and number of hits with p>1GeV for " << particles[i] << " from all entries";
    _NormBothHist_1GeV[i] = new TH3D(s24.str().c_str(), s25.str().c_str(), 18, 0, 90, 120, 0.5, 240.5, 100, 0, 2);
    _NormBothHist_1GeV[i]->SetXTitle("|lambda| / deg");
    _NormBothHist_1GeV[i]->SetYTitle("number of hits");
    _NormBothHist_1GeV[i]->SetZTitle("relative dE/dx");

    HistName = s24.str();
    _NormBothHistNames_1GeV[i].push_back(HistName.append("_0"));
    _NormBothHistNames_1GeV[i].push_back(HistName.replace(HistName.end()-1,HistName.end(),"1"));
    _NormBothHistNames_1GeV[i].push_back(HistName.replace(HistName.end()-1,HistName.end(),"2"));
    _NormBothHistNames_1GeV[i].push_back(HistName.replace(HistName.end()-1,HistName.end(),"chi2"));

    std::stringstream s26; s26 << "NormBoth" << Particles[i] << "_FitRes_1GeV";
    std::stringstream s27; s27 << "dE/dx resolution for " << particles[i] << " with p>1GeV over lambda and number of hits from fit result";
    _FitBoth_Res_1GeV[i] = new TH2D(s26.str().c_str(), s27.str().c_str(), 18, 0, 90, 120, 0.5, 240.5);
    _FitBoth_Res_1GeV[i]->SetXTitle("|lambda| / deg");
    _FitBoth_Res_1GeV[i]->SetYTitle("number of hits");
    _FitBoth_Res_1GeV[i]->SetZTitle("dE/dx resolution");

    for (int j=i+1; j<_nPart; j++)  // Separation Power
    {
      std::stringstream s17; s17 << "SP_" << SP_part[i] << "_" << SP_part[j];
      std::stringstream s18; s18 << "Separation power between " << particles[i] << " and " << particles[j];
      _SPHist[sp_c] = new TH1D(s17.str().c_str(), s18.str().c_str(), _nBinsX, histbinsX);
      _SPHist[sp_c]->SetXTitle("momentum / GeV/c");
      _SPHist[sp_c]->SetYTitle("separation power");
      sp_c++;
    }
  }

  // some other histograms for cross-checks etc.
  _ResElectrons = new TH1D("ResElectrons","dE/dx resolution distribution above .3 GeV for electrons from all entries, not normalised", 100, _minBinY, _maxBinY/2.);
  _MCmult = new TH1D("MCmult","MC Particle multiplicity",11,-.5,10.5);

  _PDGCheck        = new TH2I("PDGCheck"       ,"PDG cross check PFO vs. MCTruth",_nPart+1,-.5,_nPart+.5,_nPart+1,-.5,_nPart+.5);
  _PDGCheck_weight = new TH2D("PDGCheck_weight","PDG cross check PFO vs. MCTruth, likelihood weighted",_nPart+1,-.5,_nPart+.5,_nPart+1,-.5,_nPart+.5);
  _PDGCheck_eff    = new TH2D("PDGCheck_eff"   ,"PDG cross check PFO vs. MCTruth, efficiency (vertical sum = 1)",_nPart+1,-.5,_nPart+.5,_nPart+1,-.5,_nPart+.5);
  _PDGCheck_pur    = new TH2D("PDGCheck_pur"   ,"PDG cross check PFO vs. MCTruth, purity (horizontal sum = 1)  ",_nPart+1,-.5,_nPart+.5,_nPart+1,-.5,_nPart+.5);
  _PDGCheck->SetXTitle("MCTruth PDG");
  _PDGCheck->SetYTitle("PFO PDG");
  _PDGCheck_weight->SetXTitle("MCTruth PDG");
  _PDGCheck_weight->SetYTitle("PFO PDG");
  _PDGCheck_eff->SetXTitle("MCTruth PDG");
  _PDGCheck_eff->SetYTitle("PFO PDG");
  _PDGCheck_pur->SetXTitle("MCTruth PDG");
  _PDGCheck_pur->SetYTitle("PFO PDG");
  for (int i=0; i<_nPart+1; i++)
  {
    std::stringstream s; s << _PDG[i];
    _PDGCheck->GetXaxis()->SetBinLabel(i+1,s.str().c_str());
    _PDGCheck->GetYaxis()->SetBinLabel(i+1,s.str().c_str());
    _PDGCheck_weight->GetXaxis()->SetBinLabel(i+1,s.str().c_str());
    _PDGCheck_weight->GetYaxis()->SetBinLabel(i+1,s.str().c_str());
    _PDGCheck_eff->GetXaxis()->SetBinLabel(i+1,s.str().c_str());
    _PDGCheck_eff->GetYaxis()->SetBinLabel(i+1,s.str().c_str());
    _PDGCheck_pur->GetXaxis()->SetBinLabel(i+1,s.str().c_str());
    _PDGCheck_pur->GetYaxis()->SetBinLabel(i+1,s.str().c_str());
  }

  _CutAnaHist = new TH1D("CutAnaHist","Contamination analyser histogram with given cuts",10,-.5,9.5);
  _NTracksHist     = new TH1D("NTracksHist"    ,"Number of tracks of each event",20,0,0);
  _NTracksUsedHist = new TH1D("NTracksUsedHist","Number of tracks of each event actually used after cuts",20,0,0);
  _NTracksMomHist  = new TH1D("NTracksMomHist" ,"Number of tracks of each event actually used after cuts and with p > 100 MeV",20,0,0);
  _TrackMomHist = new TH1D("TrackMomHist","Momentum of Tracks",_nBinsX,histbinsX2);

  _dEdxOutlierAbundance = new TH1D("dEdxOutlierAbundance","Number of tracks with dE/dx values that don't match the MC PDG",5,-.5,4.5);
  _dEdxOutlierAbundance->GetXaxis()->SetBinLabel(1,"MCPion - dE/dx too large");
  _dEdxOutlierAbundance->GetXaxis()->SetBinLabel(2,"MCKaon - dE/dx too large");
  _dEdxOutlierAbundance->GetXaxis()->SetBinLabel(3,"MCKaon - dE/dx too small");
  _dEdxOutlierAbundance->GetXaxis()->SetBinLabel(4,"MCProton - dE/dx too small");
  _dEdxOutlierAbundance->SetYTitle("abundance");

  _dEdxOutlierProportion = new TH1D("dEdxOutlierProportion","Proportion of tracks with dE/dx values that don't match the MC PDG",5,-.5,4.5);
  _dEdxOutlierProportion->GetXaxis()->SetBinLabel(1,"MCPion - dE/dx too large");
  _dEdxOutlierProportion->GetXaxis()->SetBinLabel(2,"MCKaon - dE/dx too large");
  _dEdxOutlierProportion->GetXaxis()->SetBinLabel(3,"MCKaon - dE/dx too small");
  _dEdxOutlierProportion->GetXaxis()->SetBinLabel(4,"MCProton - dE/dx too small");
  _dEdxOutlierProportion->SetYTitle("proportion of MCSpecies");

  _dEdxErrorCorrelation = new TH2D("dEdxErrorCorrelation","Correlation plot of dE/dx error vs. dE/dx as cross check",100,_minBinY,_maxBinY,100,_minBinY,_maxBinY*.1);

  delete[] histbinsX;
  delete[] histbinsX2;
  delete[] histbinsY;
}


void dEdxAnalyser::processRunHeader( LCRunHeader* ) {
  _nRun++ ;
  _nEvt = 0;
} 

void dEdxAnalyser::processEvent( LCEvent * evt ) {

  //streamlog_out(MESSAGE) << _nEvt << std::endl;

  LCCollection *col_track{}, *col_t2trel{}, *col_pfo{};

  try
  {
    col_track  = evt->getCollection( _trackColName );
    col_t2trel = evt->getCollection( _tracktruthLinkName );
  }
  catch(DataNotAvailableException &e)
  {
    streamlog_out(MESSAGE) << "Input collection not found - skipping event " << _nEvt << std::endl;
    return;
  }
  int n_track = col_track->getNumberOfElements();
  int n_pfo = 0;

  if (_usePFOTracks)
  {
    try { col_pfo = evt->getCollection( _PFOColName ); }
    catch(DataNotAvailableException &e)
    {
      streamlog_out(MESSAGE) << "Input PFO collection not found - skipping event " << _nEvt << std::endl;
      return;
    }
    n_pfo = col_pfo->getNumberOfElements();
  }

  _NTracksHist->Fill(n_track);

  LCRelationNavigator* nav_t2trel = new LCRelationNavigator(col_t2trel);

  int n_track_used = 0;
  int n_track_mom = 0;
  std::vector<double> momVec;

  // check how many tracks to loop over, then loop
  int n_track_pfo = _usePFOTracks ? n_pfo                   : n_track;
  int n_track_mod = _useOneTrack  ? std::min(n_track_pfo,1) : n_track_pfo;
  for (int i=0; i<n_track_mod; ++i)
  {
    int pdg = 0, mcpdg_s = 0;
    double maxweight = 0;
    double maxtrackmom=0;

    Track* track = NULL;
    ReconstructedParticle* pfo = NULL;
    PIDHandler* pidh = NULL;

    if (_usePFOTracks)
    {
      pfo = dynamic_cast<ReconstructedParticle*>(col_pfo->getElementAt(i));
      if (abs(pfo->getCharge())   != 1) continue;
      if (pfo->getTracks().size() != 1) continue;
      track = pfo->getTracks()[0];
      pidh = new PIDHandler(col_pfo);
    }
    else track = dynamic_cast<Track*>(col_track->getElementAt(i));

    const MCParticleVec& vec_mcpar = (MCParticleVec&) nav_t2trel->getRelatedToObjects(track);
    const FloatVec& wei_mcpar = nav_t2trel->getRelatedToWeights(track);
    MCParticle* mcpar = NULL;

    if(vec_mcpar.size()!=0)
    {
      for (unsigned int j=0; j<std::min(vec_mcpar.size(), wei_mcpar.size()); j++)
      {
        if (wei_mcpar[j]>.6 && wei_mcpar[j]>maxweight)
        {
          mcpar = vec_mcpar[j];
          int mcpdg = abs(vec_mcpar[j]->getPDG());
          if(mcpdg==11 || mcpdg==13 || mcpdg==211 || mcpdg==321 || mcpdg==2212) {pdg = mcpdg; mcpdg_s = vec_mcpar[j]->getPDG();}
          else pdg = 10;  // flag: other particle
          maxweight = wei_mcpar[j];
        }
      }
    }

    _MCmult->Fill(vec_mcpar.size());

    float dEdx = track->getdEdx();

    if (pdg!=0 && dEdx>1e-8 && mcpar)
    {
      int n_rel_tracks = nav_t2trel->getRelatedFromObjects(mcpar).size();

      // If more other tracks are connected to the same MCParticle, skip this track.
      if (_cutTrackPurity) if (n_rel_tracks>1) continue;

      HelixClass trkHelixC;
      trkHelixC.Initialize_Canonical(track->getPhi(),track->getD0(),track->getZ0(),track->getOmega(),track->getTanLambda(),_bField);
      double mom = sqrt(pow(trkHelixC.getMomentum()[0],2) + pow(trkHelixC.getMomentum()[1],2) + pow(trkHelixC.getMomentum()[2],2));

      // Momentum cuts
      if (_cutMomMin) if (mom<_cutMomMin) continue;
      if (_cutMomMax) if (mom>_cutMomMax) continue;

      // If more other tracks are connected to the same MCParticle and this is not the track with the highest momentum among them, skip this track.
      if (_cutTrackPurityMom) if (n_rel_tracks>1)
      {
        const TrackVec& vec_tracks = (TrackVec&) nav_t2trel->getRelatedFromObjects(mcpar);
        for (int j=0; j<n_rel_tracks; ++j)
        {
          Track* tempTrack = vec_tracks[j];
          HelixClass tempHelix;
          tempHelix.Initialize_Canonical(tempTrack->getPhi(),tempTrack->getD0(),tempTrack->getZ0(),tempTrack->getOmega(),tempTrack->getTanLambda(),_bField);
          double tempmom = sqrt(pow(tempHelix.getMomentum()[0],2) + pow(tempHelix.getMomentum()[1],2) + pow(tempHelix.getMomentum()[2],2));
          if (tempmom>maxtrackmom) maxtrackmom = tempmom;
        }
        if (mom < maxtrackmom) continue;
      }

      // If the track is too far away from the IP, skip this track.
      if (_cutD0) if (track->getD0()>_cutD0) continue;
      if (_cutZ0) if (track->getZ0()>_cutZ0) continue;

      // Register outlier dE/dx values.
      bool cutdEdx = false;
      if (pdg== 211 &&  dEdx>(pow(log10(mom),2)+.2)*1e-6)
      {
        streamlog_out(DEBUG) << "Event " << evt->getEventNumber() << ": dE/dx does not match!  PDG = 211, dE/dx = " << dEdx << ", upper limit = " << (pow(log10(mom),2)+.2)*1e-6 << std::endl;
        _dEdxOutlierAbundance->Fill(0);
        cutdEdx = true;
      }
      if (pdg== 321 && dEdx>(pow(log10(mom)*1.3,2)+.2)*1e-6)
      {
        streamlog_out(DEBUG) << "Event " << evt->getEventNumber() << ": dE/dx does not match!  PDG = 321, dE/dx = " << dEdx << ", upper limit = " << (pow(log10(mom)*1.3,2)+.2)*1e-6 << std::endl;
        _dEdxOutlierAbundance->Fill(1);
        cutdEdx = true;
      }
      if (pdg== 321 && dEdx<(.28-mom/3.)*1e-6)
      {
        streamlog_out(DEBUG) << "Event " << evt->getEventNumber() << ": dE/dx does not match!  PDG = 321, dE/dx = " << dEdx << ", lower limit = " << (.28-mom/3.)*1e-6 << std::endl;
        _dEdxOutlierAbundance->Fill(2);
        cutdEdx = true;
      }
      if (pdg==2212 && dEdx<(.3-mom/5.)*1e-6)
      {
        streamlog_out(DEBUG) << "Event " << evt->getEventNumber() << ": dE/dx does not match!  PDG = 2212, dE/dx = " << dEdx << ", lower limit = " << (.3-mom/5.)*1e-6 << std::endl;
        _dEdxOutlierAbundance->Fill(3);
        cutdEdx = true;
      }

      // If the track dE/dx value is obviously not matching the MCParticle PDG, skip this track.
      if (_cutdEdx) if (cutdEdx) continue;

      // All cuts passed!
      n_track_used++;
      if (mom > .1) n_track_mom++;
      if (n_track > 1 ) _TrackMomHist->Fill(mom);
      momVec.push_back(mom);

      for (int j=0; j<_nPart; j++) if (pdg==_PDG[j] && mom>.3 && mom<100) _dEdxVec[j].push_back(std::array<double,5> {{mom,dEdx,track->getdEdxError(),track->getTanLambda(),(double)track->getSubdetectorHitNumbers()[_TPCindex]}});
      for (int j=0; j<_nPart; j++) if (pdg==_PDG[j]) _BBHist[j]->Fill(mom,dEdx);
      if (pdg==10) _BBHist[_nPart]->Fill(mom,dEdx);
      _BBHist[_nPart+1]->Fill(mom,dEdx);
      if (pdg==11 && mom>.3) _ResElectrons->Fill(dEdx);
      _CutAnaHist->Fill(0);
      _dEdxErrorCorrelation->Fill(dEdx,track->getdEdxError());

      if (_usePFOTracks)
      {
        //int PFO_pdg = pfo->getParticleIDs()[0]->getPDG();
        int PFO_pdg = pidh->getParticleID(pfo,pidh->getAlgorithmID("dEdxPID")).getPDG();
        if (PFO_pdg==0) PFO_pdg=10;
        for (int j=0; j<_nPart+1; j++) if (pdg==_PDG[j])
          for (int k=0; k<_nPart+1; k++) if (PFO_pdg==_PDG[k])
            { _PDGCheck->Fill(j,k); _PDGCheck_weight->Fill(j,k,pidh->getParticleID(pfo,pidh->getAlgorithmID("dEdxPID")).getLikelihood()); }
      }


      if (0 && pdg==2212 && dEdx<(.3-mom/5.)*1e-6)  //  pions that are pdg-registered as protons
      {
        streamlog_out(MESSAGE) << mom << " " << dEdx << " " << vec_mcpar.size() << " " << maxweight << std::endl;
        if (mcpdg_s== 2212) _CutAnaHist->Fill(1);
        if (mcpdg_s==-2212) _CutAnaHist->Fill(2);
        if (maxweight>=1) _CutAnaHist->Fill(3);
        for (unsigned int j=0; j<std::min(vec_mcpar.size(), wei_mcpar.size()); j++)
        {
          streamlog_out(MESSAGE) << wei_mcpar[j];
          if (j<5) _CutAnaHist->Fill(j+4);
        }
        streamlog_out(MESSAGE) << std::endl;

        LCCollection *col_mcpar{};
        col_mcpar = evt->getCollection( _MCParColName );
        MCParticle* mcpar0 = dynamic_cast<MCParticle*>(col_mcpar->getElementAt(0));
        _MCmomVec.push_back(mcpar0->getMomentum()[0]);
        _MCmomVec.push_back(mcpar0->getMomentum()[1]);
        _MCmomVec.push_back(mcpar0->getMomentum()[2]);
      }
    }

    // check which hits are inside the TPC
    TrackerHitVec trkHits = track->getTrackerHits();
    int nHitsTPC = 0;
    for(unsigned int iHit=0; iHit<trkHits.size(); iHit++){
      if(trkHits[iHit]->getType() == 0)
      {
        double x = trkHits[iHit]->getPosition()[0];
        double y = trkHits[iHit]->getPosition()[1];
        if(sqrt(x*x+y*y)>=_TPC_inner && sqrt(x*x+y*y)<=_TPC_outer) nHitsTPC++;
      }
    }

    // check the error exponents by reconstructing the dependence on the number of hits and the angle
    //int nHits = track->getSubdetectorHitNumbers()[_TPCindex];
    //float tanLam = track->getTanLambda();

    if (_usePFOTracks) delete pidh;

  }  // end of for (tracks)

  _NTracksUsedHist->Fill(n_track_used);
  _NTracksMomHist ->Fill(n_track_mom);
  std::sort(momVec.begin(),momVec.end());
  if (momVec.size() > 1) if (momVec[0] > 1.5*momVec[1])

  delete nav_t2trel;

  _nEvt++;
}

void dEdxAnalyser::check( LCEvent * ) { }

void dEdxAnalyser::end()
{
  // axes stuff - todo: the limits should be imported from ::init()!!
  //const int nBinsX=100;
  //double minBinX=-1, maxBinX=2, minBinX2=-3;
  //double maxBinY=1e-6;
  //double histbinsX[nBinsX+1], histbinsX2[nBinsX+1];
  double* histbinsX = new double[_nBinsX+1];
  double* histbinsX2= new double[_nBinsX+1];
  double minBinX2 = _minBinX - 1;

  for (int i=0; i<_nBinsX+1; i++) histbinsX[i] = pow( 10, _minBinX  + (_maxBinX-_minBinX )*i/(float)(_nBinsX) );
  for (int i=0; i<_nBinsX+1; i++) histbinsX2[i]= pow( 10, minBinX2 + (_maxBinX-minBinX2)*i/(float)(_nBinsX) );

  // set up fitting functions
  TF1* gausfit = new TF1("gausfit","gaus(0)",0,_maxBinY);
  gausfit->SetParameters(1,_maxBinY/3.,_maxBinY/10.);
  gausfit->SetParLimits(0,0,1e6);
  gausfit->SetParLimits(1,0,_maxBinY);
  gausfit->SetParLimits(2,0,_maxBinY/2.);

  TF1* gausfitrel = new TF1("gausfitrel","gaus(0)",0,2);
  gausfitrel->SetParameters(_nEvt/100.,1,.2);

  TF2* powla2 = new TF2("powla2","0.047*([0]/cos(x/180.*TMath::Pi()))**[1]*(y/[2])**[3]",0,90,0,220);
  powla2->SetParameters(1,-0.34,220,-.47);

  // fit the BB histograms for each momentum bin, put fit result histograms in _FitHist
  for (int i=0; i<_nPart; i++) _BBHist[i]->FitSlicesY(gausfit,0,-1,0,"QR",_FitHist[i]);

  // from fit results calculate separation power for each particle combination and each momentum bin
  // as well as resolution and fill into corresponding histograms
  int sp_c = 0;
  for (int i=0; i<_nPart; i++)
  {
    TH1* FitHist1_ampl  = (TH1D*)_FitHist[i]->At(1);
    TH1* FitHist1_sigma = (TH1D*)_FitHist[i]->At(2);

    for (int k=1; k<_nBinsX+1; k++)  // Resolution from fit results; 0 is underflow bin, nBinsx+1 is overflow bin
    {
      double res = FitHist1_sigma->GetBinContent(k)*1. / FitHist1_ampl->GetBinContent(k);
      if (std::isnan(res)) res = 0;
      _FitHist_Res[i]->SetBinContent(k,res);
      if (histbinsX[k] >= 1) _FitHist_ResSum[i]->Fill(res);
    }
    _FitHist_ResSum[i]->Fit("gaus","Q");

    for (unsigned int v=0; v<_dEdxVec[i].size(); v++)  // Normalised Resolution
    {
      int bin = FitHist1_ampl->FindBin(_dEdxVec[i][v][0]);
      double binMean  = FitHist1_ampl->GetBinContent(bin);
      double normMean = _dEdxVec[i][v][1]/binMean;
      double relError = _dEdxVec[i][v][1]/_dEdxVec[i][v][2];
      double lambda   = fabs(atan(_dEdxVec[i][v][3])) *180/M_PI;
      if (binMean > 0)
      {
        _ResNormHist[i]->Fill(normMean,relError);
        if (_dEdxVec[i][v][0]>1) _ResNormHist_1GeV[i]->Fill(normMean,relError);
        if (_dEdxVec[i][v][0]>1 && _dEdxVec[i][v][4]>=200) _NormLambdaHist[i]->Fill(lambda,normMean);
        if (lambda<20) _NormNHitHist[i]->Fill(_dEdxVec[i][v][4],normMean);
        _NormBothHist[i]->Fill(lambda,_dEdxVec[i][v][4],normMean);
        if (_dEdxVec[i][v][0]>1) _NormBothHist_1GeV[i]->Fill(lambda,_dEdxVec[i][v][4],normMean);
      }
    }
    _ResNormHist[i]->Fit("gaus","Q");
    _ResNormHist_1GeV[i]->Fit("gaus","Q");

    _NormLambdaHist[i]->FitSlicesY(gausfitrel,0,-1,0,"QR",_FitLambda[i]);  // angular distribution
    TH1* FitLambda_ampl  = (TH1D*)_FitLambda[i]->At(1);
    TH1* FitLambda_sigma = (TH1D*)_FitLambda[i]->At(2);
    for (int d=1; d<=18; d++)
    {
      double res = FitLambda_sigma->GetBinContent(d)*1. / FitLambda_ampl->GetBinContent(d);
      if (std::isnan(res)) res = 0;
      _FitLambda_Res[i]->SetBinContent(d,res);
    }

    _NormNHitHist[i]->FitSlicesY(gausfitrel,0,-1,0,"QR",_FitNHit[i]);  // number-of-hits distribution
    TH1* FitNHit_ampl  = (TH1D*)_FitNHit[i]->At(1);
    TH1* FitNHit_sigma = (TH1D*)_FitNHit[i]->At(2);
    for (int d=1; d<=110; d++)
    {
      double res = FitNHit_sigma->GetBinContent(d)*1. / FitNHit_ampl->GetBinContent(d);
      if (std::isnan(res)) res = 0;
      _FitNHit_Res[i]->SetBinContent(d,res);
    }

    _NormBothHist[i]->FitSlicesZ(gausfitrel,0,-1,0,-1,0,"QR");  // angular and number-of-hits distribution
    //TH2* FitBoth_ampl  = (TH2D*)gDirectory->Get(_NormBothHistNames[i][0].c_str());
    TH2* FitBoth_mean  = (TH2D*)gDirectory->Get(_NormBothHistNames[i][1].c_str());
    TH2* FitBoth_sigma = (TH2D*)gDirectory->Get(_NormBothHistNames[i][2].c_str());
    //TH2* FitBoth_chi2  = (TH2D*)gDirectory->Get(_NormBothHistNames[i][3].c_str());
    TH2* ProjZ = dynamic_cast<TH2*>(_NormBothHist[i]->Project3D("yx"));
    for (int d=1; d<=18; d++)
    {
      for (int e=1; e<=110; e++)
      {
        double mu = FitBoth_mean ->GetBinContent(d,e);
        double si = FitBoth_sigma->GetBinContent(d,e);
        //double am = FitBoth_ampl ->GetBinContent(d,e);
        double dmu= FitBoth_mean ->GetBinError(d,e);
        double dsi= FitBoth_sigma->GetBinError(d,e);
        double res = 1.* si / mu;
        double reserr = sqrt( pow(1.*dsi/mu, 2) + pow(1.*si/mu/mu*dmu, 2) );
        if (std::isnan(res) || std::isnan(reserr) || ProjZ->GetBinContent(d,e)==0) {res=0; reserr=0;}
        //if (am>700 && (j<52 || (k<5 || k>10 ))) {res=0; reserr=0;}  // fit failure protection

        _FitBoth_Res[i]->SetBinContent(d,e,res);
        _FitBoth_Res[i]->SetBinError(d,e,reserr);
      }
    }
    _FitBoth_Res[i]->Fit(powla2,"Q");

    _NormBothHist_1GeV[i]->FitSlicesZ(gausfitrel,0,-1,0,-1,0,"QR");  // angular and number-of-hits distribution above 1 GeV
    //TH2* FitBoth_1GeV_ampl  = (TH2D*)gDirectory->Get(_NormBothHistNames_1GeV[i][0].c_str());
    TH2* FitBoth_1GeV_mean  = (TH2D*)gDirectory->Get(_NormBothHistNames_1GeV[i][1].c_str());
    TH2* FitBoth_1GeV_sigma = (TH2D*)gDirectory->Get(_NormBothHistNames_1GeV[i][2].c_str());
    //TH2* FitBoth_1GeV_chi2  = (TH2D*)gDirectory->Get(_NormBothHistNames_1GeV[i][3].c_str());
    TH2* ProjZ_1GeV = dynamic_cast<TH2*>(_NormBothHist_1GeV[i]->Project3D("yx"));
    for (int d=1; d<=18; d++)
    {
      for (int e=1; e<=110; e++)
      {
        double mu = FitBoth_1GeV_mean ->GetBinContent(d,e);
        double si = FitBoth_1GeV_sigma->GetBinContent(d,e);
        double dmu= FitBoth_1GeV_mean ->GetBinError(d,e);
        double dsi= FitBoth_1GeV_sigma->GetBinError(d,e);
        double res = 1.* si / mu;
        double reserr = sqrt( pow(1.*dsi/mu, 2) + pow(1.*si/mu/mu*dmu, 2) );
        if (std::isnan(res) || std::isnan(reserr) || ProjZ_1GeV->GetBinContent(d,e)==0) {res=0; reserr=0;}

        _FitBoth_Res_1GeV[i]->SetBinContent(d,e,res);
        _FitBoth_Res_1GeV[i]->SetBinError(d,e,reserr);
      }
    }
    _FitBoth_Res_1GeV[i]->Fit(powla2,"Q");

    for (int j=i+1; j<_nPart; j++)  // Separation Power
    {
      TH1* FitHist2_ampl  = (TH1D*)_FitHist[j]->At(1);
      TH1* FitHist2_sigma = (TH1D*)_FitHist[j]->At(2);
      for (int n=1; n<_nBinsX+1; n++)  // 0 is underflow bin, nBinsx+1 overflow bin
      {
        // get mean and sigma as well as their errors to calculate sep power
        double mu1 = FitHist1_ampl->GetBinContent(n);
        double mu2 = FitHist2_ampl->GetBinContent(n);
        double s1 = FitHist1_sigma->GetBinContent(n);
        double s2 = FitHist2_sigma->GetBinContent(n);
        double mu1e = FitHist1_ampl->GetBinError(n);
        double mu2e = FitHist2_ampl->GetBinError(n);
        double s1e = FitHist1_sigma->GetBinError(n);
        double s2e = FitHist2_sigma->GetBinError(n);
        streamlog_out(DEBUG) << mu1 << " " <<  mu1e << " " << mu2 << " " << mu2e << " " << s1 << " " << s1e << " " << s2 << " " << s2e << std::endl;

        // sep power = abs(mu1 - mu2) / sqrt( .5*( sigma1^2 + sigma2^2 ) )
        // make sure denominator is not 0; error calculated following simple error propagation
        double dm = fabs( mu1 - mu2 );
        double ds = pow(s1,2) + pow(s1,2);
        double sds = sqrt(.5*ds);
        double sp = (sds==0) ? 0 : dm/sds;  // separation power
        double spe_1 = 2*pow(mu1e,2)/ds;
        double spe_2 = 2*pow(mu2e,2)/ds;
        double spe_3 = 2*pow(dm,2)*pow(s1,2)*pow(s1e,2)/pow(ds,3);
        double spe_4 = 2*pow(dm,2)*pow(s2,2)*pow(s2e,2)/pow(ds,3);
        double spe = (ds==0) ? 0 : sqrt( spe_1 + spe_2 + spe_3 + spe_4 );  // separation power error
        streamlog_out(DEBUG) << "  " << sp << " " <<  sqrt(spe_1) << " " << sqrt(spe_2) << " " << sqrt(spe_3) << " " << sqrt(spe_4) << " " << spe << std::endl;

        _SPHist[sp_c]->SetBinContent(n,sp);
        _SPHist[sp_c]->SetBinError(n,spe);
      }
      sp_c++;
    }
  }

  _ResElectrons->Fit("gaus","Q");

  for (int i=0; i<_nPart+1; i++)
  {
    double vsum = 0, hsum = 0;
    for (int j=0; j<_nPart+1; j++)
    {
      vsum += _PDGCheck->GetBinContent(i,j);
      hsum += _PDGCheck->GetBinContent(j,i);
    }
    for (int j=0; j<_nPart+1; j++)
    {
      double eff_ij = (vsum==0) ? 0 : _PDGCheck->GetBinContent(i,j)*1./vsum;
      double pur_ji = (hsum==0) ? 0 : _PDGCheck->GetBinContent(j,i)*1./hsum;
      _PDGCheck_eff->SetBinContent(i,j,eff_ij);
      _PDGCheck_pur->SetBinContent(j,i,pur_ji);
    }
  }


  // plot stuff
  if (_plotStuff)
  {
    TCanvas* can = new TCanvas;
    TImage* img = TImage::Create();
    gStyle->SetPalette(kBird);
    can->SetGrid();
    can->SetLogx();

    //if (_useLCTPCStyle) LCTPCStyle();

    std::vector<double> YMax = {1000,_maxBinY,_maxBinY/10.,10};  // y-axis maxima of dE/dx fit result histogram plots
    std::vector<double> YMaxn= {1000,2,.2,10};  // y-axis maxima of normalised fit result histogram plots

    for (int i=0; i<_nPart; i++)
    {
      PlotTH2(can, img, _BBHist[i]);
      _FitHist_Res[i]->SetAxisRange(0,.2,"Y");
      PlotTH1(can, img, _FitHist_Res[i]);

      for (int j=0; j<4; j++)
      {
        ((TH1D*) _FitHist[i]->At(j))->SetAxisRange(0,YMax[j],"Y");
        ((TH1D*) _FitHist[i]->At(j))->Draw();
        img->FromPad(can);
        std::stringstream s2; s2 << _BBHist[i]->GetName() << "_FitPar" << j << _fileFormat;
        img->WriteImage(s2.str().c_str());
      }
    }

    PlotTH2(can, img, _BBHist[_nPart]);
    PlotTH2(can, img, _BBHist[_nPart+1]);

    for (int i=0; i<sp_c; i++)
    {
      _SPHist[i]->SetAxisRange(0,20,"Y");
      PlotTH1(can, img, _SPHist[i]);
    }

    PlotTH1(can, img, _TrackMomHist);


    // linear x-axis plots
    can->SetLogx(0);

    PlotTH1(can, img, _ResElectrons);

    for (int i=0; i<_nPart; i++)
    {
      PlotTH1(can, img, _FitHist_ResSum[i]);

      gStyle->SetOptFit(1);
      PlotTH1(can, img, _ResNormHist[i]);
      PlotTH1(can, img, _ResNormHist_1GeV[i]);
      gStyle->SetOptFit(0);

      PlotTH2(can, img, _NormLambdaHist[i]);
      _FitLambda_Res[i]->SetAxisRange(0,.2,"Y");
      PlotTH1(can, img, _FitLambda_Res[i]);
//      for (int j=0; j<4; j++)
//      {
//        ((TH1D*) _FitLambda[i]->At(j))->SetAxisRange(0,YMaxn[j],"Y");
//        ((TH1D*) _FitLambda[i]->At(j))->Draw();
//        img->FromPad(can);
//        std::stringstream s3d; s3d << _NormLambdaHist[i]->GetName() << "_FitPar" << j << _fileFormat;
//        img->WriteImage(s3d.str().c_str());
//      }

      PlotTH2(can, img, _NormNHitHist[i]);
      _FitNHit_Res[i]->SetAxisRange(0,.2,"Y");
      PlotTH1(can, img, _FitNHit_Res[i]);
//      for (int j=0; j<4; j++)
//      {
//        ((TH1D*) _FitNHit[i]->At(j))->SetAxisRange(0,YMaxn[j],"Y");
//        ((TH1D*) _FitNHit[i]->At(j))->Draw();
//        img->FromPad(can);
//        std::stringstream s3g; s3g << _NormNHitHist[i]->GetName() << "_FitPar" << j << _fileFormat;
//        img->WriteImage(s3g.str().c_str());
//      }

      _FitBoth_Res[i]->SetAxisRange(0,.2,"Z");
      PlotTH2(can, img, _FitBoth_Res[i]);
      _FitBoth_Res_1GeV[i]->SetAxisRange(0,.2,"Z");
      PlotTH2(can, img, _FitBoth_Res_1GeV[i]);
    }

    PlotTH1(can, img, _CutAnaHist);

    PlotTH2(can, img, _dEdxErrorCorrelation);

    PlotTH1(can, img, _dEdxOutlierAbundance);

    _dEdxOutlierProportion->SetBinContent(1,_dEdxOutlierAbundance->GetBinContent(1)/_BBHist[2]->GetEntries());
    _dEdxOutlierProportion->SetBinContent(2,_dEdxOutlierAbundance->GetBinContent(2)/_BBHist[3]->GetEntries());
    _dEdxOutlierProportion->SetBinContent(3,_dEdxOutlierAbundance->GetBinContent(3)/_BBHist[3]->GetEntries());
    _dEdxOutlierProportion->SetBinContent(4,_dEdxOutlierAbundance->GetBinContent(4)/_BBHist[4]->GetEntries());
    PlotTH1(can, img, _dEdxOutlierProportion);

    PlotTH1(can, img, _NTracksHist);
    PlotTH1(can, img, _NTracksUsedHist);
    PlotTH1(can, img, _NTracksMomHist);

    can->SetLogz();
    can->SetGrid(0,0);
    gStyle->SetOptStat(0);
    PlotTH2(can, img, _PDGCheck);
    PlotTH2(can, img, _PDGCheck_weight);
    PlotTH2(can, img, _PDGCheck_eff);
    PlotTH2(can, img, _PDGCheck_pur);


    // some final output
    streamlog_out(DEBUG) << "CutAnaHist Entries" << std::endl;
    for (unsigned int j=0; j<10; j++) streamlog_out(DEBUG) << j << "  " << _CutAnaHist->GetBinContent(j) << std::endl;

    streamlog_out(DEBUG) << "NTracksHist Entries" << std::endl;
    for (unsigned int j=0; j<10; j++) streamlog_out(DEBUG) << j << "  " << _NTracksHist->GetBinContent(j) << std::endl;

    streamlog_out(DEBUG) << "NTracksMomHist Entries" << std::endl;
    for (unsigned int j=0; j<10; j++) streamlog_out(DEBUG) << j << "  " << _NTracksMomHist->GetBinContent(j) << std::endl;

    streamlog_out(DEBUG) << "TrackMomHist Entries" << std::endl;
    for (int j=0; j<_nBinsX; j++) streamlog_out(DEBUG) << j << "  " << histbinsX2[j] << "  " << _TrackMomHist->GetBinContent(j) << std::endl;

    streamlog_out(DEBUG) << "MCmomVec Entries" << std::endl;
    for (unsigned int j=0; j<_MCmomVec.size()/3; j++) streamlog_out(DEBUG) << j << "  " << _MCmomVec[3*j] << "  " << _MCmomVec[3*j+1] << "  " << _MCmomVec[3*j+2] << std::endl;

    delete can;
    delete img;
  }

  delete gausfit;
  delete gausfitrel;
  delete powla2;

  delete[] histbinsX;
  delete[] histbinsX2;
}

void dEdxAnalyser::PlotTH1( TCanvas* can, TImage* img, TH1* hist )
{
  hist->Draw();
  img->FromPad(can);
  std::stringstream s; s << hist->GetName() << _fileFormat;
  img->WriteImage(s.str().c_str());
}

void dEdxAnalyser::PlotTH2( TCanvas* can, TImage* img, TH2* hist )
{
  hist->Draw("colz");
  img->FromPad(can);
  std::stringstream s; s << hist->GetName() << _fileFormat;
  img->WriteImage(s.str().c_str());
}
