import os

import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.existing = cms.PSet(
  objectCategories = cms.vstring( 'Mu'
                                , 'Elec'
                                , 'UdscJet'
                                , 'BJet'
                                , 'MET'
                                )
, objectPtLimits = cms.vdouble( 400.
                              , 400.
                              , 500.
                              , 500.
                              , 300.
                              )
, resolutionFiles = cms.vstring( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution.txt'
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution.txt'
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution.txt'
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution.txt'
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafKtResolution.txt'
                               )
, resolutionFunction        = cms.string( 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])' )
, resolutionFunctionInverse = cms.string( 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])' )
, outputFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions.root'%( os.getenv( "CMSSW_BASE" ) ) )
)
