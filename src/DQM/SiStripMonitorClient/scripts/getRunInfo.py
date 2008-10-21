#!/usr/bin/env python

#
# $Id: getRunInfo.py,v 1.3.2.1 2008/09/16 17:18:46 vadler Exp $
#

## CMSSW/DQM/SiStripMonitorClient/scripts/getRunInfo.py
#
#  For a given run, this script collects information useful for SiStrip DQM
#  from web sources.
#  Questions and comments to: volker.adler@cern.ch


import sys
import os
import string
import urllib
import datetime

# Constants

LSTR_arguments = sys.argv[1:]
# numbers
TD_shiftUTC = datetime.timedelta(hours = 2) # positive for timezones with later time than UTC
INT_offset  = 8
# strings
STR_SiStrip             = 'SIST'
STR_good                = 'SS_GOOD'
STR_htmlL1Key           = '&lt;b>L1&amp;nbsp;Key:&lt;/b>'
STR_htmlHLTKey          = '&lt;b>HLT&amp;nbsp;Key:&lt;/b>'
STR_wwwDBSData          = 'https://cmsweb.cern.ch/dbs_discovery/getData'
STR_headDatasets        = 'datasets'
STR_headFiles           = 'available data files'
LSTR_summaryKeys        = ['BField', 'HLT Version', 'L1 Rate', 'HLT Rate', 'L1 Triggers', 'HLT Triggers', 'LHC Fill', 'LHC Energy', 'Initial Lumi', 'Ending Lumi', 'Run Lumi', 'Run Live Lumi']
LSTR_summaryKeysTrigger = ['L1 Key', 'HLT Key']   

# Globals

global Str_run
global Dict_cmsmonRunRegistry
global Dict_cmsmonSubsystems
global Dict_cmsmonRunSummary
global Dict_dbsDatasets
global Dict_dbsEvents
global Lstr_hltPaths
# initialise
Str_run                = sys.argv[1]
Dict_cmsmonRunRegistry = {}
Dict_cmsmonSubsystems  = {}
Dict_cmsmonRunSummary  = {}
Dict_dbsDatasets       = {}
Dict_dbsEvents         = {}
Lstr_hltPaths          = []

## FUNCTIONS

## Function Func_Exit()
#
#  Exit after error
def Func_Exit():
  """  Function Func_Exit():
  Exit after error
  """
  print '                  exit'
  print
  sys.exit(1)

## Func_GetHtmlTags(str_text)
#
# Gets HTML tags from a string
def Func_GetHtmlTags(str_text):
  """  Func_GetHtmlTags(str_text):
  Gets HTML tags from a string
  """
  dict_tags  = {}
  # first look for tags w/ values
  lstr_split = str_text.split('</')
  for str_split in lstr_split[1:]:
    str_key            = str_split.split('>')[0]
    dict_tags[str_key] = str_key in dict_tags
  # second look for tags w/o values
  lstr_split = str_text.split('/>')
  for str_split in lstr_split[:-1]:
    str_key            = str_split.split('<')[-1].split()[0]
    dict_tags[str_key] = str_key in dict_tags
  return dict_tags
 
## Func_GetHtmlTagValue(str_tag, str_text)
#
# Gets the value of the n-th oocurence a given HTML tag from a string
def Func_GetHtmlTagValue(str_tag, str_text, int_index = 1):
  """  Func_GetHtmlTagValue(str_tag, str_text):
   Gets the value of the n-th oocurence a given HTML tag from a string
  """
  if int_index > str_text.count('<'+str_tag):
    return ''
  str_1 = str_text.split('<'+str_tag)[int_index]
  if str_1[0] != '>':
    if str_1.split('>')[0][-1] == '/':
      return ''
  return str_1.split('>',1)[1].split('</'+str_tag+'>')[0]

## Func_GetHtmlTagValues(str_text)
#
# Gets HTML tag values from a string
def Func_GetHtmlTagValues(str_text):
  """  Func_GetHtmlTagValues(str_text):
  Gets HTML tag values from a string
  """
  lstr_split   = str_text.split('</')
  lstr_values  = []
  for str_split in lstr_split[:-1]:
    lstr_values.append(str_split.split('>')[-1])
  return lstr_values
 
