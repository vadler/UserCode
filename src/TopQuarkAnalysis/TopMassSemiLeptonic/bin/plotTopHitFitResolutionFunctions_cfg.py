import os

import FWCore.ParameterSet.Config as cms

# Steering

runTest = True

# Origin of existing resolution functions
# era    = 'Spring10'
era    = 'Summer11'
# Input sample
sample = 'Fall11_R4_1_L3_unambiguousOnly'
#sample = 'Fall11_R4_1_L3_totalMinDist'
#sample = 'Fall11_R4_L3_unambiguousOnly'

# Settings
overwrite = True # to throw away earlier versions of histograms, trees and functions
# Exclusive switches:
usePileUp = False
useAlt    = False
useSymm   = True
refGen    = False
refSel    = True

accuEvery = 5

# I/O
name = ''
if useAlt:
  name += 'Alt'
if refGen:
  name += 'Gen'
if useSymm:
  name += 'Symm'
inputFile = 'fitTopHitFit_from%s_%s.root'%( era, sample )
if usePileUp:
  inputFile = inputFile.replace( '.root', '_PileUp.root' )
if runTest:
  inputFile = inputFile.replace( 'root', 'test.root' )
logFile = inputFile.replace( 'root', 'log' )
if refSel:
  logFile = logFile.replace( '.', '_Ref.', 1 )
logFile = logFile.replace( '.', '_' + name + '.', 1 )
inputFile = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), inputFile )
outputFile = inputFile.replace( 'fitTopHitFit', 'plotTopHitFitResolutionFunctions' )
logFile   = logFile.replace( 'fitTopHitFit', 'plotTopHitFitResolutionFunctions' )
cfgFile   = logFile.replace( '.', '_cfg.', 1 )
cfgFile   = cfgFile.replace( '.log', '.py' )
logFile   = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), logFile )
cfgFile   = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), cfgFile )
pathPlots = '%s/output/plots/fitTopHitFitResolutionFunctions/plotTopHitFitResolutionFunctions_from%s_%s_'%( os.getenv( "CMSSW_BASE" ), era, sample )
if refSel:
  pathPlots += 'Ref_'
if runTest:
  pathPlots = ''
  #pathPlots = pathPlots.replace( 'plotTopHitFitResolutionFunctions', 'plotTopHitFitResolutionFunctionsTest' )


# Processing

process = cms.PSet()
process.verbose = cms.bool( False )
if runTest:
  process.verbose = True
process.objectCategories = cms.vstring( 'Mu'
                                      , 'UdscJet'
                                      )
if not runTest:
  process.objectCategories.append( 'Elec' )
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
, pathPlots      = cms.string( pathPlots )
, resolutionFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era ) )
)

process.plot = cms.PSet(
  onlyExisting = cms.bool( True )
, accuEvery    = cms.uint32( accuEvery )
)


# Messaging

f = open( cfgFile, 'w' )
print >> f, process.dumpPython()
f.close()
print
print 'Config file:'
print '------------'
print cfgFile
print
print 'Input file:'
print '------------'
print inputFile
print
if pathPlots != '':
  print
  print 'Plots destination:'
  print '------------------'
  print pathPlots + '*' + name + '*.png'
print
print 'Log file destination:'
print '---------------------'
print logFile
print '================================================================================'
print
