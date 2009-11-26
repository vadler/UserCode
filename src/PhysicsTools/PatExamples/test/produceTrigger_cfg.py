import FWCore.ParameterSet.Config as cms

process = cms.Process( "TEST" )

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
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/mc/Fall08/TTJets-madgraph/GEN-SIM-RECO/IDEAL_V11_redigi_v10/0000/06FC3959-4DFC-DD11-B504-00E08178C091.root',
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/mc/Fall08/TTJets-madgraph/GEN-SIM-RECO/IDEAL_V11_redigi_v10/0000/08126A32-C2FC-DD11-BFF3-00E08178C091.root',
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/mc/Fall08/TTJets-madgraph/GEN-SIM-RECO/IDEAL_V11_redigi_v10/0000/089F9442-28FC-DD11-803B-0015170AD174.root',
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/mc/Fall08/TTJets-madgraph/GEN-SIM-RECO/IDEAL_V11_redigi_v10/0000/0A1B6F50-B7FC-DD11-9744-00E08178C091.root',
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/mc/Fall08/TTJets-madgraph/GEN-SIM-RECO/IDEAL_V11_redigi_v10/0000/0A3AFC80-30FC-DD11-A41B-00E081791807.root',
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/mc/Fall08/TTJets-madgraph/GEN-SIM-RECO/IDEAL_V11_redigi_v10/0000/0E61259E-32FC-DD11-BECB-00E08178C0EF.root',
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/mc/Fall08/TTJets-madgraph/GEN-SIM-RECO/IDEAL_V11_redigi_v10/0000/0ED486E9-44FC-DD11-83ED-00E08178C091.root',
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/mc/Fall08/TTJets-madgraph/GEN-SIM-RECO/IDEAL_V11_redigi_v10/0000/109E30E1-C2FC-DD11-9459-00E08178C091.root',
        'dcap://dcache-ses-cms.desy.de:22125/pnfs/desy.de/cms/tier2/store/mc/Fall08/TTJets-madgraph/GEN-SIM-RECO/IDEAL_V11_redigi_v10/0000/121F3706-C4FC-DD11-A0DD-00E08178C091.root'
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 1000 )   # Reduce number of events for testing.
)

process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = cms.string( 'IDEAL_V11::All' )
process.load( "Configuration.StandardSequences.MagneticField_cff" )

# PAT itself
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )

process.p = cms.Path(
    process.patDefaultSequenceNoCleaning
)
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOffTriggerMatchingOld( process )

# Output module configuration
from PhysicsTools.PatAlgos.patEventContent_cff import *
process.out = cms.OutputModule( "PoolOutputModule",
    fileName       = cms.untracked.string( 'edmTrigger.root' ),
    SelectEvents   = cms.untracked.PSet(
        SelectEvents = cms.vstring( 'p' )
    ),
    outputCommands = cms.untracked.vstring( 'drop *', *patEventContentNoLayer1Cleaning ) # <== check on this
)

process.outpath = cms.EndPath(
    process.out
)

### PAT trigger ###
# The new matcher module to be completed
process.muonTriggerMatchHLTMuons = cms.EDFilter( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "selectedLayer1Muons" ),             # <== missing piece
    matched = cms.InputTag( "patTrigger" ),
    andOr          = cms.bool( False ),
    filterIdsEnum  = cms.vstring( 'TriggerMuon' ),       # <== missing piece
    filterIds      = cms.vuint32( 93 ),       # <== missing piece
    filterLabels   = cms.vstring( '*' ),
    pathNames      = cms.vstring( 'HLT_Mu3' ),       # <== missing piece
    collectionTags = cms.vstring( '*' ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( True )
)
# This adds the new matcher module to the according sequence:
process.patTriggerMatcher += process.muonTriggerMatchHLTMuons
# And this throughs out the rest:
process.patTriggerMatcher.remove( process.patTriggerElectronMatcher )
process.patTriggerMatcher.remove( process.patTriggerMuonMatcher )
process.patTriggerMatcher.remove( process.patTriggerTauMatcher )
# Here is something left to be done (PAT trigger event & Python tool):
process.patTriggerEvent.patTriggerMatches = [ "muonTriggerMatchHLTMuons" ]
switchOnTrigger( process )
