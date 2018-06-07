void EfficiencyL52D(){

  TFile* fttbar = new TFile("../Results/Analysis/analysis_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.root");


  //////// 2D histogram: efficiency vs theta and pt

  // no overlay
  TH2F *h2_reconstructable_thm_pt_noOverlay = (TH2F*) fttbar->Get("h2_thm_pt_t");
  TH2F *h2_reconstructed_thm_pt_noOverlay = (TH2F*) fttbar->Get("h2_thm_pt_f");

  TEfficiency *h2_efficiency_thm_pt_noOverlay = 0;

  if(TEfficiency::CheckConsistency(*h2_reconstructed_thm_pt_noOverlay,*h2_reconstructable_thm_pt_noOverlay)){
    h2_efficiency_thm_pt_noOverlay = new TEfficiency(*h2_reconstructed_thm_pt_noOverlay,*h2_reconstructable_thm_pt_noOverlay);
  }

  TCanvas *c_h2 = new TCanvas();
  c_h2->SetLogy();
  h2_efficiency_thm_pt_noOverlay->SetTitle("Tracking efficiency;cos(#theta);p_{T} [GeV/c]");
  h2_efficiency_thm_pt_noOverlay->Draw("COLZ");
  gPad->Update();


  c_h2->SaveAs("trkEff_2D_theta_pt_ttbar.png");

}
