#! /usr/bin/env python
# -*- coding: utf-8 -*-

__author__ =  'Lukasz Busko'                
__email__ = 'buskol.waw.pl@gmail.com'
__version__= '0.1.3'
__licenes__= 'Licenes GPL'  

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
    import myComm
    import myxml
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

class classMainFrame(importMainFrame):
    def __init__(self):
        self.ClassName = "[classMainFrame]->"
        print self.ClassName
        self.ConnectedAs = -1
        self.intSharedListCounter = 0
        self.ConsoleMsgCounter = 0
        self.QSignalObj1 = QtCore.QObject(None)
        self.QSignalObjServerList = QtCore.QObject(None)
        self.QSignalObjMsgToConsole = QtCore.QObject(None)
        self.QSignalObjProgressBarAdd = QtCore.QObject(None)
        self.xmlListFromServer = myxml.myXmlDoc(myConfigs)
        self.xmlListSharedFiles = myxml.myXmlDoc(myConfigs)
        self.mySocket = None
        self.LockSending = 1
        self.kb = 1024
        self.mb =1024*1024
        self.gb= self.mb * 1024

    def SetConfigsOnPanel(self):
        '''
        Ustawia wlasciwe wartosci startowe dla poszczegolnych elementow
        '''
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


    ####SINGALS
    def SetSignals(self):
        '''
        Podlaczam sygnal
        '''
        print self.ClassName+"Setting up Signals..."
        #moje
        QtCore.QObject.connect(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"), self.MsgToConsole)
        QtCore.QObject.connect(self.QSignalObjServerList,  QtCore.SIGNAL("RefreshTableWidget"),self.InicializedSharedServerList)
        QtCore.QObject.connect(self.QSignalObjProgressBarAdd,  QtCore.SIGNAL("RefreshProgressBarAdd"), self.ProgressBarFileAdd)
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
        
    def SetUpObjects(self):
        '''
        Inicjuje ustawienia programu
        '''
        self.SetConfigsOnPanel()
        self.SetSignals()
        QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Welcome")
        self.XMLLIstFromFIle()
        if self.xmlListSharedFiles.GetNumberOfChildren() >0:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Loading "+str(self.xmlListSharedFiles.GetNumberOfChildren())+" sharedfiles...")
        #self.tab_Options.hide()
        self.InicializeSharedList()

    def SetDirToSharedFile(self):
        '''
        Metoda, wyboru pliku z profilem udostepnianych plikow
        '''
        FileDialog = QtGui.QFileDialog.getOpenFileName(None,"Choose your shared file list","configs","*xml")
        self.lineEdit_PathSharedFile.setText(FileDialog)
    #for button bottom
    ###XML LISTY
    def RSize(self, size):
        '''
        Przelicza rozmiar i zwraca w odpowiednio czytelnych dla ludzi jednostkach
        '''
        if size < self.kb:
            return str(size)+"b"
        elif size < self.mb:
            return str (size / self.kb)+"kb"
        elif size < self.gb:
            return str(size / self.mb)+"mb"
        else:
            return str(size / self.gb)+"gb"
        
    def XMLLIstFromFIle(self):
        '''
        Sprawdza czy jest ustawiona wlasciwa sciezka dostepu do pliku
        Tworzy pliku profilu udostepniania oraz validuje go
        '''
        print self.ClassName+"Trying to load xmlfile..."
        if myConfigs.SharedFiles == "":                                                                              #tworze plik w sciezce config/shared.xml
            print self.ClassName+"Path haven't been set, using default path"
            myConfigs.SharedFiles = "/config/shared.xml"
            self.lineEdit_PathSharedFile.setText(QtCore.QString(myConfigs.SharedFiles))
            self.MsgBoxUniversal("Using default shared file profile")
            #self.MsgBoxUniversal(u"Ustaw sciżkę do pliki , wktórym mają być lub są trzymane akutalnie udostępniane pliki")
        else:
            if (self.xmlListSharedFiles.CreateDocFromFile()!=0):
                self.xmlListSharedFiles.CreateDoc("SharedFiles")
            else:
                self.xmlListSharedFiles.ValidateFiles()
        print self.ClassName+"...Done"
    def PurgeSharedWidgetandXMLList(self):
        '''
        Czysci liste xml, tablice oraz zapisuje do pliku zmiany
        '''
        self.PurgeAnyList(self.xmlListSharedFiles.GetNumberOfChildren(), self.tableWidget_SharedFileByMySelf)
        self.xmlListSharedFiles.PurgeNodes()
        self.xmlListSharedFiles.addElement("SharedFiles", None, 1, None)
        self.xmlListSharedFiles.WriteXMLDocToFile()
        QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Shared list has been cleared")
    def PurgeServerWidgetandXMLList(self):
        '''
        Czysci liste xml, tablice
        '''
        self.PurgeAnyList(self.xmlListFromServer, self.tableWidget_SharedByServer)
        self.PurgeXMLList(self.xmlListFromServer)
        self.intServerListCounter = 0
        QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Server list has been cleared")
    def PurgeAnyList(self, len, objTableWidget):
        '''
        Kasuje liste elementow widgeta tablicy
        '''
        intLenOfBuffer = len
        intRowCount = objTableWidget.rowCount()
        for index in range(1, intRowCount):
            objTableWidget.removeRow(objTableWidget.rowCount()-1)
            #QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"...Done")
        objTableWidget.clearContents
                
    def InicializeSharedList(self, intOffset =0):
        if self.xmlListSharedFiles != None:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Updating Shared list Widget")
            intLenOfBuffer = self.xmlListSharedFiles.GetNumberOfChildren()
            intRowCount = self.tableWidget_SharedFileByMySelf.rowCount()
            
            indexRow = +intOffset
#            if intLenOfBuffer >= indexRow:
#                self.tableWidget_SharedFileByMySelf.setItem(indexRow, 0, QtGui.QTableWidgetItem(""))
#                self.tableWidget_SharedFileByMySelf.setItem(indexRow, 1, QtGui.QTableWidgetItem(""))
#                self.tableWidget_SharedFileByMySelf.setItem(indexRow, 2, QtGui.QTableWidgetItem(""))
#                self.tableWidget_SharedFileByMySelf.setItem(indexRow, 3, QtGui.QTableWidgetItem(""))
#                self.tableWidget_SharedFileByMySelf.setItem(indexRow, 4, QtGui.QTableWidgetItem(""))
#                self.tableWidget_SharedFileByMySelf.setItem(indexRow, 5, QtGui.QTableWidgetItem(""))
#                self.tableWidget_SharedFileByMySelf.setItem(indexRow, 6, QtGui.QTableWidgetItem(""))
            intValueForBar = 0
            if intLenOfBuffer == 0:
                ValueForBarOffset = 100
            else:
                ValueForBarOffset = 100/intLenOfBuffer
            if intLenOfBuffer > 100:
                ValueForBarOffset = intLenOfBuffer/100
            self.progressBar_Shared.setValue(intValueForBar)
            while indexRow < intLenOfBuffer:
                ElementList = self.xmlListSharedFiles.GetNodeChildrenAsList(indexRow, 6)
                if ElementList != []:
                    self.tableWidget_SharedFileByMySelf.setItem(indexRow, 0, QtGui.QTableWidgetItem(ElementList[0]))
                    self.tableWidget_SharedFileByMySelf.setItem(indexRow, 1, QtGui.QTableWidgetItem(ElementList[1]))
                    self.tableWidget_SharedFileByMySelf.setItem(indexRow, 2, QtGui.QTableWidgetItem(self.RSize(int(ElementList[2]))))
                    self.tableWidget_SharedFileByMySelf.setItem(indexRow, 3, QtGui.QTableWidgetItem(time.ctime(float(ElementList[5]))) )
                    self.tableWidget_SharedFileByMySelf.setItem(indexRow, 4, QtGui.QTableWidgetItem(ElementList[3]+":"+ElementList[4]))
                    self.tableWidget_SharedFileByMySelf.setItem(indexRow, 5, QtGui.QTableWidgetItem("0"))
                    self.tableWidget_SharedFileByMySelf.setItem(indexRow, 6, QtGui.QTableWidgetItem("0"))
                    indexRow += 1
                    self.tableWidget_SharedFileByMySelf.insertRow(indexRow)
                    intValueForBar += ValueForBarOffset
                    self.progressBar_Shared.setValue(intValueForBar)
                else:
                    break
            self.progressBar_Shared.setValue(100)
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"...Done")
            self.LockSending = 0
        else:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Error: SharedList is Empty")
    def InicializedSharedServerList(self, intOffset =0):
        if self.xmlListFromServer != None:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Updating Shared list Widget")
            intLenOfBuffer = self.xmlListFromServer.GetNumberOfChildren()
            intRowCount = self.tableWidget_SharedByServer.rowCount()
            self.PurgeAnyList(self.xmlListFromServer, self.tableWidget_SharedByServer)
        
            indexRow = 0
            intValueForBar = 0
            if intLenOfBuffer == 0:
                ValueForBarOffset = 100
            else:
                ValueForBarOffset = 100/intLenOfBuffer
            if intLenOfBuffer > 100:
                ValueForBarOffset = intLenOfBuffer/100
            self.progressBar_MainServerShared.setValue(intValueForBar)
            while indexRow < intLenOfBuffer:
                ElementList = self.xmlListFromServer.GetNodeChildrenAsList(indexRow, 6)
                if ElementList != []:
                    self.tableWidget_SharedByServer.setItem(indexRow, 0, QtGui.QTableWidgetItem(ElementList[0]))
                    self.tableWidget_SharedByServer.setItem(indexRow, 1, QtGui.QTableWidgetItem(ElementList[1]))
                    self.tableWidget_SharedByServer.setItem(indexRow, 2, QtGui.QTableWidgetItem(self.RSize(int(ElementList[2]))))
                    self.tableWidget_SharedByServer.setItem(indexRow, 3, QtGui.QTableWidgetItem(time.ctime(float(ElementList[5]))))
                    self.tableWidget_SharedByServer.setItem(indexRow, 4, QtGui.QTableWidgetItem(ElementList[3]+":"+ElementList[4]))
                    indexRow += 1
                    self.tableWidget_SharedByServer.insertRow(indexRow)
                    intValueForBar += ValueForBarOffset
                    self.progressBar_MainServerShared.setValue(intValueForBar)
            self.progressBar_MainServerShared.setValue(100)
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"...Done")
        else:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Error: XMLServerList is Empty")
        
    def AddFilesToSharedList(self):
        '''
        Metoda dodaje wskazane pliki i foldery do listy udostepnionych plikow.
        Nastepnie uzupelnia tabele plikow
        Wszystkie operacje sa wyswietlane na consoli oraz pokazywany jest pasek postepu
        '''
        QFileDialog = QtGui.QFileDialog.getOpenFileNames(None,"Choose files and folders which you would like to share")
        intTempRet = 0
        intTempCount = 0
        FileDialog = []
        for i in QFileDialog:
            FileDialog.append(str(i))
        del QFileDialog
        intLenOfFileDialog =len(FileDialog)
        
        if intLenOfFileDialog > 0:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Processing files...")
            intOffset = self.xmlListSharedFiles.GetNumberOfChildren()
            intTempCount, intTempRet = self.xmlListSharedFiles.ValidateIfFilesAreAlreadyThere(FileDialog)
            self.xmlListSharedFiles.FileListInsertion(FileDialog)
            self.xmlListSharedFiles.WriteXMLDocToFile()
            self.intSharedListCounter += intTempCount
            if intTempRet >= intTempCount:
                QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"List Updated successfuly "+str(intTempCount)+"/"+str(intTempRet)+" elements has been add")
            else:
                QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"), "List Update Faild unknown error")
        else:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"File haven't been choosen")
        #DEBUG
        if intTempCount > 0:
            self.InicializeSharedList(intOffset)
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"List has been Refreshed")
        #
    ###Połaczenie
    def ConnectToServer(self):
        '''
        Laczy z serwerem oraz updatuje przyciski i console
        '''
        MethodName = self.ClassName+"[ConnectToServer]->"
        strRet =""
        if self.ConnectedAs > -1:
            #if self.ConnectedAs == -2:
            #    QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Server refused")
            #else:
            self.pushButton_ConnDiscon.setText(QtGui.QApplication.translate("TabWidget_MainFrame", "Połącz", None, QtGui.QApplication.UnicodeUTF8))
            #self.myTelnet.TelnetDisconnect()
            self.mySocket.myDisconnect()
            self.mySocket.join()
            del self.mySocket
            self.mySocket = None
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Disconnected from "+myConfigs.Host+"::"+str(myConfigs.Port))
            #del self.myTelnet
            #self.mySocket.join()
            #del mySocket
            self.ConnectedAs = -1
            #QtCore.QObject.emit(self.QSignalObj1,QtCore.SIGNAL("AdminPanelSignal"))
        else:
            #self.myTelnet = myTelnet.myTelnetParser(myConfigs,self.listWidget_console)
            self.mySocket = myComm.mySocketComm(myConfigs, self)
            self.mySocket.start()
            #self.mySocket.run()
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Connecting to "+myConfigs.Host+"::"+str(myConfigs.Port))
            self.ConnectedAs = 10
            #self.ConnectedAs = self.myTelnet.AmIRoot()
            #if self.ConnectedAs > -1:
