#include "tof_helpers.cpp"
#include "../reading_config/read_config.cpp"

void draw_tof_singleparticle(const string ConfigDir="../..", const string InFileBase = "../../output/SingleParticle_rv02-00-01_sv02-00-01_mILD_l5_o1_v02/", const string OutFileBase = "../../output/SP") {
  string plot_config_path  = ConfigDir + "/PlottingSettings.cnf";
  string ptype_config_path = ConfigDir + "/ParticleTypes.cnf";
  string res_config_path   = ConfigDir + "/TimeResolutions.cnf";

  float min_p    = 0,   max_p    = 100;
  float min_beta = 0.5, max_beta = 1.05;
  ConfigReader plot_settings_reader(plot_config_path);
  auto beta_p_settings = plot_settings_reader["BetaPPlots"];
  int bins_p        = stoi(beta_p_settings["NbinsBeta"]);
  int bins_beta     = stoi(beta_p_settings["NbinsP"]);
  int every_nth_bin = stoi(beta_p_settings["BinJumpsPerSlice"]); // Consider only every n-th bin (=slice) in the momentum distribution
  
  ConfigReader ptype_reader(ptype_config_path);
  ParticleVec particle_types {};
  auto ptype_names = ptype_reader.get_keys();
  for (auto & ptype_name: ptype_names) { 
    auto ptype_sec = ptype_reader[ptype_name]; // Section 
    particle_types.push_back( ParticleType(stoi(ptype_sec["pdgID"]), ptype_sec["name_s"],  ptype_name, stod(ptype_sec["mass"]), stoi(ptype_sec["colour"])) );
  }
  int N_ptypes = particle_types.size();
  
  ConfigReader res_reader(res_config_path);
  ResVec resolutions {};
  auto res_values = res_reader.get_keys();
  for (auto & res_value: res_values) {
    auto res_sec = res_reader[res_value]; 
    resolutions.push_back( Resolution(stof(res_value), stoi(res_sec["colour"])) );
  }
  int N_ress = resolutions.size();
  
  HistoMap beta_histos {};
  
  for ( auto &ptype : particle_types ) {
    for ( auto &res : resolutions) {
      beta_histos.addHisto( ptype, res );
    }
  }
  // TODO Also loop over different time estimators?? (Here: CH -> Closest Hit)

  unique_ptr<TCanvas> c_dummy {new TCanvas("dummy","",1000,800)}; // To write histos to that don't need to be written to current canvas

  unique_ptr<TCanvas> c_beta_p {new TCanvas("c_beta_p","#beta_{CH} vs momentum",400*N_ress,400*N_ptypes)}; 
  unique_ptr<TCanvas> c_beta_p_average {new TCanvas("c_beta_p_average","#beta_{CH}^{average} vs momentum",400*N_ress,400*N_ptypes)}; 
  c_beta_p->Clear();
  c_beta_p_average->Clear();
  c_beta_p->Divide(resolutions.size(), particle_types.size());
  c_beta_p_average->Divide(resolutions.size(), particle_types.size());
  int i_pad=0;
  
  vector<TFile*> closables {}; // Because ROOT... To collect files that I need to close later
  
  unique_ptr<TFile> file_beta_p {new TFile ( ( OutFileBase + "_beta_p.root" ).c_str() ,"recreate")};
  
  // Get the beta vs p histograms and save for each particle type
  for ( auto & ptype : particle_types ) {
    TFile* file = new TFile ( ( InFileBase + to_string(ptype.pdg_id) + "_lctuple.root" ).c_str() ,"r");
    TTree* tree = (TTree*) file->Get("MyLCTuple");
    
    TF1* beta_func = new TF1( ("beta_func_" + ptype.name_s).c_str(), "x / sqrt(x^2 + [0]^2)", min_p, max_p);
    beta_func->SetParameter(0, ptype.mass);
    beta_func->SetLineWidth(1);
    beta_func->SetLineColorAlpha(6, 0.5);
    file_beta_p->WriteTObject(beta_func);
    
    for (auto &res : resolutions) {
      i_pad++;
      c_beta_p->cd(i_pad);

      string h_beta_name = "h_beta_" + ptype.name_s + "_" + res.str();

      TH2F *h_beta = new TH2F( h_beta_name.c_str(),
                              ("#beta_{CH} vs momentum, " + ptype.name_l + ", " + res.w_unit() + "; p [GeV/c] ; #beta ").c_str(),
                              bins_p, min_p, max_p, bins_beta, min_beta, max_beta
                             );
      
      string tof_res = "tof" + res.str();
      string draw_string = tof_res + "len/" + tof_res + "ch/299.8:sqrt(rcmox*rcmox+rcmoy*rcmoy+rcmoz*rcmoz)>>" + h_beta_name;
      string cut_string  = tof_res + "len>1.&&" + tof_res + "ch>0.1&&(" + tof_res + "len/" + tof_res + "ch/299.8)<1.1 && sqrt(rcmox*rcmox+rcmoy*rcmoy+rcmoz*rcmoz)<100.&&abs(rccha)>0.005 && abs(rcmoz)/sqrt(rcmox*rcmox+rcmoy*rcmoy+rcmoz*rcmoz) < 0.792573 && sqrt(rcmox*rcmox+rcmoy*rcmoy) > 0.948543 && nrec==1";
        
      tree->Draw( draw_string.c_str(), cut_string.c_str(), "colz" );
      file_beta_p->WriteTObject(h_beta);
      beta_func->Draw("same");
      
      c_dummy->cd();
      beta_histos.getHisto(res, ptype)->histo = h_beta;
      beta_histos.getHisto(res, ptype)->slice_histo(every_nth_bin);
      
      c_beta_p_average->cd(i_pad);
      string h_beta_average_name = "h_beta_average_" + ptype.name_s + "_" + res.str();
      TH1F* h_beta_average = new TH1F( h_beta_average_name.c_str(),
                                       ("#beta_{CH}^{average} vs momentum, " + ptype.name_l + ", " + res.w_unit() + ", error = #sigma_{gaus}; p [GeV/c] ; #beta ").c_str(),
                                       bins_p, min_p, max_p
                                     );
      for ( auto & slice: beta_histos.getHisto(res, ptype)->slices ) {
        int bin = h_beta_average->FindBin(slice.p);
        if (slice.fit_successful) {
          h_beta_average->SetBinContent(bin, slice.mean);
          h_beta_average->SetBinError(  bin, slice.sigma);
        }
      }
      h_beta_average->SetMarkerSize(0);
      h_beta_average->SetFillColor(2);
      h_beta_average->Draw("E5");
      beta_func->Draw("same");
      
      file_beta_p->WriteTObject(h_beta_average);
      file->cd();
    }
    closables.push_back(file);
  }
  c_beta_p->Print( (OutFileBase + "_beta_p.pdf").c_str() );
  c_beta_p_average->Print( (OutFileBase + "_beta_p_average.pdf").c_str() );
  
  file_beta_p->Close();
  
  // Save the sliced histograms
  for ( auto & res: resolutions ) {
    unique_ptr<TFile> file_slices { new TFile( (OutFileBase + "_slices_" + res.w_unit() + ".root").c_str(), "RECREATE" ) };

    for ( auto & histo: beta_histos.getHistosToRes(res) ) {
      for ( auto & slice: histo->slices ) {
        slice.histo->Write();
      }
    }
    file_slices->Close();
  }
  
  unique_ptr<TFile> file_sep_pwr { new TFile( (OutFileBase + "_separation_powers.root").c_str(), "RECREATE" ) };
  // Find separation power for all combinations 
  for (int i=0; i<N_ptypes-1; i++) {
    if ( 1 == N_ptypes ) {
      cout << "Only one particle type given, not creating separation power plots." << endl;
      break;
    }
    for (int j=i+1; j<N_ptypes; j++) { // Loop avoids dublicats
      ParticleType ptype1 = particle_types[i];
      ParticleType ptype2 = particle_types[j];
      
      unique_ptr<TCanvas> c_sep_pwr {new TCanvas( ("c_sep_pwr_" + ptype1.name_s + "_" + ptype2.name_s).c_str() ,
                                                  "Separation power vs momentum",
                                                  800*resolutions.size(),800)}; 
      c_sep_pwr->Divide(resolutions.size(), 1);
      int i_seppwr_pad = 1;
      
      vector<pair<Resolution,TH1D*>> sep_pwr_hists {};
      
      for ( auto & res: resolutions ) {
        auto slices_p1 = beta_histos.getHisto(res, ptype1)->slices;
        auto slices_p2 = beta_histos.getHisto(res, ptype2)->slices;
      
        int N_slices = slices_p1.size();
        
        TH1D* h_seppwr = new TH1D(("h_seppwr_" + ptype1.name_s + ptype2.name_s + res.str()).c_str(), 
                                  (ptype1.name_l + " - " + ptype2.name_l + " TOF-separation, " + res.w_unit() + "; p [GeV/c] ; Separation Power [#sigma]").c_str(),
                                  N_slices, min_p, max_p);
                                  
        for (int i_slice=0; i_slice<N_slices; i_slice++) {
          auto slice_p1 = slices_p1[i_slice];
          auto slice_p2 = slices_p2[i_slice];
          
          double p = slice_p1.p;
          
          if ( ! (slice_p1.fit_successful && slice_p2.fit_successful) ) {
            cout << ptype1.name_l << " - " << ptype2.name_l << ", " << res.w_unit() << 
                    " : Fit in slice p=" << p << " not successful, point won't be drawn." << endl;
            continue;
          }
          
          double diff_mean     = abs(slice_p1.mean - slice_p2.mean);
          double sigma_sqr_sum = pow(slice_p1.sigma, 2) + pow(slice_p2.sigma, 2); 
          double ms_sigma      = sigma_sqr_sum / 2.0; // ms = mean squared 
          double rms_sigma     = sqrt( ms_sigma );    // rms = root mean squared
          
          double sep_pwr     = diff_mean / rms_sigma;
          double sep_pwr_err = sqrt( (pow(slice_p1.mean_err,2) + pow(slice_p2.mean_err,2))/ms_sigma + pow(diff_mean,2) * (pow(slice_p1.sigma,2)*pow(slice_p1.sigma_err,2) + pow(slice_p2.sigma,2)*pow(slice_p2.sigma_err,2)) / (4*pow(ms_sigma,3)) );
          // Check that sep pwr is not NaN
          if ( sep_pwr != sep_pwr) { sep_pwr = sep_pwr_err = 0; }
          
          h_seppwr->SetBinContent(h_seppwr->FindBin(p), sep_pwr);
          h_seppwr->SetBinError(  h_seppwr->FindBin(p), sep_pwr_err);
        }          
        
        h_seppwr->Write();
        
        c_sep_pwr->cd(i_seppwr_pad++);
        h_seppwr->Draw("pe"); // TODO Better drawing style: Set some better markers, REMOVE X ERROR
        h_seppwr->SetMinimum(0);
        h_seppwr->SetMaximum(40);
        
        sep_pwr_hists.push_back(make_pair(res,h_seppwr));
      }
      c_sep_pwr->Write();
      
      unique_ptr<TCanvas> c_sep_pwr_comb {  new TCanvas( ("c_sep_pwr_comb_" + ptype1.name_s + "_" + ptype2.name_s).c_str() ,
                                                          "Separation power vs momentum",
                                                          800,800)
                                          };
      unique_ptr<THStack> h_sep_pwr_comb { new THStack(
                                                ("h_seppwr_comb_" + ptype1.name_s + ptype2.name_s).c_str(), 
                                                (ptype1.name_l + " - " + ptype2.name_l + " TOF-separation; p [GeV/c] ; Separation Power [#sigma]").c_str())
                                          };
      unique_ptr<TLegend> leg_sep_pwr_comb { new TLegend(0.48,0.7,0.8,0.9) };
      for (auto &res_hist: sep_pwr_hists) {
        Resolution res  = res_hist.first;
        TH1D* h_seppwr  = res_hist.second;
        leg_sep_pwr_comb->AddEntry(h_seppwr, res.w_unit().c_str(), "lep");
        h_sep_pwr_comb->Add(h_seppwr);
        h_seppwr->SetLineColor(res.colour_index);
      }
      h_sep_pwr_comb->Draw("nostack");
      h_sep_pwr_comb->SetMinimum(0);
      if( h_sep_pwr_comb->GetMaximum() > 120 ) { h_sep_pwr_comb->SetMaximum(100); }
      gPad->Modified();
      gPad->Update();
      leg_sep_pwr_comb->Draw();
      c_sep_pwr_comb->Write(); 
    }
  }
  file_sep_pwr->Close();

  for ( auto & closable : closables ) { 
    closable->Close();
    delete closable;
  } 
}