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
#define SIZE_PA 2

//TGraph *Muon_plot[SIZE_PA];

double sigma[SIZE_M][SIZE_PA], error_sigma[SIZE_M][SIZE_PA];

int Mom[SIZE_M] = {1, 3, 5, 10, 15, 25, 50, 100};
float Momentum[SIZE_M] = {1, 3, 5, 10, 15, 25, 50, 100}; 
float zeros[SIZE_M];
int PA[SIZE_PA] = {40, 85};
float LimAxis;
int color, marker;

void D0Resolution(){
  for (int ll = 0; ll < SIZE_M; ll++){  
    zeros[ll] = 0;
  }
  for(int ii = 0; ii < SIZE_PA; ii++){
    for (int i = 0; i < SIZE_M; i++){

      TFile *f = new TFile(Form("../Results/Analysis/analysis_MuonsAngle_%d_Mom_%d.root",PA[ii],Mom[i]), "read");
      TTree *EvalTree = (TTree*)f->Get("MyRecoMCTruthLinker/EvalTree");
    
      vector<float> *recoD0;
      vector<float> *trueD0; 
	
      //Loading the branches in EvalTree
      EvalTree->SetBranchAddress("recoD0", &recoD0);
      EvalTree->SetBranchAddress("trueD0", &trueD0);
      
      int num_entries = EvalTree->GetEntries(); 
      
      //getting the correct size of the axis for a good fitting
      if ( (Momentum[i]/PA[ii]) > 1 ){
	LimAxis = 0.04;
      }
      else{
	if (Mom[i] == 1 && PA[ii] == 20){
	  LimAxis = 2.0;
	}
	else{
	    LimAxis = 0.07;
	}
      }
      
      TH1F* h1 = new TH1F("h1","Impact Parameter Resolution",100, (-1)*LimAxis, LimAxis); 
      
      for (int j = 0; j < num_entries; j++ ){
	EvalTree->GetEntry(j);
	for (int unsigned k = 0; k < recoD0->size(); k++){
	  float DistanceResolution = (recoD0->at(k) - trueD0->at(k));
	  h1->Fill( DistanceResolution );
	}
      }
      
      h1->Fit("gaus");
      TF1 *fit1 = (TF1*)h1->GetFunction("gaus");
      sigma[i][ii] = 1000*(fit1->GetParameter(2));
      error_sigma[i][ii] =  1000*(fit1->GetParError(2));

      std::cout << " angle " << PA[ii] << " momentum " << Mom[i] << " sigma " << sigma[i][ii] << std::endl ;
      
      
    }//loop for each .root files, here I have collected sigma and its error
  }//polar angle loop
   
  float sigma20[SIZE_M];
  float error20[SIZE_M];
  float r_error20[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma20[kk] = sigma[kk][0];
    error20[kk] = error_sigma[kk][0];
    r_error20[kk] = 100*(error_sigma[kk][0]/sigma[kk][0]);
  }  
  
  float sigma40[SIZE_M];
  float error40[SIZE_M];
  float r_error40[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma40[kk] = sigma[kk][1];
    error40[kk] = error_sigma[kk][1];
  }

  float sigma85[SIZE_M];
  float error85[SIZE_M];
  float r_error85[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma85[kk] = sigma[kk][2];
    error85[kk] = error_sigma[kk][2];
  }



  TCanvas *c_two = new TCanvas("DistanceResolution","Distance Resolution",200,10,700,500);
  //c_two -> Divide(1,2);

  //c_two->cd(1);
  TGraphErrors *Muon_plot40 = new TGraphErrors(SIZE_M, Momentum, sigma40, zeros, error40);
  Muon_plot40 -> SetTitle("Impact Parameter Resolution");
  Muon_plot40 -> SetMarkerColor(2);
  Muon_plot40 -> SetMarkerStyle(21);
  Muon_plot40 -> SetMarkerSize(1);
  Muon_plot40 -> GetXaxis() -> SetTitle("p_{T} (GeV)");
  Muon_plot40 -> GetYaxis() -> SetTitle("#sigma_{d0}(#mum)");
  Muon_plot40 -> Draw("AP");
  
  //c_two->cd(1);
  TGraphErrors *Muon_plot20 = new TGraphErrors(SIZE_M, Momentum, sigma20, zeros, error20);
  Muon_plot20 -> SetTitle("Distance Resolution");
  Muon_plot20 -> SetMarkerColor(4);
  Muon_plot20 -> SetMarkerStyle(22);
  Muon_plot20 -> SetMarkerSize(1);
  Muon_plot20 -> GetXaxis() -> SetTitle("p_{T} (GeV)");
  Muon_plot20 -> GetYaxis() -> SetTitle("#sigma_{d0}(#mum)");
  Muon_plot20 -> Draw("P");
  
  //c_two->cd(1);
  TGraphErrors *Muon_plot85 = new TGraphErrors(SIZE_M, Momentum, sigma85, zeros, error85 );
  Muon_plot85 -> SetTitle("Impact Parameter Resolution");
  Muon_plot85 -> SetMarkerColor(1);
  Muon_plot85 -> SetMarkerStyle(20);
  Muon_plot85 -> SetMarkerSize(1);
  Muon_plot85 -> GetXaxis() -> SetTitle("p_{T} (GeV)");
  Muon_plot85 -> GetYaxis() -> SetTitle("#sigma_{d0}(#mum)");
  Muon_plot85 -> Draw("P");
  c_two->SetLogx();

  TLegend *leg = new TLegend(0.6,0.7,0.75,0.95);
  //leg->SetHeader("Polar Angles"); //name of the legend
  leg->AddEntry(Muon_plot20,"#theta = 20^{o}","p");
  leg->AddEntry(Muon_plot40,"#theta = 40^{o}","p");
  leg->AddEntry(Muon_plot85,"#theta = 85^{o}","p");
  leg->Draw();

}
