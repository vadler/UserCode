import FWCore.ParameterSet.Config as cms

process = cms.Process( "HLTPROV" )

# Conditions
condition = 'startup'
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.PyReleaseValidation.autoCond import autoCond
process.GlobalTag.globaltag = cms.string( autoCond[ condition ] )

# Source
from PhysicsTools.PatAlgos.tools.myTools import pickRelValInputFiles
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    pickRelValInputFiles( cmsswVersion  = None
                        , relVal        = None
                        , dataTier      = None
                        , condition     = None
                        , globalTag     = None
                        , maxVersions   = None
                        , skipFiles     = None
                        , numberOfFiles = None
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
