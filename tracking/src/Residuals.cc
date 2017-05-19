#include "Residuals.h"
#include <iostream>


//#ifdef MARLIN_USE_AIDA
//#endif

using namespace lcio ;
using namespace marlin ;
using namespace std ;
using namespace MarlinTrk ;

Residuals aResiduals ;


Residuals::Residuals() : Processor("Residuals") {
  

  // modify processor description
  _description = "Residuals studies" ;
  

  // register steering parameters: name, description, class-variable, default value

  
  registerInputCollection( LCIO::TRACK,
                            "MarlinTrkTrack" , 
                            "Input track collection"  ,
                            _trks ,
                            std::string("MarlinTrkTracks") ) ;


  registerProcessorParameter( "TrackSystemName",
			      "Name of the track fitting system to be used (DDKalTest, aidaTT, ... )",
			      _trkSystemName,
			      std::string("DDKalTest") );

  registerProcessorParameter("SortWrtRadius",
                             "Sort the hits wrt to their radius",
                             _sortRad,
                             bool(true));
}


void Residuals::init() { 

  message<DEBUG>( "     Residuals::init()  " ) ;

  // usually a good idea to
  printParameters() ;

  /**********************************************************************************************/
  /*       Initialise the MarlinTrkSystem, needed by the tracks for fitting                     */
  /**********************************************************************************************/
  
  // set upt the geometry - choice between track systems 
  _trkSystem =  MarlinTrk::Factory::createMarlinTrkSystem( _trkSystemName , marlin::Global::GEAR , "" ) ;


  if( _trkSystem == 0 ) throw EVENT::Exception( std::string("  Cannot initialize MarlinTrkSystem of Type: ") + std::string("DDKalTest" )  ) ;
  
  
  // set the options   
  _trkSystem->setOption( MarlinTrk::IMarlinTrkSystem::CFG::useQMS,        _MSOn ) ;       //multiple scattering
  _trkSystem->setOption( MarlinTrk::IMarlinTrkSystem::CFG::usedEdx,       _ElossOn) ;     //energy loss
  _trkSystem->setOption( MarlinTrk::IMarlinTrkSystem::CFG::useSmoothing,  _SmoothOn) ;    //smoothing
  
  // initialise the tracking system
  _trkSystem->init() ;
 

  DD4hep::Geometry::LCDD& lcdd = DD4hep::Geometry::LCDD::getInstance();

  const double pos[3]={0,0,0}; 
  double bFieldVec[3]={0,0,0}; 
  lcdd.field().magneticField(pos,bFieldVec); // get the magnetic field vector from DD4hep
  _bField = bFieldVec[2]/dd4hep::tesla; // z component at (0,0,0)


  // create map of surfaces
  DD4hep::Geometry::DetElement world = lcdd.world() ;
  const aidaTT::IGeometry* geom  = & aidaTT::IGeometry::instance() ;
  const std::vector<const aidaTT::ISurface*>& surfaces = geom->getSurfaces() ;
  for(std::vector<const aidaTT::ISurface*>::const_iterator surf = surfaces.begin() ; surf != surfaces.end() ; ++surf){
    surfMap[(*surf)->id() ] = (*surf) ;
  }


  _nRun = 0 ;
  _nEvt = 0 ;
}

void Residuals::processRunHeader( LCRunHeader* run) { 

  _nRun++ ;

} 

