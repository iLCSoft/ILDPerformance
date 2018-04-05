#include "PIDTree.h"

#include <UTIL/LCRelationNavigator.h>
#include <UTIL/PIDHandler.h>
#include "TROOT.h"

#include <math.h>

#include "DDRec/Vector3D.h"

PIDTree aPIDTree ;

using namespace lcio ;
//using namespace marlin ;
//using namespace std ;

PIDTree::PIDTree() : Processor("PIDTree") {
  
  // modify processor description
  _description = "PIDTree create a ROOT TTree with PID parameters" ;

  
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


  registerInputCollection( LCIO::LCRELATION,
			   "MCTruth2TrackLinkCollectionName" , 
			   "true - track relation collection"  ,
			   _trueToTrack,
			   std::string("MCTruthMarlinTrkTracksLink") ) ;


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

  // usually a good idea tomcp->getPDG()
  printParameters() ;
  nEvt = 0;

  gROOT->ProcessLine("#include <vector>");
}

void PIDTree::processRunHeader( LCRunHeader*) { 
    
} 

void PIDTree::processEvent( LCEvent * evt ) { 

  streamlog_out(DEBUG3)  << "=================================  start processing event " << nEvt << std::endl ;

  if( isFirstEvent() ) { 
    hermTree = new TTree("hermTree","hermTree");
    hermTree->Branch("nMCParticles",&nMCParticles,"nMCParticles/I") ;
    hermTree->Branch("trueP",&trueP) ;
    hermTree->Branch("trueMass",&trueMass) ;
    hermTree->Branch("truePt",&truePt) ;
    hermTree->Branch("trueTheta",&trueTheta) ;
    hermTree->Branch("truePhi",&truePhi) ;
    hermTree->Branch("trueCharge",&trueCharge) ;
    hermTree->Branch("truePDG",&truePDG) ;
    hermTree->Branch("trueMother",&trueMother) ;
    hermTree->Branch("isTrue",&isTrue) ;
    hermTree->Branch("isSeen",&isSeen) ;
    hermTree->Branch("isSeenDEdx",&isSeenDEdx) ;
    hermTree->Branch("seenP",&seenP) ;
    hermTree->Branch("seenPt",&seenPt) ;
    hermTree->Branch("seenTheta",&seenTheta) ;
    hermTree->Branch("seenPhi",&seenPhi) ;
    hermTree->Branch("seenDEdx",&seenDEdx) ;
    hermTree->Branch("seenDEdxErr",&seenDEdxErr) ;
    hermTree->Branch("seenCharge",&seenCharge) ;
    hermTree->Branch("basicPDG", &basicPDG) ;
    hermTree->Branch("dEdxPDG",  &dEdxPDG) ;
    hermTree->Branch("showerPDG",&showerPDG) ;
    hermTree->Branch("likeliPDG",&likeliPDG) ;
    hermTree->Branch("lowmomPDG",&lowmomPDG) ;
    hermTree->Branch("dedxdist_el",&dedxdist_el) ;
    hermTree->Branch("dedxdist_mu",&dedxdist_mu) ;
    hermTree->Branch("dedxdist_pi",&dedxdist_pi) ;
    hermTree->Branch("dedxdist_ka",&dedxdist_ka) ;
    hermTree->Branch("dedxdist_pr",&dedxdist_pr) ;
    hermTree->Branch("LiPDG_el",&LiPDG_el) ;
    hermTree->Branch("LiPDG_mu",&LiPDG_mu) ;
    hermTree->Branch("LiPDG_pi",&LiPDG_pi) ;
    hermTree->Branch("LiPDG_ka",&LiPDG_ka) ;
    hermTree->Branch("LiPDG_pr",&LiPDG_pr) ;
    hermTree->Branch("dedxPDG_el",&dedxPDG_el) ;
    hermTree->Branch("dedxPDG_mu",&dedxPDG_mu) ;
    hermTree->Branch("dedxPDG_pi",&dedxPDG_pi) ;
    hermTree->Branch("dedxPDG_ka",&dedxPDG_ka) ;
    hermTree->Branch("dedxPDG_pr",&dedxPDG_pr) ;

  }
  
  streamlog_out(DEBUG) << " clearing vectors " << std::endl;
  nMCParticles = 0;
  trueP.clear();  
  trueMass.clear();  
  truePt.clear();  
  trueTheta.clear();  
  truePhi.clear();  
  trueCharge.clear();  
  truePDG.clear();  
  trueMother.clear();  
  isTrue.clear();  
  isSeen.clear();  
  isSeenDEdx.clear();  
  seenP.clear();  
  seenPt.clear();  
  seenTheta.clear();  
  seenPhi.clear();  
  seenDEdx.clear();  
  seenDEdxErr.clear();  
  seenCharge.clear();  
  basicPDG.clear();  
  dEdxPDG.clear();  
  showerPDG.clear();  
  likeliPDG.clear();  
  lowmomPDG.clear(); 
  LiPDG_el.clear(); 
  LiPDG_mu.clear(); 
  LiPDG_pi.clear(); 
  LiPDG_ka.clear(); 
  LiPDG_pr.clear();  
  dedxdist_el.clear(); 
  dedxdist_mu.clear(); 
  dedxdist_pi.clear(); 
  dedxdist_ka.clear(); 
  dedxdist_pr.clear();  
  dedxPDG_el.clear(); 
  dedxPDG_mu.clear(); 
  dedxPDG_pi.clear(); 
  dedxPDG_ka.clear(); 
  dedxPDG_pr.clear();  

  streamlog_out(DEBUG) << " iterator and navigator " << std::endl;
  LCIterator<MCParticle> mcpIt( evt, _mcParticleCollectionName ) ;
  streamlog_out(DEBUG) << " got mcpIt " << mcpIt.size() << std::endl;
  LCRelationNavigator mc2recoNav(evt->getCollection( _trueToReco )); 
  streamlog_out(DEBUG) << " got mc2recoNav from " << mc2recoNav.getFromType() << " to " << mc2recoNav.getToType() << std::endl;
  LCRelationNavigator reco2mcNav(evt->getCollection( _recoToTrue )); 
  streamlog_out(DEBUG) << " got reco2mcNav from " << reco2mcNav.getFromType() << " to " << reco2mcNav.getToType() << std::endl;
  LCRelationNavigator mc2trackNav(evt->getCollection( _trueToTrack )); 
  streamlog_out(DEBUG) << " got mc2trackNav from " << mc2trackNav.getFromType() << " to " << mc2trackNav.getToType() << std::endl;
  
  //----------------------------------------------------------------------------------------------------------------------------
  // loop over MCParticles
  //----------------------------------------------------------------------------------------------------------------------------
  
  int imcp = -1;
  while( MCParticle* mcp = mcpIt.next()  ) {
    
    if (!mcp) continue; 
    
    imcp++;
    if (mcp->getGeneratorStatus() == 1) {
      streamlog_out(DEBUG3) << "=========================================================================================" << std::endl;  
    }
    streamlog_out(DEBUG3) << " mcparticle " << imcp << " has PDG = " << mcp->getPDG() << " and genstat = " << mcp->getGeneratorStatus() << std::endl;  
    
    bool keep = false;
    // keep stable generator particles 
    // make something more sophisticated here later!
    if (mcp->getGeneratorStatus() == 1) keep = true; 
    
    if (keep) {   
    
      nMCParticles++;
    
      dd4hep::rec::Vector3D v( mcp->getVertex()[0], mcp->getVertex()[1], mcp->getVertex()[2] );
      dd4hep::rec::Vector3D e( mcp->getEndpoint()[0], mcp->getEndpoint()[1], mcp->getEndpoint()[2] );
      dd4hep::rec::Vector3D p( mcp->getMomentum()[0], mcp->getMomentum()[1], mcp->getMomentum()[2] );
    
      streamlog_out(DEBUG) << " start push_back " << std::endl;  
      trueP.push_back(p.r());
      trueMass.push_back( mcp->getMass() ) ;
      truePt.push_back(p.trans());
      trueTheta.push_back(p.theta());
      truePhi.push_back(p.phi());
      trueCharge.push_back(mcp->getCharge());
      truePDG.push_back(mcp->getPDG());
      streamlog_out(DEBUG) << " before getting parent " << std::endl;  
      if ( mcp->getParents().size() > 0 ) {
        trueMother.push_back(mcp->getParents()[0]->getPDG());
      }  
      else {
        trueMother.push_back(-1);
      }
    
      streamlog_out(DEBUG) << " get reco particle " << std::endl;  
      const EVENT::LCObjectVec& recovec = mc2recoNav.getRelatedToObjects(mcp);
      streamlog_out(DEBUG3) << " recovec has length " << recovec.size() << std::endl;  
      const EVENT::FloatVec& recoweightvec = mc2recoNav.getRelatedToWeights(mcp);
      streamlog_out(DEBUG) << " recoweightvec has length " << recoweightvec.size() << std::endl;  
      double maxtrckweight = 0;
      double maxcaloweight = 0;
      int imaxtrckweight = -1;
      int imaxcaloweight = -1;
      double trckweight, caloweight;
      for (unsigned int irel = 0; irel < recovec.size(); irel++) {
        trckweight = double((int(recoweightvec.at(irel))%10000)/1000.);
        caloweight = double((int(recoweightvec.at(irel))/10000)/1000.);
        streamlog_out(DEBUG3) << " irel " << irel << ", recoweight = " << int(recoweightvec.at(irel)) 
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
      
      streamlog_out(DEBUG3) << " found reco particle for mcp at imaxtrackweight = " << imaxtrckweight << " with weight = " << maxtrckweight << std::endl ;
      streamlog_out(DEBUG3) << " found reco particle for mcp at imaxcaloweight = " << imaxcaloweight << " with weight = " << maxcaloweight << std::endl ;
      
      
      // NOW check backwards relation as well:
      // catch cases where a PFO fully contains the MCP, but the MCP is only a small contribution to the PFO                     
      
      double mcptrckweight = -1;
      if ( imaxtrckweight > -1 && imaxtrckweight < (int)recovec.size() ) {
        ReconstructedParticle* rcptrck =  (ReconstructedParticle*) recovec.at(imaxtrckweight); 
        streamlog_out(DEBUG3) << " get MC particle for track " << std::endl;  
        const EVENT::LCObjectVec& mcptrckvec = reco2mcNav.getRelatedToObjects(rcptrck);
        streamlog_out(DEBUG3) << " mcptrckvec has length " << mcptrckvec.size() << std::endl;  
        const EVENT::FloatVec& mcptrckweightvec = reco2mcNav.getRelatedToWeights(rcptrck);
        streamlog_out(DEBUG3) << " mcptrckweightvec has length " << mcptrckweightvec.size() << std::endl; 
      
        // check whether track vector contains original MCP
        for (unsigned imcptrck = 0; imcptrck < mcptrckvec.size(); imcptrck++ ) {
          if ( mcptrckvec.at(imcptrck) == mcp ) { 
            mcptrckweight = double((int(mcptrckweightvec.at(imcptrck))%10000)/1000.);
            streamlog_out(DEBUG3) << " found original MCP in mcptrckvec at position " << imcptrck << " with weight " << mcptrckweight << std::endl; 
          } 
        }
      }  
      else {
        streamlog_out(DEBUG3) << " imaxtrckweight out of range, skipping backwards check " << std::endl;  
      }
      
      double mcpcaloweight = -1;
      if ( imaxcaloweight > -1 && imaxcaloweight < (int)recovec.size() ) {
        ReconstructedParticle* rcpcalo =  (ReconstructedParticle*) recovec.at(imaxcaloweight); 
        streamlog_out(DEBUG3) << " get MC particle for cluster " << std::endl;  
        const EVENT::LCObjectVec& mcpcalovec = reco2mcNav.getRelatedToObjects(rcpcalo);
        streamlog_out(DEBUG3) << " mcpcalovec has length " << mcpcalovec.size() << std::endl;  
        const EVENT::FloatVec& mcpcaloweightvec = reco2mcNav.getRelatedToWeights(rcpcalo);
        streamlog_out(DEBUG3) << " mcpcaloweightvec has length " << mcpcaloweightvec.size() << std::endl;  
      
        // check whether calo vector contains original MCP
        for (unsigned imcpcalo = 0; imcpcalo < mcpcalovec.size(); imcpcalo++ ) {
          if ( mcpcalovec.at(imcpcalo) == mcp ) { 
            mcpcaloweight = double((int(mcpcaloweightvec.at(imcpcalo))/10000)/1000.);
            streamlog_out(DEBUG3) << " found original MCP in mcpcalovec at position " << imcpcalo << " with weight " << mcpcaloweight << std::endl; 
          } 
        }
      }
      else {
        streamlog_out(DEBUG3) << " imaxcaloweight out of range, skipping backwards check " << std::endl;  
      }
       
      
      // give precedence to track link if both directions larger than 
      double weightcut = 0.5;
      double maxweight = 0;
      double maxbackweight = 0;
      int imaxweight = -1;
      
      if ( maxtrckweight > weightcut && mcptrckweight > weightcut ) {
        imaxweight = imaxtrckweight;
        maxweight = maxtrckweight;
        maxbackweight = mcptrckweight;
        streamlog_out(DEBUG3) << " found reco particle for mcp with bi-directional track link at imaxweight = " << imaxweight << " with weight = " << maxweight << std::endl ;
      }
      else if ( maxcaloweight > weightcut && mcpcaloweight > weightcut ) {
        imaxweight = imaxcaloweight;
        maxweight = maxcaloweight;
        maxbackweight = mcpcaloweight;
        streamlog_out(DEBUG3) << " found reco particle for mcp with bi-directional calo link at imaxweight = " << imaxweight << " with weight = " << maxweight << std::endl ;
      }
      else if( !recovec.empty() ) { // don't warn about particles that are not reconstructed
        streamlog_out(WARNING) << " neither track nor cluster link have a weight larger than " << weightcut << " in _both_ directions: " << std::endl ;
        streamlog_out(WARNING) << " maxtrckweight =  " << maxtrckweight << ", mcptrckweight = " << mcptrckweight << std::endl ;
        streamlog_out(WARNING) << " maxcaloweight =  " << maxcaloweight << ", mcpcaloweight = " << mcpcaloweight << std::endl ;
      }
        
      isTrue.push_back(maxbackweight);
      isSeen.push_back(maxweight);
      
      if (maxweight > 0 && imaxweight > -1 && imaxweight < (int) recovec.size()) {
    
        ReconstructedParticle* rcp =  (ReconstructedParticle*) recovec.at(imaxweight); 
        dd4hep::rec::Vector3D rp( rcp->getMomentum()[0], rcp->getMomentum()[1], rcp->getMomentum()[2] );
        seenP.push_back(rp.r());
        seenPt.push_back(rp.trans());
        seenTheta.push_back(rp.theta());
        seenPhi.push_back(rp.phi());
        seenCharge.push_back(rcp->getCharge());
    
      // IMPROVE HERE: use directly MCTruthTrackRelation and choose track with larger weight for dE/dx
        const EVENT::LCObjectVec& trackvec = mc2trackNav.getRelatedToObjects(mcp);
        streamlog_out(DEBUG3) << " trackvec has length = " << trackvec.size() << " for true PDG " << mcp->getPDG() << std::endl ;
        const EVENT::FloatVec& trackweightvec = mc2trackNav.getRelatedToWeights(mcp);
        double dedx = 0.;
        double dedxerr = 0.;
        double weightdedx = 0.;
        for ( unsigned itrack = 0; itrack < trackvec.size(); itrack++ ) {
          Track *track = (Track *) trackvec.at(itrack);
          if ( fabs (trackweightvec.at(itrack) - maxtrckweight) < 0.001  ) { 
            streamlog_out(DEBUG3) << " found track with weight " << trackweightvec.at(itrack) << ", filling dE/dx !" << std::endl; 
            dedx = track->getdEdx();
            dedxerr = track->getdEdxError();
            weightdedx = maxtrckweight;
          } 
          else {
            streamlog_out(DEBUG3) << " not taking track with weight " << trackweightvec.at(itrack) << " since difference with maxtrckweight = " << fabs (trackweightvec.at(itrack) - maxtrckweight) << std::endl; 
          }
          if (trackvec.size() > 1) {
            streamlog_out(DEBUG3) << " ===> track " << itrack << " has innermost hit at " << track->getRadiusOfInnermostHit () << std::endl ;
            if ( mcp->getDaughters().size() > 0 ) {
              for ( unsigned idaughter = 0; idaughter < mcp->getDaughters().size(); idaughter++ ) {
                streamlog_out(DEBUG3) << " ===> mcp daughter " << idaughter << " has PDG " << mcp->getDaughters()[idaughter]->getPDG() << std::endl ;
              }
            }
          }
        }
        seenDEdx.push_back(dedx);
        seenDEdxErr.push_back(dedxerr);
        isSeenDEdx.push_back(weightdedx);
               
        
      // Particle IDs
        LCCollection* rpcol = evt->getCollection( _pandoraPFOs ) ;
        PIDHandler *pidh = new PIDHandler(rpcol);
         
        basicPDG.push_back(pidh->getParticleID(rcp, pidh->getAlgorithmID("BasicVariablePID")).getPDG());
        dEdxPDG.push_back(pidh->getParticleID(rcp, pidh->getAlgorithmID("dEdxPID")).getPDG());
        showerPDG.push_back(pidh->getParticleID(rcp, pidh->getAlgorithmID("ShowerShapesPID")).getPDG());
        likeliPDG.push_back(pidh->getParticleID(rcp, pidh->getAlgorithmID("LikelihoodPID")).getPDG());
        lowmomPDG.push_back(pidh->getParticleID(rcp, pidh->getAlgorithmID("LowMomMuID")).getPDG());


// JL July 25, 2016 : ParticleIDVec does not seem to get filled !!!
//       
//         // getParticleIDs ("S" !) gives back a VECTOR of ParticleID objects for all hypotheses, ordered by value
//         ParticleIDVec likeliPID = pidh->getParticleIDs(rcp, pidh->getAlgorithmID("LikelihoodPID"));
//         
//         int hypcounter[5] = {0,0,0,0,0};
//         streamlog_out(DEBUG3) << " this ParticleIDVec has a size of = " << likeliPID.size() << std::endl;
//         for (int ihyp = 0; ihyp < likeliPID.size(); ihyp++) {
//           int pdg = abs(likeliPID[ihyp]->getPDG());
//           switch (pdg) {
//             case 11 :
//               LiPDG_el.push_back(likeliPID[ihyp]->getLikelihood());
//               hypcounter[0]++;
//               break;
//             case 13 :
//               LiPDG_mu.push_back(likeliPID[ihyp]->getLikelihood());
//               hypcounter[1]++;
//               break;
//             case 211 :
//               LiPDG_pi.push_back(likeliPID[ihyp]->getLikelihood());
//               hypcounter[2]++;
//               break;
//             case 321 :
//               LiPDG_ka.push_back(likeliPID[ihyp]->getLikelihood());
//               hypcounter[3]++;
//               break;
//             case 2212 :
//               LiPDG_pr.push_back(likeliPID[ihyp]->getLikelihood());
//               hypcounter[4]++;
//               break;
//             default :
//               streamlog_out(WARNING) << " found unknown PID hypothesis with pdg = " << pdg << std::endl;
//               break;
//           }  
//           if (ihyp == likeliPID.size() - 1) {
//             for (int jhyp = 0; jhyp < 5; jhyp++) {
//               if (hypcounter[jhyp] != 1) streamlog_out(ERROR) << " found wrong number of hypotheses: hypcounter [" << jhyp << "] = " << hypcounter[jhyp] << std::endl;
//             }
//           }
//         }

        
        const ParticleID* likeliPID = &(pidh->getParticleID(rcp, pidh->getAlgorithmID("LikelihoodPID")));
        float likelihood[5] = {999.,999.,999.,999.,999};
        if (likeliPID && likeliPID->getParameters().size() > 4 ) {
          for ( int ihyp = 0 ; ihyp < 5; ihyp++ ) {
            likelihood[ihyp] = likeliPID->getParameters()[ihyp];
            if ( std::isinf(likelihood[ihyp]) ) {
              streamlog_out(DEBUG3) << " likelihood [" << ihyp << "] = " << likelihood[ihyp] << std::endl;
              likelihood[ihyp] = 888.;
            }  
          }
          streamlog_out(DEBUG) << " electron hyp = " << likelihood[0] << std::endl;
          streamlog_out(DEBUG) << " muon hyp = "     << likelihood[1] << std::endl;
          streamlog_out(DEBUG) << " pion hyp = "     << likelihood[2] << std::endl;
          streamlog_out(DEBUG) << " kaon hyp = "     << likelihood[3] << std::endl;
          streamlog_out(DEBUG) << " proton hyp = "   << likelihood[4] << std::endl;
        }
        else {
          streamlog_out(DEBUG3) << " likeliPID has = " << likeliPID->getParameters().size() << " parameters " << std::endl;
        }
        LiPDG_el.push_back(likelihood[0]); 
        LiPDG_mu.push_back(likelihood[1]); 
        LiPDG_pi.push_back(likelihood[2]); 
        LiPDG_ka.push_back(likelihood[3]); 
        LiPDG_pr.push_back(likelihood[4]); 
        
        const ParticleID* dedxPID = &(pidh->getParticleID(rcp, pidh->getAlgorithmID("dEdxPID")));
        float dedxlikeli[5] = {999.,999.,999.,999.,999.};
        float distance[5] = {999.,999.,999.,999.,999.};
        if (dedxPID && dedxPID->getParameters().size() > 17 ) {
          for ( int ihyp = 0 ; ihyp < 5; ihyp++ ) {
            dedxlikeli[ihyp] = dedxPID->getParameters()[ihyp];
            if ( std::isinf(likelihood[ihyp]) ) {
              streamlog_out(DEBUG3) << " dedxlikeli [" << ihyp << "] = " << dedxlikeli[ihyp] << std::endl;
              dedxlikeli[ihyp] = 888.;
            }  
            distance[ihyp] = dedxPID->getParameters()[13+ihyp];
            if ( std::isinf(distance[ihyp]) ) {
              distance[ihyp] = 888.;
            }  
            streamlog_out(DEBUG3) << pidh->getParameterNames(pidh->getAlgorithmID("dEdxPID"))[13+ihyp] << " = " << distance[ihyp] << std::endl;
          }
          streamlog_out(DEBUG) << " dEdx liklihood electron hyp = " << dedxlikeli[0] << std::endl;
          streamlog_out(DEBUG) << " dEdx liklihood muon hyp = "     << dedxlikeli[1] << std::endl;
          streamlog_out(DEBUG) << " dEdx liklihood pion hyp = "     << dedxlikeli[2] << std::endl;
          streamlog_out(DEBUG) << " dEdx liklihood kaon hyp = "     << dedxlikeli[3] << std::endl;
          streamlog_out(DEBUG) << " dEdx liklihood proton hyp = "   << dedxlikeli[4] << std::endl;
        }
        else {
          streamlog_out(DEBUG3) << " dedxPID has = " << dedxPID->getParameters().size() << " parameters " << std::endl;
        }
        dedxdist_el.push_back(distance[0]); 
        dedxdist_mu.push_back(distance[1]); 
        dedxdist_pi.push_back(distance[2]); 
        dedxdist_ka.push_back(distance[3]); 
        dedxdist_pr.push_back(distance[4]); 
        dedxPDG_el.push_back(dedxlikeli[0]); 
        dedxPDG_mu.push_back(dedxlikeli[1]); 
        dedxPDG_pi.push_back(dedxlikeli[2]); 
        dedxPDG_ka.push_back(dedxlikeli[3]); 
        dedxPDG_pr.push_back(dedxlikeli[4]); 
        
        

      } // if reco part
      // IMPROVE HERE: CHECK if track or cluster exists!
      else {
        streamlog_out(DEBUG4) << "no ReconstructedParticle found for this mcp with PDG = " << mcp->getPDG() << " ! " << std::endl ;
        
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
        LiPDG_el.push_back(777.);
        LiPDG_mu.push_back(777.);
        LiPDG_pi.push_back(777.);
        LiPDG_ka.push_back(777.);
        LiPDG_pr.push_back(777.);
        dedxdist_el.push_back(777.); 
        dedxdist_mu.push_back(777.); 
        dedxdist_pi.push_back(777.); 
        dedxdist_ka.push_back(777.); 
        dedxdist_pr.push_back(777.); 
        dedxPDG_el.push_back(777.); 
        dedxPDG_mu.push_back(777.); 
        dedxPDG_pi.push_back(777.); 
        dedxPDG_ka.push_back(777.); 
        dedxPDG_pr.push_back(777.); 
        
         
      }
  
    }  // if gst == 1
  }  // loop over MCPs
  
  
  hermTree->Fill(); 
  nEvt++;

    if( ! (nEvt % 100)  )
      streamlog_out(MESSAGE)  << "================================ events processed:  " << nEvt << std::endl ;
  
}



void PIDTree::check( LCEvent*) { 

}


void PIDTree::end(){ 

}



