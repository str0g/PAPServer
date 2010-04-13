#! /usr/bin/env python
# -*- coding: utf-8 -*-
'''
####Author Łukasz Buśko                
####Contact buskol.waw.pl@gmail.com    
####Licenes GPL                        
'''

import telnetlib
import time, string
from lxml import etree
#Moje
from xmlSupport import XMLFileGenerator

class myTelnet():
    """
    This module keeps connection alive until disconnected method was run.
    """
    def __init__(self, obj_myConfigs):
        self.ClassName="[myTelnet]->"
        self.strSpecialChar = "\r\n\r\n"
        self.XMLVersion = 1.0
        self.ServerVersion = "0.0.0.0"
        self.intAmIroot = -1
        self.obj_myConfigs = obj_myConfigs
        self.TelnetConnect()
        self.RecivedData = 0
        self.UploadedData = 0
        self.TimeOut = obj_myConfigs.TimeOut
        self.SizeOfPacket = 1024*1024
        self.KB=1024
        self.MB=self.KB*1024
        print self.ClassName
    def Bandwidth(self, fVal1, fVal2, intdata):
        fVal2 = int(intdata/(fVal2 - fVal1))
        if fVal2 < self.KB:
            return str(fVal2)+"B"
        elif fVal2 < self.MB:
            return str(fVal2)+"KB"
        else:
            return str(fVal2)+"MB"
    def TelnetConnect(self):
        '''
        Lacze sie z serwerem i pobiera z niego podstawowe informacje
        '''
        MethodName = self.ClassName+"[TelnetConnect]->"
        print MethodName
        strBuffer = ""
        strBufferValue = ""
        try:
            self.Telnet = telnetlib.Telnet(self.obj_myConfigs.Host,
                                           self.obj_myConfigs.Port,
                                           self.obj_myConfigs.TimeOut)
            self.Telnet.write(self.obj_myConfigs.User + self.strSpecialChar)
            #self.Telnet.read_until(self.obj_myConfigs.User+"~$ ",self.TimeOut)
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
            strBufferValue = strBufferValue[strBufferValue.rfind("\n")+len("\n"):]
            print "Who am I["+strBufferValue+"]"
            if strBufferValue == "root~$ ":
                self.intAmIroot = 1
            elif strBufferValue == "unknow~$ ":
                self.intAmIroot = -1
            else:
                self.intAmIroot = 0
        except EOFError,error:
            print MethodName,error
            self.intAmIroot = -2
        
    def TelnetCommunicate(self, command):
        '''
        Ta metoda wysyla informacje do serwera, w przypadku bledu zwraca puste dane
        '''
        MethodName = self.ClassName+"[TelnetCommunicate]->"
        data=""
        try:
            self.Telnet.write(command+ self.strSpecialChar)
            self.UploadedData += len(self.obj_myConfigs.User + self.strSpecialChar)
            data = self.Telnet.read_until(self.obj_myConfigs.User+"~$ ",self.obj_myConfigs.TimeOut)
            self.RecivedData += len(data)
            print "DATA RECIVED:",data,"<]>",self.RecivedData,"bajts""<]"
        except EOFError, error:
            print MethodName+"----------------------"
            print MethodName,error
            print MethodName+"----------------------"
            data=""

        return data
    def SendWithSpecSize(self,data):
        return
    def TelnetDisconnect(self):
        '''
        Rozlaczam polaczenie
        '''
        print self.ClassName+"Connection has been closed"
        strReturn = self.TelnetCommunicate("DisconnectMe")
        print "Recived Data[", self.RecivedData,"/",len(self.Telnet.read_all()),"] bajts"
        print "Uploaded data[", self.UploadedData, "] bajts"
        self.Telnet.close
        return strReturn
    def TotalTransfer(self):
        return self.RecivedData,self.UploadedData
    def NegSet(self, intSize, intTime):
        self.SizeOfPacket = intSize
        self.TimeOut = intTime

class myTelnetParser(myTelnet, XMLFileGenerator):
    def __init__(self, obj_myConfigs,obj_listWidget_Console):
        myTelnet.__init__(self,obj_myConfigs)
        XMLFileGenerator.__init__(self)
        self.obj_myConfigs = obj_myConfigs
        self.obj_listWidget_Console = obj_listWidget_Console
        self.XMLListFromServer = None
        print self.ClassName+"[Pareser]->"
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
        return time.ctime(int(strRet[:strRet.rfind("\n")]))
    def AskForServerInfo(self):
        strReturn = self.TelnetCommunicate("GetServerInfo")
        return "Moze bedzie cos"
    def AskServerForFiles(self, strWords):
        '''
        Wysyla zapytanie do serwera
        Zwraca czy sie udalo
        '''
        retcode = 1
        strReturn = self.TelnetCommunicate("SearchFor: "+strWords)
        if  len(strReturn) >10:
            self.XMLListFromServer = CreateXMLServerFilesList(strReturn)
            retcode = 0
        return retcode
    def GetXMLListFromServer(self):
        '''
        Jezeli z lista jest wszystko w porzadku to ja zwracam
        '''
        if len(self.XMLListFromServer) >1:
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
                print self.ClassName+"List is empty nothing send aborded"
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

