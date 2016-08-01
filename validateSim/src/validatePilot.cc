#include "validatePilot.h"
#include <iostream>
#include <map>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>

#include <marlin/Global.h>
#include "lcio.h"

#include "EVENT/LCCollection.h"
#include "EVENT/CalorimeterHit.h"

#include "EVENT/LCParameters.h"

using namespace lcio ;
using namespace marlin ;
using std::cout;
using std::endl;

validatePilotProcessor avalidatePilotProcessor ;


validatePilotProcessor::validatePilotProcessor() : Processor("validationPreProcessor") {

  // modify processor description
  _description = "validatePilotProcessor gets names of simhit collections and ranges of parameters" ;

  std::string paramFileName("BLAH.txt");
  registerProcessorParameter("outputParamFilename",
                             "name of output parameter file",
                             _outfile,
                             paramFileName);

  registerProcessorParameter("simhitCollections",
			     "look at simhit collections?",
			     _simCols, int(1) );

  registerProcessorParameter("recohitCollections",
			     "look at reco hit collections?",
			     _recCols, int(1) );

}


void validatePilotProcessor::init() {
  cout << "hello from validatePilotProcessor::init" << endl;

  _indxCode.clear();
  _allranges.clear();

  return;
}

void validatePilotProcessor::processRunHeader( LCRunHeader* run) {
  cout << "hello from validatePilotProcessor::processRunHeader" << endl;
}

