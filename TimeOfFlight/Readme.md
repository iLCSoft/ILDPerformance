# ILDPerformance/TimeOfFlight

Time Of Flight (TOF) for particle separation

S. Dharani,
F. Gaede, 
J. Beyer

## Running Time Of Flight separation: Step-by-step

The scripts provided run on REC (`.slcio`) files, create LCTuple root files and run root macros to investigate the separation power.
Currently only the Closest Hits TOF-approach is taken into account.

### Single particle events

**0. Source iLCSoft**

**1. Setting the configuration**

The configuration files can be found in ```./config```.
- `ParticleTypes.cnf` : Set the single particle types that should be investigated.
- `PlottingSettings.cnf` : Set the binning for the beta-momentum plots (May be relevant for whether Gaussian fits succeed).
- `Setup.cnf` : Set the environmental parameters for the setup that should be run (e.g. versions used in slcio file production, detector model).
- `TimeResolutions.cnf` : Set the time resolutions that should be investigated. Only resolutions available in the REC files can be used.

The `Setup.cnf` can be changed by simply manipulating the entries.
To add other particle types or resolutions copy-paste and manipulate an entry in `ParticleTypes.cnf` or `TimeResolutions.cnf`, respectively.

**2. Running the Marlin and plotting**

All steps necessary to get the separation power plots are summarized in one script. This must first be made executable:
```bash
chmod u+x macros/run_singleparticle.sh
```
and can then be run like:
```bash
./macros/run_singleparticle.sh
```

The plots (and the LCTuple root files) can then be found in the appropriate directory in the `output` directory.

### ttbar events

Not yet implemented, only plotting macro and Config included.

### Comments

- The scripts that read the configuration (`run_singleparticle_lctuple.sh` and `run_singleparticle_plotting.sh`) require Python3. 
  In the scripts the exact version that is used is hardcoded.
- The REC files provided to the scripts must already contain the TOF information for each particle.
- The REC files must follow the ILD file naming conventions.
- Not all possible possibilities are tested (e.g. different TOF methods, ttbar) as of now.