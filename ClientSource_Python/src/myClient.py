#! /usr/bin/env python
# -*- coding: utf-8 -*-

__author__ =  'Lukasz Busko'                
__email__ = 'buskol.waw.pl@gmail.com'
__version__= '0.2.1'
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
    import myComm
    import myxml
    import myAdminPanel
    import tableDownloadContextMenu
    myConfigs = myConfigs.myConfigs()
    myConfigs.LoadCFG()
    from MainFrame import Ui_MainWindow as importMainWindow
except ImportError, error:
    print "[myClient]->", error
    time.sleep(2)
    sys.exit(1)

####VERSION CHECKS
myConfigs.CheckRequirements()
if myConfigs.Debug == 0:
    stdSilent = open(os.devnull, 'w')
    sys.stdout = stdSilent
    sys.stderr = stdSilent
    sys.stdin = open(os.devnull, "r")

class classMainFrame(QtGui.QMainWindow):
    def __init__(self, parent = None):
        QtGui.QWidget.__init__(self, parent)
        self.ClassName = "[classMainFrame]->"
        print self.ClassName
        self.ConnectedAs = -1
        self.intSharedTableCounter = 0
        self.ConsoleMsgCounter = 0
        self.xmlDownloadList = myxml.myXmlDoc(myConfigs)
        self.xmlSearchList = myxml.myXmlDoc(myConfigs)
        self.xmlSharedList = myxml.myXmlDoc(myConfigs)
        self.mySocket = None
        self.myAdminPanel = None
        self.MainWindowUI = importMainWindow()
        self.TrayIcon = QtGui.QSystemTrayIcon(QtGui.QIcon("img/tux_linux_inside_48.png"), self)
        self.TrayMenu = QtGui.QMenu()
        self.LockSending = 0
        self.kb = 1024
        self.mb =1024*1024
        self.gb= self.mb * 1024

    def SetConfigsOnPanel(self):
        """Ustawia wlasciwe wartosci startowe dla poszczegolnych elementow"""
        print self.ClassName+"Setting up Option lables..."
        ###Options
        ##Floating windows
        #Try
        self.setWindowIcon(QtGui.QIcon("img/tux_linux_inside_48.png"))
        self.TrayMenu.addAction("Exit",  self.MsgOnExit)
        self.TrayIcon.setContextMenu(self.TrayMenu)
        self.TrayIcon.show()
        #Server
        self.MainWindowUI.lineEdit_IP.setText(QtCore.QString(myConfigs.Host))
        self.MainWindowUI.lineEdit_Port.setText(QtCore.QString(str(myConfigs.Port)))
        #Auth
        self.MainWindowUI.lineEdit_Login.setText(QtCore.QString(myConfigs.User))
        self.MainWindowUI.lineEdit_Password.setText(QtCore.QString(myConfigs.Password))
        #Validate
        self.MainWindowUI.checkBox_ValidateFileBeforeSend.setChecked(myConfigs.ValidateBeforeSend)
        self.MainWindowUI.checkBox_ValidateOnProgramStart.setChecked(myConfigs.ValidateFilesOnStarup)
        self.MainWindowUI.checkBox_CountHashWhenAdd.setChecked(myConfigs.CountCheckSumWhenAdd)
        self.MainWindowUI.checkBox_LetToDwnNoValidFiles.setChecked(myConfigs.LetDownloadFilesWhichFaildValidation)
        #Varuables
        self.MainWindowUI.checkBox_Debug.setChecked(myConfigs.Debug)
        self.MainWindowUI.lineEdit_PathSharedFile.setText(QtCore.QString(myConfigs.SharedFiles))
        #Version
        self.MainWindowUI.dockWidget_Save.setWindowTitle(QtGui.QApplication.translate("TabWidget_MainFrame", "Version.:"+myConfigs.ClientVersion, None, QtGui.QApplication.UnicodeUTF8))
        ###SiteProject
        self.MainWindowUI.spinBox_TimeOut.setValue(myConfigs.TimeOut)
        self.MainWindowUI.horizontalSlider_Zoom.setSliderPosition(myConfigs.Zoom)
        self.MainWindowUI.horizontalSlider_Zoom.setValue(myConfigs.Zoom)
        self.MainWindowUI.progressBar_Zoom.setValue(myConfigs.Zoom)
        #
        self.ButtonAdminPanelSignal()
        print self.ClassName+"...Done"
        #OpenFileButton


    ####SINGALS
    def SetSignals(self):
        """Podlaczam sygnal"""
        print self.ClassName+"Setting up Signals..."
        self.QSignalObj1 = QtCore.QObject(None)
        self.QSignalObjServerList = QtCore.QObject(None)
        self.QSignalObjMsgToConsole = QtCore.QObject(None)
        self.QSignalObjProgressBarAdd = QtCore.QObject(None)
        self.QSignalObjPurgeTableDownload = QtCore.QObject(None)
        self.QSignalObjDeleteFromTableDownload = QtCore.QObject(None)
        #moje
        QtCore.QObject.connect(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"), self.MsgToConsole)
        QtCore.QObject.connect(self.QSignalObjServerList,  QtCore.SIGNAL("RefreshTableWidget"),self.InicializeSearchTable)
        QtCore.QObject.connect(self.QSignalObjProgressBarAdd,  QtCore.SIGNAL("RefreshProgressBarAdd"), self.ProgressBarFileAdd)
        #MAINFRAME->Main
        QtCore.QObject.connect(self.QSignalObj1, QtCore.SIGNAL("AdminPanelSignal"), self.ButtonAdminPanelSignal)
        QtCore.QObject.connect(self.MainWindowUI.pushButton_ConnDiscon, QtCore.SIGNAL("clicked()"), self.ConnectToServer)
        QtCore.QObject.connect(self.MainWindowUI.pushButton_Search , QtCore.SIGNAL("clicked()"), self.SendSearchForFiles)
        QtCore.QObject.connect(self.MainWindowUI.pushButton_AdminPanel, QtCore.SIGNAL("clicked()"), self.ButtonAdminPanel)
        QtCore.QObject.connect(self.MainWindowUI.pushButton_DNSelected, QtCore.SIGNAL("clicked()"), self.AddToDownloadTable)
        #RIGHTTOP
        #QtCore.QObject.connect(self.pushButton_Author, QtCore.SIGNAL("clicked()"), self.MsgBoxAbout)
        QtCore.QObject.connect(self.MainWindowUI.pushButton_Quit, QtCore.SIGNAL("clicked()"), self.bClose)
        #MAINFRAME->Shared 
        QtCore.QObject.connect(self.MainWindowUI.pushButton_ADD, QtCore.SIGNAL("clicked()"), self.AddFilesToSharedTable)
        QtCore.QObject.connect(self.MainWindowUI.pushButton_Del, QtCore.SIGNAL("clicked()"), self.DelFilesFromSharedTable)
        #QtCore.QObject.connect(self.MainWindowUI.tableWidget_Shared, QtCore.SIGNAL("itemSelectionChanged()"), self.TableWidgetSelectionList)
        QtCore.QObject.connect(self.MainWindowUI.pushButton_DELALL, QtCore.SIGNAL("clicked()"), self.PurgeSharedWidgetandXMLList)
        #MAINFRAME->Download
        self.MainWindowUI.tableWidget_Download.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.TableDownloadContex = tableDownloadContextMenu.t_dl_ContextMenu(self.MainWindowUI.tableWidget_Download, self)        
        QtCore.QObject.connect(self.MainWindowUI.tableWidget_Download, QtCore.SIGNAL('customContextMenuRequested(const QPoint&)'), self.TableDownloadContex.ContexMenu)
        QtCore.QObject.connect(self.QSignalObjDeleteFromTableDownload,  QtCore.SIGNAL("DeleteFromDownloadTable"), self.DelFilesFromDownloadTable)
        QtCore.QObject.connect(self.QSignalObjPurgeTableDownload,  QtCore.SIGNAL("PurgeDownloadTable"), self.PurgeDownloadWidgetandXMLList)
        #MAINFRAME->Options
        QtCore.QObject.connect(self.MainWindowUI.pushButton_Shared, QtCore.SIGNAL("clicked()"), self.SetDirToSharedFile)
        QtCore.QObject.connect(self.MainWindowUI.buttonBox_Save, QtCore.SIGNAL("accepted()"), self.StoreConfigs)
        QtCore.QObject.connect(self.MainWindowUI.buttonBox_Save, QtCore.SIGNAL("rejected()"), self.SetConfigsOnPanel)
        QtCore.QObject.connect(self.MainWindowUI.buttonBox_Save, QtCore.SIGNAL("helpRequested()"), self.SetConfigsOnPanel)
        #MAINGRAME->ProjectSite
        QtCore.QObject.connect(self.MainWindowUI.horizontalSlider_Zoom, QtCore.SIGNAL("valueChanged(int)"), self.Slider_Zoom)	
        print self.ClassName+"...Done"
        ####MSG_BOXES
        ####Methods 
        ##Main
        #for buttons on LeftTop
    def Clicked(self):
        print "Nacisnieto"
        
    def SetUpObjects(self):
        """Inicjuje ustawienia programu"""
        if self.MainWindowUI != None:
            self.MainWindowUI.setupUi(self)
            self.setFocusPolicy(QtCore.Qt.StrongFocus)
            self.show()
            self.SetConfigsOnPanel()
            self.SetSignals()
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Welcome")
            self.XMLLIstFromFIle()
            if self.xmlSharedList.GetNumberOfChildren() >0:
                QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Loading "+str(self.xmlSharedList.GetNumberOfChildren())+" sharedfiles...")
            self.InicializeSharedTable()
            self.InicializeDownloadTable()
            
            #self.MainWindowUI.tableWidget_Download.setContextMenu(self.TrayMenu)

    def SetDirToSharedFile(self):
        """Metoda, wyboru pliku z profilem udostepnianych plikow"""
        FileDialog = QtGui.QFileDialog.getOpenFileName(None,"Choose your shared file list","configs","*xml")
        self.MainWindowUI.lineEdit_PathSharedFile.setText(FileDialog)
    #for button bottom
    ###XML LISTY
    def RSize(self, size):
        """Przelicza rozmiar i zwraca w odpowiednio czytelnych dla ludzi jednostkach"""
        if size < self.kb:
            return str(size)+"b"
        elif size < self.mb:
            return str (size / self.kb)+"kb"
        elif size < self.gb:
            return str(size / self.mb)+"mb"
        else:
            return str(size / self.gb)+"gb"
        
    def XMLLIstFromFIle(self):
        """Sprawdza czy jest ustawiona wlasciwa sciezka dostepu do pliku
        Tworzy pliku profilu udostepniania oraz validuje go"""
        print self.ClassName+"Trying to load xmlSharedfiles..."
        if myConfigs.SharedFiles == "":                                                                              #tworze plik w sciezce config/shared.xml
            print self.ClassName+"Path haven't been set, using default path"
            myConfigs.SharedFiles = "/config/shared.xml"
            self.MainWindowUI.lineEdit_PathSharedFile.setText(QtCore.QString(myConfigs.SharedFiles))
            self.MsgBoxUniversal("Using default shared file profile")
            #self.MsgBoxUniversal(u"Ustaw sciżkę do pliki , wktórym mają być lub są trzymane akutalnie udostępniane pliki")
        else:
            if (self.xmlSharedList.CreateDocFromFile()!=0):
                self.xmlSharedList.CreateDoc("SharedFiles")
            else:
                self.xmlSharedList.ValidateFiles()
        print self.ClassName+"...Done"
        print self.ClassName+"Trying to load xmlDownloads..."
        if(self.xmlDownloadList.CreateDocFromFile(myConfigs.DownLoad, 1)!=0):
            self.xmlDownloadList.CreateDoc("DownloadFiles")
        print self.ClassName+"Done"
    def PurgeSharedWidgetandXMLList(self):
        """Czysci liste xml, tablice oraz zapisuje do pliku zmiany"""
        self.PurgeAnyList(self.xmlSharedList.GetNumberOfChildren(), self.MainWindowUI.tableWidget_Shared)
        self.xmlSharedList.PurgeNodes()
        self.xmlSharedList.addElement("SharedFiles", None, 1, None)
        self.xmlSharedList.WriteXMLDocToFile()
        self.InicializeSharedTable()
        QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"SharedTable has been cleared")
    def PurgeServerWidgetandXMLList(self):
        """Czysci liste xml, tablice"""
        self.PurgeAnyList(self.xmlSearchList, self.MainWindowUI.tableWidget_Shared)
        self.PurgeXMLList(self.xmlSearchList)
        self.intServerListCounter = 0
        QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"SerachTable has been cleared")
    def PurgeDownloadWidgetandXMLList(self):
        """Czysci liste xml, tablice oraz zapisuje do pliku zmiany"""
        self.PurgeAnyList(self.xmlDownloadList.GetNumberOfChildren(), self.MainWindowUI.tableWidget_Download)
        self.xmlDownloadList.PurgeNodes()
        self.xmlDownloadList.addElement("DownloadFiles", None, 1, None)
        self.xmlDownloadList.WriteXMLDocToFile()
        self.InicializeDownloadTable()
        QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"DownloadTable has been cleared")
    def PurgeAnyList(self, len, objTableWidget):
        """Kasuje liste elementow widgeta tablicy"""
        intLenOfBuffer = len
        intRowCount = objTableWidget.rowCount()
        for index in range(1, intRowCount):
            objTableWidget.removeRow(objTableWidget.rowCount()-1)
            #QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"...Done")
        objTableWidget.clearContents
                
    def InicializeSharedTable(self, intOffset =0): 
        """Metoda wstawia/updatuje elementy w tablicy pliku odeslanych przez serwer.
        Ponadto dba o wlasciwe przedstawianie pracy na pasku postepu
        Ustawia wlasciwy rozmiar kolumn
        Zeruje element 0
        """
        if self.xmlSharedList != None:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Updating SharedTable Widget")
            intLenOfBuffer = self.xmlSharedList.GetNumberOfChildren()
            intRowCount = self.MainWindowUI.tableWidget_Shared.rowCount()
            self.MainWindowUI.tableWidget_Shared.hide()
            indexRow = +intOffset
            if intLenOfBuffer >= indexRow:
                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 0, QtGui.QTableWidgetItem(""))
                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 1, QtGui.QTableWidgetItem(""))
                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 2, QtGui.QTableWidgetItem(""))
                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 3, QtGui.QTableWidgetItem(""))
                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 4, QtGui.QTableWidgetItem(""))
                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 5, QtGui.QTableWidgetItem(""))
                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 6, QtGui.QTableWidgetItem(""))
            intValueForBar = 0
            if intLenOfBuffer == 0:
                ValueForBarOffset = 100
            else:
                ValueForBarOffset = 100/intLenOfBuffer
            if intLenOfBuffer > 100:
                ValueForBarOffset = intLenOfBuffer/100
            self.MainWindowUI.progressBar_Shared.setValue(intValueForBar)
            while indexRow < intLenOfBuffer:
                ElementList = self.xmlSharedList.GetNodeChildrenAsList(indexRow, 6)
                if ElementList != []:
                    self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 0, QtGui.QTableWidgetItem(ElementList[0]))
                    self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 1, QtGui.QTableWidgetItem(ElementList[1]))
                    self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 2, QtGui.QTableWidgetItem(self.RSize(int(ElementList[2]))))
                    self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 3, QtGui.QTableWidgetItem(time.ctime(float(ElementList[5]))))
                    self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 4, QtGui.QTableWidgetItem(ElementList[3]+":"+ElementList[4]))#.setColumnWidth(100)
                    self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 5, QtGui.QTableWidgetItem("0"))
                    self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 6, QtGui.QTableWidgetItem("0"))
                    indexRow += 1
                    self.MainWindowUI.tableWidget_Shared.insertRow(indexRow)
                    intValueForBar += ValueForBarOffset
                    self.MainWindowUI.progressBar_Shared.setValue(intValueForBar)
                else:
                    break
            self.MainWindowUI.tableWidget_Shared.resizeColumnToContents (3) 
            self.MainWindowUI.tableWidget_Shared.resizeColumnToContents (4)
            self.MainWindowUI.tableWidget_Shared.show() 
            self.MainWindowUI.progressBar_Shared.setValue(100)
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"...Done")
            self.LockSending = 0
        else:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Error: SharedTable is Empty")
    def InicializeSearchTable(self, intOffset =0):
        """Metoda wstawia/updatuje elementy w tablicy pliku odeslanych przez serwer.
        Ponadto dba o wlasciwe przedstawianie pracy na pasku postepu
        Ustawia wlasciwy rozmiar kolumn
        """
        if self.xmlSearchList != None:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Updating SearchTable Widget")
            intLenOfBuffer = self.xmlSearchList.GetNumberOfChildren()
            intRowCount = self.MainWindowUI.tableWidget_Search.rowCount()
            self.PurgeAnyList(self.xmlSearchList, self.MainWindowUI.tableWidget_Search)
            self.MainWindowUI.tableWidget_Search.hide()
            
