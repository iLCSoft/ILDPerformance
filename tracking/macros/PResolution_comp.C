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

double sigma[SIZE_M][SIZE_PA], error_sigma[SIZE_M][SIZE_PA];
double sigma_new[SIZE_M][SIZE_PA], error_new_sigma_new[SIZE_M][SIZE_PA];

int Mom[SIZE_M] = {1, 3, 5, 10, 15, 25, 50, 100};
float Momentum[SIZE_M] = {1, 3, 5, 10, 15, 25, 50, 100};
float zeros[SIZE_M];
int PA[SIZE_PA] = {10, 20, 40, 85};

float LimAxis;
int color, marker;

void PResolution_comp(){
  for (int ll = 0; ll < SIZE_M; ll++){  
    zeros[ll] = 0;
  }
  for(int ii = 0; ii < SIZE_PA; ii++){
    for (int i = 0; i < SIZE_M; i++){
      TFile *f = new TFile(Form("/afs/desy.de/group/flc/pool/voutsina/SingleMuonTracks/DBD/analysis_MuonsAngle_%d_Mom_%d.root",PA[ii],Mom[i]), "read");
      TTree *EvalTree = (TTree*)f->Get("MyRecoMCTruthLinker/EvalTree");

      printf( " I am studying file analysis_MuonsAngle_%d_Mom_%d.root ",PA[ii],Mom[i]);
      
      vector<float> *PtReco;
      vector<float> *PtMCP; 
      
      EvalTree->SetBranchAddress("PtReco", &PtReco);
      EvalTree->SetBranchAddress("PtMCP", &PtMCP);
	
      int num_entries = EvalTree->GetEntries(); 

      EvalTree->GetEntry(0);
      
      
      //getting the correct size of the axis for a good fitting
      if (PA[ii] == 85){ //axis for polar angle 85
	if ( (Mom[i]/1.0) > 100){
	  LimAxis = 1/(10*Mom[i]/1.0);
	}
	else{
	  LimAxis = 1/(100*Mom[i]/1.0);
	}
      }
      else if (PA[ii] == 40){ //axis for polar angle 40
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
      else if (PA[ii] == 20 ){ //axis for polar angle 20
	if ( Mom[i] == 1 ){ LimAxis = 0.3; }
	else{
	  LimAxis = 0.01;
	}
      }
      else if (PA[ii] == 20 ){
	LimAxis = 0.04;
      }
      else if (PA[ii] == 10 ){
	LimAxis = 0.5;
      }

      TH1F* h1 = new TH1F("h1","Momentum Resolution",100, (-1)*LimAxis, LimAxis); 
      
      for (int j = 0; j < num_entries; j++ ){
	EvalTree->GetEntry(j);
	for (int unsigned k = 0; k < PtReco->size(); k++){
	  float MomentumResolution = (PtReco->at(k) - PtMCP->at(k))/pow(PtMCP->at(k), 2);
	  h1->Fill( MomentumResolution);
	}
      }
	
      h1->Fit("gaus");
      TF1 *fit1 = (TF1*)h1->GetFunction("gaus");
      sigma[i][ii] = fit1->GetParameter(2);
      error_sigma[i][ii] =  fit1->GetParError(2);
      
      //std::cout << " sigma " << sigma[i][ii] <<std::endl ;


    }//loop for each .root files, here I have collected sigma and its error
  }//polar angle loop



  for(int ii = 0; ii < SIZE_PA; ii++){
    for (int i = 0; i < SIZE_M; i++){

      TFile *fnew = new TFile(Form("../Results/Analysis/analysis_MuonsAngle_%d_Mom_%d.root",PA[ii],Mom[i]), "read");
      TTree *EvalTreeNew = (TTree*)fnew->Get("EvalTree");
            
      //printf( " I am studying file /nfs/dust/ilc/user/voutsina/testarea/CodeTest/fitting/ILDPerformance/trunk/tracking/Results/Analysis/analysis_MuonsAngle_%d_Mom_%d.root ",PA[ii],Mom[i]);

      vector<float> *PtReco;
      vector<float> *PtMCP; 
      
      EvalTreeNew->SetBranchAddress("PtReco", &PtReco);
      EvalTreeNew->SetBranchAddress("PtMCP", &PtMCP);
      
      int num_entries = EvalTreeNew->GetEntries(); 
      
      EvalTreeNew->GetEntry(0);
      
      
      //getting the correct size of the axis for a good fitting
      if (PA[ii] == 85){ //axis for polar angle 85
	if ( (Mom[i]/1.0) > 100){
	    LimAxis = 1/(10*Mom[i]/1.0);
	}
	else{
	  LimAxis = 1/(100*Mom[i]/1.0);
	}
      }
      else if (PA[ii] == 40){ //axis for polar angle 40
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
      else if (PA[ii] == 20 ){ //axis for polar angle 20
	if ( Mom[i] == 1 ){ LimAxis = 0.3; }
	else{
	  LimAxis = 0.01;
	}
      }
      else if (PA[ii] == 20 ){
	LimAxis = 0.04;
      }
      
      else if (PA[ii] == 10 ){
	LimAxis = 0.5;
      }
      
      TH1F* h1 = new TH1F("h1","Momentum Resolution",100, (-1)*LimAxis, LimAxis); 
      
      for (int j = 0; j < num_entries; j++ ){
	EvalTreeNew->GetEntry(j);
	for (int unsigned k = 0; k < PtReco->size(); k++){
	  float MomentumResolution = (PtReco->at(k) - PtMCP->at(k))/pow(PtMCP->at(k), 2);
	  h1->Fill( MomentumResolution);
	}
      }
      
      h1->Fit("gaus");
      TF1 *fit1 = (TF1*)h1->GetFunction("gaus");
      sigma_new[i][ii] = fit1->GetParameter(2);
      error_new_sigma_new[i][ii] =  fit1->GetParError(2);
      
      //std::cout << " sigma " << sigma[i][ii] <<std::endl ;
      
      
    }//loop for each .root files, here I have collected sigma and its error
  }//polar angle loop
  




  
 float sigma20[SIZE_M];
 float error20[SIZE_M];
 float r_error20[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma20[kk] = sigma[kk][1];
    error20[kk] = error_sigma[kk][1];
    r_error20[kk] = 100*(error_sigma[kk][1]/sigma[kk][1]);
    //std::cout << " polar angle = 20, momentum " << Momentum[kk] << " kk " << kk << " sigma20 " << sigma20[kk] << " error " <<  error20[kk] << std::endl ;
  }  

  float sigma10[SIZE_M];
  float error10[SIZE_M];
  float r_error10[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma10[kk] = sigma[kk][0];
    error10[kk] = error_sigma[kk][0];
    std::cout << " polar angle = 10, momentum " << Momentum[kk] << " kk " << kk << " sigma10 " << sigma10[kk] << " error " <<  error10[kk] << std::endl ;
  }
  
  float sigma40[SIZE_M];
  float error40[SIZE_M];
  float r_error40[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma40[kk] = sigma[kk][2];
    error40[kk] = error_sigma[kk][2];
    r_error40[kk] = 100*(error_sigma[kk][2]/sigma[kk][2]);
    //std::cout << " polar angle = 40, momentum " << Momentum[kk] << " kk " << kk << " sigma40 " << sigma40[kk] << " error " <<  error40[kk] << std::endl ;
  }

  float sigma85[SIZE_M];
  float error85[SIZE_M];
  float r_error85[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma85[kk] = sigma[kk][3];
    error85[kk] = error_sigma[kk][3];
    r_error85[kk] = 100*(error_sigma[kk][3]/sigma[kk][3]);
    //std::cout << " polar angle = 85, momentum " << Momentum[kk] << " kk " << kk << " sigma85 " << sigma85[kk] << " error " <<  error85[kk] << std::endl ;

  }


 float sigma_new20[SIZE_M];
 float error_new20[SIZE_M];
 float r_error_new20[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma_new20[kk] = sigma_new[kk][1];
    error_new20[kk] = error_new_sigma_new[kk][1];
    r_error_new20[kk] = 100*(error_new_sigma_new[kk][1]/sigma_new[kk][1]);
    //std::cout << " polar angle = 20, momentum " << Momentum[kk] << " kk " << kk << " sigma_new20 " << sigma_new20[kk] << " error " <<  error_new20[kk] << std::endl ;
  }  

  float sigma_new10[SIZE_M];
  float error_new10[SIZE_M];
  float r_error_new10[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma_new10[kk] = sigma_new[kk][0];
    error_new10[kk] = error_new_sigma_new[kk][0];
    std::cout << " polar angle = 10, momentum " << Momentum[kk] << " kk " << kk << " sigma_new10 " << sigma_new10[kk] << " error " <<  error_new10[kk] << std::endl ;
  }
  
  float sigma_new40[SIZE_M];
  float error_new40[SIZE_M];
  float r_error_new40[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma_new40[kk] = sigma_new[kk][2];
    error_new40[kk] = error_new_sigma_new[kk][2];
    r_error_new40[kk] = 100*(error_new_sigma_new[kk][2]/sigma_new[kk][2]);
    //std::cout << " polar angle = 40, momentum " << Momentum[kk] << " kk " << kk << " sigma_new40 " << sigma_new40[kk] << " error " <<  error_new40[kk] << std::endl ;
  }

  float sigma_new85[SIZE_M];
  float error_new85[SIZE_M];
  float r_error_new85[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma_new85[kk] = sigma_new[kk][3];
    error_new85[kk] = error_new_sigma_new[kk][3];
    r_error_new85[kk] = 100*(error_new_sigma_new[kk][3]/sigma_new[kk][3]);
    //std::cout << " polar angle = 85, momentum " << Momentum[kk] << " kk " << kk << " sigma_new85 " << sigma_new85[kk] << " error " <<  error_new85[kk] << std::endl ;

  }







  TCanvas *c_two = new TCanvas("MomentumResolutionBar","Momentum Resolution",200,10,700,500);
  //c_two->Divide(1,2);
  //c_two->cd(1);
  //TGraph *Muon_plot20 = new TGraph(SIZE_M, Momentum, sigma20);
  
  TGraphErrors* Muon_plot20  = new TGraphErrors(SIZE_M, Momentum, sigma20, zeros, error20);
  Muon_plot20 -> SetTitle("Momentum Resolution");
  Muon_plot20 -> SetMarkerColor(4);
  Muon_plot20 -> SetMarkerStyle(23);
  Muon_plot20 -> SetMarkerSize(1);
  Muon_plot20 -> GetXaxis() -> SetTitle("P (GeV)");
  Muon_plot20 -> GetYaxis() -> SetTitle("#sigma_{1/p_{T}}(GeV^{-1})");
  Muon_plot20 -> SetMinimum( pow(10,-5) );
  Muon_plot20 -> SetMaximum( 2*pow(10, -1) );
  Muon_plot20 -> Draw("AP");
  
  TGraphErrors *Muon_plot40 = new TGraphErrors(SIZE_M, Momentum, sigma40,  zeros, error40);
  Muon_plot40 -> SetTitle("Momentum Resolution");
  Muon_plot40 -> SetMarkerColor(2);
  Muon_plot40 -> SetMarkerStyle(23);
  Muon_plot40 -> SetMarkerSize(1);
  //Muon_plot40 -> GetXaxis() -> SetTitle("P (GeV)");
  //Muon_plot40 -> GetYaxis() -> SetTitle("#sigma_{1/p_{T}}(GeV^{-1})");
  //Muon_plot40 -> SetMinimum( pow(10,-5) );
  //Muon_plot40 -> SetMaximum( 2*pow(10, -1) );
  Muon_plot40 -> Draw("P");
  
  TGraphErrors *Muon_plot10 = new TGraphErrors(SIZE_M, Momentum, sigma10,  zeros, error10);
  Muon_plot10 -> SetTitle("Momentum Resolution");
  Muon_plot10 -> SetMarkerColor(6);
  Muon_plot10 -> SetMarkerStyle(23);
  Muon_plot10 -> SetMarkerSize(1);
  Muon_plot10 -> Draw("P");
  
  TGraphErrors *Muon_plot85 = new TGraphErrors(SIZE_M, Momentum, sigma85,  zeros, error85);
  Muon_plot85 -> SetTitle("Momentum Resolution");
  Muon_plot85 -> SetMarkerColor(1);
  Muon_plot85 -> SetMarkerStyle(23);
  Muon_plot85 -> SetMarkerSize(1);
  Muon_plot85 -> Draw("P");




  TGraphErrors* Muon_plot_new20  = new TGraphErrors(SIZE_M, Momentum, sigma_new20, zeros, error_new20);
  Muon_plot_new20 -> SetTitle("Momentum Resolution");
  Muon_plot_new20 -> SetMarkerColor(4);
  Muon_plot_new20 -> SetMarkerStyle(32);
  Muon_plot_new20 -> SetMarkerSize(1);
  //Muon_plot_new20 -> GetXaxis() -> SetTitle("P (GeV)");
  //Muon_plot_new20 -> GetYaxis() -> SetTitle("#sigma_{1/p_{T}}(GeV^{-1})");
  //Muon_plot_new20 -> SetMinimum( pow(10,-5) );
  //Muon_plot_new20 -> SetMaximum( 2*pow(10, -1) );
  Muon_plot_new20 -> Draw("P");
  
  TGraphErrors *Muon_plot_new40 = new TGraphErrors(SIZE_M, Momentum, sigma_new40,  zeros, error_new40);
  Muon_plot_new40 -> SetTitle("Momentum Resolution");
  Muon_plot_new40 -> SetMarkerColor(2);
  Muon_plot_new40 -> SetMarkerStyle(32);
  Muon_plot_new40 -> SetMarkerSize(1);
  //Muon_plot_new40 -> GetXaxis() -> SetTitle("P (GeV)");
  //Muon_plot_new40 -> GetYaxis() -> SetTitle("#sigma_{1/p_{T}}(GeV^{-1})");
  //Muon_plot_new40 -> SetMinimum( pow(10,-5) );
  //Muon_plot_new40 -> SetMaximum( 2*pow(10, -1) );
  Muon_plot_new40 -> Draw("P");
  
  TGraphErrors *Muon_plot_new10 = new TGraphErrors(SIZE_M, Momentum, sigma_new10,  zeros, error_new10);
  Muon_plot_new10 -> SetTitle("Momentum Resolution");
  Muon_plot_new10 -> SetMarkerColor(6);
  Muon_plot_new10 -> SetMarkerStyle(32);
  Muon_plot_new10 -> SetMarkerSize(1);
  Muon_plot_new10 -> Draw("P");
  
  TGraphErrors *Muon_plot_new85 = new TGraphErrors(SIZE_M, Momentum, sigma_new85,  zeros, error_new85);
  Muon_plot_new85 -> SetTitle("Momentum Resolution");
  Muon_plot_new85 -> SetMarkerColor(1);
  Muon_plot_new85 -> SetMarkerStyle(32);
  Muon_plot_new85 -> SetMarkerSize(1);
  Muon_plot_new85 -> Draw("P");





  c_two->SetLogx();
  c_two->SetLogy();
  c_two->SetTickx(1);
  c_two->SetTicky(1);

  TLegend *leg = new TLegend(0.6,0.7,0.75,0.95);
  //leg->SetHeader("Polar Angles"); //name of the legend
  leg->AddEntry(Muon_plot10,"#theta = 10^{o} Mokka/KalTest","p");
  leg->AddEntry(Muon_plot20,"#theta = 20^{o} Mokka/KalTest","p");
  leg->AddEntry(Muon_plot40,"#theta = 40^{o} Mokka/KalTest","p");
  leg->AddEntry(Muon_plot85,"#theta = 85^{o} Mokka/KalTest","p");
  leg->AddEntry(Muon_plot_new10,"#theta = 10^{o} DD4hep/DDKalTest","p");
  leg->AddEntry(Muon_plot_new20,"#theta = 20^{o} DD4hep/DDKalTest","p");
  leg->AddEntry(Muon_plot_new40,"#theta = 40^{o} DD4hep/DDKalTest","p");
  leg->AddEntry(Muon_plot_new85,"#theta = 85^{o} DD4hep/DDKalTest","p");
  leg->Draw();
 
}







 
