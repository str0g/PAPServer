#! /usr/bin/env python
# -*- coding: utf-8 -*-
'''
####Author Łukasz Buśko                
####Contact buskol.waw.pl@gmail.com    
####Licenes GPL                        
'''

import telnetlib, socket
import time, string
from lxml import etree
#Moje
from xmlSupport import XMLFileGenerator

class myTelnet():
    """
    This module keeps connection alive until disconnected method was run.
    """
    def __init__(self, obj_myConfigs_T):
        myTelnet.ClassName="[myTelnet]->"
        self.strSpecialChar = "\r\n\r\n"
        self.XMLVersion = 1.0
        self.ServerVersion = "0.0.0.0"
        self.intAmIroot = -1
        self.strLogin =""
        self.obj_myConfigs = obj_myConfigs_T
        self.TelnetConnect()
        self.RecivedData = 0
        self.UploadedData = 0
        self.TimeOut = obj_myConfigs_T.TimeOut
        self.SizeOfPacket = 1024*1024
        self.KB=1024
        self.MB=self.KB*1024
        print self.ClassName
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
    def TelnetConnect(self):
        '''
        Lacze sie z serwerem i pobiera z niego podstawowe informacje
        '''
        MethodName = myTelnet.ClassName+"[TelnetConnect]->"
        print MethodName
        strBuffer = ""
        strBufferValue = ""
        self.intAmIroot = -3
        self.Telnet = None
        try:
            self.Telnet = telnetlib.Telnet(self.obj_myConfigs.Host,
                                           self.obj_myConfigs.Port,
                                           self.obj_myConfigs.TimeOut)
            self.Telnet.write(self.obj_myConfigs.User + self.strSpecialChar)
            strBuffer = self.Telnet.read_until(self.obj_myConfigs.User+"~$ ",self.obj_myConfigs.TimeOut)
            self.UploadedData = len(self.obj_myConfigs.User + self.strSpecialChar)
            self.RecivedData = len(strBuffer)
            print self.ClassName+"Frist Transfer:",self.RecivedData,"bajts"
            strBufferValue = strBuffer[strBuffer.find("XML=")+len("XML="):]
            self.XMLVersion = float(strBufferValue[:strBufferValue.find(",\n")])
            print "XMLVersion["+strBufferValue[:strBufferValue.find(",\n")]+"]"

            strBufferValue = strBuffer[strBuffer.find("Server_Version=")+len("Server_Version="):]
            self.ServerVersion = strBufferValue[:strBufferValue.find(",\n")]
            print "ServerVersion["+self.ServerVersion+"]"

            strBufferValue = strBufferValue[:strBuffer.rfind("~$")]
            self.strLogin = strBufferValue[strBufferValue.rfind("\n")+len("\n"):]
            print "Who am I["+self.strLogin+"]"
            if self.strLogin == "root~$ ":
                self.intAmIroot = 1
            elif self.strLogin == "unknow~$ ":
                self.intAmIroot = -1
            else:
                self.intAmIroot = 0
            return 0
        except EOFError,error:
            print MethodName,error
            self.intAmIroot = -2
            return 1
        except socket.error, error:
            print MethodName,error
            self.intAmIroot = -2
            return 2
        
    def TelnetCommunicate(self, command, timeout = 2):
        '''
        Ta metoda wysyla informacje do serwera, w przypadku bledu zwraca puste dane
        '''
        MethodName = myTelnet.ClassName+"[TelnetCommunicate]->"
        data=""
        try:
            if self.Telnet != None:
                self.Telnet.write(command+ self.strSpecialChar)
                self.UploadedData += len(self.obj_myConfigs.User + self.strSpecialChar)
                data = self.Telnet.read_until(self.obj_myConfigs.User+"~$ ",timeout)
                self.RecivedData += len(data)
                print "DATA RECIVED:",data,"<]>",self.RecivedData,"bajts""<]"
                data = data[:data.rfind("\n"+self.strLogin)]
        except EOFError, error:
            print MethodName,error
            data=""
        except socket.error, error:
            print MethodName,error
            data=""

        return data
    def CheckIfConnectionIsAlive(self):
        if self.TelnetCommunicate("Alive", 10) != "YES":
            return 1
        else:
            return 0
    def SendWithSpecSize(self,data):
        return
    def TelnetDisconnect(self):
        '''
        Rozlaczam polaczenie
        '''
        if self.Telnet != None:
            print myTelnet.ClassName+"Connection has been closed"
            strReturn = self.TelnetCommunicate("DisconnectMe")
            print "Recived Data[", self.RecivedData,"/"#,len(self.Telnet.read_all()),"] bajts"
            print "Uploaded data[", self.UploadedData, "] bajts"
            self.Telnet.close
        else:
            print myTelnet.ClassName+"Cleaning after connection fail"
            strReturn = None
        return strReturn
    def TotalTransfer(self):
        '''
        Zwraca liczbowa wartosc ile danych zostal przetransferowanynch w bajtach
        '''
        return self.RecivedData,self.UploadedData
    def NegSet(self, intSize, intTime):
        self.SizeOfPacket = intSize
        self.TimeOut = intTime

