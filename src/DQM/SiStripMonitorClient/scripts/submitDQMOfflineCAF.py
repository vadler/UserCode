#!/usr/bin/env python

#
# $Id$
#

## CMSSW/DQM/SiStripMonitorClient/scripts/submitDQMOfflineCAF.py
#
#  This script submits batch jobs to the CAF in order to process the full
#  granularity SiStrip offline DQM.
#  Questions and comments to: volker.adler@cern.ch


import sys
import os
import os.path
import commands
import shutil
import string
import math
import urllib
import time

# Constants

# numbers
OCT_rwx_r_r = 0744
# strings
LSTR_true                = ['1','TRUE' ,'True' ,'true' ]
LSTR_false               = ['0','FALSE','False','false']
STR_default              = 'DEFAULT'
STR_none                 = 'None'
STR_nameCmsswPackage     = 'DQM/SiStripMonitorClient'
STR_magField0            = '0T' # FIXME: not here and not this way
STR_magField20           = '20T' # FIXME: not here and not this way
STR_magField30           = '30T' # FIXME: not here and not this way
STR_magField35           = '35T' # FIXME: not here and not this way
STR_magField38           = '38T' # FIXME: not here and not this way
STR_magField40           = '40T' # FIXME: not here and not this way
STR_textUsage            = """ CMSSW/DQM/SiStripMonitorClient/scripts/submitDQMOfflineCAF.py
 
 This script submits batch jobs to the CAF in order to process the full
 granularity SiStrip offline DQM.
 Questions and comments to: volker.adler@cern.ch
 
 Func_Usage(): submitDQMOfflineCAF.py (-s, --submit | -c, --create |
                                  -h, --help)
                                 [-r, --run]
                                 [-C, --CRAB]
                                 [-S, --server]
                                 [-e, --email]
                                 [-j, --jobs]
                                 [-f, --filter]
                                 [-d, --dataset]
                                 [-o, --outpath]
                                 [-m, --mergepath]                               
                               
   Function letters: One of the following options  m u s t  be used.
   
     -s, --submit
         create jobs and submit them to CAF;
         requires option '-r'
         
     -c, --create
         create jobs, but do not submit them;
         requires option '-r'
         
     -h, --help
         print this message
         
   Other options:
   
     -r, --run RUNNUMBER
         number of run to process;
         required by funtion letters '-s' and '-c'
   
     -P, --Python TRUE/FALSE
         use or use not Python configuration of CMSSW jobs;
         default: TRUE
   
     -C, --CRAB TRUE/FALSE
         submit or submit not using CRAB;
         default: TRUE
   
     -S, --server CRAB_SERVER
         CRAB server to use;
         available: None (default)
                    caf
                    bari (currently not usable for submission due to drain mode,
                          s. https://twiki.cern.ch/twiki/bin/view/CMS/CrabServer#Server_available_for_users)
                    lnl2
                    
         NOTE: CRAB server submission is disabled at the moment.
         
     -e, --email EMAIL_ADDRESS
         where the CRAB server should send its messages;
         default: volker.adler@cern.ch
         
     -j, --jobs NUMBER
         number of jobs to create;
         default: 10
         
     -f, --filter TRUE/FALSE
         use or use not HLT filters to select events to process;
         default: FALSE
      
     -d, --dataset PRIMARY_DATASET
         specify dataset for DBS query;
         available: /Cosmics/EW35_3T_v1_CRUZET4_V3P_SuperPointing_v1/RECO
                    /Cosmics/EW35_3T_v1_CRUZET4_V3P_TrackerPointing_v1/RECO
                    /Cosmics/Commissioning08-EW35_3T_v1/RECO                         (default)
                    /Cosmics/Commissioning08-EW35_3T_v1/RAW
                    /Cosmics/CRUZET4_v1_CRZT210_V1_SuperPointing_v1/RECO
                    /Cosmics/CRUZET4_v1_CRZT210_V1_TrackerPointing_v1/RECO
                    /Cosmics/Commissioning08_CRUZET4_V2P_CRUZET4_InterimReco_v3/RECO
                    /Cosmics/Commissioning08-CRUZET4_v1/RECO
                    /Cosmics/Commissioning08-CRUZET4_v1/RAW
                    /Cosmics/Commissioning08-MW33_v1/RECO
                    /Cosmics/Commissioning08-MW33_v1/RAW
                    
     -o, --outpath PATH
         path to copy job output *.root files to;
         currently (almost) no check performed;
         must be in AFS or CASTOR
         default: /castor/cern.ch/user/c/cctrack/DQM
         
     -m, --mergepath PATH
         path to merge the job output *.root files;
         currently (almost) no check performed;
         must be in AFS or on local computer (e.g. /tmp/[user])
         default: /afs/cern.ch/cms/CAF/CMSCOMM/COMM_TRACKER/DQM/SiStrip/jobs/merged
"""                        
LSTR_datatiers = ['RECO','RAW']
# argument vector
LSTR_wordArgument = sys.argv[1:]
# default arguments
BOOL_Python    = True
BOOL_CRAB      = True
STR_server     = STR_none
LSTR_server    = [STR_server,'caf','bari','lnl2']
STR_email      = 'volker.adler@cern.ch'
INT_jobs       = 10
BOOL_filter    = False
STR_dataset    = '/Cosmics/Commissioning08-EW35_3T_v1/RECO'
DICT_datasets = { '/Cosmics/EW35_3T_v1_CRUZET4_V3P_SuperPointing_v1/RECO'           :'/store/data/EW35_3T_v1/Cosmics/RECO/CRUZET4_V3P_SuperPointing_v1'           ,
                  '/Cosmics/EW35_3T_v1_CRUZET4_V3P_TrackerPointing_v1/RECO'         :'/store/data/EW35_3T_v1/Cosmics/RECO/CRUZET4_V3P_TrackerPointing_v1'         ,
                  STR_dataset                                                       :'/store/data/Commissioning08/Cosmics/RECO/EW35_3T_v1'                        ,
                  '/Cosmics/Commissioning08-EW35_3T_v1/RAW'                         :'/store/data/Commissioning08/Cosmics/RAW/EW35_3T_v1'                         ,
                  '/Cosmics/CRUZET4_v1_CRZT210_V1_SuperPointing_v1/RECO'            :'/store/data/CRUZET4_v1/Cosmics/RECO/CRZT210_V1_SuperPointing_v1'            ,
                  '/Cosmics/CRUZET4_v1_CRZT210_V1_TrackerPointing_v1/RECO'          :'/store/data/CRUZET4_v1/Cosmics/RECO/CRZT210_V1_TrackerPointing_v1'          ,
                  '/Cosmics/Commissioning08_CRUZET4_V2P_CRUZET4_InterimReco_v3/RECO':'/store/data/Commissioning08/Cosmics/RECO/CRUZET4_V2P_CRUZET4_InterimReco_v3',
                  '/Cosmics/Commissioning08-CRUZET4_v1/RECO'                        :'/store/data/Commissioning08/Cosmics/RECO/CRUZET4_v1'                        ,
                  '/Cosmics/Commissioning08-CRUZET4_v1/RAW'                         :'/store/data/Commissioning08/Cosmics/RAW/CRUZET4_v1'                         ,
                  '/Cosmics/Commissioning08-MW33_v1/RECO'                           :'/store/data/Commissioning08/Cosmics/RECO/MW33_v1'                           ,
                  '/Cosmics/Commissioning08-MW33_v1/RAW'                            :'/store/data/Commissioning08/Cosmics/RAW/MW33_v1'                            }
