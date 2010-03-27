#! /usr/bin/env python
# -*- coding: utf-8 -*-
'''
####Author Łukasz Buśko                
####Contact buskol.waw.pl@gmail.com    
####Licenes GNU/General Public License  
######################################
    def __init__(self):
	print "ToolBox Constructor"
    def __init__(self):
	print "MainFrame Constructor"                      
'''

try:
    import sys
    import os
    import time
    import string
    import hashlib
    from PyQt4 import QtGui,QtCore
    #Moje
    import myConfigs
    import myTelnet
    myConfigs = myConfigs.myConfigs()
    myConfigs.LoadCFG()
    from MainFrame import Ui_TabWidget_MainFrame as importMainFrame
    from AdminPanel import Ui_ToolBox_AdminPanel as importAdminPanel
except ImportError, error:
    print "[myClient]->", error
    time.sleep(2)
    sys.exit(1)

####VERSION CHECKS
myConfigs.CheckRequirements()
if myConfigs.Debug != 1:#cos jest nie tak...
    sys.stdout.close()
    #sys.stderr.close()
    #sys.stdin.close()

class classAdminPanel(importAdminPanel):
    def __init__(self):
	self.ClassName = "[classAdminPanel]->"
	print self.ClassName
	importAdminPanel.__init__(self)

