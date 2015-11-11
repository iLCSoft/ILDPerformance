#include <vector>

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TString.h"

using namespace std;


void plotPIDs(const char* _filename) {
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

  TFile *treefile = new TFile(_filename);
  TTree *tree = (TTree*) treefile->Get("hermTree");
    
  // cut:
  // 1: p > 1 GeV
  // 8: 1 GeV < p < 10 GeV
  int icut = 9;
     
  // particles:
  // pdg = 0: all
  // pdg = 1: all charged
  // pdg = 2: all neutral
  // other: pdg code, in particular 11, 13, 211, 321, 2212
  int npdg = 5;
  int kpdg[5] = {11, 13, 211, 321, 2212};
  TString tpdg[5] = {"electrons", "muons", "pions", "kaons", "protons"};
  
  TCanvas *cpdg[5];
  TH1F *hpdg[5][6];
  TLegend *lpdg[5][6];
  
  for (int ipdg = 0; ipdg < 5; ipdg++) {
  
    // draw
    cpdg[ipdg] = new TCanvas (tpdg[ipdg],tpdg[ipdg],750,750);
    cpdg[ipdg]->Divide(2,2);
 
    getPDGHistos(hpdg[ipdg], tree, kpdg[ipdg], icut);
  
    int ntrue = hpdg[ipdg][0]->GetEntries();
    cout << "ntrue = " << ntrue << endl;
  
    for (int ihist = 1; ihist < 5; ihist++) {
      //cout << "ipdg, ihist = " << ipdg << "," << ihist << ": npart = " << hpdg[ipdg][ihist]->GetEntries() << endl;
      cpdg[ipdg]->cd(ihist);
      gPad->SetGridy(1);
      if (ntrue > 0) {
        hpdg[ipdg][ihist]->Scale(100./ntrue);
        hpdg[ipdg][ihist]->SetMaximum(110.);
      }  
      hpdg[ipdg][ihist]->Draw("E0");
//       lpdg[ipdg][ihist] = new TLegend (0.25, 0.7, 0.7, 0.9, hpdg[ipdg][ihist]->GetTitle());
//       lpdg[ipdg][ihist]->Draw();
    }  


    TString outfile = "../Results/PIDs_n1n1hh";
    outfile += "_pdg";
    outfile += ipdg;
    outfile += "_cut";
    outfile += icut;
    //cout << outfile << endl;
    cpdg[ipdg]->Print(TString(outfile+".eps"));
    cpdg[ipdg]->Print(TString(outfile+".ps"));
    cpdg[ipdg]->Print(TString(outfile+".pdf"));

  }
  
  return;
  
}

