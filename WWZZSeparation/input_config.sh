#!/bin/bash

#------------------------------------------------------------------------#
#Set paths for stdhep files:
vvXYYX=/pnfs/desy.de/ilc/prod/ilc/mc-dbd/generated/1000-B1b_ws/6f/E1000-B1b_ws.Pvvxyyx.Gwhizard-1.95.eL.pR.I35867.001.stdhep
vvXXXX=/pnfs/desy.de/ilc/prod/ilc/mc-dbd/generated/1000-B1b_ws/6f/E1000-B1b_ws.Pxxxxvv.Gwhizard-1.95.eL.pR.I35911.001.stdhep
vvYYYY=/pnfs/desy.de/ilc/prod/ilc/mc-dbd/generated/1000-B1b_ws/6f/E1000-B1b_ws.Pyyyyvv.Gwhizard-1.95.eL.pR.I35935.001.stdhep

# Set the corresponding cross-sections:
vvXYYX_xsection="50.5806"
vvXXXX_xsection="1.63413"
vvYYYY_xsection="5.39289"
#------------------------------------------------------------------------#

#------------------------------------------------------------------------#
# Set luminosity (in inverse fb)
# and beam polarisations ( - = left-handed, + = right-handed , in % )
LUMINOSITY=1000
E_BEAM_POL=-80
P_BEAM_POL=30
#------------------------------------------------------------------------#


# Return according to argument provided to config
if [[ $1 == "filepaths" ]]; then
	echo "${vvXYYX} ${vvXXXX} ${vvYYYY}"
elif [[ $1 == "cross-sections" ]]; then
	echo "${vvXYYX_xsection} ${vvXXXX_xsection} ${vvYYYY_xsection}"
elif [[ $1 == "luminosity" ]]; then
	echo $LUMINOSITY
elif [[ $1 == "polarisations" ]]; then
	echo ${E_BEAM_POL} ${P_BEAM_POL}
else 
	echo "ERROR in input_config.sh: No valid input argument supplied."
	echo "Exiting."
	exit
fi
