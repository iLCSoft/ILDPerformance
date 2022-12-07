# ILDPerformance/dEdx

Performance plots for dE/dx.

- U. Einhaus, Dec 2022, v1.3

## Overview

The content is a processor, the *dEdxAnalyser* processor, and a root script, the *ILDPerformance_dEdxHistPlotter*, which can be compiled to an executable.
The processor can be run with the attached steering file steer_example.xml.


## dEdxAnalyserProcessor

The *dEdxAnalyser* processor loops through tracks to collect their dE/dx, fills various histograms and calculates dE/dx resolution and separation power.
Everything is stored in an AIDA-generated root-file.
Various plots can be automatically produced in the current working directory.

The required collections are Tracks, a TrackMCTruthLink and the corresponding MCParticles.
If tracks attached to PFOs should be used, also a PFO collection is required.
The number of particle species to be investigated and their properties are hard-coded and must be set:
*_nPart* is the number of species, set in this header, while the PDG number and the names of the particles (for labeling the histograms) are set in the init().
This version: *_nPart* = 5; species: electrons, muons, pions, kaon, protons.

First, in processEvent(), the dE/dx values of each track is filled in the corresponding histogram, resulting in Bethe-Bloch curves.
The correct PDG is received from the MCParticle linked to the track.
In end(), these histograms are then fitted with FitSlices, and the resulting mean and sigma values for each momentum bin are used to calculate resolution and separation power.
The separation power is calculated for each combination of the *_nPart* species.
All histograms, including the auto-generated fit results, are stored via AIDA in a root file.
Finally, many of the histograms can be plotted and saved as images to the current working directory.

Several special histograms for calibration purposes are provided.
The FiducialElectrons histograms collects electrons which are similar to electrons at the test beam. The resulting dE/dx resolution in ResNorm_FiducialElectrons should be used to calibrate the smearing factor in the Compute_dEdxProcessor.
The histogram NormLambdaFullAll_1 contains the mean dE/dx over |lambda| incl. a poly3 fit, which is used in the AngularCorrection_dEdxProcessor for correction of the angular dependence.
The histogram NormCosThFullAll_1 contains the mean dE/dx over cos(theta), and shows the information of NormLambdaFullAll_1 over an alternative angular scale.
The Bethe-Bloch curves in BBHist are fitted to provide the parameters of the reference curves for the dEdxPID in the LikelihoodPIDProcessor.
The fit results are stored with the histograms in the root file and also printed on the console at the end of the processor and in a file dEdxAnalyser_CalibrationOutput.txt in the current working directory.

You can run this Analyser over any selection of tracks, e. g. only pion and kaon tracks.
In that case, only the pion and kaon resolution as well as only the pion-kaon separation would give sensible plots, but all other plots would be generated empty alongside.
The Analyser has mostly been used to extract the dE/dx performance from single-particle random-momentum files, but also to check e. g. 6f-ttbar events.

Note that the hit energy histograms will only be filled, if the track hits are available, which is usually only the case for REC-files.
The hit number histograms are unaffected by this.

General processor parameters:

- **plotStuff** - Set true to switch on automatic image generation of many histograms in the current working directory.
  bool, default: false.
- **fileFormat** - Select the file extension to be used for the automatically generated images.
  Selection depends on root TPad::Print() capabilities, typically: [.ps, .eps, .pdf, .svg, .tex, .gif, .xpm, .png, .jpg, .tiff, .cxx, .xml, .json, .root].
  See https://root.cern.ch/doc/master/classTPad.html#ad2fc5f449e4cb5480b9fb05fda3a8307 for details.
  string, default: .png

- **usePFOTracks** - Set true to use tracks attached to charged PFO, instead of all tracks in the track collection. This should be used with events which are more busy than single particle files.
  bool, default: false.
- **useOneTrack** - Set true if from every event only the first object in the selected collection (track or PFO) should be used. Usually, don't combine this with _usePFOTracks.
  bool, default: false.
