import FWCore.ParameterSet.Config as cms

## DQM Services

# DQM store
from DQMServices.Core.DQM_cfg import *
DQM.collectorHost = ''

# DQM file saving and environment
from DQMServices.Components.DQMEnvironment_cfi import *
# File saving
dqmSaver.convention        = 'Online'
dqmSaver.dirName           = '.'
dqmSaver.producer          = 'DQM'
dqmSaver.saveByRun         = 1
dqmSaver.saveAtJobEnd      = True
dqmSaver.referenceHandling = 'qtests'
# Environment
dqmEnv.subSystemFolder = 'SiStrip'

### SiStrip DQM Modules

## SiStrip DQM reconstruction
from DQM.SiStripMonitorClient.SiStripDQMRecoConfigBadStrips_cfi   import *
## SiStrip DQM sources
from DQM.SiStripMonitorClient.SiStripDQMSourceConfigBadStrips_cfi import *
