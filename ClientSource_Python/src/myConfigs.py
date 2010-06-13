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
    '''
    Ta klasa trzyma ustawienia ,wersje programu oraz wymagania, ktore musza zostac spelnione
    '''
    def __init__(self):
        self.ClassName = "[myConfigs]->"
        self.ClientVersion = "0.0.6"
        self.myPyVersion = "2.6.4"
        self.myQt4Version = ""
        self.maxServerVersion = ""
        self.ConsoleTimeFormat = "[%Y.%m.%d %H:%M:%S]"
        self.FILE_CFG = 'configs/config.conf'
    def LoadCFG(self):
        '''
        Metoda laduje dane z pliku
        '''
        MethodName = self.ClassName+"[LoadCFG]->"
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
            self.SharedFiles = config.get('sharedfiles','sharedfiles')
            self.ValidateBeforeSend = config.getint('sharedfiles','ValidateBeforeSend')
            self.ValidateFilesOnStarup = config.getint('sharedfiles','ValidateFilesOnStarup')
            self.CountCheckSumWhenAdd = config.getint('sharedfiles','CountCheckSumWhenAdd')
            self.LetDownloadFilesWhichFaildValidation = config.getint('sharedfiles','LetDownloadFilesWhichFaildValidation')
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
        '''
        Metoda zapisuje ustawienia do pliku i zwraca czy sie udalo
        '''
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
            config.set('sharedfiles','ValidateBeforeSend',self.ValidateBeforeSend)
            config.set('sharedfiles','ValidateFilesOnStarup', self.ValidateFilesOnStarup)
            config.set('sharedfiles', 'CountCheckSumWhenAdd', self.CountCheckSumWhenAdd)
            config.set('sharedfiles', 'LetDownloadFilesWhichFaildValidation', self.LetDownloadFilesWhichFaildValidation)
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
        '''
        Metoda pobiera plik z udostepnionymi plikami i zwraca go
        '''
        try:
            fp = open( self.FILE_CFG)
            self.SharedFilesXML = fp.readbf()
            fp.close()
            #Set
            del config
        except IOError, error:
            self.Quit(MethodName+str(error), 2, 1)
    def CheckRequirements(self):
        '''
        Metoda sprawdza wersje Pythona jezeli jest nizsza od zdefiniowanej przezmnie program sie zakonczy
        '''
        if sys.version[:string.find(sys.version," ")] < self.myPyVersion:
            self.Quit("Too old Python!\nRequired atleast Python",self.myPyVersion, 2, 1)
        #print PyQt4.pyqt_version_str
        #if qt4 < self.myQt4Version:
