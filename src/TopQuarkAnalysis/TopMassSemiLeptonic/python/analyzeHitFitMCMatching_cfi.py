import FWCore.ParameterSet.Config as cms

# Steering

analyzeHitFitMCMatching = cms.EDAnalyzer(
  "AnalyzeHitFitMCMatching"

  ### Input ###

  # TQAF semi-leptonic event
, ttSemiLeptonicEvent = cms.InputTag( 'ttSemiLepEvent' )

  # Leptons to use
, leptonType = cms.string( 'Muons' ) # 'Muons' or 'Electrons'

  # Collections
, patLeptons = cms.InputTag( '' ) # dummy, needs assignemnt in main config
, patJets    = cms.InputTag( 'selectedPatJets' )
)