STR_outpath    = '/castor/cern.ch/user/c/cctrack/DQM'
BOOL_useCastor = True
STR_mergepath  = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_TRACKER/DQM/SiStrip/jobs/merged'
# option lists
LSTR_functionLetters = ['-s','-c','-h']
DICT_functionLetters = {'--submit':LSTR_functionLetters[0],
                        '--create':LSTR_functionLetters[1],
                        '--help'  :LSTR_functionLetters[2]}
LSTR_optionLetters   = ['-r','-P','-C','-S','-e','-j','-f','-d','-o','-m']
DICT_optionLetters   = {'--run'      :LSTR_optionLetters[0],  
                        '--Python'   :LSTR_optionLetters[1],  
                        '--CRAB'     :LSTR_optionLetters[2],  
                        '--server'   :LSTR_optionLetters[3],  
                        '--email'    :LSTR_optionLetters[4],  
                        '--jobs'     :LSTR_optionLetters[5],
                        '--filter'   :LSTR_optionLetters[6],
                        '--dataset'  :LSTR_optionLetters[7],
                        '--outpath'  :LSTR_optionLetters[8],
                        '--mergepath':LSTR_optionLetters[9]}
                        
# Globals

global Dict_arguments
global Str_run
global Bool_Python
global Bool_CRAB 
global Str_server
global Str_email
global Int_jobs     
global Bool_filter
global Str_dataset   
global Str_datatier
global Float_magField   
global Str_outpath
global Bool_useCastor
global Str_mergepath
# initialize
Dict_arguments = {}
Bool_Python    = BOOL_Python
Bool_CRAB      = BOOL_CRAB
Str_server     = STR_server
Str_email      = STR_email
Int_jobs       = INT_jobs
Bool_filter    = BOOL_filter
Str_dataset    = STR_dataset
Str_outpath    = STR_outpath
Bool_useCastor = BOOL_useCastor
Str_mergepath  = STR_mergepath

## Function Func_Usage()
#
#  Displays usage of the script
def Func_Usage():
  """ Function Func_Usage():
  Displays usage of the script
  """
  print STR_textUsage

## Function Func_Exit()
#
#  Exit after error
def Func_Exit():
  """ Function Func_Exit():
  Exit after error
  """
  print '                           exit'
  print
  sys.exit(1)

## Function Func_ExitUsage()
#
#  Exit after wrong invocation of script
def Func_ExitUsage():
  """ Function Func_ExitUsage():
  Exit after wrong invocation of script
  """
  print '                           exit'
  print
  Func_Usage()
  sys.exit(1)