#                self.pushButton_ConnDiscon.setText(QtGui.QApplication.translate("TabWidget_MainFrame", "Rozłącz", None, QtGui.QApplication.UnicodeUTF8))
#                self.LocalServerTime = self.myTelnet.AskForServerTime()
#                QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Connected and authorized successful, local server time is: "+str(self.LocalServerTime))
#                QtCore.QObject.emit(self.QSignalObj1,QtCore.SIGNAL("AdminPanelSignal"))
#                QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Sending list...")
#                print self.ClassName+"Error in refreshing Console"
#                if self.myTelnet.SendList(self.buf_XMLSharedFilesList, self.progressBar_MainServerShared) == 0:
#                    QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"...done")
#                else:
#                    QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"...FAILD")
#            else:
#                self.myTelnet.TelnetDisconnect()
#                del self.myTelnet
#                if self.ConnectedAs == -1:
#                    QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Authorization failed, client is going to disconnect it self from server "+myConfigs.Host+"::"+str(myConfigs.Port))
#                elif self.ConnectedAs == -2:
#                    QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Server is not reachable "+myConfigs.Host+"::"+str(myConfigs.Port))
#                else:
#                    QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Unsupported and unkown error"+myConfigs.Host+"::"+str(myConfigs.Port))
    #odswierzyc layout
    def MsgToConsole(self,Msg1):
        '''
        Wpisuje odpowidenia wiadomosc do widgeta consoli oraz ustawia na niej focus consoli
        '''
        item = QtGui.QListWidgetItem(QtCore.QString(time.strftime(myConfigs.ConsoleTimeFormat)+" "+Msg1))
        self.listWidget_console.insertItem(self.ConsoleMsgCounter, item)
        self.listWidget_console.scrollToItem(item)
        self.ConsoleMsgCounter+=1

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
                QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"AdminPanel closed Succesfuly")
            else:
                QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"AdminPanel closed with errors")
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
            if self.mySocket != None:
                self.mySocket.myDisconnect()
                self.mySocket.join()
                del self.mySocket
        print self.ClassName+"Bye Bye"
    ##Options

    
    def TableWidgetSelectionList(self):
        pass
        
    def DelFilesFromSharedList(self):
        '''
        Kasuje z listy udostepnionych plikow, zaznaczone pozycje w Tabeli plikow
        '''
        IndexTab = len(self.tableWidget_SharedFileByMySelf.selectedItems())
        intOffset = 0
        ListToDelete = []
        for index in range(-1, IndexTab):
            if (index == -1 or (index+1)%7 == 0) and (index + 1 < IndexTab):
                if self.tableWidget_SharedFileByMySelf.selectedItems()[index+1].text() != None:
                    ListToDelete.append(str(self.tableWidget_SharedFileByMySelf.selectedItems()[index+1].text()))
                else:
                    print "Blad na elemencie", self.tableWidget_SharedFileByMySelf.selectedItems()[index-5].text(), ']'
                intOffset -= 1
        self.xmlListSharedFiles.RemoveChildren(ListToDelete)
        self.xmlListSharedFiles.WriteXMLDocToFile()
        self.PurgeAnyList(self.xmlListSharedFiles.GetNumberOfChildren(), self.tableWidget_SharedFileByMySelf)
        self.InicializeSharedList()
        QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),str(len(ListToDelete))+" elements has been removed from shared list")

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
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Configs has been stored")
        else:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Failed to store configs")
    def Slider_Zoom(self,value):
        '''
        Metoda dba jedynie o ustawieni odpowiednij skali okienka w, ktorym wyswietlam strone projektu
        '''
        self.progressBar_Zoom.setValue(value)
        self.webView_ProjectSite.setZoomFactor(float(value*0.01))
    def ProgressBarFileAdd(self, value):
        self.progressBar_Shared.setValue(value)
    def SendSearchForFiles(self):
        '''
        Sprawdza czy serwer dziala jezeli tak: 
        Inicjuje wyszukiwanie plikow. 
        Jezeli wyszukiwanie sie powidolo tworzona jest lista xml do przetworzenia
        Wlacza sie updatowanie WidgetaTablicy trzymajacego wyniku poprzedniego szukania
        Uruchamiane sa metody updatowania consoli
        '''
        if self.mySocket !=None:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Asking server for ["+self.lineEdit_Search.displayText()+"]")
            self.mySocket.AskServerForFiles(str(self.lineEdit_Search.displayText()))
        else:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"You are not connected to any server")       

def myClient():
    '''
    Metoda w ktorej dzieje sie inicjaliza Qt4
    '''
    #'''
    app = QtGui.QApplication(sys.argv)
    MainFrame = classMainFrame()
    TabWidget = QtGui.QTabWidget()
    MainFrame.setupUi(TabWidget)
    MainFrame.SetUpObjects()
    TabWidget.show()
    #'''

    return app.exec_()

