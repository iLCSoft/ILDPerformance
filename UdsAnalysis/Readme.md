ILDPerformance/UdsAnalysis
===================

JER/JES estimate from uds samples in ILD performance

R. Ete, Feb/Mar 2018

---------------------


## Running uds analysis step by step

### Running ddsim on generator samples

The generator samples for uds JER/JES studies can be found in 

```shell
ls /ilc/user/s/sgreen/StdHepFiles/Z_uds/
```

To run the simulation use the following command line and adjust to your needs :

```shell
ddsim 
  --compactFile $lcgeo_DIR/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml \
  --inputFiles /pnfs/desy.de/ilc/user/s/sgreen/StdHepFiles/Z_uds/200GeV/Zuds200_01.stdhep \
  --outputFile Zuds200_01_SIM.slcio \
  --steeringFile ddsim_steer.py
```

where `ddsim_steer.py` is taken from the `iLCSoft/ILDConfig` package. This step has to be repeated for each energy point you want (40, 91, 200, 360 and 500 GeV makes a nice plot). Each generator file contains 1000 events, so you should maybe also process many files to get enough statistics for the final plots (5 files x 1000 events make nice plots).

### Running the reconstruction

To run the reconstruction use the following command line on the produced file above :

```shell
Marlin MarlinStdReco.xml \
  --constant.lcgeo_DIR=$lcgeo_DIR \
  --constant.DetectorModel=ILD_l5_o1_v02 \
  --global.LCIOInputFiles=Zuds200_01_SIM.slcio \
  --constant.OutputBaseName=Zuds200_01
```

This will create many files (that you are not interested in) and a root file (that you are interested in) called in this case `Zuds200_01_PfoAnalysis.root` output from the `PfoAnalysis` processor part of `PandoraPFA/LCPandoraAnalysis` package.

### Getting the plots

The plots are generated from the root files produced (see above) by running the binary /path/to/ILDPerformance/bin/ILDPerformance_UdsAnalysis. The help shows the following message :

```shell
./bin/ILDPerformance_UdsAnalysis -h

Usage: ILDPerformance_UdsAnalysis 
    -o file      (optional,  output file to write results to. Default is 'ILDPerformance_UdsAnalysis.root')          
    -e energies  (mandatory, input energies column separated, i.e '40:91:200:360:500')                               
    -f pattern   (mandatory, input file pattern. Should contains %{energy} key, i.e ./PfoAnalysisUds_%{energy}.root) 
    -t treename  (optional,  the tree name to look in root files, usually PfoAnalysisTree)
```

In case you end up with multiple root file per energy point you can merge them by using the root utility `hadd` as e.g:

```shell
hadd Zuds200_PfoAnalysis.root Zuds200_01_PfoAnalysis.root Zuds200_02_PfoAnalysis.root Zuds200_03_PfoAnalysis.root 
```

This will combine the three files into one called `Zuds200_PfoAnalysis.root`.

To use the ILDPerformance_UdsAnalysis with, e.g, 5 energy points on merged root files, use the following command:

```shell
./bin/ILDPerformance_UdsAnalysis -o UdsAnalysis.root -e 40:91:200:360:500 -f Zuds%{energy}_PfoAnalysis.root -t PfoAnalysisTree
```

producing the final root file `UdsAnalysis.root` with all your plots inside


## Running the job scripts on NAF2

Job scripts have been prepared to run the simulation, reconstruction and analysis in a single shot script with `qsub`. Use the `submit-qsub-full-jer-jobs` script to submit the full analysis in a single shot. This script requires a steering file (the only argument) describing the settings of your local machine. A template is provided in `./steer/default-settings.conf`. The different fields are documented in the file directly, so please read the template as documentation.

You can copy the template and modify it as you need:

```shell
cp ./steer/default-settings.conf my-settings.conf
# then edit as you need
```

You are ready the submit your jobs:

```shell
./scripts/submit-qsub-full-jer-jobs ./my-settings.conf
```

Enjoy your fantastic JER/JES plots !

