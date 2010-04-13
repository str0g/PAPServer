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
    from xmlSupport import XMLFileGenerator
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
if myConfigs.Debug != 1:
    stdSilent = os.popen("/dev/null", "w+")
    sys.stdout = stdSilent
    sys.stderr = stdSilent
    sys.stdin = os.popen("/dev/null", "r")

class classAdminPanel(importAdminPanel):
    def __init__(self):
        self.ClassName = "[classAdminPanel]->"
        print self.ClassName
        importAdminPanel.__init__(self)

class classMainFrame(importMainFrame, XMLFileGenerator):
    def __init__(self):
        self.ClassName = "[classMainFrame]->"
        print self.ClassName
        importMainFrame.__init__(self)
        XMLFileGenerator.__init__(self, myConfigs.SharedFiles)
        self.ConnectedAs = -1
        self.intSharedListCounter = 0
        self.ConsoleMsgCounter = 0
        self.QSignalObj1 = QtCore.QObject(None)
        self.buf_XMLSerwerFilesList = None

    def SetUpObjects(self):
        self.SetConfigsOnPanel()
        self.SetSignals()
        self.MsgToConsole("Welcome")
        self.XMLLIstFromFIle()
        if len(self.buf_XMLSharedFilesList)>0:
            self.MsgToConsole("Loading "+str(len(self.buf_XMLSharedFilesList))+" sharedfiles...")
        self.InicializeSharedList()
    ###XML LISTY
    def XMLLIstFromFIle(self):
        if myConfigs.SharedFiles == "":                                                                              #tworze plik w sciezce config/shared.xml
            print self.ClassName+"Path haven't been set, using default path"
            self.CreateXMLSharedFilesList()
            myConfigs.SharedFiles = "/config/shared.xml"
            self.lineEdit_PathSharedFile.setText(QtCore.QString(myConfigs.SharedFiles))
            self.MsgBoxUniversal("Using default shared file")
            self.WriteXMLToFile(myConfigs.SharedFiles, self.GetGeneratedXMLFile(self.buf_XMLSharedFilesList))
            #self.MsgBoxUniversal(u"Ustaw sciżkę do pliki , wktórym mają być lub są trzymane akutalnie udostępniane pliki")
        else:
            if not os.path.isfile(myConfigs.SharedFiles):                                                              #tworze plik w podane scizce
                print self.ClassName+"Path doesn't exist, creating file"
                self.buf_XMLSharedFilesList = self.CreateXMLSharedFilesList()
                self.WriteXMLToFile(myConfigs.SharedFiles, self.GetGeneratedXMLFile(self.buf_XMLSharedFilesList))
            else:
                try:
                    fd = open(myConfigs.SharedFiles, "rb")
                    if self.ValidateXMLDocument(fd.read(), -1) == 0:             #sprawdze czy jest poprawnym plikiem jezeli tak to otwiera, usun -1 pozniej kiedy bedzi mozna validowac
                        print self.ClassName+"File valid :-)"
                        self.buf_XMLSharedFilesList = self.OpenXMLSharedFileList(myConfigs.SharedFiles)
                        self.intSharedListCounter = len(self.buf_XMLSharedFilesList)+1
                        print "List contains ", self.intSharedListCounter, "elements"
                    else:                                                                                 # jezeli plik nie przeszedl validacji czy jest popranym xml to zostanie napisany
                        print self.ClassName+"File invalid :-("
                        self.buf_XMLSharedFilesList = self.CreateXMLSharedFilesList()
                        self.WriteXMLToFile(myConfigs.SharedFiles, self.GetGeneratedXMLFile(self.buf_XMLSharedFilesList))
                    fd.close()
                except IOError, error:
                    print error
    def PurgeSharedWidgetandXMLList(self):
        '''
        Czysci liste xml, tablice oraz zapisuje do pliku zmiany
        '''
        self.PurgeAnyList(self.buf_XMLSharedFilesList, self.tableWidget_SharedFileByMySelf)
        self.PurgeXMLList(self.buf_XMLSharedFilesList)
        self.WriteXMLToFile(myConfigs.SharedFiles, self.buf_XMLSharedFilesList)
        self.intSharedListCounter = 0
        self.MsgToConsole("Shared list has been cleared")
    def PurgeServerWidgetandXMLList(self):
        '''
        Czysci liste xml, tablice
        '''
        self.PurgeAnyList(self.buf_XMLSerwerFilesList, self.tableWidget_SharedByServer)
        self.PurgeXMLList(self.buf_XMLSerwerFilesList)
        self.intServerListCounter = 0
        self.MsgToConsole("Server list has been cleared")
    def PurgeAnyList(self, objList, objTableWidget):
        '''
        Kasuje liste elementow widgeta tablicy
        Nie wiedziec czemu nie kasuje ostatniego elementu ktory zostaje po usuwaniu...
        '''
        intLenOfBuffer = len(objList)
        intRowCount = objTableWidget.rowCount()
        if intLenOfBuffer < intRowCount:
            for index in range(1, intRowCount):
                print "Dlbuffora,ilosc wierszy,index", intLenOfBuffer, intRowCount, index
                objTableWidget.removeRow(objTableWidget.rowCount()-1)
            #self.MsgToConsole("...Done")
            objTableWidget.clearContents
                
    def InicializeSharedList(self, intOffset =0):
        intLenOfBuffer = len(self.buf_XMLSharedFilesList)
        intRowCount = self.tableWidget_SharedFileByMySelf.rowCount()
        self.PurgeAnyList(self.buf_XMLSharedFilesList, self.tableWidget_SharedFileByMySelf)
            
        print "Wazna INF:", intLenOfBuffer, intRowCount
        indexRow = intOffset
        intValueForBar = 0
        if intLenOfBuffer == 0:
            ValueForBarOffset = 100
        else:
            ValueForBarOffset = 100/intLenOfBuffer
        if intLenOfBuffer > 100:
            ValueForBarOffset = intLenOfBuffer/100
        self.progressBar_Shared.setValue(intValueForBar)
        while indexRow < intLenOfBuffer:
            self.tableWidget_SharedFileByMySelf.setItem(indexRow, 0, QtGui.QTableWidgetItem(self.buf_XMLSharedFilesList[indexRow][0].text))
            self.tableWidget_SharedFileByMySelf.setItem(indexRow, 1, QtGui.QTableWidgetItem(self.buf_XMLSharedFilesList[indexRow][1].text))
            self.tableWidget_SharedFileByMySelf.setItem(indexRow, 2, QtGui.QTableWidgetItem(self.buf_XMLSharedFilesList[indexRow][2].text))
            self.tableWidget_SharedFileByMySelf.setItem(indexRow, 3, QtGui.QTableWidgetItem(time.ctime(float(self.buf_XMLSharedFilesList[indexRow][5].text))))
            self.tableWidget_SharedFileByMySelf.setItem(indexRow, 4, QtGui.QTableWidgetItem(self.buf_XMLSharedFilesList[indexRow][3].text+":"+self.buf_XMLSharedFilesList[indexRow][4].text))
            self.tableWidget_SharedFileByMySelf.setItem(indexRow, 5, QtGui.QTableWidgetItem("0"))
            self.tableWidget_SharedFileByMySelf.setItem(indexRow, 6, QtGui.QTableWidgetItem("0"))
            indexRow += 1
            self.tableWidget_SharedFileByMySelf.insertRow(indexRow)
            intValueForBar += ValueForBarOffset
            self.progressBar_Shared.setValue(intValueForBar)
        self.progressBar_Shared.setValue(100)
        self.MsgToConsole("...Done")
    def AddFilesToSharedList(self):
        '''
        Metoda dodaje wskazane pliki i foldery do listy udostepnionych plikow.
        Nastepnie uzupelnia tabele plikow
        Wszystkie operacje sa wyswietlane na consoli oraz pokazywany jest pasek postepu
        '''
        FileDialog = QtGui.QFileDialog.getOpenFileNames(None,"Choose files and folders which you would like to share")
        intTempRet = 0
        intTempCout = 0
        intLenOfFileDialog =len(FileDialog)
        intLenOfBufferBeforeAdd = len(self.buf_XMLSharedFilesList)
        
        print FileDialog
        if intLenOfFileDialog > 0:
            self.MsgToConsole("Processing files...")
            intTempRet,  intTempCout = self.AddNodeToXMLFilesList(self.buf_XMLSharedFilesList,
                                                                  self.intSharedListCounter, 
                                                                  FileDialog, 
                                                                  self.comboBox_checkSums.currentText(),
                                                                  myConfigs.CountCheckSumWhenAdd
                                                                  ) 
            self.intSharedListCounter += (intLenOfFileDialog - intTempCout)
            print "SUPERWAZNE POZNIEJ", self.intSharedListCounter
            if intTempRet == 0:
                self.WriteXMLToFile(myConfigs.SharedFiles,self.buf_XMLSharedFilesList)
                if intTempCout == 0:
                    self.MsgToConsole("List Updated successfuly")
                else:
                    self.MsgToConsole("List Updated successfuly but "+str(intTempCout)+" elements were already there")
            elif intTempRet == 1:
                    self.MsgToConsole("List haven't been updated because all of "+str(intTempCout)+" elements were already there")
            else:
                print "List Update Faild unknown error"
        else:
            self.MsgToConsole("File added failed")
        #DEBUG
        if intLenOfFileDialog -  intTempCout > 0:
            self.InicializeSharedList(intLenOfBufferBeforeAdd)
            self.MsgToConsole("List has been Refreshed")
        #self.PrintXML(self.buf_XMLSharedFilesList)
        #
    ###Połaczenie
    def ConnectToServer(self):
        MethodName = self.ClassName+"[ConnectToServer]->"
        strRet =""
        if self.ConnectedAs > -1:
            if self.ConnectedAs == -2:
                self.MsgToConsole("Server refused")
            else:
                self.pushButton_ConnDiscon.setText(QtGui.QApplication.translate("TabWidget_MainFrame", "Połącz", None, QtGui.QApplication.UnicodeUTF8))
                self.myTelnet.TelnetDisconnect()
                self.MsgToConsole("Disconnected from "+myConfigs.Host+"::"+str(myConfigs.Port))
            del self.myTelnet
            self.ConnectedAs = -1
            QtCore.QObject.emit(self.QSignalObj1,QtCore.SIGNAL("AdminPanelSignal"))
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
                QtCore.QObject.emit(self.QSignalObj1,QtCore.SIGNAL("AdminPanelSignal"))
                self.MsgToConsole("Sending list...")
                print self.ClassName+"Error in refreshing Console"
                if self.myTelnet.SendList(self.buf_XMLSharedFilesList, self.progressBar_MainServerShared) == 0:
                    self.MsgToConsole("...done")
                else:
                    self.MsgToConsole("...FAILD")
    #odswierzyc layout
    def MsgToConsole(self,Msg1):
        self.listWidget_console.insertItem(self.ConsoleMsgCounter,QtCore.QString(time.strftime(myConfigs.ConsoleTimeFormat)+" "+Msg1))
        self.ConsoleMsgCounter+=1
    ####SINGALS
    def SetSignals(self):
        '''
        Podlaczam sygnal
        '''
        print self.ClassName+"Setting up Signals..."
        #MAINFRAME->Main
        QtCore.QObject.connect(self.QSignalObj1, QtCore.SIGNAL("AdminPanelSignal"), self.ButtonAdminPanelSignal)
        QtCore.QObject.connect(self.pushButton_ConnDiscon, QtCore.SIGNAL("clicked()"), self.ConnectToServer)
        QtCore.QObject.connect(self.pushButton_Search , QtCore.SIGNAL("clicked()"), self.SendSearchForFiles)
        QtCore.QObject.connect(self.pushButton_AdminPanel, QtCore.SIGNAL("clicked()"), self.ButtonAdminPanel)
        #RIGHTTOP
        #QtCore.QObject.connect(self.pushButton_Author, QtCore.SIGNAL("clicked()"), self.MsgBoxAbout)
        QtCore.QObject.connect(self.pushButton_Quit, QtCore.SIGNAL("clicked()"), self.MsgOnExit)
        #MAINFRAME->Shared 
        QtCore.QObject.connect(self.pushButton_ADD, QtCore.SIGNAL("clicked()"), self.AddFilesToSharedList)
        QtCore.QObject.connect(self.pushButton_Del, QtCore.SIGNAL("clicked()"), self.DelFilesFromSharedList)
        QtCore.QObject.connect(self.tableWidget_SharedFileByMySelf, QtCore.SIGNAL("itemSelectionChanged()"), self.TableWidgetSelectionList)
        QtCore.QObject.connect(self.pushButton_DELALL, QtCore.SIGNAL("clicked()"), self.PurgeSharedWidgetandXMLList)
        #MAINFRAME->Options
        QtCore.QObject.connect(self.pushButton_Shared, QtCore.SIGNAL("clicked()"), self.SetDirToSharedFile)
        QtCore.QObject.connect(self.buttonBox_Save, QtCore.SIGNAL("accepted()"), self.StoreConfigs)
        QtCore.QObject.connect(self.buttonBox_Save, QtCore.SIGNAL("rejected()"), self.SetConfigsOnPanel)
        QtCore.QObject.connect(self.buttonBox_Save, QtCore.SIGNAL("helpRequested()"), self.SetConfigsOnPanel)
        #MAINGRAME->ProjectSite
        QtCore.QObject.connect(self.horizontalSlider_Zoom, QtCore.SIGNAL("valueChanged(int)"), self.Slider_Zoom)	
        print self.ClassName+"...Done"
        ####MSG_BOXES
        ####Methods 
        ##Main
        #for buttons on LeftTop
    def ButtonAdminPanel(self):
        '''
        Pokazuje Panel admina
        '''
        print self.ClassName+"self.ConnectedAs[", self.ConnectedAs,"] DO POPRAWY"
        if self.ConnectedAs == 1:
            appTool = QtGui.QApplication
            self.ToolBox = QtGui.QToolBox()
            self.AdminPanel = classAdminPanel()
            self.AdminPanel.setupUi(self.ToolBox)
            self.ToolBox.show()
            if appTool.exec_() == 0:
                self.MsgToConsole("AdminPanel closed Succesfuly")
            else:
                self.MsgToConsole("AdminPanel closed with errors")
    #for buttons on RightTop
    def MsgBoxUniversal(self, strMessage):
        box = QtGui.QMessageBox(None)
        box.setWindowTitle(QtGui.QApplication.translate(None,"Informacja",None, QtGui.QApplication.UnicodeUTF8))
        box.setText(QtGui.QApplication.translate(None,strMessage,None, QtGui.QApplication.UnicodeUTF8))
        box.setStandardButtons(QtGui.QMessageBox.Ok)
        box.exec_()
    def ButtonAdminPanelSignal(self):
        '''
        Metoda w reakcji na sygnal pokaze lub ukryje przycisk panelu administracyjnego
        '''
        if self.ConnectedAs == 1:
            print self.ClassName+"[ButtonAdminPanel]->shown"
            self.pushButton_AdminPanel.show()
        else:
            print self.ClassName+"[ButtonAdminPanel]->hidden"
            self.pushButton_AdminPanel.hide()
        #
    def MsgOnExit(self):
        #del self.ToolBox
        if self.ConnectedAs > -1:
            print self.ClassName+"Disconnecting from server before Exit"
            self.myTelnet.TelnetDisconnect()
            del self.myTelnet
        print self.ClassName+"Bye Bye"
    ##Options
    def SetConfigsOnPanel(self):
        print self.ClassName+"Setting up Option lables..."
        ###Options
        ##Floating windows
        #Server
        self.lineEdit_IP.setText(QtCore.QString(myConfigs.Host))
        self.lineEdit_Port.setText(QtCore.QString(str(myConfigs.Port)))
        #Auth
        self.lineEdit_Login.setText(QtCore.QString(myConfigs.User))
        self.lineEdit_Password.setText(QtCore.QString(myConfigs.Password))
        #Validate
        self.checkBox_ValidateFileBeforeSend.setChecked(myConfigs.ValidateBeforeSend)
        self.checkBox_ValidateOnProgramStart.setChecked(myConfigs.ValidateFilesOnStarup)
        self.checkBox_CountHashWhenAdd.setChecked(myConfigs.CountCheckSumWhenAdd)
        self.checkBox_LetToDwnNoValidFiles.setChecked(myConfigs.LetDownloadFilesWhichFaildValidation)
        #Varuables
        self.checkBox_Debug.setChecked(myConfigs.Debug)
        self.lineEdit_PathSharedFile.setText(QtCore.QString(myConfigs.SharedFiles))
        #Version
        self.dockWidget_Save.setWindowTitle(QtGui.QApplication.translate("TabWidget_MainFrame", "Version.:"+myConfigs.ClientVersion, None, QtGui.QApplication.UnicodeUTF8))
        ###SiteProject
        self.spinBox_TimeOut.setValue(myConfigs.TimeOut)
        self.horizontalSlider_Zoom.setSliderPosition(myConfigs.Zoom)
        self.horizontalSlider_Zoom.setValue(myConfigs.Zoom)
        self.progressBar_Zoom.setValue(myConfigs.Zoom)
        #
        self.ButtonAdminPanelSignal()
        print self.ClassName+"...Done"
        #OpenFileButton
    def SetDirToSharedFile(self):
        '''
        Metoda, wyboru pliku z profilem udostepnianych plikow
        '''
        FileDialog = QtGui.QFileDialog.getOpenFileName(None,"Choose your shared file list","configs","*xml")
        self.lineEdit_PathSharedFile.setText(FileDialog)
    #for button bottom
    
    def TableWidgetSelectionList(self):
        pass
        
    def DelFilesFromSharedList(self):
        '''
        Kasuje z listy udostepnionych plikow, zaznaczone pozycje w Tabeli plikow
        '''
        #zaznaczone pliki listy
        #print "Lista:",  self.tableWidget_SharedFileByMySelf.selectedItems()
        #print "wielkosc:", len(self.tableWidget_SharedFileByMySelf.selectedItems())
        IndexTab = len(self.tableWidget_SharedFileByMySelf.selectedItems())
        intOffset = 0
        ListToDelete = []
        #self.PrintXML()
        for index in range(-1, IndexTab):
            #print "INDEX::", index
            if (index == -1 or (index+1)%7 == 0) and (index + 1 < IndexTab):
                print index,"usuwanieDBG",index,"[",  self.tableWidget_SharedFileByMySelf.selectedItems()[index].text(), ']'
                print index,"usuwanie",index,"[",  self.tableWidget_SharedFileByMySelf.selectedItems()[index+1].text(), ']'
                if self.tableWidget_SharedFileByMySelf.selectedItems()[index+1].text() != None:
                    ListToDelete.append(self.tableWidget_SharedFileByMySelf.selectedItems()[index+1].text())
                else:
                    print "Blad na elemencie", self.tableWidget_SharedFileByMySelf.selectedItems()[index-5].text(), ']'
                intOffset -= 1
        self.DeleteNodeToXMLFilesList(self.buf_XMLSharedFilesList, ListToDelete)
        self.WriteXMLToFile(myConfigs.SharedFiles, self.buf_XMLSharedFilesList)
        self.InicializeSharedList()
        self.MsgToConsole(str(len(ListToDelete))+" elements has been removed from shared list")
        #self.PrintXML(self.buf_XMLSharedFilesList)
    def StoreConfigs(self):
        '''
        Metoda pobiera z elementow gui wartosci i kopiuje je na miejsce zeminnych.
        Nastepnie wywoluje metode zapisywania ustawien i wysyla komunikat do konsoli
        '''
        MethodName = self.ClassName+"[StoreConfigs]->"
        myConfigs.Host = self.lineEdit_IP.displayText()
        myConfigs.Port = self.lineEdit_Port.displayText()
        myConfigs.User = self.lineEdit_Login.displayText()
        self.lineEdit_Password.setEchoMode(QtGui.QLineEdit.Normal)
        myConfigs.Password = self.lineEdit_Password.displayText()
        self.lineEdit_Password.setEchoMode(QtGui.QLineEdit.PasswordEchoOnEdit)
        myConfigs.TimeOut = int(self.spinBox_TimeOut.text())
        myConfigs.SharedFiles = self.lineEdit_PathSharedFile.displayText()
        myConfigs.HashType = self.comboBox_checkSums.currentText()
        myConfigs.ValidateBeforeSend = int(self.checkBox_ValidateFileBeforeSend.isChecked())
        myConfigs.ValidateFilesOnStarup = int(self.checkBox_ValidateOnProgramStart.isChecked())
        myConfigs.CountCheckSumWhenAdd = int(self.checkBox_CountHashWhenAdd.isChecked())
        myConfigs.LetDownloadFilesWhichFaildValidation = int(self.checkBox_LetToDwnNoValidFiles.isChecked())
        myConfigs.Zoom = self.progressBar_Zoom.value()
        myConfigs.Debug = int(self.checkBox_Debug.isChecked())
        if myConfigs.StoreCFG() == 0:
            self.MsgToConsole("Configs has been stored")
        else:
            self.MsgToConsole("Failed to store configs")
    def Slider_Zoom(self,value):
        '''
        Metoda dba jedynie o ustawieni odpowiednij skali okienka w, ktorym wyswietlam strone projektu
        '''
        self.progressBar_Zoom.setValue(value)
        self.webView_ProjectSite.setZoomFactor(float(value*0.01))
    def SendSearchForFiles(self):
        if self.myTelnet !=None:
            self.MsgToConsole("Asking server for ["+self.lineEdit_Search.displayText()+"]")
            retcode = self.myTelnet.AskServerForFiles(str(self.lineEdit_Search.displayText()))
            if retcode == 0:
                self.MsgToConsole("List recived")
                self.buf_XMLSerwerFilesList = GetXMLListFromServer()
            elif retcode == 1:
                self.MsgToConsole("File not found")
            else:
                self.MsgToConsole("Connection Lost")
        else:
            self.MsgToConsole("You are not connected to any server")
            

def myClient():
    '''
    Metoda w ktorej dzieje sie inicjaliza Qt4
    '''
    intTempRet = 0
    intTempCout = 0
    #'''
    app = QtGui.QApplication(sys.argv)
    MainFrame = classMainFrame()
    TabWidget = QtGui.QTabWidget()
    MainFrame.setupUi(TabWidget)
    MainFrame.SetUpObjects()
    TabWidget.show()
    #'''

    return app.exec_()

