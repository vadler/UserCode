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

# source
process.source = cms.Source("PoolSource", 
     fileNames = cms.untracked.vstring('file:/afs/cern.ch/cms/PRS/top/cmssw-data/relval200-for-pat-testing/FullSimTTBar-2_2_X_2008-11-03-STARTUP_V7-AODSIM.100.root')
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )


# PAT Layer 0+1
process.load("PhysicsTools.PatAlgos.patSequences_cff")


## Load additional RECO config
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('STARTUP_V7::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

# Load the PF MET module
process.load("PhysicsTools.PFCandProducer.pfMET_cfi")

### Use this to add the PF MET side-by-side to the PAT MET (it will be called 'selectedLayer1PFMETs')
### This doesn't take care of the trigger matching
process.layer1PFMETs = process.layer1METs.clone(
    metSource = cms.InputTag("pfMET"),
    addTrigMatch = cms.bool(False),
    addMuonCorrections = cms.bool(False),
)
process.allLayer1Objects.replace( process.layer1METs, process.layer1METs + process.layer1PFMETs)

### Use this if you want to totally replace the PAT MET with the PF MET
##
##   # Get old label for MET
##   oldMETSource = process.layer1METs.metSource
##   
##   # Replace 'old MET' with 'pfMET' in trigger matching
##   from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceParam
##   massSearchReplaceParam(process.patTrigMatch, "src", oldMETSource, cms.InputTag("pfMET"))
##   
##   # Use PF MET to make PAT MET
##   process.layer1METs.metSource = cms.InputTag("pfMET")
##   process.layer1METs.addMuonCorrections = False

# Switch off old trigger matching
from PhysicsTools.PatAlgos.tools.trigTools import switchOffTriggerMatchingOld
switchOffTriggerMatchingOld( process )

# Now we break up process.patLayer0
process.p = cms.Path(
    process.pfMET +
    process.patDefaultSequence  
)

# Output module configuration
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent
process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('PATLayer1_Output.fromAOD_PFMET_full.root'),
    # save only events passing the full path
    SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
    # save PAT Layer 1 output
    outputCommands = cms.untracked.vstring(
                'drop *', 
                'keep *_selectedLayer1PFMETs_*_*',
                *patEventContent  # you need a '*' to unpack the list of commands 'patEventContent'
    )
)
process.outpath = cms.EndPath(process.out)

