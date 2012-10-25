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
#histMax = 5. # width of pull distributions in sigma
#histMax = 8. # width of pull distributions in sigma
histMax = 10. # width of pull distributions in sigma

# Fitting
doFit          = True
fitMaxPt       = 999999.
#fitMaxPt       = 200.
fitNonRestr    = False
fitEtaBins     = False

# Transfer functions
norm  = 0
#minPt = 0.
#minPt = 20.
minPt = 50.
#maxDR = 999999.
maxDR = 0.1
#maxDR = 0.2

# Transfer pulls
#fitRange = 1. # for Gaussian fits (in sigma)
#fitRange = 2. # for Gaussian fits (in sigma)
fitRange = histMax # for Gaussian fits (in sigma)

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
outputFile = inputFile.replace( 'fitTopHitFit', 'fitTopTransferPulls' )
logFile   = logFile.replace( 'fitTopHitFit', 'fitTopTransferPulls' )
cfgFile   = logFile.replace( '.', '_cfg.', 1 )
cfgFile   = cfgFile.replace( '.log', '.py' )
logFile   = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), logFile )
cfgFile   = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), cfgFile )
pathPlots = '%s/output/plots/fitTopTransferPulls/fitTopTransferPulls_from%s_%s_'%( os.getenv( "CMSSW_BASE" ), era, sample )
if refSel:
  pathPlots += 'Ref_'
if runTest:
  #pathPlots = ''
  pathPlots = pathPlots.replace( 'fitTopTransferPulls', 'fitTopTransferPullsTest', 2 )
  pathPlots = pathPlots.replace( 'fitTopTransferPullsTest', 'fitTopTransferPulls', 1 )


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
  PullPtBins = cms.uint32( 50 )
, PullPtMax  = cms.double( histMax )
)

process.fit = cms.PSet(
  fitNonRestr  = cms.bool( fitNonRestr )
, fitEtaBins   = cms.bool( fitEtaBins )
)
#if runTest:
  #process.fit.fitNonRestr = False
  #process.fit.fitEtaBins  = False

# Parameters for the transfer functions need to beidentical to those used for the determination
# FIXME: put this into a common place!!!!
process.transfer = cms.PSet(
  fitMaxPt    = cms.double( fitMaxPt )
# Fit function: a Gaussian is always required for the first three function parameters
#, fitFunction = cms.string( '[0]*exp(-0.5*((x-[1])/[2])**2)/([2]*sqrt(2*pi))' ) # single Gaussian
, fitFunction = cms.string( '[0]*(exp(-0.5*((x-[1])/[2])**2) + [3]*exp(-0.5*((x-[4])/[5])**2))/(([2]+[3]*[5])*sqrt(2*pi))' ) # double Gaussian
, norm        = cms.int32( norm )
, dependencyFunction = cms.string( '[0]+[1]*x' )
, minPt  = cms.double( minPt )
, maxDR  = cms.double( maxDR )
)

process.transferPull = cms.PSet(
  doFit       = cms.bool( doFit )
, fitOptions  = cms.string( 'IBRS+' )
, fitRange    = cms.double( fitRange )
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
