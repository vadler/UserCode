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
overwrite = True # to throw away earlier versions of histograms, trees and functions
# Exclusive switches:
usePileUp = False
useAlt    = True # E instead of p
useNonT   = False
useSymm   = True
refGenJet = False
refSel    = True

pileUp = 'PileUpWeightTrue' # 'PileUpWeightTrue' or 'PileUpWeightObserved'

# Histograms
widthFactor = 5. # for rebinning (in units of orig. RMS)
#widthFactor = 8. # for rebinning (in units of orig. RMS)

# Fitting
doFit          = True
fitNonRestr    = True
fitEtaBins     = True
#minPtGenJet    = 0.
#minPtGenJet    = 20.
minPtGenJet    = 50.
#maxDRGenJet    = 999999.
maxDRGenJet    = 0.1
#maxDRGenJet    = 0.2
if runTest:
  fitNonRestr = False
  fitEtaBins  = False

# L5 functions
# Fit function: a Gaussian is always required for the first three function parameters
#fitFunction = 'gaus' # single Gaussian
#fitFunction = 'gaus + gaus(3)' # double Gaussian
fitFunction = '[0]*exp(-0.5*((x-[1])/[2])**2) + [4]*exp(-0.5*((log(x)-[3])/[5])**2)/x' # single ROOT-like Gaussian plus ROOT-like log-normal
#fitFunction = '[0]*exp(-0.5*((x-[1])/[2])**2)/([2]*sqrt(2*pi)) + [4]*exp(-0.5*((log(x)-[3])/[5])**2)/(x*[5]*sqrt(2*pi))' # single Gaussian plus log-normal
bkgFunction = '0'
fitOptions  = 'IBRS+'
#fitRange    = 1. # for Gaussian fits (in units of orig. RMS)
fitRange    = 2. # for Gaussian fits (in units of orig. RMS)
#fitRange    = 5. # for Gaussian fits (in units of orig. RMS)
#fitRange    = 8. # for Gaussian fits (in units of orig. RMS)
if len( fitFunction.split( ' + ' ) ) > 1: # a background function is defined
  fitRange    = widthFactor # for combined fits (in units of orig. RMS)
  bkgFunction = fitFunction.split( ' + ' )[0]
### L1FastJet {1 JetEta 3 JetPt JetA Rho 1-(z-[1])*y*[0]*[2]/x Correction L1FastJet}
### L2Relative {1 JetEta 1 JetPt [1]+[0]*log10(x)+[2]*pow(log10(x),2)+[3]*pow(log10(x),3)+[4]*pow(x/500.0,3) Correction L2Relative}
### L3Absolute {1 JetEta 1 JetPt [1]+[0]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5])) Correction L3Absolute}
### L2L3Residual { 1 JetEta 1 JetPt [1] Correction L2Relative}
### L5Flavor {1  JetEta  1   JetPt  [1] Correction L5Flavor}
jecVars = [ 'JetEta' ]
jecDims = [ 'JetPt' ]
jecFunction = '[0]'
### L7Parton {1  JetEta  2  JetPt  JetEta  1/([1]*x+[0])+[2]+([3]+[4]*log(x)+[5]*log(x)*log(x))*abs(y)+([6]+[7]*x)*y*y Response L7Parton}

# I/O
name = ''
if useAlt:
  name += 'Alt'
if refGenJet:
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
  logFile = logFile.replace( '.', '_P_.', 1 )
else:
  logFile = logFile.replace( '.', '_Pt_.', 1 )
logFile = logFile.replace( '_.', '_' + name + '.', 1 )
logFile = logFile.replace( '_.', '.', 1 )
inputFile = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), inputFile )
outputFile = inputFile.replace( 'fitTopHitFit', 'fitTopL5' )
logFile   = logFile.replace( 'fitTopHitFit', 'fitTopL5' )
cfgFile   = logFile.replace( '.', '_cfg.', 1 )
cfgFile   = cfgFile.replace( '.log', '.py' )
logFile   = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), logFile )
cfgFile   = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), cfgFile )
pathPlots = '%s/output/plots/fitTopL5/fitTopL5_from%s_%s_'%( os.getenv( "CMSSW_BASE" ), era, sample )
if refSel:
  pathPlots += 'Ref_'
if runTest:
  pathPlots = ''
  #pathPlots = pathPlots.replace( 'fitTopL5', 'fitTopL5Test', 2 )


# Processing

process = cms.PSet()
process.verbose = cms.uint32( 1 )
if runTest:
  process.verbose = 3
process.objectCategories = cms.vstring( 'UdscJet'
                                      , 'BJet'
                                      )
process.usePileUp = cms.bool( usePileUp )
process.useAlt    = cms.bool( useAlt )
process.useNonT   = cms.bool( useNonT )
process.useSymm   = cms.bool( useSymm )
process.refGenJet = cms.bool( refGenJet )
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
  FracPtBins = cms.uint32( 50 )
, FracPtMax  = cms.double(  2. )
  # Rebinning
, widthFactor = cms.double( widthFactor )
)

process.fit = cms.PSet(
  fitNonRestr  = cms.bool( fitNonRestr )
, fitEtaBins   = cms.bool( fitEtaBins )
, minPtGenJet  = cms.double( minPtGenJet )
, maxDRGenJet  = cms.double( maxDRGenJet )
)

process.jecL5 = cms.PSet(
  doFit        = cms.bool( doFit )
, fitFunction  = cms.string( fitFunction )
, bkgFunction  = cms.string( bkgFunction )
, fitOptions   = cms.string( fitOptions )
, fitRange     = cms.double( fitRange )
, jecVars      = cms.vstring( jecVars )
, jecDims      = cms.vstring( jecDims )
, jecFunction  = cms.string( jecFunction )
, writeFiles   = cms.bool( True )
, pathOut      = cms.string( '%s/src/TopQuarkAnalysis/TopMassSemiLeptonic/data/jecL5_from%s'%( os.getenv( "CMSSW_BASE" ), era ) ) # path to write the transfer functions
)
if runTest:
  process.jecL5.writeFiles = False


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
