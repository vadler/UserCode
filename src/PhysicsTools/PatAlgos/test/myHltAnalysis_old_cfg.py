import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

# Conditions
condition = 'startup'
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ condition ] )

# Source
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_0_pre4'
                        , relVal        = None
                        , dataTier      = None
                        , condition     = condition
                        , globalTag     = 'START42_V1'
                        , maxVersions   = None
                        , skipFiles     = 2
                        , numberOfFiles = 0
                        , debug         = True
                        )
  )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 1 )
)

# HLT analyzers
process.load( "HLTrigger.HLTcore.hltEventAnalyzerAOD_cfi" )
process.hltEventAnalyzerAOD.triggerName = cms.string( '@' )
process.load( "HLTrigger.HLTcore.triggerSummaryAnalyzerAOD_cfi" )

process.p = cms.Path(
  process.hltEventAnalyzerAOD
+ process.triggerSummaryAnalyzerAOD
)
