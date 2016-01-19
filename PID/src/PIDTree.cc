#include "PIDTree.h"

#include <UTIL/LCRelationNavigator.h>
#include <UTIL/PIDHandler.h>
#include "TROOT.h"



PIDTree aPIDTree ;


PIDTree::PIDTree() : Processor("PIDTree") {
  
  // modify processor description
  _description = "PIDTree does whatever it does ..." ;

  
  // register steering parameters: name, description, class-variable, default value


  registerInputCollection( LCIO::LCRELATION,
			   "MCTruth2RecoLinkCollectionName" , 
			   "true - reco relation collection"  ,
			   _trueToReco,
			   std::string("MCTruthRecoLink") ) ;


  registerInputCollection( LCIO::LCRELATION,
			   "Reco2MCTruthLinkCollectionName" , 
			   "reco - true relation collection"  ,
			   _recoToTrue,
			   std::string("RecoMCTruthLink") ) ;


  registerInputCollection( LCIO::MCPARTICLE,
			   "MCParticleCollection" , 
			   "Name of the MCParticle input collection"  ,
			   _mcParticleCollectionName ,
			   std::string("MCParticle") ) ;


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
 
  registerInputCollection( LCIO::TRACK,
			   "StudiedTracks" , 
			   "Name of the FullLDC track collection"  ,
			   _trackColName ,
			   std::string("MarlinTrkTracks") ) ;  

}


void PIDTree::init() { 

  streamlog_out(DEBUG) << "   init called  " 
		       << std::endl ;

  // usually a good idea to
  printParameters() ;
  nEvt = 0;

  gROOT->ProcessLine("#include <vector>");
}

void PIDTree::processRunHeader( LCRunHeader* run) { 
    
} 

