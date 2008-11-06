import FWCore.ParameterSet.Config as cms

from EventFilter.SiStripRawToDigi.SiStripDigis_cfi import *
siStripDigis.ProductLabel = 'source'
from RecoLocalTracker.SiStripZeroSuppression.SiStripZeroSuppression_cfi import *
from RecoLocalTracker.SiStripClusterizer.SiStripClusterizer_cfi import *

siStripDQMRecoBadStrips = cms.Sequence(
    siStripDigis           *
    siStripZeroSuppression *
    siStripClusters
)
