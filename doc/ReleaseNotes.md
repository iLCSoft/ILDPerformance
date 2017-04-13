


Oleksandr Viazlo 2017-03-21 
  - Replace ILDCellID0 with LCTrackerCellID

# v01-01

Frank Gaede 2017-03-21 
  - add AUTHORS file

Marko Petric 2017-03-21 
  - Enable CI and add LICENCE

Shaojun Lu 2017-03-14 
  -  Fixed the root macro for the crash in ROOT6 with segmentation violation.
  -  Update script to run the Mokka simulation and reconstruction for single Muon particle gun, and explicitly steer the 'UsingParticleGun=true' for reconstruction and 'PhysSampleOn=false' for Diagnostics.
  -  Added the momentum and transverse momentum cuts, and allow to be steerable in steering file.

Shaojun Lu 2017-03-06 
  -  Update the pulls macro to focus on the details.

Huong Lan Tran 2017-03-03 
  - Start to construct the JER folder

Shaojun Lu 2017-02-09 
  -  replaced the c++ standard cout with Marlin 'streamlog_out(DEBUG4)'. and allow to apply the 'Verbosity' control in the Marlin steering file.
  -  Copy one steer file 'Diagnostics.xml' here, and updated the parametere for ttbar physics sample simulation.

Shaojun Lu 2017-02-07 
  -  document the update in 'release-notes.txt'.
  -  Updated README by adding instruction for option of DD4hep sim and reco.
  -  Updated the example script to use the cvmfs ilcsoft v01-19-01, and added one example ttbar stdhep input file, for the monitor of the tracking efficiency. Please check out the ILDConfig HEAD from repository.

Frank Gaede 2016-11-03 
  -  - updated version and release notes for ---  v01-01 ----

Jenny List 2016-11-02 
  - fixed wrong root file name
  - added release notes
  - added new reference file for ZH test

Jenny List 2016-11-01 
  - updated ZH->4jets test in kinfit

Jenny List 2016-10-06 
  - made resolutions steerable in ZH5CFit
  - new reference file including pulls

Jenny List 2016-10-05 
  - added pull histos

Georgios Voutsinas 2016-09-16 
  - updating the resolution macros that compare with DBD results after moving the data to  the pool

Georgios Voutsinas 2016-09-09 
  - updating the mokka based diagnostics scripts

Georgios Voutsinas 2016-09-02 
  - adding macros that compare the resolutions with one from DBD plus a batch script to run the diagnostics to the example analysos reco files

Frank Gaede 2016-08-04 
  -  - added documentation for the PID plots    - updated some of the plots

Peter McKeown 2016-08-04 
  -  - added plots for     - efficiency and fake rate for likelihoodPID     - dE/dx     - likelihood PID     - efficiency vs. purity for likelihoodPID

Daniel Jeans 2016-08-04 
  - bug fix

Georgios Voutsinas 2016-08-03 
  - add a missing std:: in ifstream as well
  - add a missing std:: in ofstream

Georgios Voutsinas 2016-08-02 
  - update release notes and some minor fixes

Jenny List 2016-08-02 
  - added dEdx distance and dEdx likelihood to tree)

Daniel Jeans 2016-08-02 
  - various updates: split SIM/RECO collections

Daniel Jeans 2016-08-01 
  - extend for CalorimeterHits and TrackerHits; a few other cleanups
  - clean up
  - major update, more automatic procedure

Jenny List 2016-07-29 
  - significantly improved truth vs reco navigation
  - set dedx = 0 for ntrack != 1

Frank Gaede 2016-07-29 
  -  - finalized plotting of dE/dx for all particle types    ( code from P.McKeown )

Frank Gaede 2016-07-28 
  -  - make compatible w/ c++11 ( narrowing )

Jenny List 2016-07-28 
  - changed value of likelihoods to flag the absence of a reco particle for this MC particle from 2 to 5)

Georgios Voutsinas 2016-07-27 
  - update to version v01-00 - update release notes

Jenny List 2016-07-25 
  - added likelihood values for all hypotheses as LiPDG_mu etc to PIDtree; == 9 means is -inf on DST =>check with Masakazu?; == 2 means no PID or no recoParticle existing for this MCP

Huong Lan Tran 2016-07-20 
  - Modify CMakeLists.txt file to include the validateSim folder.

Daniel Jeans 2016-07-20 
  - added examples and v.short documentation
  - add simulation validation processor

Frank Gaede 2016-07-15 
  -  - added trueMass to PID TTree  - added macro plotdEdx  - bug fix for getParents.size()==0

Marko Petric 2016-06-22 
  - Added GSL because required. It would however better to fix aidaTT to propagate GSL properly

Jenny List 2016-06-17 
  - added ZHWW5CFit
  - root file name consistent with script
  - root file name consistent with macro
  - added ZH example

Jenny List 2016-06-15 
  - added first version of test on WW->4jets

Georgios Voutsinas 2016-06-10 
  - add a diagnostics script that run a simple analysis hZ->mumuX for Mokka as well

Frank Gaede 2016-06-09 
  - - added needed dependecies: MarlinTrk, aidaTT, DD4hep, GBL

Georgios Voutsinas 2016-06-09 
  - processor that calculates track - hit residuals based on aidaTT intersection calculation added

Georgios Voutsinas 2016-06-07 
  - update README - add directory structure for simulated / reconstructed files
  - updating scripts & README for running new dd4hep based simulation/reconstruction
  - updating tracking performance macros

Georgios Voutsinas 2016-05-25 
  - updates on tracking resolution performance plots

