# Start with pre-defined skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *

## ---
## Use proper input
## ---
from PhysicsTools.PatExamples.samplesCERN_cff import *
process.source.fileNames = zjetsRECO

## ---
## Determine number of events to be processed
## ---
process.maxEvents.input = 1000

## ---
## Define the path
## ---
process.p = cms.Path(
    process.patDefaultSequence
)

process.options.wantSummary = False # to suppress the long output at the end of the job

process.selectedPatMuons.cut = "isTrackerMuon==1 & isGlobalMuon==1 & innerTrack.numberOfValidHits>=11 & globalTrack.normalizedChi2<10.0  & globalTrack.hitPattern.numberOfValidMuonHits>0 & abs(dB)<0.02 & (trackIso+caloIso)/pt<0.05"

## ---
## PAT trigger matching
## --
process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff" )
process.muonTriggerMatchHLTMuons = cms.EDProducer( "PATTriggerMatcherDRLessByR",
    src     = cms.InputTag( "cleanPatMuons" ),
    matched = cms.InputTag( "patTrigger" ),
    andOr          = cms.bool( False ),
    filterIdsEnum  = cms.vstring( 'TriggerMuon' ),
    filterIds      = cms.vint32( 0 ),
    filterLabels   = cms.vstring( '*' ),
    pathNames      = cms.vstring( 'HLT_Mu9' ),
    collectionTags = cms.vstring( '*' ),
    maxDPtRel = cms.double( 0.5 ),
    maxDeltaR = cms.double( 0.5 ),
    resolveAmbiguities    = cms.bool( True ),
    resolveByMatchQuality = cms.bool( True )
)

### ============
### Python tools
### ============
### Attention: order matters

## --
## Switch to selected PAT objects in the main work flow
## --
from PhysicsTools.PatAlgos.tools.coreTools import removeCleaning
removeCleaning( process )

## --
## Switch on PAT trigger
## --
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process ) # This is optional and can be omitted.
switchOnTriggerMatching( process, ['muonTriggerMatchHLTMuons'] )
# Switch to selected PAT objects in the trigger matching
removeCleaningFromTriggerMatching( process )
