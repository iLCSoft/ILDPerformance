#include "validateAuto.h"
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>

#include <marlin/Global.h>
#include "lcio.h"

#include "EVENT/LCCollection.h"
#include "EVENT/CalorimeterHit.h"

#include "EVENT/LCParameters.h"

#include "TObjArray.h"
#include "TObjString.h"

using namespace lcio ;
using namespace marlin ;
using std::cout;
using std::endl;

validateAutoProcessor avalidateAutoProcessor ;


validateAutoProcessor::validateAutoProcessor() : Processor("validateAutoProcessor") {

  // modify processor description
  _description = "validateAutoProcessor makes histograms of basic simhit properties to quickly check simulation results" ;

  std::string paramFileName("BLAH.txt");
  registerProcessorParameter("inputParamFilename",
                             "name of input parameter file",
                             _infile,
                             paramFileName);

}


void validateAutoProcessor::init() {
  cout << "hello from validateAutoProcessor::init" << endl;

  _SimCalorimeterHitDecoder=0;
  _SimTrackerHitDecoder=0;

  // read in the collections, indices, and ranges
  cout << _infile << endl;
  std::string line;
  ifstream myfile (_infile.c_str());
  if (myfile.is_open()) {
    while ( getline (myfile,line) ) {
      TString ll(line);
      TObjArray* tor = ll.Tokenize(" ");
      assert( tor->GetEntries() == 4 );
      std::string colname = ( (TObjString*) tor->At(0) )->String().Data();
      bool isindex=false;
      TString tsvarname = ( (TObjString*) tor->At(1) )->String();
      if ( tsvarname.Contains("Index_") ) {
        isindex=true;
        TObjArray* tb = tsvarname.Tokenize("_");
        tsvarname = ( (TObjString*) tb->At(1) )->String().Data();
      }
      std::string varname = tsvarname.Data();
      float minval = ( (TObjString*) tor->At(2) )->String().Atof();
      float maxval = ( (TObjString*) tor->At(3) )->String().Atof();
      if ( allranges.find( colname ) == allranges.end() ) {
        validatePilotProcessor_maxMin ss;
        allranges[colname] = ss;
      }
      if ( isindex ) {
        allranges[colname].indx_name.push_back(varname);
        allranges[colname].indx_minmax.push_back( std::pair < int, int > ( int(minval), int(maxval) ) );
      } else if ( varname=="Energy" ) {
        allranges[colname].eminmax =  std::pair < float, float > (minval, maxval);
      } else if ( varname=="Time" ) {
        allranges[colname].tminmax =  std::pair < float, float > (minval, maxval);
      } else if ( varname=="X" ) {
        allranges[colname].xminmax =  std::pair < float, float > (minval, maxval);
      } else if ( varname=="Y" ) {
        allranges[colname].yminmax =  std::pair < float, float > (minval, maxval);
      } else if ( varname=="Z" ) {
        allranges[colname].zminmax =  std::pair < float, float > (minval, maxval);
      } else if ( varname=="Absz" ) {
        allranges[colname].abszminmax =  std::pair < float, float > (minval, maxval);
      } else if ( varname=="R" ) {
        allranges[colname].rminmax =  std::pair < float, float > (minval, maxval);
      } else {
        cout << "unknown variable? " << varname << endl;
        assert(0);
      }
      //  std::map < std::string , validatePilotProcessor_maxMin > allranges;
    }
    myfile.close();
  } else {
    cout << "Unable to open file " << _infile << endl;
    assert(0);
  }

  // variables to plot indices against
//  std::vector < std::string > index_variables;
//  index_variables.push_back("X");
//  index_variables.push_back("Y");
//  index_variables.push_back("Z");
//  index_variables.push_back("R");
//  index_variables.push_back("Phi");


  // the output root file for histograms
  std::string outfilename = "autoValidate.root";
  _fout = new TFile(outfilename.c_str(),"recreate");

  const int nbins1d=300;
  const int nbins2d=100;


  float zmaxall(0);
  float rmaxall(0);







  // make a directory for each collection
  for ( std::map < std::string , validatePilotProcessor_maxMin >::iterator itt=allranges.begin(); itt!=allranges.end(); itt++) {

    zmaxall = std::max( rmaxall, itt->second.zminmax.second);
    rmaxall = std::max( rmaxall, itt->second.rminmax.second);


    _fout->mkdir( itt->first.c_str() );
    _fout->cd( itt->first.c_str() );

    TString hname, hname1, hname2;
    float hmin, hmax, diff;
    float hmin2, hmax2, diff2;

    hname = itt->first.c_str(); hname += "_hitEn";
    hmin = itt->second.eminmax.first;
    hmax = itt->second.eminmax.second;
    diff = hmax-hmin;
    hmin-=diff/20.;
    hmax+=diff/20.;
    _hSim_HitEn[ itt->first ] = new TH1F(hname, hname,nbins1d,hmin,hmax);

    hname = itt->first.c_str(); hname += "_hitTime";
    hmin = itt->second.tminmax.first;
    hmax = itt->second.tminmax.second;
    if ( hmax > 100 ) hmax = 100;
    diff = hmax-hmin;    hmin-=diff/20.;    hmax+=diff/20.;
    _hSim_HitTime[ itt->first ] = new TH1F(hname, hname,nbins1d,hmin,hmax);


    bool isEndcap = itt->second.abszminmax.first > itt->second.zminmax.second/15.;

    cout << itt->first << " is endcap ? " << itt->second.abszminmax.first << " " << itt->second.zminmax.second << " " << itt->second.zminmax.second/15. << " " << isEndcap << endl;

    TH2F* h1 = 0;
    TH2F* h2 = 0;

    hname = itt->first.c_str(); hname += "_hitXY";
    hname1 = isEndcap ? hname + "_posZ" : hname;
    hmin = itt->second.xminmax.first;
    hmax = itt->second.xminmax.second;
    diff = hmax-hmin;    hmin-=diff/20.;    hmax+=diff/20.;
    hmin2 = itt->second.yminmax.first;
    hmax2 = itt->second.yminmax.second;
    diff2 = hmax2-hmin2; hmin2-=diff2/20.;    hmax2+=diff2/20.;
    h1 = new TH2F(hname1, hname1,nbins2d,hmin,hmax, nbins2d, hmin2, hmax2);
    if ( isEndcap ) {
      hname2 = hname + "_negZ";
      h2 = new TH2F(hname2, hname2,nbins2d,hmin,hmax, nbins2d, hmin2, hmax2);
    }
    _hSim_posXY[ itt->first ] = std::pair < TH2F* , TH2F* > (h1, h2);

    hname = itt->first.c_str(); hname += "_hitZR";
    hname1 = isEndcap ? hname + "_posZ" : hname;

    hmin = isEndcap ? itt->second.abszminmax.first : itt->second.zminmax.first;
    hmax = itt->second.zminmax.second;
    diff = hmax-hmin;    hmin-=diff/20.;    hmax+=diff/20.;

    hmin2 = itt->second.rminmax.first;
    hmax2 = itt->second.rminmax.second;
    diff2 = hmax2-hmin2; hmin2-=diff2/20.;    hmax2+=diff2/20.;
    h1 = new TH2F(hname1, hname1,nbins2d,hmin,hmax, nbins2d, hmin2, hmax2);
    if ( isEndcap ) {
      hname2 = hname + "_negZ";
      h2 = new TH2F(hname2, hname2,nbins2d,-hmax,-hmin, nbins2d, hmin2, hmax2);
    }
    _hSim_posZR[ itt->first ] = std::pair < TH2F* , TH2F* > (h1, h2);


//    std::vector < std::string > index_variables;
//    index_variables.push_back("X");
//    index_variables.push_back("Y");
//    index_variables.push_back("Z");
//    index_variables.push_back("R");
//    index_variables.push_back("Phi");

    std::vector < std::pair < TH2F* , TH2F* > > aa;
    int nbins(1);

    _hSim_index_posX[ itt->first ] = aa;
    for (size_t jj=0; jj<itt->second.indx_name.size(); jj++) {
      hname = itt->first.c_str(); hname += "_Indx"+itt->second.indx_name[jj]+"_X";
      hname1 = isEndcap ? hname + "_posZ" : hname;

      hmin = itt->second.indx_minmax[jj].first-1;
      hmax = itt->second.indx_minmax[jj].second+2;
      nbins = hmax-hmin; if (nbins>nbins2d) nbins=nbins2d;
      
      hmin2 = itt->second.xminmax.first;
      hmax2 = itt->second.xminmax.second;
      h1 = new TH2F(hname1, hname1,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      if ( isEndcap ) {
	hname2 = hname + "_negZ";
	h2 = new TH2F(hname2, hname2,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      }
      _hSim_index_posX[ itt->first ].push_back( std::pair < TH2F* , TH2F* > (h1, h2) );
    }

    for (size_t jj=0; jj<itt->second.indx_name.size(); jj++) {
      hname = itt->first.c_str(); hname += "_Indx"+itt->second.indx_name[jj]+"_Y";
      hname1 = isEndcap ? hname + "_posZ" : hname;
      hmin = itt->second.indx_minmax[jj].first-1;
      hmax = itt->second.indx_minmax[jj].second+2;
      nbins = hmax-hmin; if (nbins>nbins2d) nbins=nbins2d;

      hmin2 = itt->second.yminmax.first;
      hmax2 = itt->second.yminmax.second;
      h1 = new TH2F(hname1, hname1,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      if ( isEndcap ) {
	hname2 = hname + "_negZ";
	h2 = new TH2F(hname2, hname2,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      }
      _hSim_index_posY[ itt->first ].push_back( std::pair < TH2F* , TH2F* > (h1, h2) );
    }

    for (size_t jj=0; jj<itt->second.indx_name.size(); jj++) {
      hname = itt->first.c_str(); hname += "_Indx"+itt->second.indx_name[jj]+"_Z";
      hname1 = isEndcap ? hname + "_posZ" : hname;
      hmin = itt->second.indx_minmax[jj].first-1;
      hmax = itt->second.indx_minmax[jj].second+2;
      nbins = hmax-hmin; if (nbins>nbins2d) nbins=nbins2d;
      if ( isEndcap ) {
	hmin2 = itt->second.abszminmax.first;
	hmax2 = itt->second.abszminmax.second;
      } else {
	hmin2 = itt->second.zminmax.first;
	hmax2 = itt->second.zminmax.second;
      }
      h1 = new TH2F(hname1, hname1,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      if ( isEndcap ) {
	hname2 = hname + "_negZ";
	h2 = new TH2F(hname2, hname2,nbins,hmin,hmax, nbins2d, -hmax2, -hmin2);
      }
      _hSim_index_posZ[ itt->first ].push_back( std::pair < TH2F* , TH2F* > (h1, h2) );
    }

    for (size_t jj=0; jj<itt->second.indx_name.size(); jj++) {
      hname = itt->first.c_str(); hname += "_Indx"+itt->second.indx_name[jj]+"_R";
      hname1 = isEndcap ? hname + "_posZ" : hname;
      hmin = itt->second.indx_minmax[jj].first-1;
      hmax = itt->second.indx_minmax[jj].second+2;
      nbins = hmax-hmin; if (nbins>nbins2d) nbins=nbins2d;
      hmin2 = itt->second.rminmax.first;
      hmax2 = itt->second.rminmax.second;
      h1 = new TH2F(hname1, hname1,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      if ( isEndcap ) {
	hname2 = hname + "_negZ";
	h2 = new TH2F(hname2, hname2,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      }
      _hSim_index_posR[ itt->first ].push_back( std::pair < TH2F* , TH2F* > (h1, h2) );
    }

    for (size_t jj=0; jj<itt->second.indx_name.size(); jj++) {
      hname = itt->first.c_str(); hname += "_Indx"+itt->second.indx_name[jj]+"_Phi";
      hname1 = isEndcap ? hname + "_posZ" : hname;
      hmin = itt->second.indx_minmax[jj].first-1;
      hmax = itt->second.indx_minmax[jj].second+2;
      nbins = hmax-hmin; if (nbins>nbins2d) nbins=nbins2d;
      hmin2 = -1.1*acos(-1);
      hmax2 = 1.1*acos(-1);
      h1 = new TH2F(hname1, hname1,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      if ( isEndcap ) {
	hname2 = hname + "_negZ";
	h2 = new TH2F(hname2, hname2,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      }
      _hSim_index_posPhi[ itt->first ].push_back( std::pair < TH2F* , TH2F* > (h1, h2) );
    }

  }



  // overall view

  _fout->mkdir( "ALLCollections" );
  _fout->cd( "ALLCollections" );

  TString hname1 = "ALLCollections_overallhitZR";
  _hAll_overallposZR = new TH2F(hname1, hname1,2*nbins2d,-zmaxall*1.1,zmaxall*1.1, 2*nbins2d, 0, rmaxall*1.1);
  hname1 = "ALLCollections_Log_overallhitZR";
  _hAll_Log_overallposZR = new TH2F(hname1, hname1,2*nbins2d,-1,log10(zmaxall)*1.1, 2*nbins2d, 0.5, log10(rmaxall)*1.1);

  for ( std::map < std::string , validatePilotProcessor_maxMin >::iterator itt=allranges.begin(); itt!=allranges.end(); itt++) {
    hname1 = itt->first + "_overallhitZR";
    _h_overallposZR[itt->first] = new TH2F(hname1, hname1,2*nbins2d,-zmaxall*1.1,zmaxall*1.1, 2*nbins2d, 0, rmaxall*1.1);
    hname1 = itt->first + "_Log_overallhitZR";
    _h_Log_overallposZR[itt->first] = new TH2F(hname1, hname1,2*nbins2d,-1,log10(zmaxall)*1.1, 2*nbins2d, 0.5, log10(rmaxall)*1.1);
  }




  //
  //  _hSim_HitEn = new TH1F("Sim_HitEn", "Sim_HitEn", 1000,0,_maxE);
  //
  //  _hSim_HitTime = new TH1F("Sim_HitTime","Sim_HitTime",1000,-10,100);
  //
  //  int nbins=300;
  //
  //  for (int i=0; i<2; i++) {
  //
  //    TString endlab("");
  //    float zmin(-_maxZ);
  //    float zmax( _maxZ);
  //    if (_isEndcap) {
  //      if ( i==0 ) zmax=-_minZ;
  //      else        zmin= _minZ;
  //      endlab = "_end"; endlab+=i;
  //    } else {
  //      if (i==1) continue;
  //    }
  //
  //    _hSim_posXY[i] = new TH2F("Sim_posXY"+endlab,"Sim_posXY"+endlab, nbins, -_maxR, _maxR, nbins, -_maxR, _maxR);
  //    _hSim_posXZ[i] = new TH2F("Sim_posXZ"+endlab,"Sim_posXZ"+endlab, nbins, zmin, zmax, nbins, -_maxR, _maxR);
  //    _hSim_posYZ[i] = new TH2F("Sim_posYZ"+endlab,"Sim_posYZ"+endlab, nbins, zmin, zmax, nbins, -_maxR, _maxR);
  //    _hSim_posRZ[i] = new TH2F("Sim_posRZ"+endlab,"Sim_posRZ"+endlab, nbins, zmin, zmax, nbins, 0, _maxR);
  //  }

  return;
}

void validateAutoProcessor::processRunHeader( LCRunHeader* run) {
  cout << "hello from validateAutoProcessor::processRunHeader" << endl;
}

void validateAutoProcessor::processEvent( LCEvent * evt ) {


  for ( std::map < std::string , validatePilotProcessor_maxMin >::iterator itt=allranges.begin(); itt!=allranges.end(); itt++) {

    std::string colname = itt->first;

    try {
      LCCollection* simhitcol = evt->getCollection( colname );

      //std::vector < std::string > cellindices;
      cellindices.clear();
      for (size_t jj=0; jj<allranges[colname].indx_name.size(); jj++) {
	cellindices.push_back( allranges[colname].indx_name[jj] );
      }


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
        assert(0);
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


	  for ( size_t kk=0; kk<cellindices.size(); kk++) {
            int ii = (*_SimCalorimeterHitDecoder)( simcalhit ) [ cellindices[kk] ];
            indices.push_back( ii );
	  }

          break;

        case 2: // SimTrackerHit

          simtrkhit = dynamic_cast<SimTrackerHit*> (simhitcol->getElementAt(j));
          energy=simtrkhit->getEDep();

          for (int i=0; i<3; i++)
            pos[i] = simtrkhit->getPosition()[i];

          times.push_back( simtrkhit->getTime() );

	  for ( size_t kk=0; kk<cellindices.size(); kk++) {
            int ii = (*_SimTrackerHitDecoder)( simtrkhit ) [ cellindices[kk] ];
            indices.push_back( ii );
	  }

          break;


        default:
          cout << "unknown type! " << objtype << endl;
          assert(0);
          break;

        }

        // fill histograms
        _hSim_HitEn[colname]->Fill(energy);

	for (size_t i=0; i<times.size(); i++)
	  _hSim_HitTime[colname]->Fill( times[i] );

	bool isBarrel = _hSim_posXY[ colname ].second==0;

	if ( isBarrel ) {
	  _hSim_posXY[ colname ].first->Fill(pos[0], pos[1] );
	} else {
	  if ( pos[2]>0 ) {
	    _hSim_posXY[ colname ].first->Fill(pos[0], pos[1] );
	  } else {	    
	    _hSim_posXY[ colname ].second->Fill(pos[0], pos[1] );
	  }
	}

	float r = sqrt( pow(pos[0],2)+pow(pos[1],2) );
	if ( isBarrel ) {
	  _hSim_posZR[ colname ].first->Fill(pos[2], r );
	} else {
	  if ( pos[2]>0 ) {
	    _hSim_posZR[ colname ].first->Fill(pos[2], r );
	  } else {	    
	    _hSim_posZR[ colname ].second->Fill(pos[2], r );
	  }
	}

	_hAll_overallposZR->Fill(pos[2], r );

	_h_overallposZR[itt->first]->Fill(pos[2], r );
	

	_hAll_Log_overallposZR->Fill(log10(fabs(pos[2])), log10(fabs(r)) );

	_h_Log_overallposZR[itt->first]->Fill(log10(fabs(pos[2])), log10(fabs(r)) );

	float rad = sqrt( pos[0]*pos[0] + pos[1]*pos[1] );
	float phi = atan2( pos[1], pos[0] );

	for ( size_t kk=0; kk<cellindices.size(); kk++) {

	  if ( isBarrel ) {
	    _hSim_index_posX[colname][kk].first->Fill( indices[kk], pos[0] );
	    _hSim_index_posY[colname][kk].first->Fill( indices[kk], pos[1] );
	    _hSim_index_posZ[colname][kk].first->Fill( indices[kk], pos[2] );
	    _hSim_index_posR[colname][kk].first->Fill( indices[kk], rad );
	    _hSim_index_posPhi[colname][kk].first->Fill( indices[kk], phi );
	  } else {
	    if ( pos[2] > 0 ) {
	      _hSim_index_posX[colname][kk].first ->Fill( indices[kk], pos[0] );
	      _hSim_index_posY[colname][kk].first->Fill( indices[kk], pos[1] );
	      _hSim_index_posZ[colname][kk].first->Fill( indices[kk], pos[2] );
	      _hSim_index_posR[colname][kk].first ->Fill( indices[kk], rad );
	      _hSim_index_posPhi[colname][kk].first->Fill( indices[kk], phi );
	    } else {
	      _hSim_index_posX[colname][kk].second->Fill( indices[kk], pos[0] );
	      _hSim_index_posY[colname][kk].second->Fill( indices[kk], pos[1] );
	      _hSim_index_posZ[colname][kk].second->Fill( indices[kk], pos[2] );
	      _hSim_index_posR[colname][kk].second->Fill( indices[kk], rad );
	      _hSim_index_posPhi[colname][kk].second->Fill( indices[kk], phi );
	    }
	  }

	}

      }

    } catch(DataNotAvailableException &e) {};
  }
  return;
}



void validateAutoProcessor::check( LCEvent * evt ) {
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}

void validateAutoProcessor::end(){
  std::cout << "validateAutoProcessor::end()  " << name()
            << std::endl ;

  _fout->Write(0);
  _fout->Close();

}

