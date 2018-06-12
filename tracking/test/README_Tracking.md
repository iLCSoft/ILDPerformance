##########################################
# Ready for ILCSoft v02-00-01 and ILD_l5_v02
#
You may modify them for others.


##########################################
# prepapre ILDConfig files
#
```shell
cp -fr ILDConfig/StandardConfig/production/* .
```

##########################################
# prepapre working folders
#
```shell
./prepareWorkingFolders.sh
```

##########################################
# user may generate monitor plots
#
```shell
./run_singleMuon.sh
./run_ttbar.sh
```


##########################################
# generate monitor plots in one go for DESY
#
```shell
ssh -X naf-ilc11.desy.de (NAF2 HTCondor)
./submit_job.sh
```