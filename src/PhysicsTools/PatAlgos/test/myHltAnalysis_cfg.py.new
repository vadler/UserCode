import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

# source
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    '/store/relval/CMSSW_3_6_0_pre6/RelValTTbar/GEN-SIM-RECO/START36_V4-v1/0010/323F69C9-9F44-DF11-8BA0-002618943943.root'
  )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 100 )
)

# HLT analyzers
process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
process.hltEventAnalyzerAOD.triggerName = cms.string( '@' )
process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

process.p = cms.Path(
    process.hltEventAnalyzerAOD       +
    process.triggerSummaryAnalyzerAOD
)
