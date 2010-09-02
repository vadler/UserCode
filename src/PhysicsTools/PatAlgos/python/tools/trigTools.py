from FWCore.GuiBrowsers.ConfigToolBase import *

from PhysicsTools.PatAlgos.tools.helpers import *
from PhysicsTools.PatAlgos.patEventContent_cff import patTriggerL1RefsEventContent

_defaultTriggerMatchers      = [ 'cleanMuonTriggerMatchHLTMu9'
                               , 'cleanMuonTriggerMatchHLTDoubleIsoMu3'
                               , 'cleanPhotonTriggerMatchHLTPhoton20CleanedL1R'
                               , 'cleanElectronTriggerMatchHLTEle20SWL1R'
                               , 'cleanTauTriggerMatchHLTDoubleLooseIsoTau15'
                               , 'cleanJetTriggerMatchHLTJet15U'
                               , 'metTriggerMatchHLTMET45'
                               ]
_defaultTriggerProducer      = 'patTrigger'
_defaultTriggerEventProducer = 'patTriggerEvent'
_defaultSequence             = 'patDefaultSequence'
_defaultHltProcess           = 'HLT'
_defaultOutputModule         = 'out'

_defaultTriggerMatchersComment      = "Trigger matcher modules' labels, default: ..."
_defaultTriggerProducerComment      = "PATTriggerProducer module label, default: %s"%( _defaultTriggerProducer )
_defaultTriggerEventProducerComment = "PATTriggerEventProducer module label, default: %s"%( _defaultTriggerEventProducer )
_defaultSequenceComment             = "Name of sequence to use, default: %s"%( _defaultSequence )
_defaultHltProcessComment           = "HLT process name, default: %s"%( _defaultHltProcess )
_defaultOutputModuleComment         = "Output module label, empty label indicates no output, default: %s"%( _defaultOutputModule )


def _modulesInSequence( process, sequenceLabel ):
    return [ m.label() for m in listModules( getattr( process, sequenceLabel ) ) ]


def _addEventContent( outputCommands, eventContent ):
    for content in eventContent:
        if content not in outputCommands:
            outputCommands += [ content ]
    listToRemove = []
    for i in range( len( outputCommands ) ):
        if i in listToRemove:
            continue
        command = outputCommands[ i ]
        if command[ : 4 ] == 'keep':
            dropCommand = command.replace( 'keep ', 'drop ' )
            for j in range( i + 1, len( outputCommands ) ):
                testCommand = outputCommands[ j ]
                if testCommand == command:
                    listToRemove += [ j ]
                elif testCommand == dropCommand:
                    listToRemove += [ i, j ]
                    break
    newOutputCommands = cms.untracked.vstring()
    for i in range( len( outputCommands ) ):
        if i not in listToRemove:
            newOutputCommands += [ outputCommands[ i ] ]
    return newOutputCommands


