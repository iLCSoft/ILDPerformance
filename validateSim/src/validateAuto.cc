#include "validateAuto.h"
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>

#include <marlin/Global.h>
#include "lcio.h"

#include "EVENT/LCCollection.h"

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
  _CalorimeterHitDecoder=0;
  _TrackerHitDecoder=0;

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
      if ( _allranges.find( colname ) == _allranges.end() ) {
        validatePilotProcessor_maxMin ss;
        _allranges[colname] = ss;
      }
      if ( isindex ) {
        _allranges[colname].indx_name.push_back(varname);
        _allranges[colname].indx_minmax.push_back( std::pair < int, int > ( int(minval), int(maxval) ) );
      } else if ( varname=="Energy" ) {
        _allranges[colname].eminmax =  std::pair < float, float > (minval, maxval);
      } else if ( varname=="Time" ) {
        _allranges[colname].tminmax =  std::pair < float, float > (minval, maxval);
      } else if ( varname=="X" ) {
        _allranges[colname].xminmax =  std::pair < float, float > (minval, maxval);
      } else if ( varname=="Y" ) {
        _allranges[colname].yminmax =  std::pair < float, float > (minval, maxval);
      } else if ( varname=="Z" ) {
        _allranges[colname].zminmax =  std::pair < float, float > (minval, maxval);
      } else if ( varname=="Absz" ) {
        _allranges[colname].abszminmax =  std::pair < float, float > (minval, maxval);
      } else if ( varname=="R" ) {
        _allranges[colname].rminmax =  std::pair < float, float > (minval, maxval);
      } else {
        cout << "unknown variable? " << varname << endl;
        assert(0);
      }
      //  std::map < std::string , validatePilotProcessor_maxMin > _allranges;
    }
    myfile.close();
  } else {
    cout << "Unable to open file " << _infile << endl;
    assert(0);
  }

  // the output root file for histograms
  std::string outfilename = "autoValidate.root";
  _fout = new TFile(outfilename.c_str(),"recreate");

  const int nbins1d=300;
  const int nbins2d=100;

  float zmaxall(0);
  float rmaxall(0);

  // make a directory for each collection
  for ( std::map < std::string , validatePilotProcessor_maxMin >::iterator itt=_allranges.begin(); itt!=_allranges.end(); itt++) {

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
    _h_HitEn[ itt->first ] = new TH1F(hname, hname,nbins1d,hmin,hmax);
    _h_HitEn[ itt->first ]->GetXaxis()->SetTitle("hit energy");

    hname = itt->first.c_str(); hname += "_hitTime";
    hmin = itt->second.tminmax.first;
    hmax = itt->second.tminmax.second;
    if ( hmax > 100 ) hmax = 100;
    diff = hmax-hmin;    hmin-=diff/20.;    hmax+=diff/20.;
    _h_HitTime[ itt->first ] = new TH1F(hname, hname,nbins1d,hmin,hmax);
    _h_HitTime[ itt->first ]->GetXaxis()->SetTitle("hit time [ns]");

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
    h1->GetXaxis()->SetTitle("hit X [mm]");
    h1->GetYaxis()->SetTitle("hit Y [mm]");
    if ( isEndcap ) {
      hname2 = hname + "_negZ";
      h2 = new TH2F(hname2, hname2,nbins2d,hmin,hmax, nbins2d, hmin2, hmax2);
      h2->GetXaxis()->SetTitle("hit X [mm]");
      h2->GetYaxis()->SetTitle("hit Y [mm]");
    }
    _h_posXY[ itt->first ] = std::pair < TH2F* , TH2F* > (h1, h2);

    hname = itt->first.c_str(); hname += "_hitZR";
    hname1 = isEndcap ? hname + "_posZ" : hname;

    hmin = isEndcap ? itt->second.abszminmax.first : itt->second.zminmax.first;
    hmax = itt->second.zminmax.second;
    diff = hmax-hmin;    hmin-=diff/20.;    hmax+=diff/20.;

    hmin2 = itt->second.rminmax.first;
    hmax2 = itt->second.rminmax.second;
    diff2 = hmax2-hmin2; hmin2-=diff2/20.;    hmax2+=diff2/20.;
    h1 = new TH2F(hname1, hname1,nbins2d,hmin,hmax, nbins2d, hmin2, hmax2);
    h1->GetXaxis()->SetTitle("hit Z [mm]");
    h1->GetYaxis()->SetTitle("hit R [mm]");
    if ( isEndcap ) {
      hname2 = hname + "_negZ";
      h2 = new TH2F(hname2, hname2,nbins2d,-hmax,-hmin, nbins2d, hmin2, hmax2);
      h2->GetXaxis()->SetTitle("hit Z [mm]");
      h2->GetYaxis()->SetTitle("hit R [mm]");
    }
    _h_posZR[ itt->first ] = std::pair < TH2F* , TH2F* > (h1, h2);


    std::vector < std::pair < TH2F* , TH2F* > > aa;
    int nbins(1);

    _h_index_posX[ itt->first ] = aa;
    
    // indices vs. Z
    for (size_t jj=0; jj<itt->second.indx_name.size(); jj++) {
      hname = itt->first.c_str(); hname += "_Indx"+itt->second.indx_name[jj]+"_X";
      hname1 = isEndcap ? hname + "_posZ" : hname;

      hmin = itt->second.indx_minmax[jj].first-1;
      hmax = itt->second.indx_minmax[jj].second+2;
      nbins = hmax-hmin; if (nbins>nbins2d) nbins=nbins2d;
      
      hmin2 = itt->second.xminmax.first;
      hmax2 = itt->second.xminmax.second;
      diff2 = hmax2-hmin2; hmin2-=diff2/20.;    hmax2+=diff2/20.;

      h1 = new TH2F(hname1, hname1,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      h1->GetXaxis()->SetTitle(itt->second.indx_name[jj].c_str());
      h1->GetYaxis()->SetTitle("hit X [mm]");
      if ( isEndcap ) {
	hname2 = hname + "_negZ";
	h2 = new TH2F(hname2, hname2,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
	h2->GetXaxis()->SetTitle(itt->second.indx_name[jj].c_str());
	h2->GetYaxis()->SetTitle("hit X [mm]");
      }
      _h_index_posX[ itt->first ].push_back( std::pair < TH2F* , TH2F* > (h1, h2) );
    }

    // vs. Y
    for (size_t jj=0; jj<itt->second.indx_name.size(); jj++) {
      hname = itt->first.c_str(); hname += "_Indx"+itt->second.indx_name[jj]+"_Y";
      hname1 = isEndcap ? hname + "_posZ" : hname;
      hmin = itt->second.indx_minmax[jj].first-1;
      hmax = itt->second.indx_minmax[jj].second+2;
      nbins = hmax-hmin; if (nbins>nbins2d) nbins=nbins2d;

      hmin2 = itt->second.yminmax.first;
      hmax2 = itt->second.yminmax.second;
      diff2 = hmax2-hmin2; hmin2-=diff2/20.;    hmax2+=diff2/20.;
      h1 = new TH2F(hname1, hname1,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      h1->GetXaxis()->SetTitle(itt->second.indx_name[jj].c_str());
      h1->GetYaxis()->SetTitle("hit Y [mm]");
      if ( isEndcap ) {
	hname2 = hname + "_negZ";
	h2 = new TH2F(hname2, hname2,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
	h2->GetXaxis()->SetTitle(itt->second.indx_name[jj].c_str());
	h2->GetYaxis()->SetTitle("hit Y [mm]");
      }
      _h_index_posY[ itt->first ].push_back( std::pair < TH2F* , TH2F* > (h1, h2) );
    }

    // vs. Z
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
      diff2 = hmax2-hmin2; hmin2-=diff2/20.;    hmax2+=diff2/20.;
      h1 = new TH2F(hname1, hname1,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      h1->GetXaxis()->SetTitle(itt->second.indx_name[jj].c_str());
      h1->GetYaxis()->SetTitle("hit Z [mm]");
      if ( isEndcap ) {
	hname2 = hname + "_negZ";
	h2 = new TH2F(hname2, hname2,nbins,hmin,hmax, nbins2d, -hmax2, -hmin2);
	h2->GetXaxis()->SetTitle(itt->second.indx_name[jj].c_str());
	h2->GetYaxis()->SetTitle("hit Z [mm]");
      }
      _h_index_posZ[ itt->first ].push_back( std::pair < TH2F* , TH2F* > (h1, h2) );
    }

    // vs. R
    for (size_t jj=0; jj<itt->second.indx_name.size(); jj++) {
      hname = itt->first.c_str(); hname += "_Indx"+itt->second.indx_name[jj]+"_R";
      hname1 = isEndcap ? hname + "_posZ" : hname;
      hmin = itt->second.indx_minmax[jj].first-1;
      hmax = itt->second.indx_minmax[jj].second+2;
      nbins = hmax-hmin; if (nbins>nbins2d) nbins=nbins2d;
      hmin2 = itt->second.rminmax.first;
      hmax2 = itt->second.rminmax.second;
      diff2 = hmax2-hmin2; hmin2-=diff2/20.;    hmax2+=diff2/20.;
      h1 = new TH2F(hname1, hname1,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      h1->GetXaxis()->SetTitle(itt->second.indx_name[jj].c_str());
      h1->GetYaxis()->SetTitle("hit R [mm]");
      if ( isEndcap ) {
	hname2 = hname + "_negZ";
	h2 = new TH2F(hname2, hname2,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
	h2->GetXaxis()->SetTitle(itt->second.indx_name[jj].c_str());
	h2->GetYaxis()->SetTitle("hit R [mm]");
      }
      _h_index_posR[ itt->first ].push_back( std::pair < TH2F* , TH2F* > (h1, h2) );
    }

    // vs Phi
    for (size_t jj=0; jj<itt->second.indx_name.size(); jj++) {
      hname = itt->first.c_str(); hname += "_Indx"+itt->second.indx_name[jj]+"_Phi";
      hname1 = isEndcap ? hname + "_posZ" : hname;
      hmin = itt->second.indx_minmax[jj].first-1;
      hmax = itt->second.indx_minmax[jj].second+2;
      nbins = hmax-hmin; if (nbins>nbins2d) nbins=nbins2d;
      hmin2 = -acos(-1);
      hmax2 =  acos(-1);
      diff2 = hmax2-hmin2; hmin2-=diff2/20.;    hmax2+=diff2/20.;
      h1 = new TH2F(hname1, hname1,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
      h1->GetXaxis()->SetTitle(itt->second.indx_name[jj].c_str());
      h1->GetYaxis()->SetTitle("hit Phi [rad]");
      if ( isEndcap ) {
	hname2 = hname + "_negZ";
	h2 = new TH2F(hname2, hname2,nbins,hmin,hmax, nbins2d, hmin2, hmax2);
	h2->GetXaxis()->SetTitle(itt->second.indx_name[jj].c_str());
	h2->GetYaxis()->SetTitle("hit Phi [rad]");
      }
      _h_index_posPhi[ itt->first ].push_back( std::pair < TH2F* , TH2F* > (h1, h2) );
    }

  }



  // overall geometry view

  _fout->mkdir( "ALLCollections" );
  _fout->cd( "ALLCollections" );

  TString hname1 = "ALLCollections_overallhitZR";
  _hAll_overallposZR = new TH2F(hname1, hname1,2*nbins2d,-zmaxall*1.1,zmaxall*1.1, 2*nbins2d, 0, rmaxall*1.1);
  _hAll_overallposZR->GetXaxis()->SetTitle("Z [mm]");
  _hAll_overallposZR->GetYaxis()->SetTitle("R [mm]");
  hname1 = "ALLCollections_Log_overallhitZR";
  _hAll_Log_overallposZR = new TH2F(hname1, hname1,2*nbins2d,-1,log10(zmaxall)*1.1, 2*nbins2d, 0.5, log10(rmaxall)*1.1);
  _hAll_Log_overallposZR->GetXaxis()->SetTitle("log10(Z [mm])");
  _hAll_Log_overallposZR->GetYaxis()->SetTitle("log10(R [mm])");

  for ( std::map < std::string , validatePilotProcessor_maxMin >::iterator itt=_allranges.begin(); itt!=_allranges.end(); itt++) {
    hname1 = itt->first + "_overallhitZR";
    _h_overallposZR[itt->first] = new TH2F(hname1, hname1,2*nbins2d,-zmaxall*1.1,zmaxall*1.1, 2*nbins2d, 0, rmaxall*1.1);
    _h_overallposZR[itt->first] ->GetXaxis()->SetTitle("Z [mm]");
    _h_overallposZR[itt->first] ->GetYaxis()->SetTitle("R [mm]");
    hname1 = itt->first + "_Log_overallhitZR";
    _h_Log_overallposZR[itt->first] = new TH2F(hname1, hname1,2*nbins2d,-1,log10(zmaxall)*1.1, 2*nbins2d, 0.5, log10(rmaxall)*1.1);
    _h_Log_overallposZR[itt->first]->GetXaxis()->SetTitle("log10(Z [mm])");
    _h_Log_overallposZR[itt->first]->GetYaxis()->SetTitle("log10(R [mm])");
  }



  return;
}

void validateAutoProcessor::processRunHeader( LCRunHeader* run) {
  cout << "hello from validateAutoProcessor::processRunHeader" << endl;
}

void validateAutoProcessor::processEvent( LCEvent * evt ) {


  for ( std::map < std::string , validatePilotProcessor_maxMin >::iterator itt=_allranges.begin(); itt!=_allranges.end(); itt++) {
    std::string colname = itt->first;
    try {
      LCCollection* simhitcol = evt->getCollection( colname );
      cellindices.clear();
      for (size_t jj=0; jj<_allranges[colname].indx_name.size(); jj++) {
	cellindices.push_back( _allranges[colname].indx_name[jj] );
      }

      // work out what type of object is in this collection
      int objtype(0);
      if ( simhitcol->getTypeName () == LCIO::SIMCALORIMETERHIT ) {
        if ( _SimCalorimeterHitDecoder ) delete _SimCalorimeterHitDecoder;
        _SimCalorimeterHitDecoder = new CellIDDecoder<SimCalorimeterHit> (simhitcol);
        objtype=1;
      } else if ( simhitcol->getTypeName () == LCIO::SIMTRACKERHIT ) {
        if ( _SimTrackerHitDecoder ) delete _SimTrackerHitDecoder;
        _SimTrackerHitDecoder = new CellIDDecoder<SimTrackerHit> (simhitcol);
        objtype=2;
      } else if ( simhitcol->getTypeName () == LCIO::CALORIMETERHIT ) {
        if ( _CalorimeterHitDecoder ) delete _CalorimeterHitDecoder;
        _CalorimeterHitDecoder = new CellIDDecoder<CalorimeterHit> (simhitcol);
        objtype=3;
      } else if ( simhitcol->getTypeName () == LCIO::TRACKERHIT ) {
        if ( _TrackerHitDecoder ) delete _TrackerHitDecoder;
        _TrackerHitDecoder = new CellIDDecoder<TrackerHit> (simhitcol);
        objtype=4;
      } else {
        cout << "unknown hit type! " << simhitcol->getTypeName () << endl;
	// shouldn't get here
        assert(0);
      }

      // loop over the elements of the collection
      for (int j=0; j<simhitcol->getNumberOfElements(); j++) {

        float energy(-99);
        float pos[3]={0,0,0};
        std::vector < float > times;
        std::vector < int > indices;

        SimCalorimeterHit* simcalhit;
        SimTrackerHit* simtrkhit;
        CalorimeterHit* calhit;
        TrackerHit* trkhit;

	// get basic quantities, according to object type
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

        case 3: // CalorimeterHit
          calhit = dynamic_cast<CalorimeterHit*> (simhitcol->getElementAt(j));
          energy=calhit->getEnergy();
          for (int i=0; i<3; i++)
            pos[i] = calhit->getPosition()[i];
	  times.push_back( calhit->getTime() );
	  for ( size_t kk=0; kk<cellindices.size(); kk++) {
            int ii = (*_CalorimeterHitDecoder)( calhit ) [ cellindices[kk] ];
            indices.push_back( ii );
	  }
          break;

        case 4: // TrackerHit
          trkhit = dynamic_cast<TrackerHit*> (simhitcol->getElementAt(j));
          energy=trkhit->getEDep();
          for (int i=0; i<3; i++)
            pos[i] = trkhit->getPosition()[i];
          times.push_back( trkhit->getTime() );
	  for ( size_t kk=0; kk<cellindices.size(); kk++) {
            int ii = (*_TrackerHitDecoder)( trkhit ) [ cellindices[kk] ];
            indices.push_back( ii );
	  }
          break;

        default:
          cout << "unknown type! " << objtype << endl;
          assert(0);
          break;

        }

        // fill histograms
        _h_HitEn[colname]->Fill(energy);

	for (size_t i=0; i<times.size(); i++)
	  _h_HitTime[colname]->Fill( times[i] );

	bool isBarrel = _h_posXY[ colname ].second==0;

	if ( isBarrel ) {
	  _h_posXY[ colname ].first->Fill(pos[0], pos[1] );
	} else {
	  if ( pos[2]>0 ) {
	    _h_posXY[ colname ].first->Fill(pos[0], pos[1] );
	  } else {	    
	    _h_posXY[ colname ].second->Fill(pos[0], pos[1] );
	  }
	}

	float r = sqrt( pow(pos[0],2)+pow(pos[1],2) );
	if ( isBarrel ) {
	  _h_posZR[ colname ].first->Fill(pos[2], r );
	} else {
	  if ( pos[2]>0 ) {
	    _h_posZR[ colname ].first->Fill(pos[2], r );
	  } else {	    
	    _h_posZR[ colname ].second->Fill(pos[2], r );
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
	    _h_index_posX[colname][kk].first->Fill( indices[kk], pos[0] );
	    _h_index_posY[colname][kk].first->Fill( indices[kk], pos[1] );
	    _h_index_posZ[colname][kk].first->Fill( indices[kk], pos[2] );
	    _h_index_posR[colname][kk].first->Fill( indices[kk], rad );
	    _h_index_posPhi[colname][kk].first->Fill( indices[kk], phi );
	  } else {
	    if ( pos[2] > 0 ) {
	      _h_index_posX[colname][kk].first ->Fill( indices[kk], pos[0] );
	      _h_index_posY[colname][kk].first->Fill( indices[kk], pos[1] );
	      _h_index_posZ[colname][kk].first->Fill( indices[kk], pos[2] );
	      _h_index_posR[colname][kk].first ->Fill( indices[kk], rad );
	      _h_index_posPhi[colname][kk].first->Fill( indices[kk], phi );
	    } else {
	      _h_index_posX[colname][kk].second->Fill( indices[kk], pos[0] );
	      _h_index_posY[colname][kk].second->Fill( indices[kk], pos[1] );
	      _h_index_posZ[colname][kk].second->Fill( indices[kk], pos[2] );
	      _h_index_posR[colname][kk].second->Fill( indices[kk], rad );
	      _h_index_posPhi[colname][kk].second->Fill( indices[kk], phi );
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

