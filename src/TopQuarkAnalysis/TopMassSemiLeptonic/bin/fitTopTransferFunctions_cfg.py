import os

import FWCore.ParameterSet.Config as cms

# Steering

runTest = False

# Origin of existing resolution functions
# era    = 'Spring10'
era    = 'Summer11'
sample = 'Fall11_R4_1'

# Settings
overwrite = True # to throw away earlier versions of histograms, trees and functions
# Exclusive switches:
usePileUp = False
useAlt    = True # E instead of p
useNonT   = True
useSymm   = True
refGen    = True
refSel    = True
if runTest:
  refSel = False

pileUp = 'PileUpWeightTrue' # 'PileUpWeightTrue' or 'PileUpWeightObserved'

# Histograms
widthFactor = 5. # for rebinning (in units of orig. RMS)

# Fitting
fitNonRestr    = True
fitEtaBins     = True
#minPtParton    = 0.
#maxDRParton    = 999999.
minPtParton    = 20.
maxDRParton    = 0.2

# Transfer functions
fit1D      = True
fitMaxPt1D = 999.
#fitMaxPt1D = 200.
# Fit function: a Gaussian is always required for the first three function parameters
fitFunction1D = '[0]*exp(-0.5*((x-[1])/[2])**2)/([2]*sqrt(2*pi))' # single Gaussian
#fitFunction1D = '[0]*(exp(-0.5*((x-[1])/[2])**2) + [3]*exp(-0.5*((x-[4])/[5])**2))/(([2]+[3]*[5])*sqrt(2*pi))' # double Gaussian
fitOptions1D  = 'BRS+'
fitRange1D    = 2. # for Gaussian fits (in units of orig. RMS)
if len( fitFunction1D.split( ' + ' ) ) > 1: # a background function is defined
  fitRange1D = 5. # for combined fits (in units of orig. RMS)

# Transfer functions
fit2D      = False
fitMaxPt2D = 999.
#fitMaxPt2D = 200.
# Fit function: a Gaussian is always required for the first three function parameters
fitFunction2D = '[0]*exp(-0.5*((y-([1]+[2]*x))/([3]+[4]*x))**2)/(([3]+[4]*x)*sqrt(2*pi))' # single Gaussian
#fitFunction2D = '[0]*(exp(-0.5*((x-([1]+[2]*x))/([3]+[4]*x))**2) + ([5]+[6]*x)*exp(-0.5*((x-([7]+[8]*x))/([9]+[10]*x))**2))/((([3]+[4]*x)+([5]+[6]*x)*([9]+[10]*x))*sqrt(2*pi))' # double Gaussian
fitOptions2D  = 'BRS+'
fitRange2D    = 2. # for Gaussian fits (in units of orig. RMS)
if len( fitFunction1D.split( ' + ' ) ) > 1: # a background function is defined
  fitRange2D = 5. # for combined fits (in units of orig. RMS)

# I/O
inputFile = 'fitTopHitFit_from%s_%s.root'%( era, sample )
if usePileUp:
  inputFile = inputFile.replace( '.root', '_PileUp.root' )
logFile = inputFile.replace( 'root', 'log' )
if not runTest:
  if useAlt:
    logFile = logFile.replace( '.', '_Alt.', 1 )
  if useNonT:
    logFile = logFile.replace( '.', '_NonT.', 1 )
  if useSymm:
    logFile = logFile.replace( '.', '_Symm.', 1 )
  if refGen:
    logFile = logFile.replace( '.', '_Gen.', 1 )
  if refSel:
    logFile = logFile.replace( '.', '_Ref.', 1 )
inputFile = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), inputFile )
logFile   = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), logFile )
logFile   = logFile.replace( 'fitTopHitFit', 'fitTopTransferFunctions' )


# Processing

process = cms.PSet()
process.verbose = cms.uint32( 1 )
if runTest:
  process.verbose = 3
process.objectCategories = cms.vstring( 'UdscJet'
                                      )
if not runTest:
  process.objectCategories.append( 'BJet' )
process.overwrite = cms.bool( overwrite )
process.usePileUp = cms.bool( usePileUp )
process.useAlt    = cms.bool( useAlt )
process.useNonT   = cms.bool( useNonT )
process.useSymm   = cms.bool( useSymm )
process.refGen    = cms.bool( refGen )
process.refSel    = cms.bool( refSel )

process.pileUp    = cms.string( pileUp )

process.io = cms.PSet(
  inputFile = cms.string( inputFile )
, sample    = cms.string( sample )
)

process.histos = cms.PSet(
  # Light jets
  UdscJetPtBins = cms.uint32( 50 )
, UdscJetPtMax  = cms.double( 50. )
  # B-jets
, BJetPtBins = cms.uint32( 50 )
, BJetPtMax  = cms.double( 50. )
  # Rebinning
, widthFactor = cms.double( widthFactor )
)

process.fit = cms.PSet(
  fitNonRestr  = cms.bool( fitNonRestr )
, fitEtaBins     = cms.bool( fitEtaBins )
, minPtParton  = cms.double( minPtParton )
, maxDRParton  = cms.double( maxDRParton )
)

process.transfer1D = cms.PSet(
  fit         = cms.bool( fit1D )
, plot        = cms.bool( True )
, fitMaxPt    = cms.double( fitMaxPt1D )
, fitFunction = cms.string( fitFunction1D )
, fitOptions  = cms.string( fitOptions1D )
, fitRange    = cms.double( fitRange1D )
, fitTransfer = cms.string( '[0]+[1]*x' )
, writeFiles  = cms.bool( True )
, pathOut     = cms.string( '%s/src/TopQuarkAnalysis/TopMassSemiLeptonic/data/transfer1D_from%s'%( os.getenv( "CMSSW_BASE" ), era ) ) # path to write the transfer functions
)

if runTest:
  process.transfer1D.writeFiles = False
  process.transfer1D.plot       = False

process.transfer2D = cms.PSet(
  fit         = cms.bool( fit2D )
, plot        = cms.bool( True )
, fitMaxPt    = cms.double( fitMaxPt2D )
, fitFunction = cms.string( fitFunction2D )
, fitOptions  = cms.string( fitOptions2D )
, fitRange    = cms.double( fitRange2D )
, writeFiles  = cms.bool( True )
, pathOut     = cms.string( '%s/src/TopQuarkAnalysis/TopMassSemiLeptonic/data/transfer2D_from%s'%( os.getenv( "CMSSW_BASE" ), era ) ) # path to write the transfer functions
)

if runTest:
  process.transfer2D.writeFiles = False
  process.transfer2D.plot       = False


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
