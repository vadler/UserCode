import os

import FWCore.ParameterSet.Config as cms

# Steering
runTest   = True
rfioInput = False

process = cms.PSet()
process.objectCategories = cms.vstring( 'Mu'
                                      #, 'Elec'
                                      , 'UdscJet'
                                      , 'BJet'
                                      , 'MET'
                                      )
process.resolutionFunction        = cms.string( 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])' )
process.resolutionFunctionInverse = cms.string( 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])' )

inputFile = 'file:%s/output/inputHitFitResolutionFunctions.root'%( os.getenv( "CMSSW_BASE" ) )
if runTest:
  inputFile = inputFile.replace( 'root', 'test.root' )
if not rfioInput:
  inputFile = inputFile.replace( 'root', 'local.root' )
process.fitter = cms.PSet(
  inputFile = cms.string( inputFile )
, selections = cms.vstring( 'analyzeHitFitResolutionFunctions'
                          )
)
if not runTest:
  process.fitter.selections.append( 'analyzeHitFitResolutionFunctions_Reference' )

process.useExisting = cms.bool( False )
process.existing = cms.PSet(
  outputFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions.root'%( os.getenv( "CMSSW_BASE" ) ) )
, resolutionFiles = cms.vstring( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution.txt'
                                 #'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution.txt'
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution.txt'
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution.txt'
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafKtResolution.txt'
                               )
)
