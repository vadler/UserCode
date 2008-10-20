import FWCore.ParameterSet.Config as cms

process = cms.Process( "SiStripDQMOfflineGlobalRunCAF" )

process.MessageLogger = cms.Service( "MessageLogger",
    destinations = cms.untracked.vstring(
        'cout'
    ),
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string( 'ERROR' )
    )
)

# Magnetic Field
process.load( "Configuration.StandardSequences.MagneticField_xMAG_FIELDx_cff" )

# Geometry
process.load( "Configuration.StandardSequences.Geometry_cff" )

# Calibration 
process.load( "Configuration.StandardSequences.FrontierConditions_GlobalTag_cff" )
process.GlobalTag.connect   = "frontier://PromptProd/CMS_COND_21X_GLOBALTAG"
process.GlobalTag.globaltag = "CRUZET4_V6P::All"
process.es_prefer_GlobalTag = cms.ESPrefer('PoolDBESSource','GlobalTag')

# SiStrip DQM
process.load( "xINCLUDE_DIRECTORYx.SiStripDQMOfflineGlobalRunCAF_cff" )

# Input
process.load( "xINCLUDE_DIRECTORYx.xINPUT_FILESx" )
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( -1 )
)

# HLT Filter
process.hltFilter = cms.EDFilter( "HLTHighLevel",
    HLTPaths          = cms.vstring(
        'HLT_TrackerCosmics_CoTF',
        'HLT_TrackerCosmics_CTF' ,
        'HLT_TrackerCosmics_RS'
    ),
    andOr             = cms.bool( True ),
    TriggerResultsTag = cms.InputTag( "TriggerResults", "", "FU" )
)

# Scheduling
process.p = cms.Path(
xRECO_FROM_RAWx
xHLT_FILTERx
xDQM_FROM_RAWx
    process.SiStripDQMRecoGlobalRunCAF           *
    process.SiStripDQMSourceGlobalRunCAF_reduced *
    process.dqmSaver
)
