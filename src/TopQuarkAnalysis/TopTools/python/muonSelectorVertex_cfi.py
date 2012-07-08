import FWCore.ParameterSet.Config as cms

muonSelectorVertex = cms.EDProducer(
  "MuonSelectorVertex"
, muonSource   = cms.InputTag( 'selectedPatMuons' )
, vertexSource = cms.InputTag( 'goodOfflinePrimaryVertices' )
, maxDZ = cms.double( 1. )
)
