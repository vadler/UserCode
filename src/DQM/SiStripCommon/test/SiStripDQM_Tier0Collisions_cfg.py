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
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/FEE389F2-33E2-DE11-A62E-001617C3B76E.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/FE90F72C-43E2-DE11-8E89-001D09F2B30B.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/FE0807BE-48E2-DE11-9A06-001D09F28F25.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/FE03FBF4-54E2-DE11-93BA-001D09F2426D.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/FCBE6855-37E2-DE11-97D1-0030487A3232.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/FC0AF969-32E2-DE11-BCB5-000423D94908.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/FAC74EFB-3AE2-DE11-A244-003048D37580.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/F8FEFD77-4DE2-DE11-A384-000423D951D4.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/F8A6D469-32E2-DE11-93F8-003048D2C020.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/F82DED93-36E2-DE11-9316-000423D9870C.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/F6EF8FA7-4CE2-DE11-B192-001D09F291D7.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/F6B4F70D-5AE2-DE11-92B2-001D09F2983F.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/F2DF3D5D-57E2-DE11-8762-001D09F2512C.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/EEA5E5C6-47E2-DE11-A3AF-001D09F25438.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/EE2A7591-36E2-DE11-844C-001617E30D4A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/EC98D47F-59E2-DE11-88BD-000423D6006E.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/EA6B7FFA-3AE2-DE11-883B-001617C3B6C6.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/EA4139DF-34E2-DE11-A198-000423D6CA02.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/E8EEE2C7-47E2-DE11-8B53-000423D952C0.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/E8703F24-58E2-DE11-93B6-001D09F2514F.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/E838B5EC-3DE2-DE11-8E2A-001617C3B6DE.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/E6F0F717-47E2-DE11-AC1D-000423D99660.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/E66B4474-4BE2-DE11-8E0E-001617C3B5E4.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/E64801C9-39E2-DE11-83CE-001617C3B79A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/E47CDFC2-39E2-DE11-88F6-001D09F231B0.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/E41ABF0A-42E2-DE11-AD1E-000423D99658.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/E0A770D7-49E2-DE11-B96D-001D09F276CF.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/E051FC63-37E2-DE11-ABDD-001D09F2512C.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/DC498E79-50E2-DE11-9F20-0030486730C6.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/D6CFC672-3AE2-DE11-A2B1-001D09F231B0.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/D4BC907F-41E2-DE11-9739-0019DB29C5FC.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/D0E9A1D8-49E2-DE11-A211-0030487C6062.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/CEA81CE4-33E2-DE11-B990-001D09F23174.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/CCE81478-55E2-DE11-9B38-000423D94E70.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/CCD8CA91-36E2-DE11-9D7F-001617DBD224.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/CAD39E82-50E2-DE11-90DF-003048D37580.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/CA2716E3-33E2-DE11-AD99-001D09F282F5.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/C660A173-42E2-DE11-96AB-003048D2C020.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/C60EA517-47E2-DE11-B8DF-003048D2C108.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/C4567978-3AE2-DE11-BB2B-000423D987E0.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/C401F6E4-4BE2-DE11-B0E0-0019B9F72BFF.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/BE66658D-43E2-DE11-AD94-0030487C5CFA.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/BCE2068B-43E2-DE11-894A-000423D944F0.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/BA2226C4-3EE2-DE11-8575-001617C3B6FE.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/B88A1025-56E2-DE11-8E94-001D09F2462D.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/B2CBF2A2-4CE2-DE11-8370-001D09F25401.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/B2A7B072-42E2-DE11-B66D-003048D2C108.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/B02B247C-4DE2-DE11-AF19-000423D99660.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/ACD83B24-3AE2-DE11-A1D3-000423D98B6C.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/A4B77F7A-55E2-DE11-BA99-001D09F29538.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/A2F0E963-57E2-DE11-8AAD-001617C3B778.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/A0F5E12C-43E2-DE11-BF0B-001D09F29538.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/A0EC2860-3BE2-DE11-9351-001D09F241F0.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/9AAD9691-36E2-DE11-B3CB-001D09F24024.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/98890F7F-55E2-DE11-9D36-001D09F29597.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/90357B10-3CE2-DE11-9008-000423D98F98.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/900514C8-3EE2-DE11-A456-000423D98B08.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/8EDD9D8E-45E2-DE11-AA32-001D09F29114.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/8C13027A-53E2-DE11-A75A-001D09F28F25.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/8AC701C9-3EE2-DE11-93F1-001D09F24EE3.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/84EFB521-38E2-DE11-93A8-000423D9880C.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/847C273D-5AE2-DE11-A087-001D09F28D4A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/7EBBBC09-42E2-DE11-A583-000423D99394.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/7C489E8F-45E2-DE11-AC28-001D09F28D4A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/7A3E41D9-55E2-DE11-9CC5-001617E30D4A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/781156DE-3DE2-DE11-8A6B-000423D33970.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/76FB226C-32E2-DE11-9119-001617C3B76A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/74F58270-3AE2-DE11-85F2-000423D60FF6.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/74C32C38-41E2-DE11-A502-001D09F29321.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/7479F876-53E2-DE11-805D-001D09F232B9.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/7216D6C2-39E2-DE11-AF93-0030487A18F2.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/7097C264-3BE2-DE11-A399-000423D98868.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/707B293E-3FE2-DE11-A341-000423D9A2AE.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/6E7F6974-53E2-DE11-845C-000423D6CA6E.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/6E49C25C-57E2-DE11-87C6-0030487A18A4.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/6E3E0F77-4BE2-DE11-96D7-0030487A3C9A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/6AAA2B78-4DE2-DE11-854B-001617C3B66C.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/6A1A76C7-3EE2-DE11-982B-001D09F253C0.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/668B761D-3AE2-DE11-96F7-0019B9F7312C.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/64009BEB-3DE2-DE11-B988-001617C3B77C.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/5C8E0BD9-36E2-DE11-B679-001D09F28F1B.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/5C0ACC32-5AE2-DE11-A26D-001D09F2305C.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/5ADD76EC-3DE2-DE11-BC7A-000423D98F98.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/5A562BEC-3DE2-DE11-9435-000423D6CA6E.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/58E1FD09-42E2-DE11-8B91-000423D986C4.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/58987AE4-33E2-DE11-A432-001D09F2905B.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/583404FE-3AE2-DE11-8564-0030487C5CFA.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/56A3A963-3BE2-DE11-9FCE-001617C3B710.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/565E4F16-3CE2-DE11-A76F-0030487D1BCC.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/54401722-38E2-DE11-95E7-000423D98F98.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/540D4582-50E2-DE11-9B7C-003048D2C108.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/4CF20C84-45E2-DE11-9F9A-001D09F24600.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/4C5EFC0D-47E2-DE11-A9D5-001D09F24934.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/4AF6DBE2-33E2-DE11-B03E-003048D2C1C4.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/4A0FFB6D-57E2-DE11-BB4B-001D09F24D8A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/46F8CA83-55E2-DE11-BE4E-001D09F25393.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/46143304-5BE2-DE11-BF2B-000423D8F63C.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/441A16A4-4CE2-DE11-B7F5-001D09F24DDF.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/421EC88D-45E2-DE11-AE74-001617E30D12.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/408754C4-3EE2-DE11-B66C-001617E30D40.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/407A960E-47E2-DE11-9892-001D09F24024.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/4033E024-56E2-DE11-8CC3-001D09F25438.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/3E920389-44E2-DE11-8A87-0030486780B8.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/3E8B9ACE-55E2-DE11-BAEE-000423D98C20.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/3C12F6E2-33E2-DE11-AF6E-001D09F24353.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/3AE66E46-3FE2-DE11-A5E6-000423D992DC.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/3AE0AA3D-3FE2-DE11-A61B-000423D99896.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/3A3FB16C-32E2-DE11-B56F-001D09F24024.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/30F7C9C1-39E2-DE11-99EF-003048D2C108.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/307D51EE-54E2-DE11-8A62-001617E30D4A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/3079CB3D-3FE2-DE11-A578-0019DB29C5FC.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/2CAE01BA-5BE2-DE11-B4E9-000423D98634.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/2A773003-3BE2-DE11-AFA7-003048D3750A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/2A1BA28D-45E2-DE11-8150-003048D2BE08.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/28DAE662-37E2-DE11-93BC-001617DBD224.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/2836CEBD-39E2-DE11-8472-0016177CA7A0.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/26AB7E83-55E2-DE11-8049-001D09F2424A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/24D24155-37E2-DE11-9B23-0030486780B8.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/223BC3ED-3DE2-DE11-AEA6-001D09F24FEC.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/2085F48F-36E2-DE11-B843-000423D9863C.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/202B3CF2-54E2-DE11-B542-000423D996C8.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/1ECC7C87-44E2-DE11-819F-000423D33970.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/1EBC4591-36E2-DE11-A654-000423D6BA18.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/1E68E9DE-34E2-DE11-B26A-000423D99AAA.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/1C681C0A-42E2-DE11-99EE-000423D99BF2.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/1C3CABBE-48E2-DE11-8F9D-001D09F252DA.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/1A80C207-42E2-DE11-9380-001D09F29538.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/1A1AC218-47E2-DE11-B7BC-003048D3756A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/18C5820E-47E2-DE11-9E37-001D09F251FE.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/168D79DB-49E2-DE11-9E66-001D09F2906A.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/1284873F-3FE2-DE11-9E93-000423D99F1E.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/10447316-3CE2-DE11-A868-000423D991F0.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/0E6D4C2D-43E2-DE11-B115-0030487A1FEC.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/0A85BE90-45E2-DE11-8EE1-001D09F2514F.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/0A38D516-5AE2-DE11-BAB0-001D09F28755.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/084F0FCB-47E2-DE11-BFFD-0016177CA778.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/08314929-41E2-DE11-BDC1-000423D8FA38.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/0297DC23-58E2-DE11-AEC2-001D09F232B9.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/021BEBFC-5AE2-DE11-B1FF-000423D991D4.root',
    #'/store/express/BeamCommissioning09/ExpressPhysics/FEVT/v2/000/123/596/00D8AF8D-36E2-DE11-BEC4-001D09F24303.root'
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
