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

#define SIZE_M 8
#define SIZE_PA 4

double sigmaOmega[SIZE_M][SIZE_PA], error_sigmaOmega[SIZE_M][SIZE_PA];
double sigmaTanLambda[SIZE_M][SIZE_PA], error_sigmaTanLambda[SIZE_M][SIZE_PA];
double sigmaPhi[SIZE_M][SIZE_PA], error_sigmaPhi[SIZE_M][SIZE_PA];
double sigmaz0[SIZE_M][SIZE_PA], error_sigmaz0[SIZE_M][SIZE_PA];
double sigmad0[SIZE_M][SIZE_PA], error_sigmad0[SIZE_M][SIZE_PA];

int Mom[SIZE_M] = {1, 3, 5, 10, 15, 25, 50, 100};
float Momentum[SIZE_M] = {1, 3, 5, 10, 15, 25, 50, 100};
float zeros[SIZE_M];
int PA[SIZE_PA] = {10, 20, 40, 85};
float LimAxis;
int color, marker;

void sigma(){
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
	sigmaOmega[i][ii] = fit1->GetParameter(2);
	error_sigmaOmega[i][ii] =  fit1->GetParError(2);
	
	hTanLambda->Fit("gaus");
	TF1 *fit1 = (TF1*)hTanLambda->GetFunction("gaus");
	sigmaTanLambda[i][ii] = fit1->GetParameter(2);
	error_sigmaTanLambda[i][ii] =  fit1->GetParError(2);
      
	hPhi->Fit("gaus");
	TF1 *fit1 = (TF1*)hPhi->GetFunction("gaus");
	sigmaPhi[i][ii] = fit1->GetParameter(2);
	error_sigmaPhi[i][ii] =  fit1->GetParError(2);
	
	hd0->Fit("gaus");
	TF1 *fit1 = (TF1*)hd0->GetFunction("gaus");
	sigmad0[i][ii] = fit1->GetParameter(2);
	error_sigmad0[i][ii] =  fit1->GetParError(2);
	
	hz0->Fit("gaus");
	TF1 *fit1 = (TF1*)hz0->GetFunction("gaus");
	sigmaz0[i][ii] = fit1->GetParameter(2);
	error_sigmaz0[i][ii] =  fit1->GetParError(2);
	

      }//loop for each .root files, here I have collected sigma and its error
    }//polar angle loop
    
    
    
    float sigmaOmega10[SIZE_M];
    float errorOmega10[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaOmega10[kk] = sigmaOmega[kk][0];
      errorOmega10[kk] = error_sigmaOmega[kk][0];
    }  
    
    
    float sigmaOmega20[SIZE_M];
    float errorOmega20[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaOmega20[kk] = sigmaOmega[kk][1];
      errorOmega20[kk] = error_sigmaOmega[kk][1];
    }  
    
    float sigmaOmega40[SIZE_M];
    float errorOmega40[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaOmega40[kk] = sigmaOmega[kk][2];
      errorOmega40[kk] = error_sigmaOmega[kk][2];
    }
    
    float sigmaOmega85[SIZE_M];
    float errorOmega85[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaOmega85[kk] = sigmaOmega[kk][3];
      errorOmega85[kk] = error_sigmaOmega[kk][3];
    }
    
    float sigmaTanLambda10[SIZE_M];
    float errorTanLambda10[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaTanLambda10[kk] = sigmaTanLambda[kk][0];
      errorTanLambda10[kk] = error_sigmaTanLambda[kk][0];
    }  
    

    float sigmaTanLambda20[SIZE_M];
    float errorTanLambda20[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaTanLambda20[kk] = sigmaTanLambda[kk][1];
      errorTanLambda20[kk] = error_sigmaTanLambda[kk][1];
    }  
  
    float sigmaTanLambda40[SIZE_M];
    float errorTanLambda40[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaTanLambda40[kk] = sigmaTanLambda[kk][2];
      errorTanLambda40[kk] = error_sigmaTanLambda[kk][2];
    }
    
    float sigmaTanLambda85[SIZE_M];
    float errorTanLambda85[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaTanLambda85[kk] = sigmaTanLambda[kk][3];
      errorTanLambda85[kk] = error_sigmaTanLambda[kk][3];
    }
    
    
    float sigmaPhi10[SIZE_M];
    float errorPhi10[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaPhi10[kk] = sigmaPhi[kk][0];
      errorPhi10[kk] = error_sigmaPhi[kk][0];
    }  
    
    
    float sigmaPhi20[SIZE_M];
    float errorPhi20[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaPhi20[kk] = sigmaPhi[kk][1];
      errorPhi20[kk] = error_sigmaPhi[kk][1];
    }  
    
    float sigmaPhi40[SIZE_M];
    float errorPhi40[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaPhi40[kk] = sigmaPhi[kk][2];
      errorPhi40[kk] = error_sigmaPhi[kk][2];
    }
    
    float sigmaPhi85[SIZE_M];
    float errorPhi85[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaPhi85[kk] = sigmaPhi[kk][3];
      errorPhi85[kk] = error_sigmaPhi[kk][3];
    }
    
    
    float sigmad010[SIZE_M];
    float errord010[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmad010[kk] = sigmad0[kk][0];
      errord010[kk] = error_sigmad0[kk][0];
    }  

    
    float sigmad020[SIZE_M];
    float errord020[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmad020[kk] = sigmad0[kk][1];
      errord020[kk] = error_sigmad0[kk][1];
    }  
    
    float sigmad040[SIZE_M];
    float errord040[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmad040[kk] = sigmad0[kk][2];
      errord040[kk] = error_sigmad0[kk][2];
    }
    
    float sigmad085[SIZE_M];
    float errord085[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmad085[kk] = sigmad0[kk][3];
      errord085[kk] = error_sigmad0[kk][3];
    }
    

    float sigmaz010[SIZE_M];
    float errorz010[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaz010[kk] = sigmaz0[kk][0];
      errorz010[kk] = error_sigmaz0[kk][0];
    }  
    
    
    float sigmaz020[SIZE_M];
    float errorz020[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaz020[kk] = sigmaz0[kk][1];
      errorz020[kk] = error_sigmaz0[kk][1];
    }  
  
    float sigmaz040[SIZE_M];
    float errorz040[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaz040[kk] = sigmaz0[kk][2];
      errorz040[kk] = error_sigmaz0[kk][2];
    }
    
    float sigmaz085[SIZE_M];
    float errorz085[SIZE_M];
    for(int kk=0; kk < SIZE_M; kk++){
      sigmaz085[kk] = sigmaz0[kk][3];
      errorz085[kk] = error_sigmaz0[kk][3];
    }
    
    

    TCanvas *c_two = new TCanvas("MomentumResolutionBar","Track Parameters #sigma",200,10,700,500);
    c_two->Divide(3,2);
    c_two->cd(1);
    //TGraph *Muon_plot20 = new TGraph(SIZE_M, Momentum, sigma20);
  
    TGraphErrors* Muon_plot10  = new TGraphErrors(SIZE_M, Momentum, sigmaOmega10, zeros, errorOmega10);
    Muon_plot10 -> SetTitle("#omega");
    Muon_plot10 -> SetMarkerColor(2);
    Muon_plot10 -> SetMarkerStyle(24);
    Muon_plot10 -> SetMarkerSize(1);
    Muon_plot10 -> GetXaxis() -> SetTitle("P (GeV)");
    Muon_plot10 -> GetYaxis() -> SetTitle("#sigma = f(P,#theta)");
    Muon_plot10 -> SetMinimum( 0.50 );
    Muon_plot10 -> SetMaximum( 1.50 );
    Muon_plot10 -> Draw("AP");
    
    
    TGraphErrors* Muon_plot20  = new TGraphErrors(SIZE_M, Momentum, sigmaOmega20, zeros, errorOmega20);
    Muon_plot20 -> SetMarkerColor(4);
    Muon_plot20 -> SetMarkerStyle(22);
    Muon_plot20 -> SetMarkerSize(1);
    Muon_plot20 -> Draw("P");
  
    TGraphErrors *Muon_plot40 = new TGraphErrors(SIZE_M, Momentum, sigmaOmega40,  zeros, errorOmega40);
    Muon_plot40 -> SetMarkerColor(2);
    Muon_plot40 -> SetMarkerStyle(21);
    Muon_plot40 -> SetMarkerSize(1);
    Muon_plot40 -> Draw("P");
    
    TGraphErrors *Muon_plot85 = new TGraphErrors(SIZE_M, Momentum, sigmaOmega85,  zeros, errorOmega85);
    Muon_plot85 -> SetMarkerColor(1);
    Muon_plot85 -> SetMarkerStyle(20);
    Muon_plot85 -> SetMarkerSize(1);
    Muon_plot85 -> Draw("P");
    c_two->SetTickx(1);
    c_two->SetTicky(1);
    
    
    c_two->cd(2);
    TGraphErrors* Muon_plot10  = new TGraphErrors(SIZE_M, Momentum, sigmaTanLambda10, zeros, errorTanLambda10);
    Muon_plot10 -> SetTitle("tan #lambda");
    Muon_plot10 -> SetMarkerColor(2);
    Muon_plot10 -> SetMarkerStyle(24);
    Muon_plot10 -> SetMarkerSize(1);
    Muon_plot10 -> GetXaxis() -> SetTitle("P (GeV)");
    Muon_plot10 -> GetYaxis() -> SetTitle("#sigma = f(P,#theta)");
    Muon_plot10 -> SetMinimum( 0.50 );
    Muon_plot10 -> SetMaximum( 1.50 );
    Muon_plot10 -> Draw("AP");
    
    
    TGraphErrors* Muon_plot20  = new TGraphErrors(SIZE_M, Momentum, sigmaTanLambda20, zeros, errorTanLambda20);
    Muon_plot20 -> SetMarkerColor(4);
    Muon_plot20 -> SetMarkerStyle(22);
    Muon_plot20 -> SetMarkerSize(1);
    Muon_plot20 -> Draw("P");
    
    TGraphErrors *Muon_plot40 = new TGraphErrors(SIZE_M, Momentum, sigmaTanLambda40,  zeros, errorTanLambda40);
    Muon_plot40 -> SetMarkerColor(2);
    Muon_plot40 -> SetMarkerStyle(21);
    Muon_plot40 -> SetMarkerSize(1);
    Muon_plot40 -> Draw("P");
    
    TGraphErrors *Muon_plot85 = new TGraphErrors(SIZE_M, Momentum, sigmaTanLambda85,  zeros, errorTanLambda85);
    Muon_plot85 -> SetMarkerColor(1);
    Muon_plot85 -> SetMarkerStyle(20);
    Muon_plot85 -> SetMarkerSize(1);
    Muon_plot85 -> Draw("P");
    c_two->SetTickx(1);
    c_two->SetTicky(1);
    
    
    c_two->cd(3);
    TGraphErrors* Muon_plot10  = new TGraphErrors(SIZE_M, Momentum, sigmaPhi10, zeros, errorPhi10);
    Muon_plot10 -> SetTitle("#phi");
    Muon_plot10 -> SetMarkerColor(2);
    Muon_plot10 -> SetMarkerStyle(24);
    Muon_plot10 -> SetMarkerSize(1);
    Muon_plot10 -> GetXaxis() -> SetTitle("P (GeV)");
    Muon_plot10 -> GetYaxis() -> SetTitle("#sigma = f(P,#theta)");
    Muon_plot10 -> SetMinimum( 0.50 );
    Muon_plot10 -> SetMaximum( 1.50 );
    Muon_plot10 -> Draw("AP");
    
    
    TGraphErrors* Muon_plot20  = new TGraphErrors(SIZE_M, Momentum, sigmaPhi20, zeros, errorPhi20);
    Muon_plot20 -> SetMarkerColor(4);
    Muon_plot20 -> SetMarkerStyle(22);
    Muon_plot20 -> SetMarkerSize(1);
    Muon_plot20 -> Draw("P");
  
    TGraphErrors *Muon_plot40 = new TGraphErrors(SIZE_M, Momentum, sigmaPhi40,  zeros, errorPhi40);
    Muon_plot40 -> SetMarkerColor(2);
    Muon_plot40 -> SetMarkerStyle(21);
    Muon_plot40 -> SetMarkerSize(1);
    Muon_plot40 -> Draw("P");
    
    TGraphErrors *Muon_plot85 = new TGraphErrors(SIZE_M, Momentum, sigmaPhi85,  zeros, errorPhi85);
    Muon_plot85 -> SetMarkerColor(1);
    Muon_plot85 -> SetMarkerStyle(20);
    Muon_plot85 -> SetMarkerSize(1);
    Muon_plot85 -> Draw("P");
    c_two->SetTickx(1);
    c_two->SetTicky(1);
    
    
    
    
    c_two->cd(4);

    TGraphErrors* Muon_plot10  = new TGraphErrors(SIZE_M, Momentum, sigmad010, zeros, errord010);
    Muon_plot10 -> SetTitle("d0");
    Muon_plot10 -> SetMarkerColor(2);
    Muon_plot10 -> SetMarkerStyle(24);
    Muon_plot10 -> SetMarkerSize(1);
    Muon_plot10 -> GetXaxis() -> SetTitle("P (GeV)");
    Muon_plot10 -> GetYaxis() -> SetTitle("#sigma = f(P,#theta)");
    Muon_plot10 -> SetMinimum( 0.50 );
    Muon_plot10 -> SetMaximum( 1.50 );
    Muon_plot10 -> Draw("AP");
    

    TGraphErrors* Muon_plot20  = new TGraphErrors(SIZE_M, Momentum, sigmad020, zeros, errord020);
    Muon_plot20 -> SetMarkerColor(4);
    Muon_plot20 -> SetMarkerStyle(22);
    Muon_plot20 -> SetMarkerSize(1);
    
    Muon_plot20 -> Draw("P");
    
    TGraphErrors *Muon_plot40 = new TGraphErrors(SIZE_M, Momentum, sigmad040,  zeros, errord040);
    Muon_plot40 -> SetMarkerColor(2);
    Muon_plot40 -> SetMarkerStyle(21);
    Muon_plot40 -> SetMarkerSize(1);
    Muon_plot40 -> Draw("P");
    
    TGraphErrors *Muon_plot85 = new TGraphErrors(SIZE_M, Momentum, sigmad085,  zeros, errord085);
    Muon_plot85 -> SetMarkerColor(1);
    Muon_plot85 -> SetMarkerStyle(20);
    Muon_plot85 -> SetMarkerSize(1);
    Muon_plot85 -> Draw("P");
    c_two->SetTickx(1);
    c_two->SetTicky(1);

    
    
    c_two->cd(5);
    TGraphErrors* Muon_plot10  = new TGraphErrors(SIZE_M, Momentum, sigmaz010, zeros, errorz010);
    Muon_plot10 -> SetTitle("z0");
    Muon_plot10 -> SetMarkerColor(2);
    Muon_plot10 -> SetMarkerStyle(24);
    Muon_plot10 -> SetMarkerSize(1);
    Muon_plot10 -> GetXaxis() -> SetTitle("P (GeV)");
    Muon_plot10 -> GetYaxis() -> SetTitle("#sigma = f(P,#theta)");
    Muon_plot10 -> SetMinimum( 0.50 );
    Muon_plot10 -> SetMaximum( 1.50 );
    Muon_plot10 -> Draw("AP");
    
    
    TGraphErrors* Muon_plot20  = new TGraphErrors(SIZE_M, Momentum, sigmaz020, zeros, errorz020);
    Muon_plot20 -> SetMarkerColor(4);
    Muon_plot20 -> SetMarkerStyle(22);
    Muon_plot20 -> SetMarkerSize(1);
    Muon_plot20 -> Draw("P");
    
    TGraphErrors *Muon_plot40 = new TGraphErrors(SIZE_M, Momentum, sigmaz040,  zeros, errorz040);
    Muon_plot40 -> SetMarkerColor(2);
    Muon_plot40 -> SetMarkerStyle(21);
    Muon_plot40 -> SetMarkerSize(1);
    Muon_plot40 -> Draw("P");
    
    TGraphErrors *Muon_plot85 = new TGraphErrors(SIZE_M, Momentum, sigmaz085,  zeros, errorz085);
    Muon_plot85 -> SetMarkerColor(1);
    Muon_plot85 -> SetMarkerStyle(20);
    Muon_plot85 -> SetMarkerSize(1);
    Muon_plot85 -> Draw("P");
    c_two->SetTickx(1);
    c_two->SetTicky(1);
    
    
    







    c_two->cd(6);
    TLegend *leg = new TLegend(0.6,0.7,0.75,0.95);
    //leg->SetHeader("Polar Angles"); //name of the legend
    leg->AddEntry(Muon_plot10,"#theta = 10^{o}","p");
    leg->AddEntry(Muon_plot20,"#theta = 20^{o}","p");
    leg->AddEntry(Muon_plot40,"#theta = 40^{o}","p");
    leg->AddEntry(Muon_plot85,"#theta = 85^{o}","p");
    leg->Draw();
    
  }
  
}
