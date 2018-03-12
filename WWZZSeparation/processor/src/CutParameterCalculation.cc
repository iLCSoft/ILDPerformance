#include "WWZZProcessor.h"

void WWZZProcessor::calculateCutParameters(
		std::vector<ReconstructedParticle*> PFOs, std::vector<ReconstructedParticle*> jets,
		Observables &observ) {
	/* Function that calculates all parameters from the event 
 	that I later want to make cuts on and writes into the 
	eventInfo instance.
	*/

	// Find most VV like 2 x dijet pair combination
	findVpair(jets, observ);

	int nJets = jets.size();

	// Find within the four jets:
	// 	- minimum occuring jet energy
	// 	- minimum number of particles in one jet
	// 	- minimum number of charged particles in one jet
	float min_Ejet = 99999;
    int min_num_particles_jet			= 99999;
    int min_num_chargedparticles_jet	= 99999;

	for(int i=0; i<nJets; i++){
	    TLorentzVector jet_tlv = TLorentzVector( jets[i]->getMomentum(), jets[i]->getEnergy() );

		const ReconstructedParticleVec& jetPFOs = jets[i]->getParticles();
		int nJetPFOs = jetPFOs.size(); 

		int nCharged = 0;
		for( int k = 0; k < nJetPFOs; k++ ){
			ReconstructedParticle* jetparticle = jetPFOs[k];
			if ( jetparticle->getCharge() != 0 ) {
				nCharged += 1;
			}
		}

		if ( min_Ejet > jet_tlv.E() ) {	min_Ejet = jet_tlv.E(); }
		if ( min_num_particles_jet > nJetPFOs ) { min_num_particles_jet = nJetPFOs; }
		if ( min_num_chargedparticles_jet > nCharged ) { 
			min_num_chargedparticles_jet = nCharged; 
		}
	}

	observ.min_Ejet = min_Ejet;	
	observ.min_num_particles_jet = min_num_particles_jet;
	observ.min_num_chargedparticles_jet = min_num_chargedparticles_jet;

	
	// Find highest energy track of the event, from its TLorentzVector calculate
	// its cos(Theta)
	int nPFOs = PFOs.size();
	float max_track_E = 0;
	TLorentzVector maxE_particle_tlv;
	for(int i=0; i<nPFOs; i++){
		TLorentzVector particle_tlv = TLorentzVector(	PFOs[i]->getMomentum(), 
														PFOs[i]->getEnergy() );
		if ( particle_tlv.E() > max_track_E ) {
			maxE_particle_tlv = particle_tlv;
		}	
	}

	observ.cosTheta_EmaxTrack = maxE_particle_tlv.CosTheta();

	// Find all particles within 10degree cone around highest energy track,
	// cluster them together and save the combined energy
	TLorentzVector EmaxTrack_10cone_tlv = TLorentzVector( 0, 0, 0, 0 );
	for(int i=0; i<nPFOs; i++){
		TLorentzVector particle_tlv = TLorentzVector(	PFOs[i]->getMomentum(), 
														PFOs[i]->getEnergy() );
		float angle_to_EmaxTrack = maxE_particle_tlv.Angle(particle_tlv.Vect());
		if ( angle_to_EmaxTrack < (10.0/360.0 * 2.0*TMath::Pi()) ) {
			EmaxTrack_10cone_tlv += particle_tlv;
		}	
	}
	// Do not count Emax-track into cone
	EmaxTrack_10cone_tlv -= maxE_particle_tlv;

	observ.E_10cone_EmaxTrack = EmaxTrack_10cone_tlv.E();

}       
