# v01-12

* 2023-06-26 tmadlener ([PR#40](https://github.com/iLCSoft/ILDPerformance/pull/40))
  - Fix the CI workflow by making sure that the DD4hep headers appear early enough to have precedence over any headers that might be picked up from the underlying environment.
  - Add a workflow based on the key4hep nightly builds

* 2023-06-26 Ulrich Einhaus ([PR#39](https://github.com/iLCSoft/ILDPerformance/pull/39))
  New options:
  - lambda cut
  - PID algorithm for PDG MC-Reco plot
  - plot folder

# v01-11

* 2022-12-09 Ulrich Einhaus ([PR#38](https://github.com/iLCSoft/ILDPerformance/pull/38))
  add option to use RecoMCTruthLink instead of Trak2MCTruthLink
    add plots and fits for angular correction and Bethe-Bloch reference curve for LikelihoodPIDProcessor
    add command line output and text file output of above fit results for calibration

* 2022-11-14 Thomas Madlener ([PR#37](https://github.com/iLCSoft/ILDPerformance/pull/37))
  - Update CI to latest working nightly builds and use the same actions as everywhere else.

* 2022-11-14 Zehvogel ([PR#36](https://github.com/iLCSoft/ILDPerformance/pull/36))
  - fixed memory leak in PIDTree.cc

# v01-10

* 2021-05-06 Ulrich Einhaus ([PR#34](https://github.com/iLCSoft/ILDPerformance/pull/34))
  Update of the dEdxAnalyser to v1.2 with new features for calibration:
  - fit poly3 to mean dE/dx vs. |lambda| -> input to AngularCorrection_dEdxProcessor
  - fit Bethe-Bloch curves mean dE/dx values -> input to LikelihoodPIDProcessor
  - these two fits and the fiducial electrons dE/dx resolution are printed on the console at the end of the Analyser

* 2021-03-16 Ulrich Einhaus ([PR#33](https://github.com/iLCSoft/ILDPerformance/pull/33))
  - Update of the dEdxAnalyser with several new features
    - optional processor parameters: cuts on momentum
    - fiducial electrons histograms for comparison with beam tests
    - optional processor parameters: properties of fiducial electrons
    - optional processor parameters: axis binning of histograms
    - hit energy histograms, will only be filled if hit info is available
    - dE/dx over |lambda| incl. fit for angular correction

# v01-09

* 2021-02-22 Kollassery Swathi Sasikumar ([PR#32](https://github.com/iLCSoft/ILDPerformance/pull/32))
  - Changes made in DDDiagnostics.cc and DDDiagnostics.h package to include the 2D plots for Tracking efficiency.
  - Two steering files are modified with different cuts as per the requirement for the 1D or 2D plots.

* 2021-02-02 tmadlener ([PR#31](https://github.com/iLCSoft/ILDPerformance/pull/31))
  - Migrate CI from travis to github actions.

# v01-08

* 2020-04-13 Frank Gaede ([PR#30](https://github.com/iLCSoft/ILDPerformance/pull/30))
  - make compatible w/ -std=c++17
        - needed on macos w/ clang

* 2019-12-13 Frank Gaede ([PR#29](https://github.com/iLCSoft/ILDPerformance/pull/29))
  - update of tracking performance scripts for IDR
       - redo tracking performance and efficiency plots w/ HEAD-2019-06-04
         containing the latest improvements to the tracking efficiency by S.Lu
       - add scripts to re-run ttbar sim and rec for efficiency plots

# v01-07

* 2019-05-07 Ete Remi ([PR#28](https://github.com/iLCSoft/ILDPerformance/pull/28))
  - Added dEdX performance scripts
     - `dEdXAnalyzer` processor creates plots
     - Example steering file in `steer` directory
     - Binary `dEdxHistPlotter` to create fancy plots out of the root files produced by the processor
  - See Readme file for more information

* 2019-01-03 Jakob Beyer ([PR#27](https://github.com/iLCSoft/ILDPerformance/pull/27))
  - Add scripts to create TOF-separation plots for single particles (based on Sukees code), including config files for steering.
  - In WWZZSeparation: Remove legacy parameter in plotting script and save TH1s in plotting macro.

* 2018-11-06 Ete Remi ([PR#26](https://github.com/iLCSoft/ILDPerformance/pull/26))
  - Added `-n` option to include/exclude MC neutrino energy. 
  - Added b, c and t quarks to analysis

* 2018-10-01 Jakob Beyer ([PR#25](https://github.com/iLCSoft/ILDPerformance/pull/25))
  - Updated bash scripts, now also possible to run on already reconstructed files (DSTs).
  - Added vector boson mass lines to plots. 
  - Added another FastJet processor to the Marlin script to remove overlay which is included in new production.

# v01-06

* 2018-04-12 Jakob Beyer ([PR#22](https://github.com/ilcsoft/ILDPerformance/pull/22))
  - WWZZSeparation
     - The script can now be executed with the option to run on ILCDirac (assuming the user has a certificate). Now high statistics DSTs can be produced and analyzed. (Has been tested with a total of 15k events.)
     - The output structure was changed and the README was updated.
     - Due to a new naming convention in DSTs the analysis no longer works for ilcsoft older than 01-19-06.

* 2018-04-18 Ete Remi ([PR#23](https://github.com/ilcsoft/ILDPerformance/pull/23))
  - UdsAnalysis:
     - Fixed job script to run on the NAF

* 2018-04-05 Frank Gaede ([PR#21](https://github.com/ilcsoft/ILDPerformance/pull/21))
  - in PIDTree: don't print warnings for particles that are not reconstructed

# v01-05

* 2018-01-31 StrahinjaLukic ([PR#13](https://github.com/iLCSoft/ILDPerformance/pull/13))
  - Added TrackerHitCounter Marlin processor. This is a simple tool to count hits in the tracker elements. It reports the number of hits per run, per event and, where available, per unit area.

* 2017-12-20 Daniel Jeans ([PR#12](https://github.com/iLCSoft/ILDPerformance/pull/12))
  - simple processor to make some single photon validation plots at PFO level
  - number of PFOs, energy, energy resolution

* 2017-11-17 Shaojun Lu ([PR#7](https://github.com/iLCSoft/ILDPerformance/pull/7))
  - Tracking performance scripts for ILCSoft v01-19-05 and detector model ILD_l5_v02.
      - adapt to ILCSoft v01-19-05 and ILDConfig production.
      - in general, user could run "run_ILD_l5_v02_singleMuon.sh" for monitor track fitting.
      - in general, user could run "run_ILD_l5_v02_ttbar.sh" for the pattern recognition efficiency.
      - "README_Tracking.md" for a quick documentation.

* 2018-03-14 Jakob Beyer ([PR#15](https://github.com/iLCSoft/ILDPerformance/pull/15))
  - Adding WW/ZZ separation plots to ILDPerformance.

* 2017-12-12 Frank Gaede ([PR#9](https://github.com/iLCSoft/ILDPerformance/pull/9))
  - updated general documentation (added Usage and Sup-Packages sections)
  - updated PID package
       - updated documentation (now in README.md)
       - make all macros compatible w/ ROOT6
       - improve performance plots for PID efficiency (now all in one pdf file)
       - reduce verbosity in PIDTree.cc
       - remove all compiler warnings  for PIDTree

* 2017-12-12 Shaojun Lu ([PR#8](https://github.com/iLCSoft/ILDPerformance/pull/8))
  - Update ILD Tracking performance scripts
      - adapt to DD4hep and  use "MarlinUtil::getBzAtOrigin()" to access BField.
      - clean up all the warning for code DDDiagnostics.cc
      - improve the tracking performance scripts, to make them more user friendly.
      - remove the out-of-dated scripts for the old Mokka/Marlin
      - adapt to ILCSoft v01-19-05 and ILDConfig v01-19-05-p01 production

* 2017-12-13 Frank Gaede ([PR#10](https://github.com/iLCSoft/ILDPerformance/pull/10))
  - improve PID plots
      - create PID plots in directory of the root tree file 
         allows to create PID plots for different samples in parallel directories
      - add script `run_all.sh` to do just this

* 2018-03-28 Shaojun Lu ([PR#20](https://github.com/iLCSoft/ILDPerformance/pull/20))
  - update to the same bins as DBD for tracking efficiency plots.
  - set RunBeamCalReco=false.
  - update to next release ILCsoft v01-19-06.

* 2018-03-28 Ete Remi ([PR#19](https://github.com/iLCSoft/ILDPerformance/pull/19))
  - Added `UdsAnalysis` directory
     - Replace the old out-dated `JER` directory
     - New binary `ILDPerformance_UdsAnalysis` making plots from PfoAnalysis root files
     - Added NAF2 scripts to get JER/JES plots in a single shot

* 2018-03-26 Shaojun Lu ([PR#18](https://github.com/iLCSoft/ILDPerformance/pull/18))
  - make the scripts more generic to run for ilcsoft HEAD and release installation.
      - remove the release version "v01-19-05" information within the scripts.

* 2017-12-15 Frank Gaede ([PR#11](https://github.com/iLCSoft/ILDPerformance/pull/11))
  - fix **all compiler warnings** in ILDPerformance 
         - gcc 4.9, SL6
         - gcc 6.2, SL6
         - clang 3.9, SL6

* 2018-03-22 Shaojun Lu ([PR#16](https://github.com/iLCSoft/ILDPerformance/pull/16))
  - cleanup the massive monitoring information of tracking. 
  - cleanup the job scripts which are used on DESY NAF2 working nodes.
  - increase the statistic to 5000 single muons for each check point.
  - added RequestRuntime for expected longer runtime on DESY NAF2 working nodes
  - added FTDCollection, and count the true hits number in FTD simulation too.
  - adapt to pixel SIT, and change the name from SITSpacePoints to SITTrackerHits.
  - active minimum silicon hits number cut for MCParticles nominators.
      - count the silicon hits number from all VXD, SIT and FTD for each mcp track.

* 2018-03-23 Shaojun Lu ([PR#17](https://github.com/iLCSoft/ILDPerformance/pull/17))
  - added numberOfEvents 5000 for single Muon to fix simulation 3 events only.

# v01-04

* 2017-11-08 Shaojun Lu ([PR#5](https://github.com/ilcsoft/ILDPerformance/pull/5))
  - New implementation adapted to DD4hep framework for accessing geometry and BField.
      - keep the old with accessing gear geometry and BField as reference.
  - initialise/add more quantity/quality monitor plots.

# v01-03-01

* 2017-11-08 Shaojun Lu ([PR#6](https://github.com/ilcsoft/ILDPerformance/pull/6))
  - Add one script to prepare the working directory.
      - no error if existing, make parent directories as needed.

# v01-03

* 2017-06-20 Andre Sailer ([PR#3](https://github.com/iLCSoft/ILDPerformance/pull/3))
  - ignore warnings from external header files
  - Adapt to changes in namespaces and LCDD -->  Detector

* 2017-06-07 Shaojun Lu ([PR#2](https://github.com/iLCSoft/ILDPerformance/pull/2))
  - "PCut" and "PTCut" have been added as Marlin steerable parameters.
  - Update Diagnostics.xml to set 'PCut' and 'PTCut' to 0.0

* 2017-06-26 Andre Sailer ([PR#4](https://github.com/iLCSoft/ILDPerformance/pull/4))
  - Adapt to dd4hep namespace changes

# v01-02




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