void Residuals::processEvent( LCEvent * evt ) { 

  if( isFirstEvent() ) { 

    ResiduTree = new TTree("ResiduTree","ResiduTree") ;
    ResiduTree->Branch("TrackHitResidualsU", &TrackHitResidualsU);
    ResiduTree->Branch("TrackHitResidualsV", &TrackHitResidualsV);
    ResiduTree->Branch("DetLayer", &DetLayer);
    ResiduTree->Branch("SubDet", &SubDet);
  }

  TrackHitResidualsV.clear();     TrackHitResidualsU.clear();  
  DetLayer.clear();  SubDet.clear();

  streamlog_out(DEBUG4) << " ################# new event $$$$$$$$$$$$$$$$$$$ " << _nEvt << std::endl ;


  UTIL::BitField64 idDecoder(LCTrackerCellID::encoding_string()) ;

  LCCollection* MarlinTrks = evt->getCollection( _trks );
  int MarlinTracks = MarlinTrks->getNumberOfElements();
  for ( int ii = 0; ii < MarlinTracks; ii++ ) {
      
    Track *traj = dynamic_cast<Track*>( MarlinTrks->getElementAt( ii ) ) ;
    const EVENT::TrackState *init_trk_st = traj->getTrackState(1);

    EVENT::TrackerHitVec NewTrkHits ;

    MarlinTrk::IMarlinTrack* marlin_trk = _trkSystem->createTrack();

    TrackerHitVec initialHits = traj->getTrackerHits();


    std::vector< std::pair<float, EVENT::TrackerHit*> > r_values;
    std::vector< std::pair<float, EVENT::TrackerHit*> > z_values;
   
    if (!  _sortRad){
      // sorting of hit vector wrt to Z in order to be used for refitting
      //------------------------------------------------------------------------------------
      z_values.reserve(initialHits.size());
    
      for (TrackerHitVec::iterator it=initialHits.begin(); it!=initialHits.end(); ++it) {
	EVENT::TrackerHit* h = *it;
	float z2 = h->getPosition()[2]*h->getPosition()[2];
	z_values.push_back(std::make_pair(z2, *it));
      }
      sort(z_values.begin(),z_values.end());
    
      initialHits.clear();
      initialHits.reserve(z_values.size());
      //----------------------------------------------------------------------------------------
    }
    
    else{
      // sorting of hit vector wrt to radius in order to be used for refitting
      //------------------------------------------------------------------------------------
      r_values.reserve(initialHits.size());
    
      for (TrackerHitVec::iterator it=initialHits.begin(); it!=initialHits.end(); ++it) {
	EVENT::TrackerHit* h = *it;
	float r2 = h->getPosition()[0]*h->getPosition()[0] + h->getPosition()[1]*h->getPosition()[1] ;
	r_values.push_back(std::make_pair(r2, *it));
      }
      sort(r_values.begin(),r_values.end());
    
      initialHits.clear();
      initialHits.reserve(r_values.size());
      //----------------------------------------------------------------------------------------
    }
    
    UTIL::BitField64 cellID_encoder( lcio::LCTrackerCellID::encoding_string() ) ;
    
    for (std::vector< std::pair<float, EVENT::TrackerHit*> >::iterator it=r_values.begin(); it!=r_values.end(); ++it) {
      initialHits.push_back(it->second);
    }
    
    // Adding hit to the track
    EVENT::TrackerHitVec::iterator it = initialHits.begin();
    for( it = initialHits.begin() ; it != initialHits.end() ; ++it ){
      
      if(BitSet32((*it)->getType())[ UTIL::ILDTrkHitTypeBit::COMPOSITE_SPACEPOINT ]){        //it is a composite spacepoint
	const LCObjectVec rawObjects = (*it)->getRawHits();
	
	for(unsigned k = 0; k < rawObjects.size(); k++){
	  
	  TrackerHit* rawHit = dynamic_cast< TrackerHit* >(rawObjects[k]);
	  TrackerHitPlane* planarhit = dynamic_cast<TrackerHitPlane*>(rawHit);
	  marlin_trk->addHit(planarhit);
	  NewTrkHits.push_back(planarhit);
	  //streamlog_out(DEBUG4) << " I am adding a 1D hit to marlin_trk " << std::endl ;
	}
      }
      
      else {
	marlin_trk->addHit(*it);
	NewTrkHits.push_back(*it);	
      }
    }
    
    int init_status = marlin_trk->initialise( *init_trk_st, _bField, IMarlinTrack::backward ) ;
    int fit_status = marlin_trk->fit() ; 
    
    if ( fit_status == 0 ){
      
      //********************************************************************************************
      // Checking for LCIO track - hit residuals
      
      for(std::vector<TrackerHit*>::iterator lhit = NewTrkHits.begin(), endIter = NewTrkHits.end(); lhit < endIter; ++lhit){
	
	long64 hitid = (*lhit)->getCellID0() ;
	idDecoder.setValue(hitid) ;
	

	DetLayer.push_back(idDecoder[lcio::LCTrackerCellID::layer()]);
	SubDet.push_back(idDecoder[ lcio::LCTrackerCellID::subdet()]);

	double testRadius = sqrt(((*lhit)->getPosition()[0])*((*lhit)->getPosition()[0]) + ((*lhit)->getPosition()[1])*((*lhit)->getPosition()[1])) ;

	streamlog_out(DEBUG4) << " subdetector " << idDecoder[ lcio::LCTrackerCellID::subdet()] << " layer " << idDecoder[lcio::LCTrackerCellID::layer()] << " Z position " << (*lhit)->getPosition()[2]  << std::endl;
		

	const aidaTT::ISurface* surf3 = surfMap[ hitid ] ;
	
	
	if(surf3 != NULL){
	  
	  TrackerHit* testhit3 = dynamic_cast<TrackerHit*>(*lhit);
	  
	  double X2 = testhit3->getPosition()[0] * dd4hep::mm;
	  double Y2 = testhit3->getPosition()[1] * dd4hep::mm;
	  double Z2 = testhit3->getPosition()[2] * dd4hep::mm;

	  double globpos2[3] = {X2, Y2, Z2};
	  aidaTT::Vector3D globalPos2(globpos2) ;
	    
	  //aidaTT::Vector2D* localPos2 = new Vector2D() ;
	  DDSurfaces::Vector2D localPos2 = surf3->globalToLocal( globalPos2 ) ;	    
	    
	  double s2 = 0.;
	  int ndf = 0;
	  double chi2= 0. ;
	  TrackStateImpl ts_at_detel ;
	  int paok = marlin_trk->getTrackState(testhit3 , ts_at_detel,  chi2, ndf ) ;
	    
	  const aidaTT::Vector5 hp( ts_at_detel.getOmega() / dd4hep::mm, ts_at_detel.getTanLambda(), ts_at_detel.getPhi(), ts_at_detel.getD0() * dd4hep::mm, ts_at_detel.getZ0() * dd4hep::mm ) ;
	  aidaTT::Vector3D xx ;
	  const float *refpnt = ts_at_detel.getReferencePoint();
	  //const aidaTT::Vector3D rp(refpnt) ;
	  const aidaTT::Vector3D rp(refpnt[0] * dd4hep::mm, refpnt[1] * dd4hep::mm, refpnt[2] * dd4hep::mm ) ;
	  bool doesIt2 = aidaTT::intersectWithSurface( surf3, hp , rp, s2, xx, 0 , true ) ; 
	    

	  if(doesIt2){

	    //streamlog_out(DEBUG4) << " surface " << *surf3 << std::endl ;
	    streamlog_out(DEBUG4) << " Global coordinates: track x point " << xx[0] << ", " << xx[1] << ", " << xx[2] << "hit position " << globpos2[0] << ", " << globpos2[1] << ", " << globpos2[2] << std::endl;	  

	    DDSurfaces::Vector2D localUV2 = surf3->globalToLocal( xx ) ;
	      
	    double U = localPos2.u();
	    double V = localPos2.v();
	      
	    double tU = localUV2.u();
	    double tV = localUV2.v();
	      
	    double resU = tU - U ;
	    double resV = tV - V ;

	    TrackerHitPlane* planarhit3 = dynamic_cast<TrackerHitPlane*>(*lhit);
	    TrackerHit* tpchit3 =  dynamic_cast<TrackerHit*>(*lhit);
	    if ( idDecoder[ lcio::LCTrackerCellID::subdet()] != 4 ){   // not TPC
	      if(planarhit3 != NULL) {
		TrackHitResidualsU.push_back(resU);
		TrackHitResidualsV.push_back(resV);
	      }
	    }
	    else{       // if the subdetector is the TPC
	      if(tpchit3 != NULL) {
		TrackHitResidualsU.push_back(resU);
		TrackHitResidualsV.push_back(resV);
	      }
	    }
	  }
	}
      }
      
      //********************************************************************************************
    }  // end fit status
  }  //end loop on tracks
  
  
  
  ResiduTree->Fill();
  _nEvt ++ ;
    
  
}



void Residuals::check( LCEvent * evt ) { 
  // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void Residuals::end(){ 

}

