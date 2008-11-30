import FWCore.ParameterSet.Config as cms

patLayer0EventContent = cms.PSet(
    outputCommands = cms.untracked.vstring('keep edmTriggerResults_TriggerResults_*_*', 
        'keep *_genParticles_*_*', 
        'keep recoJetTags_*_*_*', 
        'keep recoTracks_generalTracks_*_*', 
        'keep *_offlinePrimaryVertices_*_*', 
        'keep *_caloTowers_*_*',                  # to allow embedding
        'keep CaloTowersSorted_towerMaker_*_*',   # to allow embedding
        'keep floatedmValueMap_*_*_*', 
        'keep *_layer0MuonIsolations_*_*', 
        'keep *_layer0ElectronIsolations_*_*', 
        'keep *_layer0PhotonIsolations_*_*', 
        'keep *_layer0PhotonID_*_*', 
        'keep *_layer0JetCorrFactors_*_*', 
        'keep recoPFTauDiscriminator_*_*_*',
        'keep recoCaloTauDiscriminator_*_*_*',
        'keep recoGenJetsedmAssociation_*_*_*', 
        'keep *_iterativeCone5GenJets_*_*', 
        'keep *_tauGenJets_*_*', 
        'keep *_genMet_*_*', 
        'keep recoMuons_muons__*',                       # for Layer 1 MET, to be fixed
        'keep recoMuons_paramMuons_ParamGlobalMuons_*',  # for Layer 1 MET, to be fixed
        'keep recoGenParticlesedmAssociation_*_*_*', 
        'keep *_jetFlavourAssociation_*_*', 
        'keep *_layer0JetTracksAssociator_*_*', 
        'keep *_layer0JetCharge_*_*', 
        'keep *_layer0BTags_*_*', 
        'keep *_layer0TagInfos_*_*', 
        'keep recoTrackIPTagInfos_*_*_*', 
        'keep recoSoftLeptonTagInfos_*_*_*', 
        'keep recoSecondaryVertexTagInfos_*_*_*', 
        'keep *_allLayer0Photons_*_*', 
        'keep *_allLayer0Electrons_*_*', 
        'keep *_allLayer0Muons_*_*', 
        'keep *_allLayer0Taus_*_*', 
        'keep *_allLayer0Jets_*_*', 
        'keep *_allLayer0PFJets_*_*', 
        'keep *_allLayer0METs_*_*', 
        'drop recoCandidateedmRefToBaseedmValueMap_allLayer0Photons_*_*', 
        'drop recoCandidateedmRefToBaseedmValueMap_allLayer0Electrons_*_*', 
        'drop recoCandidateedmRefToBaseedmValueMap_allLayer0Muons_*_*', 
        'drop recoCandidateedmRefToBaseedmValueMap_allLayer0Taus_*_*', 
        'drop recoCandidateedmRefToBaseedmValueMap_allLayer0Jets_*_*', 
        'drop recoCandidateedmRefToBaseedmValueMap_allLayer0METs_*_*', 
        'keep *_hltTriggerSummaryAOD_*_*', 
        'keep patTriggerPrimitivesOwned_*_*_*', 
        'keep patTriggerPrimitivesOwnededmAssociation_*_*_*'
    )
)

