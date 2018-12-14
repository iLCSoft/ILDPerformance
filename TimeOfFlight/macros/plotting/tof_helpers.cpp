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
  
  void extract_fit_parameters(string fit_name="gaus") {
    this->mean      = this->histo->GetFunction(fit_name.c_str())->GetParameter(1);
    this->sigma     = this->histo->GetFunction(fit_name.c_str())->GetParameter(2);
    this->mean_err  = this->histo->GetFunction(fit_name.c_str())->GetParError(1);
    this->sigma_err = this->histo->GetFunction(fit_name.c_str())->GetParError(2);
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
  
  void perform_slice_fit(Slice &slice, double &beta) {
    int fit_success = 1;
    ROOT::Math::MinimizerOptions::SetDefaultStrategy(2); // More accurate error calculation
    fit_success = slice.histo->Fit("gaus","QI","",beta*0.95,beta*1.05);
    if ( fit_success == 0 ) { // Confusingly meaning fit was successful
      slice.extract_fit_parameters();
      float fit_range_min = slice.mean-(2.5*slice.sigma);
      float fit_range_max = slice.mean+(2.5*slice.sigma);
      if (fit_range_max > slice.histo->GetXaxis()->GetXmax()) {fit_range_max = slice.histo->GetXaxis()->GetXmax();}
      fit_success = slice.histo->Fit("gaus","QI","",fit_range_min,fit_range_max);
      if ( fit_success == 0 ) { // Confusingly meaning fit was successful
        slice.extract_fit_parameters();
      }
    }
  }
  
  void slice_histo(int bin_step_size, bool do_fit=true){
    if (nullptr == histo) {
      cout << "Can't slice empty histo!" << endl;
      return;
    }
    
    for (int step_begin=1; step_begin<histo->GetNbinsX()+1; step_begin+=bin_step_size){
      int step_end = step_begin + bin_step_size - 1;
      if (step_end > histo->GetNbinsX()) {
        step_end = histo->GetNbinsX();
      }
      int step_center = int(float(step_begin + step_end)/2.0);
      
      double p = histo->GetXaxis()->GetBinCenter(step_center);
      double beta = p/sqrt(ptype.mass*ptype.mass + p*p);

      Slice slice;
      slice.p = p;
      TString slice_name = TString( "beta_CH_" + ptype.name_s + "_bin" + to_string(step_center) + "_p" + to_string(p) );
      slice.histo = histo->ProjectionY(slice_name,step_begin,step_end,"e");
      slice.histo->SetTitle( TString( "#beta_{CH} sliced at p=" + to_string(p) + "GeV, bin " + to_string(step_center) + ", " + ptype.name_l ) );
      
      if (do_fit) {
        this->perform_slice_fit(slice, beta);
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

// This is awful...
// TODO INSTEAD: Add option to add pdg vector to ParticleType
map<string, IntVec> pdg_map {
  {"p",   {2212}},
  {"K",   {130, 310, 311, 321}},
  {"Pi",  {111, 211}},
  {"e1",  {11}},
  {"e2",  {13}}
};
