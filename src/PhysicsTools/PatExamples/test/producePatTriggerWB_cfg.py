# Start with pre-defined skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *
process.maxEvents.input     = 1000 # Reduce number of events for testing.
process.out.fileName        = 'edmPatTriggerWB.root'
process.options.wantSummary = False # to suppress the long output at the end of the job

# Load the standard PAT config
process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
process.p = cms.Path(
    process.patDefaultSequence
)

### PAT trigger ###
process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff" )

## Define own trigger match ##
process.muonTriggerMatchHLTMuons = cms.EDFilter( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "cleanLayer1Muons" ),
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

## Execute own trigger match and remove other from path ##

# Add own trigger match to path #
process.patTriggerMatcher += process.muonTriggerMatchHLTMuons

# Remove anything else as found in PhysicsTools/PatAlgos/python/triggerLayer1/triggerMatcher_cfi.py
process.patTriggerMatcher.remove( process.patTriggerElectronMatcher )
process.patTriggerMatcher.remove( process.patTriggerMuonMatcher )
process.patTriggerMatcher.remove( process.patTriggerTauMatcher )

# Put own trigger match to TriggerEvent #
process.patTriggerEvent.patTriggerMatches = [ "muonTriggerMatchHLTMuons" ]

# Switch everything on #
from PhysicsTools.PatAlgos.tools.trigTools import switchOnTrigger
switchOnTrigger( process )
