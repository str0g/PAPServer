#! /usr/bin/env python
# -*- coding: utf-8 -*-
'''
####Author Łukasz Buśko                
####Contact buskol.waw.pl@gmail.com    
####Licenes GPL                        
'''
try:
    import codecs
    import ConfigParser 
    import os, sys, time
    import MsgsLib
    import glob, stat
    import string
    from PyQt4 import QtCore
    import PyQt4
    #Moje
    import MsgsLib
except ImportError, error:
    print "[myConfigs]->", error
    time.sleep(2)
    sys.exit(1)

if os.name == "posix":
    SLASH = "/"
    #DOS/Windows
elif os.name in ("nt", "dos", "ce","vista"):
    SLASH = "\\"
else:
    print 'Twój system operacyjny nie jest obslugiwany!\n Zdarzenia nie zostaną wysłane do dziennika systemowego'
    time.sleep(2)

class myConfigs(MsgsLib.MsgDeffinition):
    def __init__(self):
	self.ClassName = "[myConfigs]->"
	self.ClientVersion = "0.0.2"
	self.myPyVersion = "2.6.4"
	self.myQt4Version = ""
	self.maxServerVersion = ""
	self.ConsoleTimeFormat = "[%Y.%m.%d %H:%M:%S]"
	self.FILE_CFG = 'configs/config.conf'
    def LoadCFG(self):
        try:
            config = ConfigParser.ConfigParser()
            fp = open( self.FILE_CFG)
            config.readfp(fp)
            fp.close()
            #Set
            self.Host = config.get('server','Host')
	    self.Port = config.getint('server','Port')
	    self.User = config.get('connection','user')
	    self.Password = config.get('connection','password')
	    self.TimeOut = config.getint('connection','timeout')
	    self.SharedFiles = config.get('sharedfiles','filexml')
	    self.Debug = config.getint('client','debug')
	    self.Zoom = config.getint('client','zoom')
	    del config
        except IOError, error:
            self.Quit(MethodName+str(error), 2, 1)
        except ConfigParser.NoOptionError, error:
            self.Quit(MethodName+str(error), 2, 1)
        except ConfigParser.NoSectionError, error:
            self.Quit(MethodName+str(error), 2, 1)
        except ConfigParser.MissingSectionHeaderError,  error:
            self.Quit(MethodName+str(error), 2, 1)
    def StoreCFG(self):
	MethodName=self.ClassName+"[StoreCFG]->"
	intRet = -1
	try:
            config = ConfigParser.ConfigParser()
            fp = open(self.FILE_CFG)
            config.readfp(fp)
            fp.close()
	    config.set('server','host', self.Host)
	    config.set('server','port', self.Port)
	    config.set('connection','user', self.User)
	    config.set('connection','password', self.Password)
	    config.set('connection','timeout', self.TimeOut)
	    config.set('sharedfiles','sharedfiles', self.SharedFiles)
	    config.set('client','debug', self.Debug)
	    config.set('client','zoom', self.Zoom)
            try:
                with open(self.FILE_CFG, 'wb') as configfile:
                    config.write(configfile)
		intRet = 0
            except IOError, error:
                self.MsgWithDelay(MethodName+str(error), 1)
	except IOError, error:
            self.MsgWithDelay(MethodName+str(error), 2, 1)
        except ConfigParser.NoOptionError, error:
            self.MsgWithDelay(MethodName+str(error), 2, 1)
        except ConfigParser.NoSectionError, error:
            self.MsgWithDelay(MethodName+str(error), 2, 1)
        except ConfigParser.MissingSectionHeaderError,  error:
            self.MsgWithDelay(MethodName+str(error), 2, 1)
        finally:
            del config
	    return intRet
    def LoadSharedXMLFile(self):
	try:
	    fp = open( self.FILE_CFG)
            self.SharedFilesXML = fp.readbf()
            fp.close()
            #Set
	    del config
        except IOError, error:
            self.Quit(MethodName+str(error), 2, 1)
    def CheckRequirements(self):
        if sys.version[:string.find(sys.version," ")] < self.myPyVersion:
            self.Quit("Too old Python!\nRequired atleast Python",self.myPyVersion, 2, 1)
	#print PyQt4.pyqt_version_str
        #if qt4 < self.myQt4Version:
