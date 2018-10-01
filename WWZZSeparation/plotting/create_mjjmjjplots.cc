/* Script to plot m_jj:m_jj and (mjj+mjj)/2 distributions for WW and ZZ events (+nunu)

Setup largely as in
	- ILC TDR Volume 4, chapter 6.2.3 (Strong EWSB)
	- ILD Letter of intent, chapter 3.3.6
*/


float string_to_float( string str ) {
	return strtof((str).c_str(),0);
}

void create_mjjmjjplots(
    string luminosity_str, string e_beam_pol_str, string p_beam_pol_str,
    string input_dir_base ) {

  // convert input to string
  float luminosity = string_to_float(luminosity_str);
  float e_beam_pol = string_to_float(e_beam_pol_str)/100; // -> from %
  float p_beam_pol = string_to_float(p_beam_pol_str)/100;

  // Physical constants (assumed to not change all that much)
  float M_Z = 91.19; 
  float M_W = 80.38;

  // Standard ROOT plot setting
  gROOT->Reset();
  gStyle->SetOptStat(0);
  TH2::SetDefaultSumw2();

  // Names of input files to be included
  vector<string> filenames;
  filenames.push_back(( input_dir_base + "/vvxyyx/vvxyyx.root" ).c_str());
  filenames.push_back(( input_dir_base + "/vvxxxx/vvxxxx.root" ).c_str());
  filenames.push_back(( input_dir_base + "/vvyyyy/vvyyyy.root" ).c_str());

  // Name of TTree in input files
  string input_tree_name = "eventinfo";

  // Histograms for WW/ZZ events
  TH1* h1_WW = new TH1D("m_WW",
                        "WW/ZZ separation, averaged dijet-mass; (m_{jj,1} + m_{jj,2})/2 [GeV]; Events",
                        70, 50, 120);
  TH1* h1_ZZ = new TH1D("m_ZZ",
                        "WW/ZZ separation, averaged dijet-mass; (m_{jj,1} + m_{jj,2})/2 [GeV]; Events",
                        70, 50, 120);
  TH2* h2_WW = new TH2D("m_m_WW",
                        "WW/ZZ separation, 2D of dijet-masses; m_{jj,1} [GeV];  m_{jj,2} [GeV]",
                        35, 50, 120, 35, 50, 120);
  TH2* h2_ZZ = new TH2D("m_m_ZZ",
                        "WW/ZZ separation, 2D of dijet-masses; m_{jj,1} [GeV];  m_{jj,2} [GeV]",
                        35, 50, 120, 35, 50, 120);


  // Fill histograms for each input file
  for (int i = 0; i<filenames.size(); i++) {
    // Read in input TTree
    TFile *file = TFile::Open((filenames[i]).c_str());
    TTree *tree = (TTree*)file->Get(input_tree_name.c_str());

    int nEntries = tree->GetEntries();

    // Read sample specific variables, should be same for all files
    float cross_section;
    int sample_e_pol, sample_p_pol;
    tree->SetBranchAddress("cross_section", &cross_section);
    tree->SetBranchAddress("e_pol", &sample_e_pol);
    tree->SetBranchAddress("p_pol", &sample_p_pol);
    tree->GetEntry(0);

    // Calculate weight for this input sample
    float polarisation_weight =
                          ( 1 + sample_e_pol * e_beam_pol )/2 *
                          ( 1 + sample_p_pol * p_beam_pol )/2;
    float weight = ( cross_section * luminosity / nEntries ) * polarisation_weight;


    // Declare variables that are to be connected to TBranches of input file
    int true_event_type;
    float pair1_mass, pair2_mass;
    int passed_preselection;

    // Connect TBranch to addresses of the declared variables
    tree->SetBranchAddress("true_event_type", &true_event_type);
    tree->SetBranchAddress("pair1_mass", &pair1_mass);
    tree->SetBranchAddress("pair2_mass", &pair2_mass);
    tree->SetBranchAddress("passed_preselection", &passed_preselection);

    // Fill histograms with each entry
      for (int j = 0; j<nEntries; j++) {
      tree->GetEntry(j);
      if ( passed_preselection != 1 ) { continue; }
      if ( true_event_type == 1 ) {
        h1_WW->Fill((pair1_mass+pair2_mass)/2., weight);
        h2_WW->Fill(pair1_mass, pair2_mass, weight);
      }
      else if ( true_event_type == 2 ) {
        h1_ZZ->Fill((pair1_mass+pair2_mass)/2., weight);
        h2_ZZ->Fill(pair1_mass, pair2_mass, weight);
      }
    }

  }

  // Normalization the 2D mjj:mjj distributions so that ZZ and WW are comparable
  h2_WW->Scale(1./ h2_WW->Integral());
  h2_ZZ->Scale(1./ h2_ZZ->Integral());

  // WW = blue, ZZ = red
  h1_WW->SetLineColor(4);
  h1_ZZ->SetLineColor(2);
  h2_WW->SetLineColor(4);
  h2_ZZ->SetLineColor(2);

  // Do drawing and saving of plots
  TCanvas *canvas_h1 = new TCanvas("can_h1", "", 0, 0, 600, 600);
  h1_WW->Draw("hist");
  h1_ZZ->Draw("hist same");

  gPad->Update();
  float h1_y_min = gPad->GetFrame()->GetY1();
  float h1_y_max = gPad->GetFrame()->GetY2();

  TLine* h1_W_line = new TLine(M_W, h1_y_min, M_W, h1_y_max);
  TLine* h1_Z_line = new TLine(M_Z, h1_y_min, M_Z, h1_y_max);
  h1_W_line->SetLineColorAlpha(9, 0.5);
  h1_Z_line->SetLineColorAlpha(46, 0.5);
  h1_W_line->Draw("same");
  h1_Z_line->Draw("same");

  string plot_name_h1 = input_dir_base + "/m.pdf";
  canvas_h1->Print(plot_name_h1.c_str());

  TCanvas *canvas_h2 = new TCanvas("can_h2", "", 0, 0, 800, 800);
  h2_WW->Draw("box");
  h2_ZZ->Draw("box same");

  gPad->Update();
  float h2_x_min = gPad->GetFrame()->GetX1();
  float h2_x_max = gPad->GetFrame()->GetX2();
  float h2_y_min = gPad->GetFrame()->GetY1();
  float h2_y_max = gPad->GetFrame()->GetY2();

  TLine* h2_W_xline = new TLine(h2_x_min, M_W, h2_x_max, M_W);
  TLine* h2_Z_xline = new TLine(h2_x_min, M_Z, h2_x_max, M_Z);
  TLine* h2_W_yline = new TLine(M_W, h2_y_min, M_W, h2_y_max);
  TLine* h2_Z_yline = new TLine(M_Z, h2_y_min, M_Z, h2_y_max);
  h2_W_xline->SetLineColorAlpha(9, 0.5);
  h2_W_yline->SetLineColorAlpha(9, 0.5);
  h2_Z_xline->SetLineColorAlpha(46, 0.5);
  h2_Z_yline->SetLineColorAlpha(46, 0.5);
  h2_W_xline->Draw("same");
  h2_W_yline->Draw("same");
  h2_Z_xline->Draw("same");
  h2_Z_yline->Draw("same");

  string plot_name_h2 = input_dir_base + "/m_m.pdf";
  canvas_h2->Print(plot_name_h2.c_str());

  // Avoid memory leaks
  delete h1_WW;
  delete h2_WW;
  delete h1_ZZ;
  delete h2_ZZ;
  delete h1_W_line;
  delete h1_Z_line;
  delete h2_W_xline;
  delete h2_Z_xline;
  delete h2_W_yline;
  delete h2_Z_yline;
  delete canvas_h1;
  delete canvas_h2;

}