void validatePilotProcessor::processEvent( LCEvent * evt ) {

  const std::vector < std::string >* colnames = evt->getCollectionNames();

  for ( size_t icol=0; icol<colnames->size(); icol++) {

    std::string colname = colnames->at(icol);

    LCCollection* col = evt->getCollection(colname);
    const std::string coltype = col->getTypeName();

    int objtype(0);
    if ( coltype == LCIO::SIMCALORIMETERHIT ) {
      if ( _simCols==0 ) continue;
      if ( _SimCalorimeterHitDecoder ) delete _SimCalorimeterHitDecoder;
      _SimCalorimeterHitDecoder = new CellIDDecoder<SimCalorimeterHit> (col);
      objtype=1;
    } else if ( coltype == LCIO::SIMTRACKERHIT ) {
      if ( _simCols==0 ) continue;
      if ( _SimTrackerHitDecoder ) delete _SimTrackerHitDecoder;
      _SimTrackerHitDecoder = new CellIDDecoder<SimTrackerHit> (col);
      objtype=2;
    } else if ( coltype == LCIO::CALORIMETERHIT ) {
      if ( _recCols==0 ) continue;
      if ( _CalorimeterHitDecoder ) delete _CalorimeterHitDecoder;
      _CalorimeterHitDecoder = new CellIDDecoder<CalorimeterHit> (col);
      objtype=3;
    } else if ( coltype == LCIO::TRACKERHIT ) {
      if ( _recCols==0 ) continue;
      if ( _TrackerHitDecoder ) delete _TrackerHitDecoder;
      _TrackerHitDecoder = new CellIDDecoder<TrackerHit> (col);
      objtype=4;
    } else {
      //        cout << "unknown hit type! " << coltype << endl;
      continue;
    }


    cout << colname << endl;

    std::vector < std::pair < std::string, std::pair<int, int> > > thiscode;
    if ( _indxCode.find( colname ) != _indxCode.end() ) {
      thiscode=_indxCode[colname];
    } else {// not already found this collection in prev event
      // get the cell id encoding indices for this collection
      StringVec stringVec ;
      col->getParameters().getStringKeys(stringVec);
      if ( std::find( stringVec.begin(), stringVec.end(), LCIO::CellIDEncoding ) != stringVec.end() ) {
        stringVec.clear();
        col->getParameters().getStringVals( LCIO::CellIDEncoding , stringVec ) ;
        std::istringstream f(stringVec[0]);
        std::string s,ss;
        std::vector<std::string> strings;
        while (getline(f, s, ',')) {
          std::istringstream ff(s);
          getline(ff, s, ':');
          getline(ff, ss, ':');
          getline(ff, ss, ':');
          int nbit = std::atoi(ss.c_str());
          std::pair<int,int> pp;
          if ( nbit>0 ) {
            pp.first=0;
            pp.second=pow(2, nbit);
          } else { // signed value
            pp.first  = - ( pow(2, abs(nbit))/2 + 1 );
            pp.second = - pp.first;
          }
          std::pair < std::string, std::pair<int, int> > II;
          II.first = s;
          II.second = pp;
          thiscode.push_back(II);
        }
	_indxCode[colname] = thiscode;
      }
    }

    if ( col->getNumberOfElements()>0 && _indxCode.find( colname ) != _indxCode.end() )  { // this collection has cellID information attached

      float energy(-99);
      float pos[3]={0,0,0};
      std::vector < float > times;
      std::vector < int > indices;

      SimCalorimeterHit* simcalhit=0;
      SimTrackerHit* simtrkhit=0;
      CalorimeterHit* calhit=0;
      TrackerHit* trkhit=0;

      for (int j=0; j<col->getNumberOfElements(); j++) {

        energy=-999;
        for (int i=0; i<3; i++) pos[i]=0;
        times.clear();
        indices.clear();

        switch ( objtype ) {
        case 1: // simcalohit

          simcalhit = dynamic_cast<SimCalorimeterHit*> (col->getElementAt(j));
          energy=simcalhit->getEnergy();
          for (int i=0; i<3; i++)
            pos[i] = simcalhit->getPosition()[i];
          for (int i=0; i<simcalhit->getNMCContributions (); i++)
            times.push_back( simcalhit->getTimeCont (i) );

          for (size_t ii=0; ii<thiscode.size(); ii++) {
            int index = (*_SimCalorimeterHitDecoder)( simcalhit ) [ thiscode[ii].first ];
            indices.push_back( index );
          }
          break;
        case 2: // simtrackerhit
          simtrkhit = dynamic_cast<SimTrackerHit*> (col->getElementAt(j));
          energy=simtrkhit->getEDep();
          for (int i=0; i<3; i++)
            pos[i] = simtrkhit->getPosition()[i];
          times.push_back( simtrkhit->getTime() );

          for (size_t ii=0; ii<thiscode.size(); ii++) {
            int index = (*_SimTrackerHitDecoder)( simtrkhit ) [ thiscode[ii].first ];
            indices.push_back( index );
          }
          break;
        case 3: // calorimeterhit
          calhit = dynamic_cast<CalorimeterHit*> (col->getElementAt(j));
          energy=calhit->getEnergy();
          for (int i=0; i<3; i++)
            pos[i] = calhit->getPosition()[i];

	  times.push_back( calhit->getTime() );

          for (size_t ii=0; ii<thiscode.size(); ii++) {
            int index = (*_CalorimeterHitDecoder)( calhit ) [ thiscode[ii].first ];
            indices.push_back( index );
          }
          break;
        case 4: // trackerhit
          trkhit = dynamic_cast<TrackerHit*> (col->getElementAt(j));
          energy=trkhit->getEDep();
          for (int i=0; i<3; i++)
            pos[i] = trkhit->getPosition()[i];
          times.push_back( trkhit->getTime() );

          for (size_t ii=0; ii<thiscode.size(); ii++) {
            int index = (*_TrackerHitDecoder)( trkhit ) [ thiscode[ii].first ];
            indices.push_back( index );
          }
          break;
        default:
          std::cout << "unknown obj type " << objtype << endl;
        }

        if ( _allranges.find( colname )==_allranges.end() ) {
          validatePilotProcessor_maxMin vmm;
          for (size_t j=0; j<indices.size(); j++) {
	    vmm.indx_name.push_back( thiscode[j].first );
            vmm.indx_minmax.push_back( std::pair < int, int > ( indices[j], indices[j] ) );
	  }
          vmm.eminmax = std::pair < float, float > ( energy, energy );
          vmm.tminmax = std::pair < float, float > ( 0,0 );
          for (size_t j=0; j<times.size(); j++) {
            if ( times[j] < vmm.tminmax.first ) vmm.tminmax.first=times[j];
            if ( times[j] > vmm.tminmax.second ) vmm.tminmax.second=times[j];
          }
          vmm.xminmax = std::pair < float, float > (pos[0], pos[0]);
          vmm.yminmax = std::pair < float, float > (pos[1], pos[1]);
          vmm.zminmax = std::pair < float, float > (pos[2], pos[2]);

	  float radius = sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
	  float absz = fabs(pos[2]);
	  
	  vmm.rminmax = std::pair < float, float > (radius, radius);
	  vmm.abszminmax = std::pair < float, float > (absz, absz);

	  _allranges[colname]=vmm;

        } else {
          for (size_t j=0; j<indices.size(); j++) {
            if ( indices[j] < _allranges[colname].indx_minmax[j].first  ) _allranges[colname].indx_minmax[j].first=indices[j];
            if ( indices[j] > _allranges[colname].indx_minmax[j].second ) _allranges[colname].indx_minmax[j].second=indices[j];
          }
          if ( energy < _allranges[colname].eminmax.first  ) _allranges[colname].eminmax.first=energy;
          if ( energy > _allranges[colname].eminmax.second ) _allranges[colname].eminmax.second=energy;
          for (size_t j=0; j<times.size(); j++) {
            if ( times[j] < _allranges[colname].tminmax.first  ) _allranges[colname].tminmax.first=times[j];
            if ( times[j] > _allranges[colname].tminmax.second ) _allranges[colname].tminmax.second=times[j];
          }
          if ( pos[0] < _allranges[colname].xminmax.first  ) _allranges[colname].xminmax.first=pos[0];
          if ( pos[0] > _allranges[colname].xminmax.second ) _allranges[colname].xminmax.second=pos[0];

          if ( pos[1] < _allranges[colname].yminmax.first  ) _allranges[colname].yminmax.first=pos[1];
          if ( pos[1] > _allranges[colname].yminmax.second ) _allranges[colname].yminmax.second=pos[1];

          if ( pos[2] < _allranges[colname].zminmax.first  ) _allranges[colname].zminmax.first=pos[2];
          if ( pos[2] > _allranges[colname].zminmax.second ) _allranges[colname].zminmax.second=pos[2];

	  float radius = sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
          float absz = fabs(pos[2]);

	  if ( radius < _allranges[colname].rminmax.first  ) _allranges[colname].rminmax.first=radius;
	  if ( radius > _allranges[colname].rminmax.second ) _allranges[colname].rminmax.second=radius;

	  if ( absz < _allranges[colname].abszminmax.first  ) _allranges[colname].abszminmax.first=absz;
	  if ( absz > _allranges[colname].abszminmax.second ) _allranges[colname].abszminmax.second=absz;

        }




      } // elements


    } // coll with cellindx

  } // collections


  return;
}



