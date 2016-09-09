#include "TSystem.h"
#include <sstream>

void Pulls(const char* ext = ".root")
{
  const char* inDir = "../Results/Analysis";
  
  char* dir = gSystem->ExpandPathName(inDir);
  void* dirp = gSystem->OpenDirectory(dir);
  
  const char* entry;
  const char* filename[100];
  const char* filenametrunk[100];
  Int_t n = 0;
  TString str;
  
  while((entry = (char*)gSystem->GetDirEntry(dirp))) {
    str = entry;
    if(str.EndsWith(ext))
      filename[n++] = gSystem->ConcatFileName(dir, entry);
  }

  TCanvas *Pulls = new TCanvas("Pulls","Pulls",800,800);
  Pulls->Print("pulls.pdf[");
  
  for (Int_t i = 0; i < n; i++){
    Printf("file -> %s", filename[i]);
    TFile *f1 = new TFile(filename[i], "read");
    //TDirectory *d1 = (TDirectory*) f1->Get("MyRecoMCTruthLinker");
    Pulls = (TCanvas*)f1->Get("pulls");
    Pulls->SetTitle(filename[i]);

    // std::stringstream strs ;
    // strs << "Title: " << filename[i] ;

    std::string dummy("Title: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    std::sprintf( &dummy[0] , "Title: %52.52s" , filename[i] ) ;

    //Pulls->Draw();
    //Pulls->Print("pulls.pdf", strs.str().c_str() );
    Pulls->Print("pulls.pdf",  &dummy[0]);
  }

  Pulls->Print("pulls.pdf]");

}
