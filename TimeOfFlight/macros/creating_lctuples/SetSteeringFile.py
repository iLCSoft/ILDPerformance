# Needs to be run with Python3 !
import argparse
import sys

#-------------------------------------------------------------------------------

def get_TOF_branches_string(resolution):
  TOF_branches_string = """Algorithm:TOFEstimators{}ps tof{}
                           TOFFirstHit TOFClosestHits TOFClosestHitsError TOFFlightLength TOFLastTrkHit TOFLastTrkHitFlightLength
                           fh          ch             che                 len             th            thl \n""".format(resolution,resolution)
  return TOF_branches_string
 
#-------------------------------------------------------------------------------

def main(args):
  template = args.steering_template
  
  
  # TODO get all TOF branches
  # TODO Figure out how to read array of unknown size of input parameters
  
  TOF_branches = [get_TOF_branches_string(tof_resolution) for tof_resolution in args.resolutions] #TODO Somewhere make sure its in ps!!!
  
  parameter_map = {
    'INPUT_FILES'      : ' '.join(args.input_files),
    'OUTPUT_FILE_BASE' : args.output_base,
    'TOF_PID_BRANCHES' : ' '.join(TOF_branches),
  }
  
  with open(args.steering_template, "r") as template_file:
    steering_template = template_file.read()
    with open(args.result_path,'w') as steering_file:
      print("Creating {}".format(args.result_path))
      steering_file.write(steering_template.format(**parameter_map))

#-------------------------------------------------------------------------------

if __name__ == '__main__':
  parser = argparse.ArgumentParser(description = 'Config file reader')
  parser.add_argument('--steering_template',             help='Name of the template steering file')
  parser.add_argument('--input_files',        nargs='*', help='List of input slcio files')
  parser.add_argument('--output_base',                   help='')
  parser.add_argument('--resolutions',        nargs='*', help='List of time resolutions')
  parser.add_argument('--result_path',                   help='Path to with the resulting steering file should be written')
  
  args = parser.parse_args()
  sys.exit(main(args))

#-------------------------------------------------------------------------------