- **usePFOTruthLink** - Set true if the LCRelation from PFO to MCTruth instead of Track to MCTruth should be used to determine MC PDG. The name of the LCRelation still needs to be set in the TrackMCTruthLink parameter.
  bool, default: false.

You can make a track selection via the optional parameters, e. g. to reduce 'contamination' from mis-linked Tracks and MCParticles.

- **cutdEdx** - Set true if particles with a very off dE/dx value (hard-coded) should be ignored.
  bool, default: false.
- **cutTrackPurity** - Set true if particles should be ignored, where the dominating MCParticle is connected to more than one track.
  bool, default: false.
- **cutTrackPurityMom** - Set true if particles should be ignored, where the dominating MCParticle is connected to more than one track, but the track with the highest momentum should still be used.
  bool, default: false.
- **cutD0** - Tracks with a d0 larger than the given value will be ignored. Set to 0 to accept all particles.
  double, default: 0.
- **cutZ0** - Tracks with a z0 larger than the given value will be ignored. Set to 0 to accept all particles.
  double, default: 0.
- **cutMomMin** - Tracks with a momentum smaller than the given value will be ignored. Set to 0 to accept all particles.
  double, default: 0.
- **cutMomMax** - Tracks with a momentum larger than the given value will be ignored. Set to 0 to accept all particles.
  double, default: 0.
 
For calibration with beam test results, so-called fiducial electrons are selected, which ought to be similar to beam-test conditions, and their properties stored in separate histograms.
lambda is the track angle with respect to the cathode.

- **fidMomMin** - Fiducial minimum absolut momentum / GeV.
  double, default: 3.
- **fidMomMax** - Fiducial maximum absolut momentum / GeV.
  double, default: 10.
- **fidLamMin** - Fiducial minimum absolut lambda / deg.
  double, default: 1.
- **fidLamMax** - Fiducial maximum absolut lambda / deg.
  double, default: 10.
- **fidNHitsMin** - Fiducial minimum number of hits.
  int, default: 200.

For some of the histograms, the axis binning can be adapted, which can be useful in case of very high or low statistics.

- **nBinsX** - Number of bins of the momentum axis in histograms.
  int, default: 100.
- **nBinsY** - Number of bins of the dE/dx axis in histograms.
  int, default: 300.
- **minBinX** - log_10(momentum/GeV) of the lower end of the momentum axis in histograms.
  double, default: -1 (= 0.1 GeV).
- **maxBinX** - log_10(momentum/GeV) of the upper end of the momentum axis in histograms.
  double, default: 2 (= 100 GeV).
- **minBinY** - Lower end of the dE/dx axis in GeV/mm in histograms.
  double, default: 0.
- **maxBinY** - Upper end of the dE/dx axis in GeV/mm in histograms.
  double, default: 1e-6.


## dEdxHistPlotter

The dEdxHistPlotter.cc is compiled to *ILDPerformance_dEdxHistPlotter*.
It can be executed via *bin/ILDPerformance_dEdxHistPlotter*.

The *ILDPerformance_dEdxHistPlotter* runs through the output root-file of the *dEdxAnalyser* processor and produces plots of all TH1 and TH2 therein.
The difference to the processor is that the plots receive a specific ILD-like formatting.
Also, the plotter works more generically and goes through all (sub)directories of the root-file and makes plots of all TH1 and TH2.
The structure of the (sub)directories is recreated in the target folder.

The executable takes 2 necessary plus 1 optional command line inputs:
1. The root file generated by the *dEdxAnalyser* processor.
2. The target folder, in which the plots are created. This folder must exist.
3. Optionally, the file format of the plots can be specified. Default: .png

Examples:
```shell
> ./bin/ILDPerformance_dEdxHistPlotter /path/to/root/file.root /path/to/target/folder/for/plots/
> ./bin/ILDPerformance_dEdxHistPlotter /path/to/root/file.root /path/to/target/folder/for/plots/ .gif
```
