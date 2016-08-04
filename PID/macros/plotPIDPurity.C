
#include <vector>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TString.h"

using namespace std;


void plotPIDPurity(const char* _filename) {
  
  const int nBins = 100 ; 

  int trueCounter[5] = {0, 0, 0, 0, 0};
  int recoCounter[5] ={0, 0, 0, 0, 0};
  int trueandrecoCounter[5] = {0, 0, 0, 0, 0};
  int BinCentre_ht[nBins];
  int BinContent_ht[nBins];
  int BinCentre_hf[nBins];
  int BinContent_hf[nBins];
  int CutLikelihood;

  gROOT->SetStyle("ildStyle");
  ildStyle->SetOptTitle(1);
  ildStyle->SetHistLineWidth(2);
  ildStyle->SetLineWidth(2);
  ildStyle->SetCanvasBorderMode(0);
  ildStyle->SetPadBorderMode(0);
  ildStyle->SetPadColor(0);
  ildStyle->SetCanvasColor(0);
  ildStyle->SetTitleColor(1);
  ildStyle->SetStatColor(0);
//  ildStyle->SetOptStat(1110010);
  ildStyle->SetOptStat(10);
  //ildStyle->SetOptFit(10);
  ildStyle->SetPadBottomMargin(0.15); 

  ildStyle->SetPadTopMargin(0.10);
  ildStyle->SetPadRightMargin(0.025);
  ildStyle->SetPadLeftMargin(0.2); 
  ildStyle->SetMarkerStyle(20);
  ildStyle->SetMarkerSize(0.625);

  // ----- define some variables defining what to plot -----
  // int pdgCode = 13 ;  // muon
  //int pdgCode = 211 ;  // pion
  //int pdgCode = 11 ; //electron
  //int pdgCode = 321 ; //kaon
  //int pdgCode = 2212 ; //protons


  int pdgCode[5] = {11, 13, 211, 321, 2212};
  TString particleName[5] = {"electrons", "muons", "pions", "kaons", "protons"};
  TString likelihoodName[5] = {"LiPDG_el", "LiPDG_mu", "LiPDG_pi", "LiPDG_ka", "LiPDG_pr" } ;
  TH1F* ht[5] ; // true particles 
  TH1F* hf[5] ; // false particles
  TGraph* h[5] ;

  // --- open the file -----

  TFile *treefile = new TFile(_filename);
  TTree *tree = (TTree*) treefile->Get("hermTree");


  //************** create some histograms **************
  // const int nBins = 100;
  // const int maxX = 1;
  // const int maxY = 1;
  // TH2F* h1 = new TH2F("h1", "Electron likelyhood", nBins, 0, maxX, 1000, 0, maxY);
  // TH2F* h2 = new TH2F("h2", "Muon likelyhood", nBins, 0, maxX, 1000, 0, maxY);
  // TH2F* h3 = new TH2F("h3", "Pion likelyhood", nBins, 0, maxX, 1000, 0, maxY);
  // TH2F* h4 = new TH2F("h4", "Kaon likelyhood", nBins, 0, maxX, 1000, 0, maxY);
  // TH2F* h5 = new TH2F("h5", "Proton likelyhood", nBins, 0, maxX, 1000, 0, maxY);
  
  
  const double maxX  = 0. ;
  const double minX  = -200;
  
  for (int i = 0; i < 5; ++i) {

    TString hname = "ht_" + particleName[i] ;
    TString htitle = " Likely PDG true " +  particleName[i] ; 
    ht[i] = new TH1F( hname , htitle, nBins ,minX , maxX ) ;

    hname = "hf_" + particleName[i] ;
    htitle = " Likely PDG false " +  particleName[i] ; 
    hf[i] = new TH1F( hname , htitle, nBins ,minX , maxX ) ;

 
    //**********************************************************************************************  
    // read branches
    int npart;
    vector<int> *truePDG;
    vector<double> *isSeen;
    vector<int> *basicPDG;
    vector<int> *dEdxPDG;
    vector<int> *showerPDG;
    vector<int> *likeliPDG;
    vector<int> *lowmomPDG;
    vector<double> *trueCharge;
    vector<double> *seenCharge;
    vector<double> *trueP;
    vector<double> * trueTheta;
    vector<double> * seenTheta;
    vector<double> * seenP;
    vector<double> * seenDEdx;

    vector<double> * likelihood ;

    tree->SetBranchAddress("nMCParticles", &npart);        
    tree->SetBranchAddress("trueCharge", &trueCharge); 
    tree->SetBranchAddress("truePDG",    &truePDG);  
    tree->SetBranchAddress("isSeen",     &isSeen);  
    tree->SetBranchAddress("seenCharge", &seenCharge); 
    tree->SetBranchAddress("basicPDG",   &basicPDG);  
    tree->SetBranchAddress("dEdxPDG",    &dEdxPDG);  
    tree->SetBranchAddress("showerPDG",  &showerPDG);  
    tree->SetBranchAddress("likeliPDG",  &likeliPDG);  
    tree->SetBranchAddress("lowmomPDG",  &lowmomPDG); 
    
    tree->SetBranchAddress("trueP", &trueP);
    tree->SetBranchAddress("trueTheta", &trueTheta);
    tree->SetBranchAddress("seenTheta", &seenTheta);
    tree->SetBranchAddress("seenP", &seenP);
    tree->SetBranchAddress("seenDEdx", &seenDEdx);
    
    tree->SetBranchAddress( likelihoodName[i] , &likelihood );
    
    //cout << "after branch addressing "  << endl;
    
    // ************************* loop over events and particles in tree *****************************
    
    
    int nevts = tree->GetEntries();
    
    // double Efficiency[5] ;
    // double Purity[5];
    
    for (int ievt = 0; ievt < nevts; ++ievt) {
      
      tree->GetEntry(ievt);
      
      for (int ipart = 0; ipart < npart; ipart++) {  
	
	if ( isSeen->at( ipart ) < 0.5 ) continue ;// require MC-truth link weight to be > 0.5
	
	if( seenCharge->at( ipart ) == 0 ) continue; // only look at charged
	
	if( trueP->at( ipart ) < 1.0 ) continue; // momentum > 1GeV
	
	if( seenDEdx->at( ipart ) < 0 ) continue;
	
	if( seenDEdx->at( ipart ) > 1.e-6) continue;
        
	if( std::abs( truePDG->at( ipart ) ) == pdgCode[i] ) { 
	  
	  ht[i]->Fill( likelihood->at( ipart ) );
	  
	} else {  
	  
	  hf[i]->Fill( likelihood->at( ipart ) ) ;
	}
      }

    } // ----- loop over events 

    for(int iBins=0; iBins<nBins; iBins++){
      
      BinCentre_ht[iBins]  = ht[i]->GetBinCenter (iBins)  ;
      BinContent_ht[iBins] = ht[i]->GetBinContent(iBins) ;
      BinCentre_hf[iBins]  = hf[i]->GetBinCenter (iBins)  ;
      BinContent_hf[iBins] = hf[i]->GetBinContent(iBins) ;
    }
    
    double eff[ nBins ] ;
    double pur[ nBins ] ;
    
    for(int iBins=0; iBins<nBins; iBins++){
      double sum_t_le_c = 0 ;  // sum of entries in true histogram that are below cut
      double sum_t_gt_c = 0 ;  // ...
      double sum_f_le_c = 0 ;
      double sum_f_gt_c = 0 ;
      
      for(int jBins=0; jBins<nBins; jBins++){
	if( jBins < iBins){
	  sum_t_le_c += BinContent_ht[jBins];
	  sum_f_le_c += BinContent_hf[jBins];
	} else {
	  sum_t_gt_c += BinContent_ht[jBins];
	  sum_f_gt_c += BinContent_hf[jBins];
	}
      }
      
      eff[ iBins ] =  sum_t_gt_c / (sum_t_gt_c + sum_t_le_c )  ;
      pur[ iBins ] =  sum_t_gt_c / (sum_t_gt_c + sum_f_gt_c )  ;
    }
      
      
    h[i]  = new TGraph( nBins , eff , pur );
      
    
  
    // std::cout << "trueCounter[i]=" << trueCounter[i] << endl;
    // std::cout << "recoCounter[i]=" << recoCounter[i] << endl;
    // std::cout << "trueandrecoCounter[i]=" << trueandrecoCounter[i] << endl;
    
    // Efficiency[i] = recoCounter[i]/ trueCounter[i];
    // Purity[i] =  trueandrecoCounter[i]/ recoCounter[i];
    
    // std::cout << "Efficiency[i]=" << Efficiency[i] << endl;
    // std::cout << "Purity[i]=" << Purity[i] << endl;
    
  } // ==============================  loop over PDG types

  TGraph*  h1 = h[0] ;
  TGraph*  h2 = h[1] ;
  TGraph*  h3 = h[2] ;
  TGraph*  h4 = h[3] ;
  TGraph*  h5 = h[4] ;

  // double Efficiency[5] = recoCounter/ trueCounter;
  //double Purity[5] = trueandrecoCounter/ recoCounter;

  // hermTree->Draw("Efficiency[1]: Purity[1]>>h1"," abs(truePDG) == 11 ") ;
  // hermTree->Draw("Efficiency[2]: Purity[2]>>h2"," abs(truePDG) == 13 ") ;
  // hermTree->Draw("Efficiency[3]: Purity[3]>>h3"," abs(truePDG) == 211 ") ;
  // hermTree->Draw("Efficiency[4]: Purity[5]>>h4"," abs(truePDG) == 321 ") ;
  // hermTree->Draw("Efficiency[6]: Purity[6]>>h5"," abs(truePDG) == 2212 ") ;
  // **********   now plot the histotgrams ********************************************

  //TCanvas* c  = new TCanvas( " PID eff." , " xxx "  ,750,750);
  TCanvas* c  = new TCanvas( " PID efficiency vs purity" , "Efficiency purity" ,750 ,750);
  c->Divide(2,3) ;
  
  c->cd(1) ;
   
  h1->SetTitle("Electrons Efficiency vs Purity");

  h1->GetXaxis()->SetTitle("Purity" );
  h1->GetXaxis()->SetTitleSize( 0.04 ) ;
  h1->GetXaxis()->SetTitleOffset( 1.2 );

  h1->GetYaxis()->SetTitle( "#epsilon_{PID}" );
  h1->GetYaxis()->SetTitleSize( 0.06 ) ;
  h1->GetYaxis()->SetTitleOffset( 0.5 );
   
  h1->Draw() ;

  c->cd(2) ;

  h2->SetTitle("Muons Efficiency vs Purity");

  h2->GetXaxis()->SetTitle("Purity" );
  h2->GetXaxis()->SetTitleSize( 0.04 ) ;
  h2->GetXaxis()->SetTitleOffset( 1.2 );

  h2->GetYaxis()->SetTitle( "#epsilon_{PID}" );
  h2->GetYaxis()->SetTitleSize( 0.06 ) ;
  h2->GetYaxis()->SetTitleOffset( 0.5 );

  h2->Draw() ;

  c->cd(3) ;
  h3->SetTitle("Pions Efficiency vs Purity");
  
  h3->GetXaxis()->SetTitle("Purity" );
  h3->GetXaxis()->SetTitleSize( 0.04 ) ;
  h3->GetXaxis()->SetTitleOffset( 1.2 );

  h3->GetYaxis()->SetTitle( "#epsilon_{PID}" );
  h3->GetYaxis()->SetTitleSize( 0.06 ) ;
  h3->GetYaxis()->SetTitleOffset( 0.5 );
   
  h3->Draw() ;

  c->cd(4) ;

  h4->SetTitle("Kaons Efficiency vs Purity");
  
  h4->GetXaxis()->SetTitle("Purity" );
  h4->GetXaxis()->SetTitleSize( 0.04 ) ;
  h4->GetXaxis()->SetTitleOffset( 1.2 );

  h4->GetYaxis()->SetTitle( "#epsilon_{PID}" );
  h4->GetYaxis()->SetTitleSize( 0.06 ) ;
  h4->GetYaxis()->SetTitleOffset( 0.5 );
   
  h4->Draw() ;
   
  c->cd(5);
  
  h5->SetTitle("Protons Efficiency vs Purity");
  
  h5->GetXaxis()->SetTitle("Purity" );
  h5->GetXaxis()->SetTitleSize( 0.04 ) ;
  h5->GetXaxis()->SetTitleOffset( 1.2 );

  h5->GetYaxis()->SetTitle( "#epsilon_{PID}" );
  h5->GetYaxis()->SetTitleSize( 0.06 ) ;
  h5->GetYaxis()->SetTitleOffset( 0.5 );
   
  h5->Draw() ;

  TString outfile = "../Results/Eff_Purity";
  c->Print(TString(outfile+".pdf"));

  return;
  
}
