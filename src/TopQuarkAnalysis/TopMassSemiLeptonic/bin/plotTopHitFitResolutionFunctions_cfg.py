import os

import FWCore.ParameterSet.Config as cms

# Steering

runTest = True

# Origin of existing resolution functions
# era = 'Spring10'
era    = 'Summer11'
sample = 'Fall11_R4_1'

# Settings
overwrite = True # to throw away earlier versions of histograms, trees and functions
# Exclusive switches:
usePileUp = False
useAlt    = False
useSymm   = True
refGen    = False
refSel    = True
if runTest:
  refSel = False

accuEvery = 5

# I/O
inputFile = 'fitTopHitFit_from%s_%s.root'%( era, sample )
if usePileUp:
  inputFile = inputFile.replace( '.root', '_PileUp.root' )
logFile = inputFile.replace( 'root', 'log' )
if not runTest:
  if useAlt:
    logFile = logFile.replace( '.', '_Alt.', 1 )
  if useSymm:
    logFile = logFile.replace( '.', '_Symm.', 1 )
  if refGen:
    logFile = logFile.replace( '.', '_Gen.', 1 )
  if refSel:
    logFile = logFile.replace( '.', '_Ref.', 1 )
inputFile  = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), inputFile )
logFile    = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), logFile )
logFile    = logFile.replace( 'fitTopHitFit', 'plotTopHitFitResolutionFunctions' )
outputFile = logFile.replace( 'log', 'root' )
logFile    = logFile.replace( 'file:', '' )
plotPath  = '%s/output/plots'%( os.getenv( "CMSSW_BASE" ) )


# Processing

process = cms.PSet()
process.verbose = cms.bool( False )
if runTest:
  process.verbose = True
process.objectCategories = cms.vstring( 'Mu'
                                      )
if not runTest:
  process.objectCategories.append( 'Elec' )
  process.objectCategories.append( 'UdscJet' )
  process.objectCategories.append( 'BJet' )
  process.objectCategories.append( 'MET' )
process.overwrite = cms.bool( overwrite )
process.usePileUp = cms.bool( usePileUp )
process.useAlt    = cms.bool( useAlt )
process.useSymm   = cms.bool( useSymm )
process.refGen    = cms.bool( refGen )
process.refSel    = cms.bool( refSel )

process.io = cms.PSet(
  inputFiles     = cms.vstring( inputFile )
, sample         = cms.string( sample )
, outputFile     = cms.string( outputFile )
, plotPath       = cms.string( plotPath )
, resolutionFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era ) )
)

process.plot = cms.PSet(
  onlyExisting = cms.bool( True ) # True includes the possibility of writing resolution function text files.
, writeFiles   = cms.bool( True ) # True takes effect only, if "onlyExisting" is True, too.
, accuEvery    = cms.uint32( accuEvery )
)


# Messaging

print
print 'Input files:'
print '------------'
for inputFile in process.io.inputFiles:
  print inputFile
if overwrite:
  print
  print 'WARNING: existing resolution function plots will be overwritten!'
else:
  print
  print 'INFO: existing resolution function plots will be kept.'
print
print 'ROOT output file destination:'
print '---------------------'
print outputFile
print
print 'Log file destination:'
print '---------------------'
print logFile
print '================================================================================'
print
