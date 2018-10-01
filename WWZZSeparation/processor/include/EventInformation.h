#ifndef EVENT_INFORMATION_H
#define EVENT_INFORMATION_H 1 

#include "TLorentzVector.h"
#include <math.h>


struct Observables {
  /* Class that holds all information, that can be measured by the detector
    and I make a cut on */ 

  float pair1_mass {};
  float pair2_mass {};

  float miss_mass {}; // Also called: recoil mass

  float sum_E_q {}; // should be called sum_E_vis ...
  float E_T_vis {};
  float p_T_vis {};
  float E_10cone_EmaxTrack {};

  float cosTheta_miss {};
  float cosTheta_EmaxTrack {};

  // Distance parameter when clustering jets from 4 to 3 jets
  float y_34 {};

  // Each of those is the minimum over all 4 jets 
  // -> Cut on them means cut on all jets
  float min_Ejet {};
  int min_num_particles_jet {};
  int min_num_chargedparticles_jet {};

  int num_isolated_leptons {};

  TLorentzVector pair1_tlv {};
  TLorentzVector pair2_tlv {};
  TLorentzVector visible_tlv {};
  TLorentzVector miss_tlv {};

  void init() {
    pair1_tlv 	= TLorentzVector(0.0, 0.0, 0.0, 0.0);
    pair2_tlv 	= TLorentzVector(0.0, 0.0, 0.0, 0.0);
    visible_tlv = TLorentzVector(0.0, 0.0, 0.0, 0.0);
    miss_tlv 	= TLorentzVector(0.0, 0.0, 0.0, 0.0);

    pair1_mass	= -10;
    pair2_mass	= -10;

    miss_mass	= -10;

    sum_E_q		= -10;
    E_T_vis		= -10;
    p_T_vis		= -10;
    E_10cone_EmaxTrack	= -10;

    cosTheta_miss	= -2;
    cosTheta_EmaxTrack	= -2;

    y_34	= -1;

    min_Ejet	= -10;
    min_num_particles_jet			= -1;
    min_num_chargedparticles_jet	= -1;

    num_isolated_leptons = -1;

  }

  void processTLVs() {
    pair1_mass = pair1_tlv.M();
    pair2_mass = pair2_tlv.M();

    visible_tlv = pair1_tlv + pair2_tlv;
    sum_E_q		= visible_tlv.E();
    E_T_vis		= visible_tlv.Et();
    p_T_vis		= visible_tlv.Pt();

    TLorentzVector initial = TLorentzVector(1000*sin(0.014/2.0),0,0,1000);	
    miss_tlv 	= initial - pair1_tlv - pair2_tlv;
    miss_mass 	= miss_tlv.M();
    cosTheta_miss	= miss_tlv.CosTheta();
  }

};


struct ProcessInfo {
  /* Class that stores metadata about sample */

  float cross_section {};

  // Store e-/e+ polarisations: +1 = right-handed, -1 = left-handed
  int e_pol {};
  int p_pol {};

  void init() {
    cross_section = -10;
    e_pol = 0;
    p_pol = 0;
  }

};


struct EventInfo {
  /* General class that holds all information about the event 
    on all the levels of the generation */

  int evtN {};
  ProcessInfo process {};

  // True event type determined from MC-info, 1=ZZ, 2=WW, 0=other 
  int true_event_type {};

  // Storage for everything that can be actually observed
  Observables observ {};

  // Flag for whether event passed preselection
  int passed_preselection {};

  void init() {
    process.init();

    true_event_type = -1;

    observ.init();

    passed_preselection = -1;	
  }
};

#endif


