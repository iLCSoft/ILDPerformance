struct ParticleType {
  int pdg_id {};
  string name_s {};
  string name_l {};
  float mass {}; // in GeV
  Color_t colour_index {};

  ParticleType( int _pdg_id=0, string _name_s="", string _name_l="", float _mass=0, Color_t _colour_index=0 ) : 
    pdg_id(_pdg_id), name_s(_name_s),  name_l(_name_l),  mass(_mass), colour_index(_colour_index) {};
    
  // Copy constructor
  ParticleType(const ParticleType &_ptype) : 
    pdg_id(_ptype.pdg_id), name_s(_ptype.name_s), name_l(_ptype.name_l), mass(_ptype.mass), colour_index(_ptype.colour_index) {};
    
  bool operator== (const ParticleType &ptype){
    return pdg_id == ptype.pdg_id && name_s == ptype.name_s && name_l == ptype.name_l && mass == ptype.mass;
  }
};

struct Resolution {
  float value {};
  string unit {};
  Color_t colour_index {};
  
  Resolution( float _value=0, Color_t _colour_index=0 , string _unit="ps") :
    value(_value), unit(_unit), colour_index(_colour_index) {};
  
  bool operator== (const Resolution &res){
    return value == res.value && unit == res.unit;
  }
      
  string str() const {
    string value_str = to_string(value);
    if ( roundf(value) == value ) { value_str = to_string(int(value)); }
    return value_str;
  }
  
  string w_unit() const {
    return this->str() + unit;
  }
};

struct Method {
  string name_s {};
  string name_l {};
  Color_t colour_index {};
  
  Method( string _name_s="", string _name_l="", Color_t _colour_index=0 ) : 
    name_s(_name_s), name_l(_name_l), colour_index(_colour_index) {};
};

struct Slice {
  double p {};
  TH1D* histo {};
  double mean {};
  double sigma {};
  double mean_err {};
  double sigma_err {};
  bool fit_successful {false};
  
  void extract_fit_parameters(string fit_name="gaus") {
    auto fit_result = this->histo->GetFunction(fit_name.c_str());
    this->mean      = fit_result->GetParameter(1);
    this->sigma     = fit_result->GetParameter(2);
    this->mean_err  = fit_result->GetParError(1);
    this->sigma_err = fit_result->GetParError(2);
  }
};

struct Histo {
  ParticleType ptype;
  Resolution res;
  TH2F* histo {};
  vector<Slice> slices {};
  
  Histo( ParticleType &_ptype, Resolution &_res ){
    ptype = _ptype;   
    res   = _res; 
  }
  
  ~Histo(){ // TODO Had to remove this to avoid seg fault at end of ttbar script. Don't know how to fix.
    // delete histo;
  }
  
  void perform_slice_fit(Slice &slice, double &beta_low, double &beta_up) {
    if (slice.histo->GetEntries() < 1) { return; }
    
    double delta_beta = (beta_up-beta_low)/2.0;
    
    double fit_low_edge = max( 0.99 * (beta_low - 3.0*delta_beta), 0.0 );
    double fit_up_edge  = min( 1.01 * (beta_up  + 3.0*delta_beta), 1.1 );
    
    ((TF1*)gROOT->GetFunction("gaus"))->SetParLimits(0, 0, 2.0); // Gaussian amplitude
    ((TF1*)gROOT->GetFunction("gaus"))->SetParLimits(1, 0, 1.5); // Mean
    ((TF1*)gROOT->GetFunction("gaus"))->SetParLimits(2, delta_beta/2.0, 1.0); // Sigma
    
    int fit_success = 1;
    ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
    fit_success = slice.histo->Fit("gaus","QI","",fit_low_edge,fit_up_edge);
    if ( fit_success == 0 ) { // Confusingly meaning fit was successful
      slice.extract_fit_parameters();
      
      fit_low_edge = min( slice.mean-(2.5*slice.sigma), 0.99 );
      fit_up_edge  = max( slice.mean+(2.5*slice.sigma), 1.1  );
      
      ROOT::Math::MinimizerOptions::SetDefaultStrategy(2); // More accurate error calculation
      fit_success = slice.histo->Fit("gaus","QI","",fit_low_edge,fit_up_edge);
      if ( fit_success == 0 ) { // Confusingly meaning fit was successful
        slice.fit_successful = true;
        slice.extract_fit_parameters();
      }
      ROOT::Math::MinimizerOptions::SetDefaultStrategy(1); // Re-set to default
    }
    
  }
  
  void slice_histo(int bin_step_size, bool do_fit=true){
    if (nullptr == histo) {
      cout << "Can't slice empty histo!" << endl;
      return;
    }
    
    for (int slice_bin=1; slice_bin<histo->GetNbinsX()+1; slice_bin+=bin_step_size){
      double p = histo->GetXaxis()->GetBinCenter(slice_bin);
      double p_low = histo->GetXaxis()->GetBinLowEdge(slice_bin);
      double p_up  = histo->GetXaxis()->GetBinUpEdge(slice_bin);
      
      double beta_low = p_low/sqrt(ptype.mass*ptype.mass + p_low*p_low);
      double beta_up  = p_up /sqrt(ptype.mass*ptype.mass + p_up *p_up);
      
      Slice slice;
      slice.p = p;
      TString slice_name = TString( "beta_CH_" + ptype.name_s + "_bin" + to_string(slice_bin) + "_p" + to_string(p) );
      slice.histo = histo->ProjectionY(slice_name,slice_bin,slice_bin,"e");
      slice.histo->SetTitle( TString( "#beta_{CH} sliced at p=" + to_string(p) + "GeV, bin " + to_string(slice_bin) + ", " + ptype.name_l ) );
      slice.histo->Scale(1.0/slice.histo->Integral());
      
      if (do_fit) {
        this->perform_slice_fit(slice, beta_low, beta_up);
      }
      
      slices.push_back(slice);
    }
  }
  
  
};
typedef vector<Histo> HistoVec;
typedef vector<shared_ptr<Histo>> HistoPtrVec;

struct HistoMap {
  HistoPtrVec histos {};
  
  HistoMap() {};

  void addHisto( ParticleType &_ptype, Resolution &_res ) {
    histos.push_back( make_shared<Histo>(_ptype, _res) );
  }
  
  HistoPtrVec getHistosToRes(Resolution &res) const{
    HistoPtrVec histos_to_res{};
    for( auto & histo: histos ) {
      if ( histo->res == res ) { histos_to_res.push_back(histo); }
    }
    return histos_to_res;
  }
  
  HistoPtrVec getHistosToPType(ParticleType &ptype) const{
    HistoPtrVec histos_to_ptype{};
    for( auto & histo: histos ) {
      if ( histo->ptype == ptype ) { histos_to_ptype.push_back(histo); }
    }
    return histos_to_ptype;
  }
  
  shared_ptr<Histo> getHisto(Resolution &res, ParticleType &ptype) {
    for ( auto &histo: histos) {
      if (histo->res == res && histo->ptype == ptype) {
        return histo;
      }
    }
    return nullptr;
  }
};

typedef vector<ParticleType>  ParticleVec; 
typedef vector<Resolution>    ResVec; 
typedef vector<Method>        MethodVec;
typedef vector<float> FloatVec;
typedef vector<int>   IntVec;