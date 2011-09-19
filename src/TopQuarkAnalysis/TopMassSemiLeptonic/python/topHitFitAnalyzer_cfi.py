import FWCore.ParameterSet.Config as cms

topHitFitAnalyzer = cms.EDAnalyzer(
  "TopHitFitAnalyzer"
, ttSemiLeptonicEvent = cms.InputTag( 'ttSemiLepEvent' )
)