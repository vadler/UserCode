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
  print '  - action level (\'create\', \'submit\')'
  print '  - run number'
  print '  - split level'
  print '  - apply HLT filters? (\'True\'/\'False\')'
  sys.exit(1)

if len(sys.argv) < 4:
  WrongUsage()

argAction    =     sys.argv[1]
argRunNumber =     sys.argv[2]
argSplit     = int(sys.argv[3])
argFiltersOn = False
if len(sys.argv) >= 5:
  if sys.argv[4] == '1' or sys.argv[4] == 'True' or  sys.argv[4] == 'true':
    argFiltersOn = True

if argAction != 'submit' and argAction != 'create':
  WrongUsage()

# shell = os.path.basename(os.getenv('SHELL'))
# shellType = 'sh'
# if (shell.endswith('csh')):
#   shellType = 'csh'
# command = 'source ' + os.getenv('VO_CMS_SW_DIR') + '/cmsset_default.' + shellType + '; cmsenv'  
# os.system(command) # still doesn't work
# pathCmsswBase = os.getenv('CMSSW_BASE')
pathCurrentDir        = os.getcwd()
pathCurrentDirStrings = string.split(pathCurrentDir, '/src/') # assumes to be in _the_ release working area
pathCmsswBase         = pathCurrentDirStrings[0]                    # assumes to be in _the_ release working area 
pathCmsswPackage      = os.path.split(pathCurrentDirStrings[1])[0]  # works only one level below the package
  
nDigits = 1
if int(argRunNumber) >= 10:
  nDigits = int(math.log10(int(argRunNumber))) + 1
nameRun = 'R'
for iJob in range(9-nDigits):
  nameRun += '0'
nameRun += argRunNumber

if os.path.exists(nameRun):
  for root, dirs, files in os.walk(nameRun, topdown = False):
    for name in files:
      os.remove(os.path.join(root, name))
    for name in dirs:
      os.rmdir(os.path.join(root, name))
  os.rmdir(nameRun)
os.mkdir(nameRun)

nInputFiles       = 0
fileInputFilesCff = file(nameRun + '/' + nameRun + '.cff','w')
dbsParams    = urllib.urlencode({'dbsInst': 'cms_dbs_prod_global', 'blockName': '*', 'dataset': '/Cosmics/CRUZET3_CRUZET3_V2P_v3/RECO', 'userMode': 'user', 'run': argRunNumber, 'what': 'cff'})
dbsOutput    = urllib.urlopen("https://cmsweb.cern.ch/dbs_discovery/getLFN_txt", dbsParams)
pathDbsStore = '/store/data/CRUZET3/Cosmics/RECO/CRUZET3_V2P_v3' # --> configurable (s)
for iLine in dbsOutput.readlines():
  if iLine.find(pathDbsStore) > -1:
    nInputFiles += 1
    fileInputFilesCff.write(iLine)
fileInputFilesCff.close()
if nInputFiles <= argSplit:
  argSplit = nInputFiles
if argSplit == 0:
  argSplit = 1
inputFilesJob = int(nInputFiles/argSplit) + 1
if nInputFiles%argSplit == 0:
  inputFilesJob -= 1
if nInputFiles == inputFilesJob*(argSplit-1) and argSplit > 1:
  argSplit -= 1
print '> submitDQMOfflineCAF.py > input files for run ' + argRunNumber + ': ' + str(nInputFiles)
  