class SwitchOnTrigger( ConfigToolBase ):
    """  Enables trigger information in PAT
    SwitchOnTrigger( [cms.Process], triggerProducer = 'patTrigger', triggerEventProducer = 'patTriggerEvent', sequence = 'patDefaultSequence', hltProcess = 'HLT', outputModule = 'out' )
    - [cms.Process]       : the 'cms.Process'
    - triggerProducer     : PATTriggerProducer module label;
                            optional, default: 'patTrigger'
    - triggerEventProducer: PATTriggerEventProducer module label;
                            optional, default: 'patTriggerEvent'
    - sequence            : name of sequence to use;
                            optional, default: 'patDefaultSequence'
    - hltProcess          : HLT process name;
                            optional, default: 'HLT'
    - outputModule        : output module label;
                            empty label indicates no output;
                            optional, default: 'out'
    Using None as any argument restores its default value.
    """
    _label             = 'switchOnTrigger'
    _defaultParameters = dicttypes.SortedKeysDict()

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'triggerProducer'     , _defaultTriggerProducer     , _defaultTriggerProducerComment )
        self.addParameter( self._defaultParameters, 'triggerEventProducer', _defaultTriggerEventProducer, _defaultTriggerEventProducerComment )
        self.addParameter( self._defaultParameters, 'sequence'            , _defaultSequence            , _defaultSequenceComment )
        self.addParameter( self._defaultParameters, 'hltProcess'          , _defaultHltProcess          , _defaultHltProcessComment )
        self.addParameter( self._defaultParameters, 'outputModule'        , _defaultOutputModule        , _defaultOutputModuleComment )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __call__( self, process
                , triggerProducer      = None
                , triggerEventProducer = None
                , sequence             = None
                , hltProcess           = None
                , outputModule         = None
                ):
        if triggerProducer is None:
            triggerProducer = self._defaultParameters[ 'triggerProducer' ].value
        if triggerEventProducer is None:
            triggerEventProducer = self._defaultParameters[ 'triggerEventProducer' ].value
        if sequence is None:
            sequence = self._defaultParameters[ 'sequence' ].value
        if hltProcess is None:
            hltProcess = self._defaultParameters[ 'hltProcess' ].value
        if outputModule is None:
            outputModule = self._defaultParameters[ 'outputModule' ].value
        self.setParameter( 'triggerProducer'     , triggerProducer )
        self.setParameter( 'triggerEventProducer', triggerEventProducer )
        self.setParameter( 'sequence'            , sequence )
        self.setParameter( 'hltProcess'          , hltProcess )
        self.setParameter( 'outputModule'        , outputModule )
        self.apply( process )

    def toolCode( self, process ):
        triggerProducer      = self._parameters[ 'triggerProducer' ].value
        triggerEventProducer = self._parameters[ 'triggerEventProducer' ].value
        sequence             = self._parameters[ 'sequence' ].value
        hltProcess           = self._parameters[ 'hltProcess' ].value
        outputModule         = self._parameters[ 'outputModule' ].value
        # Load default producers from existing config files, if needed
        if not hasattr( process, triggerProducer ):
            if triggerProducer is self.getDefaultParameters()[ 'triggerProducer' ].value:
                process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi" )
        if not hasattr( process, triggerEventProducer ):
            if triggerEventProducer is self.getDefaultParameters()[ 'triggerEventProducer' ].value:
                process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerEventProducer_cfi" )
        # Maintain configurations
        prodSequence            = getattr( process, sequence )
        trigProdMod             = getattr( process, triggerProducer )
        trigProdMod.processName = hltProcess
        if triggerProducer in _modulesInSequence( process, sequence ):
            print 'switchOnTrigger(): PATTriggerProducer module %s exists already in sequence %s'%( triggerProducer, sequence )
            print '                   ==> entry re-used'
            if trigProdMod.onlyStandAlone.value() is True:
                trigProdMod.onlyStandAlone = False
                print '                   configuration parameter automatically changed'
                print '                   PATTriggerProducer %s.onlyStandAlone --> %s'%( triggerProducer, trigProdMod.onlyStandAlone )
            print '---------------------------------------------------------------------'
        else:
            # Sequence arithmetics for PATTriggerProducer module
            if hasattr( process, 'patTriggerSequence' ):
                process.patTriggerSequence *= trigProdMod
            else:
                process.patTriggerSequence  = cms.Sequence( trigProdMod )
                prodSequence               *= process.patTriggerSequence
        trigEvtProdMod             = getattr( process, triggerEventProducer )
        trigEvtProdMod.processName = hltProcess
        if hasattr( trigEvtProdMod, 'patTriggerProducer' ):
            trigEvtProdMod.patTriggerProducer = triggerProducer
        else:
            trigEvtProdMod.patTriggerProducer = cms.InputTag( triggerProducer )
        if triggerEventProducer in _modulesInSequence( process, sequence ):
            print 'switchOnTrigger(): PATTriggerEventProducer module %s exists already in sequence %s'%( triggerEventProducer, sequence )
            print '                   ==> entry re-used'
            print '---------------------------------------------------------------------'
        else:
            # Sequence arithmetics for PATTriggerEventProducer module
            if hasattr( process, 'patTriggerEventSequence' ):
                process.patTriggerEventSequence *= trigEvtProdMod
            else:
                process.patTriggerEventSequence = cms.Sequence( trigEvtProdMod )
                prodSequence                   *= process.patTriggerEventSequence
        # Add event content
        if outputModule is not '':
            patTriggerEventContent = [ 'keep patTriggerObjects_%s_*_%s'%( triggerProducer, process.name_() )
                                     , 'keep patTriggerFilters_%s_*_%s'%( triggerProducer, process.name_() )
                                     , 'keep patTriggerPaths_%s_*_%s'%( triggerProducer, process.name_() )
                                     , 'keep patTriggerEvent_%s_*_%s'%( triggerEventProducer, process.name_() )
                                     ]
            if hasattr( trigProdMod, 'addL1Algos' ) and trigProdMod.addL1Algos.value() is True:
                patTriggerEventContent += [ 'keep patTriggerAlgorithms_%s_*_%s'%( triggerProducer, process.name_() )
                                          ]
            if hasattr( trigProdMod, 'saveL1Refs' ) and trigProdMod.saveL1Refs.value() is True:
                patTriggerEventContent += patTriggerL1RefsEventContent
            getattr( process, outputModule ).outputCommands = _addEventContent( getattr( process, outputModule ).outputCommands, patTriggerEventContent )

