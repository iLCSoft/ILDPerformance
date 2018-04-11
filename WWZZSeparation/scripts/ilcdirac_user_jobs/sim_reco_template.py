#!/usr/bin/python

from DIRAC.Core.Base import Script
Script.parseCommandLine()
from ILCDIRAC.Interfaces.API.NewInterface.UserJob import UserJob
from ILCDIRAC.Interfaces.API.NewInterface.Applications import DDSim, Marlin
from ILCDIRAC.Interfaces.API.DiracILC import DiracILC
import os, random, subprocess, sys


def diracpath_from_pnfspath ( pnfs_path ):
    # Reformat /pnfs/desy.de/... path to be usable for ILCDirac
    split_string = pnfs_path.split("/")
    recombined=""
    for part in split_string:
        if not ( part == "pnfs" or part == "desy.de" or part == "" ) :
            recombined += "/" + part
    return "LFN:" + recombined

def detector_model_wo_option ( model ):
    # detector model name for the simulation does not need the option "o01"...
    split_string = model.split("_")
    recombined=""
    for part in split_string:
        if not part[0] == "o":
            recombined += part + "_"
    return recombined[:-1]

def main(argv):
    # Input arguments
    ildconfig_version   = "$ILDCONFIGVER"
    ilcsoft_version     = "$ILCSOFTVER"

    evts_per_run    = $EVTSPERRUN
    detector_model  = "$DETECTOR"
    sim_input       = "$SIMINPUT"
    process_name    = "$PROCESS"

    index           = $IND

    sim_input = diracpath_from_pnfspath( sim_input )
    sim_detector_model = detector_model_wo_option( detector_model )

    job_group = ilcsoft_version + "_" + ildconfig_version + "_" + process_name + "_" + detector_model
    dirac = DiracILC(True,job_group+".rep")

    # outputs to be saved onto grid SE
    RECoutput = []

    # DDSim

    evtStart   = (index-1)*evts_per_run
    evtEnd     = index*evts_per_run - 1
    RandSeed = random.randrange(11623, 99999)

    lcinputSIM  = "LFN:" + sim_input
    lcoutputSIM = ilcsoft_version + ".ILDConfig_" + ildconfig_version + ".E1000." + process_name + ".eLpR.evt%s-%s_SIM.slcio"%(str(evtStart),(str)(evtEnd))

    sim = DDSim()
    sim.setVersion(ilcsoft_version)

    sim.setDetectorModel(sim_detector_model)
    sim.setInputFile(lcinputSIM)
    sim.setSteeringFile("ddsim_steer.py")
    sim.setNumberOfEvents(evts_per_run)
    sim.setRandomSeed(RandSeed)
    sim.setEnergy(1000)
    sim.setStartFrom(evtStart)
    sim.setOutputFile(lcoutputSIM)

    # Marlin
    lcoutputDST = ilcsoft_version + ".ILDConfig_" + ildconfig_version + ".E1000." + process_name + ".eLpR.evt%s-%s_DST.slcio"%(str(evtStart),(str)(evtEnd))

    ma = Marlin()
    ma.setVersion(ilcsoft_version)
    ma.setDetectorModel(detector_model)
    ma.setSteeringFile("MarlinStdReco.xml")
    ma.setExtraCLIArguments( "--constant.lcgeo_DIR=$lcgeo_DIR --constant.DetectorModel={} --global.MaxRecordNumber=0".format(detector_model) )
    ma.setLogFile("marlin.log")
    ma.getInputFromApp(sim)
    ma.setEnergy(1000)
    ma.setOutputDstFile(lcoutputDST)

    RECoutput.append(lcoutputDST)

    # ILCDirac user job
    job = UserJob()
    job.setName("user_sim_reco")

    job.setJobGroup(job_group)

    job.setILDConfig(ildconfig_version)
    job.setCPUTime(86400)

    tmp_file_name = process_name + "_sim_reco_job_tmp.py"
    job.setInputSandbox([tmp_file_name])
    job.setOutputSandbox(["*.log","MarlinStdRecoParsed.xml","marlin*.xml","*.py "])

    job.dontPromptMe()
    job.setBannedSites(['LCG.IN2P3-CC.fr','OSG.UConn.us','LCG.Cracow.pl','OSG.MIT.us','LCG.Glasgow.uk','OSG.CIT.us','OSG.BNL.us','LCG.Brunel.uk','LCG.RAL-LCG2.uk','LCG.Oxford.uk','OSG.UCSDT2.us'])

    # run simulation job
    simres = job.append(sim)
    if not simres['OK']:
            print simres['Not ok appending ddsim to job']
            quit()


    # run Malrin reco jobs
    mares = job.append(ma)
    if not mares['OK']:
            print mares['Not ok appending Marlin to job']
            quit()

    # TODO Change this!!!
    ######################################################
    job.setOutputData(RECoutput,"test/6_WWZZSeparation/{}_ILDConfig_{}_{}".format(ilcsoft_version,ildconfig_version,detector_model),"DESY-SRM")
    ######################################################
    print RECoutput

    submit_output = job.submit(dirac)
    print submit_output


if __name__ == "__main__":
	main(sys.argv[1:])
