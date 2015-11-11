#include "pi0Tree.h"

#include <UTIL/LCRelationNavigator.h>
#include <UTIL/PIDHandler.h>
#include "TROOT.h"



Pi0Tree aPi0Tree ;


Pi0Tree::Pi0Tree() : Processor("Pi0Tree") {
  
  // modify processor description
  _description = "Pi0Tree does whatever it does ..." ;

  
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
			   "GammaGammaParticleCollection" , 
			   "Name of the gamma-gamma candidate input collection"  ,
			   _gammaGammaParticleCollectionName ,
			   std::string("GammaGammaParticles") ) ;

}


void Pi0Tree::init() { 

  streamlog_out(DEBUG) << "   init called  " 
		       << std::endl ;

  // usually a good idea to
  printParameters() ;
  nEvt = 0;

  gROOT->ProcessLine("#include <vector>");
}

void Pi0Tree::processRunHeader( LCRunHeader* run) { 
    
} 

void Pi0Tree::processEvent( LCEvent * evt ) { 

  streamlog_out(MESSAGE) << " start processing event " << std::endl;

  if( isFirstEvent() ) { 
    pi0Tree = new TTree("pi0Tree","pi0Tree");
    pi0Tree->Branch("nMCPi0",&nMCPi0,"nMCPi0/I") ;
    pi0Tree->Branch("trueE",&trueE) ;
    pi0Tree->Branch("trueP",&trueP) ;
    pi0Tree->Branch("truePt",&truePt) ;
    pi0Tree->Branch("trueTheta",&trueTheta) ;
    pi0Tree->Branch("truePhi",&truePhi) ;
    pi0Tree->Branch("truePDG",&truePDG) ;
    pi0Tree->Branch("trueMother",&trueMother) ;
    pi0Tree->Branch("isSeen",&isSeen) ;
    
    pi0Tree->Branch("nRecoPi0",&nRecoPi0,"nRecoPi0/I") ;
    pi0Tree->Branch("nTruePhotons",&nTruePhotons) ;
    pi0Tree->Branch("nTrueMeson",&nTrueMeson) ;
    pi0Tree->Branch("isTrue",&isTrue) ;
    pi0Tree->Branch("recoE",&recoE) ;
    pi0Tree->Branch("pfoE",&pfoE) ;
    pi0Tree->Branch("trueEofSeen",&trueEofSeen) ;
    pi0Tree->Branch("recoMass",&recoMass) ;
    pi0Tree->Branch("pfoMass",&pfoMass) ;
    pi0Tree->Branch("trueMassofSeen",&trueMassofSeen) ;
    pi0Tree->Branch("truePDGofSeen",&truePDGofSeen) ;

  }
  
  streamlog_out(DEBUG) << " clearing vectors " << std::endl;
  nMCPi0 = 0;
  trueE.clear();  
  trueP.clear();  
  truePt.clear();  
  trueTheta.clear();  
  truePhi.clear();  
  truePDG.clear();  
  trueMother.clear();  
  isSeen.clear();  
  
  nRecoPi0 = 0;  
  isTrue.clear();  
  recoE.clear();  
  pfoE.clear();  
  trueEofSeen.clear();  
  recoMass.clear();  
  pfoMass.clear();  
  trueMassofSeen.clear();  
  truePDGofSeen.clear();  


  streamlog_out(DEBUG) << " iterator and navigator " << std::endl;
  LCIterator<MCParticle> mcpIt( evt, _mcParticleCollectionName ) ;
  streamlog_out(DEBUG) << " got mcpIt with length of " << mcpIt.size() << std::endl;
  LCRelationNavigator mc2recoNav(evt->getCollection( _trueToReco )); 
  streamlog_out(DEBUG) << " got mc2recoNav from " << mc2recoNav.getFromType() << " to " << mc2recoNav.getToType() << std::endl;
  
  //----------------------------------------------------------------------------------------------------------------------------
  // loop over MCParticles
  //----------------------------------------------------------------------------------------------------------------------------
  
  while( MCParticle* mcp = mcpIt.next()  ) {
    
    if (!mcp) continue; 
    
    bool keep = false;
    const EVENT::MCParticleVec& daughters = mcp->getDaughters();
    // keep pi0/eta/phi which decays to gammagamma
    if (mcp->getPDG() == 111 || mcp->getPDG() == 221 || mcp->getPDG() == 331) {
      if (daughters.size()>0 && daughters[0]->getPDG() == 22) keep = true; 
    }
    
    if (keep) {   
    
      streamlog_out(DEBUG) << "found mcparticle id = " << mcp->getPDG() << ", genstat = " << mcp->getGeneratorStatus() << std::endl;  
    
      nMCPi0++;
    
      gear::Vector3D v( mcp->getVertex()[0], mcp->getVertex()[1], mcp->getVertex()[2] );
      gear::Vector3D e( mcp->getEndpoint()[0], mcp->getEndpoint()[1], mcp->getEndpoint()[2] );
      gear::Vector3D p( mcp->getMomentum()[0], mcp->getMomentum()[1], mcp->getMomentum()[2] );
    
      trueE.push_back(mcp->getEnergy());
      trueP.push_back(p.r());
      truePt.push_back(p.trans());
      trueTheta.push_back(p.theta());
      truePhi.push_back(p.phi());
      truePDG.push_back(mcp->getPDG());
      if (mcp->getParents()[0]) {
        trueMother.push_back(mcp->getParents()[0]->getPDG());
      }  
      else {
        trueMother.push_back(-1);
      }
      
      int nseen = 0;
    
      for (int idaughter = 0; idaughter < daughters.size(); idaughter++) {
        MCParticle* mcd = daughters[idaughter];
        streamlog_out(DEBUG) << " get reco particle for daughter " << idaughter << std::endl;  
        const EVENT::LCObjectVec& recovec = mc2recoNav.getRelatedToObjects(mcd);
        streamlog_out(DEBUG) << " recovec has length " << recovec.size() << std::endl;  
        const EVENT::FloatVec& recoweightvec = mc2recoNav.getRelatedToWeights(mcd);
        //streamlog_out(DEBUG) << " recoweightvec has length " << recoweightvec.size() << std::endl;  
        double maxcaloweight = 0;
        int imaxcaloweight = -1;
        for (int irel = 0; irel < recovec.size(); irel++) {
          streamlog_out(DEBUG) << " irel " << irel << ", recoweight = " << int(recoweightvec.at(irel)) 
                               << ", recoweight%10000 (track) = " << int(recoweightvec.at(irel))%10000 
                               << ", recoweight/10000 (calo) = " << int(recoweightvec.at(irel))/10000 << std::endl;  
          double caloweight = double((int(recoweightvec.at(irel))/10000)/1000.);
          if (caloweight > maxcaloweight) {
            imaxcaloweight = irel;
            maxcaloweight = caloweight;
          }
        }
      
        streamlog_out(MESSAGE) << " found reco particle for mcd at imaxcaloweight = " << imaxcaloweight << " with weight = " << maxcaloweight << std::endl ;
       
        // option: sum weights instead? check whether we really have a photon?
        nseen++;
        
      }  // loop over daughters
      
      isSeen.push_back(nseen);
    
    }  // if keep
    
  }  // loop over MCPs
      
  streamlog_out(DEBUG) << " reco iterator and navigator " << std::endl;
  LCIterator<ReconstructedParticle> ggpIt( evt, _gammaGammaParticleCollectionName ) ;
  streamlog_out(DEBUG) << " got ggpIt with length of " << ggpIt.size() << std::endl;
  LCRelationNavigator rec2mcNav(evt->getCollection( _recoToTrue )); 
  streamlog_out(DEBUG) << " got rec2mcNav from " << rec2mcNav.getFromType() << " to " << rec2mcNav.getToType() << std::endl;
  
  //----------------------------------------------------------------------------------------------------------------------------
  // loop over GammaGammaParticles
  //----------------------------------------------------------------------------------------------------------------------------
  
  while( ReconstructedParticle* ggp = ggpIt.next()  ) {
    
    if (!ggp) continue; 
    
    nRecoPi0++;
    
    //gear::Vector3D rp( ggp->getMomentum()[0], ggp->getMomentum()[1], ggp->getMomentum()[2] );
    recoE.push_back(ggp->getEnergy());
    recoMass.push_back(ggp->getMass());
    //seenPt.push_back(rp.trans());
    //seenTheta.push_back(rp.theta());
    //seenPhi.push_back(rp.phi());
            
    // get photons
    const EVENT::ReconstructedParticleVec& gammas = ggp->getParticles ();
    int istrue = 0;
    int pdg = 0;
    int ntruephoton = 0;
    int ntruemeson = 0;
    double sumTrueE = 0;
    double sumTrueP[3] = {0, 0, 0};
    double sumE = 0;
    double sumP[3] = {0, 0, 0};
    double sumWeight = 0;
    MCParticle* mcgps[2];

    for (int igamma = 0; igamma < gammas.size(); igamma++) {
      sumE += gammas[igamma]->getEnergy();
      for (int i = 0; i < 3; i++) sumP[i] += gammas[igamma]->getMomentum()[i];        
      streamlog_out(DEBUG) << " get mc particle for gamma " << igamma << std::endl;  
      const EVENT::LCObjectVec& truevec = rec2mcNav.getRelatedToObjects(gammas[igamma]);
      streamlog_out(DEBUG) << " truthvec has length " << truevec.size() << std::endl;  
      const EVENT::FloatVec& truthweightvec = rec2mcNav.getRelatedToWeights(gammas[igamma]);
      //streamlog_out(DEBUG) << " truthweightvec has length " << truthweightvec.size() << std::endl;  
      double maxcaloweight = 0;
      int imaxcaloweight = -1;
      double maxtrckweight = 0;
      int imaxtrckweight = -1;
      double maxweight = 0;
      int imaxweight = -1;
      MCParticle* mcg = 0;
      
      bool isConversion = false;
      // catch conversions
      if (truevec.size() == 2) {
        MCParticle* el1 =  (MCParticle*) truevec.at(0); 
        MCParticle* el2 =  (MCParticle*) truevec.at(1); 
        if ((el1->getPDG() == 11 && el2->getPDG() == -11) || (el1->getPDG() == -11 && el2->getPDG() == 11)) {
          MCParticle* mo1 = el1->getParents()[0];
          MCParticle* mo2 = el2->getParents()[0];
          if (mo1->getPDG() == 22 && mo1 == mo2) {
            streamlog_out(MESSAGE) << " found conversion - using mother photon: "  
                                   << " true PDGs = " << el1->getPDG() << ", " << el2->getPDG()
                                   << ", true parent PDG = " << mo1->getPDG()  << std::endl;  
            isConversion = true;
            maxweight = double( (int(truthweightvec.at(0))/10000 + int(truthweightvec.at(1))/10000)/1000. );
            // conversion mother is true photon!
            mcg = mo1;  
          }
        }
      }
        
      if (!isConversion) {  
        for (int irel = 0; irel < truevec.size(); irel++) {
          MCParticle* dummy =  (MCParticle*) truevec.at(irel); 
          streamlog_out(DEBUG) << " irel " << irel << ", trueweight = " << int(truthweightvec.at(irel)) 
                               << ", true PDG = " << dummy->getPDG()
                               << ", true parent PDG = " << dummy->getParents()[0]->getPDG()
                               << ", recoweight%10000 (track) = " << int(truthweightvec.at(irel))%10000 
                               << ", recoweight/10000 (calo) = " << int(truthweightvec.at(irel))/10000 << std::endl;  
          double truthcaloweight = double( (int(truthweightvec.at(irel))/10000)/1000. );
          if (dummy->getPDG() == 22 && truthcaloweight > maxcaloweight) {
            imaxcaloweight = irel;
            maxcaloweight = truthcaloweight;
          }
          double truthtrckweight = double((int(truthweightvec.at(irel))%10000)/1000.);
          if (dummy->getPDG() == 22 && truthtrckweight > maxtrckweight) {
            imaxtrckweight = irel;
            maxtrckweight = truthtrckweight;
          }
        }
        streamlog_out(DEBUG) << " found true photon at imaxcaloweight = " << imaxcaloweight << " with weight = " << maxcaloweight << std::endl ;
        streamlog_out(DEBUG) << " found true photon at imaxtrckweight = " << imaxtrckweight << " with weight = " << maxtrckweight << std::endl ;
        
        maxweight = maxcaloweight;
        imaxweight = imaxcaloweight;
        if (maxtrckweight > maxcaloweight) {
          maxweight = maxtrckweight;
          imaxweight = imaxtrckweight;
        }  
        
        // skip if photon has too small weight
        if (maxweight < 0.1) {
          streamlog_out(DEBUG) << " weight of photon = " << maxweight << " is too small - don't count as correct" << std::endl ;
          continue;
        }
        
        streamlog_out(DEBUG) << " found true photon at imaxweight = " << imaxweight << " with weight = " << maxweight << std::endl ;
        mcg =  (MCParticle*) truevec.at(imaxweight); 
      }
      
      
      if (!mcg) continue;
      
      // found a correct photon or a conversion
      ntruephoton++; 
      sumWeight += maxweight;  
      
      sumTrueE += mcg->getEnergy();
      for (int i = 0; i < 3; i++) sumTrueP[i] += mcg->getMomentum()[i];        

      // get parent   
      mcgps[igamma] = mcg->getParents()[0];
      if (!mcgps[igamma]) continue;
      if (mcgps[igamma]->getPDG() == 111 || mcgps[igamma]->getPDG() == 221 || mcgps[igamma]->getPDG() == 331) {
        // found photon from pi0/eta/eta'
        ntruemeson++; 
        streamlog_out(MESSAGE) << "(correct) PDG of photon's parent is = " << mcgps[igamma]->getPDG() << std::endl ;
        pdg = mcgps[igamma]->getPDG();
      }  
      else {
        streamlog_out(MESSAGE) << "(wrong) PDG of photon's parent is = " << mcgps[igamma]->getPDG() << std::endl ;
      } 
    }
    // check if really from same mother
    if (mcgps[0] && mcgps[1] && mcgps[0] == mcgps[1]) {
      istrue = 1;
      pdg = mcgps[0]->getPDG();
      streamlog_out(MESSAGE) << "(fully correct) PDG of both photons parent is = " << pdg << std::endl ;
    }
    
    nTruePhotons.push_back(ntruephoton);
    nTrueMeson.push_back(ntruemeson);
    isTrue.push_back(istrue);
    pfoE.push_back(sumE);
    pfoMass.push_back(sqrt(sumE*sumE-sumP[0]*sumP[0]-sumP[1]*sumP[1]-sumP[2]*sumP[2]));
    trueEofSeen.push_back(sumTrueE);
    trueMassofSeen.push_back(sqrt(sumTrueE*sumTrueE-sumTrueP[0]*sumTrueP[0]-sumTrueP[1]*sumTrueP[1]-sumTrueP[2]*sumTrueP[2]));
    truePDGofSeen.push_back(pdg);
        
  }  // loop over GammaGammaParticles
  
  
  pi0Tree->Fill(); 
  nEvt++;
  
  cout << "======================================== event " << nEvt << std::endl ;
  
}



void Pi0Tree::check( LCEvent * evt ) { 

}


void Pi0Tree::end(){ 

}



