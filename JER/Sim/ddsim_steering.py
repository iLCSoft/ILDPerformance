from DDSim.DD4hepSimulation import DD4hepSimulation
from SystemOfUnits import mm, GeV, MeV

SIM = DD4hepSimulation()

SIM.runType = "batch"

SIM.skipNEvents = 0

SIM.field.eps_min = 1*mm

SIM.part.minimalKineticEnergy = 1*MeV

SIM.physicsList = "QGSP_BERT"
   
SIM.enableDetailedShowerMode=True

SIM.enableGun = True

SIM.gun.energy = ENERGY*GeV
SIM.gun.particle = "PARTICLE"
SIM.gun.position = "0,0,0"
SIM.gun.isotrop = True
SIM.gun.direction = "0,0,1"
SIM.random.seed = RANDOMSEED
