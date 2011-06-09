import FWCore.ParameterSet.Config as cms

### Steering

runOnMC   = False
runMatch  = True
runCiC    = True
jetAlgo   = 'AK5'
jecLevels = [ 'L1FastJet', 'L2Relative', 'L3Absolute', ]

process = cms.Process( 'PF2PAT' )

### Logging

process.load( "FWCore.MessageService.MessageLogger_cfi" )
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options = cms.untracked.PSet(
  wantSummary = cms.untracked.bool( True )
)
process.Timing = cms.Service( "Timing"
, summaryOnly = cms.untracked.bool( True )
)

### Conditions

process.load( "Configuration.StandardSequences.Geometry_cff" )
process.load( "Configuration.StandardSequences.MagneticField_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
if runOnMC:
  process.GlobalTag.globaltag = 'START42_V12::All'
else:
  process.GlobalTag.globaltag = 'GR_R_42_V12::All'

### Input

process.source = cms.Source( "PoolSource"
, noEventSort        = cms.untracked.bool( True )
, duplicateCheckMode = cms.untracked.string( 'noDuplicateCheck' )
, fileNames          = cms.untracked.vstring()
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( -1 )
)

from PhysicsTools.PatAlgos.tools.cmsswVersionTools import pickRelValInputFiles
if runOnMC:
  process.source.fileNames = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_3'
                                                 , globalTag     = 'START42_V12'
                                                 )
else:
  process.source.fileNames = pickRelValInputFiles( cmsswVersion  = 'CMSSW_4_2_3'
                                                 , relVal        = 'Mu'
                                                 , dataTier      = 'RECO'
                                                 , globalTag     = 'GR_R_42_V12_mu2010B'
                                                 )

### Output

from PhysicsTools.PatAlgos.patEventContent_cff import patEventContent
process.out = cms.OutputModule( "PoolOutputModule"
, fileName       = cms.untracked.string( 'gentPatSkimPF2PAT.root' )
, SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring( 'p' ) )
, outputCommands = cms.untracked.vstring( 'drop *', *patEventContent )
, dropMetaData   = cms.untracked.string( 'ALL' )
)

# Outpath
process.outpath = cms.EndPath(
  process.out
)

### Cleaning

# HBHE noise filter
process.load( "CommonTools.RecoAlgos.HBHENoiseFilter_cfi" )
process.HBHENoiseFilter.minIsolatedNoiseSumE        = 999999.
process.HBHENoiseFilter.minNumIsolatedNoiseChannels = 999999
process.HBHENoiseFilter.minIsolatedNoiseSumEt       = 999999.

# Scraping filter
process.scrapingFilter = cms.EDFilter( "FilterOutScraping"
, applyfilter = cms.untracked.bool( True )
, debugOn     = cms.untracked.bool( False )
, numtrack    = cms.untracked.uint32( 10 )
, thresh      = cms.untracked.double( 0.25 )
)

process.eventCleaning = cms.Sequence(
  process.HBHENoiseFilter
+ process.scrapingFilter
)

### PAT

process.load( "PhysicsTools.PatAlgos.patSequences_cff" )
from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
usePF2PAT( process
         , runOnMC        = runOnMC
         , jetAlgo        = jetAlgo
         , jetCorrections = ( jetAlgo + 'PFchs'
                            , jecLevels
                            )
         )
process.patDefaultSequence.remove( process.patPFParticles )
process.patDefaultSequence.remove( process.selectedPatPFParticles )
process.patDefaultSequence.remove( process.countPatPFParticles )

# Muons
process.isoValMuonWithCharged.deposits.delta = 0.3
process.isoValMuonWithNeutral.deposits.delta = 0.3
process.isoValMuonWithPhotons.deposits.delta = 0.3
process.patMuons.embedTrack = True
#process.selectedPatMuons.cut = 'isGlobalMuon && pt > 10. && abs(eta) < 2.5' # RefSel (min. for veto)
process.selectedPatMuons.cut = 'isGlobalMuon && pt > 8. && abs(eta) < 2.7'

# Electrons
process.isoValElectronWithCharged.deposits.delta = 0.3
process.isoValElectronWithNeutral.deposits.delta = 0.3
process.isoValElectronWithPhotons.deposits.delta = 0.3
process.patElectrons.embedTrack = True
#process.selectedPatElectrons.cut = 'et > 15. && abs(eta) < 2.5' # RefSel (min. for veto)
process.selectedPatElectrons.cut = 'et > 12. && abs(eta) < 2.7'
if runCiC:
  process.load( "RecoEgamma.ElectronIdentification.cutsInCategoriesElectronIdentificationV06_cfi" )
  process.eidCiCSequence = cms.Sequence(
    process.eidVeryLooseMC
  + process.eidLooseMC
  + process.eidMediumMC
  + process.eidTightMC
  + process.eidSuperTightMC
  + process.eidHyperTight1MC
  )

# Jets
if 'L1FastJet' in jecLevels:
  process.pfPileUp.checkClosestZVertex = False
  process.pfJets.doAreaFastjet = True
  process.pfJets.doRhoFastjet  = False
  process.load( "RecoJets.Configuration.RecoPFJets_cff" )
  process.kt6PFJets.src = cms.InputTag( 'pfNoElectron' )
  process.patDefaultSequence.replace( process.patJetCorrFactors
                                    , process.kt6PFJets * process.patJetCorrFactors
                                    )
process.patJets.embedCaloTowers   = False
process.patJets.embedPFCandidates = False
#process.selectedPatJets.cut = 'pt > 30. && abs(eta) < 2.4' # RefSel
process.selectedPatJets.cut = 'pt > 20. && abs(eta) < 2.7'

# Misc
from PhysicsTools.PatAlgos.tools.coreTools import *
if not runOnMC:
  runOnData( process
           , names = [ 'PFAll' ]
           )
elif not runMatch:
  removeMCMatching( process
                  , names = [ 'PFAll' ]
                  )
else:
  process.patJets.addGenJetMatch = False
removeSpecificPATObjects( process
                        , names = [ 'Photons', 'Taus' ]
                        )
# The following need to be fixed _after_ the (potential) calls to 'removeSpecificPATObjects()' and 'runOnData()'
process.patJetCorrFactors.payload = jetAlgo + 'PF'
process.patJetCorrFactors.levels  = jecLevels
process.out.outputCommands += [ 'drop recoGenJets_*_*_*'
                              , 'drop recoBaseTagInfosOwned_*_*_*'
                              , 'drop CaloTowers_*_*_*'
                              , 'keep edmTriggerResults_*_*_*'
                              , 'drop edmTriggerResults_*_*_*RECO*'
                              , 'drop edmTriggerResults_*_*_*NONE*'
                              , 'keep *_hltTriggerSummaryAOD_*_*'
                              , 'keep *_offlineBeamSpot_*_*'
                              , 'keep *_offlinePrimaryVertices_*_*'
                              ]
if runOnMC and not runMatch:
  process.out.outputCommands += [ 'keep recoGenParticles_*_*_*'
                                , 'keep recoGenJets_' + jetAlgo.lower() + 'GenJets_*_*'
                                #, 'keep GenEventInfoProduct_*_*_*'
                                ]
if 'L1FastJet' in jecLevels:
  process.out.outputCommands += [ 'keep double_kt6PFJets_*_' + process.name_() ]

### Path
process.p = cms.Path( process.eventCleaning )
if runCiC:
  process.p *= process.eidCiCSequence
process.p *= process.patPF2PATSequence
