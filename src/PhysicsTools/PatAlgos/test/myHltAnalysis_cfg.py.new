import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('GR10_P_V2::All')

# source
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
#     '/store/relval/CMSSW_3_5_0_pre1/RelValTTbar/GEN-SIM-RECO/STARTUP3X_V14-v1/0006/14920B0A-0DE8-DE11-B138-002618943926.root'
    'file:/afs/cern.ch/user/v/vadler/cms/PAT/CMSSW_3_5_6/output/patTuple_withRECO_6jets_Run132440.root'
  )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)

# HLT analyzers
process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
process.hltEventAnalyzerAOD.triggerName = cms.string( '@' )
process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

process.p = cms.Path(
    process.hltEventAnalyzerAOD       +
    process.triggerSummaryAnalyzerAOD
)
