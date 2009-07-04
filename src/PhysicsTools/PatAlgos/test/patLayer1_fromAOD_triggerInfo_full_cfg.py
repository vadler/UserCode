import FWCore.ParameterSet.Config as cms

process = cms.Process( "PAT" )

# initialize MessageLogger and output report
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)

# source
process.source = cms.Source( "PoolSource", 
    fileNames = cms.untracked.vstring(
        '/store/relval/CMSSW_3_1_0_pre10/RelValTTbar/GEN-SIM-RECO/IDEAL_31X_v1/0008/CC80B73A-CA57-DE11-BC2F-000423D99896.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 100 )
)

process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = cms.string( 'MC_31X_V1::All' )
process.load( "Configuration.StandardSequences.MagneticField_cff" )

# PAT Layer 0 & 1
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
# replacements currently needed to make the electrons work
process.allLayer1Electrons.addElectronShapes = False
process.allLayer1Electrons.addElectronID     = False

process.p = cms.Path(
    process.patDefaultSequence
)

# Output module configuration
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out = cms.OutputModule( "PoolOutputModule",
    fileName       = cms.untracked.string( 'PATLayer1_Output.fromAOD_triggerInfo_full.root' ),
    SelectEvents   = cms.untracked.PSet(
        SelectEvents = cms.vstring( 'p' )
    ),
    outputCommands = cms.untracked.vstring( 'drop *', *patEventContent )
)

# Trigger
from PhysicsTools.PatAlgos.tools.trigTools import *
process.p *= process.cleanLayer1Objects
switchOnTrigger( process )

process.outpath = cms.EndPath(
    process.out
)
