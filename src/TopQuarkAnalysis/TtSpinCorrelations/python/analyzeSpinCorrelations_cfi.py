import FWCore.ParameterSet.Config as cms

analyzeSpinCorrelations = cms.EDAnalyzer(
  "AnalyzeSpinCorrelations"

  ### Input ###

  # TQAF semi-leptonic event
, ttSemiLeptonicEvent = cms.InputTag( 'ttSemiLepEvent' )

  # Leptons to use
, useMuons     = cms.bool( True )
, useElectrons = cms.bool( False )

  # Spin basis to use
, useHelicityBasis    = cms.bool( True )
, useBeamBasis        = cms.bool( True )
, useOffDiagonalBasis = cms.bool( True )

  # Histogram settings
, binsMassTt = cms.uint32( 100 )
, minMassTt  = cms.double(    0. )
, maxMassTt  = cms.double( 2000. )
, binsCos1D = cms.uint32( 25 )
, binsCos2D = cms.uint32(  9 )

  # Spin analyzer quantities
, kappaLplus = cms.double( 1. )
, kappaB     = cms.double( -0.41 )
, kappaQ     = cms.double(  0.51 )

)