void PIDTree::processEvent( LCEvent * evt ) { 

  streamlog_out(MESSAGE) << " start processing event " << std::endl;

  if( isFirstEvent() ) { 
    hermTree = new TTree("hermTree","hermTree");
    hermTree->Branch("nMCParticles",&nMCParticles,"nMCParticles/I") ;
    hermTree->Branch("trueP",&trueP) ;
    hermTree->Branch("truePt",&truePt) ;
    hermTree->Branch("trueTheta",&trueTheta) ;
    hermTree->Branch("truePhi",&truePhi) ;
    hermTree->Branch("trueCharge",&trueCharge) ;
    hermTree->Branch("truePDG",&truePDG) ;
    hermTree->Branch("trueMother",&trueMother) ;
    hermTree->Branch("isSeen",&isSeen) ;
    hermTree->Branch("seenP",&seenP) ;
    hermTree->Branch("seenPt",&seenPt) ;
    hermTree->Branch("seenTheta",&seenTheta) ;
    hermTree->Branch("seenPhi",&seenPhi) ;
    hermTree->Branch("seenDEdx",&seenDEdx) ;
    hermTree->Branch("seenCharge",&seenCharge) ;
    hermTree->Branch("basicPDG", &basicPDG) ;
    hermTree->Branch("dEdxPDG",  &dEdxPDG) ;
    hermTree->Branch("showerPDG",&showerPDG) ;
    hermTree->Branch("likeliPDG",&likeliPDG) ;
    hermTree->Branch("lowmomPDG",&lowmomPDG) ;

  }
  
  streamlog_out(DEBUG) << " clearing vectors " << std::endl;
  nMCParticles = 0;
  trueP.clear();  
  truePt.clear();  
  trueTheta.clear();  
  truePhi.clear();  
  trueCharge.clear();  
  truePDG.clear();  
  trueMother.clear();  
  isSeen.clear();  
  seenP.clear();  
  seenPt.clear();  
  seenTheta.clear();  
  seenPhi.clear();  
  seenDEdx.clear();  
  seenCharge.clear();  
  basicPDG.clear();  
  dEdxPDG.clear();  
  showerPDG.clear();  
  likeliPDG.clear();  
  lowmomPDG.clear();  


  streamlog_out(DEBUG) << " iterator and navigator " << std::endl;
  LCIterator<MCParticle> mcpIt( evt, _mcParticleCollectionName ) ;
  streamlog_out(DEBUG) << " got mcpIt " << mcpIt.size() << std::endl;
  LCRelationNavigator mc2recoNav(evt->getCollection( _trueToReco )); 
  streamlog_out(DEBUG) << " got mc2recoNav from " << mc2recoNav.getFromType() << " to " << mc2recoNav.getToType() << std::endl;
  
  //----------------------------------------------------------------------------------------------------------------------------
  // loop over MCParticles
  //----------------------------------------------------------------------------------------------------------------------------
  
  while( MCParticle* mcp = mcpIt.next()  ) {
    
    if (!mcp) continue; 
    
    streamlog_out(DEBUG) << " mcparticle id = " << mcp->getPDG() << ", genstat = " << mcp->getGeneratorStatus() << std::endl;  
    
    bool keep = false;
    // keep stable generator particles 
    // make something more sophisticated here later!
    if (mcp->getGeneratorStatus() == 1) keep = true; 
    
    if (keep) {   
    
      nMCParticles++;
    
      gear::Vector3D v( mcp->getVertex()[0], mcp->getVertex()[1], mcp->getVertex()[2] );
      gear::Vector3D e( mcp->getEndpoint()[0], mcp->getEndpoint()[1], mcp->getEndpoint()[2] );
      gear::Vector3D p( mcp->getMomentum()[0], mcp->getMomentum()[1], mcp->getMomentum()[2] );
    
      streamlog_out(DEBUG) << " start push_back " << std::endl;  
      trueP.push_back(p.r());
      truePt.push_back(p.trans());
      trueTheta.push_back(p.theta());
      truePhi.push_back(p.phi());
      trueCharge.push_back(mcp->getCharge());
      truePDG.push_back(mcp->getPDG());
      streamlog_out(DEBUG) << " before getting parent " << std::endl;  
      if (mcp->getParents()[0]) {
        trueMother.push_back(mcp->getParents()[0]->getPDG());
      }  
      else {
        trueMother.push_back(-1);
      }
    
      streamlog_out(DEBUG) << " get reco particle " << std::endl;  
      const EVENT::LCObjectVec& recovec = mc2recoNav.getRelatedToObjects(mcp);
      streamlog_out(DEBUG) << " recovec has length " << recovec.size() << std::endl;  
      const EVENT::FloatVec& recoweightvec = mc2recoNav.getRelatedToWeights(mcp);
      streamlog_out(DEBUG) << " recoweightvec has length " << recoweightvec.size() << std::endl;  
      double maxtrckweight = 0;
      double maxcaloweight = 0;
      double maxweight = 0;
      int imaxtrckweight = -1;
      int imaxcaloweight = -1;
      int imaxweight = -1;
      double trckweight, caloweight;
      for (unsigned int irel = 0; irel < recovec.size(); irel++) {
        trckweight = double((int(recoweightvec.at(irel))%10000)/1000.);
        caloweight = double((int(recoweightvec.at(irel))/10000)/1000.);
        streamlog_out(DEBUG) << " irel " << irel << ", recoweight = " << int(recoweightvec.at(irel)) 
                             << ", recoweight%10000 = " << trckweight
                             << ", recoweight/10000 = " << caloweight << std::endl;  
        if (trckweight > maxtrckweight) {
          imaxtrckweight = irel;
          maxtrckweight = trckweight;
        }
        if (caloweight > maxcaloweight) {
          imaxcaloweight = irel;
          maxcaloweight = caloweight;
        }
      }
      
      streamlog_out(DEBUG) << " found reco particle for mcp at imaxtrackweight = " << imaxtrckweight << " with weight = " << maxtrckweight << std::endl ;
      streamlog_out(DEBUG) << " found reco particle for mcp at imaxcaloweight = " << imaxcaloweight << " with weight = " << maxcaloweight << std::endl ;
       
      imaxweight = imaxcaloweight;
      maxweight = maxcaloweight;
      if (maxtrckweight > maxcaloweight) {
        imaxweight = imaxtrckweight;
        maxweight = maxtrckweight;
      }  
      if (imaxcaloweight != imaxtrckweight) 
         streamlog_out(WARNING) << " imaxcaloweight != imaxtrckweight, choosing recoparticle with larger fraction" << std::endl ;
        
       
      streamlog_out(MESSAGE) << " found reco particle for mcp at imaxweight = " << imaxweight << " with weight = " << maxweight << std::endl ;
  
    
      isSeen.push_back(maxweight);
      if (imaxweight >= 0) {
    
        ReconstructedParticle* rcp =  (ReconstructedParticle*) recovec.at(imaxweight); 
        gear::Vector3D rp( rcp->getMomentum()[0], rcp->getMomentum()[1], rcp->getMomentum()[2] );
        seenP.push_back(rp.r());
        seenPt.push_back(rp.trans());
        seenTheta.push_back(rp.theta());
        seenPhi.push_back(rp.phi());
        seenCharge.push_back(rcp->getCharge());
    
      // IMPROVE HERE: use directly MCTruthTrackRelation and choose track with larger weight for dE/dx
        const EVENT::TrackVec& trackvec = rcp->getTracks();
        streamlog_out(DEBUG) << " trackvec has length = " << trackvec.size() << std::endl ;
        double dedx = 0;
        for (unsigned int itrack = 0; itrack < trackvec.size(); itrack++) {
          dedx += trackvec.at(itrack)->getdEdx();
          if (itrack == 1) streamlog_out(WARNING) << " found reco particle with ntrack = " << trackvec.size() << " tracks - summing up dEdx " << std::endl ;
        }
        seenDEdx.push_back(dedx);
        
        
      // Particle IDs
        LCCollection* rpcol = evt->getCollection( _pandoraPFOs ) ;
        PIDHandler *pidh = new PIDHandler(rpcol);
         
        basicPDG.push_back(pidh->getParticleID(rcp, pidh->getAlgorithmID("BasicVariablePID")).getPDG());
        dEdxPDG.push_back(pidh->getParticleID(rcp, pidh->getAlgorithmID("dEdxPID")).getPDG());
        showerPDG.push_back(pidh->getParticleID(rcp, pidh->getAlgorithmID("ShowerShapesPID")).getPDG());
        likeliPDG.push_back(pidh->getParticleID(rcp, pidh->getAlgorithmID("LikelihoodPID")).getPDG());
        lowmomPDG.push_back(pidh->getParticleID(rcp, pidh->getAlgorithmID("LowMomMuID")).getPDG());

      } // if reco part
      // IMPROVE HERE: CHECK if track or cluster exists!
      else {
        streamlog_out(WARNING) << "no ReconstructedParticle found for this mcp!" << std::endl ;
        
        seenP.push_back(0);
        seenPt.push_back(0);
        seenTheta.push_back(0);
        seenPhi.push_back(0);
        seenCharge.push_back(0);
        seenDEdx.push_back(0);
        basicPDG.push_back(0);
        dEdxPDG.push_back(0);
        showerPDG.push_back(0);
        likeliPDG.push_back(0);
        lowmomPDG.push_back(0);
         
      }
  
    }  // if gst == 1
  }  // loop over MCPs
  
  
  hermTree->Fill(); 
  nEvt++;
  
  cout << "======================================== event " << nEvt << std::endl ;
  
}



void PIDTree::check( LCEvent * evt ) { 

}


void PIDTree::end(){ 

}



