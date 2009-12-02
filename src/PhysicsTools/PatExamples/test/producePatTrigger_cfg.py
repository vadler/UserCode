# Start with pre-defined skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *

# Switch to selected PAT objects
from PhysicsTools.PatAlgos.tools.coreTools import removeCleaning
removeCleaning( process )

# Define the path
process.p = cms.Path(
    process.patDefaultSequence
)

process.maxEvents.input     = 1000 # Reduce number of events for testing.
process.out.fileName        = 'edmPatTrigger.root'
process.options.wantSummary = False # to suppress the long output at the end of the job

# PAT trigger
process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff" )
process.muonTriggerMatchHLTMuons = cms.EDFilter( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "selectedLayer1Muons" ),
    matched = cms.InputTag( "patTrigger" ),
    andOr          = cms.bool( False ),
    filterIdsEnum  = cms.vstring( 'TriggerMuon' ),
    filterIds      = cms.vint32( 0 ),
    filterLabels   = cms.vstring( '*' ),
    pathNames      = cms.vstring( '*' ),
    collectionTags = cms.vstring( '*' ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( True )
)
process.patTriggerMatcher += process.muonTriggerMatchHLTMuons
process.patTriggerMatcher.remove( process.patTriggerElectronMatcher )
process.patTriggerMatcher.remove( process.patTriggerMuonMatcher )
process.patTriggerMatcher.remove( process.patTriggerTauMatcher )
process.patTriggerEvent.patTriggerMatches = [ "muonTriggerMatchHLTMuons" ]
from PhysicsTools.PatAlgos.tools.trigTools import switchOnTrigger
switchOnTrigger( process )
