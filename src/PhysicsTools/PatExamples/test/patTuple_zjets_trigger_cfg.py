import os
## ---
## Start with pre-defined skeleton process
## ---
from PhysicsTools.PatAlgos.patTemplate_cfg import *
## ... and modify it according to the needs
from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.source.fileNames    = pickRelValInputFiles( relVal    = 'RelValZMM'
                                                  , globalTag = 'START42_V12'
                                                  )
process.maxEvents.input     = -1 # reduce number of events for testing.
process.out.fileName        = '/tmp/%s/patTuple_zjets_trigger.root'%( os.getenv( "USER" ) )
process.out.outputCommands += [ "keep *_globalMuons_*_*"
                              , "keep *_generalTracks_*_*"
                              , "keep *_standAloneMuons_*_*"
                              ]
process.patJetCorrFactors.useRho = False
# for the tag'n'probe analysis
process.patMuons.embedStandAloneMuon = False
process.patMuons.embedCombinedMuon   = False
process.patMuons.embedTrack          = False
process.selectedPatMuons.cut         = 'isTrackerMuon=1 & isGlobalMuon=1 & innerTrack.numberOfValidHits>=11 & globalTrack.normalizedChi2<10.0  & globalTrack.hitPattern.numberOfValidMuonHits>0 & abs(dB)<0.02 & (trackIso+caloIso)/pt<0.05'

## ---
## Define the path
## ---
process.p = cms.Path(
  process.patDefaultSequence
)

### ========
### Plug-ins
### ========

## ---
## PAT trigger matching
## --
process.muonTriggerMatchHLTMuons = cms.EDProducer(
  # matching in DeltaR, sorting by best DeltaR
  "PATTriggerMatcherDRLessByR"
  # matcher input collections
, src     = cms.InputTag( 'cleanPatMuons' )
, matched = cms.InputTag( 'patTrigger' )
  # selections of trigger objects
, matchedCuts = cms.string( 'type( "TriggerMuon" ) && path( "HLT_IsoMu17_v*", 1, 0 )' )
  # selection of matches
, maxDPtRel   = cms.double( 0.5 ) # no effect here
, maxDeltaR   = cms.double( 0.5 )
, maxDeltaEta = cms.double( 0.2 ) # no effect here
  # definition of matcher output
, resolveAmbiguities    = cms.bool( True )
, resolveByMatchQuality = cms.bool( True )
)

### ============
### Python tools
### ============

## --
## Switch on PAT trigger
## --
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTriggerMatching( process, triggerMatchers = [ 'muonTriggerMatchHLTMuons' ] )
