#! /usr/bin/env python
# -*- coding: utf-8 -*-
'''
####Author Łukasz Buśko                
####Contact buskol.waw.pl@gmail.com    
####Licenes GPL                        
'''
try:
    import os, time, sys,  socket
    from PyQt4 import QtGui, QtCore
    from threading import Thread
    #Moje
    from xmlSupport import XMLFileGenerator
    from lxml import etree
except ImportError, error:
    print "[myComm]->", error
    time.sleep(2)
    sys.exit(1)
    
    
class mySocketComm(Thread, XMLFileGenerator):
    def __init__(self, obj_myConfigs_T, parent):
        Thread.__init__(self)
        XMLFileGenerator.__init__(self)
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
    def run(self):
        try:
            self.mySocket = socket.create_connection((self.obj_myConfigs.Host,self.obj_myConfigs.Port))#,self.obj_myConfigs.TimeOut )
            self.Parent.pushButton_ConnDiscon.setText(QtGui.QApplication.translate("TabWidget_MainFrame", 
                                                                                                                                "Rozłącz", 
                                                                                                                                None, 
                                                                                                                                QtGui.QApplication.UnicodeUTF8))
            self.ConnLoop()
        except socket.error, error:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Server does not exist")
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
            self.SendList(self.Parent.buf_XMLSharedFilesList)
        #obsluga tego co przyslal server
        if self.strRecivedData.find("<")== 0 and (self.strRecivedData.find("</")!= -1 or self.strRecivedData.find("/>")!= -1):
            if (self.strRecivedData.find("<SharedFiles>")!=-1):
                QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"List recived")
                self.ServerResponseForFileAsk()
            elif (self.strRecivedData.find("<SharedFiles />") != -1):
                QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"File not found")
        elif self.strRecivedData == "999":
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"Syntax error")
        elif self.strRecivedData == "Login:":
            self.send(self.obj_myConfigs.User)
        else:
            if(self.strRecivedData.find("XML")!=-1 and self.strRecivedData.find("Server_Version")!=-1):
                self.ReadServerInfo()
            elif(self.strRecivedData.find("Time")==0):
                self.ServerTime()
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
                self.RecivedData += len(self.strRecivedData)
                print "DATA RECIVED:",self.strRecivedData,"<]>",self.RecivedData,"bajts""<]"
                self.strRecivedData = self.strRecivedData[:self.strRecivedData.rfind("\n"+self.strLogin)]
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
            if self.strLogin == "root~$ ":
                self.intAmIroot = 1
            elif self.strLogin == "unknow~$ ":
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
            self.send("GetServerTime")
    def send(self, data):
        print "Wyslane:\n["+data+"]"
        self.UploadedData += self.mySocket.send(data+self.strSpecialChar)
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
        if self.ValidateXMLDocument(self.strRecivedData, -1) == 0:
            self.XMLListFromServer = self.CreateXMLServerFilesList(self.strRecivedData)
            if  len(self.XMLListFromServer)  > 0:
                self.Parent.buf_XMLSerwerFilesList = self.XMLListFromServer
                QtCore.QObject.emit(self.Parent.QSignalObjServerList, QtCore.SIGNAL("RefreshTableWidget"))
            else:
                QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"...file not found")
        else:
            QtCore.QObject.emit(self.Parent.QSignalObjMsgToConsole, QtCore.SIGNAL("MsgToConsole(QString)"),"...not valid xml list")
    def RestartServer(self):
        '''
        Wysylam do serwera rzadanie restartu
        '''
        self.send("Restart")
        return "komunikat restart"
    def ShutdownServer(self):
        '''
        Wysylam do serwera rzadanie wylaczenia lagodnego
        '''
        self.send("Shutdown")
        return "komunikat shutdown"
    def ShutdownServerForced(self):
        '''
        Wysylam do serwera rzadanie wylaczenia wymuszonego
        '''
        print self.ClassName+"ShutdownForce"
        self.send("ShutdownForced")
        return "komunikat shutdown forced"
    def SendList(self, objXML):
        if objXML != None:
            if len(objXML)>1:
                strToSend = etree.tostring(objXML)
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