## Function Func_ExitBool()
#
#  Exit after wrong assignment of bool option
def Func_ExitBool(int_index):
  """ Function Func_ExitBool():
  Exit after wrong assignment of bool option
  """
  print '> submitDQMOfflineCAF.py > option %s expects 0/1, FALSE/TRUE, False/True or false/true' %(str(DICT_optionLetters.items()[int_index]))
  Func_Exit()

## Function Func_MkDir()
#
#  Create new directory
def Func_MkDir(str_path):
  """ Function Func_MkDir():
  Create new directory
  """
  if os.path.exists(str_path):
    for str_root, str_dirs, str_files in os.walk(str_path, topdown = False):
      for name in str_files:
        os.remove(os.path.join(str_root, name))
      for name in str_dirs:
        os.rmdir(os.path.join(str_root, name))
    os.rmdir(str_path)
  os.mkdir(str_path)
  
## Main program

print
  
# Current environment

str_pathCurrentDir       = os.getcwd()
str_pathCmsswBase        = os.getenv('CMSSW_BASE')
if not str_pathCmsswBase:
  print '> submitDQMOfflineCAF.py > CMSSW environment not set properly;'
  print '                           please use'
  print
  print '                           $ cd [your/CMSSW/release/area]/src'
  print '                           $ cmsenv'
  print
  Func_Exit()
str_nameCmsswRel         = os.path.basename(str_pathCmsswBase)
str_pathCmsswBaseSrc     = str_pathCmsswBase + '/src'
str_pathCmsswBasePackage = str_pathCmsswBaseSrc + '/' + STR_nameCmsswPackage
str_suffixShell          = 'csh'
if not os.getenv('SHELL')[-3:] == str_suffixShell:
  str_suffixShell = 'sh'

# Check function letters

if len(LSTR_wordArgument) == 0:
  Func_ExitUsage()
int_nFunctionLetters = 0
for str_argument in LSTR_wordArgument:
  if str_argument in LSTR_functionLetters       or\
     DICT_functionLetters.has_key(str_argument)   :
    int_nFunctionLetters += 1
if int_nFunctionLetters == 0:
  print '> submitDQMOfflineCAF.py > no or unknow function letter used'
  Func_ExitUsage()
elif int_nFunctionLetters > 1:
  print '> submitDQMOfflineCAF.py > too many function letter used'
  Func_ExitUsage()
    
# Check options

str_argumentFormer = ''
bool_standBy       = False
for str_argument in LSTR_wordArgument:
  if not ( str_argument in LSTR_functionLetters or DICT_functionLetters.has_key(str_argument) or\
           str_argument in LSTR_optionLetters   or DICT_optionLetters.has_key(str_argument)     ):
    if str_argument[0] == '-':
      print '> submitDQMOfflineCAF.py > unknown option used'
      Func_ExitUsage()
    if not bool_standBy:
      print '> submitDQMOfflineCAF.py > value without option used'
      Func_ExitUsage()
    Dict_arguments[str_argumentFormer] = str_argument
    bool_standBy                       = False
  else:
    if bool_standBy:
      Dict_arguments[str_argumentFormer] = STR_default
      if str_argumentFormer in LSTR_optionLetters or\
         DICT_optionLetters.has_key(str_argumentFormer):
        print '> submitDQMOfflineCAF.py > option "%s" w/o value' %(str_argumentFormer)
        print '                           default used'
        print
    bool_standBy = not ( str_argument in LSTR_functionLetters       or\
                         DICT_functionLetters.has_key(str_argument)   )
    if not bool_standBy:
      Dict_arguments[str_argument] = STR_default
  str_argumentFormer = str_argument
if bool_standBy:
  Dict_arguments[str_argumentFormer] = STR_default
  if str_argumentFormer in LSTR_optionLetters       or\
     DICT_optionLetters.has_key(str_argumentFormer)   :
    print '> submitDQMOfflineCAF.py > option "%s" w/o value' %(str_argumentFormer)
    print '                           default used'
    print
    
# Correct arguments' dictionary

dict_arguments2 = Dict_arguments
for str_key, str_value in dict_arguments2.items():
  if str_key in DICT_functionLetters.keys():
    del Dict_arguments[str_key]
    Dict_arguments[DICT_functionLetters[str_key]] = str_value
  if str_key in DICT_optionLetters.keys():
    del Dict_arguments[str_key]
    Dict_arguments[DICT_optionLetters[str_key]] = str_value
    
# Help (exit)

if Dict_arguments.has_key(LSTR_functionLetters[2]):
  Func_Usage()
  sys.exit(0)
  
# Check and assign arguments

# run number
if Dict_arguments.has_key(LSTR_optionLetters[0])        and\
   Dict_arguments[LSTR_optionLetters[0]] != STR_default    :
  Str_run = Dict_arguments[LSTR_optionLetters[0]]
else:   
  print '> submitDQMOfflineCAF.py > no run number given'
  Func_Exit()
# use Python
if Dict_arguments.has_key(LSTR_optionLetters[1])        and\
   Dict_arguments[LSTR_optionLetters[1]] != STR_default    :
  Bool_Python = Dict_arguments[LSTR_optionLetters[1]]
  if Dict_arguments[LSTR_optionLetters[1]] in LSTR_true:
    Bool_Python = True
  elif Dict_arguments[LSTR_optionLetters[1]] in LSTR_false:  
    Bool_Python = False
  else:
    Func_ExitBool(1)
