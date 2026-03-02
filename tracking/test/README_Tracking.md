# ILD Performance Configuration

**Status:** Ready for `ILCSoft v01-19-05` and `ILD_l5_v02`.  
_Note: These configurations may be modified for other versions._

---

## 1. Prepare ILDConfig Files

Copy the standard production configuration files to the current directory:

```shell
cp -fr ILDConfig/StandardConfig/production/* .
```

## 2. Prepare Working Folders

Initialize the necessary directory structure:

```shell
./prepareWorkingFolders.sh
```

## 3. Generate Monitor Plots

Users can generate performance monitor plots for specific samples:

```shell
# For Single Muon samples
./run_ILD_l5_v02_singleMuon.sh

# For ttbar samples
./run_ILD_l5_v02_ttbar.sh
```

## 4. Batch Generation (DESY NAF)

To generate monitor plots in a single batch on the DESY infrastructure (NAF2 HTCondor):

```shell
# Connect to the NAF node
ssh -X naf-ilc11.desy.de

# Submit the jobs
./submit_job.sh
```
