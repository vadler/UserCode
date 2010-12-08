import os
from subprocess import *

from FWCore.GuiBrowsers.ConfigToolBase import *

from Configuration.PyReleaseValidation.autoCond import autoCond

class PickRelValInputFile( ConfigToolBase ):
    """
    """

    _label             = 'pickRelValInputFile'
    _defaultParameters = dicttypes.SortedKeysDict()

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'condition'   , 'startup'                                                           , '' )
        self.addParameter( self._defaultParameters, 'globalTag'   , autoCond[ self.getDefaultParameters()[ 'condition' ].value ][ : -5 ], 'auto' )
        self.addParameter( self._defaultParameters, 'cmsswVersion', os.getenv( "CMSSW_VERSION" )                                        , 'auto' )
        self.addParameter( self._defaultParameters, 'relVal'      , 'RelValTTbar'                                                       , '' )
        self.addParameter( self._defaultParameters, 'dataTier'    , 'GEN-SIM-RECO'                                                      , '' )
        self.addParameter( self._defaultParameters, 'maxVersions' , 9                                                                   , '' )
        self.addParameter( self._defaultParameters, 'fileNumber'  , 1                                                                   , '' )
        self.addParameter( self._defaultParameters, 'debug'       , False                                                               , '' )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def __call__( self
                , condition    = None
                , globalTag    = None
                , cmsswVersion = None
                , relVal       = None
                , dataTier     = None
                , maxVersions  = None
                , fileNumber   = None
                , debug        = None
                ):
        if condition is None:
            condition = self.getDefaultParameters()[ 'condition' ].value
        if globalTag is None:
            globalTag = self.getDefaultParameters()[ 'globalTag' ].value
        if cmsswVersion is None:
            cmsswVersion = self.getDefaultParameters()[ 'cmsswVersion' ].value
        if relVal is None:
            relVal = self.getDefaultParameters()[ 'relVal' ].value
        if dataTier is None:
            dataTier = self.getDefaultParameters()[ 'dataTier' ].value
        if maxVersions is None:
            maxVersions = self.getDefaultParameters()[ 'maxVersions' ].value
        if fileNumber is None:
            fileNumber = self.getDefaultParameters()[ 'fileNumber' ].value
        if debug is None:
            debug = self.getDefaultParameters()[ 'debug' ].value
        self.setParameter( 'condition'   , condition )
        self.setParameter( 'globalTag'   , globalTag )
        self.setParameter( 'cmsswVersion', cmsswVersion )
        self.setParameter( 'relVal'      , relVal )
        self.setParameter( 'dataTier'    , dataTier )
        self.setParameter( 'maxVersions' , maxVersions )
        self.setParameter( 'fileNumber'  , fileNumber )
        self.setParameter( 'debug'       , debug )
        return self.apply()

    def apply( self ):
        condition    = self._parameters[ 'condition'    ].value
        globalTag    = self._parameters[ 'globalTag'    ].value
        cmsswVersion = self._parameters[ 'cmsswVersion' ].value
        relVal       = self._parameters[ 'relVal'       ].value
        dataTier     = self._parameters[ 'dataTier'     ].value
        maxVersions  = self._parameters[ 'maxVersions'  ].value
        fileNumber   = self._parameters[ 'fileNumber'      ].value
        debug        = self._parameters[ 'debug'        ].value
        if debug:
            print 'DEBUG %s: Called with...'%( self._label )
            for key in self._parameters.keys():
               print '    %s:\t'%( key ),
               print self._parameters[ key ].value,
               if self._parameters[ key ].value is self.getDefaultParameters()[ key ].value:
                   print ' (default)'
               else:
                   print

        command      = 'nsls'
        rfdirPath    = '/store/relval/%s/%s/%s/%s-v'%( cmsswVersion, relVal, dataTier, globalTag )
        argument     = '/castor/cern.ch/cms%s'%( rfdirPath )
        filePath     = ''
        fileCount    = {}
        validVersion = 0

        for version in range( 1, maxVersions + 1 ):
            fileCount[ version ] = 0
            if debug:
                print 'DEBUG %s: Checking directory \'%s%i\''%( self._label, argument, version )
            directories = Popen( [ command, '%s%i'%( argument, version ) ], stdout = PIPE, stderr = PIPE ).communicate()[0]
            for directory in directories.splitlines():
                files = Popen( [ command, '%s%i/%s'%( argument, version, directory ) ], stdout = PIPE, stderr = PIPE ).communicate()[0]
                for file in files.splitlines():
                    if len( file ) > 0:
                        if debug:
                            print 'DEBUG %s: File \'%s\' found'%( self._label, file )
                        fileCount[ version ] += 1
                        validVersion = version
                    if fileCount[ version ] >= fileNumber:
                        filePath = '%s%i/%s/%s'%( rfdirPath, version, directory, file )
                        break
                if debug:
                    print 'DEBUG %s: %i file(s) found'%( self._label, fileCount[ version ] )
                if fileCount[ version ] >= fileNumber:
                    break
            if fileCount[ version ] >= fileNumber:
              break
        if validVersion == 0:
            print 'ERROR pickRelValInputFile()'
            print '    No RelVal file(s) found at all in \'%s*\''%( argument )
        elif fileCount[ validVersion ] < fileNumber:
            print 'ERROR pickRelValInputFile()'
            print '    Only %i RelVal file(s) found in \'%s%i\''%( fileCount[ validVersion ], argument, validVersion )

        if debug:
            print 'DEBUG %s: returning file \'%s\''%( self._label, filePath )
        return filePath

pickRelValInputFile = PickRelValInputFile()
