import os

import FWCore.ParameterSet.Config as cms

# Steering

runTest   = True
rfioInput = False

# Origin of existing resolution functions
# era = 'Spring10'
era = 'Summer11'

# Lepton
lepton = 'Mu'
# lepton = 'Elec'

# Settings
overwrite = True # to throw away earlier versions of histograms, trees and functions
# !!! Exclusive switches:
useAlt  = False # 'True' makes sense only, if available in input (will not crash other wise)
useSymm = True # 'True' makes sense only, if available in input (will not crash other wise)
refGen  = False # 'True' makes sense only, if available in input (will not crash other wise)
refSel  = False # 'True' makes sense only, if available in input (will not crash other wise) and with enough statistics
if runTest:
  refSel = False

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
process.refSel    = cms.bool( refSel )

process.fitter = cms.PSet(
  inputFile                    = cms.string( inputFile )
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

process.existing = cms.PSet(
  resolutionFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era ) )
  # skip resolutions without existing counterpart?
, onlyExisting   = cms.bool( True ) # True includes the writing of resolution function text files.
, pathOut        = cms.string( '%s/src/TopQuarkAnalysis/TopHitFit/data/resolution_from%s'%( os.getenv( "CMSSW_BASE" ), era ) ) # path to write the new resolution functions
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