# use CRAB
if Dict_arguments.has_key(LSTR_optionLetters[2])        and\
   Dict_arguments[LSTR_optionLetters[2]] != STR_default    :
  if Dict_arguments[LSTR_optionLetters[2]] in LSTR_true:
    Bool_CRAB = True
  elif Dict_arguments[LSTR_optionLetters[2]] in LSTR_false:  
    Bool_CRAB = False
  else:
    Func_ExitBool(2)
# name of CRAB server
if Dict_arguments.has_key(LSTR_optionLetters[3])        and\
   Dict_arguments[LSTR_optionLetters[3]] != STR_default    :
  Str_server = Dict_arguments[LSTR_optionLetters[3]]
# email address to be used by CRAB server
if Dict_arguments.has_key(LSTR_optionLetters[4])        and\
   Dict_arguments[LSTR_optionLetters[4]] != STR_default    :
  Str_email = Dict_arguments[LSTR_optionLetters[4]]
# number of jobs to create
if Dict_arguments.has_key(LSTR_optionLetters[5])        and\
   Dict_arguments[LSTR_optionLetters[5]] != STR_default    :
  Int_jobs = int(Dict_arguments[LSTR_optionLetters[5]])
# use HLT to filter events
if Dict_arguments.has_key(LSTR_optionLetters[6])        and\
   Dict_arguments[LSTR_optionLetters[6]] != STR_default    :
  if Dict_arguments[LSTR_optionLetters[6]] in LSTR_true:
    Bool_filter = True
  elif Dict_arguments[LSTR_optionLetters[6]] in LSTR_false:  
    Bool_filter = False
  else:
    Func_ExitBool(6)
# primary dataset
if Dict_arguments.has_key(LSTR_optionLetters[7])        and\
   Dict_arguments[LSTR_optionLetters[7]] != STR_default    :
  Str_dataset = Dict_arguments[LSTR_optionLetters[7]]
# path for job output
if Dict_arguments.has_key(LSTR_optionLetters[8])        and\
   Dict_arguments[LSTR_optionLetters[8]] != STR_default    :
  Str_outpath = Dict_arguments[LSTR_optionLetters[8]]
# path for merged output
if Dict_arguments.has_key(LSTR_optionLetters[9])        and\
   Dict_arguments[LSTR_optionLetters[9]] != STR_default    :
  Str_mergepath = Dict_arguments[LSTR_optionLetters[9]]
  
# React on arguments

# on use Python
str_nameIncludePath   = 'python'
str_suffixCfg         = '_cfg.py'
str_suffixCff         = '_cff.py'
str_nameInputFilesCff = 'inputFiles_cff'
str_suffixDBS         = 'py'
str_prefixProcess     = 'process.'
str_delimiter         = ' *'
if not Bool_Python:
  str_nameIncludePath   = 'data'
  str_suffixCfg         = '.cfg'
  str_suffixCff         = '.cff'
  str_nameInputFilesCff = 'inputFiles.cff'
  str_suffixDBS         = 'cff'
  str_prefixProcess     = ''
  str_delimiter         = ','
# on use CRAB
if Bool_CRAB:
  str_buffer  = commands.getoutput('which crab')
  if str_buffer.find('which: no crab in') >= 0:
    print '> submitDQMOfflineCAF.py > CRAB environment not set properly;'
    print '                           please use'
    print
    print '                           $ source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.%s' %(str_suffixShell)
    print
    Func_Exit()
# on name of CRAB server
if not Str_server in LSTR_server:
  print '> submitDQMOfflineCAF.py > CRAB server "%s" not available' %(Str_server)
  Func_Exit()
elif Str_server == LSTR_server[2]: # FIXME: put "bari" back as soon as available for submission again (https://twiki.cern.ch/twiki/bin/view/CMS/CrabServer#Server_available_for_users).
  print '> submitDQMOfflineCAF.py > CRAB server "%s" currently in drain mode' %(Str_server)
  print '                           and not available for submission'
  Func_Exit()
elif not Str_server == STR_server: # FIXME: CRAB server submission disabled at the moment. 
  print '> submitDQMOfflineCAF.py > CRAB server submission disabled at the moment'
  Func_Exit()
# on primary dataset
# data tier
# FIXME: more sophisticated LFN determination for dataset
if not DICT_datasets.has_key(Str_dataset):
  print '> submitDQMOfflineCAF.py > dataset "%s" not registered' %(Str_dataset)
  Func_Exit()
Str_datatier = string.split(Str_dataset, '/')[-1]
# magnetic field
# FIXME: more sophisticated magn. field determination for dataset
str_magField = STR_magField0
if Str_dataset.find('_3T_') >= 0:
  str_magField = STR_magField30
if not Str_datatier in LSTR_datatiers:
  print '> submitDQMOfflineCAF.py > datatier "%s" not processable' %(Str_datatier)
  Func_Exit()
# on path for job output
# use CASTOR
if string.split(Str_outpath,'/')[1] == 'afs':
  Bool_useCastor = False
