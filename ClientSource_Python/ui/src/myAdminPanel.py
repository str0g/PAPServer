#! /usr/bin/env python
# -*- coding: utf-8 -*-

__author__ =  'Lukasz Busko'                
__email__ = 'buskol.waw.pl@gmail.com'
__version__= '0.0.3'
__licenes__= 'Licenes GPL'  

try:
    import sys
    import os
    import time
    from PyQt4 import QtGui,QtCore
    #from AdminPanel import Ui_ToolBox_AdminPanel as importAdminPanel
    from AdminPanel import Ui_Window_AdminPanel as adminToolbox
except ImportError, error:
    print "[myAdminPanel]->", error
    time.sleep(2)
    sys.exit(1)

class myAdminPanel(QtGui.QMainWindow):
    def __init__(self, parent = None):
        self.ClassName = "[classAdminPanel]->"
        QtGui.QWidget.__init__(self, parent)
        self.Parent = parent
        self.ToolBoxUI = adminToolbox()
    def SetSignals(self):
        self.QSignalObjButtonRestart = QtCore.QObject(None)
        self.QSignalObjButtonShutdown = QtCore.QObject(None)
        self.QSignalObjButtonShutdownForced = QtCore.QObject(None)
        QtCore.QObject.connect(self.ToolBoxUI.pushButton_RestartServer, QtCore.SIGNAL("clicked()"), self.bServerRestart)
        QtCore.QObject.connect(self.ToolBoxUI.pushButton_ShutdownServer, QtCore.SIGNAL("clicked()"), self.bServerShutdown)
        QtCore.QObject.connect(self.ToolBoxUI.pushButton_ForceShutdownServer, QtCore.SIGNAL("clicked()"), self.bServerShutdownF)
    def run(self):
        if self.ToolBoxUI != None:
            self.ToolBoxUI.setupUi(self)
            self.SetSignals()
            self.setFocusPolicy(QtCore.Qt.StrongFocus)
            self.show()
        else:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"ToolBox object have NOT been created!")
    def Clean(self):
        if self.ToolBoxUI != None:
            del self.ToolBoxUI
            self.ToolBoxUI = None
        if self.Parent == None:
            self.Parent.setActvieWindow()
        self.close()
    def bServerRestart(self):
        if self.Parent.mySocket != None:
            self.Parent.mySocket.ServerRestart()
    def bServerShutdown(self):
        if self.Parent.mySocket != None:
            self.Parent.mySocket.ServerShutdown()
    def bServerShutdownF(self):
        if self.Parent.mySocket != None:
            self.Parent.mySocket.ServerShutdownForced()
