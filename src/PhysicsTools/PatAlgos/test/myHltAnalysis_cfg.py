import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

# source
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    '/store/relval/CMSSW_3_5_0_pre1/RelValTTbar/GEN-SIM-RECO/STARTUP3X_V14-v1/0006/14920B0A-0DE8-DE11-B138-002618943926.root'
    #'file:/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_5_X_2010-01-06-0200/output/myPatLayer1_fromAOD_full.root'
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
