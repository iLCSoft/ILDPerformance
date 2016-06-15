/////////////////////////////////////////////////////////////////////////////////////////
#include<sstream>


void WW5CFit_compare (const char* reffilename = "WW5CFit.root", 
              const char* testfilename = "../rootfiles/WW5CFit_test.root", 
              const char* legtitle = "OPALFitter, WW -> 4jets",
              const char* outfilename = "comparison_WW5C") {

  gStyle->SetOptTitle(0);
  gStyle->SetTitleOffset(1.5,"y");
  gStyle->SetPadLeftMargin(0.2);
  
  const int nfile = 2;
  TFile *histfile[nfile];
  histfile[0] = new TFile(testfilename);
  histfile[1] = new TFile(reffilename);
  
   
  const int nhist = 4;
  TString histnames[nhist];

  histnames[0] = "MyWW5CFit/hJetMass";
  //histnames[0] = "MyWW5CFit/hFitError";
  histnames[1] = "MyWW5CFit/hNItBest";
  histnames[2] = "MyWW5CFit/hFitProb";
  histnames[3] = "MyWW5CFit/hRecWMassBest";
  
  bool logy[nhist] = {true, true, true, false};
  
  TString axistitle[nhist];

  axistitle[0] = "Jet mass / GeV";
//  axistitle[0] = "Error flag";
  axistitle[1] = "Number of iterations";
  axistitle[2] = "Fit probability";
  axistitle[3] = "W mass / GeV";
  
  double leg_left[nhist] = {0.4, 0.4, 0.4, 0.6};
  double leg_right[nhist] = {0.9, 0.9, 0.9, 0.9};
   
  TH1F *h[nhist][nfile];
  
  // read histos
  for (int ifile = 0; ifile < nfile; ++ifile) {
    if (!histfile[ifile]) {
      cout << "ERROR: couldn't open file " << histfile[ifile] << endl;
      return; 
    }  
    for (int ihist = 0; ihist < nhist; ++ihist) {
      cout << "reading histogram " << histnames[ihist] << " from ifile " << ifile << endl;
      h[ihist][ifile] = (TH1F *) histfile[ifile]->Get(histnames[ihist]);
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
        h[ihist][ifile]->SetMarkerSize(0.75);   
      } 
      else if (ifile == 1) {  // ref histo
        double norm = h[ihist][0]->GetEntries() / h[ihist][1]->GetEntries();
        cout << "scaling reference histogram by " <<  norm << endl;
        h[ihist][ifile]->Scale(norm);
        h[ihist][ifile]->SetLineColor(2);   
        //h[ihist][ifile]->SetHistLineWidth(4);   
      }    
    }
  }
 
     
  cout << "FINISHED READING HISTOGRAMS!" << endl;

  // draw
  TCanvas *c = new TCanvas ("c","compare",600,600);
  c->Divide(2,2);
  TLegend *leg[nhist];
  for (int ihist = 0; ihist < nhist; ++ihist) {
    c->cd(ihist+1);
    if (logy[ihist]) gPad->SetLogy();
    leg[ihist] = new TLegend (leg_left[ihist], 0.7, leg_right[ihist], 0.9, legtitle);
    h[ihist][1]->Draw("hist");
    leg[ihist]->AddEntry(h[ihist][1],"Reference","L");
    h[ihist][0]->Draw("e0same");
    leg[ihist]->AddEntry(h[ihist][0],"Test file","P");
    
    leg[ihist]->Draw();   
  }
  
  // --------------- save to pdf file ----------------------------------

  std::string epsFile( std::string( outfilename ) + std::string( ".eps" ) ) ;
  std::string pdfFile( std::string( outfilename ) + std::string( ".pdf" ) ) ;
  std::string cFile  ( std::string( outfilename ) + std::string( ".C" ) ) ;
  c->Print( epsFile.c_str() ) ;
  c->Print( pdfFile.c_str() ) ;
  c->Print( cFile.c_str() ) ;



}
