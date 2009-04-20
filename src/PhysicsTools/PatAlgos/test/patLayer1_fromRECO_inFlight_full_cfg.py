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
     fileNames = cms.untracked.vstring('file:/afs/cern.ch/cms/PRS/top/cmssw-data/relval200-for-pat-testing/FullSimTTBar-2_2_X_2008-11-03-STARTUP_V7-FEVTSIM.100.root')
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = cms.string('STARTUP_V7::All')
process.load("Configuration.StandardSequences.MagneticField_cff")

# PAT Layer 0+1
process.load("PhysicsTools.PatAlgos.patSequences_cff")
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.inFlightMuons = cms.EDProducer("PATGenCandsFromSimTracksProducer",
        src           = cms.InputTag("g4SimHits"), # use "famosSimHits" for FAMOS
        setStatus     = cms.int32(-1),
        particleTypes = cms.vstring("mu+"),       # picks also mu-, of course
        filter        = cms.vstring("pt > 0.5"),  # just for testing
        makeMotherLink = cms.bool(True),
        writeAncestors = cms.bool(True), # save also the intermediate GEANT ancestors of the muons
        genParticles   = cms.InputTag("genParticles"),
)
process.muMatch3 = process.muonMatch.clone(mcStatus = cms.vint32(3))
process.muMatch1 = process.muonMatch.clone(mcStatus = cms.vint32(1))
process.muMatchF = process.muonMatch.clone(mcStatus = cms.vint32(-1),
                                           matched  = cms.InputTag("inFlightMuons"))
process.patDefaultSequence.replace(process.muonMatch, process.muMatch1+process.muMatch3+process.muMatchF)
process.allLayer1Muons.genParticleMatch = cms.VInputTag(
    cms.InputTag("muMatch3"),
    cms.InputTag("muMatch1"), 
    cms.InputTag("muMatchF"),
)

#process.content = cms.EDAnalyzer("EventContentAnalyzer")
process.p = cms.Path(
    process.inFlightMuons + 
    process.patDefaultSequence  
)

# Output module configuration
from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent
process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('PATLayer1_Output.fromRECO_inFlight_full.root'),
    # save only events passing the full path
    SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
    outputCommands = cms.untracked.vstring(
                'drop *', 
                'keep *_inFlightMuons_*_*',
                *patEventContent  # you need a '*' to unpack the list of commands 'patEventContent'
    )
)
process.outpath = cms.EndPath(process.out)

