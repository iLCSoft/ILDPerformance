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
First make it executable, then run it:
```shell
chmod u+x run_performance_check.sh
./run_performance_check.sh --numberOfEvents=number_of_events --DetectorModel=detector_model
```
- *number_of_events*: Number of events generated and anaylsed *for each* input file. Default is 10 (which is not enough for a good performance plot).
- *detector_model*: Detector model used in the simulation. Default is ILD_l5_o1_v02.

#### 3.1  Output
The output will be in the *output* directory in the subdirectory of the specified detector model.
It will contain:

- *.root* files for each input process. This is so that the plotting can be redone easily.
- *m.pdf* : This is the 1D performance plot.
- *m_m.pdf* : This is the 2D performance plot.

Explanation of the plots can be found below.
### (4.) Readjusting the plot

If the layout of the performance plots should be changed this can be done by either of the following:

- Changing your *.rootlogon.C* file in your home directory.
- Changing the end of the *create_mjjmjjplots.cc* script in the *plotting* directory.

After doing so simply re-run the plotting inside the *scripts* directory using:
```shell
./run_plotting.sh --DetectorModel=detector_model
```
(assuming *run_plotting.sh* is still executable).

In the same manner it is possible to change the luminosity and beam polarisations (they only enter in the plotting) and simply re-run the plotting.

## Physics of the performance plots

The analysis is largely similar to the one in the [ILD Letter of Intent](https://arxiv.org/abs/1006.3396) (chapter 3.3.6).

Differences are:

- The event selection includes a cut on isolated leptons.
- The pairing of the 4 jets to two boson candidates uses a different criterium.




















.

