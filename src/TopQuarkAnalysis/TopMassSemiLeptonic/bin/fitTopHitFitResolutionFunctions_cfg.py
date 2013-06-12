import os

import FWCore.ParameterSet.Config as cms

# Steering

# Origin of existing resolution functions
era    = 'Summer11'
# Input sample
sample = 'Summer12_MadGraph_L3_unambiguousOnly'
#sample = 'Summer12_MCatNLO_L3_unambiguousOnly'

# Objects
objects = []
objects.append( 'UdscJet' )
objects.append( 'BJet' )
objects.append( 'Mu' )
objects.append( 'Elec' )

# Settings
verbose    = 1
overwrite  = True # to throw away earlier versions of histograms, trees and functions
plot       = True
writeFiles = True
# Exclusive switches:
usePileUp = False
useAlt    = False
useNonT   = False
useSymm   = True
refGen    = False
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

# Resolution functions
# Fit function
fitFunction = 'gaus'
# Resolution function
resolutionFunction        = 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])'
resolutionFunctionInverse = 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])'
# derived formulas
resolutionFunctionRel           = 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])/x'
resolutionFunctionInverseRel    = 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])/x'
resolutionFunctionInverseInv    = 'sqrt(([2]*[2]*x+[1]*[1])*x+[0]*[0])*x'
resolutionFunctionInverseInvRel = 'sqrt(([2]*[2]*x+[1]*[1])*x+[0]*[0])'
norm            = 0 # index of parameter holding the normalisation of the fit functikon
fitOptions      = 'RS'
fitOptionsSigma = 'MERS'
#fitRange        = 1. # for Gaussian fits (in units of orig. RMS)
fitRange        = 2. # for Gaussian fits (in units of orig. RMS)

# I/O
resolutionFile = 'file:%s/output/existingHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era )
name = ''
if useAlt:
  name += 'Alt'
if refGen:
  name += 'Gen'
if useSymm:
  name += 'Symm'
inputFile = 'analyzeHitFit_from%s_%s.root'%( era, sample )
logFile    = inputFile.replace( 'analyzeHitFit', 'fitTopHitFitResolutionFunctions' )
outputFile = logFile
logFile    = logFile.replace( '.root', '.log' )
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
  pathPlots = '%s/output/plots/fitTopHitFitResolutionFunctions/fitTopHitFitResolutionFunctions_from%s_%s_'%( os.getenv( "CMSSW_BASE" ), era, sample )
  if refSel:
    pathPlots += 'Ref_'
pathOut = ''
if writeFiles:
  pathOut = '%s/src/TopQuarkAnalysis/TopHitFit/data/resolution_from%s'%( os.getenv( "CMSSW_BASE" ), era ) # path to write the transfer functions


# Processing

process = cms.PSet()
process.verbose = cms.uint32( verbose )
process.objectCategories = cms.vstring( objects )
process.usePileUp = cms.bool( usePileUp )
process.useAlt    = cms.bool( useAlt )
process.useSymm   = cms.bool( useSymm )
process.refGen    = cms.bool( refGen )
process.refSel    = cms.bool( refSel )

process.pileUp    = cms.string( pileUp )

process.io = cms.PSet(
  inputFile      = cms.string( inputFile )
, outputFile     = cms.string( outputFile )
, overwrite      = cms.bool( overwrite )
, sample         = cms.string( sample )
, pathPlots      = cms.string( pathPlots )
, resolutionFile = cms.string( resolutionFile )
)

