void Residuals()
{
  TFile *f1 = new TFile("../scripts/ildperf.root", "read");
  TDirectory *d1 = (TDirectory*) f1->Get("MyRecoMCTruthLinker");
  TCanvas *Residuals = (TCanvas*)d1->Get("residuals");
  Residuals->Draw();
}