elif string.split(Str_outpath,'/')[1] != 'castor':
  print '> submitDQMOfflineCAF.py > output path not accepted'
  Func_ExitUsage()
# on path for merged output
if string.split(Str_mergepath,'/')[1] != 'afs':
  print '> submitDQMOfflineCAF.py > merge path not accepted'
  Func_ExitUsage()
  
# Prepare work area

# string identifying run (run name)  
int_nDigits = 1
if int(Str_run) >= 10:
  int_nDigits = int(math.log10(int(Str_run))) + 1
str_nameRun = 'R'
for int_iJob in range(9-int_nDigits):
  str_nameRun += '0'
str_nameRun += Str_run
#  directories
Func_MkDir(str_nameRun)
str_pathRunIncludeDir = str_pathCmsswBasePackage + '/' + str_nameIncludePath + '/' + str_nameRun
Func_MkDir(str_pathRunIncludeDir)
str_nameInputFilesFile = str_nameRun + '/' + str_nameRun + '.txt'

# Dealing with input files

int_nInputFiles    = 0
file_inputFilesCff = file(str_nameInputFilesFile, 'w')
str_dbsParams    = urllib.urlencode({'dbsInst': 'cms_dbs_prod_global', 'blockName': '*', 'dataset': Str_dataset, 'userMode': 'user', 'run': Str_run, 'what': str_suffixDBS})
lstr_dbsOutput   = urllib.urlopen("https://cmsweb.cern.ch/dbs_discovery/getLFN_txt", str_dbsParams)
str_pathDbsStore = DICT_datasets[Str_dataset]
for str_iLine in lstr_dbsOutput.readlines():
  if str_iLine.find(str_pathDbsStore) > -1:
    int_nInputFiles += 1
    file_inputFilesCff.write(str_iLine)
if int_nInputFiles == 0:
  print '> submitDQMOfflineCAF.py > no input files found in DBS for run ' + Str_run
  Func_Exit()
file_inputFilesCff.close()
if int_nInputFiles <= Int_jobs:
  Int_jobs = int_nInputFiles
if Int_jobs == 0:
  Int_jobs = 1
nInputFilesJob = int(int_nInputFiles/Int_jobs) + 1
if int_nInputFiles%Int_jobs == 0:
  nInputFilesJob -= 1
if int_nInputFiles == nInputFilesJob*(Int_jobs-1) and Int_jobs > 1:
  Int_jobs -= 1
str_nJobs = str(Int_jobs)
print '> submitDQMOfflineCAF.py > input files for run ' + Str_run + ':   ' + str(int_nInputFiles)
print

# Create scripts

# prepare merge script  
int_nLinesRead      = 0
file_inputFilesCff  = file(str_nameInputFilesFile, 'r')
lstr_linesInput     = file_inputFilesCff.readlines()
str_nameMergeScript = 'merge' + str_nameRun + '.job'
file_mergeScript = file(str_nameRun + '/' + str_nameMergeScript, 'w')
file_mergeScript.write('#!/bin/tcsh\n')
file_mergeScript.write('cd ' + str_pathCmsswBaseSrc + '\n')
file_mergeScript.write('cmsenv\n')
file_mergeScript.write('setenv STAGE_SVCCLASS cmscaf\n')
file_mergeScript.write('hadd -f ' + Str_mergepath + '/DQM_SiStrip_' + str_nameRun + '-' + Str_datatier + '-CAF_' + str_nameCmsswRel +'-standAlone.root \\\n') # FIXME: make configurable
# create harvesting config file
str_sedCommand  = 'sed '
str_sedCommand += '-e \"s#xRUN_NUMBERx#'         + Str_run + '#g\" '
str_sedCommand += '-e \"s#xMERGED_INPUT_FILEx#'  + Str_mergepath + '/DQM_SiStrip_' + str_nameRun + '-' + Str_datatier + '-CAF_' + str_nameCmsswRel +'-standAlone.root#g\" '
str_sedCommand += '-e \"s#xMERGED_OUTPUT_FILEx#' + Str_mergepath + '/DQM_SiStrip_' + str_nameRun + '-' + Str_datatier + '-CAF_' + str_nameCmsswRel +'.root#g\" '
str_sedCommand += str_pathCmsswBasePackage + '/test/SiStripCAFHarvest_template' + str_suffixCfg + ' > ' + str_nameRun + '/SiStripCAFHarvest' + str_suffixCfg
os.system(str_sedCommand)

