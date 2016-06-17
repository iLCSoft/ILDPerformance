/////////////////////////////////////////////////////////////////////////////////////////
#include<sstream>


void ZH5CFit_compare (const char* reffilename = "ZH5CFit.root", 
                      const char* testfilename = "../rootfiles/ZH5CFit_test.root", 
                      const char* legtitle = "OPALFitter, ZH -> 4jets",
                      const char* outfilename = "comparison_ZH5CFit") {

  gStyle->SetOptTitle(0);
  gStyle->SetTitleOffset(1.5,"y");
  gStyle->SetPadLeftMargin(0.2);
  
  const int nfile = 2;
  TFile *histfile[nfile];
  histfile[0] = new TFile(testfilename);
  histfile[1] = new TFile(reffilename);
  
   
  const int nhist = 8;
  TString histnames[nhist][nfile];

  histnames[0][0] = "MyZH5CFit/hJetMass";
  //histnames[0] = "MyZH5CFit/hFitError";
  histnames[1][0] = "MyZH5CFit/hNItBest";
  histnames[2][0] = "MyZH5CFit/hFitProb";
  histnames[3][0] = "MyZH5CFit/hPhotonEnergy";
  
  histnames[4][0] = "MyZH5CFit/hRecHMassBest";
  histnames[5][0] = "MyZH5CFit/hRecHMassAll";
  histnames[6][0] = "MyZH5CFit/hRecHMassNoFitBest";
  histnames[7][0] = "MyZH5CFit/hRecZMassNoFitBest";
  
  histnames[0][1] = "MyZH5CFit/hJetMass";
  //histnames[0] = "MyZH5CFit/hFitError";
  histnames[1][1] = "MyZH5CFit/hNItBest";
  histnames[2][1] = "MyZH5CFit/hFitProb";
  histnames[3][1] = "MyZH5CFit/hPhotonEnergy";
  
  histnames[4][1] = "MyZH5CFit/hRecHMassBest";
  histnames[5][1] = "MyZH5CFit/hRecHMassAll";
  histnames[6][1] = "MyZH5CFit/hRecHMassNoFitBest";
  histnames[7][1] = "MyZH5CFit/hRecZMassNoFitBest";
  
  bool logy[nhist] = {true, true, true, true, true, true, true, true};
  
  TString axistitle[nhist];

  axistitle[0] = "Jet mass / GeV";
//  axistitle[0] = "Error flag";
  axistitle[1] = "Number of iterations";
  axistitle[2] = "Fit probability";
  axistitle[3] = "Photon Energy / GeV";
  
  axistitle[4] = "H mass / GeV  (best)";
  axistitle[5] = "H mass / GeV  (all)";
  axistitle[6] = "H mass / GeV  (best, no fit)";
  axistitle[7] = "Z mass / GeV  (best, no fit)";
  
  double leg_left[nhist]  = {0.4, 0.4, 0.4, 0.4, 0.7, 0.4, 0.7, 0.6};
  double leg_right[nhist] = {0.9, 0.9, 0.9, 0.9, 1.0, 0.9, 1.0, 0.9};
   
  TH1F *h[nhist][nfile];
  
  // read histos
  for (int ifile = 0; ifile < nfile; ++ifile) {
    if (!histfile[ifile]) {
      cout << "ERROR: couldn't open file " << histfile[ifile] << endl;
      return; 
    }  
    for (int ihist = 0; ihist < nhist; ++ihist) {
      cout << "reading histogram " << histnames[ihist][ifile] << " from ifile " << ifile << " with name " << histfile[ifile]->GetName() << endl;
      h[ihist][ifile] = (TH1F *) histfile[ifile]->Get(histnames[ihist][ifile])->Clone();
      if (!h[ihist][ifile]) {
        cout << "ERROR: couldn't find histogram" << endl;
        return; 
      }  
      h[ihist][ifile]->StatOverflows(true);
      h[ihist][ifile]->SetXTitle(axistitle[ihist]);
      h[ihist][ifile]->SetYTitle("Events");
      if (ifile == 0){  // test histo
        h[ihist][ifile]->SetMarkerColor(1);    
        h[ihist][ifile]->SetMarkerStyle(20);    
        h[ihist][ifile]->SetMarkerSize(0.5);   
      } 
      else if (ifile == 1) {  // ref histo
        double norm = h[ihist][0]->GetEntries() / h[ihist][1]->GetEntries();
        cout << "scaling reference histogram by " <<  norm << endl;
        h[ihist][ifile]->Scale(norm);
        h[ihist][ifile]->SetLineColor(2); 
        double max = h[ihist][0]->GetMaximum(); 
        if (h[ihist][1]->GetMaximum() > max) max = h[ihist][1]->GetMaximum();
        h[ihist][ifile]->SetMaximum(1.2*max); 
        if (ihist == 2) h[ihist][ifile]->SetMinimum(10.); 
        if (ihist == 3) h[ihist][ifile]->SetMinimum(1.); 
        //h[ihist][ifile]->SetHistLineWidth(4);   
      }    
    }
  }
 
     
  cout << "FINISHED READING HISTOGRAMS!" << endl;

  // draw
  TCanvas *c1 = new TCanvas ("c1","compare",600,600);
  c1->Divide(2,2);
  TLegend *leg1[nhist];
  for (int ihist = 0; ihist < 4; ++ihist) {
    c1->cd(ihist+1);
    if (logy[ihist]) gPad->SetLogy();
    leg1[ihist] = new TLegend (leg_left[ihist], 0.7, leg_right[ihist], 0.9, legtitle);
    h[ihist][1]->Draw("hist");
    leg1[ihist]->AddEntry(h[ihist][1],"Reference","L");
    h[ihist][0]->Draw("e0same");
    leg1[ihist]->AddEntry(h[ihist][0],"Test file","P");
    
    leg1[ihist]->Draw();   
  }
  
  TCanvas *c2 = new TCanvas ("c2","masses",600,600);
  c2->Divide(2,2);
  TLegend *leg2[nhist];
  for (int ihist = 4; ihist < nhist; ++ihist) {
    c2->cd(ihist-3);
    if (logy[ihist]) gPad->SetLogy();
    if ( ihist == 5 ) leg2[ihist] = new TLegend (leg_left[ihist], 0.4, leg_right[ihist], 0.6, legtitle);
    else leg2[ihist] = new TLegend (leg_left[ihist], 0.7, leg_right[ihist], 0.9, legtitle);
    h[ihist][1]->Draw("hist");
    leg2[ihist]->AddEntry(h[ihist][1],"Reference","L");
    h[ihist][0]->Draw("e0same");
    leg2[ihist]->AddEntry(h[ihist][0],"Test file","P");
    
    leg2[ihist]->Draw();   
  }
  
  // --------------- save to pdf file ----------------------------------

  std::string epsFile1( std::string( "../plots/" )  + std::string( outfilename ) + std::string( "_1" )  + std::string( ".eps" ) ) ;
  std::string pdfFile1( std::string( "../plots/" )  +  std::string( outfilename ) + std::string( "_1" ) + std::string( ".pdf" ) ) ;
  //std::string cFile1  ( std::string( "../plots/" )  +  std::string( outfilename ) + std::string( "_1" ) + std::string( ".C" ) ) ;
  c1->Print( epsFile1.c_str() ) ;
  c1->Print( pdfFile1.c_str() ) ;
  //c1->Print( cFile1.c_str() ) ;

  std::string epsFile2( std::string( "../plots/" )  +  std::string( outfilename ) + std::string( "_2" )  + std::string( ".eps" ) ) ;
  std::string pdfFile2( std::string( "../plots/" )  +  std::string( outfilename ) + std::string( "_2" ) + std::string( ".pdf" ) ) ;
  //std::string cFile2  ( std::string( "../plots/" )  +  std::string( outfilename ) + std::string( "_2" ) + std::string( ".C" ) ) ;
  c2->Print( epsFile2.c_str() ) ;
  c2->Print( pdfFile2.c_str() ) ;
  //c2->Print( cFile2.c_str() ) ;



}
