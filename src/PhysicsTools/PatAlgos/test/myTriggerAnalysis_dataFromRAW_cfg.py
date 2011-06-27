import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

# Conditions
condition = 'com10'
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ condition ] )

# Source
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/30B69E98-C67F-E011-8697-003048D37560.root'
  , '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/80E66677-C27F-E011-8A15-001D09F29321.root'
  , '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/8E69B419-B57F-E011-821E-001617C3B65A.root'
  , '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/A219173E-B07F-E011-ACC8-0030487CD6DA.root'
  , '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/B028587F-D47F-E011-94FD-0030487CD906.root'
  , '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/CCCA933C-BE7F-E011-A992-0030486730C6.root'
  , '/store/data/Run2011A/MuHad/RAW/v1/000/165/121/DCC9E8A9-BA7F-E011-A73A-003048D37580.root'
  )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 1 )
)

# Trigger analyzers
process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
process.hltEventAnalyzerAOD.triggerName = cms.string( '@' )
process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )
process.load( "L1Trigger.GlobalTriggerAnalyzer.l1GtAnalyzer_cfi" )
process.l1GtAnalyzer.AlgorithmName = "L1_SingleMu7"
process.l1GtAnalyzer.ConditionName = "SingleMu_0x0B"
process.load( "L1Trigger.GlobalTriggerAnalyzer.l1GtTrigReport_cfi" )
# process.l1GtTrigReport.PrintVerbosity = 101

process.p = cms.Path(
  process.l1GtAnalyzer
+ process.hltEventAnalyzerAOD
+ process.l1GtTrigReport
#   process.l1GtTrigReport
+ process.triggerSummaryAnalyzerAOD
)

# Message logger
process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.debugModules.append( 'l1GtAnalyzer' ) # FIXME: this does not work yet
process.MessageLogger.categories.append( 'L1GtAnalyzer' )
process.MessageLogger.categories.append( 'L1GtTrigReport' )
process.MessageLogger.cerr.threshold = 'DEBUG'
process.MessageLogger.cerr.DEBUG          = cms.untracked.PSet( limit = cms.untracked.int32( 0 ) )
process.MessageLogger.cerr.L1GtAnalyzer   = cms.untracked.PSet( limit = cms.untracked.int32( -1 ) )
process.MessageLogger.cerr.L1GtTrigReport = cms.untracked.PSet( limit = cms.untracked.int32( -1 ) )