# loop over single jobs
if Bool_CRAB:
  os.chdir(str_nameRun)     
  str_outputDir         = '.'
  str_nameRunIncludeDir = STR_nameCmsswPackage + '/' + str_nameIncludePath + '/' + str_nameRun
  if Bool_Python:
    str_nameRunIncludeDir  = str_nameRunIncludeDir.replace('/','.')
    str_nameRunIncludeDir  = str_nameRunIncludeDir.replace('.python.','.')
  # create main configuration file
  str_sedCommand = 'sed '
  if Bool_filter:
    str_sedCommand += '-e \"s#xHLT_FILTERx#    ' + str_prefixProcess + 'hltFilter' + str_delimiter + '#g\" '
  else:
    str_sedCommand += '-e \"s#xHLT_FILTERx#\#     ' + str_prefixProcess + 'hltFilter' + str_delimiter + '#g\" '
  if Str_datatier == 'RECO':
    str_sedCommand += '-e \"s#xRECO_FROM_RAWx#\#     ' + str_prefixProcess + 'SiStripDQMRecoFromRaw' + str_delimiter + '#g\" '
    str_sedCommand += '-e \"s#xDQM_FROM_RAWx#\#     ' + str_prefixProcess + 'SiStripDQMSourceGlobalRunCAF_fromRAW' + str_delimiter + '#g\" '
  else:
    str_sedCommand += '-e \"s#xRECO_FROM_RAWx#    ' + str_prefixProcess + 'SiStripDQMRecoFromRaw' + str_delimiter + '#g\" '
    str_sedCommand += '-e \"s#xDQM_FROM_RAWx#    ' + str_prefixProcess + 'SiStripDQMSourceGlobalRunCAF_fromRAW' + str_delimiter + '#g\" '
  str_sedCommand += '-e \"s#xMAG_FIELDx#'         + str_magField             + '#g\" '
  str_sedCommand += '-e \"s#xINCLUDE_DIRECTORYx#' + str_nameRunIncludeDir    + '#g\" '
  str_sedCommand += '-e \"s#xINPUT_FILESx#'       + str_nameInputFilesCff + '#g\" '
  str_sedCommand += str_pathCmsswBasePackage + '/test/SiStripDQMOfflineGlobalRunCAF_template' + str_suffixCfg + ' > SiStripDQMOfflineGlobalRunCAF' + str_suffixCfg
  os.system(str_sedCommand)
  # create included input files list
  str_pathInputFilesJobCff = str_pathRunIncludeDir + '/' + str_nameInputFilesCff
  if Bool_Python:
    str_pathInputFilesJobCff += '.py'
  file_inputFilesJobCff = file(str_pathInputFilesJobCff, 'w')
  if Bool_Python:
    file_inputFilesJobCff.write('import FWCore.ParameterSet.Config as cms\n\nsource = cms.Source ("PoolSource",\n    fileNames = cms.untracked.vstring (\n')
    for str_linesInput in lstr_linesInput:
      # protections vs. those annoying DBS output format changes come here:
#       file_inputFilesJobCff.write(str_linesInput)
      str_actualLine = str_linesInput.replace(') );',',')
      file_inputFilesJobCff.write(str_actualLine)
    file_inputFilesJobCff.write('    )\n)\n')
  else:
    file_inputFilesJobCff.write('  source = PoolSource {\n    untracked vstring fileNames = {\n')
    for str_linesInput in lstr_linesInput:
      file_inputFilesJobCff.write(str_linesInput)
    file_inputFilesJobCff.write('    }\n  }\n')
    file_inputFilesJobCff.close()
  file_inputFilesJobCff.close()
  # create included configuration file
  str_sedCommand = 'sed '
  str_sedCommand += '-e \"s#xOUTPUT_DIRECTORxY#' + str_outputDir + '#g\" '
  str_sedCommand += str_pathCmsswBasePackage + '/' + str_nameIncludePath + '/SiStripDQMOfflineGlobalRunCAF_template' + str_suffixCff + ' > ' + str_pathRunIncludeDir + '/SiStripDQMOfflineGlobalRunCAF' + str_suffixCff
  os.system(str_sedCommand)
  for int_iJob in range(Int_jobs):
    for n_iActualLine in range(int_nLinesRead, min(int_nLinesRead+nInputFilesJob, int_nInputFiles)):
      int_nLinesRead += 1
    # extend merge script
    str_nJobs = str(int_iJob+1)
    str_lineMergeScript = Str_outpath + '/DQM_SiStrip_' + str_nameRun + '_' + str_nJobs + '.root'
    if Bool_useCastor:
      str_lineMergeScript = 'rfio:' + str_lineMergeScript
    if int_nLinesRead < int_nInputFiles:
      str_lineMergeScript += ' \\'
    str_lineMergeScript += '\n'  
    file_mergeScript.write(str_lineMergeScript)
    # FIXME: This protection is currently needed. Review calculations again!
    if int_nLinesRead >= int_nInputFiles:
      print '> submitDQMOfflineCAF.py > number of created job: ' + str_nJobs
      print
      break
  os.chdir(str_pathCurrentDir)
