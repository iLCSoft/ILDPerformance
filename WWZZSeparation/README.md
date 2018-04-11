# WWZZPerformance
(J. Beyer, DESY)

Script to create performance plots of *WW/ZZ* separation in their hadronic decays using *2neutrino+4quark* samples at 1TeV.

This is part of the ILDPerformance package.

## Running the whole performance script chain

### 1. Initialize ilcsoft and compile ILDPerformance
Follow the instructions in the README to ILDPerformance.
Compiling ILDPerformance will automatically include compiling the processor for this analysis.

### 2. Set input file paths and ILC run settings
Open the *input_config.sh* file and change/set the input *.stdhep* files as well as the luminosity and beam polarisations of the run.
If the input *.stdhep* files are changed make sure the respective cross-sections (also in *input_config.sh*) are still correct or adapt them.



### 3. Run the main execution script
All scripts are of this packaged are centrally made executable and called using the *run_performance_check.sh* script.
First make it executable:
```shell
chmod u+x run_performance_check.sh
```

#### 3.1 Running locally
The full script can be run on the local machine (assuming *ilcsoft* is installed). Then everything will be run with the local *ilcsoft* version and the newest ILDConfig version. Local execution can be done using:
```shell
./run_performance_check.sh --numberOfEvents=number_of_events --DetectorModel=detector_model
```
- *number_of_events*: Number of events generated and anaylsed *for each* input file. Default is 10 (which is not enough for a good performance plot).
- *detector_model*: Detector model used in the simulation. Default is ILD_l5_o1_v02.


#### 3.2 Running with ILCDirac

(Tutorial to *ILCDirac* for FLC on the [FLC wiki](http://flcwiki.desy.de/ILCDirac) )

While possible it is not recommended to run this script on the local machine (significant statistics will take too long). To allow faster running of the detector simulation and reconstruction the script makes use of *ILCDirac*. To run with *ILCDirac* use:
```shell
./run_performance_check.sh --numberOfEvents=number_of_events --DetectorModel=detector_model --UseILCDirac ilcsoft_version ildconfig_version
```
- *ilcsoft_version*: A version of *ilcsoft* supported by *ILCDirac*.
- *ildconfig_version*: A version of *ILDConfig* known to *ILCDirac*.

**Assumptions/Requirements:**
- The detector model name should be like *ILD_l5_o2_v02* , the *o2* part will be automatically removed for the detector simulation.
- The *.stdhep* file paths should begin with */pnfs/desy.de* , they are automatically adjusted to useful to *ILCDirac*.
- The user running the script must have a *ILCDirac* grid certificate, know their password and ideally run from the DESY-NAF.
- **The script can only be run ONCE!** per detector model - ilcsoft version - ILDConfig version - combination. To run it again (if absolutely necessary, creates new DSTs) change the output path for the slcio files (see below). 

**slcio output path:** The output path in your *pnfs/desy.de/...* directory is set in the *scripts/ilcdirac_user_jobs/sim_reco_template.py* script (almost at the end of the script).

**Note:** Even though a version of *ilcsoft* is given as input the analysis of the *DST* files happens within the locally installed version!

#### 3.3 Note to old ilcsoft versions

*ilcsoft* versions older than 01-19-06 will not work with the analysis of the output DST files. The reason is a changed naming convention in the MCParticles collection (changed from *MCParticleSkimmed* to *MCParticle*).


### (4.) Readjusting the plot

If the layout of the performance plots should be changed this can be done by either of the following:

- Changing your *.rootlogon.C* file in your home directory.
- Changing the end of the *create_mjjmjjplots.cc* script in the *plotting* directory.

After doing so simply re-run the plotting inside the *scripts* directory using:
```shell
./run_plotting.sh --InputBase=output_subdirectory
```
(assuming *run_plotting.sh* is still executable).

- *output_subdirectory*: Absolute path of the subdirectory in the *output* directory that contains the performance plots and process subdirectories.

In the same manner it is possible to change the luminosity and beam polarisations (they only enter in the plotting) and simply re-run the plotting.

##  Output
The output will be in the *output* directory in the subdirectory of the specified detector model and software versions.
It will contain:

- Process subdirectories, each will contain:
  - the *DST* files created in the simulation+reconstruction,
  - the *.root* file result from the analysis,
  - when using *ILCDirac* the *.rep* and submission script.
- *m.pdf* : This is the 1D performance plot.
- *m_m.pdf* : This is the 2D performance plot.


## Physics of the performance plots

The analysis is largely similar to the one in the [ILD Letter of Intent](https://arxiv.org/abs/1006.3396) (chapter 3.3.6).

Differences are:

- The event selection includes a cut on isolated leptons.
- The pairing of the 4 jets to two boson candidates uses a different criterium.




















.