switchOnTrigger = SwitchOnTrigger()


class SwitchOnTriggerStandAlone( ConfigToolBase ):
    """  Enables trigger information in PAT, limited to stand-alone trigger objects
    SwitchOnTriggerStandAlone( [cms.Process], triggerProducer = 'patTrigger', sequence = 'patDefaultSequence', hltProcess = 'HLT', outputModule = 'out' )
    - [cms.Process]       : the 'cms.Process'
    - triggerProducer     : PATTriggerProducer module label;
                            optional, default: 'patTrigger'
    - sequence            : name of sequence to use;
                            optional, default: 'patDefaultSequence'
    - hltProcess          : HLT process name;
                            optional, default: 'HLT'
    - outputModule        : output module label;
                            empty label indicates no output;
                            optional, default: 'out'
    Using None as any argument restores its default value.
    """
    _label             = 'switchOnTriggerStandAlone'
    _defaultParameters = dicttypes.SortedKeysDict()

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'triggerProducer', _defaultTriggerProducer, _defaultTriggerProducerComment )
        self.addParameter( self._defaultParameters, 'sequence'       , _defaultSequence       , _defaultSequenceComment )
        self.addParameter( self._defaultParameters, 'hltProcess'     , _defaultHltProcess     , _defaultHltProcessComment )
        self.addParameter( self._defaultParameters, 'outputModule'   , _defaultOutputModule   , _defaultOutputModuleComment )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __call__( self, process
                , triggerProducer      = None
                , sequence             = None
                , hltProcess           = None
                , outputModule         = None
                ):
        if triggerProducer is None:
            triggerProducer = self._defaultParameters[ 'triggerProducer' ].value
        if sequence is None:
            sequence = self._defaultParameters[ 'sequence' ].value
        if hltProcess is None:
            hltProcess = self._defaultParameters[ 'hltProcess' ].value
        if outputModule is None:
            outputModule = self._defaultParameters[ 'outputModule' ].value
        self.setParameter( 'triggerProducer', triggerProducer )
        self.setParameter( 'sequence'       , sequence )
        self.setParameter( 'hltProcess'     , hltProcess )
        self.setParameter( 'outputModule'   , outputModule )
        self.apply( process )

    def toolCode( self, process ):
        triggerProducer = self._parameters[ 'triggerProducer' ].value
        sequence        = self._parameters[ 'sequence' ].value
        hltProcess      = self._parameters[ 'hltProcess' ].value
        outputModule    = self._parameters[ 'outputModule' ].value
        # Load default producer from existing config file, if needed
        if not hasattr( process, triggerProducer ):
            if triggerProducer is self.getDefaultParameters()[ 'triggerProducer' ].value:
                process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cfi" )
        # Maintain configuration
        prodSequence            = getattr( process, sequence )
        trigProdMod             = getattr( process, triggerProducer )
        trigProdMod.processName = hltProcess
        if triggerProducer in _modulesInSequence( process, sequence ):
            print 'switchOnTriggerStandAlone(): PATTriggerProducer module %s exists already in sequence %s'%( triggerProducer, sequence )
            print '                             ==> entry re-used'
            print '---------------------------------------------------------------------'
        else:
            # Sequence arithmetics for PATTriggerProducer module
            if trigProdMod.onlyStandAlone.value() is False:
                trigProdMod.onlyStandAlone = True
                print 'switchOnTriggerStandAlone(): configuration parameter automatically changed'
                print '                             PATTriggerProducer %s.onlyStandAlone --> %s'%( triggerProducer, trigProdMod.onlyStandAlone )
                print '---------------------------------------------------------------------'
            if hasattr( process, 'patTriggerSequence' ):
                process.patTriggerSequence *= trigProdMod
            else:
                process.patTriggerSequence  = cms.Sequence( trigProdMod )
                prodSequence               *= process.patTriggerSequence
        # Add event content
        if outputModule is not '':
            patTriggerEventContent = [ 'keep patTriggerObjectStandAlones_%s_*_%s'%( triggerProducer, process.name_() )
                                     ]
            if hasattr( trigProdMod, 'saveL1Refs' ) and trigProdMod.saveL1Refs.value() is True:
                patTriggerEventContent += patTriggerL1RefsEventContent
            getattr( process, outputModule ).outputCommands = _addEventContent( getattr( process, outputModule ).outputCommands, patTriggerEventContent )

