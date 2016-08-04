#include "TH1.h"
/** Very simply plotting of likely PDG
 *  Should add colors for standard PDG types ...
 * 
 */
void plotPIDLike(const char* _filename ) {

  gROOT->SetStyle("ildStyle");
  // use small markers:
  ildStyle->SetMarkerStyle(1);

  TFile *treefile = new TFile(_filename);

  const int nBins = 100 ;
  const double maxX  = 10 ;
  const double minX  = -200;

  TH1F* h1= new TH1F( "h1", "Likely PDG True electorns", nBins ,minX , maxX ) ;
  TH1F* h2= new TH1F( "h2", "Likely PDG values for electorns", nBins ,minX , maxX ) ;
  TH1F* h3= new TH1F( "h3", "Likely PDG True muons", nBins ,minX , maxX ) ;
  TH1F* h4= new TH1F( "h4", "Likely PDG values for muons", nBins , minX, maxX ) ;
  TH1F* h5= new TH1F( "h5", "Likely PDG True pions", nBins , minX,  maxX) ;
  TH1F* h6= new TH1F( "h6", "Likely PDG values for pions", nBins , minX, maxX ) ;
  TH1F* h7= new TH1F( "h7", "Likely PDG True Kaons", nBins ,minX , maxX) ;
  TH1F* h8= new TH1F( "h8", "Likely PDG values for Kaons", nBins ,minX ,maxX  ) ;
  TH1F* h9= new TH1F( "h9", "Likely PDG True Protons", nBins ,minX ,maxX ) ;
  TH1F* h10= new TH1F( "h10", "Likely PDG vlaues for Protons", nBins ,minX ,maxX ) ;
  

  hermTree->Draw("LiPDG_el>>h1" ,"abs(trueCharge)>0.5&&isSeen>0.7&&trueP>1.0&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)==11");
  hermTree->Draw("LiPDG_el>>h2" ,"abs(trueCharge)>0.5&&isSeen>0.7&&trueP>1.0&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)!=11");
  hermTree->Draw("LiPDG_mu>>h3" ,"abs(trueCharge)>0.5&&isSeen>0.7&&trueP>1.0&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)==13");
  hermTree->Draw("LiPDG_mu>>h4" ,"abs(trueCharge)>0.5&&isSeen>0.7&&trueP>1.0&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)!=13");
  hermTree->Draw("LiPDG_pi>>h5" ,"abs(trueCharge)>0.5&&isSeen>0.7&&trueP>1.0&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)==211");
  hermTree->Draw("LiPDG_pi>>h6" ,"abs(trueCharge)>0.5&&isSeen>0.7&&trueP>1.0&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)!=211");
  hermTree->Draw("LiPDG_ka>>h7" ,"abs(trueCharge)>0.5&&isSeen>0.7&&trueP>1.0&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)==321");
  hermTree->Draw("LiPDG_ka>>h8" ,"abs(trueCharge)>0.5&&isSeen>0.7&&trueP>1.0&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)!=321");
  hermTree->Draw("LiPDG_pr>>h9" ,"abs(trueCharge)>0.5&&isSeen>0.7&&trueP>1.0&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)==2212");
  hermTree->Draw("LiPDG_pr>>h10","abs(trueCharge)>0.5&&isSeen>0.7&&trueP>1.0&&seenDEdx>0.&&seenDEdx<1.e-6&&abs(truePDG)!=2212");

  TCanvas* c2 =  new TCanvas("c2","Likely PDG for true and fake particles",750,750 );
  
  c2->Divide(2,3) ;
  
  
  c2->cd(1) ;
gPad->SetLogy() ;
  // h1->SetFillStyle( 3001);
  //h1->SetLineColor( kRed);
  //h1->Draw( "hist same");

  //h2->SetFillStyle( 3001);
  //h2->SetLineColor( kBlue);
  //h2->Draw( "hist same");
  
  h1->SetLineColor( kBlue);
  h2->SetLineColor( kRed);
		    

  h2->GetXaxis()->SetTitle("Likely PDG" );
  h2->GetXaxis()->SetTitleSize( 0.04 ) ;
  h2->GetXaxis()->SetTitleOffset( 1.5 );
 
 h2->Draw( "hist");
 h1->Draw( "hist same");
 //h1->Draw() ;
  //h2->Draw("same");

  
 c2->cd(2) ;
gPad->SetLogy() ; 

 h3->SetLineColor( kBlue);
  h4->SetLineColor( kRed);

 h4->GetXaxis()->SetTitle("Likely PDG" );
 h4->GetXaxis()->SetTitleSize( 0.04 ) ;
 h4->GetXaxis()->SetTitleOffset( 1.5 );

  h3->SetLineColor( kBlue);
  h4->SetLineColor( kRed);

h4->Draw("hist");
h3->Draw("hist same");
 

 
c2->cd(3) ;
 
gPad->SetLogy() ;

h5->SetLineColor( kBlue);
  h6->SetLineColor( kRed);

 h6->GetXaxis()->SetTitle("Likely PDG" );
 h6->GetXaxis()->SetTitleSize( 0.04 ) ;
 h6->GetXaxis()->SetTitleOffset( 1.5 );

 h5->Draw( "hist");
 h6->Draw( "hist same");

c2->cd(4) ;
gPad->SetLogy() ;

h7->SetLineColor( kBlue);
  h8->SetLineColor( kRed);

 h8->GetXaxis()->SetTitle("Likely PDG" );
 h8->GetXaxis()->SetTitleSize( 0.04 ) ;
 h8->GetXaxis()->SetTitleOffset( 1.5 );

h8->Draw( "hist");
 h7->Draw( "hist same");

 c2->cd(5) ;

gPad->SetLogy() ;

h9->SetLineColor( kBlue);
  h10->SetLineColor( kRed);

  h10->GetXaxis()->SetTitle("Likely PDG" );
  h10->GetXaxis()->SetTitleSize( 0.04 ) ;
  h10->GetXaxis()->SetTitleOffset( 1.5 );

  h10->Draw( "hist");
 h9->Draw( "hist same");


TString outfile = "../Results/Likely_PDG";
   c2->Print(TString(outfile+".pdf"));
  
}
