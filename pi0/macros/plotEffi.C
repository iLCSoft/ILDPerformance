#include <vector>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TString.h"

using namespace std;


void plotEffi(const char* _filename) {
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
  ildStyle->SetOptStat(0);
  ildStyle->SetOptFit(111);
  
  ildStyle->SetPadBottomMargin(0.175);
  ildStyle->SetPadTopMargin(0.10);
  ildStyle->SetPadRightMargin(0.025);
  ildStyle->SetPadLeftMargin(0.12);
 
  ildStyle->SetMarkerStyle(20);
  ildStyle->SetMarkerSize(0.625);

  TFile *treefile = new TFile(_filename);
  TTree *tree = (TTree*) treefile->Get("pi0Tree");
    
  TH1F *h1 = new TH1F("h1","h1;E[GeV], seen", 10,0.,50.);  
  TH1F *h2 = new TH1F("h2","h2;E[GeV], all", 10,0.,50.);  
  TH1F *h3 = new TH1F("h3","h3;E_{#pi^{0}/#eta/#eta'} [GeV];efficiency", 10,0.,50.);  
//  TH1F *h3 = new TH1F("h3","h3;E_{#pi^{0}cand.} [GeV];efficiency", 10,0.,50.);  
  h3->Sumw2();   
  h3->GetXaxis()->SetTitleOffset(1.2);
  h3->GetYaxis()->SetTitleOffset(0.75);
  h3->SetMaximum(0.75);
    
  TH1F *h11 = new TH1F("h11","h11;E[GeV], seen", 12,0.,12.);  
  TH1F *h12 = new TH1F("h12","h12;E[GeV], all", 12,0.,12.);  
//  TH1F *h13 = new TH1F("h13","h13;E_{#pi^{0}cand.} [GeV];efficiency", 12,0.,12.);  
  TH1F *h13 = new TH1F("h13","h13;E_{#pi^{0}/#eta/#eta'} [GeV];efficiency", 12,0.,12.);  
  h13->Sumw2();  
  h13->GetXaxis()->SetTitleOffset(1.2);
  h13->GetYaxis()->SetTitleOffset(0.75);
  h13->SetMaximum(0.75);
    
  // ``findable'' mesons: both photons reconstructed as one photon each
  tree->Draw("pfoE>>h1","isTrue");
  tree->Draw("trueE>>h2","weightToPhotons>1.5&&abs(cos(trueTheta))<0.995");
  h3->Divide(h1,h2,1,1,"B");
  h3->Fit("pol0","ME","",5.,50.);
  
  tree->Draw("pfoE>>h11","isTrue");
  tree->Draw("trueE>>h12","weightToPhotons>1.5&&abs(cos(trueTheta))<0.995");
  h13->Divide(h11,h12,1,1,"B");

  
  TCanvas *c1 = new TCanvas("c1","effi",600,600);  
  c1->Divide(1,2);
  c1->cd(1);
  h3->Draw("E0");
  c1->cd(2);
  h13->Draw("E0");
  
  TString outfile1 = "../Results/pi0_e2e2h_Effi_correctGGParticles";
  //cout << outfile << endl;
  c1->Print(TString(outfile1+".eps"));
  c1->Print(TString(outfile1+".ps"));
  c1->Print(TString(outfile1+".pdf"));


  return;
  
}
