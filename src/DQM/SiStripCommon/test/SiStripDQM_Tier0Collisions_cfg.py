import FWCore.ParameterSet.Config as cms

# Process
process = cms.Process( "T0Collisions" )

process.load( "FWCore.MessageService.MessageLogger_cfi" )

process.load( "Configuration.StandardSequences.Services_cff" )
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.globaltag = 'GR09_R_34X_V2::All'
process.load( "Configuration.StandardSequences.GeometryIdeal_cff" )
process.load( "Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff" )

process.load( "Configuration.StandardSequences.RawToDigi_Data_cff" )
process.load( "Configuration.StandardSequences.Reconstruction_cff" )
# skip events with HV off
process.fourthPLSeeds.ClusterCheckPSet.MaxNumberOfCosmicClusters = 20000
process.fifthSeeds.ClusterCheckPSet.MaxNumberOfCosmicClusters    =  5000
# PV overrides
process.offlinePrimaryVerticesWithBS.PVSelParameters.maxDistanceToBeam    =   2
process.offlinePrimaryVerticesWithBS.TkFilterParameters.maxNormalizedChi2 =  20
process.offlinePrimaryVerticesWithBS.TkFilterParameters.minSiliconHits    =   6
process.offlinePrimaryVerticesWithBS.TkFilterParameters.maxD0Significance = 100
process.offlinePrimaryVerticesWithBS.TkFilterParameters.minPixelHits      =   1
process.offlinePrimaryVerticesWithBS.TkClusParameters.zSeparation         =  10
process.offlinePrimaryVertices.PVSelParameters.maxDistanceToBeam          =   2
process.offlinePrimaryVertices.TkFilterParameters.maxNormalizedChi2       =  20
process.offlinePrimaryVertices.TkFilterParameters.minSiliconHits          =   6
process.offlinePrimaryVertices.TkFilterParameters.maxD0Significance       = 100
process.offlinePrimaryVertices.TkFilterParameters.minPixelHits            =   1
process.offlinePrimaryVertices.TkClusParameters.zSeparation               =  10
process.load( "DQMOffline.Configuration.DQMOffline_cff" )
process.load( "DQMOffline.Configuration.DQMOffline_SecondStep_cff" )
process.DQMStore.referenceFileName = ''
process.DQMStore.collateHistograms = False
process.dqmSaver.convention = 'Offline'
process.dqmSaver.workflow   = '/MinimumBias/CMSSW_3_5_0_pre3/RECO'
process.dqmSaver.dirName    = '/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_3_5_0_pre3/output'
# process.load( "DQMServices.Components.DQMDaqInfo_cfi" )
process.load( "DQMOffline.Configuration.DQMOffline_Certification_cff" )

