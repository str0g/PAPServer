#! /usr/bin/env python
# -*- coding: utf-8 -*-

__author__ =  'Lukasz Busko'                
__email__ = 'buskol.waw.pl@gmail.com'
__version__= '0.1.9'
__licenes__= 'Licenes GPL'  

try:
    import os, time, sys,  socket
    from PyQt4 import QtGui, QtCore
    from threading import Thread
    #Moje
    import myxml
except ImportError, error:
    print "[myComm]->", error
    time.sleep(2)
    sys.exit(1)
    
    
class mySocketComm(Thread):
    def __init__(self, obj_myConfigs_T, parent):
        Thread.__init__(self)
        self.ClassName ="[mySocketComm]->"
        self.Parent = parent
        self.strSpecialChar = "\r\n\r\n"
        self.XMLVersion = 1.0
        self.ServerVersion = "0.0.0.0"
        self.intAmIroot = -1
        self.strLogin =""
        self.obj_myConfigs = obj_myConfigs_T
        self.mySocket=None
        self.RecivedData = 0
        self.UploadedData = 0
        self.TimeOut = obj_myConfigs_T.TimeOut
        self.SizeOfPacket = 1024*1024
        self.KB=1024
        self.MB=self.KB*1024
        self.xmlInBuffer = myxml.myXmlDoc(obj_myConfigs_T)
        self.xmlOutBuffer = myxml.myXmlDoc(obj_myConfigs_T)
        self.xmlFileBuffer = myxml.myXmlDoc(obj_myConfigs_T)
        self.SetSignals()
    def SetSignals(self):
        self.QSignalObjSend = QtCore.QObject(None)
        QtCore.QObject.connect(self.QSignalObjSend, QtCore.SIGNAL("send(String)"), self.send)
    def run(self):
        try:
            self.mySocket = socket.create_connection((self.obj_myConfigs.Host,self.obj_myConfigs.Port))#,self.obj_myConfigs.TimeOut )
            self.Parent.MainWindowUI.pushButton_ConnDiscon.setText(QtGui.QApplication.translate("TabWidget_MainFrame", 
                                                                                                                                "Rozłącz", 
                                                                                                                                None, 
                                                                                                                                QtGui.QApplication.UnicodeUTF8))
            self.ConnLoop()
        except socket.error, error:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),str(error))
            self.DataLoop = 1
        except socket.herror, error:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Socket error: "+str(error))
            self.DataLoop = 1
        except socket.gaierror, error:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Socket error: "+str(error))
            self.DataLoop = 1
        except socket.timeout, error:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Socket error: "+str(error))
            self.DataLoop = 1
        except AttributeError,  error:
            print "Standard error during cleaning process", error
            self.DataLoop = 1
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Socket error: "+str(error))
    def ConnLoop(self):
        self.DataLoop = 0
        self.strRecivedData=""
        while(self.DataLoop == 0):
            try:
                self.strRecivedData = self.mySocket.recv(8192)
                self.CutAndCountData()
                self.DataParser()
            except socket.error, error:
                QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Socket error: "+str(error))
                self.DataLoop = 1
            except socket.herror, error:
                QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Socket error: "+str(error))
                self.DataLoop = 1
            except socket.gaierror, error:
                QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Socket error: "+str(error))
                self.DataLoop = 1
            except socket.timeout, error:
                QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Socket error: "+str(error))
                self.DataLoop = 1
            except AttributeError,  error:
                print "Standard error during cleaning process", error
                self.DataLoop = 1
    def DataParser(self): 
        #akcje wysylki
        if self.Parent.LockSending == 0 and self.strLogin!="":
            self.SendList(self.Parent.xmlListSharedFiles)
        #obsluga tego co przyslal server
        if self.strRecivedData.find("<")== 0 and (self.strRecivedData.find("</")!= -1 or self.strRecivedData.find("/>")!= -1):
            if (self.strRecivedData.find("<SharedFiles>")!=-1):
                QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"List recived")
                self.ServerResponseForFileAsk()
            elif (self.strRecivedData.find("<SharedFiles />") != -1):
                QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"File not found")
        elif self.strRecivedData == "100":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Server is going to Restart it self when you reconnect")
        elif self.strRecivedData == "101":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Server has been Restarted")
        elif self.strRecivedData == "102":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Server is going to Shutdown it self when you reconnect")
        elif self.strRecivedData == "103":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Server has been Shutdown")
        elif self.strRecivedData == "150":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Sever is searching files")
        elif self.strRecivedData == "333":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Sever: file searching has failed")
        elif self.strRecivedData == "404":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Sever: File not found")
        elif self.strRecivedData == "602":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Sever faild to create xml document")
        elif self.strRecivedData == "613":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Sever faild to parse xml document")
        elif self.strRecivedData == "614":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Sever recived empty xml document")
        elif self.strRecivedData == "900":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Sever recived not implemented comand")
        elif self.strRecivedData == "944":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Server: Search sytax error")
        elif self.strRecivedData == "999":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Syntax error")
        else:
            if(self.strRecivedData.find("XML")!=-1 and self.strRecivedData.find("Server_Version")!=-1):
                self.ReadServerInfo()
            elif(self.strRecivedData.find("Time")==0):
                self.ServerTime()
            elif self.strRecivedData == "Login:":
                self.send(self.obj_myConfigs.User)
            elif(self.strRecivedData == ""):
                pass
            else:
                if len(self.strRecivedData) > 100:
                    QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Server send unknow information("+str(len(self.strRecivedData))+")")
                else:
                    QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Server send unkonw information["+self.strRecivedData+"]("+str(len(self.strRecivedData))+")")
    def CutAndCountData(self):
        '''
        Ta metoda wysyla informacje do serwera, w przypadku bledu zwraca puste dane
        '''
        try:
                counter = len(self.strRecivedData)
                self.RecivedData += counter
                print "DATA RECIVED:",self.strRecivedData,"<]>",self.RecivedData,"bajts""<]"
                self.strRecivedData = self.strRecivedData[:self.strRecivedData.rfind("\n"+self.strLogin)]
                if counter == 0:
                    self.DataLoop = 1
        except EOFError, error:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Cutting data error: "+str(error))
            self.strRecivedData=""
    def ReadServerInfo(self):
        strBuffer = self.strRecivedData
        strBufferValue = ""
        self.intAmIroot = -3
        try:
            strBufferValue = strBuffer[strBuffer.find("XML=")+len("XML="):]
            self.XMLVersion = float(strBufferValue[:strBufferValue.find(",\n")])
            print "XMLVersion["+strBufferValue[:strBufferValue.find(",\n")]+"]"

            strBufferValue = strBuffer[strBuffer.find("Server_Version=")+len("Server_Version="):]
            self.ServerVersion = strBufferValue[:strBufferValue.find(",\n")]
            print "ServerVersion["+self.ServerVersion+"]"

            strBufferValue = strBufferValue[:strBuffer.rfind("~$")]
            self.strLogin = strBufferValue[strBufferValue.rfind("LoggedAs ")+len("LoggedAs "):]
            self.strLogin = self.strLogin[:self.strLogin.find("\n")-1]
            print "Who am I["+self.strLogin+"]"
            if self.strLogin == "root":
                self.intAmIroot = 1
            elif self.strLogin == "unknow":
                self.intAmIroot = -1
            else:
                self.intAmIroot = 0
        except EOFError,error:
            print MethodName,error
            self.intAmIroot = -2
        except socket.error, error:
            print MethodName,error
            self.intAmIroot = -2
        finally:
            self.Parent.ConnectedAs = self.intAmIroot
            QtCore.QObject.emit(self.Parent.QSignalObj1,QtCore.SIGNAL("AdminPanelSignal"))
            self.send("GetServerTime")
    def send(self, data):
        if self.mySocket != None:
            print "Wyslane:\n["+data+"]"
            self.UploadedData += self.mySocket.send(data+self.strSpecialChar)
        QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"You are not conneted to any server")
    def Bandwidth(self, fVal1, fVal2, intdata):
        '''
        Zwraca przepustowosc w B, KB, MB w zaleznosci od osiagnietej przepustowosci
        '''
        fVal2 = float(intdata/(fVal2 - fVal1))
        if fVal2 < self.KB:
            return str(int(fVal2))+"B"
        elif fVal2 < self.MB:
            return str(round(fVal2/self.KB, 1))+"KB"
        else:
            return str(round(fVal2/self.MB, 2))+"MB"
    def myDisconnect(self):
        '''
        Rozlaczam polaczenie
        '''
        if self.mySocket != None:
            print self.ClassName+"Connection has been closed"
            self.send("DisconnectMe")
            Summary = "Disconnected from "+self.obj_myConfigs.Host+"::"+str(self.obj_myConfigs.Port)+" in["+str(self.RecivedData)+"]out["+str(self.UploadedData) +"] bajts"
            self.Parent.ConnectedAs = -1
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),Summary)
            self.mySocket.close()
            del self.mySocket
            self.mySocket = None
            time.sleep(1)
        else:
            print "Cleaning after connection fail"
    def ServerTime(self):
        '''
        Pobiera czas z serwera, moze ten czas zostanie uzyty kiedys pozniej :-)
        '''
        self.Parent.LocalServerTime = time.ctime(int(self.strRecivedData[self.strRecivedData.find(" ")+len(str(" ")):]))
        QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"), "Connected and authorized successful as "+self.strLogin+", local server time is: "+str(self.Parent.LocalServerTime))
    def AskForServerInfo(self):
        strReturn = self.mySocketCommunicate("GetServerInfo")
        return "Moze bedzie cos"
    def AskServerForFiles(self, strWords):
        '''
        Wysyla zapytanie do serwera
        '''
        self.send("SearchFor: "+strWords)
    def ServerResponseForFileAsk(self):
        if self.Parent.xmlListFromServer.CreateDocFromFile(self.strRecivedData) == 0:
            if  self.Parent.xmlListFromServer.GetNumberOfChildren()  > 0:
                QtCore.QObject.emit(self.Parent.QSignalObjServerList, QtCore.SIGNAL("RefreshTableWidget"))
                self.Parent.xmlListFromServer.PrintXMLDoc()
            else:
                QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"...file not found")
        else:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"...not valid xml list")
        self.Parent.xmlListFromServer.PrintXMLDoc()
    def ServerRestart(self):
        '''
        Wysylam do serwera rzadanie restartu
        '''
        self.send("Restart")
        return "komunikat restart"
    def ServerShutdown(self):
        '''
        Wysylam do serwera rzadanie wylaczenia lagodnego
        '''
        self.send("Shutdown")
        return "komunikat shutdown"
    def ServerShutdownForced(self):
        '''
        Wysylam do serwera rzadanie wylaczenia wymuszonego
        '''
        print self.ClassName+"ShutdownForce"
        self.send("ShutdownForced")
        return "komunikat shutdown forced"
    def SendList(self, objXML):
        if objXML != None:
            if objXML.GetNumberOfChildren()>1:
                strToSend =  objXML.GetXMLAsText()
                intLengthOfXML = len(strToSend)
                print intLengthOfXML, "wazne"
                QtCore.QObject.emit(self.Parent.QSignalObjProgressBarAdd,  QtCore.SIGNAL("RefreshProgressBarAdd"), 0)
                intIndex = 0
                intBandWidth = time.time()
                while intIndex < intLengthOfXML:
                    if intIndex < self.SizeOfPacket:
                        strReturn = self.send(strToSend[intIndex:])
                        intIndex += self.SizeOfPacket
                    else:
                        strReturn = self.send(strToSend[intIndex - self.SizeOfPacket:intIndex])
                    print "[", strReturn, "]"
                    print "*"*20
                    if strReturn == " ":
                        print self.ClassName+"Data has been lost, resending"
                    elif strReturn == "":#xml ze statusem ok i timeoutem jezeli potrzeba
                        print self.ClassName+"Chunk of size", self.SizeOfPacket, "has been send"
                        intIndex += self.SizeOfPacket
                        print self.ClassName+"Data send:", intIndex, "Data left:", intLengthOfXML - intIndex
                        print self.ClassName+"Bandwidth:", self.Bandwidth(intBandWidth, time.time(), intIndex)
                        QtCore.QObject.emit(self.Parent.QSignalObjProgressBarAdd,  QtCore.SIGNAL("RefreshProgressBarAdd"),(float(intIndex)/float(intLengthOfXML))*100) 
                    else:#potem popraw
                        print self.ClassName+"Chunk of size", self.SizeOfPacket, "has been send"
                        intIndex += self.SizeOfPacket
                        print self.ClassName+"Data send:", intIndex, "Data left:", intLengthOfXML - intIndex
                        print self.ClassName+"Bandwidth:", self.Bandwidth(intBandWidth, time.time(), intIndex)
                        QtCore.QObject.emit(self.Parent.QSignalObjProgressBarAdd,  QtCore.SIGNAL("RefreshProgressBarAdd"),(float(intIndex)/float(intLengthOfXML))*100) 
                        print "Some errors"
                        self.Parent.LockSending = 1
                if intIndex >= intLengthOfXML:
                    QtCore.QObject.emit(self.Parent.QSignalObjProgressBarAdd,  QtCore.SIGNAL("RefreshProgressBarAdd"),100)
            else:
                QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("You are not sharing any files!"))
        else:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("SharingFiles xml does not exists!"))
