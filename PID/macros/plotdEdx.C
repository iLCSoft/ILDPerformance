#include "TH1.h"
/** Very simply plotting of dEdx as a function of beta*gamma (p/m).
 *  Should add colors for standard PDG types ...
 * 
 */
void plotdEdx(const char* _filename ) {

  gROOT->SetStyle("ildStyle");
  // use small markers:
  ildStyle->SetMarkerStyle(1);

  TFile *treefile = new TFile(_filename);

  const int nBins = 10000 ;
  const double maxX  = 1e3 ;
  const double maxY  = 0.0000005 ;

  TH2F* h1 = new TH2F("h1","All Partilces",nBins, 0.3, maxX, 1000,0,maxY);
  TH2F* h2 = new TH2F("h2","Electrons",nBins, 0.3, maxX, 1000,0,maxY);
  TH2F* h3 = new TH2F("h3","muons",nBins, 0.3, maxX, 1000,0,maxY); 
  TH2F* h4 = new TH2F("h4","pions",nBins, 0.3, maxX, 1000,0,maxY);
  TH2F* h5 = new TH2F("h5","kaons",nBins, 0.3, maxX, 1000,0,maxY);
  TH2F* h6 = new TH2F("h6","protons",nBins, 0.3, maxX, 1000,0,maxY);

 
  h2->SetMarkerColor(kBlue);
  h3->SetMarkerColor(8);
  h4->SetMarkerColor(kRed);
  h5->SetMarkerColor(5);
  h6->SetMarkerColor(1);

  //  hermTree->Draw("seenDEdx:trueP/trueMass>>h1","abs(trueCharge)>0.5&&trueMass>0.&&seenDEdx>0.&&seenDEdx<1.e-6") ;
  hermTree->Draw("seenDEdx:trueP/trueMass>>h2","abs(trueCharge)>0.5&&isSeen>0.7&&trueMass>0.&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)==11") ;
  hermTree->Draw("seenDEdx:trueP/trueMass>>h3","abs(trueCharge)>0.5&&isSeen>0.7&&trueMass>0.&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)==13 ");
  hermTree->Draw("seenDEdx:trueP/trueMass>>h4","abs(trueCharge)>0.5&&isSeen>0.7&&trueMass>0.&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)==211");
  hermTree->Draw("seenDEdx:trueP/trueMass>>h5","abs(trueCharge)>0.5&&isSeen>0.7&&trueMass>0.&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)==321");
  hermTree->Draw("seenDEdx:trueP/trueMass>>h6","abs(trueCharge)>0.5&&isSeen>0.7&&trueMass>0.&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)==2212");


  TCanvas* c2 =  new TCanvas("c2","dE/dx of charged particles",750,750 );
  
  c2->Divide(2,3) ;
  
  
  c2->cd(1) ;
  gPad->SetLogx();
  
  h1->GetXaxis()->SetTitle("true P/true Mass" );
  h1->GetXaxis()->SetTitleSize( 0.04 ) ;
  h1->GetXaxis()->SetTitleOffset( 1.5 );
 
  h1->GetYaxis()->SetTitle( "seendEdx" );
  h1->GetYaxis()->SetTitleSize( 0.06 ) ;
  h1->GetYaxis()->SetTitleOffset( 1 );
  
  h1->Draw() ;
  h2->Draw("same");
  h3->Draw("same");
  h4->Draw("same");
  h5->Draw("same");
  h6->Draw("same") ;
  //hermTree->Draw("seenDEdx:trueP/trueMass","abs(trueCharge)>0.5&&trueMass>0.&&seenDEdx>0.&&seenDEdx<1.e-6") ;
 
  
 c2->cd(2) ;
 gPad->SetLogx(); 
  h2->GetXaxis()->SetTitle("true P/true Mass" );
  h2->GetXaxis()->SetTitleSize( 0.04 ) ;
  h2->GetXaxis()->SetTitleOffset( 1.5 );

  h2->GetYaxis()->SetTitle( "seendEdx" );
  h2->GetYaxis()->SetTitleSize( 0.06 ) ;
  h2->GetYaxis()->SetTitleOffset( 1 );

 h2->Draw();
 
c2->cd(3) ;
 gPad->SetLogx();

  h3->GetXaxis()->SetTitle("true P/true Mass" );
  h3->GetXaxis()->SetTitleSize( 0.04 ) ;
  h3->GetXaxis()->SetTitleOffset( 1.5 );

  h3->GetYaxis()->SetTitle( "seendEdx" );
  h3->GetYaxis()->SetTitleSize( 0.06 ) ;
  h3->GetYaxis()->SetTitleOffset(1 );

h3->Draw();

c2->cd(4) ;
gPad->SetLogx();

  h4->GetXaxis()->SetTitle("true P/true Mass" );
  h4->GetXaxis()->SetTitleSize( 0.04 ) ;
  h4->GetXaxis()->SetTitleOffset( 1.5 );

  h4->GetYaxis()->SetTitle( "seendEdx" );
  h4->GetYaxis()->SetTitleSize( 0.06 ) ;
  h4->GetYaxis()->SetTitleOffset( 1 );

h4->Draw();

 c2->cd(5) ;
gPad->SetLogx() ;

 h5->GetXaxis()->SetTitle("true P/true Mass" );
 h5->GetXaxis()->SetTitleSize( 0.04 ) ;
 h5->GetXaxis()->SetTitleOffset( 1.5 );
 
 h5->GetYaxis()->SetTitle( "seendEdx" );
 h5->GetYaxis()->SetTitleSize( 0.06 ) ;
 h5->GetYaxis()->SetTitleOffset( 1 );

h5->Draw();

 c2->cd(6) ;
 gPad->SetLogx() ;

 h6->GetXaxis()->SetTitle("true P/true Mass" );
 h6->GetXaxis()->SetTitleSize( 0.04 ) ;
 h6->GetXaxis()->SetTitleOffset( 1.5 );

  h6->GetYaxis()->SetTitle( "seendEdx" );
  h6->GetYaxis()->SetTitleSize( 0.06 ) ;
  h6->GetYaxis()->SetTitleOffset( 1 );

 h6->Draw();
  
}
