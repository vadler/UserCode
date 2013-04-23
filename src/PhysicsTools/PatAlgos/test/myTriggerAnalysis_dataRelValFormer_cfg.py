import FWCore.ParameterSet.Config as cms

# Steering
cmsswVersion = 'CMSSW_6_1_1'
globalTag    = 'GR_R_61_V6'
condition    = 'com10_5E33v4'

process = cms.Process( "HLTPROV" )

# Messaging
process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.debugModules.append( 'l1GtAnalyzer' ) # FIXME: this does not work yet
process.MessageLogger.categories.append( 'L1GtAnalyzer' )
process.MessageLogger.categories.append( 'L1GtTrigReport' )
process.MessageLogger.cerr.threshold = 'DEBUG'
process.MessageLogger.cerr.DEBUG          = cms.untracked.PSet( limit = cms.untracked.int32( 0 ) )
process.MessageLogger.cerr.L1GtAnalyzer   = cms.untracked.PSet( limit = cms.untracked.int32( -1 ) )
process.MessageLogger.cerr.L1GtTrigReport = cms.untracked.PSet( limit = cms.untracked.int32( -1 ) )

# Conditions
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag( process.GlobalTag, 'auto:%s'%( condition ) )

## Input
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    pickRelValInputFiles( cmsswVersion  = cmsswVersion
                        , relVal        = 'SingleMu'
                        , dataTier      = 'RECO'
                        , condition     = condition
                        , globalTag     = '%s_RelVal_mu2012A'%( globalTag )
                        , maxVersions   = None
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
process.hltEventAnalyzerAOD.triggerName    = cms.string( '@' )
process.hltEventAnalyzerReAOD = process.hltEventAnalyzerAOD.clone( processName    = cms.string( 'reHLT' )
                                                                 , triggerResults = cms.InputTag( 'TriggerResults', '', 'reHLT' )
                                                                 , triggerEvent   = cms.InputTag( 'hltTriggerSummaryAOD', '', 'reHLT' )
                                                                 )
process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )
process.triggerSummaryAnalyzerAOD.inputTag = cms.InputTag( 'hltTriggerSummaryAOD', '', 'HLT' )
process.triggerSummaryAnalyzerReAOD = process.triggerSummaryAnalyzerAOD.clone( inputTag = cms.InputTag( 'hltTriggerSummaryAOD', '', 'reHLT' )
                                                                             )
process.load( "L1Trigger.GlobalTriggerAnalyzer.l1GtAnalyzer_cfi" )
process.l1GtAnalyzer.AlgorithmName = "L1_SingleMu7"
process.l1GtAnalyzer.ConditionName = "SingleMu_0x0B"
process.load( "L1Trigger.GlobalTriggerAnalyzer.l1GtTrigReport_cfi" )
# process.l1GtTrigReport.PrintVerbosity = 101
process.pL1 = cms.Path(
  process.l1GtAnalyzer
+ process.l1GtTrigReport
)
process.pHlt = cms.Path(
  process.hltEventAnalyzerAOD
+ process.triggerSummaryAnalyzerAOD
)
process.pReHlt = cms.Path(
  process.hltEventAnalyzerReAOD
+ process.triggerSummaryAnalyzerReAOD
)
