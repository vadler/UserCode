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

# Settings
overwrite = False # to throw away earlier versions of histograms, trees and functions
# Exclusive switches:
usePileUp = False
useAlt    = True # E instead of p
useNonT   = True
useSymm   = True
refGen    = True
refSel    = True

pileUp = 'PileUpWeightTrue' # 'PileUpWeightTrue' or 'PileUpWeightObserved'

# Histograms
widthFactor = 5. # for rebinning (in units of orig. RMS)
#widthFactor = 8. # for rebinning (in units of orig. RMS)

# Fitting
doFit          = True
scale          = True
fitMaxPt       = 999999.
#fitMaxPt       = 200.
fitNonRestr    = False
fitEtaBins     = True
#minPt    = 0.
#minPt    = 20.
minPt    = 50.
#maxDR    = 999999.
maxDR    = 0.1
#maxDR    = 0.2

# Transfer functions
# Fit function: a Gaussian is always required for the first three function parameters
useBkg   = False
#fitFunction = '[0]*exp(-0.5*((x-[1])/[2])**2)/([2]*sqrt(2*pi))' # single Gaussian
fitFunction = '[0]*(exp(-0.5*((x-[1])/[2])**2) + [3]*exp(-0.5*((x-[4])/[5])**2))/(([2]+[3]*[5])*sqrt(2*pi))' # double Gaussian
norm           = 0
fitOptions  = 'IBRS+'
fitRange    = 1. # for Gaussian fits (in units of orig. RMS)
#fitRange    = 2. # for Gaussian fits (in units of orig. RMS)
if len( fitFunction.split( ' + ' ) ) > 1: # a background function is defined
  fitRange = widthFactor # for combined fits (in units of orig. RMS)
  useBkg   = True

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
  inputFile = inputFile.replace( '.root', '.test.root' )
logFile = inputFile.replace( 'root', 'log' )
if refSel:
  logFile = logFile.replace( '.', '_Ref.', 1 )
if useNonT:
  if useAlt:
    logFile = logFile.replace( '.', '_E_.', 1 )
  else:
    logFile = logFile.replace( '.', '_P_.', 1 )
else:
  if useAlt:
    logFile = logFile.replace( '.', '_Et_.', 1 )
  else:
    logFile = logFile.replace( '.', '_Pt_.', 1 )
logFile = logFile.replace( '_.', '_' + name + '.', 1 )
logFile = logFile.replace( '_.', '.', 1 )
inputFile = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), inputFile )
outputFile = inputFile.replace( 'fitTopHitFit', 'fitTopTransferFunctions' )
logFile   = logFile.replace( 'fitTopHitFit', 'fitTopTransferFunctions' )
cfgFile   = logFile.replace( '.', '_cfg.', 1 )
cfgFile   = cfgFile.replace( '.log', '.py' )
logFile   = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), logFile )
cfgFile   = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), cfgFile )
pathPlots = '%s/output/plots/fitTopTransferFunctions/fitTopTransferFunctions_from%s_%s_'%( os.getenv( "CMSSW_BASE" ), era, sample )
if refSel:
  pathPlots += 'Ref_'
if runTest:
  pathPlots = ''
  #pathPlots = pathPlots.replace( 'fitTopTransferFunctions', 'fitTopTransferFunctionsTest', 2 )
  #pathPlots = pathPlots.replace( 'fitTopTransferFunctionsTest', 'fitTopTransferFunctions', 1 )


# Processing

process = cms.PSet()
process.verbose = cms.uint32( 1 )
if runTest:
  process.verbose = 3
process.objectCategories = cms.vstring( 'UdscJet'
#process.objectCategories = cms.vstring( 'BJet'
#process.objectCategories = cms.vstring( 'Mu'
#process.objectCategories = cms.vstring( 'Elec'
                                      )
process.overwrite = cms.bool( overwrite )
process.usePileUp = cms.bool( usePileUp )
process.useAlt    = cms.bool( useAlt )
process.useNonT   = cms.bool( useNonT )
process.useSymm   = cms.bool( useSymm )
process.refGen    = cms.bool( refGen )
process.refSel    = cms.bool( refSel )

process.pileUp    = cms.string( pileUp )

process.io = cms.PSet(
  inputFile  = cms.string( inputFile )
, outputFile = cms.string( outputFile )
, overwrite  = cms.bool( overwrite )
, sample     = cms.string( sample )
, pathPlots  = cms.string( pathPlots )
)

process.histos = cms.PSet(
  # Binning
  DeltaPtBins = cms.uint32( 50 )
, DeltaPtMax  = cms.double( 50. )
  # Rebinning
, widthFactor = cms.double( widthFactor )
)

process.fit = cms.PSet(
  fitNonRestr  = cms.bool( fitNonRestr )
, fitEtaBins   = cms.bool( fitEtaBins )
, minPt  = cms.double( minPt )
, maxDR  = cms.double( maxDR )
)
#if runTest:
  #process.fit.fitNonRestr = False
  #process.fit.fitEtaBins  = False

process.transfer = cms.PSet(
  doFit       = cms.bool( doFit )
, scale       = cms.bool( scale )
, fitMaxPt    = cms.double( fitMaxPt )
, fitFunction = cms.string( fitFunction )
, norm        = cms.int32( norm )
, useBkg      = cms.bool( useBkg )
, fitOptions  = cms.string( fitOptions )
, fitRange    = cms.double( fitRange )
, dependencyFunction = cms.string( '[0]+[1]*x' )
, writeFiles  = cms.bool( True )
, pathOut     = cms.string( '%s/src/TopQuarkAnalysis/TopMassSemiLeptonic/data/transfer_from%s'%( os.getenv( "CMSSW_BASE" ), era ) ) # path to write the transfer functions
)
#if runTest:
  #process.transfer.writeFiles = False


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
print '----------'
print inputFile
print
print 'Output file:'
print '-----------'
print outputFile
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
