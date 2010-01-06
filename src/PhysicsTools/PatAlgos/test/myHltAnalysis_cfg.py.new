import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

# source
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    #'/store/relval/CMSSW_3_4_0_pre7/RelValTTbar/GEN-SIM-RECO/STARTUP3X_V14-v1/0003/0AC12757-A3E1-DE11-A20D-0026189437F8.root'
    'file:/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_4_1/output/myPatLayer1_fromAOD_full.root'
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
