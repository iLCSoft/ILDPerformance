#include "TFile.h"
#include "TCanvas.h"
#include "TKey.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TText.h"
#include <iostream>
using std::cout;
using std::endl;

/*
plot results of validation processors
DJeans July 2016

root> .L plotallHits.C++
root> plotallHits()

 */

void plotallHits() {

  const int MAXF=50;

  TString infilenames[MAXF];
  for (int i=0; i<MAXF; i++) infilenames[i]="";

  int jinf(0);
  infilenames[jinf++] = "VXDCollection.root";
  infilenames[jinf++] = "FTDCollection.root";
  infilenames[jinf++] = "SITCollection.root";
  infilenames[jinf++] = "SETCollection.root";
  infilenames[jinf++] = "TPCCollection.root";
  infilenames[jinf++] = "BeamCalCollection.root";
  infilenames[jinf++] = "LumiCalCollection.root";
  infilenames[jinf++] = "LHCalCollection.root";
  infilenames[jinf++] = "EcalBarrelCollection.root";
  infilenames[jinf++] = "EcalEndcapsCollection.root";
  infilenames[jinf++] = "EcalEndcapRingCollection.root";
  infilenames[jinf++] = "HcalBarrelRegCollection.root";
  infilenames[jinf++] = "HcalEndcapsCollection.root";
  infilenames[jinf++] = "HcalEndcapRingCollection.root";
  infilenames[jinf++] = "YokeBarrelCollection.root";
  infilenames[jinf++] = "YokeEndcapsCollection.root";

  TString plname = "allHits.ps";
  TCanvas* cc = new TCanvas("validate","validate",500,370);
  cc->Print(plname+"[");

  TText tt;
  tt.SetTextColor(2);
  tt.SetTextSize(0.02);


  gStyle->SetMarkerSize(0.1);
  gStyle->SetTitleSize(0.15,"ff");

  std::vector < TString > vnames;
  vnames.push_back("Sim_HitEn");
  vnames.push_back("Sim_HitTime");
  vnames.push_back("Sim_posXY");
  vnames.push_back("Sim_posXZ");
  vnames.push_back("Sim_posYZ");
  vnames.push_back("Sim_posRZ");

  std::vector <TString> exts;
  exts.push_back("");
  exts.push_back("_end0");
  exts.push_back("_end1");

  TH1F* h;
  TH2F* hh;

  for (int iif=0; iif<jinf; iif++) {
    if ( infilenames[iif]=="" ) continue;
    TFile* infile = new TFile(infilenames[iif], "read");
    cc->Clear();
    cc->Divide(3,3);
    int ic(1);
    for (size_t j=0; j<vnames.size(); j++) {
      for (size_t i=0; i<exts.size(); i++) {
	TObject* tobj = infile->Get(vnames[j]+exts[i]);
	if ( tobj ) {
	  cc->cd(ic++);
	  if ( tobj->InheritsFrom("TH1F") ) {
	    h = (TH1F*) tobj;
	    h->Draw();
	  } else if ( tobj->InheritsFrom("TH2F") ) {
	    hh = (TH2F*) tobj;
	    hh->Draw("box");
	  }
	}
      }
    }
    cc->cd();
    tt.DrawTextNDC(0.1, 0.01, infilenames[iif]);

    cc->Print(plname);


    cc->Clear();
    cc->Divide(5,7);
    ic=1;

    TString oldIndex="BLAH";

    TIter next(infile->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)next())) {
      TClass *cll = gROOT->GetClass(key->GetClassName());
      if (cll->InheritsFrom("TH2F")) {

	TH2F* h = (TH2F*)key->ReadObj();
	TString hn = h->GetName();
	
	bool isv(false);
	for (size_t i=0; i<vnames.size(); i++) {
	  if ( hn.Contains(vnames[i]) ) {
	    isv=true;
	    continue;
	  }
	}
	if (isv) continue;
	

	cout << hn << endl;
	TString thisIndex = ( (TObjString*) ( hn.Tokenize("_")->At(2) ) )->GetString();

	if ( thisIndex!=oldIndex ) {
	  oldIndex=thisIndex;
	  if ( ic>1 ) {
	    cc->cd();
	    tt.DrawTextNDC(0.1, 0.01, infilenames[iif]);
	    cc->Print(plname);
	  }
	  cc->Clear();
	  cc->Divide(5,2);
	  ic=1;
	}

	cc->cd(ic++);

	if ( h->GetNbinsX()>10 ) {
	  // look at range, if lots of empty bins, restrict range

	  for (int ii=0; ii< int( h->GetNbinsX()/10.); ii++) {
	    float nn = h->Integral( ii*10, h->GetNbinsX(), 1, h->GetNbinsY() );
	    if ( nn == 0 ) {

	      float xxm = h->GetXaxis()->GetBinCenter( 10*ii );
	      if ( h->GetXaxis()->GetBinCenter(1)>0 ) 
		h->GetXaxis()->SetRangeUser( 0, xxm );
	      else 
		h->GetXaxis()->SetRangeUser( -xxm, xxm );
		

	      break;
	    }
	  }

	}

	h->Draw("box");

      }
    }

    if ( ic>1 ) {
      cc->cd();
      tt.DrawTextNDC(0.1, 0.01, infilenames[iif]);
      cc->Print(plname);
    }

    //    cc->Print(plname);

    infile->Close();

  }


  cc->Print(plname+"]");
}
