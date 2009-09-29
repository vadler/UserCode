import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

# source
process.source = cms.Source("PoolSource", 
    fileNames = cms.untracked.vstring(
        '/store/relval/CMSSW_3_3_0_pre5/RelValTTbar/GEN-SIM-RECO/STARTUP31X_V7-v1/0003/00D6950D-0FAB-DE11-A478-001D09F29114.root'
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
