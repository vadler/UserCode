import os

import FWCore.ParameterSet.Config as cms

# Steering

# Origin of existing resolution functions
# era    = 'Spring10'
era    = 'Summer11'
# Input sample
#sample = 'Fall11_R4_1_L3_unambiguousOnly'
#sample = 'Fall11_R4_1_L3_totalMinDist'
sample = 'Fall11_R4_L3_unambiguousOnly'

# Objects
objects = []
objects.append( 'UdscJet' ) # default: dGauss+linear
objects.append( 'BJet' )    # default: dGauss+linear
#objects.append( 'Mu' )      # default: sGauss+squared
#objects.append( 'Elec' )    # default: sGauss+squared

# Settings
overwrite  = True # to throw away earlier versions of histograms, trees and functions
plot       = True
writeFiles = True
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
fitEtaBins     = False
minPt    = 0.
#minPt    = 20.
#minPt    = 27. # Mu
#minPt    = 40. # Elec
#minPt    = 50. # UdscJet
#minPt    = 60. # BJet
#maxDR    = 999999.
maxDR    = 0.1
#maxDR    = 0.2

# Transfer functions
# Fit function
fitFunction        = 'dGauss' # 'sGauss', 'dGauss', 'lCB', 'uCB'
dependencyFunction = 'linear' # 'linear', 'squared', 'resolution'
norm        = 0 # index of parameter holding the normalisation of the fit functikon
fitOptions  = 'IBRS+'
fitRange    = 1. # for Gaussian fits (in units of orig. RMS)
#fitRange    = 2. # for Gaussian fits (in units of orig. RMS)
if fitFunction == 'dGauss': # a background function is defined
  fitRange = widthFactor # for combined fits (in units of orig. RMS)

# I/O
name = ''
if useAlt:
  name += 'Alt'
if refGen:
  name += 'Gen'
if useSymm:
  name += 'Symm'
inputFile = 'analyzeHitFit_from%s_%s.root'%( era, sample )
logFile    = inputFile.replace( 'analyzeHitFit', 'fitTopTransferFunctions' )
outputFile = logFile
logFile    = logFile.replace( 'root', 'log' )
if usePileUp:
  outputFile = outputFile.replace( '.root', '_PileUp.root' )
  logFile    = logFile.replace( '.log', '_PileUp.log' )
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
cfgFile    = logFile.replace( '.log', '_cfg.py' )
inputFile  = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), inputFile )
outputFile = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), outputFile )
logFile    = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), logFile )
cfgFile    = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), cfgFile )
pathPlots = ''
if plot:
  pathPlots = '%s/output/plots/fitTopTransferFunctions/fitTopTransferFunctions_from%s_%s_'%( os.getenv( "CMSSW_BASE" ), era, sample )
  if refSel:
    pathPlots += 'Ref_'
pathOut = ''
if writeFiles:
  pathOut = '%s/src/TopQuarkAnalysis/TopMassSemiLeptonic/data/transfer_from%s'%( os.getenv( "CMSSW_BASE" ), era ) # path to write the transfer functions


# Processing

process = cms.PSet()
process.verbose = cms.uint32( 1 )
process.objectCategories = cms.vstring( objects )
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

process.transfer = cms.PSet(
  doFit       = cms.bool( doFit )
, scale       = cms.bool( scale )
, fitMaxPt    = cms.double( fitMaxPt )
, fitFunction = cms.string( fitFunction )
, norm        = cms.int32( norm )
, fitOptions  = cms.string( fitOptions )
, fitRange    = cms.double( fitRange )
, dependencyFunction = cms.string( dependencyFunction )
, pathOut     = cms.string( pathOut )
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
print '----------'
print inputFile
print
print 'Output file:'
print '-----------'
print outputFile
if writeFiles:
  pathOut = process.transfer.pathOut.value() + '/gentTransferFunction_' + sample + '_*' + name + '*.txt'
  if refSel:
    pathOut = pathOut.replace( sample, sample + '_Ref', 1)
  print
  print 'Output destination:'
  print '------------------'
  print pathOut
if plot:
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
