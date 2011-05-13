import FWCore.ParameterSet.Config as cms

step2 = cms.EDFilter(
  "VertexSelector"
, src    = cms.InputTag( 'offlinePrimaryVertices' )
, cut    = cms.string( '!isFake && ndof > 4 && fabs(z) <= 24. && fabs(position.rho) <= 2.' )
, filter = cms.bool( True )
)
