#! /usr/bin/env python
# -*- coding: utf-8 -*-
"""Ten plik zawiera klasy obslugujaca menu contextowe dla tablicy download
"""
__author__ =  'Lukasz Busko'                
__email__ = 'buskol.waw.pl@gmail.com'
__version__= '0.0.4'
__licenes__= 'Licenes GPL'


import sys,os, time
from PyQt4 import QtCore,QtGui

class t_dl_ContextMenu(QtGui.QMenu):
    def __init__(self, table, parent):
        QtGui.QMenu.__init__(self)
        self.ClassName = "[t_dl_ContextMenu]->"
        self.Table = table
        self.Parent = parent
        self.SetMenu()
    def SetMenu(self):
        """Ustawia nazwy menu contextowego i akcje z nim zwiazane"""
        self.addAction("Start", self.Start)
        self.addAction("Stop", self.Stop)
        self.addAction("Anuluj", self.Abort)
        self.addAction(QtGui.QApplication.translate("t_dl_ContextMenu", "Usuń", None, QtGui.QApplication.UnicodeUTF8), self.DelFromHdisk)
        self.addAction(QtGui.QApplication.translate("t_dl_ContextMenu", "Usuń z listy", None, QtGui.QApplication.UnicodeUTF8), self.DelFromList)
        self.addAction(QtGui.QApplication.translate("t_dl_ContextMenu","Wyczyść ściągnięte", None, QtGui.QApplication.UnicodeUTF8), self.DelDownloadedFromList)
        self.addAction(QtGui.QApplication.translate("t_dl_ContextMenu","Wyczyść liste", None, QtGui.QApplication.UnicodeUTF8), self.PurgeList)
    def ContexMenu(self, point):
        """Handler  do ktorego trzeba sie odwolac aby menu dzialalo"""
        self.exec_(self.Table.mapToGlobal(point))
    def Start(self):
        """Aktywuje sciaganie poprzez wyslanie rzadania do serwera"""
        print "[start]"
        if self.Parent.mySocket != None:
            print "[start sciagania]"
            self.Parent.mySocket.send("")#xml generujacy zadanie
    def Stop(self):
        """Stopuje sciaganie poprzez wyslanie rzadania do serwera"""
        print "[stop]"
        if self.Parent.mySocket != None:
            print "[stop sciagania]"
            self.Parent.mySocket.send("")#xml generujacy zadanie
    def Abort(self):
        """Anuluje sciaganie poprzez wyslanie rzadania do serwera i skasowanie pliku z dysku"""
        print "[abort]"
        self.Stop()
        self.DelFromHdisk()
    def DelFromHdisk(self):
        """Kasuje plik z dysku"""
        MethodName = self.ClassName+"[delFromHdisk]->"
        try:
            for i in self.Parent.DelFilesFromDownloadTableListGen():
                os.chmod(i, 0644)
                os.remove(i)
        except IOError, error:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),MethodName+str(error))
        except OSError, error:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"), "Faild to remove file:"+str(error))
        except IndexError,  error:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),MethodName+str(error))
    def DelFromList(self):
        """Kasuje z listy xml i wywoluje odswierze interfejsu"""
        QtCore.QObject.emit(self.Parent.QSignalObjDeleteFromTableDownload, QtCore.SIGNAL("DeleteFromDownloadTable"))
    def PurgeList(self):
        """Kasuje z listy xml i wywoluje odswierze interfejsu"""
        QtCore.QObject.emit(self.Parent.QSignalObjPurgeTableDownload, QtCore.SIGNAL("PurgeDownloadTable"))
    def DelDownloadedFromList(self):
        """Kasuje z listy pliki, ktore zostaly juz sciagniete"""
        print "[delDownloaded]"
        self.Parent.xmlDownloadList.RemoveChildrenByIndex(self.Parent.xmlDownloadList.SearchByElementValue(["0"], 0))