switchOnTriggerStandAlone = SwitchOnTriggerStandAlone()


class SwitchOnTriggerMatching( ConfigToolBase ):
    """  Enables trigger matching in PAT
    SwitchOnTriggerMatching( [cms.Process], triggerMatchers = [default list], triggerProducer = 'patTrigger', triggerEventProducer = 'patTriggerEvent', sequence = 'patDefaultSequence', hltProcess = 'HLT', outputModule = 'out' )
    - [cms.Process]       : the 'cms.Process'
    - triggerMatchers     : PAT trigger matcher module labels (list)
                            optional; default: defined in 'triggerMatchingDefaultSequence'
                            (s. PhysicsTools/PatAlgos/python/triggerLayer1/triggerMatcher_cfi.py)
    - triggerProducer     : PATTriggerProducer module label;
                            optional, default: 'patTrigger'
    - triggerEventProducer: PATTriggerEventProducer module label;
                            optional, default: 'patTriggerEvent'
    - sequence            : name of sequence to use;
                            optional, default: 'patDefaultSequence'
    - hltProcess          : HLT process name;
                            optional, default: 'HLT'
    - outputModule        : output module label;
                            empty label indicates no output;
                            optional, default: 'out'
    Using None as any argument restores its default value.
    """
    _label             = 'switchOnTriggerMatching'
    _defaultParameters = dicttypes.SortedKeysDict()

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'triggerMatchers'     , _defaultTriggerMatchers     , _defaultTriggerMatchersComment )
        self.addParameter( self._defaultParameters, 'triggerProducer'     , _defaultTriggerProducer     , _defaultTriggerProducerComment )
        self.addParameter( self._defaultParameters, 'triggerEventProducer', _defaultTriggerEventProducer, _defaultTriggerEventProducerComment )
        self.addParameter( self._defaultParameters, 'sequence'            , _defaultSequence            , _defaultSequenceComment )
        self.addParameter( self._defaultParameters, 'hltProcess'          , _defaultHltProcess          , _defaultHltProcessComment )
        self.addParameter( self._defaultParameters, 'outputModule'        , _defaultOutputModule        , _defaultOutputModuleComment )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __call__( self, process
                , triggerMatchers      = None
                , triggerProducer      = None
                , triggerEventProducer = None
                , sequence             = None
                , hltProcess           = None
                , outputModule         = None
                ):
        if triggerMatchers is None:
            triggerMatchers = self._defaultParameters[ 'triggerMatchers' ].value
        if triggerProducer is None:
            triggerProducer = self._defaultParameters[ 'triggerProducer' ].value
        if triggerEventProducer is None:
            triggerEventProducer = self._defaultParameters[ 'triggerEventProducer' ].value
        if sequence is None:
            sequence = self._defaultParameters[ 'sequence' ].value
        if hltProcess is None:
            hltProcess = self._defaultParameters[ 'hltProcess' ].value
        if outputModule is None:
            outputModule = self._defaultParameters[ 'outputModule' ].value
        self.setParameter( 'triggerMatchers'     , triggerMatchers )
        self.setParameter( 'triggerProducer'     , triggerProducer )
        self.setParameter( 'triggerEventProducer', triggerEventProducer )
        self.setParameter( 'sequence'            , sequence )
        self.setParameter( 'hltProcess'          , hltProcess )
        self.setParameter( 'outputModule'        , outputModule )
        self.apply( process )

    def toolCode( self, process ):
        triggerMatchers      = self._parameters[ 'triggerMatchers' ].value
        triggerProducer      = self._parameters[ 'triggerProducer' ].value
        triggerEventProducer = self._parameters[ 'triggerEventProducer' ].value
        sequence             = self._parameters[ 'sequence' ].value
        hltProcess           = self._parameters[ 'hltProcess' ].value
        outputModule         = self._parameters[ 'outputModule' ].value
        # Load default producers from existing config file, if needed
        if not hasattr( process, 'triggerMatchingDefaultSequence' ):
            for matcher in triggerMatchers:
                if matcher in self.getDefaultParameters()[ 'triggerMatchers' ].value:
                    process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerMatcher_cfi" )
                    break
        # Switch on PAT trigger information if needed
        if ( triggerProducer not in _modulesInSequence( process, sequence ) or triggerEventProducer not in _modulesInSequence( process, sequence ) ):
            print 'switchOnTriggerMatching(): PAT trigger production switched on automatically using'
            print '                           switchOnTrigger( process, %s, %s, %s, %s, %s )'%( hltProcess, triggerProducer, triggerEventProducer, sequence, outputModule )
            print '---------------------------------------------------------------------'
            switchOnTrigger( process, triggerProducer, triggerEventProducer, sequence, hltProcess, outputModule )
        # Maintain configurations
        prodSequence   = getattr( process, sequence )
        trigEvtProdMod = getattr( process, triggerEventProducer )
        if trigEvtProdMod.patTriggerProducer.value() is not triggerProducer:
            print 'switchOnTriggerMatching(): Configuration conflict found'
            print '                           triggerProducer = %s'%( triggerProducer )
            print '                           differs from'
            print '                           %s.patTriggerProducer = %s'%( triggerEventProducer, trigEvtProdMod.patTriggerProducer )
            print '                           parameter automatically changed'
            print '                           ==> triggerProducer --> %s'%( trigEvtProdMod.patTriggerProducer )
            triggerProducer = trigEvtProdMod.patTriggerProducer
        for matcher in triggerMatchers:
            trigMchMod         = getattr( process, matcher )
            trigMchMod.matched = triggerProducer
            if matcher in _modulesInSequence( process, sequence ):
                print 'switchOnTriggerMatching(): PAT trigger matcher %s exists already in sequence %s'%( matcher, sequence )
                print '                           ==> entry re-used'
                print '---------------------------------------------------------------------'
            else:
                # Sequence arithmetics for PAT trigger matcher modules
                process.patTriggerSequence *= trigMchMod
        matchers = getattr( trigEvtProdMod, 'patTriggerMatches' )
        if len( matchers ) > 0:
            print 'switchOnTriggerMatching(): PAT trigger matchers already attached to existing PATTriggerEventProducer %s'%( triggerEventProducer )
            print '                           configuration parameters automatically changed'
            for matcher in matchers:
                trigMchMod = getattr( process, matcher )
                if trigMchMod.matched.value() is not triggerProducer:
                    removeIfInSequence( process, matcher, 'patTriggerSequence' )
                    trigMchMod.matched          = triggerProducer
                    process.patTriggerSequence *= trigMchMod
                    print '                           PAT trigger matcher %s.matched --> %s'%( matcher, trigMchMod.matched )
            print '---------------------------------------------------------------------'
        else:
            trigEvtProdMod.patTriggerMatches = cms.VInputTag()
        trigEvtProdMod.patTriggerMatches += triggerMatchers
        # Add event content
        if outputModule is not '':
            patTriggerEventContent = []
            for matcher in triggerMatchers:
                patTriggerEventContent += [ 'keep patTriggerObjectsedmAssociation_%s_%s_%s'%( triggerEventProducer, matcher, process.name_() )
                                          , 'keep *_%s_*_*'%( getattr( process, matcher ).src.value() )
                                          ]
            getattr( process, outputModule ).outputCommands = _addEventContent( getattr( process, outputModule ).outputCommands, patTriggerEventContent )

