void ComparePulls()
{
  //please modify the path accordingly to point to the desired file
  TFile *f1 = new TFile("../Results/Analysis/analysis_MuonsAngle_85_Mom_1.root", "read");
  TDirectory *d1 = (TDirectory*) f1->Get("MyRecoMCTruthLinker");
  TH1F *D0 = (TH1F*)d1->Get("d0pull");
  TH1F *Z0 = (TH1F*)d1->Get("z0pull");
  TH1F *Omega = (TH1F*)d1->Get("OmegaPull");
  TH1F *TanLambda = (TH1F*)d1->Get("TanLambdaPull");
  TH1F *Phi = (TH1F*)d1->Get("PhiPull");

  TFile *f2 = new TFile("../Results/Analysis/analysis_AntiMuonsAngle_85_Mom_1.root", "read");
  TDirectory *d2 = (TDirectory*) f2->Get("MyRecoMCTruthLinker");
  TH1F *D02 = (TH1F*)d2->Get("d0pull");
  TH1F *Z02 = (TH1F*)d2->Get("z0pull");
  TH1F *Omega2 = (TH1F*)d2->Get("OmegaPull");
  TH1F *TanLambda2 = (TH1F*)d2->Get("TanLambdaPull");
  TH1F *Phi2 = (TH1F*)d2->Get("PhiPull");

  TCanvas *rescomp =  new TCanvas("rescomp","Track par. residuals",800,800);
  TPad *pad1 = new TPad("pad1","",0,0,1,1);
  TPad *pad2 = new TPad("pad2","",0,0,1,1);

  rescomp->Divide(3,2);

  //pad1->cd();
  rescomp->cd(1);
  Omega->Draw();
  //pad1->Update();
  TF1 *OmegaFit = Omega->GetFunction("gaus");
  TPaveStats *ps1 = (TPaveStats*)Omega->GetListOfFunctions()->FindObject("stats");
  ps1->SetX1NDC(0.4); ps1->SetX2NDC(0.6);
  ps1->SetTextColor(kBlue);
  OmegaFit->SetLineColor(kBlue);
  //pad2->cd();
  Omega2->Draw("same");
  //pad2->Update();
  Omega2->SetLineColor(kRed);
  TPaveStats *ps2 = (TPaveStats*)Omega2->GetListOfFunctions()->FindObject("stats");
  ps2->SetX1NDC(0.65); ps2->SetX2NDC(0.85);
  ps2->SetTextColor(kRed);

  rescomp->cd(2);
  Phi->Draw();
  //Phi->SetLineColor(kRed);
  TF1 *PhiFit = Phi->GetFunction("gaus");
  PhiFit->SetLineColor(kBlue);
  Phi2->Draw("same");
  Phi2->SetLineColor(kRed);

  rescomp->cd(3);
  TanLambda->Draw();
  TF1 *TanLambdaFit = TanLambda->GetFunction("gaus");
  TanLambdaFit->SetLineColor(kBlue);
  TanLambda2->Draw("same");
  TanLambda2->SetLineColor(kRed);

  rescomp->cd(4);
  D0->Draw();
  TF1 *D0Fit = D0->GetFunction("gaus");
  D0Fit->SetLineColor(kBlue);
  D02->SetLineColor(kRed);
  D02->Draw("same");

  rescomp->cd(5);
  Z0->Draw();
  TF1 *Z0Fit = Z0->GetFunction("gaus");
  Z0Fit->SetLineColor(kBlue);
  Z02->SetLineColor(kRed);
  Z02->Draw("same");


  rescomp->cd(6);

  leg = new TLegend(0.2,0.26,0.75,0.88);
  leg->AddEntry(Phi,"Muons 5 GeV","l");
  leg->AddEntry(Phi2,"Anti-muons 5 GeV","l");
  leg->Draw();




}
