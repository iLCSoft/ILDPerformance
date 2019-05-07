#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#include "TFile.h"
#include "TDirectory.h"
#include "TList.h"
#include "TKey.h"
#include "ILDStyle_Standalone.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TStyle.h"
#include "TPaletteAxis.h"
#include "TPaveStats.h"

#include "TH2.h"

using namespace std;

// g++ -o dEdxHistPlotter.exe dEdxHistPlotter.cc `root-config --cflags --glibs`

string fileFormat = ".png";

TCanvas* can1D = new TCanvas("Canvas1D","Canvas1D",800,700);
TCanvas* can2D = new TCanvas("Canvas2D","Canvas2D",900,700);
TImage* img = TImage::Create();


const char* namepart(string name, int fl, unsigned int n)
{
  if (name.size()>=n)
  {
    if (fl==1) return (string (name,0,n)).c_str();
    else return (string (name,name.size()-n,name.size())).c_str();
  }
  else return (const char*)("");
}

void PlotHist(TObject* obj, string outpath)
{
  if (obj->InheritsFrom("TH3")) return;

  string name = obj->GetName();
  std::stringstream s; s << outpath << "/" << name << fileFormat;

  string check_f2(name,0,2);
  string check_f3(name,0,3);
  string check_l3(name,name.size()-3,name.size());
  //string check_l5(name,name.size()-5,name.size());

  // from the style file:
  gStyle->SetPadTopMargin(0.08);
  gStyle->SetPadRightMargin(0.08);
  gStyle->SetOptStat(0);
  gROOT->ForceStyle();

  bool log = false;
  double ymax = 0;
  bool capmis = false;
  bool cap_sp = false;
  //bool MargR = false;
  //bool MargL = false;

  if ((!strcmp(namepart(name,1,2),"BB") && strcmp(namepart(name,-1,3),"Sum")))
  {
    gStyle->SetPadTopMargin(0.12);
    log = true;
  }

  if (!strcmp(namepart(name,1,2),"SP"))
  {
    log = true;
    ymax = 10;
    cap_sp = true;
  }

  if (!strcmp(namepart(name,1,3),"Res"))
  {
    gStyle->SetOptStat("mr");
    gROOT->ForceStyle();

    if (!strcmp(namepart(name,-1,5),"_Norm") || !strcmp(namepart(name,-1,5),"_1GeV"))
    {
      gStyle->SetPadTopMargin(0.12);

    }
    else
    {
      capmis = true;
      gStyle->SetPadRightMargin(0.14);
    }
  }


  if (obj->InheritsFrom("TH2"))
  {
    //cout << "TH2: " << obj->GetName() << endl;
    TH2* hist = dynamic_cast<TH2*>(obj);

    gStyle->SetPadRightMargin(0.24);
    can2D->UseCurrentStyle();
    if (log) can2D->SetLogx();
    can2D->cd();
    can2D->Update();
    hist->Draw("colz");
    TPaletteAxis* palette = (TPaletteAxis*)hist->GetListOfFunctions()->FindObject("palette");
    if (palette)
    {
      palette->SetX1NDC(0.78);
      palette->SetX2NDC(0.82);
      palette->SetY1NDC(0.18);
      palette->SetY2NDC(0.92);
    }
    img->FromPad(can2D);
  }
  else
  {

    can1D->UseCurrentStyle();
    TH1* hist = dynamic_cast<TH1*>(obj);
    can1D->cd();
    can1D->Update();

    hist->SetMinimum(0);
    if (ymax) hist->SetMaximum(ymax);
    hist->Draw();
    can1D->UseCurrentStyle();
    if (log) can1D->SetLogx();
    if (capmis) {hist->SetXTitle("dE/dx / (GeV/mm)"); hist->SetYTitle("abundance");}
    if (cap_sp) {hist->SetYTitle("separation power");}
    can1D->Update();

    TPaveStats* tps = dynamic_cast<TPaveStats*>(hist->GetListOfFunctions()->FindObject("stats"));
    if (tps)
    {
      //cout << "JOJOJO" << endl;
      tps->SetX1NDC(.6);
      tps->SetY1NDC(.75);
      tps->SetY2NDC(.9);
    }


    //can1D->Update();
    //can1D->Modified();
    img->FromPad(can1D);
    //can1D->SaveAs(s.str().c_str());
  }


  img->WriteImage(s.str().c_str());
}


void DirectoryHandler(TObject* dirobj, string outpath)
{
  TDirectory* dir = dynamic_cast<TDirectory*>(dirobj);
  string newpath = outpath;
  if (!dir->InheritsFrom("TFile")) newpath.append(dir->GetName());
  mkdir(newpath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  TIter iter(dir->GetListOfKeys());
    while (TObject* obj = iter())
    {
      TKey* key = dynamic_cast<TKey*>(obj);
      cout << key->GetClassName() << " " << key->GetTitle() << endl;
      const char* type = key->GetClassName();

      if (!strcmp(type,"TDirectoryFile") || !strcmp(type,"TDirectory"))
        DirectoryHandler(key->ReadObj(), newpath);

      string type0(type,2);
      if (!strcmp(type0.c_str(),"TH"))
        PlotHist(key->ReadObj(),newpath);
    }
}


void HistPlotter(string infile, string outpath)
{
  cout << "HistPlotter called." << endl;
  TFile* file = new TFile(infile.c_str(), "read");

  if (file->IsOpen())
  {
    TStyle* MyStyle = new  TStyle("MyStyle", "My Style");
    ILDStyle(MyStyle);
    MyStyle->cd();
    gROOT->ForceStyle();
    gStyle->ls();

    gStyle->SetPalette(kBird);
    gStyle->SetPadGridX(1);
    gStyle->SetPadGridY(1);

    can1D->UseCurrentStyle();

    can2D->UseCurrentStyle();
    can2D->SetGrid();


    DirectoryHandler(file, outpath);
  }
}



int main(int argv, char **argc)
{

  if(argv < 3)
    {
      cout << "Two inputs required: input root file, output path \n";
      cout << "Optional third input: output file format, e. g. .png \nThe end." << endl;
      return 0;
    }

  string infile  = string(argc[1]);
  string outpath = string(argc[2]);
  if (outpath.back()!='/') outpath.append("/");
  if (argv > 3) fileFormat = string(argc[3]);

  cout << "Input root file: "  << infile << endl;
  cout << "Output directory: " << outpath << endl;
  cout << "Output file format: " << fileFormat << endl;

  if (access(argc[1], R_OK) == -1)
  {
    cout << "Input file is not accessible! \nThe end." << endl;
    return 0;
  }
  if (access(argc[2], W_OK) == -1)
  {
    cout << "Output path is not write accessible! \nThe end." << endl;
    return 0;
  }

  HistPlotter(infile, outpath);

  cout << "Done." << endl;
  return 1;
}
