import FWCore.ParameterSet.Config as cms

process = cms.Process( "SiStripDQMBadStrips" )

### Miscellanous ###

## Logging ##

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

## Profiling ##

# # Performance #
#
# process.ProfilerService = cms.Service( "ProfilerService",
#     paths = cms.untracked.vstring(
#         'FullEvent'
#     )
# )

# Memory #

process.SimpleMemoryCheck = cms.Service( "SimpleMemoryCheck",
#     oncePerEventMode = cms.untracked.bool( True ),
    ignoreTotal      = cms.untracked.int32( 0 )
)

### Import ###

## Magnetic fiels ##

process.load( "Configuration.StandardSequences.MagneticField_38T_cff" )

## Geometry ##

process.load( "Configuration.StandardSequences.Geometry_cff" )

## Calibration ##

# Global tag #
 
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.connect   = 'frontier://FrontierProd/CMS_COND_21X_GLOBALTAG'
process.GlobalTag.globaltag = 'CRAFT_V4P::All'
process.es_prefer_GlobalTag = cms.ESPrefer( 'PoolDBESSource', 'GlobalTag' )

# Exclude masking #

from CalibTracker.SiStripESProducers.SiStripQualityESProducer_cfi import *
siStripQualityESProducer.ListOfRecordToMerge = cms.VPSet(
    cms.PSet( record = cms.string( 'SiStripDetCablingRcd' ), tag = cms.string( '' ) ),
    cms.PSet( record = cms.string( 'SiStripBadChannelRcd' ), tag = cms.string( '' ) )
)

### SiStrip DQM ###

process.load( "DQM.SiStripMonitorClient.SiStripDQMBadStrips_cff" )

### Input ###

## PoolSource ## 

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/data/Commissioning08/RandomTriggers/RAW/v1/000/068/665/422164BB-FEA8-DD11-85E7-001D09F24E39.root',
        '/store/data/Commissioning08/RandomTriggers/RAW/v1/000/068/665/486C391B-00A9-DD11-807C-001D09F24498.root',
        '/store/data/Commissioning08/RandomTriggers/RAW/v1/000/068/665/5AD8595F-04A9-DD11-8627-0030487A1FEC.root',
        '/store/data/Commissioning08/RandomTriggers/RAW/v1/000/068/665/90E5BCC7-1DA9-DD11-813C-001617C3B70E.root',
        '/store/data/Commissioning08/RandomTriggers/RAW/v1/000/068/665/BC010482-01A9-DD11-AC2D-001D09F28EA3.root',
        '/store/data/Commissioning08/RandomTriggers/RAW/v1/000/068/665/F8554B2C-07A9-DD11-A5AC-001D09F244BB.root'
    ),
    skipEvents = cms.untracked.uint32(0)
)

## Input steering ##

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( -1 )
)

### Output ###

## DQM ##

# DQM Saver path #

process.dqmSaver.dirName = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_TRACKER/DQM/SiStrip/jobs/output'

### Scheduling ###

## Paths ##

# DQM path #

process.p = cms.Path(
    process.siStripDQMRecoBadStrips   *
    process.siStripDQMSourceBadStrips *
    process.dqmSaver
)
