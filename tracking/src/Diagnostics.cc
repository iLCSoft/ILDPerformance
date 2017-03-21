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

  // Additions for higgsino analysis

  registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
			   "BCALParticles" , 
			   "Name of the BCALParticles input collection"  ,
			   _BCALParticleCollectionName ,
			   std::string("BCALParticles") ) ;

  registerInputCollection( LCIO::CLUSTER,
			   "BCALCLusters" , 
			   "Name of the BCAL clusters input collection"  ,
			   _BCALClusterCollectionName ,
			   std::string("BCALCLusters") ) ;


  registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
                            "PFOs" , 
                            "particle flow objects"  ,
                            _pandoraPFOs ,
                            std::string("PandoraPFOs") ) ;


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
			   "StudiedTracks" , 
			   "Name of the FullLDC track collection"  ,
			   _trackColName ,
			   std::string("MarlinTrkTracks") ) ;  

  registerProcessorParameter("TrkEffOn",
                             "Enables cuts to define the examined track sample",
                             _trkEffOn,
                             bool(false));

  registerProcessorParameter("SiTrkEffOn",
                             "Enables cuts to define the examined track sample",
                             _siTrkEffOn,
                             bool(false));

  registerProcessorParameter("PhysSampleOn",
                             "Enable when we run diagnostics to a simulated physics sample",
                             _physSampleOn,
                             bool(false));

  registerProcessorParameter("CosThetaCut",
                             "Maximum costheta of examined MCParticles sample",
                             _cosTheta,
                             double(0.9));

  registerProcessorParameter("PCut",
                             "Minimum momentum of examined MCParticles sample (in GeV)",
                             _pCut,
                             double(0.3));

  registerProcessorParameter("PTCut",
                             "Minimum transverse momentum of examined MCParticles sample (in GeV)",
                             _ptCut,
                             double(0.3));

  registerProcessorParameter("DistFromIP",
                             "Maximum distance from IP of examined MCParticles sample (in mm)",
                             _originCut,
                             double(10.0));

  registerProcessorParameter("RequiredPurity",
                             "ratio of hits belonging to the dominant MC particle to total number of track hits",
                             _minPurity,
                             double(0.74));

  registerProcessorParameter("RequiredSiHits",
                             "minimum required hits on Si detectors in order to consider the track found",
                             _minSiHits,
                             int(4));

  registerProcessorParameter("ReqInnVXDHit",
                             "Requirement that the innermost VXD hit should be found",
                             _reqInnVXDHit,
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
  //TApplication theApp("tapp", 0, 0);

  PI = (double)acos((double)(-1.0));
  TWOPI = (double)(2.0)*PI;
}

void Diagnostics::processRunHeader( LCRunHeader* run) { 
    
} 

