import os

import FWCore.ParameterSet.Config as cms

# era = 'Spring10'
# import TopQuarkAnalysis.TopObjectResolutions.stringResolutions_etEtaPhi_Spring10_cff as resolutions
era = 'Summer11'
import TopQuarkAnalysis.TopObjectResolutions.stringResolutions_etEtaPhi_Summer11_cff as resolutions

# Object categories to use, available: 'muon', 'elec', 'udsc', 'bjet', 'met'
objectCategories = cms.vstring( 'muon'
                              , 'elec'
                              , 'udsc'
                              , 'bjet'
                              , 'met'
                              )

process = cms.PSet()

process.existing = cms.PSet(
  # P_t limits for the plots, in line with the categories in "objectCategories"
  objectPtLimits = cms.vdouble( 400.
                              , 400.
                              , 500.
                              , 500.
                              , 300.
                              )
  # Kinematic variables to use; available: 'Et', 'Eta', 'Phi'
, kinematicProperties = cms.vstring( 'Et'
                                   , 'Eta'
                                   , 'Phi'
                                   )
  # output file
, outputFile = cms.string( 'file:%s/output/existingTQAFResolutionFunctions_%s.root'%( os.getenv( "CMSSW_BASE" ), era ) )
)

resolutionPSets = cms.VPSet()
parametrisation = ''
for kinProp in process.existing.kinematicProperties:
  parametrisation += kinProp
for cat in objectCategories:
  labelResolution   = cat + 'Resolution'
  labelResolutionPF = labelResolution + 'PF'
  if hasattr( resolutions, labelResolution ):
    if getattr( resolutions, labelResolution ).parametrization == parametrisation:
      resolutionPSet = cms.PSet(
        functions = cms.VPSet( getattr( resolutions, labelResolution ).functions )
      , label     = cms.string( labelResolution )
      )
      resolutionPSets.append( resolutionPSet )
    else:
      print '%s has the wrong parametrisation %s --> skipping!'%( labelResolution, getattr( resolutions, labelResolution ).parametrization )
  if hasattr( resolutions, labelResolutionPF ):
    if getattr( resolutions, labelResolutionPF ).parametrization == parametrisation:
      resolutionPSet = cms.PSet(
        functions = cms.VPSet( getattr( resolutions, labelResolutionPF ).functions )
      , label     = cms.string( labelResolutionPF )
      )
      resolutionPSets.append( resolutionPSet )
    else:
      print '%s has the wrong parametrisation %s --> skipping!'%( labelResolutionPF, getattr( resolutions, labelResolutionPF ).parametrization )

process.resolutions = cms.VPSet( resolutionPSets )
