void EfficiencyL5(){

  TFile* f = new TFile("../Results/Analysis/analysis_ILD_l5_v02_v01-19-05_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.root");

  TCanvas *eff = (TCanvas*) f->Get("eff");
  eff->Draw();
  eff->SaveAs("trkEff_ILD_l5_v02_v01-19-05.png");

  TCanvas *effPM = (TCanvas*) f->Get("effPM");
  effPM->Draw();
  effPM->SaveAs("trkEff_v02_ILD_l5_v02_v01-19-05.png");

}
