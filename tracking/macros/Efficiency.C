void Efficiency()
{
  TFile *f1 = new TFile("../scripts/ildperf.root", "read");
  TDirectory *d1 = (TDirectory*) f1->Get("MyRecoMCTruthLinker");
  TCanvas *eff = (TCanvas*) d1->Get("eff");
  eff->Draw();

}
