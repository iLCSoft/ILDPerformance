#include <vector>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TString.h"

using namespace std;


void plotERes(const char* _filename) {
  gROOT->SetStyle("ildStyle");
  ildStyle->SetOptTitle(0);
  ildStyle->SetHistLineWidth(2);
  ildStyle->SetLineWidth(2);
  ildStyle->SetCanvasBorderMode(0);
  ildStyle->SetPadBorderMode(0);
  ildStyle->SetPadColor(0);
  ildStyle->SetCanvasColor(0);
  ildStyle->SetTitleColor(1);
  ildStyle->SetStatColor(0);
//  ildStyle->SetOptStat(1110010);
  ildStyle->SetOptStat(1110);
  ildStyle->SetOptFit(111);
  
  ildStyle->SetPadBottomMargin(0.15);
  ildStyle->SetPadTopMargin(0.10);
  ildStyle->SetPadRightMargin(0.025);
  ildStyle->SetPadLeftMargin(0.1);
 
  ildStyle->SetMarkerStyle(20);
  ildStyle->SetMarkerSize(0.625);

  TFile *treefile = new TFile(_filename);
  TTree *tree = (TTree*) treefile->Get("pi0Tree");
    
  TH1F *h1 = new TH1F("h1","h1;(E_PFO-E_truth)/E_truth[%], correct ggp's",100,-30.,50.);  
  TH1F *h2 = new TH1F("h2","h2;(E_FIT-E_truth)/E_truth[%], correct ggp's",100,-30.,50.);  
  TH1F *h3 = new TH1F("h3","h3;(E_PFO-E_truth)/E_truth[%], all ggp's",100,-30.,50.);  
  TH1F *h4 = new TH1F("h4","h4;(E_FIT-E_truth)/E_truth[%], all ggp's",100,-30.,50.);  
  TH1F *h5 = new TH1F("h5","h5;(E_PFO-E_truth)/E_truth[%], wrong ggp's",100,-30.,50.);  
  TH1F *h6 = new TH1F("h6","h6;(E_FIT-E_truth)/E_truth[%], wrong ggp's",100,-30.,50.);  
    
  TCanvas *c1 = new TCanvas("c1","correct",600,600);  
  c1->Divide(1,2);
  c1->cd(1);
  tree->Draw("(pfoE-trueEofSeen)/trueEofSeen*100.>>h1","isTrue");
  h1->SetLineColor(1);
  h1->SetMaximum(250);
  h1->Fit("gaus","ME","",-15.,15.);
  c1->cd(2);
  tree->Draw("(recoE-trueEofSeen)/trueEofSeen*100.>>h2","isTrue");
  h2->SetLineColor(4);
  h2->SetMaximum(250);
  h2->Fit("gaus","ME","",-5.,5.);
  
  TString outfile1 = "../Results/pi0_e2e2h_Eres_correctGGParticles";
  //cout << outfile << endl;
  c1->Print(TString(outfile1+".eps"));
  c1->Print(TString(outfile1+".ps"));
  c1->Print(TString(outfile1+".pdf"));

  TCanvas *c2 = new TCanvas("c2","all",600,600);  
  c2->Divide(1,2);
  c2->cd(1);
  tree->Draw("(pfoE-trueEofSeen)/trueEofSeen*100.>>h3","");
  h3->SetLineColor(1);
  h3->SetMaximum(350);
  h3->Fit("gaus","ME","",-10.,10.);
  c2->cd(2);
  tree->Draw("(recoE-trueEofSeen)/trueEofSeen*100.>>h4","");
  h4->SetLineColor(4);
  h4->SetMaximum(350);
  h4->Fit("gaus","ME","",-5.,5.);

  TString outfile2 = "../Results/pi0_e2e2h_Eres_allGGParticles";
  //cout << outfile << endl;
  c2->Print(TString(outfile2+".eps"));
  c2->Print(TString(outfile2+".ps"));
  c2->Print(TString(outfile2+".pdf"));

  TCanvas *c3 = new TCanvas("c3","wrong",600,600);  
  c3->Divide(1,2);
  c3->cd(1);
  tree->Draw("(pfoE-trueEofSeen)/trueEofSeen*100.>>h5","!isTrue");
  h5->SetLineColor(1);
  h5->SetMaximum(350);
  h5->Fit("gaus","ME","",-20.,20.);
  c3->cd(2);
  tree->Draw("(recoE-trueEofSeen)/trueEofSeen*100.>>h6","!isTrue");
  h6->SetLineColor(4);
  h6->SetMaximum(350);
  h6->Fit("gaus","ME","",-20.,20.);

  TString outfile3 = "../Results/pi0_e2e2h_Eres_wrongGGParticles";
  //cout << outfile << endl;
  c3->Print(TString(outfile3+".eps"));
  c3->Print(TString(outfile3+".ps"));
  c3->Print(TString(outfile3+".pdf"));

  return;
  
}