switchOnTriggerMatching = SwitchOnTriggerMatching()


class SwitchOnTriggerMatchingStandAlone( ConfigToolBase ):
    """  Enables trigger matching in PAT
    SwitchOnTriggerMatchingStandAlone( [cms.Process], triggerMatchers = [default list], triggerProducer = 'patTrigger', sequence = 'patDefaultSequence', hltProcess = 'HLT', outputModule = 'out' )
    - [cms.Process]  : the 'cms.Process'
    - triggerMatchers: PAT trigger matcher module labels (list)
                       optional; default: defined in 'triggerMatchingDefaultSequence'
                       (s. PhysicsTools/PatAlgos/python/triggerLayer1/triggerMatcher_cfi.py)
    - triggerProducer: PATTriggerProducer module label;
                       optional, default: 'patTrigger'
    - sequence       : name of sequence to use;
                       optional, default: 'patDefaultSequence'
    - hltProcess     : HLT process name;
                       optional, default: 'HLT'
    - outputModule   : output module label;
                       empty label indicates no output;
                       optional, default: 'out'
    Using None as any argument restores its default value.
    """
    _label             = 'switchOnTriggerMatchingStandAlone'
    _defaultParameters = dicttypes.SortedKeysDict()

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'triggerMatchers', _defaultTriggerMatchers, _defaultTriggerMatchersComment )
        self.addParameter( self._defaultParameters, 'triggerProducer', _defaultTriggerProducer, _defaultTriggerProducerComment )
        self.addParameter( self._defaultParameters, 'sequence'       , _defaultSequence       , _defaultSequenceComment )
        self.addParameter( self._defaultParameters, 'hltProcess'     , _defaultHltProcess     , _defaultHltProcessComment )
        self.addParameter( self._defaultParameters, 'outputModule'   , _defaultOutputModule   , _defaultOutputModuleComment )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __call__( self, process
                , triggerMatchers = None
                , triggerProducer = None
                , sequence        = None
                , hltProcess      = None
                , outputModule    = None
                ):
        if triggerMatchers is None:
            triggerMatchers = self._defaultParameters[ 'triggerMatchers' ].value
        if triggerProducer is None:
            triggerProducer = self._defaultParameters[ 'triggerProducer' ].value
        if sequence is None:
            sequence = self._defaultParameters[ 'sequence' ].value
        if hltProcess is None:
            hltProcess = self._defaultParameters[ 'hltProcess' ].value
        if outputModule is None:
            outputModule = self._defaultParameters[ 'outputModule' ].value
        self.setParameter( 'triggerMatchers', triggerMatchers )
        self.setParameter( 'triggerProducer', triggerProducer )
        self.setParameter( 'sequence'       , sequence )
        self.setParameter( 'hltProcess'     , hltProcess )
        self.setParameter( 'outputModule'   , outputModule )
        self.apply( process )

    def toolCode( self, process ):
        triggerMatchers = self._parameters[ 'triggerMatchers' ].value
        triggerProducer = self._parameters[ 'triggerProducer' ].value
        sequence        = self._parameters[ 'sequence' ].value
        hltProcess      = self._parameters[ 'hltProcess' ].value
        outputModule    = self._parameters[ 'outputModule' ].value
        # Load default producers from existing config file, if needed
        if not hasattr( process, 'triggerMatchingDefaultSequence' ):
            for matcher in triggerMatchers:
                if matcher in self.getDefaultParameters()[ 'triggerMatchers' ].value:
                    process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerMatcher_cfi" )
                    break
        # Switch on PAT trigger information if needed
        if triggerProducer not in _modulesInSequence( process, sequence ):
            print 'switchOnTriggerMatchingStandAlone(): PAT trigger production switched on automatically using'
            print '                                     switchOnTriggerStandAlone( process, %s, %s, %s, %s )'%( hltProcess, triggerProducer, sequence, outputModule )
            print '---------------------------------------------------------------------'
            switchOnTriggerStandAlone( process, triggerProducer, sequence, hltProcess, outputModule )
        # Maintain configurations
        for matcher in triggerMatchers:
            trigMchMod         = getattr( process, matcher )
            trigMchMod.matched = triggerProducer
            if matcher in _modulesInSequence( process, sequence ):
                print 'switchOnTriggerMatchingStandAlone(): PAT trigger matcher %s exists already in sequence %s'%( matcher, sequence )
                print '                                     ==> entry re-used'
                print '---------------------------------------------------------------------'
            else:
                # Sequence arithmetics for PAT trigger matcher modules
                process.patTriggerSequence *= trigMchMod
        # Add event content
        if outputModule is not '':
            patTriggerEventContent = []
            for matcher in triggerMatchers:
                patTriggerEventContent += [ 'keep patTriggerObjectStandAlonesedmAssociation_%s_*_%s'%( matcher, process.name_() )
                                          , 'keep *_%s_*_*'%( getattr( process, matcher ).src.value() )
                                          ]
            getattr( process, outputModule ).outputCommands = _addEventContent( getattr( process, outputModule ).outputCommands, patTriggerEventContent )

