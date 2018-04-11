#!/usr/bin/python

import sys, os, getopt, inspect
from string import Template
import math
import ast

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

    # Get directory of this script
    this_script_path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))

    # Open Template file and read in lines
    fR = file(this_script_path + "/sim_reco_template.py","r")
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

    tmp_file_name = process_name + "_sim_reco_job_tmp.py"

    for ind in range(1, n_jobs+1): #1 to n_jobs
        stren = str(ind)
        t = Template(lines_w_args)
        if os.path.exists(tmp_file_name):
            os.remove(tmp_file_name)
        fT = file(tmp_file_name,"w")
        fT.write(t.safe_substitute(IND=stren))
        fT.close()
        os.system("python {}".format(tmp_file_name))

    # Look into status of jobs logged in the repository file.
    # When all either Done or Killed proceed. Warn if any fail.
    rep_file = ilcsoft_version + "_" + ildconfig_version + "_" + process_name + "_" + detector_model + ".rep"

    warned_about_failure = False
    finished = False
    while ( not finished ):
        # Get status of jobs in repository, remove line break at the end
        rep_status = os.popen( "dirac-repo-monitor {}".format(rep_file) ).read()[:-1]
        status_dict = ast.literal_eval( rep_status ) # Interpret output as python dict
        print rep_status
        print status_dict
        # Check if all jobs are done (or killed) before proceeding
        if 'Done' in status_dict:
            if status_dict['Done'] == n_jobs:
                finished=True
            elif 'Killed' in status_dict:
                if status_dict['Done'] + status_dict['Killed'] == n_jobs:
                    finished=True
        elif 'Failed' in status_dict and not warned_about_failure:
            print "WARNING: some jobs failed!"
            warned_about_failure = True

if __name__ == "__main__":
	main(sys.argv[1:])
