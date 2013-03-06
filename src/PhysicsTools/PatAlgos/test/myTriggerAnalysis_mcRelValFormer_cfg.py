import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

# Steering
cmsswVersion = 'CMSSW_6_2_0_pre2'
globalTag    = 'START61_V11'

## Messaging
process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.debugModules.append( 'l1GtAnalyzer' ) # FIXME: this does not work yet
process.MessageLogger.categories.append( 'L1GtAnalyzer' )
process.MessageLogger.categories.append( 'L1GtTrigReport' )
process.MessageLogger.cerr.threshold = 'DEBUG'
process.MessageLogger.cerr.DEBUG          = cms.untracked.PSet( limit = cms.untracked.int32( 0 ) )
process.MessageLogger.cerr.L1GtAnalyzer   = cms.untracked.PSet( limit = cms.untracked.int32( -1 ) )
process.MessageLogger.cerr.L1GtTrigReport = cms.untracked.PSet( limit = cms.untracked.int32( -1 ) )

# Conditions
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = '%s::All'%( globalTag )

## Input
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(

    pickRelValInputFiles( #formerVersion = True
                          cmsswVersion  = cmsswVersion
                        , relVal        = 'RelValProdTTbar'
                        , dataTier      = 'AODSIM'
                        , condition     = None
                        , globalTag     = globalTag
                        , maxVersions   = 1
                        , skipFiles     = None
                        , numberOfFiles = None
                        , debug         = True
                        )
  )
, skipBadFiles = cms.untracked.bool( True )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 1 )
)

## Processing
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
