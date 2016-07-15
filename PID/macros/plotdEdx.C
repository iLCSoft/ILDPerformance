/** Very simply plotting of dEdx as a function of beta*gamma (p/m).
 *  Should add colors for standard PDG types ...
 * 
 */
void plotdEdx(const char* _filename ) {

  gROOT->SetStyle("ildStyle");
  // use small markers:
  ildStyle->SetMarkerStyle(1);

  TFile *treefile = new TFile(_filename);

  TCanvas* c1 =  new TCanvas("c1","dE/dx of charged particles",750,750 );
 
  gPad->SetLogx() ;

  hermTree->Draw("seenDEdx:trueP/trueMass","abs(trueCharge)>0.5&&trueMass>0.&&seenDEdx>0.&&seenDEdx<1.e-6") ;

}
