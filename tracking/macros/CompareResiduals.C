void CompareResiduals()
{

  TFile *f1 = new TFile("first.root", "read");
  //TDirectory *d1 = (TDirectory*) f1->Get("MyRecoMCTruthLinker");
  TH1F *D0 = (TH1F*)f1->Get("d0Residual");
  TH1F *Z0 = (TH1F*)f1->Get("z0Residual");
  TH1F *Omega = (TH1F*)f1->Get("OmegaResidual");
  TH1F *TanLambda = (TH1F*)f1->Get("TanLambdaResidual");
  TH1F *Phi = (TH1F*)f1->Get("PhiResidual");

  TFile *f2 = new TFile("second.root", "read");
  //TDirectory *d2 = (TDirectory*) f2->Get("MyRecoMCTruthLinker");
  TH1F *D02 = (TH1F*)f2->Get("d0Residual");
  TH1F *Z02 = (TH1F*)f2->Get("z0Residual");
  TH1F *Omega2 = (TH1F*)f2->Get("OmegaResidual");
  TH1F *TanLambda2 = (TH1F*)f2->Get("TanLambdaResidual");
  TH1F *Phi2 = (TH1F*)f2->Get("PhiResidual");

  TCanvas *rescomp =  new TCanvas("rescomp","Track par. residuals",800,800);

  rescomp->Divide(3,2);

  rescomp->cd(1);
  Omega->Draw();
  Omega->SetLineColor(kRed);
  Omega2->Draw("same");

  rescomp->cd(2);
  Phi->Draw();
  Phi->SetLineColor(kRed);
  Phi2->Draw("same");

  rescomp->cd(3);
  TanLambda->Draw();
  TanLambda->SetLineColor(kRed);
  TanLambda2->Draw("same");

  rescomp->cd(4);
  D0->Draw();
  D0->SetLineColor(kRed);
  D02->Draw("same");

  rescomp->cd(5);
  Z0->Draw();
  Z0->SetLineColor(kRed);
  Z02->Draw("same");

  rescomp->cd(6);

  leg = new TLegend(0.2,0.26,0.75,0.88);
  leg->AddEntry(Phi,"starting helix","l");
  leg->AddEntry(Phi2,"helix refitted with aida","l");
  leg->Draw();




}
