import os

import FWCore.ParameterSet.Config as cms

# Steering

runTest   = True
rfioInput = True

# Origin of existing resolution functions
# era = 'Spring10'
era = 'Summer11'

# Settings
overwrite = True # to throw away earlier versions of histograms, trees and functions
# !!! Exclusive switches:
usePileUp = False
useAlt    = False
useSymm   = True
refGen    = False
refSel    = False
if runTest:
  refSel = False

pileUp = 'PileUpWeightTrue' # 'PileUpWeightTrue' or 'PileUpWeightObserved'

fitOptions  = 'RS'
widthFactor = 5. # for rebinning     (in units of orig. RMS)

fitResFuncs             = False
fitOptionsSigmaResFuncs = 'MRS'
fitRangeResFuncs        = 2. # for Gaussian fits (in units of orig. RMS)

fitJecsL5L7        = False
fitJecsL5L7Around1 = True
fitRangeJecsL5L7   = 1.5 # for Gaussian fits (in units of orig. RMS)

fitTransfer1D          = True
fitFromStartTransfer1D = True
fitRangeTransfer1D     = 5. # for Gaussian fits (in units of orig. RMS)

fitTransfer2D          = True
fitFromStartTransfer2D = True
widthFactorTransfer2D = 10. # for rebinning     (in units of orig. RMS)

inputFile = 'file:%s/output/fitTopHitFit_from%s.root'%( os.getenv( "CMSSW_BASE" ), era )
if runTest:
  inputFile = inputFile.replace( 'root', 'test.root' )
if not rfioInput:
  inputFile = inputFile.replace( 'root', 'local.root' )
logFile = inputFile.replace( 'root', 'log' )


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
, resolutionFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era ) )
)

process.histos = cms.PSet(
  # Muons
  MuDeltaPtBins = cms.uint32( 50 )
, MuDeltaPtMax  = cms.double( 12.5 )
, MuDeltaPtInvBins = cms.uint32( 50 )
, MuDeltaPtInvMax  = cms.double( 0.0025 )
, MuDeltaEtaBins = cms.uint32( 50 )
, MuDeltaEtaMax  = cms.double( 0.02 )
, MuDeltaEtaInvBins = cms.uint32( 50 )
, MuDeltaEtaInvMax  = cms.double( 0.02 )
, MuDeltaPhiBins = cms.uint32( 50 )
, MuDeltaPhiMax  = cms.double( 0.002 )
, MuDeltaPhiInvBins = cms.uint32( 50 )
, MuDeltaPhiInvMax  = cms.double( 0.002 )
  # Electrons
, ElecDeltaPtBins = cms.uint32( 50 )
, ElecDeltaPtMax  = cms.double( 12.5 )
, ElecDeltaPtInvBins = cms.uint32( 50 )
, ElecDeltaPtInvMax  = cms.double( 0.0025 )
, ElecDeltaEtaBins = cms.uint32( 50 )
, ElecDeltaEtaMax  = cms.double( 0.02 )
, ElecDeltaEtaInvBins = cms.uint32( 50 )
, ElecDeltaEtaInvMax  = cms.double( 0.02 )
, ElecDeltaPhiBins = cms.uint32( 50 )
, ElecDeltaPhiMax  = cms.double( 0.002 )
, ElecDeltaPhiInvBins = cms.uint32( 50 )
, ElecDeltaPhiInvMax  = cms.double( 0.002 )
  # Light jets
, UdscJetDeltaPtBins = cms.uint32( 50 )
, UdscJetDeltaPtMax  = cms.double( 50. )
, UdscJetDeltaPtInvBins = cms.uint32( 50 )
, UdscJetDeltaPtInvMax  = cms.double( 0.02 )
, UdscJetDeltaEtaBins = cms.uint32( 50 )
, UdscJetDeltaEtaMax  = cms.double( 0.5 )
, UdscJetDeltaEtaInvBins = cms.uint32( 50 )
, UdscJetDeltaEtaInvMax  = cms.double( 0.5 )
, UdscJetDeltaPhiBins = cms.uint32( 50 )
, UdscJetDeltaPhiMax  = cms.double( 0.5 )
, UdscJetDeltaPhiInvBins = cms.uint32( 50 )
, UdscJetDeltaPhiInvMax  = cms.double( 0.5 )
  # B-jets
, BJetDeltaPtBins = cms.uint32( 50 )
, BJetDeltaPtMax  = cms.double( 50. )
, BJetDeltaPtInvBins = cms.uint32( 50 )
, BJetDeltaPtInvMax  = cms.double( 0.02 )
, BJetDeltaEtaBins = cms.uint32( 50 )
, BJetDeltaEtaMax  = cms.double( 0.5 )
, BJetDeltaEtaInvBins = cms.uint32( 50 )
, BJetDeltaEtaInvMax  = cms.double( 0.5 )
, BJetDeltaPhiBins = cms.uint32( 50 )
, BJetDeltaPhiMax  = cms.double( 0.5 )
, BJetDeltaPhiInvBins = cms.uint32( 50 )
, BJetDeltaPhiInvMax  = cms.double( 0.5 )
  # MET
, METDeltaPtBins = cms.uint32( 50 )
, METDeltaPtMax  = cms.double( 100. )
, METDeltaPtInvBins = cms.uint32( 50 )
, METDeltaPtInvMax  = cms.double( 0.025 )
, METDeltaEtaBins = cms.uint32( 50 )
, METDeltaEtaMax  = cms.double( 10. )
, METDeltaEtaInvBins = cms.uint32( 50 )
, METDeltaEtaInvMax  = cms.double( 10. )
, METDeltaPhiBins = cms.uint32( 50 )
, METDeltaPhiMax  = cms.double( 1.6 )
, METDeltaPhiInvBins = cms.uint32( 50 )
, METDeltaPhiInvMax  = cms.double( 1.6 )
  # Fitting (general)
, fitOptions  = cms.string( fitOptions )
  # Rebinning
, widthFactor = cms.double( widthFactor )
)

