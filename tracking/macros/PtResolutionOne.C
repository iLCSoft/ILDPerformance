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
using namespace std;

#define SIZE 8
double sigma[SIZE], error_sigma[SIZE], zeros[SIZE];
int Mom[SIZE] = {1, 3, 5, 10, 15, 25, 50, 100};
double Momentum[SIZE] = {1, 3, 5, 10, 15, 25, 50, 100}; 
int PA = 85;

void PtResolutionOne(){
  for (int ll = 0; ll < SIZE; ll++){  
    zeros[ll] = 0;
  }
  for(int unsigned i = 0; i < SIZE; i++){
    TFile *f = new TFile(Form("../Results/Analysis/analysis_MuonsAngle_85_Mom_%d.root",Mom[i]), "read");
    TTree *EvalTree = (TTree*)f->Get("MyRecoMCTruthLinker/EvalTree");



    vector<float> *PtReco;
    vector<float> *PtMCP; 

    //Loading the branches in EvalTree
    EvalTree->SetBranchAddress("PtReco", &PtReco);
    EvalTree->SetBranchAddress("PtMCP", &PtMCP);
  
    int num_entries = EvalTree->GetEntries(); 

    EvalTree->GetEntry(0);

    float LimAxis = PtReco->at(0);
    float centering =  (PtReco->at(0) - PtMCP->at(0))/pow(PtMCP->at(0), 2.0);
   
    //getting the correct size of the axis for a good fitting
    if (PA == 85){ //axis for polar angle 85
      if ( (Mom[i]/1.0) > 100){
	LimAxis = 1/(10*Mom[i]/1.0);
      }
      else{
	LimAxis = 1/(100*Mom[i]/1.0);
      }
    }
    else if (PA == 40){ //axis for polar angle 40
      if ( (Mom[i]/1.0) > 100  && Mom[i] != 300 && Mom[i] != 500){ //I need alternative axis for 300 and 500 GeV.
	LimAxis = 1/(10*Mom[i]/1.0);
      }
      else if (Mom[i] == 300 || Mom[i] == 500){
	LimAxis = 0.0015;
      }
      else{
	LimAxis = 1/(100*Mom[i]/1.0);
      }
    }      
    else{ //axis for polar angle 20
      if ( Mom[i] == 1 && PA == 20 ){
	LimAxis = 0.3;
      }
      else{
	LimAxis = 0.01;
      }
    }
    TH1F* h1 = new TH1F("h1","Momentum Resolution",100, (-1)*LimAxis, LimAxis); 
    
    for (int j = 0; j < num_entries; j++ ){
      EvalTree->GetEntry(j);
      for (int unsigned k = 0; k < PtReco->size(); k++){
	float MomentumResolution = (PtReco->at(k) - PtMCP->at(k))/pow(PtMCP->at(k), 2);
	h1->Fill( MomentumResolution - centering);
      }
    }

    h1->Fit("gaus");
    TF1 *fit1 = (TF1*)h1->GetFunction("gaus");
    sigma[i] = fit1->GetParameter(2);
    error_sigma[i] =  fit1->GetParError(2);

  }//loop for each .root files, here I have collected sigma and its error
  
  //printing sigma
  cout << "sigma = ( " ;  
  for(int unsigned i = 0; i < SIZE; i++){
    cout << sigma[i] << " ";   
  }
  cout << ")" << std::endl;
  /*
  //printing error_sigma
  cout << "error_sigma = ( " ;  
  for(int unsigned i = 0; i < SIZE; i++){
    cout << error_sigma[i] << " ";   
  }
  cout << ")" << std::endl;
  
  //printing relative_error_sigma
  cout << "error_sigma = ( " ;  
  for(int unsigned i = 0; i < SIZE; i++){
    cout << 100*error_sigma[i]/sigma[i] << " ";   
  }
  cout << ")" << std::endl;
  */
  //plotting the sigma and momentum
  TCanvas *c1 = new TCanvas("Momentum-test","Momentum Resolution",200,10,700,500);
  TGraphErrors *Pt_Plot = new TGraphErrors(SIZE, Momentum, sigma, zeros, error_sigma);
  Pt_Plot -> SetTitle("Momentum Resolution");
  Pt_Plot -> SetMarkerColor(4);
  Pt_Plot -> SetMarkerStyle(22);
  Pt_Plot -> SetMarkerSize(1);
  Pt_Plot -> GetXaxis() -> SetTitle("p_{T} (GeV)");
  Pt_Plot -> GetYaxis() -> SetTitle("#sigma_{1/p_{T}}(GeV^{-1})");
  Pt_Plot -> SetMinimum( pow(10,-5) );
  Pt_Plot -> SetMaximum( 2*pow(10, -1) );
  Pt_Plot -> Draw("AP");
  c1->SetLogx();
  c1->SetLogy();

}
