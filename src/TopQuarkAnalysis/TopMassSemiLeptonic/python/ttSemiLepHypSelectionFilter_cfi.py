import FWCore.ParameterSet.Config as cms

ttSemiLepHypSelectionFilter = cms.EDFilter(
  "TtSemiLepHypSelectionFilter"
  # Underlying hypothesis
, ttSemiLepEvt        = cms.InputTag( 'ttSemiLepHypGenMatch' )
, ttSemiLepHyp        = cms.string( 'kGenMatch' ) # kGeom, kWMassMaxSumPt, kMaxSumPtWMass, kGenMatch, kMVADisc, kKinFit, kKinSolution, kWMassDeltaTopMass, kHitFit
, ttSemiLepHypLeptons = cms.InputTag( 'patMuons' )
, ttSemiLepHypJets    = cms.InputTag( 'patJets' )
  # Selectors to test
, processName         = cms.string( 'SKIM' )
, leptonSelector      = cms.string( 'selectedPatMuons' )
, jetSelector         = cms.string( 'selectedPatJets' )
)
