import os

import FWCore.ParameterSet.Config as cms

# Steering

runTest   = True
rfioInput = False

# Origin of existing resolution functions
# era = 'Spring10'
era = 'Summer11'

# Selection to fit for
# selection = 'analyzeHitFitResolutionFunctions'
selection = 'analyzeHitFitResolutionFunctions_Reference'

# Lepton
lepton = 'Mu'
# lepton = 'Elec'

# Settings
refGen   = False # 'True' makes sense only, if available in input (will not crash other wise)
useSymm  = False # 'True' makes sense only, if available in input (will not crash other wise)
usePtRel = False # ( Delta p_t ) / p_t instead of Delta ( 1 / p_t ) in inverse p_t
useJetEt = False

inputFile = 'file:%s/output/fitTopHitFitResolutionFunctions_from%s.root'%( os.getenv( "CMSSW_BASE" ), era )
if usePtRel:
  inputFile = inputFile.replace( '.root', '_ptRel.root' )
if useJetEt:
  inputFile = inputFile.replace( '.root', '_jetEt.root' )
if runTest:
  inputFile = inputFile.replace( 'root', 'test.root' )
if not rfioInput:
  inputFile = inputFile.replace( 'root', 'local.root' )
logFile = inputFile.replace( 'root', 'log' )

# Processing

process = cms.PSet()
process.verbose = cms.uint32( 0 )
if runTest:
  process.verbose = 3
process.objectCategories = cms.vstring( lepton
                                      )
if not runTest:
  process.objectCategories.append( 'UdscJet' )
  process.objectCategories.append( 'BJet' )
  process.objectCategories.append( 'MET' )
process.refGen  = cms.bool( refGen )
process.useSymm = cms.bool( useSymm )

process.fitter = cms.PSet(
  inputFile                 = cms.string( inputFile )
, selection                 = cms.string( selection )
, resolutionFunction        = cms.string( 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])' )
, resolutionFunctionInverse = cms.string( 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])' )
)
if runTest:
  process.fitter.selection = 'analyzeHitFitResolutionFunctions'

# process.useExisting = cms.bool( True )
process.useExisting = cms.bool( False ) # nothing done there yet
process.existing = cms.PSet(
  resolutionFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era ) )
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