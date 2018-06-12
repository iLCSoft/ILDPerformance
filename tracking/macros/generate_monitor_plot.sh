#!/bin/bash

ILDMODEL_REC=ILD_l5_o1_v02
ILCSOFTVER=v02-00-01

. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc49_sl6/${ILCSOFTVER}/init_ilcsoft.sh
#. /cvmfs/ilc.desy.de/sw/x86_64_gcc49_sl6/${ILCSOFTVER}/init_ilcsoft.sh

root -b -q D0ResolutionL5.C

root -b -q Z0ResolutionL5.C

root -b -q PResolutionL5.C

root -b -q meanL5.C

root -b -q sigmaL5.C

root -b -q EfficiencyL5.C

OUTPUTPATH= ~/www/ILDPerformance_${ILCSOFTVER}_${ILDMODEL_REC}
mkdir -p ${OUTPUTPATH}

cp D0Resolution.png ${OUTPUTPATH}/D0Resolution_${ILDMODEL_REC}_${ILCSOFTVER}.png
cp D0_fit.pdf ${OUTPUTPATH}/D0_fit_${ILDMODEL_REC}_${ILCSOFTVER}.pdf
cp Z0Resolution.png ${OUTPUTPATH}/Z0Resolution_${ILDMODEL_REC}_${ILCSOFTVER}.png
cp Z0_fit.pdf ${OUTPUTPATH}/Z0_fit_${ILDMODEL_REC}_${ILCSOFTVER}.pdf
cp PResolution.png ${OUTPUTPATH}/PResolution_${ILDMODEL_REC}_${ILCSOFTVER}.png
cp PR_fit.pdf ${OUTPUTPATH}/PR_fit_${ILDMODEL_REC}_${ILCSOFTVER}.pdf
cp pull_mean.png ${OUTPUTPATH}/pull_mean_${ILDMODEL_REC}_${ILCSOFTVER}.png
cp pull_sigma.png ${OUTPUTPATH}/pull_sigma_${ILDMODEL_REC}_${ILCSOFTVER}.png

cp trkEff_momentum_ttbar.png ${OUTPUTPATH}/trkEff_momentum_ttbar_${ILDMODEL_REC}_${ILCSOFTVER}.png
cp trkEff_pt_ttbar.png ${OUTPUTPATH}/trkEff_pt_ttbar_${ILDMODEL_REC}_${ILCSOFTVER}.png
cp trkEff_theta_ttbar.png ${OUTPUTPATH}/trkEff_theta_ttbar_${ILDMODEL_REC}_${ILCSOFTVER}.png
