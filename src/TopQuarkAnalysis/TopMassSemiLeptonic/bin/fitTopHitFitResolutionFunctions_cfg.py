import os

import FWCore.ParameterSet.Config as cms

process = cms.PSet()
process.objectCategories = cms.vstring( 'Mu'
                                      #, 'Elec'
                                      , 'UdscJet'
                                      , 'BJet'
                                      , 'MET'
                                      )
process.resolutionFunction        = cms.string( 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])' )
process.resolutionFunctionInverse = cms.string( 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])' )

process.fitter = cms.PSet(
  inputFile = cms.string( 'file:%s/output/inputHitFitResolutionFunctions.root'%( os.getenv( "CMSSW_BASE" ) ) )
, selections = cms.vstring( 'analyzeHitFitResolutionFunctions'
                          #, 'analyzeHitFitResolutionFunctions_Reference'
                          )
)

process.useExisting = cms.bool( True )
process.existing = cms.PSet(
  outputFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions.root'%( os.getenv( "CMSSW_BASE" ) ) )
, resolutionFiles = cms.vstring( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution.txt'
                                 #'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution.txt'
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution.txt'
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution.txt'
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafKtResolution.txt'
                               )
)