void getPDGHistos (TH1F **hist, TTree *tree, int &pdg, int &itruecut) {

  cout << "pdg = " << pdg << ", itruecut = " << itruecut << endl;
  TString partname = "";
  switch (pdg) {
    case 0: partname = ", all"; break;
    case 1: partname = ", all charged"; break;
    case 2: partname = ", all neutral"; break;
    case 11: partname = ", electrons"; break;
    case 13: partname = ", muons"; break;
    case 15: partname = ", taus"; break;
    case 111: partname = ",  pi0"; break;
    case 211: partname = ", pi+-"; break;
    case 321: partname = ", K+-"; break;
    case 2212: partname = ", protons"; break;
  }
  
  
  TString cutname = "truePt";
  TString cuttitle = "";
  double cutmin = 1;
  double cutmax = 1000000000;
  //cout << "itruecut = " << itruecut << endl;
  switch (itruecut) {
    case 1: cutname = "trueP";     cutmin = 1; cutmax = 1000000000; cuttitle = " with P > 1 GeV"; break;
    case 2: cutname = "trueP";     cutmin = 0; cutmax = 2.;         cuttitle = " with P < 2 GeV"; break; 
    case 3: cutname = "trueTheta"; cutmin = 0; cutmax = 3.2; break;
    case 4: cutname = "truePhi";   cutmin = 0; cutmax = 3.2; break; 
    case 5: cutname = "trueTheta"; cutmin = 0; cutmax = 0.95; cuttitle = " with |cos(theta)|<0.95";   break;
    case 6: cutname = "truePt";    cutmin = 0; cutmax = 1;    cuttitle = " with pt < 1 GeV";          break;
    case 7: cutname = "truePt";    cutmin = 1; cutmax = 10;   cuttitle = " with 1 GeV < pt < 10 GeV"; break; 
    case 8: cutname = "trueP";     cutmin = 1; cutmax = 10;   cuttitle = " with 1 GeV < p < 10 GeV";  break; 
    case 9: cutname = "trueP";     cutmin = 3; cutmax = 1000000000; cuttitle = " with P > 3 GeV"; break;
  }
  //cout << "cutname = " << cutname << endl;
  
  // book histos
  TString hname[6] = {"MCTruth", "BasicPID", "dEdxPID", "ShowerPID", "LikeliPID", "LowMomPID"};
  for (int i = 0; i < 6; ++i) {
    hist[i] = new TH1F(hname[i],hname[i] + partname + cuttitle, 5, -0.5, 4.5);
    hist[i]->Sumw2();
    hist[i]->SetTitle(hname[i] + partname + cuttitle);
    hist[i]->GetYaxis()->SetTitle(TString("%    "));
    hist[i]->GetXaxis()->SetTitleSize(0.06);
    hist[i]->GetYaxis()->SetTitleSize(0.06);
    hist[i]->GetXaxis()->SetTitleOffset(1.0);
    hist[i]->GetYaxis()->SetTitleOffset(1.5);
    hist[i]->GetXaxis()->SetLabelSize(0.1);
    hist[i]->GetYaxis()->SetLabelSize(0.06);
    hist[i]->GetXaxis()->SetLabelOffset(0.02);
    hist[i]->GetYaxis()->SetLabelOffset(0.01);
    hist[i]->GetXaxis()->SetBinLabel(1,"e");
    hist[i]->GetXaxis()->SetBinLabel(2,"mu");
    hist[i]->GetXaxis()->SetBinLabel(3,"pi");
    hist[i]->GetXaxis()->SetBinLabel(4,"K");
    hist[i]->GetXaxis()->SetBinLabel(5,"p");
  }  
  
  
  // read branches
  int npart;
  vector<double> *cutvar;
  vector<int> *truePDG;
  vector<double> *isSeen;
  vector<int> *basicPDG;
  vector<int> *dEdxPDG;
  vector<int> *showerPDG;
  vector<int> *likeliPDG;
  vector<int> *lowmomPDG;
  vector<double> *trueCharge;
  vector<double> *seenCharge;
  tree->SetBranchAddress("nMCParticles", &npart);        
  tree->SetBranchAddress(cutname,      &cutvar);  
  tree->SetBranchAddress("trueCharge", &trueCharge); 
  tree->SetBranchAddress("truePDG",    &truePDG);  
  tree->SetBranchAddress("isSeen",     &isSeen);  
  tree->SetBranchAddress("seenCharge", &seenCharge); 
  tree->SetBranchAddress("basicPDG",   &basicPDG);  
  tree->SetBranchAddress("dEdxPDG",    &dEdxPDG);  
  tree->SetBranchAddress("showerPDG",  &showerPDG);  
  tree->SetBranchAddress("likeliPDG",  &likeliPDG);  
  tree->SetBranchAddress("lowmomPDG",  &lowmomPDG);  
  
  //cout << "after branch addressing "  << endl;

  // loop over tree
  bool truecut  = false;
  int nevts = tree->GetEntries();
  int idmap[3000];
  idmap[11] = 0;  
  idmap[13] = 1;
  idmap[211] = 2;
  idmap[321] = 3;
  idmap[2212] = 4;
  for (int ievt = 0; ievt < nevts; ++ievt) {
    tree->GetEntry(ievt);
   
    for (int ipart = 0; ipart < truePDG->size(); ipart++) {  
      truecut = (pdg==0 || (pdg==1 && trueCharge->at(ipart)!=0) || (pdg==2 && trueCharge->at(ipart)==0) || (pdg > 10 && abs(truePDG->at(ipart)) == pdg));
      if (itruecut==5) {   // cos theta
        truecut &= (itruecut==0 || (fabs(cos(cutvar->at(ipart))) > cutmin && fabs(cos(cutvar->at(ipart))) < cutmax)); 
      }  
      else {
        truecut &= (itruecut==0 || (cutvar->at(ipart) > cutmin && cutvar->at(ipart) < cutmax)); 
      }   
     
      if (!truecut) continue;
           
      if (isSeen->at(ipart)<0.5) continue;
      if (seenCharge->at(ipart)==0) continue;
      
      hist[0]->Fill(idmap[abs(truePDG->at(ipart))]);
      hist[1]->Fill(idmap[abs(basicPDG->at(ipart))]);
      hist[2]->Fill(idmap[abs(dEdxPDG->at(ipart))]);
      hist[3]->Fill(idmap[abs(showerPDG->at(ipart))]);
      hist[4]->Fill(idmap[abs(likeliPDG->at(ipart))]);
      hist[5]->Fill(idmap[abs(lowmomPDG->at(ipart))]);
    }
  }
  
//   for (int i = 0; i < 6; ++i) {
// //     cout << "pdg, i = " << pdg << "," << i << ": npart = " << hist[i]->GetEntries() << endl;
//     hist[i]->SetTitle(hname[i] + partname + cuttitle + TString("; ntot = ") + TString(hist[i]->GetEntries()));
//   }
  double ymax = hist[0]->GetMaximum()*1.2;
  for (int i = 0; i < 6; ++i) {
    hist[i]->SetAxisRange(0, ymax, "Y");
  }
  
  return;

}
