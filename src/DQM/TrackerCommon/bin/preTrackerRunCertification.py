#!/usr/bin/env python

# For documentation of the RR XML-RPC handler, look into https://twiki.cern.ch/twiki//bin/view/CMS/DqmRrApi

import sys
import xmlrpclib

server = xmlrpclib.ServerProxy( 'http://pccmsdqm04.cern.ch/runregistry/xmlrpc' )
data = server.DataExporter.export( 'GLOBAL', 'xml_all', {} )
file = open( sys.argv[1], 'w' )
file.write( data )
file.close()
