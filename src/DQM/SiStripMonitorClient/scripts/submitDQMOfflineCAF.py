#!/afs/cern.ch/cms/sw/slc4_ia32_gcc345/external/python/2.4.2-cms2/bin/python

import sys
import os
import os.path
import shutil
import string
import math
import urllib
import time

def WrongUsage():
  print '> submitDQMOfflineCAF.py > wrong usage'
  print '  requires arguments:'
  print '  - run number'
  print '  - split level'
  print '  - action level (\'create\', \'submit\')'
  sys.exit(1)

if len(sys.argv) != 4:
  WrongUsage()

actionLevel =     sys.argv[1]
runNumber   =     sys.argv[2]
splitLevel  = int(sys.argv[3])

if actionLevel != 'submit' and actionLevel != 'create':
  WrongUsage()

# shell = os.path.basename(os.getenv('SHELL'))
# shellType = 'sh'
# if (shell.endswith('csh')):
#   shellType = 'csh'
# command = 'source ' + os.getenv('VO_CMS_SW_DIR') + '/cmsset_default.' + shellType + '; cmsenv'  
# os.system(command) # still doesn't work
# cmsswBase = os.getenv('CMSSW_BASE')
currentDir  = os.getcwd()
pathStrings = string.split(currentDir, '/src/') # assumes to be in _the_ release working area
cmsswBase   = pathStrings[0]                    # assumes to be in _the_ release working area 
packageDir  = os.path.split(pathStrings[1])[0]  # works only one level below the package
  
digits = 1
if int(runNumber) >= 10:
  digits = int(math.log10(int(runNumber))) + 1
runName = 'R'
for i in range(9-digits):
  runName += '0'
runName += runNumber

for root, dirs, files in os.walk(runName, topdown = False):
  for name in files:
    os.remove(os.path.join(root, name))
  for name in dirs:
    os.rmdir(os.path.join(root, name))
os.rmdir(runName)
os.mkdir(runName)

dbsParams  = urllib.urlencode({'dbsInst': 'cms_dbs_prod_global', 'blockName': '*', 'dataset': '/Cosmics/CRUZET3_CRUZET3_V2P_v3/RECO', 'userMode': 'user', 'run': runNumber, 'what': 'cff'})
dbsOutput  = urllib.urlopen("https://cmsweb.cern.ch/dbs_discovery/getLFN_txt", dbsParams)
inputFiles = 0
storePath  = '/store/data/CRUZET3/Cosmics/RECO/CRUZET3_V2P_v3'
inputCff = file(runName + '/' + runName + '.cff','w')
for inputLine in dbsOutput.readlines():
  if inputLine.find(storePath) > -1:
    inputFiles = inputFiles + 1
    inputCff.write(inputLine)
inputCff.close()
inputFilesJob = int(inputFiles/splitLevel) + 1
if inputFiles == inputFilesJob * (splitLevel-1):
  splitLevel -= 1
print '> submitDQMOfflineCAF.py > input files for run' + runNumber + ': ' + str(inputFiles)
  
readLines = 0
inputCff = file(runName + '/' + runName + '.cff','r')
inputLines = inputCff.readlines()
for i in range(splitLevel):
  digits = 1
  if i >= 10:
    digits = int(math.log10(i)) + 1
  jobName = runName + "_"
  for j in range(4-digits):
    jobName += '0'
  jobName += str(i)
  jobDir   = runName + "/" + jobName
  os.mkdir(jobDir)
  os.chdir(jobDir)
  inputCffNameJob = 'inputFiles.cff'
  os.system('sed -e \"s#INCLUDE_DIRECTORY#' + pathStrings[1] + '/' + jobDir + '#g\" -e \"s#INPUT_FILES#' + pathStrings[1] + '/' + jobDir + '/' + inputCffNameJob + '#g\" ' + cmsswBase + '/src/DQM/SiStripMonitorClient/test/SiStripDQMOfflineGlobalRunCAF_template.cfg > SiStripDQMOfflineGlobalRunCAF.cfg')
  inputCffJob = file(inputCffNameJob, 'w')
  inputCffJob.write('  source = PoolSource {\n')
  inputCffJob.write('    untracked vstring fileNames = {\n')
  for j in range(readLines, min(readLines+inputFilesJob, inputFiles)):
    thisLine    = string.split('      \'/store/data/' + string.split(inputLines[j], '/store/data/')[1], '.root\',')[0] + '.root\',\n'
    actualLine  = thisLine
    if (j+1)%inputFilesJob == 0 or readLines == inputFiles-1:
      actualLine = string.split(thisLine, ',')[0] + '\n'
    inputCffJob.write(actualLine)
    readLines += 1
  inputCffJob.write('    }\n')
  inputCffJob.write('  }\n')
  inputCffJob.close()
  os.system('sed -e \"s#OUTPUT_DIRECTORY#/tmp/' + os.getenv('USER') + '/' + jobDir + '#g\" ' + cmsswBase + '/src/DQM/SiStripMonitorClient/data/SiStripDQMOfflineGlobalRunCAF_template.cff > SiStripDQMOfflineGlobalRunCAF.cff')
  os.system('sed -e \"s#CMSSW_BASE#' + cmsswBase + '#g\" -e \"s#RUN_NAME#' + runName + '#g\" -e \"s#JOB_NAME#' + jobName + '#g\" -e \"s#CURRENT_DIR#' + currentDir + '#g\" ' + cmsswBase + '/src/DQM/SiStripMonitorClient/scripts/SiStripDQMOfflineCAF_template.job > SiStripDQMOfflineCAF.job')
  os.chmod('SiStripDQMOfflineCAF.job',484)
  if actionLevel == 'submit':
    print '> submitDQMOfflineCAF.py >'
    print '  ' + os.getcwd() + ' : bsub -q cmscaf SiStripDQMOfflineCAF.job'
    os.system('bsub -q cmscaf SiStripDQMOfflineCAF.job')
  os.chdir(currentDir)

if actionLevel == 'submit':
  sleep(5)
  os.system('bjobs -q cmscaf')
