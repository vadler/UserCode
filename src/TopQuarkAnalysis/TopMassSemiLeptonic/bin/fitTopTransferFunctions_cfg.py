import os

import FWCore.ParameterSet.Config as cms

# Steering

runTest   = True

# Origin of existing resolution functions
# era    = 'Spring10'
era    = 'Summer11'
sample = 'Fall11_R4_1'

# Settings
overwrite = True # to throw away earlier versions of histograms, trees and functions
# Exclusive switches:
usePileUp = False
useAlt    = False
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
fitEtaPt       = True
#minPtParton    = 0.
#maxDRParton    = 999999.
minPtParton    = 20.
maxDRParton    = 0.2
# Fit function: a Gaussian is always required for the first three function parameters
fitFunction = '[1]*exp(-0.5*((x-[0])/[2])**2)' # single ROOT-like Gaussian
##fitFunction = '[1]*exp(-0.5*((x-[0])/[2])**2)/([2]*sqrt(2*pi))' # single Gaussian
##fitFunction = '( [1]*exp(-0.5*((x-[0])/[2])**2) ) + ( [4]*exp(-0.5*((x-[0])/[5])**2) )' # double ROOT-like Gaussian with common mean
#fitFunction = '( [1]*exp(-0.5*((x-[0])/[2])**2) ) + ( [4]*exp(-0.5*((log(x)-[3])/[5])**2)/x )' # single ROOT-like Gaussian plus ROOT-like log-normal
##fitFunction = '( [1]*exp(-0.5*((x-[0])/[2])**2)/([2]*sqrt(2*pi)) ) + ( [4]*exp(-0.5*((log(x)-[3])/[5])**2)/(x*[5]*sqrt(2*pi)) )' # single Gaussian plus log-normal
fitOptions  = 'BRS+'
bkgFunction = '0'
fitRange    = 2. # for Gaussian fits (in units of orig. RMS)
if len( fitFunction.split( ' + ' ) ) > 1: # a background function is defined
  bkgFunction = fitFunction.split( ' + ' )[1]
  fitRange    = 5. # for combined fits (in units of orig. RMS)

# Transfer functions
fitTransfer = True

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
, fitEtaPt     = cms.bool( fitEtaPt )
, fitFunction  = cms.string( fitFunction )
, fitOptions   = cms.string( fitOptions )
, fitRange     = cms.double( fitRange )
, bkgFunction  = cms.string( bkgFunction )
, minPtParton  = cms.double( minPtParton )
, maxDRParton  = cms.double( maxDRParton )
)

process.transfer = cms.PSet(
  fit        = cms.bool( fitTransfer )
, writeFiles = cms.bool( True )
, pathOut    = cms.string( '%s/src/TopQuarkAnalysis/TopMassSemiLeptonic/data/transfer_from%s'%( os.getenv( "CMSSW_BASE" ), era ) ) # path to write the transfer functions
)

#if runTest:
  #process.transfer.writeFiles = False


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
