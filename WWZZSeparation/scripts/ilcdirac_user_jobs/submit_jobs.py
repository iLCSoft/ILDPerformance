#!/usr/bin/python

import sys, os, subprocess, getopt
from string import Template
import math

def print_usage():
    print "In submit_jobs.py:"
    print "Usage: python submit_jobs.py [options]"
    print "Options:"
    print "-h Help"
    print "-N / --numberOfEvents number_events"
    print "--DetectorModel detector_model"
    print "--stdhepInput stdhep_input_file_path"
    print "--processName process_name"
    print "--ILCSoftVer ilcsoft_version"
    print "--ILDConfigVer ildconfig_version"

def main(argv):
    """ Using the template for the ilcdirac submit script create temporary
    scripts using all given parameter. Split up all events over several scripts
    so they can be run in parallel. """

    n_events = 10
    detector_model = 'ILD_l5_o1_v02'
    sim_input = '/pnfs/desy.de/ilc/prod/ilc/mc-dbd/generated/1000-B1b_ws/6f/E1000-B1b_ws.Pxxxxvv.Gwhizard-1.95.eL.pR.I35911.001.stdhep'
    process_name = 'test_name'
    ildconfig_version  = "v01-19-05-p01"
    ilcsoft_version    = "ILCSoft-01-19-05_gcc49"

    try:
        # Read command line arguments, for details see documentation of getopt.getopt
        opts, args = getopt.getopt(argv,"hN:",["numberOfEvents=","DetectorModel=","stdhepInput=","processName=","ILCSoftVer=","ILDConfigVer="])
    except getopt.GetoptError:
        print_usage()
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print_usage()
            sys.exit()
        elif opt in ("-N", "--numberOfEvents"):
            n_events = int(arg)
        elif opt in ("--DetectorModel"):
            detector_model = arg
        elif opt in ("--stdhepInput"):
            sim_input = arg
        elif opt in ("--processName"):
            process_name = arg
        elif opt in ("--ILCSoftVer"):
            ilcsoft_version = arg
        elif opt in ("--ILDConfigVer"):
            ildconfig_version = arg

    # Open Template file and read in lines
    fR = file("sim_reco_template.py","r")
    lines = fR.read()
    fR.close()

    if ( n_events > 99. ):
        n_jobs = 100
    elif ( n_events > 9 ):
        n_jobs = 10
    else:
        n_jobs = 1

    # Number of events will always be rounded up to next i*n_jobs (i is integer)
    evts_per_run = int(math.ceil( float(n_events) / n_jobs ))

    # Replace lines in template with arguments, use safe_substitute because not all values are replaced
    lines_w_args = (Template(lines)).safe_substitute(ILDCONFIGVER=ildconfig_version,ILCSOFTVER=ilcsoft_version,EVTSPERRUN=evts_per_run,DETECTOR=detector_model,SIMINPUT=sim_input,PROCESS=process_name)

    # List for subprocess objects to track whether they are done
    subprocesses = []

    tmp_file_name = process_name + "_sim_reco_job_tmp.py"

    for ind in range(1, n_jobs+1): #1 to n_jobs
        stren = str(ind)
        t = Template(lines_w_args)
        if os.path.exists(tmp_file_name):
            os.remove(tmp_file_name)
        fT = file(tmp_file_name,"w")
        fT.write(t.safe_substitute(IND=stren))
        fT.close()
        subprocesses.append( subprocess.Popen("python {}".format(tmp_file_name), shell=True) )

    # This returns as soon as all subprocesses are finished
    # -> Counting on subprocesses to finish only when sim+reco concluded
    exit_codes = [ p.wait() for p in subprocesses ]

if __name__ == "__main__":
	main(sys.argv[1:])