void Diagnostics::processEvent( LCEvent * evt ) { 

  streamlog_out(MESSAGE) << " start processing event " << std::endl;

  if( isFirstEvent() ) { 
    EvalTree = new TTree("EvalTree","EvalTree");
    EvalTree->Branch("foundTrk",&foundTrk) ;
    EvalTree->Branch("BCalParts",&BCalParts,"BCalParts/I") ;
    EvalTree->Branch("BCalCls",&BCalCls,"BCalCls/I") ;
    EvalTree->Branch("pfos",&pfos,"pfos/I") ;
    EvalTree->Branch("PtReco",&PtReco) ;
    EvalTree->Branch("GhostsPt",&GhostsPt) ;
    EvalTree->Branch("SiTrksPt",&SiTrksPt) ;
    EvalTree->Branch("ghostTrkChi2OverNdof",&ghostTrkChi2OverNdof);
    EvalTree->Branch("foundTrkChi2OverNdof",&foundTrkChi2OverNdof);
    EvalTree->Branch("PtMCP",&PtMCP) ;
    EvalTree->Branch("InvWgt",&InvWgt) ;
    EvalTree->Branch("Wgt",&Wgt) ;
    EvalTree->Branch("TrackSiHits",&TrackSiHits) ;
    EvalTree->Branch("VXDHits",&VXDHits) ;
    EvalTree->Branch("SITHits",&SITHits) ;
    EvalTree->Branch("FTDHits",&FTDHits) ;
    EvalTree->Branch("TPCHits",&TPCHits) ;
    EvalTree->Branch("ghost_hits",&ghost_hits) ;
    EvalTree->Branch("MarlinTracks",&MarlinTracks,"MarlinTracks/I") ;
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
    EvalTree->Branch("MarlinTrkHits",&MarlinTrkHits) ;
    EvalTree->Branch("BadTrksD0",&BadTrksD0) ;
    EvalTree->Branch("BadTrksZ0",&BadTrksZ0) ;

    OmegaPull = new TH1F("OmegaPull","Omega pull",100,-10,10);
    PhiPull = new TH1F("PhiPull","Phi pull",100,-10,10);
    TanLambdaPull = new TH1F("TanLambdaPull","TanLambda",100,-10,10);
    d0pull = new TH1F("d0pull","d0 pull",100,-10,10);
    z0pull = new TH1F("z0pull","z0",100,-10,10); 

    OmegaResidual = new TH1F("OmegaResidual","Omega Residual",50,-0.001,0.001);
    PhiResidual = new TH1F("PhiResidual","Phi Residual",50,-0.005,0.005);
    TanLambdaResidual = new TH1F("TanLambdaResidual","TanLambda Residual",50,-0.005,0.005);
    d0Residual = new TH1F("d0Residual","d0 Residual",50,-0.1,0.1);
    z0Residual = new TH1F("z0Residual","z0 Residual",50,-0.1,0.1); 


    double bins[nBins+1] = { 0.01, 0.1, 0.2, 0.3, 0.5 , 0.7 , 1.0 , 2., 5.0 , 10. , 20. , 50. , 100. , 300.  } ;

    hist_pt_t  = new TH1F( "hist_pt_t", "Pt distributions of true tracks", nBins , bins ) ;
    hist_pt_f  = new TH1F( "hist_pt_f", "Pt distribution of found tracks", nBins , bins ) ;

    hist_th_t  = new TH1F( "hist_th_t", "Cos theta distributions of true tracks", 21, -1., 1. ) ;
    hist_th_f  = new TH1F( "hist_th_f", "Cos theta distribution of found tracks", 21, -1., 1. ) ;

    
    pulls = new TCanvas("pulls","Track par. pulls",800,800);
    residuals =  new TCanvas("residuals","Track par. residuals",800,800);
    eff = new TCanvas("eff","Trk Eff",800,800);

    myfunc = new TF1("myfunc","gaus(0)");

    gpt = new TGraphAsymmErrors() ;
    gth = new TGraphAsymmErrors() ;
  }

  foundTrk.clear();  PtReco.clear();  PtMCP.clear();  Wgt.clear();   InvWgt.clear();   TrackSiHits.clear();
  trueD0.clear();    trueZ0.clear();  trueOmega.clear();   truePhi.clear();  trueTanLambda.clear();
  recoD0.clear();    recoZ0.clear();  recoOmega.clear();   recoPhi.clear();  recoTanLambda.clear();
  recoD0Error.clear();    recoZ0Error.clear();  recoOmegaError.clear();   recoPhiError.clear();  recoTanLambdaError.clear();
  GhostsPt.clear();  
  foundTrkChi2OverNdof.clear();    ghostTrkChi2OverNdof.clear();  ghostCosTheta.clear();
  MarlinTrkHits.clear();   VXDHits.clear();  SITHits.clear();  FTDHits.clear();  TPCHits.clear();
  BadTrksZ0.clear();  BadTrksD0.clear(); ghost_hits.clear();


  typedef std::map< Track* , int> SiTrackMap;
  //SiTrackMap TrackMap ;
  SiTrackMap MarlinTrkMap ;

  int flagTrack = 0; int flagTrueReco = 0;  int flagSiRel = 0; int flagRecoToTrue = 0 ; int  flagTrueToReco = 0 ;
  int  flagBCALPart = 0 ;     int  flagBCALCluster = 0 ;   int flagPFO = 0 ;

  const StringVec*  colNames = evt->getCollectionNames() ;
  for( StringVec::const_iterator it = colNames->begin() ; it != colNames->end() ; it++ ){

    if  ( _trackColName == *it )
      flagTrack = 1 ;

    if  ( _trueToReco  == *it )
      flagTrueReco = 1 ; 

    if  ( _vxdTrkHitRelations == *it )
      flagSiRel = 1 ;

    if  ( _recoToTrue == *it )
      flagRecoToTrue = 1 ;

    if  ( _trueToReco == *it )
      flagTrueToReco = 1 ;

    if (  _BCALParticleCollectionName == *it )
      flagBCALPart = 1 ;

    if (  _BCALClusterCollectionName == *it )
      flagBCALCluster = 1 ;

    if ( _pandoraPFOs  == *it )
      flagPFO = 1 ;

  }


  if ( flagTrack == 1 ){
    LCCollection* MarlinTrks = evt->getCollection( _trackColName );
    MarlinTracks = MarlinTrks->getNumberOfElements();

    for ( int ii = 0; ii < MarlinTracks; ii++ ) {

      Track *MarlinRecoTrack = dynamic_cast<Track*>( MarlinTrks->getElementAt( ii ) ) ;
      MarlinTrkMap[MarlinRecoTrack] ++ ;
      MarlinTrkHits.push_back(MarlinRecoTrack->getTrackerHits().size());
      FTDHits.push_back(MarlinRecoTrack->getSubdetectorHitNumbers()[4] );
      VXDHits.push_back(MarlinRecoTrack->getSubdetectorHitNumbers()[0] );
      TPCHits.push_back(MarlinRecoTrack->getSubdetectorHitNumbers()[6] );
      SITHits.push_back(MarlinRecoTrack->getSubdetectorHitNumbers()[2] );
    }
  }


  if (flagBCALPart == 1 ){
    LCCollection* BCalParty = evt->getCollection( _BCALParticleCollectionName );
    BCalParts = BCalParty->getNumberOfElements();
  }

  if (flagBCALCluster == 1 ){
    LCCollection* BCalClassy = evt->getCollection( _BCALParticleCollectionName );
    BCalCls = BCalClassy->getNumberOfElements();
  }
  
  if (flagPFO == 1 ){
    LCCollection* Particles = evt->getCollection( _pandoraPFOs );
    pfos = Particles->getNumberOfElements();
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
	    
	    UTIL::BitField64 encoder( lcio::LCTrackerCellID::encoding_string() ) ;
	    encoder.reset();
	    encoder.setValue(simHit->getCellID0()) ;
	    
	    int layer = 0 ;  
	    layer = encoder[lcio::LCTrackerCellID::layer()] ;
	    
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

	if ( _physSampleOn ){
	
	  APPLY_CUT( DEBUG, cut,  mcp->getGeneratorStatus() == 1   ) ;   // no documentation lines

	}
	
	gear::Vector3D v( mcp->getVertex()[0], mcp->getVertex()[1], mcp->getVertex()[2] );
	gear::Vector3D e( mcp->getEndpoint()[0], mcp->getEndpoint()[1], mcp->getEndpoint()[2] );
	gear::Vector3D p( mcp->getMomentum()[0], mcp->getMomentum()[1], mcp->getMomentum()[2] );
      	
	APPLY_CUT( DEBUG, cut, v.r() < _originCut   ) ;   // start at IP+/-10cm
	
	// ==== vzeros =========
	// APPLY_CUT( DEBUG, cut, v.rho() > 100.   ) ;   // non prompt track
	// bool isVzero = false ;
	// if(   mcp->getParents().size() > 0  ) {
	//   isVzero =  ( std::abs( mcp->getParents()[0]->getCharge() )  < 0.1   ) ;
	// }
	// APPLY_CUT( DEBUG, cut,  isVzero    ) ;   // start at IP+/-10cm
	// ==== vzeros =========
	
	//APPLY_CUT( DEBUG, cut, e.rho()==0.0  || e.rho() > 400.   ) ; // end at rho > 40 cm
	
	APPLY_CUT( DEBUG, cut, p.r() > _pCut ) ; 
	
	APPLY_CUT( DEBUG, cut, p.rho() > _ptCut ) ; 
	
	APPLY_CUT( DEBUG, cut, fabs( cos( p.theta() ) )  < _cosTheta  ) ; //FIXME 0.9 <=> .99  //  | cos( theta ) | > 0.99

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
      double pzmcp =  mcpTracks[ii]->getMomentum()[2]  ;
      ptmcp = sqrt( pxmcp*pxmcp + pymcp*pymcp ) ;

      hist_pt_t->Fill(ptmcp);

      TVector3 p( pxmcp, pymcp,  pzmcp ) ;
      double costhmcp  = fabs(cos( p.Theta() )) ;

      if (fabs(mcpTracks[ii]->getCharge())<10.){
	hist_th_t->Fill(costhmcp);
      }

      
      streamlog_out(DEBUG4) << " Checking: pt mcp = " << ptmcp << " of particle " << mcpTracks[ii] << " no of tracks associated " << testFromWgt.size() << std::endl ;
      
      int foundFlag = 0 ;
      
      for (unsigned int jj=0;jj<trkvec.size();jj++){

	int IPFlag = 0 ; 
      
	FloatVec testWgt = nav.getRelatedToWeights(mcpTracks[ii]);
	
	int SiHits = ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[0] + ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[2];  // *2 cause there are spacepoints

	int TotalHits = ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[0] + ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[2] + ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[6] ;
	int TotalHitsInFit = ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[1] + ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[3] + ((Track*)trkvec[jj])->getSubdetectorHitNumbers()[7] ;

	streamlog_out(DEBUG4) << " MCParticle " << mcpTracks[ii] << " index " << jj << " track " << trkvec[jj] << " purity " << testFromWgt[jj] << " Silicon hits " << SiHits << " total hits " << TotalHits << " Total hits in fit " << TotalHitsInFit << std::endl ;
	streamlog_out(DEBUG4) << " Reco track " << ((Track*)trkvec[jj]) << "  Pt " << (fabs(1./(((Track*)trkvec[jj])->getOmega()))/1000.0) << " Inverse relative weight to MC particle "  << testWgt[jj] << " purity " << testFromWgt[jj] <<  " Silicon hits " << SiHits << std::endl ;
	
	InvWgt.push_back(testWgt[jj]);
	Wgt.push_back(testFromWgt[jj]);

	//------------------------------------------------------------
	// Innermost VXD hit requirement

	TrackerHitVec trkHits = ((Track*)trkvec[jj])->getTrackerHits() ;
	EVENT::TrackerHitVec::iterator it = trkHits.begin();
	
	for( it = trkHits.begin() ; it != trkHits.end() ; ++it ){
	  
	  EVENT::TrackerHit *TestHit = *it ;
	  
	  UTIL::BitField64 encoder( lcio::LCTrackerCellID::encoding_string() ) ;
	  encoder.reset();
	  encoder.setValue(TestHit->getCellID0()) ;
	  
	  int layer = 0 ;  int det_id = 0 ;
	  layer = encoder[lcio::LCTrackerCellID::layer()] ;
	  det_id  = encoder[lcio::LCTrackerCellID::subdet()] ;
	  
	  // Checking propagation from SIT to VXD
	  if ( det_id == lcio::ILDDetID::VXD && layer == 0 ){
	    
	    streamlog_out(DEBUG4) << " Hit on the innermost layer found " << std::endl;
	    IPFlag = 1 ;
	  }

	}
	//______________________________________________________________________________
	
	if ( _siTrkEffOn ) {

	  if ( _reqInnVXDHit ){
	    if ( testFromWgt[jj] > _minPurity && SiHits >= _minSiHits && IPFlag==1 ) { foundFlag = 1; }
	  }
	  else {
	  if ( testFromWgt[jj] > _minPurity && SiHits >= _minSiHits ) { foundFlag = 1; }
	  }
	}
	else {
	  if ( testFromWgt[jj] > _minPurity ) { foundFlag = 1; }
	  streamlog_out(DEBUG4) << " I use that cut " << std::endl;
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
	  // double rec_phi = ((Track*)trkvec[jj])->getPhi();
	  double rec_phi_error = ((Track*)trkvec[jj])->getCovMatrix()[2];
	  double rec_tanlambda = ((Track*)trkvec[jj])->getTanLambda() ;
	  double rec_tanlambda_err = ((Track*)trkvec[jj])->getCovMatrix()[14] ;
	  float recoPt = fabs(((3.0/10000.0)*_bField)/(((Track*)trkvec[jj])->getOmega())) ;

	  
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

	  double testPhi = ((Track*)trkvec[jj])->getPhi();
	  if (testPhi<0) testPhi = testPhi+TWOPI;

	  recoPhi.push_back(testPhi);
	  recoPhiError.push_back(((Track*)trkvec[jj])->getCovMatrix()[2]);
	  recoTanLambda.push_back(((Track*)trkvec[jj])->getTanLambda());
	  recoTanLambdaError.push_back(((Track*)trkvec[jj])->getCovMatrix()[14]);
	  OmegaPull->Fill((rec_omega-ommcp)/(sqrt(rec_omega_error)));
	  PhiPull->Fill((testPhi-phmcp)/(sqrt(rec_phi_error)));
	  TanLambdaPull->Fill((rec_tanlambda-tLmcp)/(sqrt(rec_tanlambda_err)));
	  d0pull->Fill((rec_d0-d0mcp)/(sqrt(rec_d0_err)));
	  z0pull->Fill((rec_z0-z0mcp)/(sqrt(rec_z0_err)));
	  OmegaResidual->Fill(rec_omega-ommcp);
	  PhiResidual->Fill(testPhi-phmcp);
	  TanLambdaResidual->Fill(rec_tanlambda-tLmcp);
	  d0Residual->Fill(rec_d0-d0mcp);
	  z0Residual->Fill(rec_z0-z0mcp);
	  PtMCP.push_back(p2.rho());
	  PtReco.push_back(recoPt);
	  hist_pt_f->Fill(ptmcp);

	  //if (recoP>1.){
	  //hist_th_f->Fill(cos ( atan ( (1.0 / rec_tanlambda )) ));
	  hist_th_f->Fill(costhmcp);
	    //}
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
	streamlog_out(DEBUG4) << " Found silicon tracks " << (*ii).first << ": " << (*ii).second << endl;
	if ( (*ii).second == 1 ) {

	  const EVENT::LCObjectVec& mcprelvec = nav.getRelatedFromObjects((*ii).first);

	  double ghost_Cos_Theta = cos ( atan ( (1.0 / ((*ii).first)->getTanLambda()))) ;

	  TrackerHitVec ghostTrkHits = ((*ii).first)->getTrackerHits();
	  ghost_hits.push_back(ghostTrkHits.size());

	  if (ghost_Cos_Theta < _cosTheta ) {
	    streamlog_out(DEBUG4) << " Adding to the ghosts - bkg track list, track " <<  (*ii).first << " with momentum " << fabs(((3.0/10000.0)*_bField) / ((*ii).first)->getOmega()) << " & no of hits " <<  ghostTrkHits.size() << " being related to " << mcprelvec.size() << " MCParticles "  << std::endl ;
	    
	    GhostsPt.push_back( fabs(((3.0/10000.0)*_bField) / ((*ii).first)->getOmega())) ;
	    ghostCosTheta.push_back( ghost_Cos_Theta ) ;

	    double ghostTrkChi2 = ((*ii).first)->getChi2();
	    double ghostTrkNdf = ((*ii).first)->getNdf();
	    BadTrksD0.push_back(((*ii).first)->getD0());
	    BadTrksZ0.push_back(((*ii).first)->getZ0());
	    
	    
	    if ( ghostTrkNdf != 0 ) { ghostTrkChi2OverNdof.push_back( ghostTrkChi2/ghostTrkNdf ) ; }
	    
	    ghostCounter++ ;
	    
	  }
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

  //TApplication theApp("tapp", 0, 0);
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

  residuals->Divide(3,2);
  residuals->cd(1);
  OmegaResidual->Draw();

  residuals->cd(2);
  PhiResidual->Draw();

  residuals->cd(3);
  TanLambdaResidual->Draw();

  residuals->cd(4);
  d0Residual->Draw();

  residuals->cd(5);
  z0Residual->Draw();

  residuals->Write();


  eff->Divide(1,2);

  eff->cd(1);
  gPad->SetLogx() ;
  gpt->Divide( hist_pt_f , hist_pt_t , "v" ) ;
  gpt->SetMarkerColor( kRed ) ;
  gpt->SetLineColor( kRed ) ;
  gpt->GetYaxis()->SetTitle( "#epsilon_{trk}" );
  gpt->GetXaxis()->SetTitle( "p_{t}/GeV" );
  gpt->Draw("AP");
  
  eff->cd(2);
  //gPad->SetLogx() ;
  gth->Divide( hist_th_f , hist_th_t , "v" ) ;
  gth->SetMarkerColor( kRed ) ;
  gth->SetLineColor( kRed ) ;
  gth->GetYaxis()->SetTitle( "#epsilon_{trk}" );
  gth->GetXaxis()->SetTitle( "cos(#theta)" );
  gth->Draw("AP");
  

  gpt->Write("gpt");
  gth->Write("gth");
  eff->Write();

  streamlog_out(DEBUG4) << " Number of ghosts being related to an MCParticle " << ghostCounter << std::endl ;
  /*
  TApplication theApp("tapp", 0, 0);

  TControlBar bar2("vertical");
  bar2.AddButton("Residuals","residuals->Draw()", "...  >>Residuals<<");
  bar2.Show();
  theApp.Run();
  //theApp.SetReturnFromRun(true);
  */
}



