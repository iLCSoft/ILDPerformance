struct Section {
  string key{};
  map<string,string> variables{};
  
  void clear() {
    key = "";
    variables = {};
  }
  
  string operator   [](string variable) const  {return variables.at(variable);}
  string & operator [](string variable)        {return variables[variable];}
};

class ConfigReader{
  ofstream config_file {};
  string file_name {};
  vector<Section> sections {};
    
  int find_key_index (string key) const {
    int index = 9999;
    for ( int i=0; i<sections.size(); i++) {
      if (sections[i].key == key) {
        index = i;
        break;
      }
    }
    return index;
  }  
    
  public:
    ConfigReader( string _file_name ): file_name(_file_name) {
      this->read_file();
    }
    
    vector<string> get_keys() {
      vector<string> keys {};
      for (auto & section: this->sections) { keys.push_back(section.key); }
      return keys;
    }
    
    Section operator   [](string key) const  {return sections[find_key_index(key)];}
    Section & operator [](string key)        {return sections[find_key_index(key)];}
  
  private:
    
    string find_key(string line){
      string key = "";
      if (line.find("[") != string::npos) {
        auto first = line.find("[")+1;
        auto last = line.find("]");
        key = line.substr(first,last-first);
      }
      return key;
    }
    
    pair<string,string> find_variable(string line){
      pair<string,string> variable_value {};
      if (line.find("=") != string::npos) {
        string line_no_spaces = line;
        line_no_spaces.erase(remove_if(line_no_spaces.begin(), line_no_spaces.end(), ::isspace), line_no_spaces.end());
        auto delimiter  = line_no_spaces.find("=");
        string variable = line_no_spaces.substr(0, delimiter);
        string value    = line_no_spaces.substr(delimiter+1);
        variable_value  = make_pair(variable, value);
      }
      return variable_value;
    } 
    
    void read_file(){
      ifstream myfile(file_name.c_str());
      string line;
      if (myfile.is_open()) {
        Section section {};
        while ( getline (myfile,line) ) {
          string check_for_key = find_key(line);
          if (check_for_key != "") {
            if ( section.key != "" ) { sections.push_back(section); } // Save finished section
            section.clear();
            section.key = check_for_key;
            continue;
          }
          auto check_for_variable = find_variable(line);
          if (check_for_variable.first != "") {
            section.variables[check_for_variable.first] = check_for_variable.second;
          }
        }
        if ( section.key != "" ) { sections.push_back(section); } // Save last section
        myfile.close();
      } else {
        cout << "ERROR: Unable to open config file!" << endl; 
      }
    }
  
};
// 
// 
// void read_config() {
//   string conf_file="../../config/ParticleTypes.cnf";
//   ConfigReader reader(conf_file);
//   auto keys = reader.get_keys();
//   for (auto & key: keys) { 
//     cout << key << endl;
//   }
//   cout << reader["Proton"]["mass"] << endl;
// 
// }