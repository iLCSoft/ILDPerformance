#ifndef _validatePilotProcessor_maxMin_
#define _validatePilotProcessor_maxMin_ 1

struct validatePilotProcessor_maxMin {
  std::vector < std::string > indx_name;
  std::vector < std::pair < int, int > > indx_minmax;
  std::pair < float, float > eminmax;
  std::pair < float, float > tminmax;
  std::pair < float, float > xminmax;
  std::pair < float, float > yminmax;
  std::pair < float, float > zminmax;
  std::pair < float, float > rminmax;
  std::pair < float, float > abszminmax;
};

#endif
