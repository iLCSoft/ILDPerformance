#!/bin/bash

ILDMODEL=ILD_l5_v02
ILCSOFTVER=v01-19-05

. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc49_sl6/${ILCSOFTVER}/init_ilcsoft.sh

root -b -q D0ResolutionL5.C

root -b -q PResolutionL5.C

root -b -q meanL5.C

root -b -q sigmaL5.C

root -b -q EfficiencyL5.C

OUTPUTPATH= ~/www/ILDPerformance_${ILCSOFTVER}
mkdir -p ${OUTPUTPATH}

cp IPResolution_${ILDMODEL}.png ${OUTPUTPATH}/IPResolution_${ILDMODEL}_${ILCSOFTVER}.png
cp D0_fit_${ILDMODEL}.pdf ${OUTPUTPATH}/D0_fit_${ILDMODEL}_${ILCSOFTVER}.pdf
cp PResolution_${ILDMODEL}.png ${OUTPUTPATH}/PResolution_${ILDMODEL}_${ILCSOFTVER}.png
cp PR_fit_${ILDMODEL}.pdf ${OUTPUTPATH}/PR_fit_${ILDMODEL}_${ILCSOFTVER}.pdf
cp pull_mean_${ILDMODEL}.png ${OUTPUTPATH}/pull_mean_${ILDMODEL}_${ILCSOFTVER}.png
cp pull_sigma_${ILDMODEL}.png ${OUTPUTPATH}/pull_sigma_${ILDMODEL}_${ILCSOFTVER}.png

cp trkEff_pt_ttbar_${ILDMODEL}.png ${OUTPUTPATH}/trkEff_pt_ttbar_${ILDMODEL}_${ILCSOFTVER}.png
cp trkEff_theta_ttbar_${ILDMODEL}.png ${OUTPUTPATH}/trkEff_theta_ttbar_${ILDMODEL}_${ILCSOFTVER}.png
