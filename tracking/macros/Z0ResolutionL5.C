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

//#define SIZE_M 1
//#define SIZE_PA 1

//TGraph *Muon_plot[SIZE_PA];

double sigma[SIZE_M][SIZE_PA], error_sigma[SIZE_M][SIZE_PA];

int Mom[SIZE_M] = {1, 3, 5, 10, 15, 25, 50, 100};
float Momentum[SIZE_M] = {1, 3, 5, 10, 15, 25, 50, 100}; 
float zeros[SIZE_M];
int PA[SIZE_PA] = {10, 20, 40, 85};

float LimAxis;
int color, marker;

void Z0ResolutionL5(){

  TCanvas *Z0_fit = new TCanvas("Z0 fit","Z0 fit",800,800);
  Z0_fit->Print("Z0_fit.pdf[");

  for (int ll = 0; ll < SIZE_M; ll++){  
    zeros[ll] = 0;
  }
  for(int ii = 0; ii < SIZE_PA; ii++){
    for (int i = 0; i < SIZE_M; i++){

      TFile *f = new TFile(Form("../Results/Analysis/analysis_MuonsAngle_%d_Mom_%d.root",PA[ii],Mom[i]), "read");
      //TTree *EvalTree = (TTree*)f->Get("EvalTree");
      TTree *EvalTree;
      f->GetObject("EvalTree", EvalTree);

      printf( " I am studying file analysis_MuonsAngle_%d_Mom_%d.root ",PA[ii],Mom[i]);    

      vector<float> *recoZ0 = 0;
      vector<float> *trueZ0 = 0; 
	
      //Loading the branches in EvalTree
      EvalTree->SetBranchAddress("recoZ0", &recoZ0);
      EvalTree->SetBranchAddress("trueZ0", &trueZ0);

      EvalTree->SetBranchStatus("*",0); //disable all branches
      EvalTree->SetBranchStatus("recoZ0",1);
      EvalTree->SetBranchStatus("trueZ0",1);
      
      //getting the correct size of the axis for a good fitting
      /*
      if ( (Momentum[i]/PA[ii]) > 1 || (Mom[i] == 15 && PA[ii] == 10) ){
	LimAxis = 0.04;
      }
      else if ((Mom[i] == 1 && PA[ii] == 20) || (Mom[i] == 3 && PA[ii] == 10) || (Mom[i] == 5 && PA[ii] == 10) ){
	  LimAxis = 1.0;
      }
      else if (Mom[i] == 1 && PA[ii] == 10){
	LimAxis = 3.0;
      }
      else{
	LimAxis = 0.07;
      }
      */
      if (PA[ii] < 11 ){
	   LimAxis =  30.0/Mom[i];
      }
      else if (PA[ii] < 20 ){
	   LimAxis =  10.0/Mom[i];
      }
      else if (PA[ii] >= 20 && PA[ii] < 85){
	if( Mom[i] > 10 ){
	  LimAxis =  0.8/Mom[i];
	}else {
	  LimAxis =  0.5/Mom[i];
	}
      }else if (PA[ii] == 85 ){
	if( Mom[i] > 9 ){
	  LimAxis =  1.0/Mom[i];
	}else {
	  LimAxis =  0.15/Mom[i];
	}
      }

      std::string dummy("Title: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
      std::sprintf( &dummy[0] , "Title: MuonsAngle_%d_Mom_%d" , PA[ii], Mom[i] ) ;
      
      TH1F* h1 = new TH1F("h1","Impact Parameter Resolution",100, (-1)*LimAxis, LimAxis); 
      
      Long64_t nentries = EvalTree->GetEntriesFast(); 
      for (Long64_t jentry=0; jentry<nentries;jentry++) {
	EvalTree->GetEntry(jentry);
	for (int unsigned k = 0; k < recoZ0->size(); k++){
	  float DistanceResolution = (recoZ0->at(k) - trueZ0->at(k));
	  h1->Fill( DistanceResolution );
	}
      }
      
      h1->Fit("gaus");
      TF1 *fit1 = (TF1*)h1->GetFunction("gaus");
      sigma[i][ii] = (fit1->GetParameter(2));
      error_sigma[i][ii] =  (fit1->GetParError(2));

      //std::cout << " angle " << PA[ii] << " momentum " << Mom[i] << " sigma " << sigma[i][ii] << std::endl ;
      
      Z0_fit->Print("Z0_fit.pdf",&dummy[0]);

    }//loop for each .root files, here I have collected sigma and its error
  }//polar angle loop

  Z0_fit->Print("Z0_fit.pdf]");
 
  float sigma20[SIZE_M];
  float error20[SIZE_M];
  float r_error20[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma20[kk] = sigma[kk][1];
    error20[kk] = error_sigma[kk][1];
    r_error20[kk] = 100*(error_sigma[kk][1]/sigma[kk][1]);
  }  
  

  float sigma40[SIZE_M];
  float error40[SIZE_M];
  float r_error40[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma40[kk] = sigma[kk][2];
    error40[kk] = error_sigma[kk][2];
  }

  float sigma10[SIZE_M];
  float error10[SIZE_M];
  float r_error10[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma10[kk] = sigma[kk][0];
    error10[kk] = error_sigma[kk][0];
  }

  float sigma85[SIZE_M];
  float error85[SIZE_M];
  float r_error85[SIZE_M];
  for(int kk=0; kk < SIZE_M; kk++){
    sigma85[kk] = sigma[kk][3];
    error85[kk] = error_sigma[kk][3];
  }



  TCanvas *c_two = new TCanvas("Z0Resolution","Z0 Resolution",200,10,700,700);
  //c_two -> Divide(1,2);

  //c_two->cd(1);
  TGraphErrors *Muon_plot40 = new TGraphErrors(SIZE_M, Momentum, sigma40, zeros, error40);
  Muon_plot40 -> SetTitle("Z0 Resolution");
  Muon_plot40 -> SetMarkerColor(2);
  Muon_plot40 -> SetMarkerStyle(21);
  Muon_plot40 -> SetMarkerSize(1);
  Muon_plot40 -> GetXaxis() -> SetTitle("Momentum (GeV)");
  Muon_plot40 -> GetYaxis() -> SetTitle("#sigma_{Z0}(mm)");
  Muon_plot40 -> SetMinimum( pow(10,-3) );
  Muon_plot40 -> SetMaximum( 10.8 );
  Muon_plot40 -> Draw("AP");
    
  //c_two->cd(1);
  TGraphErrors *Muon_plot20 = new TGraphErrors(SIZE_M, Momentum, sigma20, zeros, error20);
  Muon_plot20 -> SetTitle("Z0 Resolution");
  Muon_plot20 -> SetMarkerColor(4);
  Muon_plot20 -> SetMarkerStyle(22);
  Muon_plot20 -> SetMarkerSize(1);
  Muon_plot20 -> GetXaxis() -> SetTitle("Momentum (GeV)");
  Muon_plot20 -> GetYaxis() -> SetTitle("#sigma_{Z0}(mm)");
  Muon_plot20 -> Draw("P");

  TGraphErrors *Muon_plot10 = new TGraphErrors(SIZE_M, Momentum, sigma10, zeros, error10);
  Muon_plot10 -> SetTitle("Z0 Resolution");
  Muon_plot10 -> SetMarkerColor(6);
  Muon_plot10 -> SetMarkerStyle(22);
  Muon_plot10 -> SetMarkerSize(1);
  Muon_plot10 -> GetXaxis() -> SetTitle("Momentum (GeV)");
  Muon_plot10 -> GetYaxis() -> SetTitle("#sigma_{Z0}(mm)");
  Muon_plot10 -> Draw("P");
  
  //c_two->cd(1);
  TGraphErrors *Muon_plot85 = new TGraphErrors(SIZE_M, Momentum, sigma85, zeros, error85 );
  Muon_plot85 -> SetTitle("Z0 Resolution");
  Muon_plot85 -> SetMarkerColor(1);
  Muon_plot85 -> SetMarkerStyle(20);
  Muon_plot85 -> SetMarkerSize(1);
  Muon_plot85 -> GetXaxis() -> SetTitle("Momentum (GeV)");
  Muon_plot85 -> GetYaxis() -> SetTitle("#sigma_{Z0}(mm)");
  Muon_plot85 -> Draw("P");
  c_two->SetLogx();
  c_two->SetLogy();
  c_two->SetTickx(1);
  c_two->SetTicky(1);

  TLegend *leg = new TLegend(0.65,0.70,0.80,0.89);
  //leg->SetHeader("Polar Angles"); //name of the legend
  leg->AddEntry(Muon_plot10,"#theta = 10^{o}","p");
  leg->AddEntry(Muon_plot20,"#theta = 20^{o}","p");
  leg->AddEntry(Muon_plot40,"#theta = 40^{o}","p");
  leg->AddEntry(Muon_plot85,"#theta = 85^{o}","p");
  leg->Draw();

  TF1 *fpr10 = new TF1("fpr10","sqrt( 25.e-6 + 1.e-4/(x*x*0.17364817766*0.17364817766) )",0.5,200.0); //for theta =10 degree
  fpr10->SetLineColor(6); 
  //fpr10->Draw("same");

  TF1 *fpr20 = new TF1("fpr20","sqrt( 25.e-6 + 1.e-4/(x*x*0.34202014332*sqrt(0.34202014332)) )",0.5,200.0); //for theta =20 degree
  fpr20->SetLineColor(4); 
  //fpr20->Draw("same");

  TF1 *fpr40 = new TF1("fpr40","sqrt( 25.e-6 + 1.e-4/(x*x*0.64278760968*sqrt(0.64278760968)) )",0.5,200.0); //for theta =40 degree
  fpr40->SetLineColor(2); 
  //fpr40->Draw("same");

  TF1 *fpr85 = new TF1("fpr85","sqrt( 25.e-6 + 1.e-4/(x*x*0.99619469809*sqrt(0.99619469809)) )",0.5,200.0); //for theta =85 degree
  fpr85->SetLineColor(1); 
  //fpr85->Draw("same");

  c_two->SaveAs("Z0Resolution.png");

}
