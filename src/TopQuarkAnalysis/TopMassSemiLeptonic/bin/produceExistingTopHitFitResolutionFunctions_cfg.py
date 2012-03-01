import os

import FWCore.ParameterSet.Config as cms

# era = 'Spring10'
era = 'Summer11'

process = cms.PSet()

process.existing = cms.PSet(
  # Object categories to use, available: 'Mu', 'Elec', 'UdscJet', 'BJet', 'MET'
  objectCategories = cms.vstring( 'Mu'
                                , 'Elec'
                                , 'UdscJet'
                                , 'BJet'
                                , 'MET'
                                )
  # P_t limits for the plots, in line with the categories in "objectCategories"
, objectPtLimits = cms.vdouble( 400.
                              , 400.
                              , 500.
                              , 500.
                              , 300.
                              )
  # Text file sources, in line with the categories in "objectCategories"
, resolutionFiles = cms.vstring( 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution_%s.txt'%( era )
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution_%s.txt'%( era )
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution_%s.txt'%( era )
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution_%s.txt'%( era )
                               , 'TopQuarkAnalysis/TopHitFit/data/resolution/tqafKtResolution_%s.txt'%( era )
                               )
  # Kinematic variables to use; available: 'Pt', 'Eta', 'Phi'
, kinematicProperties = cms.vstring( 'Pt'
                                   , 'Eta'
                                   , 'Phi'
                                   )
  # resolution function formulas
, resolutionFunction        = cms.string( 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])' )
, resolutionFunctionInverse = cms.string( 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])' )
  # derived formulas
, resolutionFunctionRel           = cms.string( 'sqrt(([0]*[0]*x+[1]*[1])*x+[2]*[2])/x' )
, resolutionFunctionInverseRel    = cms.string( 'sqrt(([0]*[0]/x+[1]*[1])/x+[2]*[2])/x' )
, resolutionFunctionInverseInv    = cms.string( 'sqrt(([2]*[2]*x+[1]*[1])*x+[0]*[0])*x' )
, resolutionFunctionInverseInvRel = cms.string( 'sqrt(([2]*[2]*x+[1]*[1])*x+[0]*[0])' )
  # output file
, outputFile = cms.string( 'file:%s/output/existingHitFitResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era ) )
)
