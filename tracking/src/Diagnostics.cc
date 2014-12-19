#include "Diagnostics.h"

#include <UTIL/LCRelationNavigator.h>
#include "TROOT.h"


//------------------------------------------------------------------------------------------------------------
struct WSort {  // sort track segments wtr to higher inverse relative weight
  inline bool operator()(const std::pair<EVENT::MCParticle*, EVENT::LCRelation*> &l, const std::pair<EVENT::MCParticle*, EVENT::LCRelation*> &r) {      
    return ( fabs( (l.second)->getWeight() ) > fabs(  (r.second)->getWeight()  ) ) ;   
  }
};

struct invWSort {  // sort track segments wtr to higher inverse relative weight
  inline bool operator()(const std::pair<EVENT::Track*, EVENT::LCRelation*> &l, const std::pair<EVENT::Track*, EVENT::LCRelation*> &r) {      
    return ( fabs( (l.second)->getWeight() ) > fabs(  (r.second)->getWeight()  ) ) ;   
  }
};
//-------------------------------------------------------------------------------------------------------------

//======================================================================================================
// 
#define APPLY_CUT( LEVEL, Cut, Exp )  if( (Exp) == false ) {  if ( Cut ) \
    streamlog_out( LEVEL ) << "  ***** failed cut:  [ " <<  #Exp \
			   <<  " ] in evt: " << evt->getEventNumber()	\
			   << " run: "  << evt->getRunNumber()   << std::endl ; \
    Cut = false ; }

//======================================================================================================

Diagnostics aDiagnostics ;


Diagnostics::Diagnostics() : Processor("Diagnostics") {
  
  // modify processor description
  _description = "Diagnostics does whatever it does ..." ;

  
  // register steering parameters: name, description, class-variable, default value


  registerInputCollection( LCIO::LCRELATION,
			   "MCTracksTruthLinkCollectionName" , 
			   "true - reco relation collection"  ,
			   _trueToReco,
			   std::string("MCTracksTruthLink") ) ;


  registerInputCollection( LCIO::LCRELATION,
			   "TracksMCTruthLinkCollectionName" , 
			   "reco - true relation collection"  ,
			   _recoToTrue,
			   std::string("TracksMCTruthLink") ) ;

  registerInputCollection( LCIO::LCRELATION,
			   "SITTrackerHitRelations" , 
			   "SIT tracker hits to sim hits"  ,
			   _sitTrkHitRelations,
			   std::string("SITTrackerHitRelations") ) ;

  registerInputCollection( LCIO::LCRELATION,
			   "VXDTrackerHitRelations" , 
			   "VXD tracker hits to sim hits"  ,
			   _vxdTrkHitRelations,
			   std::string("VXDTrackerHitRelations") ) ;


  registerInputCollection( LCIO::MCPARTICLE,
			   "MCParticleCollection" , 
			   "Name of the MCParticle input collection"  ,
			   _mcParticleCollectionName ,
			   std::string("MCParticle") ) ;


  StringVec exampleSimHits ;
  exampleSimHits.push_back("VXDCollection") ;
  exampleSimHits.push_back("SITCollection") ;
  exampleSimHits.push_back("TPCCollection") ;
  
  registerInputCollections( LCIO::SIMTRACKERHIT,
			   "SimTrackerHitCollections" ,
			   "Names of the SimTrackerHits input collection"  ,
			   _simTrkHitCollectionNames ,
			    exampleSimHits ) ;

  registerInputCollection( LCIO::TRACKERHIT,
			   "SpcPntsCollection" ,
			   "Name of the SIT SpacePoints collection"  ,
			   _sitSpacePoints ,
			   std::string("SITSpacePoints")  ) ;

  registerInputCollection( LCIO::TRACKERHIT,
			   "VXDTrackerHits" ,
			   "Name of the VXDTrackerHit collection"  ,
			   _vxdTrackerHits ,
			   std::string("VXDTrackerHits")  ) ;

  registerInputCollection( LCIO::TRACK,
			   "MarlinTrkTracks" , 
			   "Name of the FullLDC track collection"  ,
			   _trackColName ,
			   std::string("MarlinTrkTracks") ) ;  

  registerInputCollection( LCIO::TRACK,
			   "ClupatraTracks" , 
			   "Name of the Clupatra track collection"  ,
			   _cleoColName ,
			   std::string("ClupatraTracks") ) ;  


  registerInputCollection( LCIO::TRACK,
			   "SeedTracks" , 
			   "Name of the TPC track collection"  ,
			   _seedTrackColName ,
			   std::string("SeedTracks") ) ;

  registerInputCollection( LCIO::TRACK,
			   "SiTracks" , 
			   "Name of the Silicon track collection"  ,
			   _siTrackColName ,
			   std::string("SiTracks") ) ;

  registerProcessorParameter("TrkEffOn",
                             "Enables cuts to define the examined track sample",
                             _trkEffOn,
                             bool(false));

}