process.resFuncs = cms.PSet(
  fit             = cms.bool( fitResFuncs )
, fitFunction     = cms.string( 'gaus' )
, fitOptionsSigma = cms.string( fitOptionsSigmaResFuncs )
, fitRange        = cms.double( fitRangeResFuncs )
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

if runTest:
  process.resFuncs.writeFiles = False

process.jecsL5L7 = cms.PSet(
  fit        = cms.bool( fitJecsL5L7 )
, fitFunction = cms.string( 'gaus' )
, fitRange   = cms.double( fitRangeJecsL5L7 )
, fitAround1 = cms.bool( fitJecsL5L7Around1 )
, writeFiles = cms.bool( True )
, pathOut    = cms.string( '%s/src/TopQuarkAnalysis/TopMassSemiLeptonic/data/jecL5L7_from%s'%( os.getenv( "CMSSW_BASE" ), era ) ) # path to write the transfer functions
)

#if runTest:
  #process.jecsL5L7.writeFiles = False

process.transfer = cms.PSet(
  transferFunction = cms.string( 'gaus' )
)

process.transfer1D = cms.PSet(
  fit          = cms.bool( fitTransfer1D )
, fitFromStart = cms.bool( fitFromStartTransfer1D )
, fitRange     = cms.double( fitRangeTransfer1D )
  # transfer function formulas
#, transferFunction = cms.string( '[0]*(exp(-0.5*((x-[1])/[2])**2)+[3]*exp(-0.5*((x-[4])/[5])**2))/(sqrt(2*pi)*([2]+[3]*[5])))' )
, transferFunction = cms.string( 'gaus(0)+gaus(3)' )
#, transferFunction = cms.string( 'gaus' )
, writeFiles = cms.bool( True )
, pathOut    = cms.string( '%s/src/TopQuarkAnalysis/TopMassSemiLeptonic/data/transfer1D_from%s'%( os.getenv( "CMSSW_BASE" ), era ) ) # path to write the transfer functions
)

#if runTest:
  #process.transfer1D.writeFiles = False

process.transfer2D = cms.PSet(
  fit          = cms.bool( fitTransfer2D )
, fitFromStart = cms.bool( fitFromStartTransfer2D )
, widthFactor  = cms.double( widthFactorTransfer2D )
  # transfer function formulas
#, transferFunction = cms.string( '[0]*(exp(-0.5*((y-([1]+[6]*x))/([2]+[7]*x))**2)+([3]+[8]*x)*exp(-0.5*((y-([4]+[9]*x))/([5]+[10]*x))**2))/(sqrt(2*pi)*(([2]+[7]*x)+([3]+[8]*x)*([5]+[10]*x))))' )
, transferFunction = cms.string( '[0]*(exp(-0.5*((y-([1]+[6]*x))/([2]+[7]*x))**2))/(sqrt(2*pi)*([2]+[7]*x))' )
, writeFiles = cms.bool( True )
, pathOut    = cms.string( '%s/src/TopQuarkAnalysis/TopMassSemiLeptonic/data/transfer2D_from%s'%( os.getenv( "CMSSW_BASE" ), era ) ) # path to write the transfer functions
)

#if runTest:
  #process.transfer2D.writeFiles = False


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