switchOnTriggerMatchingStandAlone = SwitchOnTriggerMatchingStandAlone()


class SwitchOnTriggerMatchEmbedding( ConfigToolBase ):
    """  Enables embedding of trigger matches into PAT objects
    SwitchOnTriggerMatchEmbedding( [cms.Process], triggerMatchers = [default list], triggerProducer = 'patTrigger', sequence = 'patDefaultSequence', hltProcess = 'HLT', outputModule = 'out' )
    - [cms.Process]  : the 'cms.Process'
    - triggerMatchers: PAT trigger matcher module labels (list)
                       optional; default: defined in 'triggerMatchingDefaultSequence'
                       (s. PhysicsTools/PatAlgos/python/triggerLayer1/triggerMatcher_cfi.py)
    - triggerProducer: PATTriggerProducer module label;
                       optional, default: 'patTrigger'
    - sequence       : name of sequence to use;
                       optional, default: 'patDefaultSequence'
    - hltProcess     : HLT process name;
                       optional, default: 'HLT'
    - outputModule   : output module label;
                       empty label indicates no output;
                       optional, default: 'out'
    Using None as any argument restores its default value.
    """
    _label             = 'switchOnTriggerMatchEmbedding'
    _defaultParameters = dicttypes.SortedKeysDict()

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'triggerMatchers', _defaultTriggerMatchers, _defaultTriggerMatchersComment )
        self.addParameter( self._defaultParameters, 'triggerProducer', _defaultTriggerProducer, _defaultTriggerProducerComment )
        self.addParameter( self._defaultParameters, 'sequence'       , _defaultSequence       , _defaultSequenceComment )
        self.addParameter( self._defaultParameters, 'hltProcess'     , _defaultHltProcess     , _defaultHltProcessComment )
        self.addParameter( self._defaultParameters, 'outputModule'   , _defaultOutputModule   , _defaultOutputModuleComment )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __call__( self, process
                , triggerMatchers = None
                , triggerProducer = None
                , sequence        = None
                , hltProcess      = None
                , outputModule    = None
                ):
        if triggerMatchers is None:
            triggerMatchers = self._defaultParameters[ 'triggerMatchers' ].value
        if triggerProducer is None:
            triggerProducer = self._defaultParameters[ 'triggerProducer' ].value
        if sequence is None:
            sequence = self._defaultParameters[ 'sequence' ].value
        if hltProcess is None:
            hltProcess = self._defaultParameters[ 'hltProcess' ].value
        if outputModule is None:
            outputModule = self._defaultParameters[ 'outputModule' ].value
        self.setParameter( 'triggerMatchers', triggerMatchers )
        self.setParameter( 'triggerProducer', triggerProducer )
        self.setParameter( 'sequence'       , sequence )
        self.setParameter( 'hltProcess'     , hltProcess )
        self.setParameter( 'outputModule'   , outputModule )
        self.apply( process )

    def toolCode( self, process ):
        triggerMatchers = self._parameters[ 'triggerMatchers' ].value
        triggerProducer = self._parameters[ 'triggerProducer' ].value
        sequence        = self._parameters[ 'sequence' ].value
        hltProcess      = self._parameters[ 'hltProcess' ].value
        outputModule    = self._parameters[ 'outputModule' ].value
        dictEmbedders = { 'selectedPatPhotons'  : 'PATTriggerMatchPhotonEmbedder'
                        , 'cleanPatPhotons'     : 'PATTriggerMatchPhotonEmbedder'
                        , 'selectedPatElectrons': 'PATTriggerMatchElectronEmbedder'
                        , 'cleanPatElectrons'   : 'PATTriggerMatchElectronEmbedder'
                        , 'selectedPatMuons'    : 'PATTriggerMatchMuonEmbedder'
                        , 'cleanPatMuons'       : 'PATTriggerMatchMuonEmbedder'
                        , 'selectedPatTaus'     : 'PATTriggerMatchTauEmbedder'
                        , 'cleanPatTaus'        : 'PATTriggerMatchTauEmbedder'
                        , 'selectedPatJets'     : 'PATTriggerMatchJetEmbedder'
                        , 'cleanPatJets'        : 'PATTriggerMatchJetEmbedder'
                        , 'patMETs'             : 'PATTriggerMatchMETEmbedder'
                        }
        # Load default producers from existing config file, if needed
        if not hasattr( process, 'patTriggerMatchEmbedderDefaultSequence' ):
            process.load( "PhysicsTools.PatAlgos.triggerLayer1.triggerMatchEmbedder_cfi" )
        # Switch on PAT trigger matching if needed
        for matcher in triggerMatchers:
            if matcher not in _modulesInSequence( process, sequence ):
                print 'switchOnTriggerMatchEmbedding(): PAT trigger matching switched on automatically using'
                print '                                 switchOnTriggerMatchingStandAlone( process, %s, %s, %s, %s, %s )'%( hltProcess, triggerMatchers, triggerProducer, sequence, outputModule )
                print '---------------------------------------------------------------------'
                switchOnTriggerMatchingStandAlone( process, triggerMatchers, triggerProducer, sequence, hltProcess, '' )
                break
        # Maintain configurations
        dictConfig = {}
        for matcher in triggerMatchers:
            trigMchMod = getattr( process, matcher )
            if trigMchMod.src.value() in dictConfig:
                dictConfig[ trigMchMod.src.value() ] += [ matcher ]
            else:
                dictConfig[ trigMchMod.src.value() ] = [ matcher ]
        patTriggerEventContent = []
        for srcInput in dictConfig.keys():
            if dictEmbedders.has_key( srcInput ):
                label      = srcInput + 'TriggerMatch'
                trigEmbMod = getattr( process, label )
                if label in _modulesInSequence( process, sequence ):
                    print 'switchOnTriggerMatchEmbedding(): PAT trigger match embedder %s exists already in sequence %s'%( label, sequence )
                    print '                                 ==> entry moved'
                    print '---------------------------------------------------------------------'
                    removeIfInSequence( process, label, 'patTriggerSequence' )
                trigEmbMod.matches         += cms.VInputTag( dictConfig[ srcInput ] )
                process.patTriggerSequence *= trigEmbMod
                # Add event content
                patTriggerEventContent += [ 'drop *_%s_*_*'%( srcInput )
                                          , 'keep *_%s_*_%s'%( label, process.name_() )
                                          ]
            else:
                print 'switchOnTriggerMatchEmbedding(): Invalid input source for trigger match embedding'
                print '                                 ==> %s with matchers %s is skipped'%( srcInput, dictConfig[ srcInput ] )
                print '---------------------------------------------------------------------'
        if outputModule is not '':
            getattr( process, outputModule ).outputCommands = _addEventContent( getattr( process, outputModule ).outputCommands, patTriggerEventContent )

switchOnTriggerMatchEmbedding = SwitchOnTriggerMatchEmbedding()
