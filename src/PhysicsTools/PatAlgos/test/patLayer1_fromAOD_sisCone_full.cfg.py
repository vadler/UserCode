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

from PhysicsTools.PatAlgos.tools.jetTools import *

## ==== Example with CaloJets
switchJetCollection(process, 
        cms.InputTag('sisCone5CaloJets'),  # Jet collection; must be already in the event when patDefaultSequence is executed
        doJTA=True,            # Run Jet-Track association & JetCharge
        doBTagging=True,       # Run b-tagging
        jetCorrLabel=('SC5','Calo'), # example jet correction name; set to None for no JEC
        doType1MET=True,       # recompute Type1 MET using these jets
        genJetCollection=cms.InputTag("sisCone5GenJets")) 

## ==== FOR BASIC JETS
### make some basic jets for testing
# from RecoJets.JetProducers.BasicJetIcone5_cfi import iterativeCone5BasicJets
# process.iterativeCone5BasicJets = iterativeCone5BasicJets.clone(src = cms.InputTag("towerMaker"))
#
### configure PAT
# switchJetCollection(process, 
#        cms.InputTag('iterativeCone5BasicJets'), # Jet collection; must be already in the event patLayer0 sequence is executed
#        doJTA=True,
#        doBTagging=True,
#        jetCorrLabel=None,#=('S5','Calo'), # If you have JES corrections, you can apply them even to BasicJets
#        doType1MET=False)                  # Type1MET dows not work on BasicJets :-(

# Switch off old trigger matching
from PhysicsTools.PatAlgos.tools.trigTools import switchOffTriggerMatchingOld
switchOffTriggerMatchingOld( process )

#process.content = cms.EDAnalyzer("EventContentAnalyzer")
process.p = cms.Path(
                ## process.iterativeCone5BasicJets +  ## Turn on this to run tests on BasicJets
                process.patDefaultSequence  
            )


# Output module configuration
process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('PATLayer1_Output.fromAOD_sisCone_full.root'),
    SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
    outputCommands = cms.untracked.vstring('drop *')
)
process.outpath = cms.EndPath(process.out)
# save PAT Layer 1 output
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out.outputCommands += patEventContent
