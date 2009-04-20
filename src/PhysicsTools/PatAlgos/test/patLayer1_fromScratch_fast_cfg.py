import FWCore.ParameterSet.Config as cms

process = cms.Process("PAT")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append('PATSummaryTables')
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    default          = cms.untracked.PSet( limit = cms.untracked.int32(0)  ),
    PATSummaryTables = cms.untracked.PSet( limit = cms.untracked.int32(-1) )
)
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

# FAMOS source
process.load("FastSimulation.Configuration.ttbar_cfi")
# some FAMOS setup (FIXME maybe this should go)
process.load("PhysicsTools.PatAlgos.famos.boostrapWithFamos_cff")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

# PAT Layer 0+1
process.load("PhysicsTools.PatAlgos.patSequences_cff")

## Load additional RECO config
# Magnetic field now needs to be in the high-level py
process.load("Configuration.StandardSequences.MagneticField_cff")

# Switch off old trigger matching
from PhysicsTools.PatAlgos.tools.trigTools import switchOffTriggerMatchingOld
switchOffTriggerMatchingOld( process )

#process.content = cms.EDAnalyzer("EventContentAnalyzer")
process.p = cms.Path(
                process.famosWithEverything        +    # run full FAMOS (ttbar events)
                #process.content                   +    # to get a dump of the output of FAMOS
                process.patDefaultSequence
            )


# Output module configuration
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent
process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('PATLayer1_Output.fromScratch_fast.root'),
    # save only events passing the full path
    SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
    # save PAT Layer 1 output
    outputCommands = cms.untracked.vstring('drop *', *patEventContent ) # you need a '*' to unpack the list of commands 'patEventContent'
)
process.outpath = cms.EndPath(process.out)

