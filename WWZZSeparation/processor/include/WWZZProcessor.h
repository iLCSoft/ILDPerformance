#ifndef WWZZProcessor_h
#define WWZZProcessor_h 1

#include "EventInformation.h"
#include "StandardIncludes.h"

class WWZZProcessor : public Processor {
 
	public:

		virtual Processor*  newProcessor() { return new WWZZProcessor ; }
  
  
		WWZZProcessor() ;
		WWZZProcessor(const WWZZProcessor &) = delete;
		WWZZProcessor& operator=(const WWZZProcessor &) = delete ;

		/** Called at the begin of the job before anything is read.
		 * Use to initialize the processor, e.g. book histograms.
		 */
		virtual void init() ;
	
		/** Called for every run.
		 */
		virtual void processRunHeader( LCRunHeader* run ) ;
		
		/** Called for every event - the working horse.
		 */
		virtual void processEvent( LCEvent * evt ) ; 

			/** Subroutines 
			*/
			virtual void analyseEvent(
				LCCollection* colMC, LCCollection* colPFOs, LCCollection* colJets, 
				LCCollection* colIsoLeps, EventInfo &info);
			
			std::vector<MCParticle*> checkMCFinalState(LCCollection* AllMC); 
			std::vector<ReconstructedParticle*> checkPOFinalState(LCCollection* AllPOs) ;
			
			// Using Generator level information
			virtual std::vector<MCParticle*> findGenLevelNeutrinos(std::vector<MCParticle*> MCparticles);
			virtual std::vector<MCParticle*> findGenLevelQuarks(std::vector<MCParticle*> MCparticles);	
			
			// True event type finding
			virtual	void findAllowedVVCandidates(
				std::vector<std::vector<std::vector<MCParticle*>>> all_VV_candidates, 
				std::vector<std::vector<std::vector<MCParticle*>>> &possible_VV_candidates, 
				std::string VV_name );
			
			virtual void findTrueEventType(std::vector<MCParticle*> MCparticles, EventInfo &info); 
			
			// V pair finding
			template<typename LCIOParticleType> void fillVPairInformation( 
				std::vector<LCIOParticleType*> V1, std::vector<LCIOParticleType*> V2, 
				Observables &observ);
			
			template<typename LCIOParticleType>	void getAllVVTo4ParticleCombinations ( 
				std::vector<LCIOParticleType*> particles, 
				std::vector<std::vector<std::vector<LCIOParticleType*>>> &VV_candidates );
			
			template<typename LCIOParticleType>	void getVVCandidatesMasses( 
				std::vector<std::vector<std::vector<LCIOParticleType*>>> VV_candidates, 
				std::vector<std::vector<float>> &candidates_masses ) ;
			
			template<typename LCIOParticleType>	int findIndexBestPairInVVCandidates( 
				std::vector<std::vector<std::vector<LCIOParticleType*>>> VV_candidates );
			
			template<typename LCIOParticleType> void findVpair(
				std::vector<LCIOParticleType*> jets, Observables &observ);
			
			// Cut parameter calculation
			virtual void calculateCutParameters(
				std::vector<ReconstructedParticle*> PFOs, std::vector<ReconstructedParticle*> jets,
				Observables &observ);
			
			// Event selection
			virtual bool passesSelectionCriteria( Observables &observ );
			virtual void eventSelection( EventInfo &info );

		virtual void check( LCEvent * evt ) ; 
		
		/** Called after data processing for clean up.
		 */
		virtual void end() ;
  
	protected:

		/** Input collection name.
		 */
		std::string _colAllPFOs{};
		std::string _colFastJets{};
		std::string _colIsoleps{};
		std::string _colMC{};
		
		/** Parameters during Marlin analysis
		 */
		
		int _nRun {};
		int _nEvt {};
		
		EventInfo _info{};
		

		/** Sample cross section
 		*/
		float _cross_section {};

		/** Output File specifics
		 */
		TFile* _otfile{};
		TTree* _tree{}; 
		
		std::string _rootfilename{};
} ;


// This needs to be at end of this header, otherwise template functionality doesn't work
#include "VpairFinder.h"  
#endif



