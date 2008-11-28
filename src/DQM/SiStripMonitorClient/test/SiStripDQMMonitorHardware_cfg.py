import FWCore.ParameterSet.Config as cms

process = cms.Process( "SiStripDQMMonitorHardware" )

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

### SiStrip DQM ###

## DQM Services

# DQM store

process.load( "DQMServices.Core.DQM_cfg" )
process.DQM.collectorHost = ''

# DQM file saving and environment

process.load( "DQMServices.Components.DQMEnvironment_cfi" )
# File saving
process.dqmSaver.convention        = 'Online'
process.dqmSaver.dirName           = '/afs/cern.ch/user/v/vadler/cms/SiStripDQM/CMSSW_2_2_0_pre1/output'
process.dqmSaver.producer          = 'DQM'
process.dqmSaver.saveByRun         = 1
process.dqmSaver.saveAtJobEnd      = True
process.dqmSaver.referenceHandling = 'qtests'
# Environment
process.dqmEnv.subSystemFolder = 'SiStrip'

## SiStrip DQM sources

process.load( "DQM.SiStripMonitorHardware.siStripFEDMonitor_cfi" )

### Input ###

## PoolSource ## 

process.source = cms.Source( "PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/025864D5-AFAF-DD11-A3B8-000423D94A04.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/047062C3-A9AF-DD11-AC96-000423D9970C.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/04F5A727-A3AF-DD11-8410-001617DBCF1E.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/060E2734-A2AF-DD11-ADA8-001617C3B73A.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/084EB9D2-B6AF-DD11-BA4A-000423D991F0.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/08AB7642-A5AF-DD11-BB61-001617C3B6DC.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/0A678D63-9BAF-DD11-BD0E-001D09F2441B.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/0CEC58F9-9EAF-DD11-B041-000423D98B28.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/109854AA-ABAF-DD11-B19F-001617C3B6DE.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/123F0E2B-AAAF-DD11-A11B-001617C3B76A.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/125AA707-A6AF-DD11-A350-000423D992A4.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/12B0852D-9CAF-DD11-B977-001617E30F4C.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/14440138-9EAF-DD11-848E-000423D6CA42.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/1493B1D0-9CAF-DD11-AE45-001D09F24489.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/16F9DB77-A9AF-DD11-8755-000423D60FF6.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/18AA72B9-B4AF-DD11-AB92-001617E30D38.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/18FE0B1D-A0AF-DD11-9603-001617E30D4A.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/1ADEBD19-9CAF-DD11-BDD7-001617E30D38.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/1C7C9A85-A9AF-DD11-AEEF-000423D9A2AE.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/1EB33B86-B0AF-DD11-82AE-000423D94AA8.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/1ECA36BA-A1AF-DD11-B073-000423D99160.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/2077B2DE-B6AF-DD11-87C0-000423D99EEE.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/2208780B-B4AF-DD11-856A-000423D991F0.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/226C1020-A3AF-DD11-A6D5-000423D94524.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/22DE924D-B3AF-DD11-83FB-000423D98E6C.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/26F6544E-ACAF-DD11-82F6-00161757BF42.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/285CDECC-AFAF-DD11-A2CB-001617DC1F70.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/2891696D-AEAF-DD11-BE2F-000423D985E4.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/289205D2-B6AF-DD11-8065-000423D98EC4.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/2AD2D679-B5AF-DD11-B65C-000423D9989E.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/2E340004-A1AF-DD11-A008-001617C3B5D8.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/2E44D0FB-A5AF-DD11-A78D-000423D98B28.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/2E884F88-B5AF-DD11-9955-000423D99CEE.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/32E34DC9-A8AF-DD11-975C-000423D9989E.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/348E67B9-B5AF-DD11-A301-001617C3B76E.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/34E61EA2-9FAF-DD11-B525-001617DBD332.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/3672D1A5-A6AF-DD11-A289-000423D944F8.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/382F156B-A7AF-DD11-829A-001617E30D12.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/3C290927-A9AF-DD11-AB32-000423D94E70.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/3C881E7E-B0AF-DD11-864D-000423D99F1E.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/3CFF43AA-ABAF-DD11-9043-001617C3B5F4.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/4040D71E-AFAF-DD11-BCFE-001617C3B70E.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/4062D06B-B5AF-DD11-B3F9-001617E30F58.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/442524DE-AAAF-DD11-A1BC-000423D94C68.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/4A5311F1-B2AF-DD11-B244-00161757BF42.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/4C7CBCE9-B1AF-DD11-BEC2-000423D985B0.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/4C89544C-A5AF-DD11-9F89-000423D95220.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/4E36B0B1-ADAF-DD11-A06C-001617C3B706.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/4E60E5EE-B1AF-DD11-9A4C-000423D99614.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/4EB5CB5C-B3AF-DD11-8F80-000423D94A20.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/50E13B78-A9AF-DD11-810C-000423D99B3E.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/52B6F43E-9EAF-DD11-BD09-001617C3B69C.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/54BA150B-A1AF-DD11-8687-00161757BF42.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/560D534C-A0AF-DD11-8EF0-000423D8FA38.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/569CF1E2-9FAF-DD11-991F-000423D9863C.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/56AF7E23-9CAF-DD11-8DC3-000423D98FBC.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/5802545E-A0AF-DD11-8F50-001617C3B710.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/58DC7B79-B5AF-DD11-84BD-000423D98BE8.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/58DEDE96-B0AF-DD11-9265-000423D8F63C.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/6205CD11-A8AF-DD11-AB69-001617E30D38.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/660B2374-B3AF-DD11-9393-000423D98634.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/66751318-9CAF-DD11-A368-001617C3B6E2.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/6832A9F2-9EAF-DD11-A2DC-000423D95220.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/686A76D1-9CAF-DD11-9A8A-001D09F24303.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/6A217758-A7AF-DD11-BA91-000423D944DC.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/6AD7E59F-A4AF-DD11-9B4C-000423D98E6C.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/6ECF6AF2-B2AF-DD11-808D-000423D99F3E.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/72963C9B-9FAF-DD11-9A73-000423D9989E.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/72F1A8F8-ACAF-DD11-8701-000423D9A2AE.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/7437319F-A4AF-DD11-BAE5-001617E30E2C.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/74F0A997-A7AF-DD11-A9FB-001617C3B706.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/78AEBD1B-A3AF-DD11-8FDE-0019DB2F3F9B.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/7C023BBB-A6AF-DD11-8F71-001617C3B6CE.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/7C04CD3F-B1AF-DD11-A176-000423D94534.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/7C9C5CBC-A8AF-DD11-B181-001617DBD224.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/7CC8A3E9-9EAF-DD11-A32D-000423D94A04.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/7E072853-ACAF-DD11-92A3-000423D9989E.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/800D3207-B4AF-DD11-AB80-000423D98750.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/84C712D0-9CAF-DD11-B764-0030487D1BCC.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/84FFCB46-9EAF-DD11-A257-001617C3B5D8.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/86220C16-9CAF-DD11-93C3-001617E30E28.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/882DF4F0-B2AF-DD11-9B42-000423D98834.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/88A7256F-A7AF-DD11-BFFD-000423D98EC4.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/88BE5223-AFAF-DD11-97BC-000423D98868.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/90CCB757-A5AF-DD11-AFB3-000423D986A8.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/90D1C104-ADAF-DD11-8CC6-000423D99CEE.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/92E94FD1-9CAF-DD11-B642-001D09F2462D.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/9464A550-B4AF-DD11-A1E7-000423D6A6F4.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/964A22B8-A1AF-DD11-8503-000423D9863C.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/9869FACD-A1AF-DD11-A9A8-001617E30D0A.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/9EB6E6A7-ABAF-DD11-9E30-001617E30D4A.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/A0984586-B7AF-DD11-9738-000423D944FC.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/A844887E-AAAF-DD11-83BF-0019DB29C5FC.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/AA30220D-A8AF-DD11-9050-000423D6B42C.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/AA60D2F2-A5AF-DD11-8642-000423D944FC.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/AC11DF77-A2AF-DD11-AF91-001617C3B70E.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/AC2F27CF-A8AF-DD11-BA95-000423D987E0.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/B6A1CF82-B7AF-DD11-9CCD-000423D98EC4.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/B87B7188-9DAF-DD11-8DE8-000423D9939C.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/B8932DE1-B1AF-DD11-A979-000423D99160.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/BA2FD006-A1AF-DD11-A7CF-001617E30E28.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/BA81D6CC-B4AF-DD11-847A-001617DBD472.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/BC9ED725-AFAF-DD11-8A7D-000423D98F98.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/BE247074-AEAF-DD11-A87D-001617E30CA4.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/BE70258A-B0AF-DD11-9B78-000423D98920.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/BEE4E495-B7AF-DD11-A1E8-000423D94E70.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/C62672AF-ADAF-DD11-9B67-000423D94908.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/C64DF511-A1AF-DD11-B367-000423D99996.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/C6C5F231-B1AF-DD11-9510-000423D98800.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/C8537819-B4AF-DD11-9B06-001617DBD556.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/C8BF56F8-B2AF-DD11-9DED-000423D6B444.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/CC08BE7C-A2AF-DD11-B280-000423D6A6F4.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/D2A05989-9DAF-DD11-BAD6-000423D987E0.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/D42154B7-A6AF-DD11-A549-001617C3B706.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/D4880E93-A4AF-DD11-9AD4-00161757BF42.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/D8E41B35-B8AF-DD11-998A-000423D99264.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/DE06B225-AAAF-DD11-A179-001617C3B6FE.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/E01CA9D6-A3AF-DD11-B84F-000423D94524.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/E2ADDACB-ADAF-DD11-BD9E-001617C3B79A.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/E4059FDD-AFAF-DD11-AFE3-000423D98E6C.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/E4536C0B-ADAF-DD11-9AA4-000423D94908.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/E495644F-ACAF-DD11-9CBC-000423D6B2D8.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/EC303A81-9DAF-DD11-827B-000423D99996.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/ECC793FB-AAAF-DD11-AFEF-000423D996C8.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/F00D0B34-B1AF-DD11-A7C6-000423D98AF0.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/F00F4DE4-A3AF-DD11-A4DA-001617DBD316.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/F02F2151-ACAF-DD11-AC35-000423D6CA72.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/F240AA86-B7AF-DD11-B0CF-001617C3B654.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/F25329EA-A3AF-DD11-876D-001617E30CA4.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/F254D82C-B6AF-DD11-9A8A-000423D992DC.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/F4845F1D-A3AF-DD11-9587-000423D98750.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/F673E742-A5AF-DD11-B0A6-001617C3B70E.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/FA018B27-AEAF-DD11-A055-000423D987E0.root',
        '/store/data/Commissioning08/Cosmics/RAW/v1/000/070/675/FCBC6F67-B3AF-DD11-899B-000423DD2F34.root'
    ),
    skipEvents = cms.untracked.uint32(0)
)

## Input steering ##

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 100 )
)

### Output ###

## DQM ##

# DQM Saver path #

process.dqmSaver.dirName = '/afs/cern.ch/user/v/vadler/scratch0/cms/SiStripDQM/CMSSW_2_2_0_pre1/output'

### Scheduling ###

## Paths ##

# DQM path #

process.p = cms.Path(
    process.siStripFEDMonitor *
    process.dqmSaver
)
