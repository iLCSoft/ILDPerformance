#include "validateDST_SinglePhoton.h"
#include <iostream>
#include <map>
#include <math.h>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <marlin/Global.h>
#include "lcio.h"

#include "EVENT/LCCollection.h"
#include "EVENT/MCParticle.h"

#include "EVENT/ReconstructedParticle.h"

using namespace lcio ;
using namespace marlin ;
using std::cout;
using std::endl;

validateDST_SinglePhotonProcessor avalidateDST_SinglePhotonProcessor ;


validateDST_SinglePhotonProcessor::validateDST_SinglePhotonProcessor() : Processor("validateDST_SinglePhotonProcessor") {

  // modify processor description
  _description = "validateDST_SinglePhotonProcessor makes histograms of basic properties for photons" ;

  registerProcessorParameter("outputFilename",
                             "name of output file",
                             _outfile,
                             std::string( "validatePhoton.root") );


}


void validateDST_SinglePhotonProcessor::init() {
  cout << "hello from validateDST_SinglePhotonProcessor::init" << endl;


  // the output root file for histograms
  _fout = new TFile(_outfile.c_str(),"recreate");

  _h_allMC_en = new TH1F("allMC_logen","allMC_logen",300,-1.5,2.5);
  _h_allMC_costh = new TH1F("allMC_costh","allMC_costh",500,-0.1,1.1);
  _h_allMC_phi = new TH1F("allMC_phi","allMC_phi",100,0,2*3.14159);
  _h_allMC_endcapxy = new TH2F("endcapMC_xy","endcapMC_xy",1000,-100,2000,1000,-100,2000);
  _h_mcDecPt = new TH2F("mcDecPt","mcDecPt", 500, 0, 2500, 500, 0, 2000 );

  return;
}

void validateDST_SinglePhotonProcessor::processRunHeader( LCRunHeader* ) {
  cout << "hello from validateDST_SinglePhotonProcessor::processRunHeader" << endl;
}


