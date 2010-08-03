import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )

## MessageLogger
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )

## Options and Output Report
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( False )
)

## Source
process.source = cms.Source( "PoolSource"
, fileNames  = cms.untracked.vstring(
    '/store/data/Commissioning10/MinimumBias/RAW-RECO/Apr1Skim_GOODCOLL-v1/0139/FA7B208C-B33E-DF11-A713-003048679010.root'
  , '/store/data/Commissioning10/MinimumBias/RAW-RECO/v8/000/132/658/E8443DDC-AF41-DF11-90D2-003048D4777E.root'
  )
, skipEvents = cms.untracked.uint32( 7800 )
)
## Maximal Number of Events
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 100 )
)

# HLT analyzers
process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
process.hltEventAnalyzerAOD.triggerName = cms.string( '@' )
process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

process.p = cms.Path(
  process.hltEventAnalyzerAOD
+ process.triggerSummaryAnalyzerAOD
)
