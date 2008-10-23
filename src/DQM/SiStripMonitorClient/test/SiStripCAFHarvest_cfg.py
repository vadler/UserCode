import FWCore.ParameterSet.Config as cms

process = cms.Process( "SiStripDQMConvertOfflineGlobalRun" )

### Miscellanous ###

process.options = cms.untracked.PSet(
   fileMode    = cms.untracked.string( 'FULLMERGE' ),
   wantSummary = cms.untracked.bool( True )
)

# Logging #
process.MessageLogger = cms.Service( "MessageLogger",
    destination = cms.untracked.vstring(
        'cout'
    ),
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string( 'INFO' )
    )
)

# # Profiling #
# process.ProfilerService = cms.Service( "ProfilerService",
#     paths = cms.untracked.vstring(
#         'FullEvent'
#     )
# )

# Memory check #
process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck",
#     oncePerEventMode = cms.untracked.bool( True ),
    ignoreTotal      = cms.untracked.int32( 0 )
)

### Import ###

# Magnetic fiels #
process.load( "Configuration.StandardSequences.MagneticField_0T_cff" )
# Geometry #
process.load( "Configuration.StandardSequences.Geometry_cff" )
# Calibration 
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.connect   = 'frontier://PromptProd/CMS_COND_21X_GLOBALTAG'
process.GlobalTag.globaltag = 'CRUZET4_V6P::All'
process.es_prefer_GlobalTag = cms.ESPrefer( 'PoolDBESSource', 'GlobalTag' )

### Input ###

# Source #
process.source = cms.Source( "PoolSource",
    processingMode = cms.untracked.string( 'Runs' ),
    fileNames      = cms.untracked.vstring(
        'rfio:/castor/cern.ch/user/v/vadler/cms/SiStripDQM/OfflineDQM/test/SiStripDQMOfflineGlobalRunCAF_1.root',
        'rfio:/castor/cern.ch/user/v/vadler/cms/SiStripDQM/OfflineDQM/test/SiStripDQMOfflineGlobalRunCAF_2.root',
        'rfio:/castor/cern.ch/user/v/vadler/cms/SiStripDQM/OfflineDQM/test/SiStripDQMOfflineGlobalRunCAF_3.root',
        'rfio:/castor/cern.ch/user/v/vadler/cms/SiStripDQM/OfflineDQM/test/SiStripDQMOfflineGlobalRunCAF_4.root',
        'rfio:/castor/cern.ch/user/v/vadler/cms/SiStripDQM/OfflineDQM/test/SiStripDQMOfflineGlobalRunCAF_5.root',
        'rfio:/castor/cern.ch/user/v/vadler/cms/SiStripDQM/OfflineDQM/test/SiStripDQMOfflineGlobalRunCAF_6.root',
        'rfio:/castor/cern.ch/user/v/vadler/cms/SiStripDQM/OfflineDQM/test/SiStripDQMOfflineGlobalRunCAF_7.root',
        'rfio:/castor/cern.ch/user/v/vadler/cms/SiStripDQM/OfflineDQM/test/SiStripDQMOfflineGlobalRunCAF_8.root',
        'rfio:/castor/cern.ch/user/v/vadler/cms/SiStripDQM/OfflineDQM/test/SiStripDQMOfflineGlobalRunCAF_9.root',
        'rfio:/castor/cern.ch/user/v/vadler/cms/SiStripDQM/OfflineDQM/test/SiStripDQMOfflineGlobalRunCAF_10.root'
    )
)
# Input steering #
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( -1 )
)

### SiStrip DQM ###

process.load( "DQM.SiStripMonitorClient.SiStripDQMOfflineGlobalRunCAF_cff" )

### Output ###

# DQM store #
# process.DQMStore.referenceFileName = ''
# process.DQMStore.collateHistograms = False
# process.DQMStore.verbose = 1

# EDM2ME #
# process.EDMtoMEConverter.convertOnEndLumi = False
# process.EDMtoMEConverter.convertOnEndRun  = True

# DQM saver #
process.dqmSaver.dirName = '/afs/cern.ch/user/v/vadler/scratch0/cms/SiStripDQM/CMSSW_2_1_10/output'

### Scheduling ###

process.p = cms.Path(
    process.EDMtoMEConverter        *
    process.SiStripOfflineDQMClient *
    process.qTester                 *
    process.dqmSaver
)
