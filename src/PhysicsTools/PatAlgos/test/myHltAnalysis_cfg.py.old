import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

# source
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    '/store/relval/CMSSW_3_8_6/RelValTTbar/GEN-SIM-RECO/START38_V13-v1/0065/2856A4C7-B5E7-DF11-BE1D-00304867BFA8.root'
  )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 1 )
)

# conditions
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('START38_V14::All')

# HLT analyzers
process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
process.hltEventAnalyzerAOD.triggerName = cms.string( '@' )
process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

process.p = cms.Path(
  process.hltEventAnalyzerAOD
+ process.triggerSummaryAnalyzerAOD
)