## Func_GetHtmlTagValueAttr(str_tag, str_text)
#
# Gets the (last) attribute of a given HTML tag value from a string
def Func_GetHtmlTagValueAttr(str_value, str_text):
  """  Func_GetHtmlTagValueAttr(str_value, str_text):
  Gets the (last) attributes of a given HTML tag value from a string
  """
  return str_text.split('\">'+str_value+'<')[0].split('=\"')[-1]
 
## MAIN PROGRAM

print
print '> getRunInfo.py > information on run \t*** ' + Str_run + ' ***'
print

# Get run information from the web

# Run registry
# get run registry
str_cmsmonRunRegistry     = urllib.urlencode({'format':'xml', 'intpl':'xml', 'qtype':'RUN_NUMBER', 'sortname':'RUN_NUMBER'})
file_cmsmonRunRegistry    = urllib.urlopen("http://pccmsdqm04.cern.ch/runregistry/runregisterdata", str_cmsmonRunRegistry)
lstr_cmsmonRunRegistry    = []
str_cmsmonRunRegistryLong = ''
for str_cmsmonRunRegistry in file_cmsmonRunRegistry.readlines():
  lstr_cmsmonRunRegistry.append(str_cmsmonRunRegistry) # store run registry information
  str_cmsmonRunRegistryLong += str_cmsmonRunRegistry.splitlines()[0]
bool_foundRun = False
str_cmsmonRun = ''
for int_runIndex in range(1,int(str_cmsmonRunRegistryLong.split('<RUNS')[1].split('>')[0].split('total=\"')[1].split('\"')[0])):
  str_cmsmonRun = Func_GetHtmlTagValue('RUN', str_cmsmonRunRegistryLong, int_runIndex)
  if Func_GetHtmlTagValue('NUMBER', str_cmsmonRun) == Str_run:
    bool_foundRun = True
    break
dict_cmsmonHtmlTags = Func_GetHtmlTags(str_cmsmonRun)
for str_cmsmonHtmlTag in dict_cmsmonHtmlTags.keys():
  if dict_cmsmonHtmlTags[str_cmsmonHtmlTag] == False:
    Dict_cmsmonRunRegistry[str_cmsmonHtmlTag] = Func_GetHtmlTagValue(str_cmsmonHtmlTag, str_cmsmonRun)
if ( not bool_foundRun ):
  print '> getRunInfo.py > run ' + Str_run + ' not found in run registry'
  Func_Exit()
if Dict_cmsmonRunRegistry['SUBSYSTEMS'].find(STR_SiStrip) < 0:
  print '> getRunInfo.py > SiStrip was not in this run'
  Func_Exit()

# get run DBS entries
str_dbsRuns      = urllib.urlencode({'ajax':'0', '_idx':'0', 'pagerStep':'0', 'userMode':'user', 'release':'Any', 'tier':'Any', 'dbsInst':'cms_dbs_caf_analysis_01', 'primType':'Any', 'primD':'Any', 'minRun':Str_run, 'maxRun':Str_run})
file_dbsRuns     = urllib.urlopen("https://cmsweb.cern.ch/dbs_discovery/getRunsFromRange", str_dbsRuns)
lstr_dbsRuns     = []
lstr_dbsDatasets = []
for str_dbsRuns in file_dbsRuns.readlines():
  lstr_dbsRuns.append(str_dbsRuns) # store run DBS information
  if str_dbsRuns.find(STR_wwwDBSData) >= 0:
    if str_dbsRuns.split('&amp;proc=')[1].find('&amp;') >= 0:
      lstr_dbsDatasets.append(str_dbsRuns.split('&amp;proc=')[1].split('&amp;')[0])
    else:
      lstr_dbsDatasets.append(str_dbsRuns.split('&amp;proc=')[1])