void Diagnostics::init() { 

  streamlog_out(DEBUG) << "   init called  " 
		       << std::endl ;

  // usually a good idea to
  printParameters() ;
  nEvt = 0;

  _bField = Global::GEAR->getBField().at( gear::Vector3D(0., 0., 0.) ).z();    //The B field in z direction

  ghostCounter = 0 ;

  gROOT->ProcessLine("#include <vector>");

}

void Diagnostics::processRunHeader( LCRunHeader* run) { 
    
} 

void Diagnostics::processEvent( LCEvent * evt ) { 

  streamlog_out(MESSAGE) << " start processing event " << std::endl;

  if( isFirstEvent() ) { 
    EvalTree = new TTree("EvalTree","EvalTree");
    EvalTree->Branch("foundTrk",&foundTrk) ;
    EvalTree->Branch("PtReco",&PtReco) ;
    EvalTree->Branch("GhostsPt",&GhostsPt) ;
    EvalTree->Branch("SiTrksPt",&SiTrksPt) ;
    EvalTree->Branch("ghostTrkChi2OverNdof",&ghostTrkChi2OverNdof);
    EvalTree->Branch("SiTrkChi2OverNdof",&SiTrkChi2OverNdof);
    EvalTree->Branch("CluChi2OverNdof",&CluChi2OverNdof);
    EvalTree->Branch("foundTrkChi2OverNdof",&foundTrkChi2OverNdof);
    EvalTree->Branch("PtMCP",&PtMCP) ;
    EvalTree->Branch("InvWgt",&InvWgt) ;
    EvalTree->Branch("Wgt",&Wgt) ;
    EvalTree->Branch("TrackSiHits",&TrackSiHits) ;
    EvalTree->Branch("SiHitsSiTrk",&SiHitsSiTrk) ;
    EvalTree->Branch("VXDHits",&VXDHits) ;
    EvalTree->Branch("SITHits",&SITHits) ;
    EvalTree->Branch("MarlinTracks",&MarlinTracks,"MarlinTracks/I") ;
    EvalTree->Branch("SeedTracks",&SeedTracks,"SeedTracks/I") ;
    EvalTree->Branch("SiliconTracks",&SiliconTracks,"SiliconTracks/I") ;
    EvalTree->Branch("trueD0",&trueD0) ;
    EvalTree->Branch("trueZ0",&trueZ0) ;
    EvalTree->Branch("recoD0",&recoD0) ;
    EvalTree->Branch("recoZ0",&recoZ0) ;
    EvalTree->Branch("recoD0error",&recoD0error) ;
    EvalTree->Branch("recoZ0error",&recoZ0error) ;
    EvalTree->Branch("trueOmega",&trueOmega) ;
    EvalTree->Branch("truePhi",&truePhi) ;
    EvalTree->Branch("trueTanLambda",&trueTanLambda) ;
    EvalTree->Branch("recoOmega",&recoOmega) ;
    EvalTree->Branch("recoOmegaError",&recoOmegaError) ;
    EvalTree->Branch("recoPhi",&recoPhi) ;
    EvalTree->Branch("recoPhiError",&recoPhiError) ;
    EvalTree->Branch("recoTanLambda",&recoTanLambda) ;
    EvalTree->Branch("recoTanLambdaError",&recoTanLambdaError) ;
    EvalTree->Branch("ghostCosTheta",&ghostCosTheta) ;
    EvalTree->Branch("siTrksCosTheta",&siTrksCosTheta) ;
    EvalTree->Branch("MarlinTrkHits",&MarlinTrkHits) ;


    OmegaPull = new TH1F("OmegaPull","Omega pull",100,-10,10);
    PhiPull = new TH1F("PhiPull","Phi pull",100,-10,10);
    TanLambdaPull = new TH1F("TanLambdaPull","TanLambda",100,-10,10);
    d0pull = new TH1F("d0pull","d0 pull",100,-10,10);
    z0pull = new TH1F("z0pull","z0",100,-10,10); 

    //double bins[nBins+1] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.7, 0.8, 1.0, 2., 5.0 , 10. , 20. , 50. , 100. , 300. , 500. } ;
    double bins[nBins+1] = { 0.01, 0.1, 0.2, 0.3, 0.5 , 0.7 , 1.0 , 2., 5.0 , 10. , 20. , 50. , 100. , 300.  } ;

    hist_pt_t  = new TH1F( "hist_pt_t", "Pt distributions of true tracks", nBins , bins ) ;
    hist_pt_f  = new TH1F( "hist_pt_f", "Pt distribution of found tracks", nBins , bins ) ;
    
    pulls = new TCanvas("pulls","Track par. pulls",800,800);
    eff = new TCanvas("eff","Trk Eff",800,800);

    myfunc = new TF1("myfunc","gaus(0)");

    gpt = new TGraphAsymmErrors() ;
  }

  foundTrk.clear();  PtReco.clear();  PtMCP.clear();  Wgt.clear();   InvWgt.clear();   TrackSiHits.clear();
  trueD0.clear();    trueZ0.clear();  trueOmega.clear();   truePhi.clear();  trueTanLambda.clear();
  recoD0.clear();    recoZ0.clear();  recoOmega.clear();   recoPhi.clear();  recoTanLambda.clear();
  recoD0Error.clear();    recoZ0Error.clear();  recoOmegaError.clear();   recoPhiError.clear();  recoTanLambdaError.clear();
  GhostsPt.clear();  SiTrksPt.clear();  SiHitsSiTrk.clear();
  foundTrkChi2OverNdof.clear();    SiTrkChi2OverNdof.clear();    ghostTrkChi2OverNdof.clear();  ghostCosTheta.clear();
  siTrksCosTheta.clear();  MarlinTrkHits.clear();   CluChi2OverNdof.clear();
  VXDHits.clear();  SITHits.clear();

  typedef std::map< Track* , int> SiTrackMap;
  SiTrackMap TrackMap ;
  SiTrackMap MarlinTrkMap ;

  int flagTrack = 0; int flagSeedTrack = 0; int flagSiTrack = 0; int flagTrueReco = 0;  int flagSiRel = 0;
  int flagRecoToTrue = 0 ; int  flagTrueToReco = 0 ;   int flagCleo = 0 ;

  const StringVec*  colNames = evt->getCollectionNames() ;
  for( StringVec::const_iterator it = colNames->begin() ; it != colNames->end() ; it++ ){

    if  ( _trackColName == *it )
      flagTrack = 1 ;

    if  ( _seedTrackColName == *it )
      flagSeedTrack = 1 ;

    if  ( _siTrackColName == *it )
      flagSiTrack = 1 ;

    if ( _cleoColName == *it )
      flagCleo = 1 ;

    if  ( _trueToReco  == *it )
      flagTrueReco = 1 ; 

    if  ( _vxdTrkHitRelations == *it )
      flagSiRel = 1 ;

    if  ( _recoToTrue == *it )
      flagRecoToTrue = 1 ;

    if  ( _trueToReco == *it )
      flagTrueToReco = 1 ;

  }


  if ( flagTrack == 1 ){
    LCCollection* MarlinTrks = evt->getCollection( _trackColName );
    MarlinTracks = MarlinTrks->getNumberOfElements();

    for ( int ii = 0; ii < MarlinTracks; ii++ ) {

      Track *MarlinRecoTrack = dynamic_cast<Track*>( MarlinTrks->getElementAt( ii ) ) ;
      MarlinTrkMap[MarlinRecoTrack] ++ ;
      MarlinTrkHits.push_back(MarlinRecoTrack->getTrackerHits().size());

    }
  }


  if ( flagCleo == 1 ){
    LCCollection* CluTrks = evt->getCollection( _cleoColName );
    int ClupatraTracks = CluTrks->getNumberOfElements();

    for ( int ii = 0; ii < ClupatraTracks; ii++ ) {

      Track *CluRecoTrack = dynamic_cast<Track*>( CluTrks->getElementAt( ii ) ) ;
      double TPCTrkChi2 = CluRecoTrack->getChi2();
      double TPCTrkNdf = CluRecoTrack->getNdf();

      //int ClupaHits = ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[6] + ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[7];

      if (TPCTrkNdf<0){
	streamlog_out(DEBUG4) << " No of TPC hits (pat.rec.) " << CluRecoTrack->getSubdetectorHitNumbers()[7] << " No of TPC hits used in fit " << CluRecoTrack->getSubdetectorHitNumbers()[6] << std::endl ;
      }

       CluChi2OverNdof.push_back( TPCTrkNdf ) ; 

    }
  }

  
  if ( flagSeedTrack == 1 ) {
    LCCollection* SeedTrks = evt->getCollection( _seedTrackColName );
    SeedTracks = SeedTrks->getNumberOfElements();
  }
  
  if ( flagSiTrack == 1 ) {

    LCCollection* SiTrks = evt->getCollection( _siTrackColName );
    SiliconTracks = SiTrks->getNumberOfElements();

    for ( int ii = 0; ii < SiliconTracks; ii++ ) {
      Track *SiRecoTrack = dynamic_cast<Track*>( SiTrks->getElementAt( ii ) ) ;
      streamlog_out(DEBUG2) << " checking track " << SiRecoTrack << std::endl ;
      SiTrksPt.push_back( fabs(((3.0/10000.0)*_bField) / SiRecoTrack->getOmega())) ;
      SiHitsSiTrk.push_back(SiRecoTrack->getSubdetectorHitNumbers()[0] + SiRecoTrack->getSubdetectorHitNumbers()[2]);

      double SiTrkChi2 = SiRecoTrack->getChi2();
      double SiTrkNdf = SiRecoTrack->getNdf();

      SiTrkChi2OverNdof.push_back( SiTrkChi2/SiTrkNdf ) ; 
      //else { streamlog_out(DEBUG4) << " What da heel wazzit? " << std::endl ; }

      double Si_Tracks_Cos_Theta = cos ( atan ( (1.0 / SiRecoTrack->getTanLambda()))) ;
      siTrksCosTheta.push_back( Si_Tracks_Cos_Theta ) ;

      TrackMap[SiRecoTrack] ++ ;

    }
  }
  

  for( SiTrackMap::iterator ii=TrackMap.begin(); ii!=TrackMap.end(); ++ii) {
    //cout << " Iteration over whole silicon track sample " << (*ii).first << ": " << (*ii).second << endl;
  }

  if ( flagRecoToTrue == 1 && flagTrueToReco == 1 ){   // condition for the existence of the relation collections
    

    LCCollection* trkToMcp = evt->getCollection( _recoToTrue ) ;
    LCRelationNavigator navRtT( trkToMcp ) ;
    
    LCCollection* mcpToTrk = evt->getCollection( _trueToReco ) ;
    LCRelationNavigator nav( mcpToTrk ) ;
    LCIterator<MCParticle> mcpIt( evt, _mcParticleCollectionName ) ;
  

    //-----------------------------------------------------------------------------------------------------
    // Creating the MCParticle hitmaps for VXD, SIT and TPC
    //-----------------------------------------------------------------------------------------------------

    typedef std::map< MCParticle* , int > HITMAP ;
    typedef std::map< MCParticle* , std::map<int, int> > HitsInLayersPerMCP;
    
    HITMAP hitMapVXD ;
    HITMAP hitMapSIT ;
    HITMAP hitMapTPC ;
    HITMAP noOfLayersPerMCP ;
    
    HitsInLayersPerMCP HitsInLayersPerMCP_VXD ;
  
    for( unsigned i=0,iN=_simTrkHitCollectionNames.size() ; i<iN ; ++i){

      const LCCollection* col = 0 ;
      try{ col = evt->getCollection( _simTrkHitCollectionNames[i] ) ; } catch(DataNotAvailableException&) {}
      if( col ){
	streamlog_out(DEBUG4) << " We examine collection " << _simTrkHitCollectionNames[i] << " with " << col->getNumberOfElements() << " hits " << std::endl;
	
	if (_simTrkHitCollectionNames[i]=="VXDCollection"){
	  for( int j = 0, jN = col->getNumberOfElements() ; j<jN ; ++j ) {
	    SimTrackerHit* simHit = (SimTrackerHit*) col->getElementAt( j ) ; 
	    MCParticle* mcp = simHit->getMCParticle() ;
	    
	    UTIL::BitField64 encoder( lcio::ILDCellID0::encoder_string ) ;
	    encoder.reset();
	    encoder.setValue(simHit->getCellID0()) ;
	    
	    int layer = 0 ;  
	    layer = encoder[lcio::ILDCellID0::layer] ;
	    
	    streamlog_out(DEBUG2) << " MC particle " << mcp << " creates hit " << simHit << " in layer " << layer << std::endl ;
	    
	    hitMapVXD[ mcp ] ++ ;
	    
	    //HitsInLayers[ layer ] ++ ;
	    
	    HitsInLayersPerMCP_VXD[mcp][layer] ++ ;
	    
	  }
	}
	

	if (_simTrkHitCollectionNames[i]=="SITCollection"){
	  for( int j = 0, jN = col->getNumberOfElements() ; j<jN ; ++j ) {
	    SimTrackerHit* simHit = (SimTrackerHit*) col->getElementAt( j ) ; 
	    MCParticle* mcp = simHit->getMCParticle() ;
	    hitMapSIT[ mcp ] ++ ;
	  }
	}
	if (_simTrkHitCollectionNames[i]=="TPCCollection"){
	  for( int j = 0, jN = col->getNumberOfElements() ; j<jN ; ++j ) {
	    SimTrackerHit* simHit = (SimTrackerHit*) col->getElementAt( j ) ; 
	    MCParticle* mcp = simHit->getMCParticle() ;
	    hitMapTPC[ mcp ] ++ ;
	  }
	}
      }
    }
    
    for( std::map< MCParticle* , std::map<int, int> >::iterator ii=HitsInLayersPerMCP_VXD.begin(); ii!=HitsInLayersPerMCP_VXD.end(); ++ii)
      {
	streamlog_out(DEBUG3) << "TEST: mcp " << (*ii).first <<  " has hitted " << ((*ii).second).size() << " layers "  << std::endl ;
	noOfLayersPerMCP[ (*ii).first ] = ((*ii).second).size() ;
      }
    
    
  
    //_____________________________________________________________________________________________________________________________  

    
    
    
    //----------------------------------------------------------------------------------------------------------------------------
    // One may here apply the cuts in order to define the MC particles subsample taken into consideration 
    //----------------------------------------------------------------------------------------------------------------------------
    
    MCParticleVec mcpTracks ;
    
    while( MCParticle* mcp = mcpIt.next()  ){
      
      streamlog_out(DEBUG2) << " mcparticle id = " << mcp->getPDG() << std::endl;  
      streamlog_out(DEBUG2) << " mcparticle " << mcp << " VXD hits " << hitMapVXD[ mcp ] << " SIT hits " << hitMapSIT[ mcp ] << " TPC hits " << hitMapTPC[ mcp ] << std::endl ;
      
      bool cut = true ;
      //    APPLY_CUT( DEBUG, cut,  std::abs( mcp->getCharge() )  > 0.5  ) ;
      if(  fabs( mcp->getCharge() ) < 0.5 ) cut = false ; // silent cut
      if( cut )
	streamlog_out( DEBUG ) <<  lcshort( mcp ) << std::endl ;
      
      if ( _trkEffOn ) {
	
	//APPLY_CUT( DEBUG, cut,  mcp->getGeneratorStatus() == 1   ) ;   // no documentation lines
	
	gear::Vector3D v( mcp->getVertex()[0], mcp->getVertex()[1], mcp->getVertex()[2] );
	gear::Vector3D e( mcp->getEndpoint()[0], mcp->getEndpoint()[1], mcp->getEndpoint()[2] );
	gear::Vector3D p( mcp->getMomentum()[0], mcp->getMomentum()[1], mcp->getMomentum()[2] );
      
	APPLY_CUT( DEBUG, cut, v.r() < 10.   ) ;   // start at IP+/-10cm
	
	// ==== vzeros =========
	// APPLY_CUT( DEBUG, cut, v.rho() > 100.   ) ;   // non prompt track
	// bool isVzero = false ;
	// if(   mcp->getParents().size() > 0  ) {
	//   isVzero =  ( std::abs( mcp->getParents()[0]->getCharge() )  < 0.1   ) ;
	// }
	// APPLY_CUT( DEBUG, cut,  isVzero    ) ;   // start at IP+/-10cm
	// ==== vzeros =========
	
	//APPLY_CUT( DEBUG, cut, e.rho()==0.0  || e.rho() > 400.   ) ; // end at rho > 40 cm
	
	//APPLY_CUT( DEBUG, cut, p.rho() > 0.3 ) ; //FIXME 1. Gev <->  pt> 100 MeV
	
	//APPLY_CUT( DEBUG, cut, pt > 0.3 ) ; 
	
	APPLY_CUT( DEBUG, cut, fabs( cos( p.theta() ) )  < 0.9  ) ; //FIXME 0.9 <=> .99  //  | cos( theta ) | > 0.99

	APPLY_CUT( DEBUG, cut, !(mcp->isDecayedInTracker())) ;
	
	//APPLY_CUT( DEBUG, cut, hitMapVXD[ mcp ]  > 5  && noOfLayersPerMCP[ mcp ] > 5  ) ; //  Accept as examined track subsample the MCParticles that creates certain number of hits to each subdetector
	
	//APPLY_CUT( DEBUG, cut, hitMapVXD[ mcp ]  > 5  && hitMapSIT[ mcp ] > 3  ) ; 
	
	//....
	
      } 
      
      if( cut ) {
	mcpTracks.push_back( mcp ) ;
      streamlog_out(DEBUG4) << " mcparticle that survives cuts " << mcp << std::endl;  
      //streamlog_out(DEBUG4) << " mcparticle " << mcp << " VXD hits " << hitMapVXD[ mcp ] << " SIT hits " << hitMapSIT[ mcp ] << " TPC hits " << hitMapTPC[ mcp ] << std::endl ;
      streamlog_out(DEBUG4) << " mcparticle " << mcp << " makes hits in " << noOfLayersPerMCP[ mcp ] << " VXD layers " << std::endl ;
      }
    }
    //_________________________________________________________________________________________________________________________________________________
    
    




    streamlog_out(DEBUG4) << "  number of mcparticles surviving the cuts " << mcpTracks.size() << std::endl ;
    
    for(int ii=0,nn=mcpTracks.size() ; ii<nn ; ++ii) {

      const EVENT::LCObjectVec& trkvec = navRtT.getRelatedFromObjects(mcpTracks[ii]);
      FloatVec testFromWgt = navRtT.getRelatedFromWeights(mcpTracks[ii]);
      

      double ptmcp = 0;    
      double pxmcp =  mcpTracks[ii]->getMomentum()[0]  ;
      double pymcp =  mcpTracks[ii]->getMomentum()[1]  ;
      ptmcp = sqrt( pxmcp*pxmcp + pymcp*pymcp ) ;
      
      hist_pt_t->Fill(ptmcp);
      
      streamlog_out(DEBUG4) << " Checking: pt mcp = " << ptmcp << " of particle " << mcpTracks[ii] << " no of tracks associated " << testFromWgt.size() << std::endl ;
      
      int foundFlag = 0 ;
      
      for (int jj=0;jj<trkvec.size();jj++){

	int IPFlag = 0 ; 
      
	FloatVec testWgt = nav.getRelatedToWeights(mcpTracks[ii]);
	
	int SiHits = ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[0] + ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[2];  // *2 cause there are spacepoints

	int SITHits = ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[2];

	int TotalHits = ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[0] + ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[2] + ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[6] ;
	int TotalHitsInFit = ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[1] + ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[3] + ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[7] ;

	streamlog_out(DEBUG4) << " MCParticle " << mcpTracks[ii] << " index " << jj << " track " << trkvec[jj] << " purity " << testFromWgt[jj] << " Si hits " << SiHits << " SIT hits " << SITHits << " total hits " << TotalHits << " Total hits in fit " << TotalHitsInFit << std::endl ;
	streamlog_out(DEBUG4) << " Reco track " << ((Track*)trkvec[jj]) << "  Pt " << (fabs(1./(((Track*)trkvec[jj])->getOmega()))/1000.0) << " Inverse relative weight to MC particle "  << testWgt[jj] << " purity " << testFromWgt[jj] <<  " Silicon hits " << SiHits << std::endl ;
	
	InvWgt.push_back(testWgt[jj]);
	Wgt.push_back(testFromWgt[jj]);

	//------------------------------------------------------------
	// Innermost VXD hit requirement

	TrackerHitVec trkHits = ((Track*)trkvec[jj])->getTrackerHits() ;
	EVENT::TrackerHitVec::iterator it = trkHits.begin();
	
	for( it = trkHits.begin() ; it != trkHits.end() ; ++it ){
	  
	  EVENT::TrackerHit *TestHit = *it ;
	  
	  UTIL::BitField64 encoder( lcio::ILDCellID0::encoder_string ) ;
	  encoder.reset();
	  encoder.setValue(TestHit->getCellID0()) ;
	  
	  int layer = 0 ;  int det_id = 0 ;
	  layer = encoder[lcio::ILDCellID0::layer] ;
	  det_id  = encoder[lcio::ILDCellID0::subdet] ;
	  
	  // Checking propagation from SIT to VXD
	  if ( det_id == lcio::ILDDetID::VXD && layer == 0 ){
	    
	    streamlog_out(DEBUG4) << " Hit on the innermost layer found " << std::endl;
	    IPFlag = 1 ;
	  }

	}
	//______________________________________________________________________________
	
	if ( _trkEffOn ) {
	  if ( testFromWgt[jj] > 0.74 && SiHits > 3 ) { foundFlag = 1; }
	  //if ( testWgt[jj] > 0.74 ) { foundFlag = 1; }  // For testing SIT track segment reconstruction efficiency
	}
	
	else {
	  if ( testFromWgt[jj] > 0.74 ) { foundFlag = 1; }
	}
	
	HelixClass helix ;
	
	float pos[3] ;
	pos[0] = mcpTracks[ii]->getVertex()[0] ;
	pos[1] = mcpTracks[ii]->getVertex()[1] ;
	pos[2] = mcpTracks[ii]->getVertex()[2] ;
	float mom[3] ;
	mom[0] = mcpTracks[ii]->getMomentum()[0] ;
	mom[1] = mcpTracks[ii]->getMomentum()[1] ;
	mom[2] = mcpTracks[ii]->getMomentum()[2] ;
      
	gear::Vector3D p2( mcpTracks[ii]->getMomentum()[0], mcpTracks[ii]->getMomentum()[1], mcpTracks[ii]->getMomentum()[2] );

	//double costhmcp  = cos( p2.theta() ) ;
	float q = mcpTracks[ii]->getCharge() ;
	helix.Initialize_VP( pos , mom, q,  _bField ) ;
	double d0mcp = helix.getD0() ;
	double phmcp = helix.getPhi0() ;
	double ommcp = helix.getOmega() ;
	double z0mcp = helix.getZ0() ;
	double tLmcp = helix.getTanLambda() ;


	if (foundFlag==1){
	  
	  MarlinTrkMap[((Track*)trkvec[jj])] ++ ;
	  
	  TrackSiHits.push_back(SiHits);

	  double rec_d0 = ((Track*)trkvec[jj])->getD0() ;
	  double rec_d0_err = ((Track*)trkvec[jj])->getCovMatrix()[0] ;
	  double rec_z0 = ((Track*)trkvec[jj])->getZ0() ;
	  double rec_z0_err = ((Track*)trkvec[jj])->getCovMatrix()[9] ;
	  double rec_omega = ((Track*)trkvec[jj])->getOmega();
	  double rec_omega_error = ((Track*)trkvec[jj])->getCovMatrix()[5];
	  double rec_phi = ((Track*)trkvec[jj])->getPhi();
	  double rec_phi_error = ((Track*)trkvec[jj])->getCovMatrix()[2];
	  double rec_tanlambda = ((Track*)trkvec[jj])->getTanLambda() ;
	  double rec_tanlambda_err = ((Track*)trkvec[jj])->getCovMatrix()[14] ;
	  float recoPt = fabs(((3.0/10000.0)*_bField)/(((Track*)trkvec[jj])->getOmega())) ;
	  float recoP = recoPt * sqrt(1+(((Track*)trkvec[jj])->getTanLambda())*(((Track*)trkvec[jj])->getTanLambda()));
	  
	  trueD0.push_back(d0mcp);
	  trueZ0.push_back(z0mcp);
	  trueOmega.push_back(ommcp);
	  truePhi.push_back(phmcp);
	  trueTanLambda.push_back(tLmcp);
	  recoD0.push_back(((Track*)trkvec[jj])->getD0());
	  recoD0error.push_back(((Track*)trkvec[jj])->getCovMatrix()[0]);
	  recoZ0.push_back(((Track*)trkvec[jj])->getZ0());
	  recoZ0error.push_back(((Track*)trkvec[jj])->getCovMatrix()[9]);
	  recoOmega.push_back(((Track*)trkvec[jj])->getOmega());
	  recoOmegaError.push_back(((Track*)trkvec[jj])->getCovMatrix()[5]);
	  recoPhi.push_back(((Track*)trkvec[jj])->getPhi());
	  recoPhiError.push_back(((Track*)trkvec[jj])->getCovMatrix()[2]);
	  recoTanLambda.push_back(((Track*)trkvec[jj])->getTanLambda());
	  recoTanLambdaError.push_back(((Track*)trkvec[jj])->getCovMatrix()[14]);
	  OmegaPull->Fill((rec_omega-ommcp)/(sqrt(rec_omega_error)));
	  PhiPull->Fill((rec_phi-phmcp)/(sqrt(rec_phi_error)));
	  TanLambdaPull->Fill((rec_tanlambda-tLmcp)/(sqrt(rec_tanlambda_err)));
	  d0pull->Fill((rec_d0-d0mcp)/(sqrt(rec_d0_err)));
	  z0pull->Fill((rec_z0-z0mcp)/(sqrt(rec_z0_err)));
	  PtMCP.push_back(p2.rho());
	  PtReco.push_back(recoPt);
	  hist_pt_f->Fill(ptmcp);

	  double foundTrkChi2 = ((Track*)trkvec[jj])->getChi2();
	  double foundTrkNdf = ((Track*)trkvec[jj])->getNdf();
	  
	  if ( foundTrkNdf != 0 ) { foundTrkChi2OverNdof.push_back( foundTrkChi2/foundTrkNdf ) ; }
	  //else { streamlog_out(DEBUG4) << " What da heel wazzit? " << std::endl ; }	  
	  
	}
	
	
	if (foundFlag==1)
	  break;
	
	
      }
      
      foundTrk.push_back(foundFlag);
      
      if (foundFlag==1) { streamlog_out(DEBUG4) << " Particle " << mcpTracks[ii] << " which makes " <<  hitMapVXD[ mcpTracks[ii] ] + hitMapSIT[ mcpTracks[ii] ] +  hitMapTPC[ mcpTracks[ii] ]<< " hits, found " << " and " <<  hitMapVXD[ mcpTracks[ii] ] << " VXD hits " << std::endl ; }
      if (foundFlag==0) { streamlog_out(DEBUG4) << " Particle " << mcpTracks[ii] << " which makes " <<  hitMapVXD[ mcpTracks[ii] ] + hitMapSIT[ mcpTracks[ii] ] +  hitMapTPC[ mcpTracks[ii] ]<< " hits " << " and " <<  hitMapVXD[ mcpTracks[ii] ] << " VXD hits and " <<  hitMapSIT[ mcpTracks[ii] ] << " SIT hits,  NOT found " << std::endl ; }
      //if (foundFlag==0) { streamlog_out(DEBUG4) << " Particle " << mcpTracks[ii] << " which crosses " <<   noOfLayersPerMCP[ mcp ]  << " layers, NOT found " << std::endl ; }
      
    }  // end loop on tracks
    
    
    for( SiTrackMap::iterator ii=MarlinTrkMap.begin(); ii!=MarlinTrkMap.end(); ++ii)
      {
	cout << " Found silicon tracks " << (*ii).first << ": " << (*ii).second << endl;
	if ( (*ii).second == 1 ) {
	  GhostsPt.push_back( fabs(((3.0/10000.0)*_bField) / ((*ii).first)->getOmega())) ;
	  const EVENT::LCObjectVec& mcprelvec = nav.getRelatedFromObjects((*ii).first);

	  double ghost_Cos_Theta = cos ( atan ( (1.0 / ((*ii).first)->getTanLambda()))) ;
	  ghostCosTheta.push_back( ghost_Cos_Theta ) ;

	  streamlog_out(DEBUG4) << " Adding to the ghosts - bkg track list, track " <<  (*ii).first << " with momentum " << fabs(((3.0/10000.0)*_bField) / ((*ii).first)->getOmega()) << " being related to " << mcprelvec.size() << " MCParticles "  << std::endl ;

	  double ghostTrkChi2 = ((*ii).first)->getChi2();
	  double ghostTrkNdf = ((*ii).first)->getNdf();
	  
	  if ( ghostTrkNdf != 0 ) { ghostTrkChi2OverNdof.push_back( ghostTrkChi2/ghostTrkNdf ) ; }

	  ghostCounter++ ;

	}
      }
    
  }  // condition for the existence of the relation collections
  
  
  EvalTree->Fill(); 
  nEvt++;
  
  cout << " event " << nEvt << std::endl ;
  
}



