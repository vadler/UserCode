import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

# source
process.source = cms.Source("PoolSource", 
    fileNames = cms.untracked.vstring(
        '/store/relval/CMSSW_3_3_0_pre2/RelValTTbar/GEN-SIM-RECO/STARTUP31X_V7-v1/0002/BA958CA5-B19B-DE11-90C6-0018F3D0961A.root'
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
