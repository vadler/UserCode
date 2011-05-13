import FWCore.ParameterSet.Config as cms

pvSelection = cms.PSet(
  minNdof = cms.double( 4. )
, maxZ    = cms.double( 24. )
, maxRho  = cms.double( 2. )
)
pvSrc = cms.InputTag( 'offlinePrimaryVertices' )

step2 = cms.EDFilter(
#   "VertexSelector"
# , src    = pvSrc
# , cut    = cms.string( '!isFake && ndof > 4 && abs(z) <= 24. && abs(position.rho) <= 2.' )
# , filter = cms.bool( True )
  "PrimaryVertexFilter" # checks for fake PVs automatically
, pvSelection
, pvSrc = pvSrc
)

goodPrimaryVertices = cms.EDProducer(
  "PrimaryVertexObjectFilter" # checks for fake PVs automatically
, filterParams = pvSelection
, src          = pvSrc
)
