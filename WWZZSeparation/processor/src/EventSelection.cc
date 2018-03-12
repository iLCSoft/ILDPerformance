#include "WWZZProcessor.h"

bool WWZZProcessor::passesSelectionCriteria( Observables &observ ){
	/* Function checking each selection criteria and return total result.
 	*/

	bool pass_isolep		= ( observ.num_isolated_leptons == 0 );	
	bool pass_jetcharged	= ( observ.min_num_chargedparticles_jet > 1 );
	bool pass_jetparticles	= ( observ.min_num_particles_jet > 2 );	
	bool pass_jetE			= ( observ.min_Ejet > 10 );			
	bool pass_y34			= ( observ.y_34 > 0.0001 );						
	bool pass_missmass		= ( observ.miss_mass > 200 );
	bool pass_ETvis			= ( observ.E_T_vis > 150 );					
	bool pass_pTvis			= ( observ.p_T_vis > 40 );						
	bool pass_cosThetamiss	= ( observ.cosTheta_miss < 0.99 );
	bool pass_cosThetaEmax	= ( observ.cosTheta_EmaxTrack < 0.99);
	bool pass_coneEmax		= ( observ.E_10cone_EmaxTrack > 2.0 );

	return 	pass_isolep && 
			pass_jetcharged && pass_jetparticles && pass_jetE && 
			pass_y34 &&
			pass_missmass && pass_cosThetamiss &&
			pass_ETvis && pass_pTvis && 
			pass_cosThetaEmax && pass_coneEmax;
}


void WWZZProcessor::eventSelection( EventInfo &info ) {
	/* Check if the preselection is passed using the variables stores in info and set the 
 	flag in info accordingly 
	*/

	if ( passesSelectionCriteria( info.observ ) ) {
		info.passed_preselection = 1;
	} else {
		info.passed_preselection = 0;
	}

}
