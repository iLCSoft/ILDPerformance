#include "tof_helpers.cpp"

// TODO More status reports!!!
// TODO CHANGE DEFAULT IN FILE NAME
void draw_tof_ttbar( const string InFile = "lctuplep.root", const string OutFileBase = "ttbar" ){

  ParticleVec particle_types {};
  particle_types.push_back( ParticleType(2212, "p",  "Proton", 0.938272, kRed+1) );
  particle_types.push_back( ParticleType(321,  "K",  "Kaon", 0.493677, kGreen+2) );
  particle_types.push_back( ParticleType(211,  "Pi", "Pion", 0.13957, kViolet) );
  particle_types.push_back( ParticleType(11,   "e1", "Electron", 0.000511, kCyan+1) );
  particle_types.push_back( ParticleType(13,   "e2", "Muon", 0.1056584, kBlue-3) );

  ResVec resolutions {};
  resolutions.push_back( Resolution(0,  kBlue) );
  resolutions.push_back( Resolution(10, kGreen+1) );
  resolutions.push_back( Resolution(50, kRed+2) );
  
  HistoMap beta_histos {};
  ParticleType ttbar = ParticleType("ttbar", "ttbar");
  for ( auto &res : resolutions) {
    beta_histos.addHisto( ttbar, res );
  }
  
  int bins_per_slice = 10;
  
  unique_ptr<TCanvas> c_dummy {new TCanvas("dummy","",1000,800)}; // To write histos to that don't need to be written to current canvas
  
  unique_ptr<TFile> file { new TFile (InFile.c_str(),"r") };
  TTree* tree = (TTree*) file->Get("MyLCTuple");

  unique_ptr<TFile> file_all_ptypes { new TFile( (OutFileBase + "_all_particles.root").c_str(), "RECREATE" ) };

  for (auto &res : resolutions) {

    unique_ptr<TCanvas> c_beta_p { new TCanvas( ("c_beta_p_" + res.str()).c_str(),
                                                ("#beta vs momentum, ttbar, " + res.w_unit()).c_str(),
                                                500,500) }; //(plot, name, [ 10, -5, -10 100 all looks same] )
    c_beta_p->Clear();

    string h_beta_name = "h_beta_" + res.str();
    TH2F *h_beta = new TH2F( h_beta_name.c_str(),
                             ("#beta vs momentum, ttbar, " + res.w_unit()).c_str(), 
                             200, 0, 10., 200, 0.5, 1.); // (plot ,name ,nbin x ,x low ,x up ,nbin y ,y low ,y up)   
                             
    string tof_res = "tof" + res.str();
    string draw_string = tof_res + "len/" + tof_res + "ch/299.8:sqrt(rcmox*rcmox+rcmoy*rcmoy+rcmoz*rcmoz)>>" + h_beta_name;
    string cut_string  = tof_res + "len>1.&&" + tof_res + "ch>0.1&&(" + tof_res + "len/" + tof_res + "ch/299.8)<1.1 && sqrt(rcmox*rcmox+rcmoy*rcmoy+rcmoz*rcmoz)<10.&&abs(rccha)>0.01";
    tree->Draw( draw_string.c_str(), cut_string.c_str() );

    h_beta->Write();

    for ( auto & ptype : particle_types ) {
      TF1* beta_func = new TF1( ("beta_func_" + ptype.name_l).c_str(), "x / sqrt(x^2 + [0]^2)", 0., 10);
      beta_func->SetParameter(0, ptype.mass);
      beta_func->Draw("same");
      beta_func->SetLineColor(ptype.colour_index);
      
    }
    c_beta_p->Update();
    c_beta_p->Write();
    c_beta_p->Close();
    
    // c_dummy->cd();
    beta_histos.getHisto(res, ttbar)->histo = h_beta;
    beta_histos.getHisto(res, ttbar)->slice_histo(bins_per_slice, false);
    
    int N_slices = beta_histos.getHisto(res, ttbar)->slices.size();
    unique_ptr<TCanvas> c_beta_p_slices { new TCanvas(  ("c_beta_p_slices_" + res.str()).c_str(),
                                                        ("#beta, p slices, ttbar, " + res.w_unit()).c_str(),
                                                        500*N_slices,500) }; //(plot, name, [ 10, -5, -10 100 all looks same] )
    c_beta_p_slices->Divide(N_slices, 1);
    int i_pad = 1;
    
    unique_ptr<TDirectory> dir_slices { file_all_ptypes->mkdir( ("slices_" + res.str()).c_str() ) };
    for ( auto & slice: beta_histos.getHisto(res, ttbar)->slices ) {
      c_beta_p_slices->cd(i_pad++);
      slice.histo->Draw();
      
      for ( auto & ptype : particle_types ) {
        double p = slice.p;
        double beta = p/sqrt(ptype.mass*ptype.mass + p*p);
        TLine *line = new TLine(beta,0.0,beta,1000);
        line->SetLineColor(ptype.colour_index);
        line->Draw();
      }
      
      dir_slices->cd();
      slice.histo->Write();
    }
    dir_slices->Write();
    file_all_ptypes->cd();
    c_beta_p_slices->Update();
    c_beta_p_slices->Write();
    c_beta_p_slices->Close();
  }
  
  file_all_ptypes->Close();
  
  //////////////////////////////////////////////////////////////////////////////
  unique_ptr<TFile> file_diff { new TFile( (OutFileBase + "_tof_diffs.root").c_str(), "RECREATE" ) };

  MethodVec tof_methods {}; // TODO Move this to start and make universal also in other draw
  tof_methods.push_back(Method("ch", "Closest Hits"));
  tof_methods.push_back(Method("fh", "First Hit"));
  tof_methods.push_back(Method("th", "Last Tracker Hit"));

  for (int i=0; i<resolutions.size()-1; i++) {
    if ( 1 == resolutions.size() ) {
      cout << "Only one resolution given, not creating difference plots." << endl;
      break;
    }
    Resolution res_1 = resolutions[i];
    
    for (int j=i+1; j<resolutions.size(); j++) {
      Resolution res_2 = resolutions[j];
    
      for ( auto & method: tof_methods ) {
        unique_ptr<TCanvas> c_tof_diff { new TCanvas( ("c_tof_diff_" + res_1.str() + res_2.str() + method.name_s).c_str(),
                                                      ("TOF difference " + method.name_l + ", " + res_1.w_unit() + "-" + res_2.w_unit()).c_str(),
                                                      500, 500) };
        c_tof_diff->Clear(); 
        string h_diff_name = "h_tof_diff_" + res_1.str() + res_2.str() + method.name_s;
        TH1D *h_tof_diff = new TH1D(  h_diff_name.c_str(),
                                      ("#Delta TOF_{" + method.name_s + "}, " + res_1.w_unit() + "-" + res_2.w_unit() + "; #Delta TOF; #").c_str(), 
                                      1000, -1. ,1.);
        
                                      
        string draw_string = "tof" + res_1.str() + method.name_s + "-tof" + res_2.str() + method.name_s + ">>" + h_diff_name;
        string cut_string  = "tof" + res_1.str() + method.name_s + " != -1 && tof" + res_2.str() + method.name_s + " != -1";
        tree->Draw(draw_string.c_str(), cut_string.c_str());
        
        c_tof_diff->Update();
        c_tof_diff->Write();
      } 
    }
  }

  file_diff->Close();


  //////////////////////////////////////////////////////////////////////////////

  const int max_N_rec = 3000;
  
  Int_t nmcp;
  Int_t mcpdg[max_N_rec];
  Int_t N_rec;
  Float_t rcmox[max_N_rec], rcmoy[max_N_rec], rcmoz[max_N_rec], rccha[max_N_rec];

  Int_t r2mnrel, r2mf[max_N_rec], r2mt[max_N_rec];
  Float_t r2mw[max_N_rec];
  Float_t m2rw[max_N_rec];
   
  tree->SetBranchAddress("nmcp",&nmcp);
  tree->SetBranchAddress("mcpdg",&mcpdg);
  tree->SetBranchAddress("nrec",&N_rec);
  tree->SetBranchAddress("rcmox",rcmox);
  tree->SetBranchAddress("rcmoy",rcmoy);
  tree->SetBranchAddress("rcmoz",rcmoz);
  tree->SetBranchAddress("rccha",rccha);
  tree->SetBranchAddress("r2mnrel",&r2mnrel);
  tree->SetBranchAddress("r2mf",&r2mf);
  tree->SetBranchAddress("r2mt",&r2mt);
  tree->SetBranchAddress("r2mw",&r2mw);
  
  int N_entries = tree->GetEntries();
  
  unique_ptr<TFile> file_individual_ptypes { new TFile( (OutFileBase + "_individual_types.root").c_str(), "RECREATE" ) };
  
  for ( auto & res: resolutions ) {
    Float_t tof_len[max_N_rec];
    Float_t tof_ch[max_N_rec];
    tree->SetBranchAddress( ("tof" + res.str() + "len").c_str()  ,&tof_len);
    tree->SetBranchAddress( ("tof" + res.str() + "ch").c_str()   ,&tof_ch );
    
    for ( auto & ptype: particle_types ) {
      
      // TODO delete or make unique
      TH2F *h_beta_p = new TH2F(  ("ttbar_beta_p_" + ptype.name_s + "_" + res.str()).c_str(),
                                  ("#beta vs p, " + ptype.name_l + ", " + res.w_unit() + "; p; #beta; #" + ptype.name_l).c_str(),
                                  200, 0, 10., 200, 0.5, 1.05 );
        
                                  // for (int i=0; i< 566; i++)
      for (int i=0; i<N_entries; i++) {
        tree->GetEntry(i);
       
        for (int i_rec=0; i_rec<N_rec; i_rec++) {
          Double_t weight = 0.0;
          Double_t calweight;
          Int_t i_mc = -1;
          int final_i_rel = -1;
          
          for (int k=0; k<r2mnrel; k++) {
            if (r2mf[k] == i_rec) {     
              calweight =  double((int(r2mw[k])/10000)/1000.);
              if (calweight > weight) {
                final_i_rel = k ;
                weight = calweight;
              }
            }  
          }
          
          if ( weight > 0.9 ) {

            if (final_i_rel < 0 || final_i_rel >= r2mnrel ) {
              cout << "ERROR: MCParticle index out of range: final = " << final_i_rel << ", r2mnrel = " << r2mnrel << endl;
              continue;
            }
            i_mc  = r2mt[final_i_rel];     

            if (i_mc < 0 || i_mc >= nmcp ) {
              cout << "ERROR: MCParticle index out of range: MCP = " << i_mc << ", nmcp = " << nmcp << endl;
              continue; 
            }

            IntVec ptype_pdgs = pdg_map[ptype.name_s];
            bool mc_is_ptype = ( find(ptype_pdgs.begin(), ptype_pdgs.end(), abs(mcpdg[i_mc])) != ptype_pdgs.end() );

            if ( mc_is_ptype ) {
              double p = sqrt(rcmox[i_rec]*rcmox[i_rec]+rcmoy[i_rec]*rcmoy[i_rec]+rcmoz[i_rec]*rcmoz[i_rec]);
              double beta = (tof_len[i_rec]/tof_ch[i_rec]/299.8);

              if ( beta <1.1 && p <10. && tof_len[i_rec]>1. && tof_ch[i_rec]>0.1 && abs(rccha[i_rec])>0.01 && abs(rcmoz[i_rec])/sqrt(rcmox[i_rec]*rcmox[i_rec]+rcmoy[i_rec]*rcmoy[i_rec]+rcmoz[i_rec]*rcmoz[i_rec]) > 0.79257283 ) {
                h_beta_p->Fill(p, beta);	
              }   
            }
          }
        }
      }
      h_beta_p->Draw("box");
      h_beta_p->Write();
    }
  }
  file_individual_ptypes->Close();
  
  file->Close();
}