else:
  for int_iJob in range(Int_jobs):
    int_nDigits = 1
    if int_iJob >= 10:
      int_nDigits = int(math.log10(int_iJob)) + 1
    str_nameJob = str_nameRun + "_"
    for int_iDigit in range(4-int_nDigits):
      str_nameJob += '0'
    str_nameJob += str(int_iJob)
    # prepare job dir
    str_nameJobDir        = str_nameRun + "/" + str_nameJob
    str_outputDir         = '/tmp/' + os.getenv('USER') + '/' + str_nameJobDir
    str_pathJobIncludeDir = str_pathRunIncludeDir + '/' + str_nameJob
    str_nameJobIncludeDir = STR_nameCmsswPackage + '/' + str_nameIncludePath + '/' + str_nameJobDir
    if Bool_Python:
      str_nameJobIncludeDir = str_nameJobIncludeDir.replace('/','.')
      str_nameJobIncludeDir = str_nameJobIncludeDir.replace('.python.','.')
    os.mkdir(str_nameJobDir)
    os.chdir(str_nameJobDir)     
    # create main configuration file
    str_sedCommand = 'sed '
    if Bool_filter:
      str_sedCommand += '-e \"s#xHLT_FILTERx#    ' + str_prefixProcess + 'hltFilter' + str_delimiter + '#g\" '
    else:
      str_sedCommand += '-e \"s#xHLT_FILTERx#\#     ' + str_prefixProcess + 'hltFilter' + str_delimiter + '#g\" '
    if Str_datatier == 'RECO':
      str_sedCommand += '-e \"s#xRECO_FROM_RAWx#\#     ' + str_prefixProcess + 'SiStripDQMRecoFromRaw' + str_delimiter + '#g\" '
      str_sedCommand += '-e \"s#xDQM_FROM_RAWx#\#     ' + str_prefixProcess + 'SiStripDQMSourceGlobalRunCAF_fromRAW' + str_delimiter + '#g\" '
    else:
      str_sedCommand += '-e \"s#xRECO_FROM_RAWx#    ' + str_prefixProcess + 'SiStripDQMRecoFromRaw' + str_delimiter + '#g\" '
      str_sedCommand += '-e \"s#xDQM_FROM_RAWx#    ' + str_prefixProcess + 'SiStripDQMSourceGlobalRunCAF_fromRAW' + str_delimiter + '#g\" '
    str_sedCommand += '-e \"s#xMAG_FIELDx#'         + str_magField             + '#g\" '
    str_sedCommand += '-e \"s#xINCLUDE_DIRECTORYx#' + str_nameJobIncludeDir    + '#g\" '
    str_sedCommand += '-e \"s#xINPUT_FILESx#'       + str_nameInputFilesCff + '#g\" '
    str_sedCommand += str_pathCmsswBasePackage + '/test/SiStripDQMOfflineGlobalRunCAF_template' + str_suffixCfg + ' > SiStripDQMOfflineGlobalRunCAF' + str_suffixCfg
    os.system(str_sedCommand)
    # prepare job include dir
    os.mkdir(str_pathJobIncludeDir)
    # create included input files list
    str_pathInputFilesJobCff = str_pathJobIncludeDir + '/' + str_nameInputFilesCff
    if Bool_Python:
      str_pathInputFilesJobCff += '.py'
    file_inputFilesJobCff = file(str_pathInputFilesJobCff, 'w')
    if Bool_Python:
      file_inputFilesJobCff.write('import FWCore.ParameterSet.Config as cms\n\nsource = cms.Source ("PoolSource",\n    fileNames = cms.untracked.vstring (\n')
      for n_iActualLine in range(int_nLinesRead, min(int_nLinesRead+nInputFilesJob, int_nInputFiles)):
        # protections vs. those annoying DBS output format changes come here:
#         str_linesInput = lstr_linesInput[n_iActualLine]
        str_linesInput = lstr_linesInput[n_iActualLine].replace(') );',',')
        # fix commata and end of line
        str_actualLine = str_linesInput
        if (n_iActualLine+1)%nInputFilesJob == 0 or int_nLinesRead == int_nInputFiles-1:
          str_actualLine = string.split(str_linesInput, ',')[0] + '\n'
        file_inputFilesJobCff.write(str_actualLine)
        int_nLinesRead += 1
      file_inputFilesJobCff.write('    )\n)\n')
    else:
      file_inputFilesJobCff.write('  source = PoolSource {\n    untracked vstring fileNames = {\n')
      for n_iActualLine in range(int_nLinesRead, min(int_nLinesRead+nInputFilesJob, int_nInputFiles)):
        str_linesInput = lstr_linesInput[n_iActualLine]
        # fix commata and end of line
        str_actualLine = str_linesInput
        if (n_iActualLine+1)%nInputFilesJob == 0 or int_nLinesRead == int_nInputFiles-1:
          str_actualLine = string.split(str_linesInput, ',')[0] + '\n'
        file_inputFilesJobCff.write(str_actualLine)
        int_nLinesRead += 1
      file_inputFilesJobCff.write('    }\n  }\n')
    file_inputFilesJobCff.close()
    # extend merge script
    str_lineMergeScript = Str_outpath + '/DQM_SiStrip_' + str_nameJob + '.root'
    if Bool_useCastor:
      str_lineMergeScript = 'rfio:' + str_lineMergeScript
    if int_nLinesRead < int_nInputFiles:
      str_lineMergeScript += ' \\'
    str_lineMergeScript += '\n'  
    file_mergeScript.write(str_lineMergeScript)
    # create included configuration file
    str_sedCommand = 'sed '
    str_sedCommand += '-e \"s#xOUTPUT_DIRECTORYx#' + str_outputDir + '#g\" '
    str_sedCommand += str_pathCmsswBasePackage + '/' + str_nameIncludePath + '/SiStripDQMOfflineGlobalRunCAF_template' + str_suffixCff + ' > ' + str_pathJobIncludeDir + '/SiStripDQMOfflineGlobalRunCAF' + str_suffixCff
    os.system(str_sedCommand)
    # create job script
    str_sedCommand = 'sed '
    str_sedCommand += '-e \"s#xCMSSW_BASEx#' + str_pathCmsswBase   + '#g\" '
    str_sedCommand += '-e \"s#xRUN_NAMEx#'    + str_nameRun        + '#g\" '
    str_sedCommand += '-e \"s#xJOB_NAMEx#'    + str_nameJob        + '#g\" '
    str_sedCommand += '-e \"s#xCURRENT_DIRx#' + str_pathCurrentDir + '#g\" '
    str_sedCommand += '-e \"s#xSUFFIXx#'      + str_suffixCfg      + '#g\" '
    if Bool_useCastor:
      str_sedCommand += '-e \"s#xCOPYx#rfcp#g\" '
    else:
      str_sedCommand += '-e \"s#xCOPYx#cp#g\" '
    str_sedCommand += '-e \"s#xOUTPUT_DIRx#' + Str_outpath + '#g\" '
    str_sedCommand += str_pathCmsswBasePackage + '/scripts/SiStripDQMOfflineCAF_template.job > SiStripDQMOfflineCAF.job'
    os.system(str_sedCommand)
    # finalize job creation
    os.chdir(str_pathCurrentDir)
    # FIXME: This protection is currently needed. Review calculations again!
    if int_nLinesRead >= int_nInputFiles:
      str_nJobs = str(int_iJob+1)
      print '> submitDQMOfflineCAF.py > number of created jobs: ' + str_nJobs
      print
      break

