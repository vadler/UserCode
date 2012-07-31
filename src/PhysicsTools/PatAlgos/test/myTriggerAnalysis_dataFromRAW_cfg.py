import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

# Conditions
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.autoCond import autoCond
condition                   = 'com10_5E33v4'
process.GlobalTag.globaltag = cms.string( autoCond[ condition ][ 0 ] )
l1Menu                      = autoCond[ condition ][ 1 ].split( ',' )
process.GlobalTag.toGet.append( cms.PSet( tag     = cms.string( l1Menu[ 0 ] )
                                        , record  = cms.string( l1Menu[ 1 ] )
                                        , connect = cms.untracked.string( l1Menu[ 2 ] )
                                        )
                              )

# Source
from PhysicsTools.PatAlgos.myPatTuple_dataFromRAW_cff import fileNames
process.source = cms.Source("PoolSource",
  fileNames
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