Georgios Voutsinas 2016-05-24 
  - updating tracking macros, adding some more hit info at diagnostics

Frank Gaede 2016-03-07 
  -  - made compatible with c++11      - removed -ansi -pedantic -Wno-long-long

Georgios Voutsinas 2016-02-22 
  - adding analysis macro

Frank Gaede 2016-02-19 
  -  - copy all config files (not only Pandora)

Frank Gaede 2016-02-18 
  -  - renamed bbudsc_3evt_ddsim.steer to ddsim.steer  - introduced variable $ILDCONFIG

Frank Gaede 2016-02-16 
  -  - updated release notes - pointing to:  - added Changelog.txt
  -  - updated version  --------- v00-02 ----------

Georgios Voutsinas 2016-01-21 
  - example of full analysis - HZ->mumuX - using the new simulation/reconstruction software added it uses the NAF facility to parallelise the simulation in order to save time

Jenny List 2016-01-19 
  - fixed bug reported by Strahinja

Georgios Voutsinas 2016-01-14 
  - removing some deprecated scripts
  - updating diagnostics for new simulation in order not to duplicate code, we call steering files from ILDConfig and here only edit the parameters so the new simulation diagnostics work only with HEAD ILCSoft installations

Georgios Voutsinas 2015-12-22 
  - macros - scripts for calculation of the RMS 90 of the energy resolution added

Graham Wilson 2015-12-16 
  - minor changes for debugging of pi0tree and fix couple of minor compiler warnings in tracking, JER, PID.

Jenny List 2015-12-11 
  - changed mcp->getParent(i) to mcp->getParents()[i] in PIDTree.cc

Georgios Voutsinas 2015-12-10 
  - protection against empty fits added
  - Improvements & cosmetics to tracking performance scripts and macros
  - adding macro that plots the sigma of the track pulls as a function of P and polar angle

Georgios Voutsinas 2015-12-04 
  - restoring runAll script
  - adding energy resolution to ILDPerformance - not yet fully complete - only for testing for now

Georgios Voutsinas 2015-11-29 
  - adding ddsim.py - replacing runNewSim.py
  - Updated generation - simulation - reconstruction chain for new DD4hep sim for time being by default refits the tracks with aidaTT if the user wants to study a different track collection (i.e. KalTest fitted tracks ) they have to modify Diagnostics.xml accordingly

Georgios Voutsinas 2015-11-27 
  - macro that creates booklet with the track parameter pulls for all the analysed files

Frank Gaede 2015-11-25 
  -  - made compatible with bash

Georgios Voutsinas 2015-11-24 
  - change the UseParticleGun parameter of RecoMCTruthLinker in the Diagnostics steering file to true - adding macro to directly compare the pull histos of 2 files
  - change the UseParticleGun parameter of RecoMCTruthLinker in the Diagnostics steering file to true - adding macro to directly compare the pull histos of 2 files

Jenny List 2015-11-11 
  - fixed true X->gammagamma selection in ILDPerformance/pi0
  - refined efficiency definition in ILDPerformance/pi0
  - now the contents of ILDPerformance/pi0
  - first commit of ILDPerformance/pi0

Georgios Voutsinas 2015-11-05 
  - fixing an issue with phi

Georgios Voutsinas 2015-11-03 
  - Updating the steering with the latest RecoMCTruthLinker par., remove warnings and more small changes

Jenny List 2015-10-14 
  - updated PID/README
  - oops - removed accidentally committed plots again
  - added PID subdirectory

Georgios Voutsinas 2015-07-03 
  - adding plot eff vs costheta & macro to compare efficiencies

Georgios Voutsinas 2015-06-19 
  - making some updates/fixes to scripts running the new simulation/reconstruction

Rosa Simoniello 2015-06-17 
  - rescale phi angle in a fixed range, add processor to skim charged mc particple in some theta region

Georgios Voutsinas 2015-06-10 
  - release notes added

Georgios Voutsinas 2015-06-09 
  - minor bug fixes

Georgios Voutsinas 2015-04-29 
  - digging more info on bad tracks, updating README

Georgios Voutsinas 2015-04-13 
  - scripts to run and test the new simulation work on going uses HEAD versions of ILCSoft packages

Georgios Voutsinas 2015-02-18 
  - correcting PatRec.sh script

Georgios Voutsinas 2015-02-17 
  - steering file parameters tuning - macros plot 20deg polar angle also
  - Some cosmetics to the macros

Georgios Voutsinas 2015-02-05 
  - change from hard coded values to processor parameters the definition of efficiency and of the filters that define the examined MCParticle subsample

Georgios Voutsinas 2015-02-03 
  - adding macros
  - Readme updated, user friendliness improved, automatically initialising ROOT GUI after running the scripts, more macros added

Georgios Voutsinas 2014-12-23 
  - Macro that compares the track parameters residuals of two track collections

Georgios Voutsinas 2014-12-19 
  - adding canvas with residuals

Frank Gaede 2014-12-19 
  -  - make sure ROOT has the dictionary for stdd::vector (needed e.g. on MacOS)

Georgios Voutsinas 2014-12-09 
  - macros for D0 - momentum resolution
  - fixes at the Pt resolution macro

Georgios Voutsinas 2014-12-08 
  - first testing macro to plot Pt resolution
  - steering files for analysis
  - correction in runAnalysis script
  - Scripts to simulate, reconstruct and analyse single tracks of various momenta and polar angles added

Georgios Voutsinas 2014-12-05 
  - first import of track diagnostics processor

Frank Gaede 2014-12-05 
  -  - add empty directories
  -  - created empty package structure
