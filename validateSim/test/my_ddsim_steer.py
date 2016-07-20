from DDSim.DD4hepSimulation import DD4hepSimulation
from SystemOfUnits import mm, GeV, MeV
SIM = DD4hepSimulation()

## The compact XML file
SIM.compactFile = "/home/jeans/ilcsoft/v01-17-09/lcgeo/v00-07/ILD/compact/ILD_o1_v05/ILD_o1_v05.xml"

SIM.enableDetailedShowerMode = True

SIM.enableGun = True

## number of events to simulate, used in batch mode
SIM.numberOfEvents = 5


## Outputfile from the simulation,only lcio output is supported
SIM.outputFile = "dummyOutput.slcio"

## Physics list to use in simulation
SIM.physicsList = "QGSP_BERT"

## Verbosity use integers from 1(most) to 7(least) verbose
## or strings: VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL, ALWAYS
SIM.printLevel = 3

## The type of action to do in this invocation
## batch: just simulate some events, needs numberOfEvents, and input file or gun
## vis: enable visualisation
## run: enable run the macro
## shell: enable interactive session
SIM.runType = "batch"

## Skip first N events when reading a file
SIM.skipNEvents = 0

## Steering file to change default behaviour
SIM.steeringFile = None


################################################################################
## Configuration for the DDG4 ParticleGun 
################################################################################
##  direction of the particle gun, 3 vector 
SIM.gun.direction = (0, 1, 0)
SIM.gun.particle = "gamma"
SIM.gun.multiplicity = 1
SIM.gun.energy = 10.*GeV
##  isotropic distribution for the particle gun 
SIM.gun.isotrop = False
##  position of the particle gun, 3 vector 
SIM.gun.position = (0.0, 0.0, 0.0)

