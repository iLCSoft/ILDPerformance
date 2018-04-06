# Check if ILCDirac is loaded by testing if ilcdirac-version command exists
if ! type "ilcdirac-version" &> /dev/null; then
  # Doesn't exist, load ILCDirac
  source /cvmfs/clicdp.cern.ch/DIRAC/bashrc
  dirac-proxy-init -g ilc_user
fi

# Test if the versions are available on ILCDirac
grid_versions=$(dirac-ilc-show-software)
if ! [[ $grid_versions == *" ${ILCSOFT_VERSION} "* ]]; then
  echo "ERROR in initialize_ilcdirac.sh: iLCsoft version not available on grid!"
  echo "Exiting now."
  exit
elif ! [[ $grid_versions == *" ${ILDCONFIG_VERSION} "* ]]; then
  echo "ERROR in initialize_ilcdirac.sh: ILDConfig version not available on grid!"
  echo "Exiting now."
  exit
fi
