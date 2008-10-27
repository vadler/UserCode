#!/usr/bin/env python

#
# $Id$
#

## CMSSW/DQM/SiStripMonitorClient/scripts/submitDQMOfflineCAF.py
#
#  This script submits CRAB/LSF jobs to the CAF in order to process the full
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
import datetime
import smtplib

# Constants

# numbers
OCT_rwx_r_r          = 0744
LFLOAT_valueMagField = [0.0, 2.0, 3.0, 3.5, 3.8, 4.0]
TD_shiftUTC          = datetime.timedelta(hours = 2) # positive for timezones with later time than UTC
# strings
LSTR_true                = ['1','TRUE' ,'True' ,'true' ]
LSTR_false               = ['0','FALSE','False','false']
STR_default              = 'DEFAULT'
STR_none                 = 'None'
STR_nameCmsswPackage     = 'DQM/SiStripMonitorClient'
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
      
     -d, --dataset PRIMARY_DATASET
         specify dataset for DBS query;
         required by funtion letters '-s' and '-c'
   
     -C, --CRAB TRUE/FALSE
         submit or submit not using CRAB;
         default: TRUE
         
         NOTE: This script runs only with CRAB 2.4.0 or higher.
   
     -S, --server CRAB_SERVER
         CRAB server to use;
         available: None (default)
                    caf  (works, but slow)
                    bari (CRAB version >= 2.3.2,
                          s. https://twiki.cern.ch/twiki/bin/view/CMS/CrabServer#Server_available_for_users)
                    
         NOTE: CRAB server submission is disabled at the moment.
         
     -e, --email EMAIL_ADDRESS
         where the CRAB server should send its messages;
         default: volker.adler@cern.ch
         
     -j, --jobs NUMBER
         number of jobs to create;
         default: 10
         
     -g, --global-tag GLOBAL_TAG
         global tag to be used;
         default: CRAFT_V3P::All
         
     -f, --filter TRUE/FALSE
         use or use not HLT filters to select events to process;
         default: FALSE
                    
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
BOOL_CRAB      = True
LSTR_server    = [STR_none,'caf','bari']
STR_server     = LSTR_server[0]
STR_email      = 'volker.adler@cern.ch'
INT_jobs       = 10
STR_globalTag  = 'CRAFT_V3P::All'
BOOL_filter    = False
STR_outpath    = '/castor/cern.ch/user/c/cctrack/DQM'
BOOL_useCastor = True
STR_mergepath  = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_TRACKER/DQM/SiStrip/jobs/merged'
# option lists
LSTR_functionLetters = ['-s','-c','-h']
DICT_functionLetters = {'--submit':LSTR_functionLetters[0],
                        '--create':LSTR_functionLetters[1],
                        '--help'  :LSTR_functionLetters[2]}
LSTR_optionLetters   = ['-r','-C','-S','-e','-j','-g','-f','-d','-o','-m']
DICT_optionLetters   = {'--run'       :LSTR_optionLetters[0],  
                        '--CRAB'      :LSTR_optionLetters[1],  
                        '--server'    :LSTR_optionLetters[2],  
                        '--email'     :LSTR_optionLetters[3],  
                        '--jobs'      :LSTR_optionLetters[4],
                        '--global-tag':LSTR_optionLetters[5],
                        '--filter'    :LSTR_optionLetters[6],
                        '--dataset'   :LSTR_optionLetters[7],
                        '--outpath'   :LSTR_optionLetters[8],
                        '--mergepath' :LSTR_optionLetters[9]}
STR_mailSmtp        = 'localhost'
STR_mailServer      = '@mail.cern.ch'
STR_mailTextOpener  = """Dear """ + os.getenv('USER').capitalize() + """,

on """ + str(time.ctime()) + """, you have submitted run """
STR_mailTextExplain = """
for SiStrip offline DQM at the CAF.
Unfortunately, this needed to be done from your private account. So, only you
are able to finalize this submission -- even after the end of your shift."""
STR_mailTextCRABNo  = """
To do so, please forward all emails from the LSF batch system referring to the
respective jobs to the list  t h i s  message was sent to."""
STR_mailTextFinish  = """
-- and then your shift is  r e a l l y  done :-)

We are very sorry for the inconvenience.
Thanks a lot!

Best regards,
your SiStrip DQM team

P.S.:
To reply to this email, simply use the "Reply to all" function of your email
client.
"""
                        
# Globals

global Dict_arguments
global Str_run
global Bool_CRAB 
global Str_server
global Str_email
global Int_jobs     
global Str_globalTag
global Bool_filter
global Str_dataset   
global Str_datatier
global Float_magField   
global Str_outpath
global Bool_useCastor
global Str_mergepath
# initialize
Dict_arguments = {}
Bool_CRAB      = BOOL_CRAB
Str_server     = STR_server
Str_email      = STR_email
Int_jobs       = INT_jobs
Str_globalTag  = STR_globalTag
Bool_filter    = BOOL_filter
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
str_nameCmsswRel         = os.getenv('CMSSW_VERSION')
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
  print '> submitDQMOfflineCAF.py > no or unknown function letter used'
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
# use CRAB
if Dict_arguments.has_key(LSTR_optionLetters[1])        and\
   Dict_arguments[LSTR_optionLetters[1]] != STR_default    :
  if Dict_arguments[LSTR_optionLetters[1]] in LSTR_true:
    Bool_CRAB = True
  elif Dict_arguments[LSTR_optionLetters[1]] in LSTR_false:  
    Bool_CRAB = False
  else:
    Func_ExitBool(2)
# name of CRAB server
if Dict_arguments.has_key(LSTR_optionLetters[2])        and\
   Dict_arguments[LSTR_optionLetters[2]] != STR_default    :
  Str_server = Dict_arguments[LSTR_optionLetters[2]]
# email address to be used by CRAB server
if Dict_arguments.has_key(LSTR_optionLetters[3])        and\
   Dict_arguments[LSTR_optionLetters[3]] != STR_default    :
  Str_email = Dict_arguments[LSTR_optionLetters[3]]
# number of jobs to create
if Dict_arguments.has_key(LSTR_optionLetters[4])        and\
   Dict_arguments[LSTR_optionLetters[4]] != STR_default    :
  Int_jobs  = int(Dict_arguments[LSTR_optionLetters[4]])
# global tag
if Dict_arguments.has_key(LSTR_optionLetters[5])        and\
   Dict_arguments[LSTR_optionLetters[5]] != STR_default    :
  Str_globalTag  = Dict_arguments[LSTR_optionLetters[5]]
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
else:   
  print '> submitDQMOfflineCAF.py > no primary dataset given'
  Func_Exit()
# path for job output
if Dict_arguments.has_key(LSTR_optionLetters[8])        and\
   Dict_arguments[LSTR_optionLetters[8]] != STR_default    :
  Str_outpath = Dict_arguments[LSTR_optionLetters[8]]
# path for merged output
if Dict_arguments.has_key(LSTR_optionLetters[9])        and\
   Dict_arguments[LSTR_optionLetters[9]] != STR_default    :
  Str_mergepath = Dict_arguments[LSTR_optionLetters[9]]
  
# React on arguments

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
# on number of jobs
if Int_jobs == 0:
  Int_jobs = 1
  print '> submitDQMOfflineCAF.py > number of requested jobs was 0'
  print '                           set to 1'
# on primary dataset
# data tier
Str_datatier = Str_dataset.split('/')[-1]
if not Str_datatier in LSTR_datatiers:
  print '> submitDQMOfflineCAF.py > datatier "%s" not processable' %(Str_datatier)
  Func_Exit()
# on path for job output
# use CASTOR
if Str_outpath.split('/')[1] == 'afs':
  Bool_useCastor = False
elif Str_outpath.split('/')[1] != 'castor':
  print '> submitDQMOfflineCAF.py > output path not accepted'
  Func_ExitUsage()
str_castorCp = 'cp'
if Bool_useCastor:
  str_castorCp = 'rfcp'
# on path for merged output
if Str_mergepath.split('/')[1] != 'afs':
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
str_pathRunIncludeDir = str_pathCmsswBasePackage + '/python/' + str_nameRun
Func_MkDir(str_pathRunIncludeDir)
str_nameInputFilesFile = str_nameRun + '/' + str_nameRun + '.txt'
str_nameRunIncludeDir  = STR_nameCmsswPackage.replace('/','.') + '.' + str_nameRun

# Retrieving information from the web

# input files
int_nInputFiles    = 0
file_inputFilesCff = file(str_nameInputFilesFile, 'w')
# DBS query for list of input files
str_dbsParams  = urllib.urlencode({'dbsInst':'cms_dbs_prod_global', 'blockName':'*', 'dataset':Str_dataset, 'userMode':'user', 'run':Str_run, 'what':'py'})
file_dbsOutput = urllib.urlopen("https://cmsweb.cern.ch/dbs_discovery/getLFN_txt", str_dbsParams)
for str_iLine in file_dbsOutput.readlines():
  lstr_wordsLine = str_iLine.split("/")
  if len(lstr_wordsLine) >= 5:
    if                  lstr_wordsLine[1]  == 'store'      and\
                        lstr_wordsLine[2]  == 'data'       and\
       Str_dataset.find(lstr_wordsLine[3]) >= 0            and\
       Str_dataset.find(lstr_wordsLine[4]) >= 0            and\
                        lstr_wordsLine[5]  == Str_datatier and\
                    len(lstr_wordsLine[7]) == 3               :
      int_nInputFiles += 1
      file_inputFilesCff.write(str_iLine)
if int_nInputFiles == 0:
  print '> submitDQMOfflineCAF.py > no input files found in DBS for run ' + Str_run + ' in dataset ' + Str_dataset
  Func_Exit()
file_inputFilesCff.close()
if int_nInputFiles < Int_jobs:
  Int_jobs = int_nInputFiles
# FIXME: following calculation have to be reviewed
int_nInputFilesJob = int(int_nInputFiles/Int_jobs) + 1
if int_nInputFiles%Int_jobs == 0:
  int_nInputFilesJob -= 1
if int_nInputFiles == int_nInputFilesJob*(Int_jobs-1) and Int_jobs > 1:
  Int_jobs -= 1
str_nJobs = str(Int_jobs)
print '> submitDQMOfflineCAF.py > input files for run ' + Str_run + ':   ' + str(int_nInputFiles)
print

# # magnetic field
# # extract time stamps of the run
# str_cmsmonParams  = urllib.urlencode({'RUN':Str_run})
# file_cmsmonOutput = urllib.urlopen("http://cmsmon.cern.ch/cmsdb/servlet/RunSummary", str_cmsmonParams)
# str_timeBegin     = ''
# str_timeEnd       = ''
# for str_cmsmonOutput in file_cmsmonOutput.readlines():
#   if str_cmsmonOutput.find('HREF=Component?RUN=' + Str_run + '&NAME=TRACKER') >= 0:
#     lstr_timeQuery = str_cmsmonOutput.split('HREF=Component?RUN=' + Str_run + '&NAME=TRACKER&')[1].split('>TRACKER')[0].split('&')
#     for str_timeQuery in lstr_timeQuery:
#       str_nameStamp = str_timeQuery.split('=')[0]
#       lstr_timeDate = str_timeQuery.split('=')[1].split('_')[0].split('.')
#       lstr_timeTime = str_timeQuery.split('=')[1].split('_')[1].split(':')
#       dt_stampOld   = datetime.datetime(int(lstr_timeDate[0]),int(lstr_timeDate[1]),int(lstr_timeDate[2]),int(lstr_timeTime[0]),int(lstr_timeTime[1]),int(lstr_timeTime[2]))
#       dt_stampNew   = dt_stampOld - TD_shiftUTC
#       str_timeStamp = str(dt_stampNew).replace('-','.') 
#       if str_nameStamp == 'TIME_BEGIN':
#         str_timeBegin = str_timeStamp
#       elif str_nameStamp == 'TIME_END':
#         str_timeEnd = str_timeStamp
# # get magnetic field itself
# str_cmsmonParams  = urllib.urlencode({'TIME_BEGIN':str_timeBegin, 'TIME_END':str_timeEnd})
# file_cmsmonOutput = urllib.urlopen("http://cmsmon.cern.ch/cmsdb/servlet/MagnetHistory", str_cmsmonParams)
float_avMagMeasure = 3.8
# for str_cmsmonOutput in file_cmsmonOutput.readlines():
#   if str_cmsmonOutput.find('BFIELD, Tesla') >= 0:
#     float_avMagMeasure = float(str_cmsmonOutput.split('</A>')[0].split('>')[-1])
# determine corresponding configuration file to be included
float_magField = 0.0
str_magField   = '0'
for float_valueMagField in LFLOAT_valueMagField:
  if math.fabs(float_valueMagField-float_avMagMeasure) < math.fabs(float_magField-float_avMagMeasure):
    float_magField = float_valueMagField
    str_magField   = str(int(float_magField*10))
print '> submitDQMOfflineCAF.py > (average) magnetic field in run ' + Str_run + ':   ' + str(float_avMagMeasure) + ' T'
print '                           using ' + str(float_magField) + ' T for configuration'
print
str_magField += 'T'

# Create scripts

int_nLinesRead      = 0
file_inputFilesCff  = file(str_nameInputFilesFile, 'r')
lstr_linesInput     = file_inputFilesCff.readlines()
file_inputFilesCff.close()

# create harvesting config file and job script
str_sedCommand  = 'sed '
str_sedCommand += '-e \"s#xMAG_FIELDx#'         + str_magField             + '#g\" '
str_sedCommand += '-e \"s#xGLOBAL_TAGx#'        + Str_globalTag            + '#g\" '
str_sedCommand += '-e \"s#xINCLUDE_DIRECTORYx#' + str_nameRunIncludeDir    + '#g\" '
str_sedCommand += '-e \"s#xMERGE_PATHx#'        + Str_mergepath            + '#g\" '
str_sedCommand += str_pathCmsswBasePackage + '/test/SiStripCAFHarvest_template_cfg.py > ' + str_nameRun + '/SiStripCAFHarvest_cfg.py'
os.system(str_sedCommand)
str_sedCommand  = 'sed '
str_sedCommand += '-e \"s#xCMSSW_BASEx#'    + str_pathCmsswBase    + '#g\" '
str_sedCommand += '-e \"s#xRUN_NAMEx#'      + str_nameRun          + '#g\" '
str_sedCommand += '-e \"s#xMERGE_PATHx#'    + Str_mergepath        + '#g\" '
str_sedCommand += '-e \"s#xCURRENT_DIRx#'   + str_pathCurrentDir   + '#g\" '
str_sedCommand += '-e \"s#xDATA_TIERx#'     + Str_datatier         + '#g\" '
str_sedCommand += '-e \"s#xCMSSW_VERSIONx#' + str_nameCmsswRel + '#g\" '
str_sedCommand += str_pathCmsswBasePackage + '/scripts/SiStripDQMCAFHarvest_template.job > ' + str_nameRun + '/SiStripCAFHarvest.job'
os.system(str_sedCommand)
# prepare included CAF input files list
str_pathInputFilesCAFCff = str_pathRunIncludeDir + '/inputFilesCAF_cff.py'
file_inputFilesCAFCff = file(str_pathInputFilesCAFCff, 'w')
file_inputFilesCAFCff.write('import FWCore.ParameterSet.Config as cms\n\nsource = cms.Source ("PoolSource",\n    processingMode = cms.untracked.string( \'Runs\' ),\n    fileNames      = cms.untracked.vstring(\n')

# loop over single jobs
if Bool_CRAB:
  os.chdir(str_nameRun)     
  str_outputDir = '.'
  str_sedCommand = 'sed '
  if Bool_filter:
    str_sedCommand += '-e \"s#xHLT_FILTERx#    #g\" '
  else:
    str_sedCommand += '-e \"s#xHLT_FILTERx#\#     #g\" '
  if Str_datatier == 'RECO':
    str_sedCommand += '-e \"s#xRECO_FROM_RAWx#\#     #g\" '
    str_sedCommand += '-e \"s#xDQM_FROM_RAWx#\#     #g\" '
  else:
    str_sedCommand += '-e \"s#xRECO_FROM_RAWx#    #g\" '
    str_sedCommand += '-e \"s#xDQM_FROM_RAWx#    #g\" '
  str_sedCommand += '-e \"s#xMAG_FIELDx#'         + str_magField          + '#g\" '
  str_sedCommand += '-e \"s#xGLOBAL_TAGx#'        + Str_globalTag         + '#g\" '
  str_sedCommand += '-e \"s#xINCLUDE_DIRECTORYx#' + str_nameRunIncludeDir + '#g\" '
  str_sedCommand += '-e \"s#xOUTPUT_DIRECTORYx#'  + str_outputDir         + '#g\" '
  str_sedCommand += '-e \"s#xRUN_NUMBERx#'        + str_nameRun           + '#g\" '
  str_sedCommand += str_pathCmsswBasePackage + '/test/SiStripDQMOfflineGlobalRunCAF_template_cfg.py > SiStripDQMOfflineGlobalRunCAF_cfg.py'
  os.system(str_sedCommand)
  # create included input files list
  str_pathInputFilesJobCff = str_pathRunIncludeDir + '/inputFiles_cff.py'
  file_inputFilesJobCff = file(str_pathInputFilesJobCff, 'w')
  file_inputFilesJobCff.write('import FWCore.ParameterSet.Config as cms\n\nsource = cms.Source ("PoolSource",\n    fileNames = cms.untracked.vstring (\n')
  nLines = 0
  for str_linesInput in lstr_linesInput:
    nLines += 1
    str_correctedLine = str_linesInput.replace(') );',',')
    if nLines == len(lstr_linesInput):
      str_actualLine = str_correctedLine.replace(',','\n    )\n)\n')
    elif nLines%255 == 0:
      str_actualLine = str_correctedLine.replace(',','\n    )\n)\nsource.fileNames.extend(\n    (')
    else:
      str_actualLine = str_correctedLine
    file_inputFilesJobCff.write(str_actualLine)
  file_inputFilesJobCff.close()
  for int_iJob in range(Int_jobs):
    # extend included CAF input files list
    str_lineInput = Str_outpath + '/SiStripDQMOfflineGlobalRunCAF-' + str_nameRun + '_' + str(int_iJob+1) + '.root'
    if Bool_useCastor:
      str_lineInput = 'rfio:' + str_lineInput
    str_lineInput = '        \'' + str_lineInput + '\''
    if int_iJob == Int_jobs-1:
      str_lineInput += '\n'
      file_inputFilesCAFCff.write(str_lineInput)
      print '> submitDQMOfflineCAF.py > number of created jobs: ' + str(int_iJob+1)
      print
      break
    str_lineInput += ',\n'
    file_inputFilesCAFCff.write(str_lineInput)
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
    str_nameJobIncludeDir = STR_nameCmsswPackage.replace('/','.') + '.' + str_nameJobDir.replace('/','.')
    os.mkdir(str_nameJobDir)
    os.chdir(str_nameJobDir)     
    # create main configuration file
    str_sedCommand = 'sed '
    if Bool_filter:
      str_sedCommand += '-e \"s#xHLT_FILTERx#    #g\" '
    else:
      str_sedCommand += '-e \"s#xHLT_FILTERx#\#     #g\" '
    if Str_datatier == 'RECO':
      str_sedCommand += '-e \"s#xRECO_FROM_RAWx#\#     #g\" '
      str_sedCommand += '-e \"s#xDQM_FROM_RAWx#\#     #g\" '
    else:
      str_sedCommand += '-e \"s#xRECO_FROM_RAWx#    #g\" '
      str_sedCommand += '-e \"s#xDQM_FROM_RAWx#    #g\" '
    str_sedCommand += '-e \"s#xMAG_FIELDx#'         + str_magField          + '#g\" '
    str_sedCommand += '-e \"s#xGLOBAL_TAGx#'        + Str_globalTag         + '#g\" '
    str_sedCommand += '-e \"s#xINCLUDE_DIRECTORYx#' + str_nameJobIncludeDir + '#g\" '
    str_sedCommand += '-e \"s#xOUTPUT_DIRECTORYx#'  + str_outputDir         + '#g\" '
    str_sedCommand += '-e \"s#xRUN_NUMBERx#'        + str_nameRun           + '#g\" '
    str_sedCommand += str_pathCmsswBasePackage + '/test/SiStripDQMOfflineGlobalRunCAF_template_cfg.py > SiStripDQMOfflineGlobalRunCAF_cfg.py'
    os.system(str_sedCommand)
    # prepare job include dir
    os.mkdir(str_pathJobIncludeDir)
    # create included input files list
    str_pathInputFilesJobCff = str_pathJobIncludeDir + '/inputFiles_cff.py'
    file_inputFilesJobCff = file(str_pathInputFilesJobCff, 'w')
    file_inputFilesJobCff.write('import FWCore.ParameterSet.Config as cms\n\nsource = cms.Source ("PoolSource",\n    fileNames = cms.untracked.vstring (\n')
    for n_iActualLine in range(int_nLinesRead, min(int_nLinesRead+int_nInputFilesJob, int_nInputFiles)):
      # protections vs. those annoying DBS output format changes come here:
#       str_linesInput = lstr_linesInput[n_iActualLine]
      str_linesInput = lstr_linesInput[n_iActualLine].replace(') );',',')
      # fix commata and end of line
      str_actualLine = str_linesInput
      if (n_iActualLine+1)%int_nInputFilesJob == 0 or int_nLinesRead == int_nInputFiles-1:
        str_actualLine = str_linesInput.split(',')[0] + '\n'
      file_inputFilesJobCff.write(str_actualLine)
      int_nLinesRead += 1
    file_inputFilesJobCff.write('    )\n)\n')
    file_inputFilesJobCff.close()
    # create job script
    str_sedCommand = 'sed '
    str_sedCommand += '-e \"s#xCMSSW_BASEx#'  + str_pathCmsswBase   + '#g\" '
    str_sedCommand += '-e \"s#xRUN_NAMEx#'    + str_nameRun        + '#g\" '
    str_sedCommand += '-e \"s#xJOB_NAMEx#'    + str_nameJob        + '#g\" '
    str_sedCommand += '-e \"s#xCURRENT_DIRx#' + str_pathCurrentDir + '#g\" '
    str_sedCommand += '-e \"s#xCOPYx#'        + str_castorCp       + '#g\" '
    str_sedCommand += '-e \"s#xOUTPUT_DIRx#'  + Str_outpath + '#g\" '
    str_sedCommand += str_pathCmsswBasePackage + '/scripts/SiStripDQMOfflineCAF_template.job > SiStripDQMOfflineCAF.job'
    os.system(str_sedCommand)
    # finalize job creation
    os.chdir(str_pathCurrentDir)
    # extend included CAF input files list
    str_lineInput = Str_outpath + '/SiStripDQMOfflineGlobalRunCAF-' + str_nameRun + '_' + str(int_iJob+1) + '.root'
    if Bool_useCastor:
      str_lineInput = 'rfio:' + str_lineInput
    str_lineInput = '        \'' + str_lineInput + '\''
    # FIXME: This protection is currently needed. Review calculations again!
    if int_nLinesRead >= int_nInputFiles:
      str_lineInput += '\n'
      file_inputFilesCAFCff.write(str_lineInput)
      str_nJobs = str(int_iJob+1)
      print '> submitDQMOfflineCAF.py > number of created jobs: ' + str_nJobs
      print
      break
    str_lineInput += ',\n'
    file_inputFilesCAFCff.write(str_lineInput)
    
# Finish scripts
    
# finish included CAF input files list
file_inputFilesCAFCff.write('    )\n)\n')
file_inputFilesCAFCff.close()

# Compile

os.chdir(str_pathRunIncludeDir+'/..')
os.system('scramv1 b python')
os.chdir(str_pathCurrentDir)
print

# CRAB

# create CRAB configuration
if Bool_CRAB:
  os.chdir(str_nameRun)
  lstr_outpath = Str_outpath.split('/', 3)
  str_outpath  = lstr_outpath[0] + '/' + lstr_outpath[1] + '/' + lstr_outpath[2] 
  str_sedCommand  = 'sed '
  str_sedCommand += '-e \"s#xSERVER_NAMEx#'        + Str_server               + '#g\" '
  str_sedCommand += '-e \"s#xDATASETPATHx#'        + Str_dataset              + '#g\" '
  str_sedCommand += '-e \"s#xRUNSELECTIONx#'       + Str_run                  + '#g\" '
  str_sedCommand += '-e \"s#xNUMBER_OF_JOBSx#'     + str_nJobs                + '#g\" '
  str_sedCommand += '-e \"s#xEMAILx#'              + Str_email                + '#g\" '
  str_sedCommand += '-e \"s#xRUN_NUMBERx#'         + str_nameRun              + '#g\" '
  str_sedCommand += '-e \"s#xUI_WORKING_DIRx#crab' + str_nameRun              + '#g\" '
  str_sedCommand += '-e \"s#xSTORAGE_PATHx#'       + str_outpath              + '#g\" '
  str_sedCommand += '-e \"s#xLFNx#'                + lstr_outpath[3]          + '#g\" '
  str_sedCommand += '-e \"s#xCOPY_DATAx#'          + str(int(Bool_useCastor)) + '#g\" '
  str_sedCommand += str_pathCmsswBasePackage + '/test/SiStripDQMOfflineCAF_template.crab > crab.cfg'
  os.system(str_sedCommand)
  os.system('crab -create')
  os.chdir(str_pathCurrentDir)

# Submit jobs

if Dict_arguments.has_key(LSTR_functionLetters[0]):
  os.chdir(str_nameRun)
  if Bool_CRAB:
    print '> submitDQMOfflineCAF.py >'
    print '  ' + os.getcwd() + ' : crab -submit -c crab' + str_nameRun
    os.system('crab -submit -c crab' + str_nameRun)
    print
    time.sleep(5)
    os.system('crab -status -c crab' + str_nameRun)
  else:
    for int_iJob in range(Int_jobs):
      int_nDigits = 1
      if int_iJob >= 10:
        int_nDigits = int(math.log10(int_iJob)) + 1
      str_nameJobDir = str_nameRun + "_"
      for int_iDigit in range(4-int_nDigits):
        str_nameJobDir += '0'
      str_nameJobDir += str(int_iJob)
      os.chdir(str_nameJobDir)     
      os.chmod('SiStripDQMOfflineCAF.job',OCT_rwx_r_r)
      print '> submitDQMOfflineCAF.py >'
      print '  ' + os.getcwd() + ' : bsub -q cmscaf SiStripDQMOfflineCAF.job'
      os.system('bsub -q cmscaf SiStripDQMOfflineCAF.job')
      print
      os.chdir('../')
    time.sleep(5)
    os.system('bjobs -q cmscaf')
  os.chmod('SiStripCAFHarvest.job',OCT_rwx_r_r)
  os.chdir(str_pathCurrentDir)

# Send reminder email to submitter (not needed for CRAB)
    
if Dict_arguments.has_key(LSTR_functionLetters[0]) and not Bool_CRAB:
  str_mailFrom    = os.getenv('USER') + STR_mailServer
  str_mailTo      = [str_mailFrom,
                     'volker.adler@cern.ch',
                     'suchandra.dutta@cern.ch',
                     'domenico.giordano@cern.ch',
                     'vitaliano.ciulli@cern.ch']
  str_mailSubject = 'Your SiStrip offline DQM shift on ' + str(datetime.date.today()) + ', run ' + Str_run
  str_mailText    = STR_mailTextOpener + Str_run + STR_mailTextExplain
  if Bool_CRAB:
    str_mailText += """
To do so, please login on lxplus and then:

$ cd """ + str_pathCurrentDir + '/' + str_nameRun + """
$ cmsenv
$ source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.[SHELL]
$ crab -status -c crab""" + str_nameRun + """

As soon as all jobs are in 'Done' status, retrieve the output with

$ crab -getoutput -c crab""" + str_nameRun + """
"""
  else:
    str_mailText += STR_mailTextCRABNo
  str_mailText += STR_mailTextFinish
  str_mailMessage = """From: %s
To: %s
Subject: %s

%s
"""  % (str_mailFrom, ", ".join(str_mailTo), str_mailSubject, str_mailText)   
  server = smtplib.SMTP(STR_mailSmtp)
  server.sendmail(str_mailFrom, str_mailTo, str_mailMessage)
  server.quit()
