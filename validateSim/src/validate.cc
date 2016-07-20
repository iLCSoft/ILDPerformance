#include "validate.h"
#include <iostream>
#include <map>
#include <sstream>

#include <marlin/Global.h>
#include "lcio.h"

#include "EVENT/LCCollection.h"
#include "EVENT/CalorimeterHit.h"

#include "EVENT/LCParameters.h"

using namespace lcio ;
using namespace marlin ;
using std::cout;
using std::endl;

validateProcessor avalidateProcessor ;


validateProcessor::validateProcessor() : Processor("validationProcessor") {
  
  // modify processor description
  _description = "validateProcessor makes histograms of basic simhit properties to quickly check simulation results" ;

  std::string incol("EcalBarrelCollection");
  registerProcessorParameter("inputCollectionName",
			     "name of input collection",
			     _inColl,
			     incol);

  registerProcessorParameter("eMax","maximum energy", _maxE, float(0.01) );
  registerProcessorParameter("rMax","maximum dimension in R dir", _maxR, float(2000) );
  registerProcessorParameter("zMax","maximum dimension in Z dir", _maxZ, float(2000) );
  registerProcessorParameter("zMin","minimum dimension in Z dir (for endcaps)", _minZ, float(0) );

  registerProcessorParameter("inEndcap","is this in the endcap?", _isEndcap, bool (false) );

}


void validateProcessor::init() { 
  cout << "hello from validateProcessor::init" << endl;

  _SimCalorimeterHitDecoder=0;


  std::string outfilename = _inColl;
  outfilename+=".root";
  _fout = new TFile(outfilename.c_str(),"recreate");

  _hSim_HitEn = new TH1F("Sim_HitEn", "Sim_HitEn", 1000,0,_maxE);

  _hSim_HitTime = new TH1F("Sim_HitTime","Sim_HitTime",1000,-10,100);

  int nbins=300;

  for (int i=0; i<2; i++) {

    TString endlab("");
    float zmin(-_maxZ);
    float zmax( _maxZ);
    if (_isEndcap) {
      if ( i==0 ) zmax=-_minZ;
      else        zmin= _minZ;
      endlab = "_end"; endlab+=i;
    } else {
      if (i==1) continue;
    }

    _hSim_posXY[i] = new TH2F("Sim_posXY"+endlab,"Sim_posXY"+endlab, nbins, -_maxR, _maxR, nbins, -_maxR, _maxR);
    _hSim_posXZ[i] = new TH2F("Sim_posXZ"+endlab,"Sim_posXZ"+endlab, nbins, zmin, zmax, nbins, -_maxR, _maxR);
    _hSim_posYZ[i] = new TH2F("Sim_posYZ"+endlab,"Sim_posYZ"+endlab, nbins, zmin, zmax, nbins, -_maxR, _maxR);
    _hSim_posRZ[i] = new TH2F("Sim_posRZ"+endlab,"Sim_posRZ"+endlab, nbins, zmin, zmax, nbins, 0, _maxR);
  }

  return;
}

void validateProcessor::processRunHeader( LCRunHeader* run) { 
  cout << "hello from validateProcessor::processRunHeader" << endl;
}

