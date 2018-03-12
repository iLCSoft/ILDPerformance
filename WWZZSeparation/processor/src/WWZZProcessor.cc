#include "WWZZProcessor.h"

WWZZProcessor aWWZZProcessor ;

WWZZProcessor::WWZZProcessor() : Processor("WWZZProcessor") {

    _description = "WWZZProcessor: Search for e+e- -> vvWW/vvZZ -> vvqqqq at the ILD detector" ;


    // Standard input collections
    registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
            "InputAllPFOsCollection",  
            "Name of the PFOs collection", 
            _colAllPFOs,   
            std::string("PandoraPFOs") 
    );      

    registerInputCollection( LCIO::MCPARTICLE,
            "MCParticleCollection",
            "Name of the MC particle collection",
            _colMC,
            std::string("MCParticle") 
    );


	// Process specific input collections
    registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
            "InputFastJetsCollection",  
            "Name of the jet collection", 
            _colFastJets,   
            std::string("FastJets") 
    );      

	registerInputCollection( LCIO::RECONSTRUCTEDPARTICLE,
            "InputIsolepsCollection",
            "Name of the isolated lepton collection",
            _colIsoleps,
            std::string("Isoleps") 
    );


	// Name of output root file
    registerProcessorParameter( "RootFileName",
            "Name of Root file (default: output.root)",
            _rootfilename,
            std::string("/nfs/dust/ilc/group/ild/beyerjac/VBS/nunu_hadrons/output/root_files/output.root") 
    );

	// Cross section of sample
	registerProcessorParameter( "CrossSection",
			"Cross section of sample stdhep file",
			_cross_section,
			float(0.0)
	);
			

}



void WWZZProcessor::init() { 
    streamlog_out(DEBUG) << "   init called  " << std::endl ;

    _otfile = new TFile( _rootfilename.c_str() , "RECREATE" );   

	_tree = new TTree( "eventinfo" , "events" );

	// Create tree branches and link them to the variables in the info classes
	//
	_tree->Branch("N_evt",	&_info.evtN,	"N_evt/I");	

	_tree->Branch("cross_section",	&_info.process.cross_section,	"cross_section/F");
	_tree->Branch("e_pol",			&_info.process.e_pol,			"e_pol/I");
	_tree->Branch("p_pol",			&_info.process.p_pol,			"p_pol/I");

	_tree->Branch("true_event_type",		&_info.true_event_type,		"true_event_type/I");
	_tree->Branch("passed_preselection",	&_info.passed_preselection,	"passed_preselection/I");

	_tree->Branch("pair1_mass",	&_info.observ.pair1_mass,	"pair1_mass/F");	
	_tree->Branch("pair2_mass",	&_info.observ.pair2_mass,	"pair2_mass/F");	
	

    _nRun = 0 ;
    _nEvt = 0 ;
}


void WWZZProcessor::processRunHeader( LCRunHeader* /*run*/) { 
    _nRun++ ;
} 



void WWZZProcessor::processEvent( LCEvent * evt ) { 

    // this gets called for every event 
    // usually the working horse ...

	// Clear memory
	memset( &_info, 0, sizeof(_info) );
	_info.init();
	_info.evtN = _nEvt;

	// (Get information from the event header (e.g. cross-section))
	// -> Currently unavailable from stdhep file -> Use manual input
	_info.process.cross_section = _cross_section; // evt->getParameters().getFloatVal("CrossSection_fb");
	_info.process.e_pol = -1; //evt->getParameters().getFloatVal("Pol_em");
	_info.process.p_pol = +1; //evt->getParameters().getFloatVal("Pol_ep");

	// All of these collections should always be non-empty
    LCCollection* colAllPFOs 			= evt->getCollection( _colAllPFOs 		) ;
	LCCollection* colFastJets 			= evt->getCollection( _colFastJets 		) ;
    LCCollection* colMC 				= evt->getCollection( _colMC 			) ;

	// Isoleps could be empty (ideally)
	LCCollection* colIsoleps = NULL;
    try{
        colIsoleps = evt->getCollection( _colIsoleps );
    }
    catch( lcio::DataNotAvailableException e )
    {
        streamlog_out(WARNING) << _colIsoleps << " collection not available" << std::endl;
        colIsoleps = NULL;
    }

	// Call analysis script on event
    streamlog_out(DEBUG) << "   processing event: " << evt->getEventNumber() 
       << "   in run:  " << evt->getRunNumber() << std::endl ;

	analyseEvent( colMC, colAllPFOs, colFastJets, colIsoleps, _info );

	eventSelection( _info );		

	// Fill the event into the tree
	_tree->Fill();

    _nEvt ++ ;
}


void WWZZProcessor::check( LCEvent * /*evt*/ ) { 
    // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void WWZZProcessor::end(){ 

    _otfile->cd();
	_tree->Write();
    _otfile->Close();

}

