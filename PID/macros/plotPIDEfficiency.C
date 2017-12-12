#include <vector>
#include <sstream>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TString.h"

#include "PIDHelper.h"

using namespace std;

void make_plots(const char* filename, const char* pidName, int pdgCode, const TString& pdfFile ) ;

/// plot the PID efficiency and fake rate for a given particle type for various PID algorithms as function of momentum and polar angle
void plotPIDEfficiency(const char* filename) {

  TString pdfFile = getPathPrefix( filename ) ;
  pdfFile += "PID_efficiencies_all.pdf" ;

  make_plots( filename, "basicPDG", 11  , pdfFile+"(" );  // first plot
  make_plots( filename, "basicPDG", 13  , pdfFile );
  make_plots( filename, "basicPDG", 211 , pdfFile );
  make_plots( filename, "basicPDG", 321 , pdfFile );
  make_plots( filename, "basicPDG", 2212, pdfFile );

  make_plots( filename, "dEdxPDG", 11  , pdfFile );
  make_plots( filename, "dEdxPDG", 13  , pdfFile );
  make_plots( filename, "dEdxPDG", 211 , pdfFile );
  make_plots( filename, "dEdxPDG", 321 , pdfFile );
  make_plots( filename, "dEdxPDG", 2212, pdfFile );
  
  make_plots( filename, "showerPDG", 11  , pdfFile );
  make_plots( filename, "showerPDG", 13  , pdfFile );
  make_plots( filename, "showerPDG", 211 , pdfFile );
  make_plots( filename, "showerPDG", 321 , pdfFile );
  make_plots( filename, "showerPDG", 2212, pdfFile );
  
  make_plots( filename, "likeliPDG", 11  , pdfFile );
  make_plots( filename, "likeliPDG", 13  , pdfFile );
  make_plots( filename, "likeliPDG", 211 , pdfFile );
  make_plots( filename, "likeliPDG", 321 , pdfFile );
  make_plots( filename, "likeliPDG", 2212, pdfFile );   
  
  make_plots( filename, "lowmomPDG", 11  , pdfFile );
  make_plots( filename, "lowmomPDG", 13  , pdfFile );
  make_plots( filename, "lowmomPDG", 211 , pdfFile );
  make_plots( filename, "lowmomPDG", 321 , pdfFile );
  make_plots( filename, "lowmomPDG", 2212, pdfFile+")" ); // last plot
}


