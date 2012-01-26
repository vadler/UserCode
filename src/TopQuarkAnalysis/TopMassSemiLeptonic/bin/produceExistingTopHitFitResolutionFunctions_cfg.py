import os

import FWCore.ParameterSet.Config as cms

# era = 'Spring10'
era = 'Summer11'

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
, resolutionFiles = cms.vstring( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution_%s.txt'%( era )
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution_%s.txt'%( era )
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution_%s.txt'%( era )
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution_%s.txt'%( era )
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafKtResolution_%s.txt'%( era )
                               )
, resolutionFunction        = cms.string( 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])' )
, resolutionFunctionInverse = cms.string( 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])' )
, outputFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era ) )
)