#            if intLenOfBuffer >= indexRow:
#                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 0, QtGui.QTableWidgetItem(""))
#                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 1, QtGui.QTableWidgetItem(""))
#                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 2, QtGui.QTableWidgetItem(""))
#                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 3, QtGui.QTableWidgetItem(""))
#                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 4, QtGui.QTableWidgetItem(""))
#                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 5, QtGui.QTableWidgetItem(""))
#                self.MainWindowUI.tableWidget_Shared.setItem(indexRow, 6, QtGui.QTableWidgetItem(""))
        
            indexRow = 0
            intValueForBar = 0
            if intLenOfBuffer == 0:
                ValueForBarOffset = 100
            else:
                ValueForBarOffset = 100/intLenOfBuffer
            if intLenOfBuffer > 100:
                ValueForBarOffset = intLenOfBuffer/100
            self.MainWindowUI.progressBar_MainServerShared.setValue(intValueForBar)
            while indexRow < intLenOfBuffer:
                ElementList = self.xmlSearchList.GetNodeChildrenAsList(indexRow, 6)
                if ElementList != []:
                    self.MainWindowUI.tableWidget_Search.setItem(indexRow, 0, QtGui.QTableWidgetItem(ElementList[0]))
                    self.MainWindowUI.tableWidget_Search.setItem(indexRow, 1, QtGui.QTableWidgetItem(ElementList[1]))
                    self.MainWindowUI.tableWidget_Search.setItem(indexRow, 2, QtGui.QTableWidgetItem(self.RSize(int(ElementList[2]))))
                    self.MainWindowUI.tableWidget_Search.setItem(indexRow, 3, QtGui.QTableWidgetItem(time.ctime(float(ElementList[5]))))
                    self.MainWindowUI.tableWidget_Search.setItem(indexRow, 4, QtGui.QTableWidgetItem(ElementList[3]+":"+ElementList[4]))
                    indexRow += 1
                    self.MainWindowUI.tableWidget_Search.insertRow(indexRow)
                    intValueForBar += ValueForBarOffset
                    self.MainWindowUI.progressBar_MainServerShared.setValue(intValueForBar)
            self.MainWindowUI.tableWidget_Search.resizeColumnToContents (3)
            self.MainWindowUI.tableWidget_Search.resizeColumnToContents (4)
            self.MainWindowUI.tableWidget_Search.show()
            self.MainWindowUI.progressBar_MainServerShared.setValue(100)
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"...Done")
        else:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Error: XMLServerList is Empty")
    def InicializeDownloadTable(self):
        """Metoda wstawia/updatuje elementy w wybrane do sciagniecia .
        Ustawia wlasciwy rozmiar kolumn
        Zeruje element 0
        """
        if self.xmlDownloadList != None:
            intLenOfBuffer = self.xmlDownloadList.GetNumberOfChildren()
            intRowCount = self.MainWindowUI.tableWidget_Download.rowCount()
            self.PurgeAnyList(self.xmlDownloadList , self.MainWindowUI.tableWidget_Download)
            self.MainWindowUI.tableWidget_Download.hide()

            indexRow = 0
            if intLenOfBuffer >= indexRow:
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 0, QtGui.QTableWidgetItem(""))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 1, QtGui.QTableWidgetItem(""))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 2, QtGui.QTableWidgetItem(""))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 3, QtGui.QTableWidgetItem(""))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 4, QtGui.QTableWidgetItem(""))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 5, QtGui.QTableWidgetItem(""))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 6, QtGui.QTableWidgetItem(""))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 7, QtGui.QTableWidgetItem(""))

            indexRow = 0
            while indexRow < intLenOfBuffer:
                ElementList = self.xmlDownloadList.GetNodeChildrenAsList(indexRow, 10)
                if ElementList != []:
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 0, QtGui.QTableWidgetItem(ElementList[9]))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 1, QtGui.QTableWidgetItem(self.RSize(int(ElementList[8]))))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 2, QtGui.QTableWidgetItem(self.RSize((int(ElementList[7])))))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 3, QtGui.QTableWidgetItem(ElementList[0]))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 4, QtGui.QTableWidgetItem(ElementList[1]))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 5, QtGui.QTableWidgetItem(self.RSize(int(ElementList[2]))))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 6, QtGui.QTableWidgetItem(time.ctime(float(ElementList[5]))))
                    self.MainWindowUI.tableWidget_Download.setItem(indexRow, 7, QtGui.QTableWidgetItem(ElementList[3]+":"+ElementList[4]))
                    indexRow += 1
                    self.MainWindowUI.tableWidget_Download.insertRow(indexRow)
            self.MainWindowUI.tableWidget_Download.resizeColumnToContents (6)
            self.MainWindowUI.tableWidget_Download.resizeColumnToContents (7)
            self.MainWindowUI.tableWidget_Download.show()
        else:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Error: XMLDownload is Empty")
    def AddFilesToSharedTable(self):
        """Metoda dodaje wskazane pliki i foldery do listy udostepnionych plikow.
        Nastepnie uzupelnia tabele plikow
        Wszystkie operacje sa wyswietlane na consoli oraz pokazywany jest pasek postepu"""
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
            intOffset = self.xmlSharedList.GetNumberOfChildren()
            intTempCount, intTempRet = self.xmlSharedList.ValidateIfFilesAreAlreadyThere(FileDialog)
            self.xmlSharedList.FileListInsertion(FileDialog, str(self.MainWindowUI.comboBox_checkSums.currentText()))
            self.xmlSharedList.WriteXMLDocToFile()
            self.intSharedTableCounter += intTempCount
            if intTempRet >= intTempCount:
                QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"List Updated successfuly "+str(intTempCount)+"/"+str(intTempRet)+" elements has been add")
            else:
                QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"), "List Update Faild unknown error")
        else:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"File haven't been choosen")
        #DEBUG
        if intTempCount > 0:
            self.InicializeSharedTable(intOffset)
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"List has been Refreshed")
        #
    def DelFilesFromSharedTable(self):
        """Kasuje z listy udostepnionych plikow, zaznaczone pozycje w Tabeli plikow"""
        IndexTab = len(self.MainWindowUI.tableWidget_Shared.selectedItems())
        ListToDelete = []
        for index in range(-1, IndexTab):
            if (index == -1 or (index+1)%7 == 0) and (index + 1 < IndexTab):
                if self.MainWindowUI.tableWidget_Shared.selectedItems()[index+1].text() != None:
                    ListToDelete.append(str(self.MainWindowUI.tableWidget_Shared.selectedItems()[index+1].text()))
                else:
                    print "Blad na elemencie", self.MainWindowUI.tableWidget_Shared.selectedItems()[index-5].text(), ']'
        self.xmlSharedList.RemoveChildrenByIndex(self.xmlSharedList.SearchByElementValue(ListToDelete, 0))
        self.xmlSharedList.WriteXMLDocToFile()
        self.PurgeAnyList(self.xmlSharedList.GetNumberOfChildren(), self.MainWindowUI.tableWidget_Shared)
        self.InicializeSharedTable()
        QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),str(len(ListToDelete))+" elements has been removed from SharedTable")
        
    def AddToDownloadTable(self):
        """Dodaj wyselekcjonowane elementy z talbicy search to download"""
        IndexTab = len(self.MainWindowUI.tableWidget_Search.selectedItems())
        intOffset = 0
        ListToAppend = []
        for index in range(-1, IndexTab):
            if (index == -1 or (index+1)%5 == 0) and (index + 1 < IndexTab):
                if self.MainWindowUI.tableWidget_Search.selectedItems()[index+1].text() != None:
                    ListToAppend.append(str(self.MainWindowUI.tableWidget_Search.selectedItems()[index+1].text()))
                else:
                    print "Blad na elemencie", self.MainWindowUI.tableWidget_Search.selectedItems()[index-5].text(), ']'
                intOffset -= 1
        self.xmlDownloadList.addChildrenFromOtherXMLDownload(self.xmlSearchList,ListToAppend )
        self.InicializeDownloadTable()
        QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),str(len(ListToAppend))+" elements has been add to DownloadTable")
    def DelFilesFromDownloadTableListGen(self):
        IndexTab = len(self.MainWindowUI.tableWidget_Download.selectedItems())
        ListToDelete = []
        for index in range(IndexTab):
            if (index == 3 or (index-3)%8 == 0) and (index != 0):
                if self.MainWindowUI.tableWidget_Download.selectedItems()[index].text() != None:
                    ListToDelete.append(str(self.MainWindowUI.tableWidget_Download.selectedItems()[index].text()))
                else:
                    print "Blad na elemencie", self.MainWindowUI.tableWidget_Download.selectedItems()[index-8].text(), ']'
        return ListToDelete
    def DelFilesFromDownloadTable(self):
        """Kasuje z listy udostepnionych plikow, zaznaczone pozycje w Tabeli plikow"""
        ListToDelete = self.DelFilesFromDownloadTableListGen()
        self.xmlDownloadList.RemoveChildrenByIndex(self.xmlDownloadList.SearchByElementValue(ListToDelete, 0))
        self.PurgeAnyList(self.xmlDownloadList.GetNumberOfChildren(), self.MainWindowUI.tableWidget_Download)
        self.InicializeDownloadTable()
        QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),str(len(ListToDelete))+" elements has been removed from DownloadTable")
    ###Połaczenie
    def ConnectToServer(self):
        """Laczy z serwerem oraz updatuje przyciski i console"""
        MethodName = self.ClassName+"[ConnectToServer]->"
        strRet =""
        if self.ConnectedAs > -1:
            self.MainWindowUI.pushButton_ConnDiscon.setText(QtGui.QApplication.translate("TabWidget_MainFrame", "Połącz", None, QtGui.QApplication.UnicodeUTF8))
            self.mySocket.myDisconnect()
            self.mySocket.join()
            del self.mySocket
            self.mySocket = None
            self.ConnectedAs = -1
            QtCore.QObject.emit(self.QSignalObj1,QtCore.SIGNAL("AdminPanelSignal"))
        else:
            self.mySocket = myComm.mySocketComm(myConfigs, self)
            self.mySocket.start()
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Connecting to "+myConfigs.Host+"::"+str(myConfigs.Port))
            #self.ConnectedAs = 10
    #odswierzyc layout
    def MsgToConsole(self,Msg1):
        """Wpisuje odpowidenia wiadomosc do widgeta consoli oraz ustawia na niej focus consoli"""
        item = QtGui.QListWidgetItem(QtCore.QString(time.strftime(myConfigs.ConsoleTimeFormat)+" "+Msg1))
        self.MainWindowUI.listWidget_console.insertItem(self.ConsoleMsgCounter, item)
        self.MainWindowUI.listWidget_console.scrollToItem(item)
        self.ConsoleMsgCounter+=1

    def ButtonAdminPanel(self):
        """"Pokazuje Panel admina jezeli jestem zalogowany jak root'"""
        if self.ConnectedAs == 1 and self.myAdminPanel == None:
            self.myAdminPanel = myAdminPanel.myAdminPanel(self)
            self.myAdminPanel.run()
        else:
            if self.myAdminPanel != None:
                QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Shutting down AdminPanel")
                self.myAdminPanel.Clean()
                del self.myAdminPanel
                self.myAdminPanel =None
    #for buttons on RightTop
    def MsgBoxUniversal(self, strMessage):#wywal to
        box = QtGui.QMessageBox(None)
        box.setWindowTitle(QtGui.QApplication.translate(None,"Informacja",None, QtGui.QApplication.UnicodeUTF8))
        box.setText(QtGui.QApplication.translate(None,strMessage,None, QtGui.QApplication.UnicodeUTF8))
        box.setStandardButtons(QtGui.QMessageBox.Ok)
        box.exec_()
    def ButtonAdminPanelSignal(self):
        """Metoda w reakcji na sygnal pokaze lub ukryje przycisk panelu administracyjnego"""
        if self.ConnectedAs == 1:
            print self.ClassName+"[ButtonAdminPanel]->shown"
            self.MainWindowUI.pushButton_AdminPanel.show()
        else:
            print self.ClassName+"[ButtonAdminPanel]->hidden"
            self.MainWindowUI.pushButton_AdminPanel.hide()
        #
    ##Options

    def StoreConfigs(self):
        """Metoda pobiera z elementow gui wartosci i kopiuje je na miejsce zeminnych.
        Nastepnie wywoluje metode zapisywania ustawien i wysyla komunikat do konsoli"""
        MethodName = self.ClassName+"[StoreConfigs]->"
        myConfigs.Host = self.MainWindowUI.lineEdit_IP.displayText()
        myConfigs.Port = self.MainWindowUI.lineEdit_Port.displayText()
        myConfigs.User = self.MainWindowUI.lineEdit_Login.displayText()
        self.MainWindowUI.lineEdit_Password.setEchoMode(QtGui.QLineEdit.Normal)
        myConfigs.Password = self.MainWindowUI.lineEdit_Password.displayText()
        self.MainWindowUI.lineEdit_Password.setEchoMode(QtGui.QLineEdit.PasswordEchoOnEdit)
        myConfigs.TimeOut = int(self.MainWindowUI.spinBox_TimeOut.text())
        myConfigs.SharedFiles = self.MainWindowUI.lineEdit_PathSharedFile.displayText()
        myConfigs.HashType = self.MainWindowUI.comboBox_checkSums.currentText()
        myConfigs.ValidateBeforeSend = int(self.MainWindowUI.checkBox_ValidateFileBeforeSend.isChecked())
        myConfigs.ValidateFilesOnStarup = int(self.MainWindowUI.checkBox_ValidateOnProgramStart.isChecked())
        myConfigs.CountCheckSumWhenAdd = int(self.MainWindowUI.checkBox_CountHashWhenAdd.isChecked())
        myConfigs.LetDownloadFilesWhichFaildValidation = int(self.MainWindowUI.checkBox_LetToDwnNoValidFiles.isChecked())
        myConfigs.Zoom = self.MainWindowUI.progressBar_Zoom.value()
        myConfigs.Debug = int(self.MainWindowUI.checkBox_Debug.isChecked())
        if myConfigs.StoreCFG() == 0:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Configs has been stored")
        else:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Failed to store configs")
    def Slider_Zoom(self,value):
        """Metoda dba jedynie o ustawieni odpowiednij skali okienka w, ktorym wyswietlam strone projektu"""
        self.MainWindowUI.progressBar_Zoom.setValue(value)
        self.MainWindowUI.webView_ProjectSite.setZoomFactor(float(value*0.01))
    def ProgressBarFileAdd(self, value):
        self.MainWindowUI.progressBar_Shared.setValue(value)
    def SendSearchForFiles(self):
        """Sprawdza czy serwer dziala jezeli tak: 
        Inicjuje wyszukiwanie plikow. 
        Jezeli wyszukiwanie sie powidolo tworzona jest lista xml do przetworzenia
        Wlacza sie updatowanie WidgetaTablicy trzymajacego wyniku poprzedniego szukania
        Uruchamiane sa metody updatowania consoli"""
        if self.mySocket !=None:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Asking server for ["+self.MainWindowUI.lineEdit_Search.displayText()+"]")
            self.mySocket.AskServerForFiles(str(self.MainWindowUI.lineEdit_Search.displayText()))
        else:
            QtCore.QObject.emit(self.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"You are not connected to any server")       
    def bClose(self):
        self.close()
    def MsgOnExit(self):
        """Poza wyczyszczeniem zajmowanej pamieci przed wyjsciem
        Uruchamia zapis listy sciaganych elementow, z filtrem znajdowania sie elementow na serwerze.
        """
        if self.ConnectedAs > -1:
            print self.ClassName+"Disconnecting from server before Exit"
            if self.mySocket != None:
                print self.ClassName+"Destroying mySocket"
                self.mySocket.myDisconnect()
                self.mySocket.join()
                del self.mySocket
            if self.myAdminPanel != None:
                print self.ClassName+"Destroying myAdminPanel"
                self.myAdminPanel.Clean()
                del self.myAdminPanel
                self.myAdminPanel =None
        if self.xmlDownloadList != None:
            print self.ClassName+"Destroying xmlDownloadList"
            self.xmlDownloadList.RemoveChildrenByIndex(self.xmlDownloadList.SearchByElementValueDiff(["0"], 6))
            self.xmlDownloadList.WriteXMLDocToFile()
        if self.xmlSearchList != None:
            print self.ClassName+"Destroying xmlSearchList"
            del self.xmlSearchList
        if self.xmlSharedList !=None:
            print self.ClassName+"Destroying xmlSharedList"
            del self.xmlSharedList
        if self.TrayIcon != None:
            print self.ClassName+"Destroying TryIcon"
            del self.TrayIcon
        if self.TrayMenu != None:
            print self.ClassName+"Destroying TryMenu"
            del self.TrayMenu
        print self.ClassName+"Bye Bye"
        #self.close()
    def closeEvent(self, event):
        self.MsgOnExit()

def myClient():
    """Metoda w ktorej dzieje sie inicjaliza Qt4"""
    app = QtGui.QApplication(sys.argv)
    mainWindow = classMainFrame()
    mainWindow.SetUpObjects()
    mainWindow.show()

    return app.exec_()

