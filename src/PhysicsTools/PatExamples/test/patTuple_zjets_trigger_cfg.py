## ---
## Start with pre-defined skeleton process
## ---
from PhysicsTools.PatAlgos.patTemplate_cfg import *
## ... and modify it according to the needs
from PhysicsTools.PatExamples.samplesFNAL_cff import zjetsRECO
process.source.fileNames    = zjetsRECO
process.maxEvents.input     = -1 # reduce number of events for testing.
process.out.fileName        = '/uscms_data/d3/vadler/PATTutorial/Nov10/patTuple_zjets_trigger.root'
process.out.outputCommands += [
  "keep *_globalMuons_*_*"
, "keep *_generalTracks_*_*"
, "keep *_standAloneMuons_*_*"
]
# for the tag'n'probe analysis
process.patMuons.embedStandAloneMuon = False
process.patMuons.embedCombinedMuon   = False
process.patMuons.embedTrack          = False
process.selectedPatMuons.cut         = 'isTrackerMuon=1 & isGlobalMuon=1 & innerTrack.numberOfValidHits>=11 & globalTrack.normalizedChi2<10.0  & globalTrack.hitPattern.numberOfValidMuonHits
>0 & abs(dB)<0.02 & (trackIso+caloIso)/pt<0.05'

## ---
## Define the path
## ---
process.p = cms.Path(
  process.patDefaultSequence
)

## ---
## PAT trigger matching
## --
process.muonTriggerMatchHLTMuons = cms.EDProducer(
  "PATTriggerMatcherDRLessByR"
, src     = cms.InputTag( 'cleanPatMuons' )
, matched = cms.InputTag( 'patTrigger' )
, andOr          = cms.bool( False )
, filterIdsEnum  = cms.vstring( 'TriggerMuon' )
, filterIds      = cms.vint32( 0 )
, filterLabels   = cms.vstring( '*' )
, pathNames      = cms.vstring( 'HLT_Mu9' )
, collectionTags = cms.vstring( '*' )
, maxDPtRel   = cms.double( 0.5 ) # no effect here
, maxDeltaR   = cms.double( 0.5 )
, maxDeltaEta = cms.double( 0.2 ) # no effect here
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( True )
)

## --
## Switch on PAT trigger
## --
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTriggerMatching( process, triggerMatchers = [ 'muonTriggerMatchHLTMuons' ] )
