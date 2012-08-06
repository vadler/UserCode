import os

import FWCore.ParameterSet.Config as cms

# Steering
runTest = True

# Origin of existing resolution functions
# era = 'Spring10'
era = 'Summer11'

process = cms.PSet()
process.verbose = cms.bool( False )
if runTest:
  process.verbose = True
process.plotter = cms.PSet(
  outputFile = cms.string( 'file:%s/output/plotHitFitResolutionFunctions_from%s.root'%( os.getenv( "CMSSW_BASE" ), era ) )
, inputFiles = cms.vstring( 'file:%s/output/fitTopHitFit_from%s.root'%( os.getenv( "CMSSW_BASE" ), era )
                          )
, objectCategories = cms.vstring( 'Mu'
, objectCategories = cms.vstring( 'UdscJet'
                                )
, selection = cms.string( 'analyzeHitFitResolutionFunctions' )
)
if runTest:
  process.plotter.outputFile = 'file:%s/output/plotHitFitResolutionFunctions_from%s.test.root'%( os.getenv( "CMSSW_BASE" ), era )
else:
  process.plotter.objectCategories.append( 'Elec' )
  process.plotter.objectCategories.append( 'BJet' )
  process.plotter.objectCategories.append( 'MET' )
  #process.plotter.selection = 'analyzeHitFitResolutionFunctions_Reference'
