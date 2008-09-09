#!/usr/bin/env python

#
# $Id$
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
# strings
STR_SiStrip     = 'SiStrip'
STR_good        = 'SS_GOOD'
STR_htmlL1Key   = '&lt;b>L1&amp;nbsp;Key:&lt;/b>'
STR_htmlHLTKey  = '&lt;b>HLT&amp;nbsp;Key:&lt;/b>'
# Globals

global Str_run
global Dict_cmsmonRunRegistry
global Dict_cmsmonSubsystems
# initialise
Str_run                = sys.argv[1]
Dict_cmsmonRunRegistry = {}
Dict_cmsmonSubsystems  = {}
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
  lstr_split = str_text.split('</')
  lstr_tags  = []
  for str_split in lstr_split[1:]:
    lstr_tags.append(str_split.split('>')[0])
  return lstr_tags
 
## Func_GetHtmlTagValue(str_tag, str_text)
#
# Gets the value of a given HTML tag from a string
def Func_GetHtmlTagValue(str_tag, str_text):
  """  Func_GetHtmlTagValue(str_tag, str_text):
  Gets the value of a given HTML tag from a string
  """
  return str_text.split('<'+str_tag+'>')[1].split('</'+str_tag+'>')[0]

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
# Gets the attributes of a given HTML tag value from a string
def Func_GetHtmlTagValueAttr(str_value, str_text):
  """  Func_GetHtmlTagValueAttr(str_tag, str_text):
  Gets the attributes of a given HTML tag value from a string
  """
  return str_text.split('\">'+str_value+'<')[0].split('=\"')[-1]
 
## MAIN PROGRAM

print
print '> getRunInfo.py > information on run \t*** ' + Str_run + ' ***'
print

# Get run information from the web

# Run registry
# get run registry
str_cmsmonRunRegistry      = urllib.urlencode({'template':'text'})
file_cmsmonRunRegistry     = urllib.urlopen("http://cmsmon.cern.ch/runregistry/allrundata")
str_cmsmonRunRegistryLong1 = ''
str_cmsmonRunRegistryLong2 = ''
for str_cmsmonRunRegistry in file_cmsmonRunRegistry.readlines():
  str_cmsmonRunRegistryLong1 += str_cmsmonRunRegistry
for str_cmsmonRunRegistry in str_cmsmonRunRegistryLong1.splitlines():
  str_cmsmonRunRegistryLong2 += str_cmsmonRunRegistry
if str_cmsmonRunRegistryLong2.find('<run id=\"'+Str_run+'\">') < 0:
  print '> getRunInfo.py > run ' + Str_run + ' not found in run registry'
  Func_Exit()
str_cmsmonRun = str_cmsmonRunRegistryLong2.split('<run id=\"'+Str_run+'\">')[1].split('</run>')[0]
for str_cmsmonHtmlTag in Func_GetHtmlTags(str_cmsmonRun):
  Dict_cmsmonRunRegistry[str_cmsmonHtmlTag] = Func_GetHtmlTagValue(str_cmsmonHtmlTag, str_cmsmonRun)
# check SiStrip
if Dict_cmsmonRunRegistry['subsystems'].find(STR_SiStrip) < 0:
  print '> getRunInfo.py > SiStrip was not in this run'
  Func_Exit()
str_htmlSubsystems = Dict_cmsmonRunRegistry['formatted_subsystems'].replace('&lt;','<')
for str_htmlSubsystem in Func_GetHtmlTagValues(str_htmlSubsystems):
  Dict_cmsmonSubsystems[str_htmlSubsystem] = Func_GetHtmlTagValueAttr(str_htmlSubsystem, str_htmlSubsystems)
print '> getRunInfo.py > SiStrip DQM from global : ' + Dict_cmsmonSubsystems[STR_SiStrip][3:]
if not Dict_cmsmonSubsystems[STR_SiStrip] == STR_good:
  Func_Exit()
print

# get run DBS entries
str_dbsRuns  = urllib.urlencode({'ajax':'0', 'userMode':'user', 'release':'Any', 'tier':'Any', 'dbsInst':'cms_dbs_prod_global', 'primType':'Any', 'primD':'Any', 'minRun':Str_run, 'maxRun':Str_run})
file_dbsRuns = urllib.urlopen("https://cmsweb.cern.ch/dbs_discovery/getRunsFromRange", str_dbsRuns)
# for str_dbsRuns in file_dbsRuns.readlines():
#   print str_dbsRuns
# print