class classMainFrame(importMainFrame):
    def __init__(self):
	self.ClassName = "[classMainFrame]->"
	print self.ClassName
	importMainFrame.__init__(self)
	self.ConnectedAs = -1
	self.ConsoleMsgCounter = 0

    def SetUpObjects(self):
	self.CreateAdminPanel()
	self.ShowAdminPanel()
	self.SetButtonAdminPanel()
	self.SetConfigsOnPanel()
	self.SetSignals()
	self.MsgToConsole("Welcome")
    def CreateAdminPanel(self):
	self.ToolBox = QtGui.QToolBox()
	self.AdminPanel = classAdminPanel()
	self.AdminPanel.setupUi(self.ToolBox)
    def ShowAdminPanel(self):
	if self.ConnectedAs == 1:
	    self.ToolBox.show()
	else:
	    self.ToolBox.hide()
    def SetButtonAdminPanel(self):
	if self.ConnectedAs == 1:
	    self.pushButton_AdminPanel.show()
	else:
	    self.pushButton_AdminPanel.hide()
    def ConnectToServer(self):
	MethodName = self.ClassName+"[ConnectToServer]->"
	strRet =""
	if self.ConnectedAs > -1:
            self.pushButton_ConnDiscon.setText(QtGui.QApplication.translate("TabWidget_MainFrame", "Połącz", None, QtGui.QApplication.UnicodeUTF8))
	    self.myTelnet.TelnetDisconnect()
	    del self.myTelnet
	    self.MsgToConsole("Disconnected from "+myConfigs.Host+"::"+str(myConfigs.Port))
	    self.ConnectedAs = -1
	else:
	    self.myTelnet = myTelnet.myTelnetParser(myConfigs,self.listWidget_console)
	    self.MsgToConsole("Connecting to "+myConfigs.Host+"::"+str(myConfigs.Port))
	    self.ConnectedAs = self.myTelnet.AmIRoot()
	    if self.ConnectedAs == -1:
	    	self.myTelnet.TelnetDisconnect()
	    	del self.myTelnet
	    	self.MsgToConsole("Authorization failed, client is going to disconnect it self from server "+myConfigs.Host+"::"+str(myConfigs.Port))
	    else:
            	self.pushButton_ConnDiscon.setText(QtGui.QApplication.translate("TabWidget_MainFrame", "Rozłącz", None, QtGui.QApplication.UnicodeUTF8))
	    	self.LocalServerTime = self.myTelnet.AskForServerTime()
	    	self.MsgToConsole("Connected and authorized successful, local server time is: "+str(self.LocalServerTime))

	#odswierzyc layout
    def MsgToConsole(self,Msg1):
	self.listWidget_console.insertItem(self.ConsoleMsgCounter,QtCore.QString(time.strftime(myConfigs.ConsoleTimeFormat)+" "+Msg1))
	self.ConsoleMsgCounter+=1
    ####SINGALS
    def SetSignals(self):
	#MAINFRAME->Main
        QtCore.QObject.connect(self.pushButton_ConnDiscon, QtCore.SIGNAL("clicked()"), self.ConnectToServer)
	QtCore.QObject.connect(self.pushButton_AdminPanel, QtCore.SIGNAL("clicked()"), self.ShowAdminPanel)
	#RIGHTTOP
	QtCore.QObject.connect(self.pushButton_Author, QtCore.SIGNAL("clicked()"), self.MsgBoxAbout)
	QtCore.QObject.connect(self.pushButton_Quit, QtCore.SIGNAL("clicked()"), self.MsgOnExit)
	#MAINFRAME->Shared
	#MAINFRAME->Options
	QtCore.QObject.connect(self.buttonBox_Save, QtCore.SIGNAL("accepted()"), self.StoreConfigs)
	QtCore.QObject.connect(self.buttonBox_Save, QtCore.SIGNAL("rejected()"), self.SetConfigsOnPanel)
	QtCore.QObject.connect(self.checkBox_Debug, QtCore.SIGNAL("stateChange()"), self.CheckBoxChange_Debug)
	#MAINGRAME->ProjectSite
	QtCore.QObject.connect(self.horizontalSlider_Zoom, QtCore.SIGNAL("valueChanged(int)"), self.Slider_Zoom)
    ####MSG_BOXES
    ####Methods 
    ##Main
    #for buttons on RightTop
    def MsgBoxAbout(importAdminPanel):#Niedorobione i nie dziala...
	box = QtGui.QMessageBox(importAdminPanel)
	box.setWindowTitle(QtGui.QApplication.translate("About"))
	box.setText(QtGui.QApplication.translate(None,"You have clicked button",None, QtGui.QApplication.UnicodeUTF8))
	box.setStandardButtons(QtGui.QMessageBox.Ok)
	box.exec_()
    def MsgOnExit(self):
	del self.ToolBox
	self.myTelnet.TelnetDisconnect()
	del self.myTelnet
	print self.ClassName+"Bye Bye"
    ##Options
    def SetConfigsOnPanel(self):
	self.lineEdit_IP.insert(QtCore.QString(myConfigs.Host))
	self.lineEdit_Port.insert(QtCore.QString(str(myConfigs.Port)))
	self.lineEdit_Login.insert(QtCore.QString(myConfigs.User))
	self.lineEdit_Password.insert(QtCore.QString(myConfigs.Password))
	self.lineEdit_PathSharedFile.insert(QtCore.QString(myConfigs.SharedFiles))
	#
	self.spinBox_TimeOut.setValue(myConfigs.TimeOut)
	self.horizontalSlider_Zoom.setSliderPosition(myConfigs.Zoom)
	self.horizontalSlider_Zoom.setValue(myConfigs.Zoom)
        self.progressBar_Zoom.setValue(myConfigs.Zoom)
	self.checkBox_Debug.setCheckState(myConfigs.Debug)
	#
    #for button bottom
    def StoreConfigs(self):
	myConfigs.Host = self.lineEdit_IP.displayText()
	myConfigs.Port = self.lineEdit_Port.displayText()
	myConfigs.User = self.lineEdit_Login.displayText()
        self.lineEdit_Password.setEchoMode(QtGui.QLineEdit.Normal)
	myConfigs.Password = self.lineEdit_Password.displayText()
        self.lineEdit_Password.setEchoMode(QtGui.QLineEdit.PasswordEchoOnEdit)
	myConfigs.TimeOut = int(self.spinBox_TimeOut.text())
	myConfigs.SharedFiles = self.lineEdit_PathSharedFile.displayText()
	myConfigs.Zoom = self.progressBar_Zoom.value()
	if myConfigs.StoreCFG() == 0:
	    self.MsgToConsole("Configs has been stored")
	else:
	    self.MsgToConsole("Failed to store configs")
    def CheckBoxChange_Debug(self):#Zjebane do poprawy
	if myConfigs.Debug == 1:
	    myConfigs.Debug = 0
	else:
	    myConfig.Debug = 1
	self.checkBox_Debug.setCheckState(myConfigs.Debug)
    def Slider_Zoom(self,value):
        self.progressBar_Zoom.setValue(value)
	self.webView_ProjectSite.setZoomFactor(float(value*0.01))

	
def myClient():
    print "[myClient]->"
    app = QtGui.QApplication(sys.argv)
    MainFrame = classMainFrame()
    TabWidget = QtGui.QTabWidget()
    MainFrame.setupUi(TabWidget)
    MainFrame.SetUpObjects()
    TabWidget.show()
    
    return app.exec_()

