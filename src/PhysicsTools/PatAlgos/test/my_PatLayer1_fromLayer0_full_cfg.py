# The following comments couldn't be translated into the new config version:

# use patLayer0_withoutPFTau if you can't get PFTaus working 
import FWCore.ParameterSet.Config as cms

process = cms.Process("PATbis")

### General ###

# initialize MessageLogger
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

### Input ###

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'file:output/PATLayer0_Output.fromAOD_full.root'
    )
)

### PAT steering ###

process.load("PhysicsTools.PatAlgos.patLayer1_cff")
# process.load("PhysicsTools.PatAlgos.test.patLayer1_ReplaceDefaults_full_cff")

process.p = cms.Path(
    process.patLayer1
)


### Output ###

# define the event content
process.patEventContent = cms.PSet(
    outputCommands = cms.untracked.vstring('drop *')
)
process.load("PhysicsTools.PatAlgos.patLayer1_EventContent_cff")
process.patEventContent.outputCommands.extend(process.patLayer1EventContent.outputCommands)

# only accept events passing the complete path
process.patEventSelection = cms.PSet(
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('p')
    )
)

# the actual output module
process.out = cms.OutputModule("PoolOutputModule",
    process.patEventSelection,
    process.patEventContent,
    verbose = cms.untracked.bool(False),
    fileName = cms.untracked.string('output/PATLayer1_Output.fromLayer0_full.root')
)

process.outpath = cms.EndPath(process.out)