void validatePilotProcessor::check( LCEvent * evt ) {
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}

void validatePilotProcessor::end(){
  std::cout << "validatePilotProcessor::end()  " << name()
            << std::endl ;


  std::string label;

  ofstream myfile (_outfile.c_str());
  if (myfile.is_open()) {

    for ( std::map < std::string , validatePilotProcessor_maxMin >::iterator itt = _allranges.begin(); itt!=_allranges.end(); itt++) {

      validatePilotProcessor_maxMin vmm = itt->second;

      for (size_t i=0; i<vmm.indx_minmax.size(); i++) {
	label = itt->first + " Index_" + vmm.indx_name[i];  myfile << label << " " << vmm.indx_minmax[i].first << " " << vmm.indx_minmax[i].second << endl;
      }

      label = itt->first + " Energy"; myfile << label << " " << vmm.eminmax.first << " " << vmm.eminmax.second << endl;
      label = itt->first + " Time";   myfile << label << " " << vmm.tminmax.first << " " << vmm.tminmax.second << endl;
      label = itt->first + " X";      myfile << label << " " << vmm.xminmax.first << " " << vmm.xminmax.second << endl;
      label = itt->first + " Y";      myfile << label << " " << vmm.yminmax.first << " " << vmm.yminmax.second << endl;
      label = itt->first + " Z";      myfile << label << " " << vmm.zminmax.first << " " << vmm.zminmax.second << endl;
      label = itt->first + " Absz";   myfile << label << " " << vmm.abszminmax.first << " " << vmm.abszminmax.second << endl;
      label = itt->first + " R";      myfile << label << " " << vmm.rminmax.first << " " << vmm.rminmax.second << endl;

    }

    myfile.close();

  } else cout << "Unable to open file" << endl;

}

