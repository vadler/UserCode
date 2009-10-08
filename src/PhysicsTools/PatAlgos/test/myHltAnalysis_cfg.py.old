import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

# source
process.source = cms.Source("PoolSource", 
    fileNames = cms.untracked.vstring(
        '/store/relval/CMSSW_3_3_0_pre6/RelValTTbar/GEN-SIM-RECO/STARTUP31X_V8-v1/0005/9EA6A564-FCB0-DE11-A4D1-001D09F29849.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 10 )
)

# HLT analyzers
process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
process.hltEventAnalyzerAOD.triggerName = cms.string( '@' )
process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )
                                                                       
process.p = cms.Path(
    process.hltEventAnalyzerAOD       +
    process.triggerSummaryAnalyzerAOD
)