# get run summary
str_cmsmonRunSummary  = urllib.urlencode({'RUN':Str_run})
file_cmsmonRunSummary = urllib.urlopen("http://cmsmon.cern.ch/cmsdb/servlet/RunSummary", str_cmsmonRunSummary)

# Determine further information

# get magnetic field
lstr_dateStart = Dict_cmsmonRunRegistry['RUN_START_TIME'].split(' ')[0].split('.')
lstr_timeStart = Dict_cmsmonRunRegistry['RUN_START_TIME'].split(' ')[1].split(':')
lstr_dateEnd   = Dict_cmsmonRunRegistry['RUN_END_TIME'].split(' ')[0].split('.')
lstr_timeEnd   = Dict_cmsmonRunRegistry['RUN_END_TIME'].split(' ')[1].split(':')
dt_oldStart    = datetime.datetime(int(lstr_dateStart[0]),int(lstr_dateStart[1]),int(lstr_dateStart[2]),int(lstr_timeStart[0]),int(lstr_timeStart[1]),int(lstr_timeStart[2]))
dt_oldEnd      = datetime.datetime(int(lstr_dateEnd[0]),  int(lstr_dateEnd[1]),  int(lstr_dateEnd[2]),  int(lstr_timeEnd[0]),  int(lstr_timeEnd[1]),  int(lstr_timeEnd[2]))
dt_newStart    = dt_oldStart - TD_shiftUTC
dt_newEnd      = dt_oldEnd   - TD_shiftUTC
str_cmsmonMagnetHistory  = urllib.urlencode({'TIME_BEGIN':dt_newStart, 'TIME_END':dt_newEnd})
file_cmsmonMagnetHistory = urllib.urlopen("http://cmsmon.cern.ch/cmsdb/servlet/MagnetHistory", str_cmsmonMagnetHistory)
float_avMagMeasure = -999.0
for str_cmsmonMagnetHistory in file_cmsmonMagnetHistory.readlines():
  if str_cmsmonMagnetHistory.find('BFIELD, Tesla') >= 0:
    float_avMagMeasure = float(str_cmsmonMagnetHistory.split('</A>')[0].split('>')[-1])
# get L1 and HLT key
str_htmlL1Key  = Dict_cmsmonRunRegistry['comment'].split(STR_htmlL1Key)[1].split('&lt;')[0]
str_htmlHLTKey = Dict_cmsmonRunRegistry['comment'].split(STR_htmlHLTKey)[1].split('&lt;')[0]
    
# Print information

print '> getRunInfo.py > global name             : ' + Dict_cmsmonRunRegistry['RUN_GLOBALNAME']
print '> getRunInfo.py > status                  : ' + Dict_cmsmonRunRegistry['RUN_STATUS']
print '> getRunInfo.py > in DBS                  : ' + Dict_cmsmonRunRegistry['RUN_INDBS']
print '> getRunInfo.py > subsystems              : ' + Dict_cmsmonRunRegistry['subsystems']
print '> getRunInfo.py > (average) magnetic field: ' + str(float_avMagMeasure) + ' T'
print '> getRunInfo.py > # of triggers           : ' + Dict_cmsmonRunRegistry['RUN_EVENTS']
print '> getRunInfo.py > start time              : ' + Dict_cmsmonRunRegistry['RUN_START_TIME']
print '> getRunInfo.py > end time                : ' + Dict_cmsmonRunRegistry['RUN_END_TIME']
print '> getRunInfo.py > L1 key                  : ' + str_htmlL1Key
print '> getRunInfo.py > HLT key                 : ' + str_htmlHLTKey
print '> getRunInfo.py > L1 sources              : ' + Dict_cmsmonRunRegistry['l1sources']
print '> getRunInfo.py > event rate              : ' + Dict_cmsmonRunRegistry['RUN_RATE'] + ' Hz'
print '> getRunInfo.py > stop reason             : ' + Dict_cmsmonRunRegistry['stop_reason']
print '> getRunInfo.py > shifter                 : ' + Dict_cmsmonRunRegistry['RUN_SHIFTER']
print '> getRunInfo.py > shifter\'s comment       : ' + Dict_cmsmonRunRegistry['RUN_COMMENT']
print