int_maxLenDbsDatasets = 0
for str_dbsDatasets in lstr_dbsDatasets:
  str_dbsLFN  = urllib.urlencode({'dbsInst':'cms_dbs_prod_global', 'blockName':'*', 'dataset':str_dbsDatasets, 'userMode':'user', 'run':Str_run})
  file_dbsLFN = urllib.urlopen("https://cmsweb.cern.ch/dbs_discovery/getLFNlist", str_dbsLFN)
  lstr_dbsLFN = []
  int_events  = 0
  for str_dbsLFN in file_dbsLFN.readlines():
    lstr_dbsLFN.append(str_dbsLFN)
    if str_dbsLFN.find('contians') >= 0 and str_dbsLFN.find('file(s)'): # FIXME: be careful, this typo might be corrected sometimes on the web page...
      Dict_dbsDatasets[str_dbsDatasets] = str_dbsLFN.split()[1]
    if str_dbsLFN.startswith('/store/data/'):
      int_events += int(Func_GetHtmlTagValue('td' ,lstr_dbsLFN[len(lstr_dbsLFN)-4]))
  Dict_dbsEvents[str_dbsDatasets] = str(int_events)
  if len(str_dbsDatasets) > int_maxLenDbsDatasets:
    int_maxLenDbsDatasets = len(str_dbsDatasets)
    
# Print information

# from run registry
print '> getRunInfo.py > * information from run registry *'
print
if 'GLOBAL_NAME' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > global name                  : ' + Dict_cmsmonRunRegistry['GLOBAL_NAME']
if 'STATUS' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > status                       : ' + Dict_cmsmonRunRegistry['STATUS']
if 'IN_DBS' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > in DBS                       : ' + Dict_cmsmonRunRegistry['IN_DBS']
if 'SUBSYSTEMS' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > subsystems                   : ' + Dict_cmsmonRunRegistry['SUBSYSTEMS']
if 'EVENTS' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > # of triggers                : ' + Dict_cmsmonRunRegistry['EVENTS']
if 'START_TIME' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > start time (local)           : ' + Dict_cmsmonRunRegistry['START_TIME']
if 'END_TIME' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > end time (local)             : ' + Dict_cmsmonRunRegistry['END_TIME']
if 'L1KEY' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > L1 key                       : ' + Dict_cmsmonRunRegistry['L1KEY']
if 'HLTKEY' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > HLT key                      : ' + Dict_cmsmonRunRegistry['HLTKEY']
if 'L1SOURCES' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > L1 sources                   : ' + Dict_cmsmonRunRegistry['L1SOURCES']
# if 'RUN_RATE' in Dict_cmsmonRunRegistry:
#   print '> getRunInfo.py > event rate                   : ' + Dict_cmsmonRunRegistry['RUN_RATE'] + ' Hz'
if 'STOP_REASON' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > stop reason                  : ' + Dict_cmsmonRunRegistry['STOP_REASON']
if 'SHIFTER' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > DQM shifter                  : ' + Dict_cmsmonRunRegistry['SHIFTER']
if 'CREATE_USER' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > entry created by             : ' + Dict_cmsmonRunRegistry['CREATE_USER']
if 'CREATE_TIME' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > entry creation time          : ' + Dict_cmsmonRunRegistry['CREATE_TIME']
if 'ONLINE_COMMENT' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > DQM online shifter\'s comment : ' + Dict_cmsmonRunRegistry['ONLINE_COMMENT']
if 'OFFLINE_COMMENT' in Dict_cmsmonRunRegistry:
  print '> getRunInfo.py > DQM offline shifter\'s comment: ' + Dict_cmsmonRunRegistry['OFFLINE_COMMENT']
print
# from DBS
print '> getRunInfo.py > * information from DBS *'
print
str_print = '> getRunInfo.py > ' + STR_headDatasets
for int_i in range(int_maxLenDbsDatasets-len(STR_headDatasets)):
  str_print += ' '
str_print += ' '
int_length = len(str_print)
print str_print + STR_headFiles
str_print = '> '
for int_i in range(int_length+len(STR_headFiles)/2+INT_offset+8):
  str_print += '-'
print str_print
for str_dbsDatasets in lstr_dbsDatasets:
  str_print = '                  ' + str_dbsDatasets
  for int_i in range(int_maxLenDbsDatasets-len(str_dbsDatasets)):
    str_print += ' '
  str_print += ' '
  for int_i in range(len(STR_headFiles)/2-len(Dict_dbsDatasets[str_dbsDatasets])):
    str_print += ' '
  str_print += Dict_dbsDatasets[str_dbsDatasets] + ' ('
  for int_i in range(INT_offset-len(Dict_dbsEvents[str_dbsDatasets])):
    str_print += ' '
  print str_print + Dict_dbsEvents[str_dbsDatasets] + ' events)'
print  
