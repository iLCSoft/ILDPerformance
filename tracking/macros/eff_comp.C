void eff_comp(){

  gStyle->SetFillColor(0);

  //option for graphs with statistical error
  
  TFile *f1 = new TFile("file1.root","read");
  TDirectory *d1 = (TDirectory*) f1->Get("MyRecoMCTruthLinker");
  TGraphAsymmErrors *grb1 = (TGraphAsymmErrors*)d1->Get("gpt");
  TGraphAsymmErrors *grth1 = (TGraphAsymmErrors*)d1->Get("gth");
  grb1->SetFillColor(0);

  TFile *f2 = new TFile("file2.root","read");
  TDirectory *d2 = (TDirectory*) f2->Get("MyRecoMCTruthLinker");
  TGraphAsymmErrors *grb2 = (TGraphAsymmErrors*)d2->Get("gpt");
  TGraphAsymmErrors *grth2 = (TGraphAsymmErrors*)d2->Get("gth");
  
  TFile *f3 = new TFile("file3.root","read");
  TDirectory *d3 = (TDirectory*) f3->Get("MyRecoMCTruthLinker");
  TGraphAsymmErrors *grb3 = (TGraphAsymmErrors*)d3->Get("gpt");
  TGraphAsymmErrors *grth3 = (TGraphAsymmErrors*)d3->Get("gth");
  
  
  TFile *ofile = new TFile("effcomp.root","RECREATE");

  TCanvas *c = new TCanvas("c","efficiency",800,800);
  c->Divide(1,2);
  c->SetFillColor(0);
  c->cd(1);  
  gPad->SetLogx() ;
  
  grb1->SetMarkerColor(4);
  grb1->SetMarkerStyle(22);
  grb1->Draw("AP");
  grb2->SetMarkerColor(2);
  grb2->SetMarkerStyle(21);
  grb2->Draw("P");
  grb3->SetMarkerColor(6);
  grb3->SetMarkerStyle(22);
  grb3->Draw("P");
  
  c->cd(2);
  grth1->SetMarkerColor(4);
  grth1->SetMarkerStyle(22);
  grth1->Draw("AP");
  grth2->SetMarkerColor(2);
  grth2->SetMarkerStyle(21);
  grth2->Draw("P");
  grth3->SetMarkerColor(6);
  grth3->SetMarkerStyle(22);
  grth3->Draw("P");

  leg = new TLegend(0.2,0.36,0.55,0.68);
  leg->AddEntry(grb1,"","p");
  leg->AddEntry(grb2,"","p");
  leg->AddEntry(grb3,"","p");
  //leg->AddEntry(grb4,"nominal VXD03","p");
  leg->Draw();


  c->Write();  

  ofile->Write();
}
