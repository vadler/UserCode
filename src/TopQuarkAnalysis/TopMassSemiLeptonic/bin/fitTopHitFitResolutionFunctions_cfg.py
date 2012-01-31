import os

import FWCore.ParameterSet.Config as cms

# Steering
runTest   = True
rfioInput = False

# era = 'Spring10'
era = 'Summer11'

process = cms.PSet()
process.verbose = cms.bool( False )
if runTest:
  process.verbose = True
process.objectCategories = cms.vstring( 'Mu'
                                      #, 'Elec'
                                      , 'UdscJet'
                                      , 'BJet'
                                      #, 'MET'
                                      )
process.resolutionFunction        = cms.string( 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])' )
process.resolutionFunctionInverse = cms.string( 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])' )

inputFile = 'file:%s/output/fitHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era )
if runTest:
  inputFile = inputFile.replace( 'root', 'test.root' )
if not rfioInput:
  inputFile = inputFile.replace( 'root', 'local.root' )
logFile = inputFile.replace( 'root', 'log' )
process.fitter = cms.PSet(
  inputFile = cms.string( inputFile )
, selections = cms.vstring( 'analyzeHitFitResolutionFunctions'
                          )
)
if not runTest:
  process.fitter.selections.append( 'analyzeHitFitResolutionFunctions_Reference' )

process.useExisting = cms.bool( True )
process.existing = cms.PSet(
  resolutionFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era ) )
)

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