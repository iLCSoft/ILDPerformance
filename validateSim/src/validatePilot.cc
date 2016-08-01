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

}


void validatePilotProcessor::init() {
  cout << "hello from validatePilotProcessor::init" << endl;


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

      int objtype(0);
      if ( col->getTypeName () == LCIO::SIMCALORIMETERHIT ) {
        if ( _SimCalorimeterHitDecoder ) delete _SimCalorimeterHitDecoder;
        _SimCalorimeterHitDecoder = new CellIDDecoder<SimCalorimeterHit> (col);
        objtype=1;
      } else if ( col->getTypeName () == LCIO::SIMTRACKERHIT ) {
        if ( _SimTrackerHitDecoder ) delete _SimTrackerHitDecoder;
        _SimTrackerHitDecoder = new CellIDDecoder<SimTrackerHit> (col);
        objtype=2;
      } else {
        cout << "unknown hit type! " << col->getTypeName () << endl;
        return;
      }

      float energy(-99);
      float pos[3]={0,0,0};
      std::vector < float > times;
      std::vector < int > indices;

      SimCalorimeterHit* simcalhit=0;
      SimTrackerHit* simtrkhit=0;

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
        default:
          std::cout << "unknown obj type " << objtype << endl;
        }

        if ( allranges.find( colname )==allranges.end() ) {
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

	  allranges[colname]=vmm;

        } else {
          for (size_t j=0; j<indices.size(); j++) {
            if ( indices[j] < allranges[colname].indx_minmax[j].first  ) allranges[colname].indx_minmax[j].first=indices[j];
            if ( indices[j] > allranges[colname].indx_minmax[j].second ) allranges[colname].indx_minmax[j].second=indices[j];
          }
          if ( energy < allranges[colname].eminmax.first  ) allranges[colname].eminmax.first=energy;
          if ( energy > allranges[colname].eminmax.second ) allranges[colname].eminmax.second=energy;
          for (size_t j=0; j<times.size(); j++) {
            if ( times[j] < allranges[colname].tminmax.first  ) allranges[colname].tminmax.first=times[j];
            if ( times[j] > allranges[colname].tminmax.second ) allranges[colname].tminmax.second=times[j];
          }
          if ( pos[0] < allranges[colname].xminmax.first  ) allranges[colname].xminmax.first=pos[0];
          if ( pos[0] > allranges[colname].xminmax.second ) allranges[colname].xminmax.second=pos[0];

          if ( pos[1] < allranges[colname].yminmax.first  ) allranges[colname].yminmax.first=pos[1];
          if ( pos[1] > allranges[colname].yminmax.second ) allranges[colname].yminmax.second=pos[1];

          if ( pos[2] < allranges[colname].zminmax.first  ) allranges[colname].zminmax.first=pos[2];
          if ( pos[2] > allranges[colname].zminmax.second ) allranges[colname].zminmax.second=pos[2];

	  float radius = sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
          float absz = fabs(pos[2]);

	  if ( radius < allranges[colname].rminmax.first  ) allranges[colname].rminmax.first=radius;
	  if ( radius > allranges[colname].rminmax.second ) allranges[colname].rminmax.second=radius;

	  if ( absz < allranges[colname].abszminmax.first  ) allranges[colname].abszminmax.first=absz;
	  if ( absz > allranges[colname].abszminmax.second ) allranges[colname].abszminmax.second=absz;

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

    for ( std::map < std::string , validatePilotProcessor_maxMin >::iterator itt = allranges.begin(); itt!=allranges.end(); itt++) {

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