# Input
process.source = cms.Source( "PoolSource",
  fileNames = cms.untracked.vstring(
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/FA54A056-42E2-DE11-A6DB-001617E30D40.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/E8477DE8-38E2-DE11-9DAB-0016177CA778.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/E2EFCB1E-3FE2-DE11-9378-001D09F2438A.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/DC3FB2D7-41E2-DE11-8DF0-0019B9F72BAA.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/D6AB55BC-3AE2-DE11-8F92-000423D999CA.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/CEA3B783-49E2-DE11-AEED-001617C3B6DE.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/BCE2ACEE-39E2-DE11-B4E4-001D09F2A465.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/BC0F7A9A-39E2-DE11-A501-000423D99660.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/B8A665B2-5CE2-DE11-9783-0019B9F730D2.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/B6BA8FF3-5CE2-DE11-855D-001D09F29597.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/ACA7031A-3FE2-DE11-9811-001D09F248F8.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/A64AF18E-5DE2-DE11-B0FA-0019B9F72D71.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/A07C7B62-4CE2-DE11-8E7E-000423D174FE.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/9C4F961D-52E2-DE11-9F99-0030487A18F2.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/9885B454-4DE2-DE11-8190-001D09F23944.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/987D20CF-4BE2-DE11-B5D6-001D09F24934.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/8EFA9549-43E2-DE11-BEDB-001617C3B79A.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/868D6A6E-41E2-DE11-BF09-000423D98EA8.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/8452FFCE-74E2-DE11-A170-001D09F29146.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/8425696B-55E2-DE11-8D3B-001D09F24024.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/7A739F80-57E2-DE11-A39C-000423D94990.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/7458DD23-56E2-DE11-ABF6-000423D98DC4.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/6E57E243-3DE2-DE11-8FB2-000423D6B42C.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/6C67F1D6-41E2-DE11-ABFE-001D09F24489.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/6A136363-58E2-DE11-AD2A-001D09F242EA.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/60ADABBE-56E2-DE11-98CB-001D09F23174.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/601B7AF5-5BE2-DE11-95B1-001617C3B6C6.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/601535FC-54E2-DE11-8FC5-000423D98804.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/58A1705E-4CE2-DE11-B7DE-000423D9A2AE.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/56A84451-58E2-DE11-893D-0019B9F72BAA.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/409C06BC-5CE2-DE11-AABD-001617E30D12.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/3E05D418-44E2-DE11-ADB2-000423D6B42C.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/3C806072-49E2-DE11-9A17-001D09F28F25.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/2EBD6495-39E2-DE11-A0B3-000423D99394.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/26444E28-48E2-DE11-AC11-001D09F28F25.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/24870114-40E2-DE11-9452-001D09F2A465.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/144987C2-36E2-DE11-925D-0030487A18F2.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/10E72957-42E2-DE11-B6D2-000423D6CA42.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/0CD30995-46E2-DE11-90D3-001D09F24934.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/08B352CB-54E2-DE11-86E4-003048D3750A.root',
    '/store/data/BeamCommissioning09/MinimumBias/RAW/v1/000/123/596/08220761-4BE2-DE11-8FF0-0016177CA7A0.root'
  ),
  skipEvents = cms.untracked.uint32( 0 )
)
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32( 100 )
)
process.options = cms.untracked.PSet(
  Rethrow     = cms.untracked.vstring( 'ProductNotFound' ),
  fileMode    = cms.untracked.string( 'FULLMERGE' ),
  wantSummary = cms.untracked.bool( False )
)
process.physicsBitSelector = cms.EDFilter( "PhysDecl",
  applyfilter = cms.untracked.bool( True ) ,
  debugOn     = cms.untracked.bool( False )
)
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
process.load('HLTrigger.HLTfilters.hltLevel1GTSeed_cfi')
process.hltLevel1GTSeed.L1TechTriggerSeeding     = cms.bool(True)
process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string('0 AND ( 40 OR 41 ) AND NOT ( 36 OR 37 OR 38 OR 39 )')

process.l1GT = cms.Sequence(
  process.gtDigis *
  process.hltLevel1GTSeed
)
process.raw2Digi = cms.Sequence(
  process.siPixelDigis +
  process.siStripDigis
)
process.reco = cms.Sequence(
  process.trackerlocalreco   *
  process.offlineBeamSpot    *
  process.recopixelvertexing *
  process.ckftracks          *
  process.vertexreco         *
  process.ctfTracksPixelLess *
  process.logErrorHarvester
)
process.dqm = cms.Sequence(
  process.SiStripDQMTier0          *
  process.siPixelOfflineDQM_source *
  process.DQMMessageLogger
)

process.path = cms.Path(
#   process.l1GT               *
#   process.physicsBitSelector *
  process.raw2Digi           *
  process.reco               *
  process.dqm                *
  process.dqmRefHistoRootFileGetter *
  process.SiStripOfflineDQMClient   *
  process.sipixelEDAClient          *
  process.DQMMessageLoggerClient    *
  process.siStripDaqInfo           *
  process.siPixelDaqInfo           *
  process.siStripDcsInfo           *
  process.siPixelDcsInfo           *
  process.siStripCertificationInfo *
  process.siPixelCertification     *
  process.dqmSaver
)
