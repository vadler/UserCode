import FWCore.ParameterSet.Config as cms

process = cms.Process( "PAT" )

# initialize MessageLogger and output report
process.load( "FWCore.MessageLogger.MessageLogger_cfi" )
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.categories.append( 'PATSummaryTables' )
process.MessageLogger.cerr.INFO = cms.untracked.PSet(
    default          = cms.untracked.PSet(
        limit = cms.untracked.int32( 0 )
    ),
    PATSummaryTables = cms.untracked.PSet(
        limit = cms.untracked.int32( -1 )
    )
)
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)

# source
process.source = cms.Source( "PoolSource", 
    fileNames = cms.untracked.vstring(
        '/store/relval/CMSSW_2_2_10/RelValTTbar/GEN-SIM-RECO/STARTUP_V11_v1/0003/EA8326A6-CA3D-DE11-BC02-001D09F24F65.root',
        '/store/relval/CMSSW_2_2_10/RelValTTbar/GEN-SIM-RECO/STARTUP_V11_v1/0003/D89D8968-293E-DE11-8854-001D09F29597.root',
        '/store/relval/CMSSW_2_2_10/RelValTTbar/GEN-SIM-RECO/STARTUP_V11_v1/0003/D49A4D44-043E-DE11-B78D-001D09F297EF.root',
        '/store/relval/CMSSW_2_2_10/RelValTTbar/GEN-SIM-RECO/STARTUP_V11_v1/0003/AAD38DE1-C73D-DE11-898C-0019B9F72BFF.root',
        '/store/relval/CMSSW_2_2_10/RelValTTbar/GEN-SIM-RECO/STARTUP_V11_v1/0003/A080EEB3-BF3D-DE11-AC46-001D09F2437B.root',
        '/store/relval/CMSSW_2_2_10/RelValTTbar/GEN-SIM-RECO/STARTUP_V11_v1/0003/98EF784D-CB3D-DE11-A9D7-001D09F24498.root',
        '/store/relval/CMSSW_2_2_10/RelValTTbar/GEN-SIM-RECO/STARTUP_V11_v1/0003/6EFB8F88-C93D-DE11-8C72-001D09F23A84.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 1000 )   # Reduce number of events for testing.
)

process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = cms.string( 'STARTUP_V11::All' )
process.load( "Configuration.StandardSequences.MagneticField_cff" )

# PAT itself
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
process.load( "PhysicsTools.PatExamples.mcMatch_cfi" )   # The file to modify!

# Comment these lines in order to run the default rather than
# your OWN MC matching from PhysicsTools/PatExamples/python/mcMatching_cfi
# CAUTION: Uncommented, this does NOT run out-of-the-box!
# Own muon match
process.patMCTruth_LeptonPhoton.remove( process.muonMatch )
process.patMCTruth_LeptonPhoton += process.myMuonMatch
process.allLayer1Muons.genParticleMatch = "myMuonMatch"
process.allLayer1Muons.embedGenMatch = True
# Own jet match to MC jets
process.patMCTruth_Jet.remove( process.jetGenJetMatch )
process.patMCTruth_Jet += process.myJetGenJetMatch
process.allLayer1Jets.genJetMatch = "myJetGenJetMatch"

process.p = cms.Path(
    process.patDefaultSequence
)
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOffTriggerMatchingOld( process )

# Output module configuration
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out = cms.OutputModule( "PoolOutputModule",
    fileName       = cms.untracked.string( 'edmPatMcMatch.root' ),
    SelectEvents   = cms.untracked.PSet(
        SelectEvents = cms.vstring( 'p' )
    ),
    outputCommands = cms.untracked.vstring( 'drop *', *patEventContent )
)
process.out.outputCommands += [ 'keep recoGenParticles_genParticles_*_*' ] # Keeps the MC objects for references

process.outpath = cms.EndPath(
    process.out
)