void validateDST_SinglePhotonProcessor::processEvent( LCEvent * evt ) {

  bool decInTrk = false;

  float mccosth(-99);
  float mcphi(-99);
  float mcphiFolded8(-99);
  float mcphiFolded4(-99);
  int imcenergy(-99);

  float mcendcapx(0);
  float mcendcapy(0);

  bool inEndcap(false), inBarrel(false); // , inOverlap(false);

  float photonDecPt[3]={0};

  //---------------------------------
  // MC information
  //---------------------------------
  try {
    LCCollection* mccol = evt->getCollection( "MCParticlesSkimmed" );

    assert ( mccol );

    MCParticle* photon = dynamic_cast<MCParticle*>(mccol->getElementAt(0));

    assert ( photon );

    if ( photon->getPDG()!=22 ) {
      cout << "not a photon!" << endl;
      assert(0);
    }

    imcenergy = int( 10*photon->getEnergy()+0.001 ); // for numerical prec.. this is in units of 100MeV.


    mccosth = photon->getMomentum()[2]/photon->getEnergy();
    mcphi = atan2( photon->getMomentum()[1], photon->getMomentum()[0] );
    //    if ( mcphi>M_PI ) mcphi -= 2*M_PI;
    if ( mcphi<0 ) mcphi += 2*M_PI;

    // 8-fold symm in barrel
    int sector = mcphi/(3.14159/4);
    mcphiFolded8 = mcphi - sector*3.14159/4.;

    // 4-fold in endcaps
    sector = mcphi/(3.14159/2);
    mcphiFolded4 = mcphi - sector*3.14159/2.;

    // position on endcap face (folded 4-fold)
    float zendcap=2450.;
    float rendcap=zendcap*tan(acos(fabs(mccosth)));
    mcendcapx = rendcap*cos(mcphiFolded4);
    mcendcapy = rendcap*sin(mcphiFolded4);


    _h_allMC_en->Fill( log10( photon->getEnergy() ) );
    _h_allMC_costh -> Fill( fabs(mccosth) );
    _h_allMC_phi -> Fill( mcphi );

    inEndcap = fabs(mccosth)>0.8;
    inBarrel = fabs(mccosth)<0.7;
    //    inOverlap = !inEndcap && !inBarrel;

    if ( inEndcap ) {
      _h_allMC_endcapxy -> Fill( mcendcapx, mcendcapy );
    }

    if ( photon->getDaughters().size()>0 ) {
      for (int i=0; i<3; i++) {
	photonDecPt[i]=photon->getDaughters()[0]->getVertex()[i];
      }
      float convRad = sqrt( pow(photonDecPt[0],2) + pow(photonDecPt[1],2) );
      float convZ = fabs(photonDecPt[2]);
      _h_mcDecPt->Fill(convZ, convRad);
      decInTrk= convZ < 2200 && convRad<1700 ;
    }

    if ( enHistos.find( imcenergy ) == enHistos.end() ) {

      std::map < int, TH1F* > histos;
      std::map < int, TH2F* > histos2d;

      TString hnn="photonEn"; hnn+=imcenergy; hnn+="_";

      std::string varname;

      float emax = imcenergy/5.;
      if ( imcenergy<10 ) { // under 1 GeV
	emax*=2;
      }

      histos[ivar_npfo] = new TH1F(hnn+"nPFO", hnn+"nPFO", 10,0,10);
      histos[ivar_npfo22] = new TH1F(hnn+"nPFO22", hnn+"nPFO22", 10,0,10);

      histos2d[ivar_npfoCosth] = new TH2F(hnn+"nPFOCosth", hnn+"nPFOCosth", 500,-0.05,1.05,10,0,10);

      histos2d[ivar_enCosth] = new TH2F(hnn+"enCosth", hnn+"enCosth", 500,-0.05,1.05,1000,0,emax);
      histos2d[ivar_totenCosth] = new TH2F(hnn+"totenCosth", hnn+"totenCosth", 500,-0.05,1.05,1000,0,emax);

      histos2d[ivar_barenPhi] = new TH2F(hnn+"enBarrelPhi", hnn+"enBarrelPhi", 100,-0.05,3.141/4+0.05,1000,0,emax);
      histos2d[ivar_bartotenPhi] = new TH2F(hnn+"totenBarrelPhi", hnn+"totenBarrelPhi", 100,-0.05,3.141/4+0.05,1000,0,emax);

      histos2d[ivar_negendEnX] = new TH2F(hnn+"negEndcapEnX", hnn+"negEndcapEnX",100, -10, 2000, 1000,0,emax);
      histos2d[ivar_negendEnY] = new TH2F(hnn+"negEndcapEnY", hnn+"negEndcapEnY",100, -10, 2000, 1000,0,emax);

      histos2d[ivar_posendEnX] = new TH2F(hnn+"posEndcapEnX", hnn+"posEndcapEnX",100, -10, 2000, 1000,0,emax);
      histos2d[ivar_posendEnY] = new TH2F(hnn+"posEndcapEnY", hnn+"posEndcapEnY",100, -10, 2000, 1000,0,emax);

      enHistos[imcenergy]=histos;      
      enHistos2d[imcenergy]=histos2d;
    }


  } catch(DataNotAvailableException &e) {};


  if ( decInTrk ) return;


  try {

    int npfo22(0);

    LCCollection* pfocol = evt->getCollection( "PandoraPFOs" );

    float toten(0);
    for (int j=0; j<pfocol->getNumberOfElements(); j++) {
      ReconstructedParticle* pfo = dynamic_cast<ReconstructedParticle*> (pfocol->getElementAt(j));

      if ( pfo->getType()==22 ) npfo22++;

      toten+=pfo->getEnergy();

      enHistos2d[imcenergy][ivar_enCosth]->Fill(fabs(mccosth), pfo->getEnergy() );

      if ( inBarrel ) {
	enHistos2d[imcenergy][ivar_barenPhi]->Fill(mcphiFolded8, pfo->getEnergy() );
      }

      if ( inEndcap ) {
	if ( mccosth>0 ) {
	  enHistos2d[imcenergy][ivar_posendEnX]->Fill(mcendcapx, pfo->getEnergy() );
	  enHistos2d[imcenergy][ivar_posendEnY]->Fill(mcendcapy, pfo->getEnergy() );
	} else {
	  enHistos2d[imcenergy][ivar_negendEnX]->Fill(mcendcapx, pfo->getEnergy() );
	  enHistos2d[imcenergy][ivar_negendEnY]->Fill(mcendcapy, pfo->getEnergy() );
	}	  
      }

    }

    enHistos2d[imcenergy][ivar_totenCosth]->Fill(fabs(mccosth), toten );

    if ( inBarrel ) {
      enHistos2d[imcenergy][ivar_bartotenPhi]->Fill(mcphiFolded8, toten );
    }



    //    cout << enHistos[imcenergy].find("nPFO") << " " << 

    std::string temp = "nPFO";

    enHistos2d[imcenergy][ivar_npfoCosth]->Fill(fabs(mccosth), pfocol->getNumberOfElements());

    enHistos[imcenergy][ivar_npfo]->Fill(pfocol->getNumberOfElements());
    enHistos[imcenergy][ivar_npfo22]->Fill(npfo22);


  } catch(DataNotAvailableException &e) {};


  return;
}


void validateDST_SinglePhotonProcessor::check( LCEvent * ) {
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}

void validateDST_SinglePhotonProcessor::end(){
  std::cout << "validateDST_SinglePhotonProcessor::end()  " << name()
            << std::endl ;

  _fout->Write(0);
  _fout->Close();

//  cout << "layer ranges: " << endl;
//
//  for (int i=0; i<NCALS; i++) {
//    for (size_t j=0; j<colnames[i].size(); j++) 
//      cout << colnames[i][j] << " ";
//    cout << minlayer[i] << " " << maxlayer[i] << endl;
//  }
//
//  cout << "layer weights: " << endl;
//  for (int i=0; i<NCALS; i++) {
//    cout << "collection " << i << "  ";
//    for (size_t j=0; j<colnames[i].size(); j++) 
//      cout << colnames[i][j] << " ";
//    cout << endl;
//    for (int j=0; j<MAXLAYS; j++) {
//      cout << "layer " << j << " " << layerWeights[i][j] << endl;
//    }
//  }



}

