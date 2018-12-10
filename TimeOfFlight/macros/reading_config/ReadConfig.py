# Needs to be run with Python3 !
import configparser # For easy config file handling
import argparse
import sys
import os

#-------------------------------------------------------------------------------

def pwd():
  '''Like pwd in bash, return path of this file'''
  return os.path.abspath(os.path.dirname(__file__))

#-------------------------------------------------------------------------------

def main(args):
  conf_dir = '../../config/'
  config = configparser.ConfigParser()
  config.read( pwd() + '/' + conf_dir + args.conf_file )
  
  if args.format == 'list_of_keys':
    print(' '.join(config.sections()))
  elif args.format == 'single_value':
    print(config[args.key][args.variables])
  elif args.format == 'list_of_variables':
    all_variables = [config[key][args.variables] for key in config.sections()]
    print(' '.join(all_variables))
  else: 
   return    

#-------------------------------------------------------------------------------

if __name__ == '__main__':
  parser = argparse.ArgumentParser(description = 'Config file reader')
  parser.add_argument('--conf_file',  help='Name of the config file to read')
  parser.add_argument('--format',     default='list_of_keys',  help='Format of the requested output')
  parser.add_argument('--key',        nargs='?',  help='Names of the requested keys (to be specified when asking for single value)')
  parser.add_argument('--variables',  nargs='?',  help='Names of the requested variables')
  
  args = parser.parse_args()
  sys.exit(main(args))

#-------------------------------------------------------------------------------