class myTelnetParser(myTelnet, XMLFileGenerator):
    def __init__(self, obj_myConfigs,obj_listWidget_Console):
        myTelnet.__init__(self,obj_myConfigs)
        XMLFileGenerator.__init__(self)
        #self.obj_myConfigs = obj_myConfigs
        self.obj_listWidget_Console = obj_listWidget_Console
        self.XMLListFromServer = None
        myTelnetParser.ClassName= self.ClassName+"[Pareser]->"
        print self.ClassName
    def AmIRoot(self):
        '''
        Zwraca wartosc zmiennej czy jestem rootem na serwerze
        '''
        print self.ClassName+"AmIRoot"
        return self.intAmIroot
    def AskForServerTime(self):
        '''
        Pobiera czas z serwera, moze ten czas zostanie uzyty kiedys pozniej :-)
        '''
        print self.ClassName+"Asking Server for time as int"
        strRet = self.TelnetCommunicate("GetServerTime")
        if strRet == "":
            return "Server did not responed"
        else:
            return time.ctime(int(strRet))
    def AskForServerInfo(self):
        strReturn = self.TelnetCommunicate("GetServerInfo")
        return "Moze bedzie cos"
    def AskServerForFiles(self, strWords):
        '''
        Wysyla zapytanie do serwera
        Zwraca czy sie udalo
        '''
        retcode = 1
        strReturn = self.TelnetCommunicate("SearchFor: "+strWords, 3)
        if  len(strReturn) >10:
            self.XMLListFromServer = self.CreateXMLServerFilesList(strReturn)
            retcode = 0
        return retcode
    def GetXMLListFromServer(self):
        '''
        Jezeli z lista jest wszystko w porzadku to ja zwracam
        '''
        if len(self.XMLListFromServer) >0:
            return self.XMLListFromServer
        else:
            return None
    def SendList(self, objXML, objBarWidget):
        retcode = 1
        if objXML != None:
            if len(objXML)>1:
                strToSend = etree.tostring(objXML)
                intLengthOfXML = len(strToSend)
                print intLengthOfXML, "wazne"
                objBarWidget.setValue(0)
                intIndex = 0
                intBandWidth = time.time()
                while intIndex < intLengthOfXML:
                    if intIndex < self.SizeOfPacket:
                        strReturn = self.TelnetCommunicate(strToSend[intIndex:])
                        intIndex += self.SizeOfPacket
                    else:
                        strReturn = self.TelnetCommunicate(strToSend[intIndex - self.SizeOfPacket:intIndex])
                    print "[", strReturn, "]"
                    print "*"*20
                    if strReturn == " ":
                        print self.ClassName+"Data has been lost, resending"
                    elif strReturn == "":#xml ze statusem ok i timeoutem jezeli potrzeba
                        print self.ClassName+"Chunk of size", self.SizeOfPacket, "has been send"
                        intIndex += self.SizeOfPacket
                        print self.ClassName+"Data send:", intIndex, "Data left:", intLengthOfXML - intIndex
                        print self.ClassName+"Bandwidth:", self.Bandwidth(intBandWidth, time.time(), intIndex)
                        objBarWidget.setValue((float(intIndex)/float(intLengthOfXML))*100) 
                    else:#potem popraw
                        print self.ClassName+"Chunk of size", self.SizeOfPacket, "has been send"
                        intIndex += self.SizeOfPacket
                        print self.ClassName+"Data send:", intIndex, "Data left:", intLengthOfXML - intIndex
                        print self.ClassName+"Bandwidth:", self.Bandwidth(intBandWidth, time.time(), intIndex)
                        objBarWidget.setValue((float(intIndex)/float(intLengthOfXML))*100) 
                        print "Some error"
                        
                if intIndex >= intLengthOfXML:
                    objBarWidget.setValue(100)
                    retcode = 0
            else:
                print self.ClassName+"List is empty! Sending has been aborded"
        else:
            print self.ClassName+"List does NOT exist!"
        return retcode
    def RestartServer(self):
        '''
        Wysylam do serwera rzadanie restartu
        '''
        strReturn = self.TelnetCommunicate("Restart")
        return "komunikat restart"
    def ShutdownServer(self):
        '''
        Wysylam do serwera rzadanie wylaczenia lagodnego
        '''
        strReturn = self.TelnetCommunicate("Shutdown")
        return "komunikat shutdown"
    def ShutdownServerForced(self):
        '''
        Wysylam do serwera rzadanie wylaczenia wymuszonego
        '''
        print self.ClassName+"ShutdownForce"
        strReturn = self.TelnetCommunicate("ShutdownForced")
        return "komunikat shutdown forced"

