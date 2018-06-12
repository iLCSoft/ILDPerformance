void EfficiencyL5(){

  TFile* fttbar = new TFile("../Results/Analysis/analysis_E0500-TDR_ws.Pyycyyc.Gwhizard-1.95.eR.pL.I36919.05.root");

  //////// vs momentum

  // no overlay

  TH1F *h_reconstructable_p_noOverlay = (TH1F*) fttbar->Get("hist_p_t");
  TH1F *h_reconstructed_p_noOverlay = (TH1F*) fttbar->Get("hist_p_f");

  TEfficiency *h_efficiency_p_noOverlay = 0;

  if(TEfficiency::CheckConsistency(*h_reconstructed_p_noOverlay,*h_reconstructable_p_noOverlay)){
    h_efficiency_p_noOverlay = new TEfficiency(*h_reconstructed_p_noOverlay,*h_reconstructable_p_noOverlay);
  }

  ////////


  //////// vs theta

  // no overlay

  TH1F *h_reconstructable_theta_noOverlay = (TH1F*) fttbar->Get("hist_th_t");
  TH1F *h_reconstructed_theta_noOverlay = (TH1F*) fttbar->Get("hist_th_f");

  TEfficiency *h_efficiency_theta_noOverlay = 0;

  if(TEfficiency::CheckConsistency(*h_reconstructed_theta_noOverlay,*h_reconstructable_theta_noOverlay)){
    h_efficiency_theta_noOverlay = new TEfficiency(*h_reconstructed_theta_noOverlay,*h_reconstructable_theta_noOverlay);
  }




  //////// vs pt

  // no overlay

  TH1F *h_reconstructable_pt_noOverlay = (TH1F*) fttbar->Get("hist_pt_t");
  TH1F *h_reconstructed_pt_noOverlay = (TH1F*) fttbar->Get("hist_pt_f");

  TEfficiency *h_efficiency_pt_noOverlay = 0;

  if(TEfficiency::CheckConsistency(*h_reconstructed_pt_noOverlay,*h_reconstructable_pt_noOverlay)){
    h_efficiency_pt_noOverlay = new TEfficiency(*h_reconstructed_pt_noOverlay,*h_reconstructable_pt_noOverlay);
  }

  ////////

  //////// vs theta

  // no overlay

  TH1F *h_reconstructable_theta_noOverlay = (TH1F*) fttbar->Get("hist_th_t");
  TH1F *h_reconstructed_theta_noOverlay = (TH1F*) fttbar->Get("hist_th_f");

  TEfficiency *h_efficiency_theta_noOverlay = 0;

  if(TEfficiency::CheckConsistency(*h_reconstructed_theta_noOverlay,*h_reconstructable_theta_noOverlay)){
    h_efficiency_theta_noOverlay = new TEfficiency(*h_reconstructed_theta_noOverlay,*h_reconstructable_theta_noOverlay);
  }



  //////// PLOTTING

  // vs momentum
  TCanvas *c_p = new TCanvas();
  c_p->SetLogx();
  h_efficiency_p_noOverlay->SetTitle(";momentum [GeV/c];Tracking efficiency");
  h_efficiency_p_noOverlay->SetLineColor(kRed);
  h_efficiency_p_noOverlay->SetMarkerColor(kRed);
  h_efficiency_p_noOverlay->SetMarkerStyle(20);
  h_efficiency_p_noOverlay->SetMarkerSize(0.75);
  h_efficiency_p_noOverlay->Draw("");
  gPad->Update();

  auto graph_p_noOverlay = h_efficiency_p_noOverlay->GetPaintedGraph();

  graph_p_noOverlay->GetYaxis()->SetRangeUser(0.0,1.03);
  graph_p_noOverlay->GetXaxis()->SetLimits(0.1,100.0);
  graph_p_noOverlay->GetXaxis()->SetTitleOffset(1.2);

  gPad->Update();

  auto legend0_p = new TLegend(0.461153,0.242604,0.906015,0.363905);
  legend0_p->SetHeader("t#bar{t} @ 500GeV");
  legend0_p->AddEntry(h_efficiency_pt_noOverlay,"No background","epl");
  legend0_p->Draw();

  c_p->SaveAs("trkEff_p_ttbar.png");

  // vs pt
  TCanvas *c_pt = new TCanvas();
  c_pt->SetLogx();
  h_efficiency_pt_noOverlay->SetTitle(";p_{T} [GeV/c];Tracking efficiency");
  h_efficiency_pt_noOverlay->SetLineColor(kRed);
  h_efficiency_pt_noOverlay->SetMarkerColor(kRed);
  h_efficiency_pt_noOverlay->SetMarkerStyle(20);
  h_efficiency_pt_noOverlay->SetMarkerSize(0.75);
  h_efficiency_pt_noOverlay->Draw("");
  gPad->Update();

  auto graph_pt_noOverlay = h_efficiency_pt_noOverlay->GetPaintedGraph();

  graph_pt_noOverlay->GetYaxis()->SetRangeUser(0.0,1.03);
  graph_pt_noOverlay->GetXaxis()->SetLimits(0.1,100.0);
  graph_pt_noOverlay->GetXaxis()->SetTitleOffset(1.2);

  gPad->Update();

  auto legend0_pt = new TLegend(0.461153,0.242604,0.906015,0.363905);
  legend0_pt->SetHeader("t#bar{t} @ 500GeV");
  legend0_pt->AddEntry(h_efficiency_pt_noOverlay,"No background","epl");
  legend0_pt->Draw();

  c_pt->SaveAs("trkEff_pt_ttbar.png");


 // vs theta
  TCanvas *c_theta = new TCanvas();
  h_efficiency_theta_noOverlay->SetTitle(";cos(#theta) ;Tracking efficiency");
  h_efficiency_theta_noOverlay->SetLineColor(kRed);
  h_efficiency_theta_noOverlay->SetMarkerColor(kRed);
  h_efficiency_theta_noOverlay->SetMarkerStyle(20);
  h_efficiency_theta_noOverlay->SetMarkerSize(0.75);
  h_efficiency_theta_noOverlay->Draw("");
  gPad->Update();

  auto graph_theta_noOverlay = h_efficiency_theta_noOverlay->GetPaintedGraph();

  graph_theta_noOverlay->GetYaxis()->SetRangeUser(0.7,1.01);
  graph_theta_noOverlay->GetXaxis()->SetLimits(0.0,1.0);
  graph_theta_noOverlay->GetXaxis()->SetTitleOffset(1.2);

  gPad->Update();

  auto legend0_theta = new TLegend(0.461153,0.242604,0.906015,0.363905);
  legend0_theta->SetHeader("t#bar{t} @ 500GeV");
  legend0_theta->AddEntry(h_efficiency_theta_noOverlay,"No background","epl");
  legend0_theta->Draw();

  c_theta->SaveAs("trkEff_theta_ttbar.png");

}
