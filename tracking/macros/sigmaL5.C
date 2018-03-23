#include "TH1F.h"
#include "TF1.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TTree.h"
#include "TFile.h"
#include "iostream"
#include "vector"
#include "TLegend.h"
#include "TGraphErrors.h"
#include <math.h>  
#include "TLegend.h"
using namespace std;

#define SIZE_M 7
#define SIZE_PA 4

double sigmaOmega[SIZE_M][SIZE_PA], error_sigmaOmega[SIZE_M][SIZE_PA];
double sigmaTanLambda[SIZE_M][SIZE_PA], error_sigmaTanLambda[SIZE_M][SIZE_PA];
double sigmaPhi[SIZE_M][SIZE_PA], error_sigmaPhi[SIZE_M][SIZE_PA];
double sigmaz0[SIZE_M][SIZE_PA], error_sigmaz0[SIZE_M][SIZE_PA];
double sigmad0[SIZE_M][SIZE_PA], error_sigmad0[SIZE_M][SIZE_PA];

int Mom[SIZE_M] = {1, 3, 5, 10, 15, 25, 50};
float Momentum[SIZE_M] = {1, 3, 5, 10, 15, 25, 50};
float zeros[SIZE_M];
int PA[SIZE_PA] = {10, 20, 40, 85};
float LimAxis;
int color, marker;

