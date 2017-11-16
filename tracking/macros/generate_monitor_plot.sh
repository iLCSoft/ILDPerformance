#!/bin/bash

ILCSOFTVER=v01-19-05

. /afs/desy.de/project/ilcsoft/sw/x86_64_gcc49_sl6/${ILCSOFTVER}/init_ilcsoft.sh

root -b -q D0ResolutionL5.C

root -b -q PResolutionL5.C

root -b -q meanL5.C

root -b -q sigmaL5.C

#root -b -q EfficiencyL5.C
