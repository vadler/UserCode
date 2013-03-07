import os

import FWCore.ParameterSet.Config as cms

# Steering

#Input file
inputFile = 'fitTopTransferFunctions_fromSummer11_Fall11_R4_L3_unambiguousOnly.test.root'

# Objects
objects = []
objects.append( 'UdscJet' ) # default: dGauss+linear
objects.append( 'BJet' )    # default: dGauss+linear
#objects.append( 'Mu' )      # default: sGauss+squared
#objects.append( 'Elec' )    # default: sGauss+squared

# Settings
overwrite  = True # to throw away earlier versions of histograms, trees and functions
plot       = True
writeFiles = False
# Exclusive switches:
useAlt    = True # E instead of p
useNonT   = True
useSymm   = True
refGen    = True
refSel    = True

# Fitting
fitNonRestr    = False
fitEtaBins     = False

# I/O
logFile = inputFile.replace( 'fitTopTransferFunctions', 'compareTopTransferFunctions' )
logFile = logFile.replace( '.test.root', '.test.log' )
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
name = ''
if useAlt:
  name += 'Alt'
if refGen:
  name += 'Gen'
if useSymm:
  name += 'Symm'
logFile = logFile.replace( '_.', '_' + name + '.', 1 )
logFile = logFile.replace( '_.', '.', 1 )
pathPlots = ''
if plot:
  pathPlots = logFile.replace( '.test.log', '' )
  if refSel:
    pathPlots += 'Ref_'
  pathPlots = '%s/output/plots/fitTopTransferFunctions/%s_'%( os.getenv( "CMSSW_BASE" ), pathPlots )
inputFile  = 'file:%s/output/%s'%( os.getenv( "CMSSW_BASE" ), inputFile )
logFile    = '%s/output/%s'%( os.getenv( "CMSSW_BASE" ), logFile )
cfgFile    = logFile.replace( '.test.log', '_cfg.test.py' )


# Processing

process = cms.PSet()
process.verbose = cms.uint32( 3 )
process.objectCategories = cms.vstring( objects )
process.useAlt    = cms.bool( useAlt )
process.useNonT   = cms.bool( useNonT )
process.useSymm   = cms.bool( useSymm )
process.refGen    = cms.bool( refGen )
process.refSel    = cms.bool( refSel )

process.io = cms.PSet(
  inputFile  = cms.string( inputFile )
, overwrite  = cms.bool( overwrite )
, pathPlots  = cms.string( pathPlots )
)

process.fit = cms.PSet(
  fitNonRestr  = cms.bool( fitNonRestr )
, fitEtaBins   = cms.bool( fitEtaBins )
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
