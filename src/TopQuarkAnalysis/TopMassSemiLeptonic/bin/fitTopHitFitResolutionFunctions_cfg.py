import os

import FWCore.ParameterSet.Config as cms

# Steering

runTest   = False
rfioInput = True

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

pileUp = 'PileUpWeightTrue' # 'PileUpWeightTrue' or 'PileUpWeightObserved'

# Histograms
widthFactor = 5. # for rebinning (in units of orig. RMS)

# Fitting
fitOptions      = 'RS'
fitOptionsSigma = 'MERS'
fitRange        = 2. # for Gaussian fits (in units of orig. RMS)

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
if not rfioInput:
  inputFile = inputFile.replace( 'root', 'local.root' )
logFile = inputFile.replace( 'root', 'log' )
if refSel:
  logFile = logFile.replace( '.', '_Ref.', 1 )
logFile = logFile.replace( '.', '_' + name + '.', 1 )
inputFile = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), inputFile )
outputFile = inputFile.replace( 'fitTopHitFit', 'fitTopHitFitResolutionFunctions' )
logFile   = logFile.replace( 'fitTopHitFit', 'fitTopHitFitResolutionFunctions' )
cfgFile   = logFile.replace( '.', '_cfg.', 1 )
cfgFile   = cfgFile.replace( '.log', '.py' )
logFile   = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), logFile )
cfgFile   = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), cfgFile )
pathPlots = '%s/output/plots/fitTopHitFitResolutionFunctions/fitTopHitFitResolutionFunctions_from%s_%s_'%( os.getenv( "CMSSW_BASE" ), era, sample )
if refSel:
  pathPlots += 'Ref_'
if runTest:
  #pathPlots = ''
  pathPlots = pathPlots.replace( 'fitTopHitFitResolutionFunctions', 'fitTopHitFitResolutionFunctionsTest', 2 )
  pathPlots = pathPlots.replace( 'fitTopHitFitResolutionFunctionsTest', 'fitTopHitFitResolutionFunctions', 1 )


# Processing

process = cms.PSet()
process.verbose = cms.uint32( 1 )
if runTest:
  process.verbose = 3
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

process.pileUp    = cms.string( pileUp )

process.io = cms.PSet(
  inputFile      = cms.string( inputFile )
, sample         = cms.string( sample )
, pathPlots      = cms.string( pathPlots )
, resolutionFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era ) )
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

process.fit = cms.PSet(
  fitFunction     = cms.string( 'gaus' )
, fitOptions      = cms.string( fitOptions )
, fitOptionsSigma = cms.string( fitOptionsSigma )
, fitRange        = cms.double( fitRange )
  # resolution function formulas
, resolutionFunction        = cms.string( 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])' )
, resolutionFunctionInverse = cms.string( 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])' )
  # derived formulas
, resolutionFunctionRel           = cms.string( 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])/x' )
, resolutionFunctionInverseRel    = cms.string( 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])/x' )
, resolutionFunctionInverseInv    = cms.string( 'sqrt(([2]*[2]*x+[1]*[1])*x+[0]*[0])*x' )
, resolutionFunctionInverseInvRel = cms.string( 'sqrt(([2]*[2]*x+[1]*[1])*x+[0]*[0])' )
  # skip resolutions without existing counterpart?
, onlyExisting = cms.bool( True ) # True includes the possibility of writing resolution function text files.
, writeFiles   = cms.bool( True ) # True takes effect only, if "onlyExisting" is True, too.
, pathOut      = cms.string( '%s/src/TopQuarkAnalysis/TopHitFit/data/resolution_from%s'%( os.getenv( "CMSSW_BASE" ), era ) ) # path to write the resolution functions
)

#if runTest or not rfioInput:
if not rfioInput:
  process.fit.writeFiles = False


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
if process.fit.writeFiles:
  pathOut = process.fit.pathOut.value() + '/gentResolution_' + sample + '_*' + name + '*.txt'
  if refSel:
    pathOut = pathOut.replace( sample, sample + '_Ref', 1)
  print 'Output destination:'
  print '------------------'
  print pathOut
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
