#!/bin/bash

ILDMODEL=ILD_l5_v02
ILCSOFTVER=v01-19-05

. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc49_sl6/${ILCSOFTVER}/init_ilcsoft.sh

root -b -q D0ResolutionL5.C

root -b -q PResolutionL5.C

root -b -q meanL5.C

root -b -q sigmaL5.C

root -b -q EfficiencyL5.C

mkdir -p ~/www/ILDPerformance_${ILCSOFTVER}

cp IPResolution_${ILDMODEL}_${ILCSOFTVER}.png ~/www/ILDPerformance_${ILCSOFTVER}
cp D0_fit_${ILDMODEL}_${ILCSOFTVER}.pdf ~/www/ILDPerformance_${ILCSOFTVER}
cp PResolution_${ILDMODEL}_${ILCSOFTVER}.png ~/www/ILDPerformance_${ILCSOFTVER}
cp PR_fit_${ILDMODEL}_${ILCSOFTVER}.pdf ~/www/ILDPerformance_${ILCSOFTVER}
cp pull_mean_${ILDMODEL}_${ILCSOFTVER}.png ~/www/ILDPerformance_${ILCSOFTVER}
cp pull_sigma_${ILDMODEL}_${ILCSOFTVER}.png ~/www/ILDPerformance_${ILCSOFTVER}

cp trkEff_pt_ttbar_${ILDMODEL}_${ILCSOFTVER}.png ~/www/ILDPerformance_${ILCSOFTVER}
cp trkEff_theta_ttbar_${ILDMODEL}_${ILCSOFTVER}.png ~/www/ILDPerformance_${ILCSOFTVER}