void validateProcessor::processEvent( LCEvent * evt ) { 

  _fout->cd();

  try {
    LCCollection* simhitcol = evt->getCollection(_inColl);

    //    cout << "collection " << _inColl << " type = " << simhitcol->getTypeName () << endl;

    // get the cell id encoding indices
    _indxCode.clear();
    StringVec stringVec ;
    simhitcol->getParameters().getStringVals( LCIO::CellIDEncoding , stringVec ) ;

    //    cout << stringVec[0] << endl;

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

      //      cout << s << " " << pp.first << " " << pp.second << endl;

      _indxCode[s] = pp;
    }

    bool firstEvt = _hSim_posX_index[0].size()==0;

    int nfbins=300;

    for (  std::map < std::string, std::pair<int, int> >::iterator itt = _indxCode.begin(); itt!=_indxCode.end(); itt++) {
      // for first event, set up the histogarms
      if ( firstEvt ) {

	int nbins = itt->second.second-itt->second.first;
	if ( nbins>300 ) nbins=300;
	int indmin = itt->second.first;
	int indmax = itt->second.second;

	if ( itt->first=="x" || itt->first=="y" ) {
	  nbins=3000;
	  indmin=-3000;
	  indmax= 3000;
	}

	std::string hn;


	for ( int ie=0; ie<2; ie++) {

	  if ( !_isEndcap && ie==1 ) continue;

	  float zmin(-_maxZ);
	  float zmax( _maxZ);
	  if (_isEndcap) {
	    if ( ie==0 ) zmax=-_minZ;
	    else         zmin= _minZ;
	  }

	  std::string ee = "";
	  if (_isEndcap) ee = ie==0 ? "_end0" : "_end1";

	  //	  cout << hn << " " << itt->first << " isendcap: " << _isEndcap << ee << endl;

	  hn = "Sim_posX_";   hn+=itt->first+ee; _hSim_posX_index[ie].push_back( new TH2F( hn.c_str(), hn.c_str(), nbins, indmin, indmax, nfbins, -_maxR, _maxR) );
	  hn = "Sim_posY_";   hn+=itt->first+ee; _hSim_posY_index[ie].push_back( new TH2F( hn.c_str(), hn.c_str(), nbins, indmin, indmax, nfbins, -_maxR, _maxR) );
	  hn = "Sim_posZ_";   hn+=itt->first+ee; _hSim_posZ_index[ie].push_back( new TH2F( hn.c_str(), hn.c_str(), nbins, indmin, indmax, nfbins, zmin, zmax ) );
	  hn = "Sim_posR_";   hn+=itt->first+ee; _hSim_posR_index[ie].push_back( new TH2F( hn.c_str(), hn.c_str(), nbins, indmin, indmax, nfbins, 0, _maxR) );
	  hn = "Sim_posPhi_"; hn+=itt->first+ee; _hSim_posPhi_index[ie].push_back( new TH2F( hn.c_str(), hn.c_str(), nbins, indmin, indmax, nfbins, -acos(-1), acos(-1)) );

	}
      }
    }
    assert( _hSim_posX_index[0].size() == _indxCode.size() );


    int objtype(0);

    if ( simhitcol->getTypeName () == LCIO::SIMCALORIMETERHIT ) {
      if ( _SimCalorimeterHitDecoder ) delete _SimCalorimeterHitDecoder;
      _SimCalorimeterHitDecoder = new CellIDDecoder<SimCalorimeterHit> (simhitcol);
      objtype=1;
    } else if ( simhitcol->getTypeName () == LCIO::SIMTRACKERHIT ) {
      if ( _SimTrackerHitDecoder ) delete _SimTrackerHitDecoder;
      _SimTrackerHitDecoder = new CellIDDecoder<SimTrackerHit> (simhitcol);
      objtype=2;
    } else {
      cout << "unknown hit type! " << simhitcol->getTypeName () << endl;
    }


    for (int j=0; j<simhitcol->getNumberOfElements(); j++) {

      float energy(-99);
      float pos[3]={0,0,0};
      std::vector < float > times;
      std::vector < int > indices;

      SimCalorimeterHit* simcalhit;
      SimTrackerHit* simtrkhit;

      switch ( objtype ) {

      case 1: // SimCalorimeterHit

	simcalhit = dynamic_cast<SimCalorimeterHit*> (simhitcol->getElementAt(j));
	energy=simcalhit->getEnergy();

	for (int i=0; i<3; i++)	
	  pos[i] = simcalhit->getPosition()[i];

	for (int i=0; i<simcalhit->getNMCContributions (); i++) 
	  times.push_back( simcalhit->getTimeCont (i) );

	for (  std::map < std::string, std::pair<int, int> >::iterator itt = _indxCode.begin(); itt!=_indxCode.end(); itt++) {
	  int ii = (*_SimCalorimeterHitDecoder)( simcalhit ) [ itt->first ];
	  indices.push_back( ii );
	}

	break;

      case 2: // SimTrackerHit
	
	simtrkhit = dynamic_cast<SimTrackerHit*> (simhitcol->getElementAt(j));
        energy=simtrkhit->getEDep();

	for (int i=0; i<3; i++)
          pos[i] = simtrkhit->getPosition()[i];

	times.push_back( simtrkhit->getTime() );

	for (  std::map < std::string, std::pair<int, int> >::iterator itt = _indxCode.begin(); itt!=_indxCode.end(); itt++) {
          int ii = (*_SimTrackerHitDecoder)( simtrkhit ) [ itt->first ];
          indices.push_back( ii );
        }

        break;


      default:
	cout << "unknown type! " << objtype << endl;
	assert(0);
	break;

      }

      // fill histograms
      _hSim_HitEn->Fill(energy);

      float r = sqrt( pow(pos[0],2)+pow(pos[1],2) );

      int ie(0);
      if (_isEndcap && pos[2]>0 ) ie=1;
      _hSim_posXY[ie]->Fill(pos[0], pos[1]);
      _hSim_posXZ[ie]->Fill(pos[2], pos[0]);
      _hSim_posYZ[ie]->Fill(pos[2], pos[1]);
      _hSim_posRZ[ie]->Fill(pos[2], r);

      for (size_t i=0; i<times.size(); i++)
	_hSim_HitTime->Fill( times[i] );

      for (size_t i=0; i<indices.size(); i++) {
	_hSim_posX_index[ie][i]->Fill( indices[i], pos[0] );
	_hSim_posY_index[ie][i]->Fill( indices[i], pos[1] );
	_hSim_posZ_index[ie][i]->Fill( indices[i], pos[2] );
	_hSim_posR_index[ie][i]->Fill( indices[i], r );
	_hSim_posPhi_index[ie][i]->Fill( indices[i], atan2( pos[1], pos[0]) );

      }

      fillSpecificHistos();

    }

  }
  catch(DataNotAvailableException &e) {};


  return;
}



void validateProcessor::check( LCEvent * evt ) { 
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}

void validateProcessor::end(){
  std::cout << "validateProcessor::end()  " << name() 
 	    << std::endl ;

  _fout->Write(0);
  _fout->Close();

}