void make_plots(const char* filename, const char* pidName, int pdgCode, const TString& pdfFile) {

  std::string pName("unknown") ;
  switch( pdgCode ){
  case 11:
    pName = "electrons" ;
    break ;
  case 13:
    pName = "muons" ;
    break ;
  case 211:
    pName = "pions" ;
    break ;
  case 321:
    pName = "kaons" ;
    break ;
  case 2212:
    pName = "protons" ;
    break ;
  }


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


  // --- open the file -----

  TFile *treefile = new TFile(filename);
  TTree *tree = (TTree*) treefile->Get("hermTree");


  //************** create some histograms **************


   
   const int nBins = 9 ; //made edit to creat histograms- i.e. define variable
   double bins[nBins+1] = {  0.1, 0.5 , 1.0 , 2., 5.0 , 10. , 20. , 50. , 100. , 300.  } ;
   // const int nBins = 13 ; //made edit to creat histograms- i.e. define variable
   // double bins[nBins+1] = { 0.01, 0.1, 0.2, 0.3, 0.5 , 0.7 , 1.0 , 2., 5.0 , 10. , 20. , 50. , 100. , 300.  } ;




   TH1F* hist_p_t = new TH1F( "hist_p_t", "P of true particles", nBins , bins ) ;
   TH1F* hist_Sp_t = new TH1F( "hist_Sp_t", "P of observed particles", nBins , bins ) ;
   
   TH1F* hist_th_t  = new TH1F( "hist_th_t", "Cos theta of true particles", 11, -1., 1. ) ;
   TH1F* hist_Sth_t = new TH1F( "hist_Sth_t", "Cos theta of observed particles", 11, -1., 1. ) ;
   
   TH1F* hist_p_tnot = new TH1F("hist_p_tnot", "P of true particles which are not of the type in question", nBins, bins );
   TH1F* hist_th_tnot = new TH1F("hist_th_tnot", "Cos theta of true particles which are not of the type in question", 11, -1., 1. ) ;

   TH1F* hist_Failed_p = new TH1F( "hist_Failed_p", "Tagging of falsely identified particles (momentum)", nBins, bins );

   TH1F* hist_Failed_th = new TH1F( "hist_Failed_th", "Tagging of falsely identified particles (cos theta)", 11, -1., 1. );

  //**********************************************************************************************  
  // read branches
  int npart;
  vector<int>      *truePDG = new vector<int> ;
  vector<double>   *isSeen = new vector<double> ;
  vector<int>      *thePDG = new vector<int> ;
  vector<double>   *trueCharge = new vector<double> ;
  vector<double>   *seenCharge = new vector<double> ;
  vector<double>   *trueP = new vector<double> ;
  vector<double>   *trueTheta = new vector<double> ;
  vector<double>   *seenTheta = new vector<double> ;
  vector<double>   *seenP = new vector<double> ;            
  tree->SetBranchAddress("nMCParticles", &npart);        
  tree->SetBranchAddress("trueCharge", &trueCharge); 
  tree->SetBranchAddress("truePDG",    &truePDG);  
  tree->SetBranchAddress("isSeen",     &isSeen);  
  tree->SetBranchAddress("seenCharge", &seenCharge); 

  tree->SetBranchAddress(pidName,  &thePDG); 

  tree->SetBranchAddress("trueP", &trueP);
  tree->SetBranchAddress("trueTheta", &trueTheta);
  tree->SetBranchAddress("seenTheta", &seenTheta);
  tree->SetBranchAddress("seenP", &seenP);
   


  // ************************* loop over events and particles in tree *****************************
  int nevts = tree->GetEntries();
  
  for (int ievt = 0; ievt < nevts; ++ievt) {
    
    tree->GetEntry(ievt);
    
    for (int ipart = 0; ipart < npart; ipart++) {  
      
      if ( isSeen->at(ipart ) < 0.5 ) continue ;// require MC-truth link weight to be > 0.5
      
      if( seenCharge->at( ipart ) == 0 ) continue; // only look at charged
      
      if( std::abs( truePDG->at( ipart ) ) != pdgCode ) continue ;
      
      hist_p_t->Fill( trueP->at(ipart) ) ;
      
      hist_th_t->Fill( cos( trueTheta->at(ipart) ) ) ;
      
      if( std::abs( thePDG->at(ipart)) != pdgCode ) continue;
      
      hist_Sp_t->Fill( trueP->at(ipart) );
      
      hist_Sth_t->Fill( cos( trueTheta->at(ipart)));
      
    }
    
    for (int ipart = 0; ipart < npart; ipart++) {  
      
      if ( isSeen->at(ipart ) < 0.5 ) continue ;// require MC-truth link weight to be > 0.5
      
      if( seenCharge->at( ipart ) == 0 ) continue; // only look at charged
      
      if( std::abs( truePDG->at( ipart ) ) == pdgCode ) continue ;
      
      hist_p_tnot->Fill( trueP->at(ipart) ) ;
      
      hist_th_tnot ->Fill( cos( trueTheta->at(ipart) ) ) ;
      
      if( std::abs( thePDG->at(ipart)) != pdgCode  ) continue;
      
      hist_Failed_p->Fill( trueP->at(ipart) );
      
      hist_Failed_th->Fill( cos( trueTheta->at(ipart)));
      
    }

  }
  
  
  TCanvas* c  = new TCanvas( " PID efficiency" , "Efficiency" ,750 ,750);
  c->Divide(2,3) ;
  
  c->cd(1) ;
  gPad->SetLogx() ;
  
  
  hist_p_t->GetXaxis()->SetTitle("true P/GeV" );
  hist_p_t->GetXaxis()->SetTitleSize( 0.04 ) ;
  hist_p_t->GetXaxis()->SetTitleOffset( 1.2 );
  
  hist_p_t->Draw() ;
  
  c->cd(2) ;
  
  hist_th_t->GetXaxis()->SetTitle( "true cos(#theta)" );
  hist_th_t->GetXaxis()->SetTitleSize( 0.04 ) ;
  hist_th_t->GetXaxis()->SetTitleOffset( 1.3 );
  hist_th_t->Draw() ;
  
  

   c->cd(3) ;
   gPad->SetLogx() ;
   hist_Sp_t->Draw() ;

   hist_p_t->GetXaxis()->SetTitle( "true P/GeV" );
   hist_p_t->GetXaxis()->SetTitleSize( 0.04 ) ;
   hist_p_t->GetXaxis()->SetTitleOffset( 1.2 );

  

   c->cd(4) ;
   hist_Sth_t->Draw() ;

   hist_Sth_t->GetXaxis()->SetTitle("true cos(#theta)" );
   hist_Sth_t->GetXaxis()->SetTitleSize( 0.04 ) ;
   hist_Sth_t->GetXaxis()->SetTitleOffset( 1.2 );

   
   c->cd(5);
   //hist_p_t.Sumw2();
   //hist_Sp_t.Sumw2();
   
   TGraphAsymmErrors* hist_eff_p = new TGraphAsymmErrors ;
   hist_eff_p->Divide(hist_Sp_t, hist_p_t, "v");
   
   hist_eff_p->Draw();
   
   gPad->SetLogx() ;
   hist_eff_p->SetName(" hist_eff_p" ) ;
   
   //histo->SetTitle(";x "#epsilon";y"true P/GeV"");
   
   hist_eff_p->GetYaxis()->SetTitle( "#epsilon_{PID}" );
   hist_eff_p->GetYaxis()->SetTitleSize( 0.06 ) ;
   hist_eff_p->GetYaxis()->SetTitleOffset( 0.5 );
   
   hist_eff_p->GetXaxis()->SetTitle("true P/GeV" );
   hist_eff_p->GetXaxis()->SetTitleSize( 0.06 ) ;
   hist_eff_p->GetXaxis()->SetTitleOffset( 1.2 );


   c->cd(6);
   //hist_th_t.Sumw2();
   //hist_Sth_t.Sumw2();
   
   TGraphAsymmErrors* hist_eff_th = new TGraphAsymmErrors ;
   hist_eff_th->Divide(hist_Sth_t, hist_th_t, "v");
   hist_eff_th->Draw();
   hist_eff_th->SetName(" hist_eff_th" ) ;
   
   
   hist_eff_th->GetYaxis()->SetTitle( "#epsilon_{PID}" );
   hist_eff_th->GetYaxis()->SetTitleSize( 0.06 ) ;
   hist_eff_th->GetYaxis()->SetTitleOffset( 0.5 );

   hist_eff_th->GetXaxis()->SetTitle( "true cos(#theta)" );
   hist_eff_th->GetXaxis()->SetTitleSize( 0.06 ) ;
   hist_eff_th->GetXaxis()->SetTitleOffset( 0.9 );
  
  
   TString outfile = getPathPrefix( filename ) ;
   outfile += "PID_efficiencies_pdg";
   outfile +=  pdgCode ;
   //c->Print(TString(outfile+".pdf"));
   
   TCanvas* k  = new TCanvas( " PID Failed " , " PID Failed " ,750 ,750);
   k->Divide(2,3) ;

   k->cd(1);
   gPad->SetLogx() ;
  
   hist_p_tnot->GetXaxis()->SetTitle("true P/GeV" );
   hist_p_tnot->GetXaxis()->SetTitleSize( 0.04 ) ;
   hist_p_tnot->GetXaxis()->SetTitleOffset( 1.2 );
   hist_p_tnot->Draw();

   k->cd(2);
   hist_th_tnot->GetXaxis()->SetTitle( "true cos(#theta)" );
   hist_th_tnot->GetXaxis()->SetTitleSize( 0.04 ) ;
   hist_th_tnot->GetXaxis()->SetTitleOffset( 1.2 );
   hist_th_tnot->Draw();

   k->cd(3);
   gPad->SetLogx() ;
   hist_Failed_p->GetXaxis()->SetTitle( "true P/GeV" );
   hist_Failed_p->GetXaxis()->SetTitleSize( 0.04 ) ;
   hist_Failed_p->GetXaxis()->SetTitleOffset( 1.2 );
   hist_Failed_p->Draw();

   k->cd(4);
   hist_Failed_th->GetXaxis()->SetTitle("true cos(#theta)" );
   hist_Failed_th->GetXaxis()->SetTitleSize( 0.04 ) ;
   hist_Failed_th->GetXaxis()->SetTitleOffset( 1.2 );
   hist_Failed_th->Draw();

   k->cd(5);
   
   TGraphAsymmErrors* hist_FailRate_p = new TGraphAsymmErrors ;
   hist_FailRate_p->Divide( hist_Failed_p, hist_p_tnot, "v" );
   gPad->SetLogx() ;
   
   hist_FailRate_p->GetYaxis()->SetTitle( "Fake Rate" );
   hist_FailRate_p->GetYaxis()->SetTitleSize( 0.06 ) ;
   hist_FailRate_p->GetYaxis()->SetTitleOffset( 1. );
   
   hist_FailRate_p->GetXaxis()->SetTitle("true P/GeV" );
   hist_FailRate_p->GetXaxis()->SetTitleSize( 0.06 ) ;
   hist_FailRate_p->GetXaxis()->SetTitleOffset( 1.2 );

   hist_FailRate_p->Draw();
   hist_FailRate_p->SetName(" hist_FailRate_p" ) ;

   k->cd(6);
   
   TGraphAsymmErrors* hist_FailRate_th = new TGraphAsymmErrors ;
  
   hist_FailRate_th->Divide( hist_Failed_th, hist_th_tnot, "v" );
   
   hist_FailRate_th->GetYaxis()->SetTitle( "Fake Rate" );
   hist_FailRate_th->GetYaxis()->SetTitleSize( 0.06 ) ;
   hist_FailRate_th->GetYaxis()->SetTitleOffset( 1. );

   hist_FailRate_th->GetXaxis()->SetTitle( "true cos(#theta)" );
   hist_FailRate_th->GetXaxis()->SetTitleSize( 0.06 ) ;
   hist_FailRate_th->GetXaxis()->SetTitleOffset( 0.9 );
   
   hist_FailRate_th->Draw();
 
   hist_FailRate_th->SetName(" hist_FailRate_th" ) ;
   
   outfile =  getPathPrefix( filename ) ;
   outfile += "PID_Failed_pdg";
   outfile +=  pdgCode ;
   // k->Print(TString(outfile+".pdf"));

   // ------ draw them both in one plot --------------------

   /// create an empty histogram to set the axes range:

   float minEff = 0. ;
   float maxEff = 1.01 ;
   float minpt = 0.1 ;
   float maxpt = 100. ;
   float minct = -1. ;
   float maxct = 1. ;
   
   std::stringstream pTitleS ;
   pTitleS << "PID efficiency/fake rate vs P [" << pidName << "] - " << pName ;
   std::stringstream cTitleS ;
   cTitleS << "PID efficiency/fake rate vs cos(theta) [" << pidName << "] - " << pName ;

   TH2F* hpt = new TH2F( "hpt", pTitleS.str().c_str() ,10, minpt, maxpt, 10, minEff , maxEff  ) ;
   TH2F* hth = new TH2F( "hth", cTitleS.str().c_str() , 10, minct, maxct, 10, minEff , maxEff  ) ;


   TCanvas* c2  = new TCanvas( "c2" , "Efficiency and Fake Rate" ,750 ,750);

   c2->Divide(1,2) ;
   ildStyle->SetOptStat(0000);
   c2->cd(1) ;
   gPad->SetLogx() ;
    hpt->Draw() ;

   hpt->GetYaxis()->SetTitle( "#epsilon_{PID}" );
   hpt->GetYaxis()->SetTitleSize( 0.06 ) ;
   hpt->GetYaxis()->SetTitleOffset( 0.5 );
   
   hpt->GetXaxis()->SetTitle("true P/GeV" );
   hpt->GetXaxis()->SetTitleSize( 0.06 ) ;
   hpt->GetXaxis()->SetTitleOffset( 1.2 );
   
   hpt->GetEntries();

   hist_eff_p->SetMarkerStyle(  kOpenTriangleUp ) ; // kDot) ; //kOpenCircle) ;
   hist_eff_p->SetMarkerColor( kBlue) ;
   hist_eff_p->SetLineColor( kBlue ) ;
   hist_eff_p->Draw("P") ;

   hist_FailRate_p->SetMarkerStyle( kOpenCircle) ;
   hist_FailRate_p->SetMarkerColor( kRed) ;
   hist_FailRate_p->SetLineColor( kRed ) ;
   hist_FailRate_p->Draw("P") ;

   // TLegend* leg = new TLegend(0.6, 0.3, .9, .5);
   // leg->AddEntry( "hist_eff_p" ,  "efficiency" , "lep") ;
   // leg->AddEntry( "hist_FailRate_p"   ,  "fake rate" , "lep") ;
   // leg->Draw();



   // -------------

   c2->cd(2) ;
   hth->Draw() ;
  
   hth->GetYaxis()->SetTitle( "#epsilon_{PID}" );
   hth->GetYaxis()->SetTitleSize( 0.06 ) ;
   hth->GetYaxis()->SetTitleOffset( 0.5 );

   hth->GetXaxis()->SetTitle( "true cos(#theta)" );
   hth->GetXaxis()->SetTitleSize( 0.06 ) ;
   hth->GetXaxis()->SetTitleOffset( 0.9 );

   hth->GetEntries();
   hist_eff_th->SetMarkerStyle(  kDot) ; //kOpenCircle) ;
   hist_eff_th->SetMarkerColor( kBlue) ;
   hist_eff_th->SetLineColor( kBlue ) ;
   hist_eff_th->Draw("P") ;

   
   hist_FailRate_th->SetMarkerStyle(  kDot) ; //kOpenCircle) ;
   hist_FailRate_th->SetMarkerColor( kRed) ;
   hist_FailRate_th->SetLineColor( kRed ) ;
   hist_FailRate_th->Draw("P") ;
   hist_FailRate_th->Draw("P") ;   

   outfile =  getPathPrefix( filename ) ;
   outfile += "PID_efficiency_";
   outfile += pidName ;
   outfile += "_pdg";
   outfile +=  pdgCode ;


   TString fName = pdfFile.Length() > 0 ? pdfFile : TString(outfile+".pdf") ;

   c2->Print( fName );



   // cleanup...

   delete truePDG ;
   delete isSeen ;
   delete thePDG ;
   delete trueCharge ;
   delete seenCharge ;
   delete trueP ;
   delete trueTheta ;
   delete seenTheta ;
   delete seenP ;

   delete hist_eff_p ;
   delete hist_eff_th ;
   delete k ;
   delete hist_FailRate_p ;
   delete hist_FailRate_th ;
   delete hpt ;
   delete hth ;
   delete c ;
   delete c2 ;

   treefile->Close() ;

   return;
  
}
