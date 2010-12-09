import os
from subprocess import *

from FWCore.GuiBrowsers.ConfigToolBase import *

from Configuration.PyReleaseValidation.autoCond import autoCond

class PickRelValInputFiles( ConfigToolBase ):
    """
    """

    _label             = 'pickRelValInputFiles'
    _defaultParameters = dicttypes.SortedKeysDict()

    def getDefaultParameters( self ):
        return self._defaultParameters

    def __init__( self ):
        ConfigToolBase.__init__( self )
        self.addParameter( self._defaultParameters, 'cmsswVersion' , os.getenv( "CMSSW_VERSION" )                                        , 'auto from environment' )
        self.addParameter( self._defaultParameters, 'relVal'       , 'RelValTTbar'                                                       , '' )
        self.addParameter( self._defaultParameters, 'dataTier'     , 'GEN-SIM-RECO'                                                      , '' )
        self.addParameter( self._defaultParameters, 'condition'    , 'startup'                                                           , '' )
        self.addParameter( self._defaultParameters, 'globalTag'    , autoCond[ self.getDefaultParameters()[ 'condition' ].value ][ : -5 ], 'auto from \'condition\'' )
        self.addParameter( self._defaultParameters, 'maxVersions'  , 9                                                                   , '' )
        self.addParameter( self._defaultParameters, 'skipFiles'    , 0                                                                   , '' )
        self.addParameter( self._defaultParameters, 'numberOfFiles', 1                                                                   , '' )
        self.addParameter( self._defaultParameters, 'debug'        , False                                                               , '' )
        self._parameters = copy.deepcopy( self._defaultParameters )
        self._comment = ""

    def __call__( self
                , cmsswVersion  = None
                , relVal        = None
                , dataTier      = None
                , condition     = None
                , globalTag     = None
                , maxVersions   = None
                , skipFiles     = None
                , numberOfFiles = None
                , debug         = None
                ):
        if cmsswVersion is None:
            cmsswVersion = self.getDefaultParameters()[ 'cmsswVersion' ].value
        if relVal is None:
            relVal = self.getDefaultParameters()[ 'relVal' ].value
        if dataTier is None:
            dataTier = self.getDefaultParameters()[ 'dataTier' ].value
        if condition is None:
            condition = self.getDefaultParameters()[ 'condition' ].value
        if globalTag is None:
            globalTag = autoCond[ condition ][ : -5 ] # auto from 'condition'
        if maxVersions is None:
            maxVersions = self.getDefaultParameters()[ 'maxVersions' ].value
        if skipFiles is None:
            skipFiles = self.getDefaultParameters()[ 'skipFiles' ].value
        if numberOfFiles is None:
            numberOfFiles = self.getDefaultParameters()[ 'numberOfFiles' ].value
        if debug is None:
            debug = self.getDefaultParameters()[ 'debug' ].value
        self.setParameter( 'cmsswVersion' , cmsswVersion )
        self.setParameter( 'relVal'       , relVal )
        self.setParameter( 'dataTier'     , dataTier )
        self.setParameter( 'condition'    , condition )
        self.setParameter( 'globalTag'    , globalTag )
        self.setParameter( 'maxVersions'  , maxVersions )
        self.setParameter( 'skipFiles'    , skipFiles )
        self.setParameter( 'numberOfFiles', numberOfFiles )
        self.setParameter( 'debug'        , debug )
        return self.apply()

    def apply( self ):
        cmsswVersion  = self._parameters[ 'cmsswVersion'  ].value
        relVal        = self._parameters[ 'relVal'        ].value
        dataTier      = self._parameters[ 'dataTier'      ].value
        condition     = self._parameters[ 'condition'     ].value # only used for GT determination in initialization, if GT not explicitly given
        globalTag     = self._parameters[ 'globalTag'     ].value
        maxVersions   = self._parameters[ 'maxVersions'   ].value
        skipFiles     = self._parameters[ 'skipFiles'     ].value
        numberOfFiles = self._parameters[ 'numberOfFiles' ].value
        debug         = self._parameters[ 'debug'         ].value
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
        filePaths    = []
        validVersion = 0

        for version in range( maxVersions, 0, -1 ):
            filePaths = []
            fileCount = 0
            if debug:
                print 'DEBUG %s: Checking directory \'%s%i\''%( self._label, argument, version )
            directories = Popen( [ command, '%s%i'%( argument, version ) ], stdout = PIPE, stderr = PIPE ).communicate()[0]
            for directory in directories.splitlines():
                files = Popen( [ command, '%s%i/%s'%( argument, version, directory ) ], stdout = PIPE, stderr = PIPE ).communicate()[0]
                for file in files.splitlines():
                    if len( file ) > 0:
                        if debug:
                            print 'DEBUG %s: File \'%s\' found'%( self._label, file )
                        fileCount += 1
                        validVersion = version
                    if fileCount > skipFiles:
                        filePath = '%s%i/%s/%s'%( rfdirPath, version, directory, file )
                        filePaths.append( filePath )
                    if len( filePaths ) >= numberOfFiles:
                        break
                if debug:
                    print 'DEBUG %s: %i file(s) found'%( self._label, fileCount )
                if len( filePaths ) >= numberOfFiles:
                    break
            if len( filePaths ) >= numberOfFiles:
              break

        if validVersion == 0:
            print 'ERROR pickRelValInputFiles()'
            print '    No RelVal file(s) found at all in \'%s*\''%( argument )
        elif len( filePaths ) == 0:
            print 'ERROR pickRelValInputFiles()'
            print '    No RelVal file(s) picked up in \'%s%i\''%( argument, validVersion )
        elif len( filePaths ) < numberOfFiles:
            print 'WARNING pickRelValInputFiles()'
            print '    Only %i RelVal files picked up in \'%s%i\''%( len( filePaths ), argument, validVersion )

        if debug:
            print 'DEBUG %s: returning file(s) %s'%( self._label, filePaths )
        return filePaths

pickRelValInputFiles = PickRelValInputFiles()