nLinesRead = 0
fileInputFilesCff = file(nameRun + '/' + nameRun + '.cff', 'r')
linesInput = fileInputFilesCff.readlines()
nameMergeScript = 'merge' + nameRun + '.job'
fileMergeScript = file(nameRun + '/' + nameMergeScript, 'w')
fileMergeScript.write('hadd -f ' + os.getenv('HOME') + '/scratch0/cms/SiStripDQM/CMSSW_2_0_10/output/DQM_SiStrip' + nameRun + '_CAF-standAlone.root \\\n') # --> configurable
for iJob in range(argSplit):
  nDigits = 1
  if iJob >= 10:
    nDigits = int(math.log10(iJob)) + 1
  nameJob = nameRun + "_"
  for j in range(4-nDigits):
    nameJob += '0'
  nameJob += str(iJob)
  pathJobDir   = nameRun + "/" + nameJob
  os.mkdir(pathJobDir)
  os.chdir(pathJobDir)
  nameInputFilesJobCff = 'inputFiles.cff'
  if argFiltersOn:
    os.system('sed -e \"s#HLT_FILTER#    hltFilter,#g\" -e \"s#INCLUDE_DIRECTORY#' + pathCurrentDirStrings[1] + '/' + pathJobDir + '#g\" -e \"s#INPUT_FILES#' + pathCurrentDirStrings[1] + '/' + pathJobDir + '/' + nameInputFilesJobCff + '#g\" ' + pathCmsswBase + '/src/DQM/SiStripMonitorClient/test/SiStripDQMOfflineGlobalRunCAF_template.cfg > SiStripDQMOfflineGlobalRunCAF.cfg')
  else:
    os.system('sed -e \"s#HLT_FILTER#//     hltFilter,#g\" -e \"s#INCLUDE_DIRECTORY#' + pathCurrentDirStrings[1] + '/' + pathJobDir + '#g\" -e \"s#INPUT_FILES#' + pathCurrentDirStrings[1] + '/' + pathJobDir + '/' + nameInputFilesJobCff + '#g\" ' + pathCmsswBase + '/src/DQM/SiStripMonitorClient/test/SiStripDQMOfflineGlobalRunCAF_template.cfg > SiStripDQMOfflineGlobalRunCAF.cfg')
  fileInputFilesJobCff = file(nameInputFilesJobCff, 'w')
  fileInputFilesJobCff.write('  source = PoolSource {\n    untracked vstring fileNames = {\n')
  for j in range(nLinesRead, min(nLinesRead+inputFilesJob, nInputFiles)):
    line1  = string.split('      \'/store/data/' + string.split(linesInput[j], '/store/data/')[1], '.root\',')[0] + '.root\',\n'
    line2  = line1
    if (j+1)%inputFilesJob == 0 or nLinesRead == nInputFiles-1:
      line2 = string.split(line1, ',')[0] + '\n'
    fileInputFilesJobCff.write(line2)
    nLinesRead += 1
  fileInputFilesJobCff.write('    }\n  }\n')
  fileInputFilesJobCff.close()
  if nLinesRead == nInputFiles:
    fileMergeScript.write('rfio:' + os.getenv('CASTOR_HOME') + '/cms/SiStripDQM/OfflineDQM/CAF/DQM_SiStrip_' + nameJob + '.root\n')
  else:
    fileMergeScript.write('rfio:' + os.getenv('CASTOR_HOME') + '/cms/SiStripDQM/OfflineDQM/CAF/DQM_SiStrip_' + nameJob + '.root \\\n')
  os.system('sed -e \"s#OUTPUT_DIRECTORY#/tmp/' + os.getenv('USER') + '/' + pathJobDir + '#g\" ' + pathCmsswBase + '/src/DQM/SiStripMonitorClient/data/SiStripDQMOfflineGlobalRunCAF_template.cff > SiStripDQMOfflineGlobalRunCAF.cff')
  os.system('sed -e \"s#CMSSW_BASE#' + pathCmsswBase + '#g\" -e \"s#RUN_NAME#' + nameRun + '#g\" -e \"s#JOB_NAME#' + nameJob + '#g\" -e \"s#CURRENT_DIR#' + pathCurrentDir + '#g\" ' + pathCmsswBase + '/src/DQM/SiStripMonitorClient/scripts/SiStripDQMOfflineCAF_template.job > SiStripDQMOfflineCAF.job')
  os.chmod('SiStripDQMOfflineCAF.job',484)
  if argAction == 'submit':
    print '> submitDQMOfflineCAF.py >'
    print '  ' + os.getcwd() + ' : bsub -q cmscaf SiStripDQMOfflineCAF.job'
    os.system('bsub -q cmscaf SiStripDQMOfflineCAF.job')
  os.chdir(pathCurrentDir)
fileMergeScript.close()
os.chmod(nameRun + '/' + nameMergeScript,484)

if argAction == 'submit':
  time.sleep(5)
  os.system('bjobs -q cmscaf')