# Compile

if Bool_Python:
  os.chdir(str_pathCmsswBasePackage)
  os.system('scramv1 b python')
  os.chdir(str_pathCurrentDir)
  print
    
# finish scripts
    
# finish merge script
file_mergeScript.write('cd ' + str_pathCurrentDir + '/' + str_nameRun + '/\n')
file_mergeScript.write('cmsRun SiStripCAFHarvest' + str_suffixCfg + '\n')
file_mergeScript.close()
# create CRAB configuration
if Bool_CRAB:
  os.chdir(str_nameRun)     
  str_sedCommand  = 'sed '
  str_sedCommand += '-e \"s#xSERVER_NAMEx#'        + Str_server    + '#g\" '
  str_sedCommand += '-e \"s#xDATASETPATHx#'        + Str_dataset   + '#g\" '
  str_sedCommand += '-e \"s#xRUNSELECTIONx#'       + Str_run       + '#g\" '
  str_sedCommand += '-e \"s#xNUMBER_OF_JOBSx#'     + str_nJobs     + '#g\" '
  str_sedCommand += '-e \"s#xEMAILx#'              + Str_email     + '#g\" '
  str_sedCommand += '-e \"s#xOUTPUT_FILEx#'        + str_nameRun   + '#g\" '
  str_sedCommand += '-e \"s#xUI_WORKING_DIRx#crab' + str_nameRun   + '#g\" '
  str_sedCommand += '-e \"s#xSTORAGE_PATHx#'       + Str_outpath   + '#g\" '
  str_sedCommand += '-e \"s#xSUFFIXx#'             + str_suffixCfg + '#g\" '
  if Bool_useCastor:
    str_sedCommand += '-e \"s#xCOPY_DATAx#1#g\" '
  else:
    str_sedCommand += '-e \"s#xCOPY_DATAx#0#g\" '
  str_sedCommand += str_pathCmsswBasePackage + '/test/SiStripDQMOfflineCAF_template.crab > crab.cfg'
  os.system(str_sedCommand)
  os.system('crab -create')
  os.chdir(str_pathCurrentDir)

# submit jobs

if Dict_arguments.has_key(LSTR_functionLetters[0]):
  if Bool_CRAB:
    os.chdir(str_nameRun)
    print '> submitDQMOfflineCAF.py >'
    print '  ' + os.getcwd() + ' : crab -submit -c crab' + str_nameRun
    os.system('crab -submit -c crab' + str_nameRun)
    print
    os.chdir(str_pathCurrentDir)
    time.sleep(5)
    os.system('crab -status -c ' + str_nameRun + '/crab' + str_nameRun)
  else:
    for int_iJob in range(Int_jobs):
      int_nDigits = 1
      if int_iJob >= 10:
        int_nDigits = int(math.log10(int_iJob)) + 1
      str_nameJobDir = str_nameRun + "/" + str_nameRun + "_"
      for int_iDigit in range(4-int_nDigits):
        str_nameJobDir += '0'
      str_nameJobDir += str(int_iJob)
      os.chdir(str_nameJobDir)     
      os.chmod('SiStripDQMOfflineCAF.job',OCT_rwx_r_r)
      print '> submitDQMOfflineCAF.py >'
      print '  ' + os.getcwd() + ' : bsub -q cmscaf SiStripDQMOfflineCAF.job'
      os.system('bsub -q cmscaf SiStripDQMOfflineCAF.job')
      print
      os.chdir(str_pathCurrentDir)
    time.sleep(5)
    os.system('bjobs -q cmscaf')
  os.chmod(str_nameRun + '/' + str_nameMergeScript,OCT_rwx_r_r)
