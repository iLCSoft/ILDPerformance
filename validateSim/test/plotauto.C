#include "TFile.h"
#include "TCanvas.h"
#include "TKey.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"
#include "TText.h"
#include "TLegend.h"
#include <iostream>
#include <algorithm>
using std::cout;
using std::endl;

/*
  plot results of validation processors
  DJeans July 2016

  root> .L plotauto.C++
  root> plotauto()

*/

void plotauto(TString infilename) {

  TString plname = infilename+".ps";
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
  exts.push_back("_posZ");
  exts.push_back("_negZ");

  TH1F* h;
  TH2F* hh;
  TKey *key;
  TIter next;
  TKey *key2;
  TIter next2;

  TFile* infile = new TFile(infilename, "read");

  // overall geometry
  TDirectory* td = (TDirectory*) infile->Get("ALLCollections");
  cc->Clear();
  cc->Divide(3,2);
  cc->cd(1);
  hh = (TH2F*) td->Get("ALLCollections_overallhitZR");
  hh->Draw("box");
  cc->cd(2);
  int icol=1;
  bool first=true;
  TLegend* tl = new TLegend(0., 0., 1, 1);
  next = td->GetListOfKeys();
  while ((key = (TKey*)next())) {
    TClass *cll = gROOT->GetClass(key->GetClassName());
    if (cll->InheritsFrom("TH2F")) {
      hh = (TH2F*)key->ReadObj();
      TString hn = hh->GetName();
      if ( hn=="ALLCollections_overallhitZR" ) continue;
      if ( hn.Contains("_Log_") ) continue;
      hh->SetLineColor(icol);
      if ( first ) {hh->Draw("box"); first=false;}
      else hh->Draw("same;box");
      icol++;
      if (icol==10) icol=1;
      TString ss = hn.ReplaceAll( "_overallhitZR", "");
      tl->AddEntry(hh, ss , "l");
    }
  }

  cc->cd(4);
  hh = (TH2F*) td->Get("ALLCollections_Log_overallhitZR");
  hh->Draw("box");
  cc->cd(5);
  icol=1;
  first=true;
  next = td->GetListOfKeys();
  while ((key = (TKey*)next())) {
    TClass *cll = gROOT->GetClass(key->GetClassName());
    if (cll->InheritsFrom("TH2F")) {
      hh = (TH2F*)key->ReadObj();
      TString hn = hh->GetName();
      if ( hn=="ALLCollections_Log_overallhitZR" ) continue;
      if ( ! hn.Contains("_Log_") ) continue;
      hh->SetLineColor(icol);
      if ( first ) {hh->Draw("box"); first=false;}
      else hh->Draw("same;box");
      icol++;
      if (icol==10) icol=1;
    }
  }

  // the legend
  cc->cd(3);
  tl->Draw();

  cc->Print(plname);

  // now collection-by-collection

  next = infile->GetListOfKeys();
  while ((key = (TKey*)next())) {
    TClass *cll = gROOT->GetClass(key->GetClassName());
    if (cll->InheritsFrom("TDirectory")) {
      td = (TDirectory*)key->ReadObj();
      TString dirname = td->GetName();
      if ( dirname=="ALLCollections" ) continue;

      // is it an endcap collection?
      bool isEndcap = td->Get(dirname+"_hitXY_posZ");

      // first overall geometry
      cc->Clear();
      cc->Divide(3,2);

	cc->cd(1);
	( (TH2F*) td->Get(dirname+"_hitEn"))->Draw("box");
	cc->cd(4)->SetLogy();
	( (TH2F*) td->Get(dirname+"_hitTime"))->Draw("box");

      if ( isEndcap ) {
	cc->cd(2);
	( (TH2F*) td->Get(dirname+"_hitXY_posZ"))->Draw("box");
	cc->cd(3);
	( (TH2F*) td->Get(dirname+"_hitXY_negZ"))->Draw("box");
	cc->cd(5);
	((TH2F*) td->Get(dirname+"_hitZR_posZ"))->Draw("box");
	cc->cd(6);
	((TH2F*) td->Get(dirname+"_hitZR_negZ"))->Draw("box");
      } else {
	cc->cd(2);
	( (TH2F*) td->Get(dirname+"_hitXY"))->Draw("box");
	cc->cd(3);
	( (TH2F*) td->Get(dirname+"_hitZR"))->Draw("box");
      }

      cc->Print(plname);


      // then the cell indices

      // work out how many indices/variables we're dealing with
      std::vector < TString > indices;
      std::vector < TString > variables;
      next2 = td->GetListOfKeys();
      while ((key2 = (TKey*)next2())) {
	cll = gROOT->GetClass(key2->GetClassName());
	if (cll->InheritsFrom("TH2F")) {
	  hh = (TH2F*) key2->ReadObj();
	  TString hn = hh->GetName();
	  if ( hn.Contains("Indx") ) {
	    TString ss = hn.ReplaceAll(dirname+"_", "");
	    TString asas = ((TObjString*) (ss.Tokenize("_") -> At(0)))->GetString();
	    if ( find( indices.begin(), indices.end(), asas )==indices.end() ) indices.push_back(asas);
	    asas = ((TObjString*) (ss.Tokenize("_") -> At(1)))->GetString();
	    if ( find( variables.begin(), variables.end(), asas )==variables.end() ) variables.push_back(asas);
	  }
	}
      }



      for (int inp=0; inp<2; inp++) {
	if ( !isEndcap && inp==1 ) continue;
	cc->Clear();
	cc->Divide(indices.size(), variables.size());
	int ic=1;
	next2 = td->GetListOfKeys();
	while ((key2 = (TKey*)next2())) {
	  cll = gROOT->GetClass(key2->GetClassName());
	  if (cll->InheritsFrom("TH2F")) {
	    hh = (TH2F*) key2->ReadObj();
	    TString hn = hh->GetName();
	    if ( isEndcap ) {
	      if      ( inp==0 && ! hn.Contains("posZ") ) continue;
	      else if ( inp==1 && ! hn.Contains("negZ") ) continue;
	    }
	    if ( hn.Contains("Indx") ) {
	      TString asas = ((TObjString*) (hn.Tokenize("_") -> At(1)))->GetString();
	      asas = asas(4,asas.Length());
	      cc->cd(ic++);
	      hh->Draw("box");
	    }
	  }
	}

	cc->cd();
	for ( size_t k=0; k<variables.size(); k++) {
	  tt.DrawTextNDC( 0.0, 0.9 - (1.0*k)/(variables.size()), variables[k] );
	}

	for ( size_t k=0; k<indices.size(); k++) {
	  tt.DrawTextNDC( 0.05 + (1.0*k)/(indices.size()), 0.02, indices[k] );
	}

	tt.DrawTextNDC( 0.1, 0.99, dirname);
	if ( isEndcap ) {
	  if (inp==0 ) tt.DrawTextNDC( 0.35, 0.99, "posZ");
	  else         tt.DrawTextNDC( 0.35, 0.99, "negZ");
	}

	cc->Print(plname);
      }
    }
  }

  infile->Close();

  cc->Print(plname+"]");
}
