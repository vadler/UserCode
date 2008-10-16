import FWCore.ParameterSet.Config as cms

### DQM Services ###

## DQM store and related ##
from DQMServices.Core.DQM_cfg import *
DQM.collectorHost = ''
from DQMServices.Components.MEtoEDMConverter_cfi import *
from DQMServices.Components.EDMtoMEConverter_cfi import *

##  DQM file saving and environment ##
from DQMServices.Components.DQMEnvironment_cfi import *
# File saving #
dqmSaver.convention        = 'Online'
dqmSaver.dirName           = '.'
dqmSaver.producer          = 'DQM'
dqmSaver.saveByRun         = 1
dqmSaver.saveAtJobEnd      = True
dqmSaver.referenceHandling = 'qtests'
# Environment #
dqmEnv.subSystemFolder = 'SiStrip'

## Quality tester ##
qTester = cms.EDAnalyzer("QualityTester",
    qtList                  = cms.untracked.FileInPath( 'DQM/SiStripMonitorClient/data/sistrip_qualitytest_config.xml' ),
    getQualityTestsFromFile = cms.untracked.bool( True ),
    prescaleFactor          = cms.untracked.int32( 1 )
)

### SiStrip DQM Modules ###

## SiStrip DQM reconstruction ##
from DQM.SiStripMonitorClient.SiStripDQMRecoConfigOfflineGlobalRunCAF_cfi   import *
## SiStrip DQM sources ##
from DQM.SiStripMonitorClient.SiStripDQMSourceConfigOfflineGlobalRunCAF_cfi import *
## SiStrip DQM client ##
from DQM.SiStripMonitorClient.SiStripDQMClientConfigOfflineGlobalRunCAF_cfi import *
