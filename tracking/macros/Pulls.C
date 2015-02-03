void Pulls()
{
  TFile *f1 = new TFile("../scripts/ildperf.root", "read");
  TDirectory *d1 = (TDirectory*) f1->Get("MyRecoMCTruthLinker");
  TCanvas *Pulls = (TCanvas*)d1->Get("pulls");
  Pulls->Draw();
}
