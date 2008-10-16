import FWCore.ParameterSet.Config as cms

process = cms.Process("CAFHarvestingJob")

### Miscellanous ###

# Logging #
process.options = cms.untracked.PSet(
   wantSummary = cms.untracked.bool( True )
)
process.MessageLogger = cms.Service( "MessageLogger",
    destinations = cms.untracked.vstring(
        'cout'
    ),
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string( 'INFO' )
    )
)

# Profiling #
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

#-------------------------------------------------
## Empty Event Source
#-------------------------------------------------
process.source = cms.Source("EmptyIOVSource",
    lastValue = cms.uint64(62815),
    timetype = cms.string('runnumber'),
    firstValue = cms.uint64(62815),
    interval = cms.uint64(1)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

#-------------------------------------------------
# DQM Services
#-------------------------------------------------

process.DQMStore = cms.Service("DQMStore",
    referenceFileName = cms.untracked.string(''),
    verbose = cms.untracked.int32(1)
)

process.qTester = cms.EDFilter("QualityTester",
    qtestOnEndJob = cms.untracked.bool(True),
    qtList = cms.untracked.FileInPath('DQM/SiStripMonitorClient/data/sistrip_qualitytest_config.xml'),
    QualityTestPrescaler = cms.untracked.int32(1),
    getQualityTestsFromFile = cms.untracked.bool(True)
)

#-------------------------------------------------
## DQM Harvesting
#-------------------------------------------------
process.dqmHarvesing = cms.EDFilter("SiStripOfflineDQM",
    CreateSummary = cms.untracked.bool(True),
    InputFileName = cms.untracked.string('/afs/cern.ch/user/v/vadler/scratch0/cms/SiStripDQM/CMSSW_2_1_10/output/DQM_V0001_SiStrip_R000062815-standAlone.root'),
    OutputFileName = cms.untracked.string('/afs/cern.ch/user/v/vadler/scratch0/cms/SiStripDQM/CMSSW_2_1_10/output/DQM_V0001_SiStrip_R000062815.root'),
    GlobalStatusFilling = cms.untracked.int32(1)
)

#-------------------------------------------------
## Scheduling
#-------------------------------------------------
process.p = cms.Path(process.qTester*process.dqmHarvesing)