process.histos = cms.PSet(
  # Muons
  MuPtBins = cms.uint32( 50 )
, MuPtMax  = cms.double( 12.5 )
, MuPtInvBins = cms.uint32( 50 )
, MuPtInvMax  = cms.double( 0.0025 )
, MuEtaBins = cms.uint32( 50 )
, MuEtaMax  = cms.double( 0.02 )
, MuEtaInvBins = cms.uint32( 50 )
, MuEtaInvMax  = cms.double( 0.02 )
, MuPhiBins = cms.uint32( 50 )
, MuPhiMax  = cms.double( 0.002 )
, MuPhiInvBins = cms.uint32( 50 )
, MuPhiInvMax  = cms.double( 0.002 )
  # Electrons
, ElecPtBins = cms.uint32( 50 )
, ElecPtMax  = cms.double( 12.5 )
, ElecPtInvBins = cms.uint32( 50 )
, ElecPtInvMax  = cms.double( 0.0025 )
, ElecEtaBins = cms.uint32( 50 )
, ElecEtaMax  = cms.double( 0.02 )
, ElecEtaInvBins = cms.uint32( 50 )
, ElecEtaInvMax  = cms.double( 0.02 )
, ElecPhiBins = cms.uint32( 50 )
, ElecPhiMax  = cms.double( 0.002 )
, ElecPhiInvBins = cms.uint32( 50 )
, ElecPhiInvMax  = cms.double( 0.002 )
  # Light jets
, UdscJetPtBins = cms.uint32( 50 )
, UdscJetPtMax  = cms.double( 50. )
, UdscJetPtInvBins = cms.uint32( 50 )
, UdscJetPtInvMax  = cms.double( 0.02 )
, UdscJetEtaBins = cms.uint32( 50 )
, UdscJetEtaMax  = cms.double( 0.5 )
, UdscJetEtaInvBins = cms.uint32( 50 )
, UdscJetEtaInvMax  = cms.double( 0.5 )
, UdscJetPhiBins = cms.uint32( 50 )
, UdscJetPhiMax  = cms.double( 0.5 )
, UdscJetPhiInvBins = cms.uint32( 50 )
, UdscJetPhiInvMax  = cms.double( 0.5 )
  # B-jets
, BJetPtBins = cms.uint32( 50 )
, BJetPtMax  = cms.double( 50. )
, BJetPtInvBins = cms.uint32( 50 )
, BJetPtInvMax  = cms.double( 0.02 )
, BJetEtaBins = cms.uint32( 50 )
, BJetEtaMax  = cms.double( 0.5 )
, BJetEtaInvBins = cms.uint32( 50 )
, BJetEtaInvMax  = cms.double( 0.5 )
, BJetPhiBins = cms.uint32( 50 )
, BJetPhiMax  = cms.double( 0.5 )
, BJetPhiInvBins = cms.uint32( 50 )
, BJetPhiInvMax  = cms.double( 0.5 )
  # MET
, METPtBins = cms.uint32( 50 )
, METPtMax  = cms.double( 100. )
, METPtInvBins = cms.uint32( 50 )
, METPtInvMax  = cms.double( 0.025 )
, METEtaBins = cms.uint32( 50 )
, METEtaMax  = cms.double( 10. )
, METEtaInvBins = cms.uint32( 50 )
, METEtaInvMax  = cms.double( 10. )
, METPhiBins = cms.uint32( 50 )
, METPhiMax  = cms.double( 1.6 )
, METPhiInvBins = cms.uint32( 50 )
, METPhiInvMax  = cms.double( 1.6 )
  # Rebinning
, widthFactor = cms.double( widthFactor )
)

process.resolution = cms.PSet(
  doFit           = cms.bool( doFit )
, scale           = cms.bool( scale )
, fitMaxPt        = cms.double( fitMaxPt )
, fitFunction     = cms.string( fitFunction )
, norm            = cms.int32( norm )
, fitOptions      = cms.string( fitOptions )
, fitOptionsSigma = cms.string( fitOptionsSigma )
, fitRange        = cms.double( fitRange )
  # resolution function formulas
, resolutionFunction        = cms.string( resolutionFunction )
, resolutionFunctionInverse = cms.string( resolutionFunctionInverse )
  # derived formulas
, resolutionFunctionRel           = cms.string( resolutionFunctionRel )
, resolutionFunctionInverseRel    = cms.string( resolutionFunctionInverseRel )
, resolutionFunctionInverseInv    = cms.string( resolutionFunctionInverseInv )
, resolutionFunctionInverseInvRel = cms.string( resolutionFunctionInverseInvRel )
  # skip resolutions without existing counterpart?
, onlyExisting = cms.bool( True ) # True includes the possibility of writing resolution function text files.
, pathOut      = cms.string( pathOut )
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
print 'Output file:'
print '-----------'
print outputFile
if writeFiles:
  pathOut = process.resolution.pathOut.value() + '/gentResolutionFunction_' + sample + '_*' + name + '*.txt'
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
