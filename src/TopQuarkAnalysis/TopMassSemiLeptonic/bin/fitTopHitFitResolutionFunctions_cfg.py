import os

import FWCore.ParameterSet.Config as cms

# Steering

runTest   = True
rfioInput = False

# Origin of existing resolution functions
# era = 'Spring10'
era = 'Summer11'

# Selection to fit for
selection = 'analyzeHitFitResolutionFunctions'
# selection = 'analyzeHitFitResolutionFunctions_Reference'

# Lepton
lepton = 'Mu'
# lepton = 'Elec'

# Settings
overwrite = True # to throw away earlier versions of histograms, trees and functions
# !!! Exclusive switches:
useAlt  = False # 'True' makes sense only, if available in input (will not crash other wise)
useSymm = False # 'True' makes sense only, if available in input (will not crash other wise)
refGen  = False # 'True' makes sense only, if available in input (will not crash other wise)

inputFile = 'file:%s/output/fitTopHitFitResolutionFunctions_from%s.root'%( os.getenv( "CMSSW_BASE" ), era )
if runTest:
  inputFile = inputFile.replace( 'root', 'test.root' )
if not rfioInput:
  inputFile = inputFile.replace( 'root', 'local.root' )
logFile = inputFile.replace( 'root', 'log' )

# Processing

process = cms.PSet()
process.verbose = cms.uint32( 2 )
if runTest:
  process.verbose = 3
process.objectCategories = cms.vstring( lepton
                                      )
if not runTest:
  process.objectCategories.append( 'UdscJet' )
  process.objectCategories.append( 'BJet' )
  process.objectCategories.append( 'MET' )
process.overwrite = cms.bool( overwrite )
process.useAlt    = cms.bool( useAlt )
process.useSymm   = cms.bool( useSymm )
process.refGen    = cms.bool( refGen )

process.fitter = cms.PSet(
  inputFile                    = cms.string( inputFile )
, selection                    = cms.string( selection )
  # resolution function formulas
, resolutionFunction           = cms.string( 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])' )
, resolutionFunctionInverse    = cms.string( 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])' )
  # derived formulas
, resolutionFunctionRel           = cms.string( 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])/x' )
, resolutionFunctionInverseRel    = cms.string( 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])/x' )
, resolutionFunctionInverseInv    = cms.string( 'sqrt(([2]*[2]*x+[1]*[1])*x+[0]*[0])*x' )
, resolutionFunctionInverseInvRel = cms.string( 'sqrt(([2]*[2]*x+[1]*[1])*x+[0]*[0])' )
  # histogram options
, widthFactor = cms.double( 5. )
)
if runTest:
  process.fitter.selection = 'analyzeHitFitResolutionFunctions'

process.existing = cms.PSet(
  resolutionFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era ) )
  # skip resolutions without existing counterpart?
  onlyExisting   = cms.bool( True )
)

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