void sigmaL5(){
  for (int ll = 0; ll < SIZE_M; ll++){  
    zeros[ll] = 0;
  }

  float sigmaOmega10[SIZE_M];
  float errorOmega10[SIZE_M];
  float sigmaOmega20[SIZE_M];
  float errorOmega20[SIZE_M];
  float sigmaOmega40[SIZE_M];
  float errorOmega40[SIZE_M];
  float sigmaOmega85[SIZE_M];
  float errorOmega85[SIZE_M];
  float sigmaTanLambda10[SIZE_M];
  float errorTanLambda10[SIZE_M];
  float sigmaTanLambda20[SIZE_M];
  float errorTanLambda20[SIZE_M];
  float sigmaTanLambda40[SIZE_M];
  float errorTanLambda40[SIZE_M];
  float sigmaTanLambda85[SIZE_M];
  float errorTanLambda85[SIZE_M];
  float sigmaPhi10[SIZE_M];
  float errorPhi10[SIZE_M];
  float sigmaPhi20[SIZE_M];
  float errorPhi20[SIZE_M];
  float sigmaPhi40[SIZE_M];
  float errorPhi40[SIZE_M];
  float sigmaPhi85[SIZE_M];
  float errorPhi85[SIZE_M];
  float sigmad010[SIZE_M];
  float errord010[SIZE_M];
  float sigmad020[SIZE_M];
  float errord020[SIZE_M];
  float sigmad040[SIZE_M];
  float errord040[SIZE_M];
  float sigmad085[SIZE_M];
  float errord085[SIZE_M];
  float sigmaz010[SIZE_M];
  float errorz010[SIZE_M];
  float sigmaz020[SIZE_M];
  float errorz020[SIZE_M];
  float sigmaz040[SIZE_M];
  float errorz040[SIZE_M];
  float sigmaz085[SIZE_M];
  float errorz085[SIZE_M];

  for(int ii = 0; ii < SIZE_PA; ii++){
    for (int i = 0; i < SIZE_M; i++){
      TFile *f1 = new TFile(Form("../Results/Analysis/analysis_ILD_l5_v02_MuonsAngle_%d_Mom_%d.root",PA[ii],Mom[i]), "read");
      //TDirectory *d1 = (TDirectory*) f1->Get("MyRecoMCTruthLinker");
      
      TH1F* hOmega = (TH1F*) f1->Get("OmegaPull");
      TH1F* hPhi = (TH1F*) f1->Get("PhiPull");
      TH1F* hTanLambda = (TH1F*) f1->Get("TanLambdaPull");
      TH1F* hd0 = (TH1F*) f1->Get("d0pull");	
      TH1F* hz0 = (TH1F*) f1->Get("z0pull");	

      if (hOmega->GetEntries()>50){

	hOmega->Fit("gaus");
	TF1 *fit1 = (TF1*)hOmega->GetFunction("gaus");
	sigmaOmega[i][ii] = fit1->GetParameter(2);
	error_sigmaOmega[i][ii] =  fit1->GetParError(2);
	
	hTanLambda->Fit("gaus");
	TF1 *fit2 = (TF1*)hTanLambda->GetFunction("gaus");
	sigmaTanLambda[i][ii] = fit2->GetParameter(2);
	error_sigmaTanLambda[i][ii] =  fit2->GetParError(2);
      
	hPhi->Fit("gaus");
	TF1 *fit3 = (TF1*)hPhi->GetFunction("gaus");
	sigmaPhi[i][ii] = fit3->GetParameter(2);
	error_sigmaPhi[i][ii] =  fit3->GetParError(2);
	
	hd0->Fit("gaus");
	TF1 *fit4 = (TF1*)hd0->GetFunction("gaus");
	sigmad0[i][ii] = fit4->GetParameter(2);
	error_sigmad0[i][ii] =  fit4->GetParError(2);
	
	hz0->Fit("gaus");
	TF1 *fit5 = (TF1*)hz0->GetFunction("gaus");
	sigmaz0[i][ii] = fit5->GetParameter(2);
	error_sigmaz0[i][ii] =  fit5->GetParError(2);
	

      }//loop for each .root files, here I have collected sigma and its error
    }//polar angle loop
    
    
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaOmega10[kk] = sigmaOmega[kk][0];
      errorOmega10[kk] = error_sigmaOmega[kk][0];
    }  
    
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaOmega20[kk] = sigmaOmega[kk][1];
      errorOmega20[kk] = error_sigmaOmega[kk][1];
    }  
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaOmega40[kk] = sigmaOmega[kk][2];
      errorOmega40[kk] = error_sigmaOmega[kk][2];
    }
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaOmega85[kk] = sigmaOmega[kk][3];
      errorOmega85[kk] = error_sigmaOmega[kk][3];
    }
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaTanLambda10[kk] = sigmaTanLambda[kk][0];
      errorTanLambda10[kk] = error_sigmaTanLambda[kk][0];
    }  
    

    for(int kk=0; kk < SIZE_M; kk++){
      sigmaTanLambda20[kk] = sigmaTanLambda[kk][1];
      errorTanLambda20[kk] = error_sigmaTanLambda[kk][1];
    }  
  
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaTanLambda40[kk] = sigmaTanLambda[kk][2];
      errorTanLambda40[kk] = error_sigmaTanLambda[kk][2];
    }
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaTanLambda85[kk] = sigmaTanLambda[kk][3];
      errorTanLambda85[kk] = error_sigmaTanLambda[kk][3];
    }
    
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaPhi10[kk] = sigmaPhi[kk][0];
      errorPhi10[kk] = error_sigmaPhi[kk][0];
    }  
    
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaPhi20[kk] = sigmaPhi[kk][1];
      errorPhi20[kk] = error_sigmaPhi[kk][1];
    }  
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaPhi40[kk] = sigmaPhi[kk][2];
      errorPhi40[kk] = error_sigmaPhi[kk][2];
    }
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaPhi85[kk] = sigmaPhi[kk][3];
      errorPhi85[kk] = error_sigmaPhi[kk][3];
    }
    
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmad010[kk] = sigmad0[kk][0];
      errord010[kk] = error_sigmad0[kk][0];
    }  

    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmad020[kk] = sigmad0[kk][1];
      errord020[kk] = error_sigmad0[kk][1];
    }  
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmad040[kk] = sigmad0[kk][2];
      errord040[kk] = error_sigmad0[kk][2];
    }
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmad085[kk] = sigmad0[kk][3];
      errord085[kk] = error_sigmad0[kk][3];
    }
    

    for(int kk=0; kk < SIZE_M; kk++){
      sigmaz010[kk] = sigmaz0[kk][0];
      errorz010[kk] = error_sigmaz0[kk][0];
    }  
    
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaz020[kk] = sigmaz0[kk][1];
      errorz020[kk] = error_sigmaz0[kk][1];
    }  
  
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaz040[kk] = sigmaz0[kk][2];
      errorz040[kk] = error_sigmaz0[kk][2];
    }
    
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaz085[kk] = sigmaz0[kk][3];
      errorz085[kk] = error_sigmaz0[kk][3];
    }
    
    
  }
  
  TCanvas *c_two = new TCanvas("MomentumResolutionBar","Track Parameters #sigma",200,10,700,500);
   c_two->Divide(3,2);

   c_two->cd(1);
   c_two->cd(1)->SetLogx();
   c_two->cd(1)->SetTickx(1);
   c_two->cd(1)->SetTicky(1);

   TGraphErrors* Muon_plot_omega10  = new TGraphErrors(SIZE_M, Momentum, sigmaOmega10, zeros, errorOmega10);
   Muon_plot_omega10 -> SetTitle("#Omega");
   Muon_plot_omega10 -> SetMarkerColor(6);
   Muon_plot_omega10 -> SetMarkerStyle(24);
   Muon_plot_omega10 -> SetMarkerSize(1);
   Muon_plot_omega10 -> GetXaxis() -> SetTitle("P (GeV)");
   Muon_plot_omega10 -> GetYaxis() -> SetTitle("#sigma = f(P,#theta)");
   Muon_plot_omega10 -> SetMinimum( 0.70 );
   Muon_plot_omega10 -> SetMaximum( 1.50 );
   Muon_plot_omega10 -> Draw("AP");
   
   
   TGraphErrors* Muon_plot_omega20  = new TGraphErrors(SIZE_M, Momentum, sigmaOmega20, zeros, errorOmega20);
   Muon_plot_omega20 -> SetMarkerColor(4);
   Muon_plot_omega20 -> SetMarkerStyle(22);
   Muon_plot_omega20 -> SetMarkerSize(1);
   Muon_plot_omega20 -> Draw("P");
  
   TGraphErrors *Muon_plot_omega40 = new TGraphErrors(SIZE_M, Momentum, sigmaOmega40,  zeros, errorOmega40);
   Muon_plot_omega40 -> SetMarkerColor(2);
   Muon_plot_omega40 -> SetMarkerStyle(21);
   Muon_plot_omega40 -> SetMarkerSize(1);
   Muon_plot_omega40 -> Draw("P");
   
   TGraphErrors *Muon_plot_omega85 = new TGraphErrors(SIZE_M, Momentum, sigmaOmega85,  zeros, errorOmega85);
   Muon_plot_omega85 -> SetMarkerColor(1);
   Muon_plot_omega85 -> SetMarkerStyle(20);
   Muon_plot_omega85 -> SetMarkerSize(1);
   Muon_plot_omega85 -> Draw("P");

   
   
   c_two->cd(2);
   c_two->cd(2)->SetLogx();
   c_two->cd(2)->SetTickx(1);
   c_two->cd(2)->SetTicky(1);

   TGraphErrors* Muon_plot_tanlambda10  = new TGraphErrors(SIZE_M, Momentum, sigmaTanLambda10, zeros, errorTanLambda10);
   Muon_plot_tanlambda10 -> SetTitle("tan #lambda");
   Muon_plot_tanlambda10 -> SetMarkerColor(6);
   Muon_plot_tanlambda10 -> SetMarkerStyle(24);
   Muon_plot_tanlambda10 -> SetMarkerSize(1);
   Muon_plot_tanlambda10 -> GetXaxis() -> SetTitle("P (GeV)");
   Muon_plot_tanlambda10 -> GetYaxis() -> SetTitle("#sigma = f(P,#theta)");
   Muon_plot_tanlambda10 -> SetMinimum( 0.70 );
   Muon_plot_tanlambda10 -> SetMaximum( 1.50 );
   Muon_plot_tanlambda10 -> Draw("AP");
   
   
   TGraphErrors* Muon_plot_tanlambda20  = new TGraphErrors(SIZE_M, Momentum, sigmaTanLambda20, zeros, errorTanLambda20);
   Muon_plot_tanlambda20 -> SetMarkerColor(4);
   Muon_plot_tanlambda20 -> SetMarkerStyle(22);
   Muon_plot_tanlambda20 -> SetMarkerSize(1);
   Muon_plot_tanlambda20 -> Draw("P");
   
   TGraphErrors *Muon_plot_tanlambda40 = new TGraphErrors(SIZE_M, Momentum, sigmaTanLambda40,  zeros, errorTanLambda40);
   Muon_plot_tanlambda40 -> SetMarkerColor(2);
   Muon_plot_tanlambda40 -> SetMarkerStyle(21);
   Muon_plot_tanlambda40 -> SetMarkerSize(1);
   Muon_plot_tanlambda40 -> Draw("P");
   
   TGraphErrors *Muon_plot_tanlambda85 = new TGraphErrors(SIZE_M, Momentum, sigmaTanLambda85,  zeros, errorTanLambda85);
   Muon_plot_tanlambda85 -> SetMarkerColor(1);
   Muon_plot_tanlambda85 -> SetMarkerStyle(20);
   Muon_plot_tanlambda85 -> SetMarkerSize(1);
   Muon_plot_tanlambda85 -> Draw("P");

   
   c_two->cd(3);
   c_two->cd(3)->SetLogx();
   c_two->cd(3)->SetTickx(1);
   c_two->cd(3)->SetTicky(1);

   TGraphErrors* Muon_plot_phi10  = new TGraphErrors(SIZE_M, Momentum, sigmaPhi10, zeros, errorPhi10);
   Muon_plot_phi10 -> SetTitle("#phi");
   Muon_plot_phi10 -> SetMarkerColor(6);
   Muon_plot_phi10 -> SetMarkerStyle(24);
   Muon_plot_phi10 -> SetMarkerSize(1);
   Muon_plot_phi10 -> GetXaxis() -> SetTitle("P (GeV)");
   Muon_plot_phi10 -> GetYaxis() -> SetTitle("#sigma = f(P,#theta)");
   Muon_plot_phi10 -> SetMinimum( 0.70 );
   Muon_plot_phi10 -> SetMaximum( 1.50 );
   Muon_plot_phi10 -> Draw("AP");
   
   
   TGraphErrors* Muon_plot_phi20  = new TGraphErrors(SIZE_M, Momentum, sigmaPhi20, zeros, errorPhi20);
   Muon_plot_phi20 -> SetMarkerColor(4);
   Muon_plot_phi20 -> SetMarkerStyle(22);
   Muon_plot_phi20 -> SetMarkerSize(1);
   Muon_plot_phi20 -> Draw("P");
  
   TGraphErrors *Muon_plot_phi40 = new TGraphErrors(SIZE_M, Momentum, sigmaPhi40,  zeros, errorPhi40);
   Muon_plot_phi40 -> SetMarkerColor(2);
   Muon_plot_phi40 -> SetMarkerStyle(21);
   Muon_plot_phi40 -> SetMarkerSize(1);
   Muon_plot_phi40 -> Draw("P");
   
   TGraphErrors *Muon_plot_phi85 = new TGraphErrors(SIZE_M, Momentum, sigmaPhi85,  zeros, errorPhi85);
   Muon_plot_phi85 -> SetMarkerColor(1);
   Muon_plot_phi85 -> SetMarkerStyle(20);
   Muon_plot_phi85 -> SetMarkerSize(1);
   Muon_plot_phi85 -> Draw("P");
   
   
   
   c_two->cd(4);
   c_two->cd(4)->SetLogx();
   c_two->cd(4)->SetTickx(1);
   c_two->cd(4)->SetTicky(1);

   TGraphErrors* Muon_plot_d010  = new TGraphErrors(SIZE_M, Momentum, sigmad010, zeros, errord010);
   Muon_plot_d010 -> SetTitle("d0");
   Muon_plot_d010 -> SetMarkerColor(6);
   Muon_plot_d010 -> SetMarkerStyle(24);
   Muon_plot_d010 -> SetMarkerSize(1);
   Muon_plot_d010 -> GetXaxis() -> SetTitle("P (GeV)");
   Muon_plot_d010 -> GetYaxis() -> SetTitle("#sigma = f(P,#theta)");
   Muon_plot_d010 -> SetMinimum( 0.70 );
   Muon_plot_d010 -> SetMaximum( 1.50 );
   Muon_plot_d010 -> Draw("AP");
   

   TGraphErrors* Muon_plot_d020  = new TGraphErrors(SIZE_M, Momentum, sigmad020, zeros, errord020);
   Muon_plot_d020 -> SetMarkerColor(4);
   Muon_plot_d020 -> SetMarkerStyle(22);
   Muon_plot_d020 -> SetMarkerSize(1);
   
   Muon_plot_d020 -> Draw("P");
   
   TGraphErrors *Muon_plot_d040 = new TGraphErrors(SIZE_M, Momentum, sigmad040,  zeros, errord040);
   Muon_plot_d040 -> SetMarkerColor(2);
   Muon_plot_d040 -> SetMarkerStyle(21);
   Muon_plot_d040 -> SetMarkerSize(1);
   Muon_plot_d040 -> Draw("P");
   
   TGraphErrors *Muon_plot_d085 = new TGraphErrors(SIZE_M, Momentum, sigmad085,  zeros, errord085);
   Muon_plot_d085 -> SetMarkerColor(1);
   Muon_plot_d085 -> SetMarkerStyle(20);
   Muon_plot_d085 -> SetMarkerSize(1);
   Muon_plot_d085 -> Draw("P");
   
   
   c_two->cd(5);
   c_two->cd(5)->SetLogx();
   c_two->cd(5)->SetTickx(1);
   c_two->cd(5)->SetTicky(1);

   TGraphErrors* Muon_plot_z010  = new TGraphErrors(SIZE_M, Momentum, sigmaz010, zeros, errorz010);
   Muon_plot_z010 -> SetTitle("z0");
   Muon_plot_z010 -> SetMarkerColor(6);
   Muon_plot_z010 -> SetMarkerStyle(24);
   Muon_plot_z010 -> SetMarkerSize(1);
   Muon_plot_z010 -> GetXaxis() -> SetTitle("P (GeV)");
   Muon_plot_z010 -> GetYaxis() -> SetTitle("#sigma = f(P,#theta)");
   Muon_plot_z010 -> SetMinimum( 0.70 );
   Muon_plot_z010 -> SetMaximum( 1.50 );
   Muon_plot_z010 -> Draw("AP");
   
   
   TGraphErrors* Muon_plot_z020  = new TGraphErrors(SIZE_M, Momentum, sigmaz020, zeros, errorz020);
   Muon_plot_z020 -> SetMarkerColor(4);
   Muon_plot_z020 -> SetMarkerStyle(22);
   Muon_plot_z020 -> SetMarkerSize(1);
   Muon_plot_z020 -> Draw("P");
   
   TGraphErrors *Muon_plot_z040 = new TGraphErrors(SIZE_M, Momentum, sigmaz040,  zeros, errorz040);
   Muon_plot_z040 -> SetMarkerColor(2);
   Muon_plot_z040 -> SetMarkerStyle(21);
   Muon_plot_z040 -> SetMarkerSize(1);
   Muon_plot_z040 -> Draw("P");
   
   TGraphErrors *Muon_plot_z085 = new TGraphErrors(SIZE_M, Momentum, sigmaz085,  zeros, errorz085);
   Muon_plot_z085 -> SetMarkerColor(1);
   Muon_plot_z085 -> SetMarkerStyle(20);
   Muon_plot_z085 -> SetMarkerSize(1);
   Muon_plot_z085 -> Draw("P");
   
   

   c_two->cd(6);
   TLegend *leg = new TLegend(0.5,0.5,0.75,0.89);
   //leg->SetHeader("Polar Angles"); //name of the legend
   leg->AddEntry(Muon_plot_z010,"#theta = 10^{o}","p");
   leg->AddEntry(Muon_plot_z020,"#theta = 20^{o}","p");
   leg->AddEntry(Muon_plot_z040,"#theta = 40^{o}","p");
   leg->AddEntry(Muon_plot_z085,"#theta = 85^{o}","p");
   leg->Draw();
   
   c_two->SaveAs("pull_sigma_ILD_l5_v02.png");

}
