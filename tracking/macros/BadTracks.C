void BadTracks()
{
  TFile *f1 = new TFile("../scripts/ildperf.root", "read");
  TDirectory *d1 = (TDirectory*) f1->Get("MyRecoMCTruthLinker");
  TTree *t1 = (TTree*) d1->Get("EvalTree");

  TCanvas *BadTrks = new TCanvas();
  BadTrks->cd();
  TH1F *h1 = new TH1F("h1","Ghosts Pt",4000,0,4000);
  t1->Draw("GhostsPt>>h1");
  BadTrks->SetLogy();
}
