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

double meanOmega[SIZE_M][SIZE_PA], error_meanOmega[SIZE_M][SIZE_PA];
double meanTanLambda[SIZE_M][SIZE_PA], error_meanTanLambda[SIZE_M][SIZE_PA];
double meanPhi[SIZE_M][SIZE_PA], error_meanPhi[SIZE_M][SIZE_PA];
double meanz0[SIZE_M][SIZE_PA], error_meanz0[SIZE_M][SIZE_PA];
double meand0[SIZE_M][SIZE_PA], error_meand0[SIZE_M][SIZE_PA];

int Mom[SIZE_M] = {1, 3, 5, 10, 15, 25, 50};
float Momentum[SIZE_M] = {1, 3, 5, 10, 15, 25, 50};
float zeros[SIZE_M];
int PA[SIZE_PA] = {10, 20, 40, 85};
float LimAxis;
int color, marker;

void mean(){
  for (int ll = 0; ll < SIZE_M; ll++){  
    zeros[ll] = 0;
  }
  for(int ii = 0; ii < SIZE_PA; ii++){
    for (int i = 0; i < SIZE_M; i++){
      TFile *f1 = new TFile(Form("../Results/Analysis/analysis_MuonsAngle_%d_Mom_%d.root",PA[ii],Mom[i]), "read");
      TDirectory *d1 = (TDirectory*) f1->Get("MyRecoMCTruthLinker");
      
      TH1F* hOmega = (TH1F*) d1->Get("OmegaPull");
      TH1F* hPhi = (TH1F*) d1->Get("PhiPull");
      TH1F* hTanLambda = (TH1F*) d1->Get("TanLambdaPull");
      TH1F* hd0 = (TH1F*) d1->Get("d0pull");	
      TH1F* hz0 = (TH1F*) d1->Get("z0pull");	

      if (hOmega->GetEntries()>50){

	hOmega->Fit("gaus");
	TF1 *fit1 = (TF1*)hOmega->GetFunction("gaus");
	meanOmega[i][ii] = fit1->GetParameter(1);
	error_meanOmega[i][ii] =  fit1->GetParError(1);
	
	hTanLambda->Fit("gaus");
	TF1 *fit2 = (TF1*)hTanLambda->GetFunction("gaus");
	meanTanLambda[i][ii] = fit2->GetParameter(1);
	error_meanTanLambda[i][ii] =  fit2->GetParError(1);
      
	hPhi->Fit("gaus");
	TF1 *fit3 = (TF1*)hPhi->GetFunction("gaus");
	meanPhi[i][ii] = fit3->GetParameter(1);
	error_meanPhi[i][ii] =  fit3->GetParError(1);
	
	hd0->Fit("gaus");
	TF1 *fit4 = (TF1*)hd0->GetFunction("gaus");
	meand0[i][ii] = fit4->GetParameter(1);
	error_meand0[i][ii] =  fit4->GetParError(1);
	
	hz0->Fit("gaus");
	TF1 *fit5 = (TF1*)hz0->GetFunction("gaus");
	meanz0[i][ii] = fit5->GetParameter(1);
	error_meanz0[i][ii] =  fit5->GetParError(1);
	

      }//loop for each .root files, here I have collected mean and its error
    }//polar angle loop
    
    
    
    float meanOmega10[SIZE_M];
    float errorOmega10[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanOmega10[kk] = meanOmega[kk][0];
      errorOmega10[kk] = error_meanOmega[kk][0];
    }  
    
    
    float meanOmega20[SIZE_M];
    float errorOmega20[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanOmega20[kk] = meanOmega[kk][1];
      errorOmega20[kk] = error_meanOmega[kk][1];
    }  
    
    float meanOmega40[SIZE_M];
    float errorOmega40[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanOmega40[kk] = meanOmega[kk][2];
      errorOmega40[kk] = error_meanOmega[kk][2];
    }
    
    float meanOmega85[SIZE_M];
    float errorOmega85[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanOmega85[kk] = meanOmega[kk][3];
      errorOmega85[kk] = error_meanOmega[kk][3];
    }
    
    float meanTanLambda10[SIZE_M];
    float errorTanLambda10[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanTanLambda10[kk] = meanTanLambda[kk][0];
      errorTanLambda10[kk] = error_meanTanLambda[kk][0];
    }  
    

    float meanTanLambda20[SIZE_M];
    float errorTanLambda20[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanTanLambda20[kk] = meanTanLambda[kk][1];
      errorTanLambda20[kk] = error_meanTanLambda[kk][1];
    }  
  
    float meanTanLambda40[SIZE_M];
    float errorTanLambda40[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanTanLambda40[kk] = meanTanLambda[kk][2];
      errorTanLambda40[kk] = error_meanTanLambda[kk][2];
    }
    
    float meanTanLambda85[SIZE_M];
    float errorTanLambda85[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanTanLambda85[kk] = meanTanLambda[kk][3];
      errorTanLambda85[kk] = error_meanTanLambda[kk][3];
    }
    
    
    float meanPhi10[SIZE_M];
    float errorPhi10[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanPhi10[kk] = meanPhi[kk][0];
      errorPhi10[kk] = error_meanPhi[kk][0];
    }  
    
    
    float meanPhi20[SIZE_M];
    float errorPhi20[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanPhi20[kk] = meanPhi[kk][1];
      errorPhi20[kk] = error_meanPhi[kk][1];
    }  
    
    float meanPhi40[SIZE_M];
    float errorPhi40[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanPhi40[kk] = meanPhi[kk][2];
      errorPhi40[kk] = error_meanPhi[kk][2];
    }
    
    float meanPhi85[SIZE_M];
    float errorPhi85[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanPhi85[kk] = meanPhi[kk][3];
      errorPhi85[kk] = error_meanPhi[kk][3];
    }
    
    
    float meand010[SIZE_M];
    float errord010[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meand010[kk] = meand0[kk][0];
      errord010[kk] = error_meand0[kk][0];
    }  

    
    float meand020[SIZE_M];
    float errord020[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meand020[kk] = meand0[kk][1];
      errord020[kk] = error_meand0[kk][1];
    }  
    
    float meand040[SIZE_M];
    float errord040[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meand040[kk] = meand0[kk][2];
      errord040[kk] = error_meand0[kk][2];
    }
    
    float meand085[SIZE_M];
    float errord085[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meand085[kk] = meand0[kk][3];
      errord085[kk] = error_meand0[kk][3];
    }
    

    float meanz010[SIZE_M];
    float errorz010[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanz010[kk] = meanz0[kk][0];
      errorz010[kk] = error_meanz0[kk][0];
    }  
    
    
    float meanz020[SIZE_M];
    float errorz020[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanz020[kk] = meanz0[kk][1];
      errorz020[kk] = error_meanz0[kk][1];
    }  
  
    float meanz040[SIZE_M];
    float errorz040[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanz040[kk] = meanz0[kk][2];
      errorz040[kk] = error_meanz0[kk][2];
    }
    
    float meanz085[SIZE_M];
    float errorz085[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      meanz085[kk] = meanz0[kk][3];
      errorz085[kk] = error_meanz0[kk][3];
    }
    
    

    TCanvas *c_two = new TCanvas("MomentumResolutionBar","Track Parameters #mean",200,10,700,500);
    c_two->Divide(3,2);
    c_two->cd(1);
    //TGraph *Muon_plot20 = new TGraph(SIZE_M, Momentum, mean20);
  
    TGraphErrors* Muon_plot_omega10  = new TGraphErrors(SIZE_M, Momentum, meanOmega10, zeros, errorOmega10);
    Muon_plot_omega10 -> SetTitle("#omega");
    Muon_plot_omega10 -> SetMarkerColor(2);
    Muon_plot_omega10 -> SetMarkerStyle(24);
    Muon_plot_omega10 -> SetMarkerSize(1);
    Muon_plot_omega10 -> GetXaxis() -> SetTitle("P (GeV)");
    Muon_plot_omega10 -> GetYaxis() -> SetTitle("#mean = f(P,#theta)");
    Muon_plot_omega10 -> SetMinimum( -2. );
    Muon_plot_omega10 -> SetMaximum( 2. );
    Muon_plot_omega10 -> Draw("AP");
    
    
    TGraphErrors* Muon_plot_omega20  = new TGraphErrors(SIZE_M, Momentum, meanOmega20, zeros, errorOmega20);
    Muon_plot_omega20 -> SetMarkerColor(4);
    Muon_plot_omega20 -> SetMarkerStyle(22);
    Muon_plot_omega20 -> SetMarkerSize(1);
    Muon_plot_omega20 -> Draw("P");
  
    TGraphErrors *Muon_plot_omega40 = new TGraphErrors(SIZE_M, Momentum, meanOmega40,  zeros, errorOmega40);
    Muon_plot_omega40 -> SetMarkerColor(2);
    Muon_plot_omega40 -> SetMarkerStyle(21);
    Muon_plot_omega40 -> SetMarkerSize(1);
    Muon_plot_omega40 -> Draw("P");
    
    TGraphErrors *Muon_plot_omega85 = new TGraphErrors(SIZE_M, Momentum, meanOmega85,  zeros, errorOmega85);
    Muon_plot_omega85 -> SetMarkerColor(1);
    Muon_plot_omega85 -> SetMarkerStyle(20);
    Muon_plot_omega85 -> SetMarkerSize(1);
    Muon_plot_omega85 -> Draw("P");
    c_two->SetTickx(1);
    c_two->SetTicky(1);
    
    
    c_two->cd(2);
    TGraphErrors* Muon_plot_tanlambda10  = new TGraphErrors(SIZE_M, Momentum, meanTanLambda10, zeros, errorTanLambda10);
    Muon_plot_tanlambda10 -> SetTitle("tan #lambda");
    Muon_plot_tanlambda10 -> SetMarkerColor(2);
    Muon_plot_tanlambda10 -> SetMarkerStyle(24);
    Muon_plot_tanlambda10 -> SetMarkerSize(1);
    Muon_plot_tanlambda10 -> GetXaxis() -> SetTitle("P (GeV)");
    Muon_plot_tanlambda10 -> GetYaxis() -> SetTitle("#mean = f(P,#theta)");
    Muon_plot_tanlambda10 -> SetMinimum( -2. );
    Muon_plot_tanlambda10 -> SetMaximum( 2.00 );
    Muon_plot_tanlambda10 -> Draw("AP");
    
    
    TGraphErrors* Muon_plot_tanlambda20  = new TGraphErrors(SIZE_M, Momentum, meanTanLambda20, zeros, errorTanLambda20);
    Muon_plot_tanlambda20 -> SetMarkerColor(4);
    Muon_plot_tanlambda20 -> SetMarkerStyle(22);
    Muon_plot_tanlambda20 -> SetMarkerSize(1);
    Muon_plot_tanlambda20 -> Draw("P");
    
    TGraphErrors *Muon_plot_tanlambda40 = new TGraphErrors(SIZE_M, Momentum, meanTanLambda40,  zeros, errorTanLambda40);
    Muon_plot_tanlambda40 -> SetMarkerColor(2);
    Muon_plot_tanlambda40 -> SetMarkerStyle(21);
    Muon_plot_tanlambda40 -> SetMarkerSize(1);
    Muon_plot_tanlambda40 -> Draw("P");
    
    TGraphErrors *Muon_plot_tanlambda85 = new TGraphErrors(SIZE_M, Momentum, meanTanLambda85,  zeros, errorTanLambda85);
    Muon_plot_tanlambda85 -> SetMarkerColor(1);
    Muon_plot_tanlambda85 -> SetMarkerStyle(20);
    Muon_plot_tanlambda85 -> SetMarkerSize(1);
    Muon_plot_tanlambda85 -> Draw("P");
    c_two->SetTickx(1);
    c_two->SetTicky(1);
    
    
    c_two->cd(3);
    TGraphErrors* Muon_plot_phi10  = new TGraphErrors(SIZE_M, Momentum, meanPhi10, zeros, errorPhi10);
    Muon_plot_phi10 -> SetTitle("#phi");
    Muon_plot_phi10 -> SetMarkerColor(2);
    Muon_plot_phi10 -> SetMarkerStyle(24);
    Muon_plot_phi10 -> SetMarkerSize(1);
    Muon_plot_phi10 -> GetXaxis() -> SetTitle("P (GeV)");
    Muon_plot_phi10 -> GetYaxis() -> SetTitle("#mean = f(P,#theta)");
    Muon_plot_phi10 -> SetMinimum( -2. );
    Muon_plot_phi10 -> SetMaximum( 2.00 );
    Muon_plot_phi10 -> Draw("AP");
    
    
    TGraphErrors* Muon_plot_phi20  = new TGraphErrors(SIZE_M, Momentum, meanPhi20, zeros, errorPhi20);
    Muon_plot_phi20 -> SetMarkerColor(4);
    Muon_plot_phi20 -> SetMarkerStyle(22);
    Muon_plot_phi20 -> SetMarkerSize(1);
    Muon_plot_phi20 -> Draw("P");
  
    TGraphErrors *Muon_plot_phi40 = new TGraphErrors(SIZE_M, Momentum, meanPhi40,  zeros, errorPhi40);
    Muon_plot_phi40 -> SetMarkerColor(2);
    Muon_plot_phi40 -> SetMarkerStyle(21);
    Muon_plot_phi40 -> SetMarkerSize(1);
    Muon_plot_phi40 -> Draw("P");
    
    TGraphErrors *Muon_plot_phi85 = new TGraphErrors(SIZE_M, Momentum, meanPhi85,  zeros, errorPhi85);
    Muon_plot_phi85 -> SetMarkerColor(1);
    Muon_plot_phi85 -> SetMarkerStyle(20);
    Muon_plot_phi85 -> SetMarkerSize(1);
    Muon_plot_phi85 -> Draw("P");
    c_two->SetTickx(1);
    c_two->SetTicky(1);
    
    
    
    
    c_two->cd(4);

    TGraphErrors* Muon_plot_d010  = new TGraphErrors(SIZE_M, Momentum, meand010, zeros, errord010);
    Muon_plot_d010 -> SetTitle("d0");
    Muon_plot_d010 -> SetMarkerColor(2);
    Muon_plot_d010 -> SetMarkerStyle(24);
    Muon_plot_d010 -> SetMarkerSize(1);
    Muon_plot_d010 -> GetXaxis() -> SetTitle("P (GeV)");
    Muon_plot_d010 -> GetYaxis() -> SetTitle("#mean = f(P,#theta)");
    Muon_plot_d010 -> SetMinimum( -2. );
    Muon_plot_d010 -> SetMaximum( 2.00 );
    Muon_plot_d010 -> Draw("AP");
    

    TGraphErrors* Muon_plot_d020  = new TGraphErrors(SIZE_M, Momentum, meand020, zeros, errord020);
    Muon_plot_d020 -> SetMarkerColor(4);
    Muon_plot_d020 -> SetMarkerStyle(22);
    Muon_plot_d020 -> SetMarkerSize(1);
    
    Muon_plot_d020 -> Draw("P");
    
    TGraphErrors *Muon_plot_d040 = new TGraphErrors(SIZE_M, Momentum, meand040,  zeros, errord040);
    Muon_plot_d040 -> SetMarkerColor(2);
    Muon_plot_d040 -> SetMarkerStyle(21);
    Muon_plot_d040 -> SetMarkerSize(1);
    Muon_plot_d040 -> Draw("P");
    
    TGraphErrors *Muon_plot_d085 = new TGraphErrors(SIZE_M, Momentum, meand085,  zeros, errord085);
    Muon_plot_d085 -> SetMarkerColor(1);
    Muon_plot_d085 -> SetMarkerStyle(20);
    Muon_plot_d085 -> SetMarkerSize(1);
    Muon_plot_d085 -> Draw("P");
    c_two->SetTickx(1);
    c_two->SetTicky(1);

    
    
    c_two->cd(5);
    TGraphErrors* Muon_plot_z010  = new TGraphErrors(SIZE_M, Momentum, meanz010, zeros, errorz010);
    Muon_plot_z010 -> SetTitle("z0");
    Muon_plot_z010 -> SetMarkerColor(2);
    Muon_plot_z010 -> SetMarkerStyle(24);
    Muon_plot_z010 -> SetMarkerSize(1);
    Muon_plot_z010 -> GetXaxis() -> SetTitle("P (GeV)");
    Muon_plot_z010 -> GetYaxis() -> SetTitle("#mean = f(P,#theta)");
    Muon_plot_z010 -> SetMinimum( -2. );
    Muon_plot_z010 -> SetMaximum( 2.00 );
    Muon_plot_z010 -> Draw("AP");
    
    
    TGraphErrors* Muon_plot_z020  = new TGraphErrors(SIZE_M, Momentum, meanz020, zeros, errorz020);
    Muon_plot_z020 -> SetMarkerColor(4);
    Muon_plot_z020 -> SetMarkerStyle(22);
    Muon_plot_z020 -> SetMarkerSize(1);
    Muon_plot_z020 -> Draw("P");
    
    TGraphErrors *Muon_plot_z040 = new TGraphErrors(SIZE_M, Momentum, meanz040,  zeros, errorz040);
    Muon_plot_z040 -> SetMarkerColor(2);
    Muon_plot_z040 -> SetMarkerStyle(21);
    Muon_plot_z040 -> SetMarkerSize(1);
    Muon_plot_z040 -> Draw("P");
    
    TGraphErrors *Muon_plot_z085 = new TGraphErrors(SIZE_M, Momentum, meanz085,  zeros, errorz085);
    Muon_plot_z085 -> SetMarkerColor(1);
    Muon_plot_z085 -> SetMarkerStyle(20);
    Muon_plot_z085 -> SetMarkerSize(1);
    Muon_plot_z085 -> Draw("P");
    c_two->SetTickx(1);
    c_two->SetTicky(1);
    
    
    







    c_two->cd(6);
    TLegend *leg = new TLegend(0.6,0.7,0.75,0.95);
    //leg->SetHeader("Polar Angles"); //name of the legend
    leg->AddEntry(Muon_plot_omega10,"#theta = 10^{o}","p");
    leg->AddEntry(Muon_plot_omega20,"#theta = 20^{o}","p");
    leg->AddEntry(Muon_plot_omega40,"#theta = 40^{o}","p");
    leg->AddEntry(Muon_plot_omega85,"#theta = 85^{o}","p");
    leg->Draw();
    
  }
  
}
