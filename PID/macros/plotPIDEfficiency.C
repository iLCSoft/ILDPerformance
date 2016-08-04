#include <vector>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TString.h"

using namespace std;


void plotPIDEfficiency(const char* _filename, int pdgCode) {

  // ----- define some variables defining what to plot -----
  //int pdgCode = 13 ;  // muon
  //int pdgCode = 211 ;  // pion
  //int pdgCode = 11 ; //electron
  //int pdgCode = 321 ; //kaon
  //int pdgCode = 2212 ; //protons

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

  TFile *treefile = new TFile(_filename);
  TTree *tree = (TTree*) treefile->Get("hermTree");


  //************** create some histograms **************


   
   const int nBins = 9 ; //made edit to creat histograms- i.e. define variable
   double bins[nBins+1] = {  0.1, 0.5 , 1.0 , 2., 5.0 , 10. , 20. , 50. , 100. , 300.  } ;
   // const int nBins = 13 ; //made edit to creat histograms- i.e. define variable
   // double bins[nBins+1] = { 0.01, 0.1, 0.2, 0.3, 0.5 , 0.7 , 1.0 , 2., 5.0 , 10. , 20. , 50. , 100. , 300.  } ;




   TH1F* hist_p_t = new TH1F( "hist_p_t", "P of true muons", nBins , bins ) ;
   TH1F* hist_Sp_t = new TH1F( "hist_Sp_t", "P of observed muons", nBins , bins ) ;
   
   TH1F* hist_th_t  = new TH1F( "hist_th_t", "Cos theta of true muons", 11, -1., 1. ) ;
   TH1F* hist_Sth_t = new TH1F( "hist_Sth_t", "Cos theta of observed muons", 11, -1., 1. ) ;
   
   TH1F* hist_p_tnot = new TH1F("hist_p_tnot", "P of true particles which are not of the type in question", nBins, bins );
   TH1F* hist_th_tnot = new TH1F("hist_th_tnot", "Cos theta of true particles which are not of the type in question", 11, -1., 1. ) ;

   TH1F* hist_Failed_p = new TH1F( "hist_Failed_p", "Tagging of falsely identified particles (momentum)", nBins, bins );

   TH1F* hist_Failed_th = new TH1F( "hist_Failed_th", "Tagging of falsely identified particles (cos theta)", 11, -1., 1. );

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
   

  //cout << "after branch addressing "  << endl;

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
      
      if( std::abs( likeliPDG->at(ipart)) != pdgCode ) continue;
      
      hist_Sp_t->Fill( trueP->at(ipart) );
      
      hist_Sth_t->Fill( cos( trueTheta->at(ipart)));
      
    }
    
    for (int ipart = 0; ipart < npart; ipart++) {  
      
      if ( isSeen->at(ipart ) < 0.5 ) continue ;// require MC-truth link weight to be > 0.5
      
      if( seenCharge->at( ipart ) == 0 ) continue; // only look at charged
      
      if( std::abs( truePDG->at( ipart ) ) == pdgCode ) continue ;
      
      hist_p_tnot->Fill( trueP->at(ipart) ) ;
      
      hist_th_tnot ->Fill( cos( trueTheta->at(ipart) ) ) ;
      
      if( std::abs( likeliPDG->at(ipart)) != pdgCode  ) continue;
      
      hist_Failed_p->Fill( trueP->at(ipart) );
      
      hist_Failed_th->Fill( cos( trueTheta->at(ipart)));
      
    }



   

    // for (int ipart = 0; ipart < npart; ipart++){

    //   if ( std::abs( truePDG->at( ipart ) ) != pdgCode && 
    // 	   std::abs(likeliPDG->at(ipart)) == pdgCode ){

    // 	hist_Failed_p->Fill( trueP->at(ipart) ) ;
	
    // 	hist_Failed_th->Fill( cos( trueTheta->at(ipart) ) ); 
    //   }
    //   else continue;
    // }	


}
 

  // **********   now plot the histotgrams *********************************************

  //TCanvas* c  = new TCanvas( " PID eff." , " xxx "  ,750,750);
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
  
  
   TString outfile = "../Results/PID_efficiencies";
   outfile += "_pdg";
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
   
   TString outfile = "../Results/PID_Failed";
   outfile += "_pdg";
   outfile +=  pdgCode ;
   //   k->Print(TString(outfile+".pdf"));

   // ------ draw them both in one plot --------------------

   /// create an empty histogram to set the axes range:

   float minEff = 0. ;
   float maxEff = 1.01 ;
   float minpt = 0.1 ;
   float maxpt = 100. ;
   float minct = -1. ;
   float maxct = 1. ;
   TH2F* hpt = new TH2F( "hpt", "Efficiency/Fake rate against P" ,10, minpt, maxpt, 10, minEff , maxEff  ) ;
   TH2F* hth = new TH2F( "hth", "Efficiency/Fake rate against cos Theta" , 10, minct, maxct, 10, minEff , maxEff  ) ;


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

 TString outfile = "../Results/Eff_FakeRate";
   outfile += "_pdg";
   outfile +=  pdgCode ;
   c2->Print(TString(outfile+".pdf"));

   return;
  
}