void Diagnostics::check( LCEvent * evt ) { 

}


void Diagnostics::end(){ 

  // Writing a canvas with the pulls of the track parameters

  pulls->Divide(3,2);
  pulls->cd(1);
  OmegaPull->Draw();
  OmegaPull->Fit("gaus");
  gStyle->SetOptFit(1111);
  pulls->cd(2);
  PhiPull->Draw();
  PhiPull->Fit("gaus");
  gStyle->SetOptFit(1111);
  pulls->cd(3);
  TanLambdaPull->Draw();
  TanLambdaPull->Fit("gaus");
  gStyle->SetOptFit(1111);
  pulls->cd(4);
  d0pull->Draw();
  d0pull->Fit("gaus");
  gStyle->SetOptFit(1111);
  pulls->cd(5);
  z0pull->Draw();
  z0pull->Fit("gaus");
  gStyle->SetOptFit(1111);

  pulls->Write();

  eff->cd();
  gPad->SetLogx() ;

  gpt->Divide( hist_pt_f , hist_pt_t , "v" ) ;
  gpt->SetMarkerColor( kRed ) ;
  gpt->SetLineColor( kRed ) ;
  gpt->GetYaxis()->SetTitle( "#epsilon_{trk}" );
  gpt->GetXaxis()->SetTitle( "p_{t}/GeV" );
  gpt->Draw("AP");


  gpt->Write();
  eff->Write();

  streamlog_out(DEBUG4) << " Number of ghosts being related to an MCParticle " << ghostCounter << std::endl ;

}

