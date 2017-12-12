
# ILDPerformance/PID

Performance plots for particle ID.

-  *J.List, Sept/Oct 2015*
-  *Peter McKeown, Aug, 2016*
-  *F.Gaede, 2016/17* 
 
## Introduction

PID plots can be created for any physics sample or single particle files - either from DST or REC files.
All plots are created from a ROOT TTree which has to be created with the PIDTree processor first.


### Prerequesites

```sh 
# init iLCSoft, e.g.
source /cvmfs/ilc.desy.de/sw/x86_64_gcc49_sl6/v01-19-05/init_ilcsoft.sh

cd ILDPerformance/PID

# IMPORTANT: create an output directory for the plots, e.g. 
mkdir Results
```



Now you can create the required Root tree:

```sh    	
Marlin ./scripts/PIDTree.xml --global.LCIOInputFiles=myttbarfile.slcio --MyAIDAProcessor.FileName=./Results/PID_tree
```

This creates the file `./Results/PID_tree.root`.

As ususal, you can of course change parameters via the command line or edit the steering file accordingly.


### PID performance plots

The ROOT macros for creating performance plots are in *./macros*, e.g.:    

```sh
cd ./macros
root -l
```
     
     .x plotPIDs.C("../Results/PID_tree.root");

This creates the following pdf files:
```
../Results/PIDs_pdg11_cut9.pdf
../Results/PIDs_pdg13_cut9.pdf
../Results/PIDs_pdg211_cut9.pdf
../Results/PIDs_pdg2212_cut9.pdf
../Results/PIDs_pdg321_cut9.pdf
```
with the PID from four different algorithms for p>3GeV particles.
You can adjust the cut in the macro and optionally create .eps and .ps files.

  
#### PID efficiency and fake rate

The following creates one pdf file with efficiency and fake rate as function of momentum and polar angle 
for all PID algorithms for the ususal particle types
(in *../Results/PID_efficiencies_all.pdf*) :

```sh
root -b -q 'plotPIDEfficiency.C("../Results/PID_tree.root")'
```



#### dEdx plots

     root -b -q 'plotdEdx.C("../Results/PID_tree.root")'

creates a plot of the actual dEdx in the TPC that is used for the likelihoodPID 
(in file: *../Results/dEdx_plots_output.pdf*)


#### Likelihod  plots

    root -b -q 'plotPIDLike.C("../Results/PID_tree.root")'


creates plots for the PID-likelihood for the different particle types 
for the correct true particles (blue) and all other particles (red)
( in file: *../Results/Likely_PDG.pdf*)
	
	
#### Efficiency vs Purity	

	root -b -q 'plotPIDPurity.C ("../Results/PID_tree.root")'

creates plots of efficiency vs purity from using the PID likelihood directly
(in file: *../Results/Eff_Purity.pdf*)


--------------------------------------------------------------------------------------

## Create all plots

You can also create all plots for a given LCIO file in on go, e.g.:

```sh
cd ILDPerformance/PID

. ./run_all.sh /pnfs/desy.de/ilc/prod/ilc/mc-opt/ild/rec/calib/uds/ILD_l5_o1_v02_nobg/v01-19-05-p01/u035/rv01-19-05-p01.sv01-19-05.mILD_l5_o1_v02_nobg.Puds500.n07.d_rec_u035.slcio PID_uds500
```

this creates the root tree and pdf files with plots in directory *PID_uds500*



	



    	    	
    

    
	
