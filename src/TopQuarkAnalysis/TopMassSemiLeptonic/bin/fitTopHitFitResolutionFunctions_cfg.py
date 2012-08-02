import os

import FWCore.ParameterSet.Config as cms

# Steering

runTest   = False
rfioInput = True

# Origin of existing resolution functions
# era    = 'Spring10'
era    = 'Summer11'

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

pileUp = 'PileUpWeightTrue' # 'PileUpWeightTrue' or 'PileUpWeightObserved'

# Histograms
widthFactor = 5. # for rebinning (in units of orig. RMS)

# Fitting
fitOptions      = 'RS'
fitOptionsSigma = 'MERS'
fitRange        = 2. # for Gaussian fits (in units of orig. RMS)

# I/O
inputFile = 'fitTopHitFit_from%s.root'%( era )
sample    = 'Fall11_R4_1'
if not runTest and rfioInput:
  inputFile = inputFile.replace( '.root', '_%s.root'%( sample ) )
if usePileUp:
  inputFile = inputFile.replace( '.root', '_PileUp.root' )
if runTest:
  inputFile = inputFile.replace( 'root', 'test.root' )
if not rfioInput:
  inputFile = inputFile.replace( 'root', 'local.root' )
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
inputFile = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), inputFile )
logFile   = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), logFile )
logFile   = logFile.replace( 'fitTopHitFit', 'fitTopHitFitResolutionFunctions' )


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

if runTest or not rfioInput:
  process.fit.writeFiles = False


# Messaging

print
print 'Input file:'
print '------------'
print inputFile
print
print 'Log file destination:'
print '---------------------'
print logFile
print '================================================================================'
print
