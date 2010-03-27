#! /usr/bin/env python
# -*- coding: utf-8 -*-
'''
####Author Łukasz Buśko                
####Contact buskol.waw.pl@gmail.com    
####Licenes GPL                        
'''

import telnetlib
import time, string

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
	#self.SetVaruables(host, port, login, password,timeout)
	self.TelnetConnect()

    def TelnetConnect(self):
        MethodName = self.ClassName+"[TelnetConnect]->"
	strBuffer = ""
	strBufferValue = ""
	try:
            self.Telnet = telnetlib.Telnet(self.obj_myConfigs.Host,self.obj_myConfigs.Port,self.obj_myConfigs.TimeOut)
            self.Telnet.write(self.obj_myConfigs.User + self.strSpecialChar)
            #self.Telnet.read_until(self.obj_myConfigs.User+"~$ ",self.obj_myConfigs.TimeOut)
	    strBuffer = self.Telnet.read_until(self.obj_myConfigs.User+"~$ ",self.obj_myConfigs.TimeOut)#self.Telnet.read_some()
	    
	    strBufferValue = strBuffer[strBuffer.find("XML=")+len("XML="):]
	    self.XMLVersion = float(strBufferValue[:strBufferValue.find(",\n")])

	    strBufferValue = strBuffer[strBuffer.find("Server_Version=")+len("Server_Version="):]
	    self.ServerVersion = strBufferValue[:strBufferValue.find(",\n")]

	    strBufferValue = strBufferValue[:strBuffer.rfind("~$")]
	    strBufferValue = strBufferValue[strBufferValue.rfind("\n"):]
	    if strBufferValue == "root~$":
		self.intAmIroot = 1
	    elif strBufferValue == "unknow~$":
		self.intAmIroot = -1
	    else:
		self.intAmIroot = 0

	except EOFError,error:
	    print MethodName,error
        
    def TelnetCommunicate(self, command):
        MethodName = self.ClassName+"[TelnetCommunicate]->"
        data=""
        try:
            self.Telnet.write(command+ self.strSpecialChar)
            data = self.Telnet.read_until(self.obj_myConfigs.User+"~$ ",self.obj_myConfigs.TimeOut)
        except EOFError, error:
            print MethodName,error
            data=""
        
	print "DATA RECIVED:",data,"<]>"
        return data
            
    def TelnetDisconnect(self):
	print self.ClassName+"Connection has been closed"
	strReturn = self.TelnetCommunicate("DisconnectMe")
	self.Telnet.close
	return strReturn

class myTelnetParser(myTelnet):
    def __init__(self, obj_myConfigs,obj_listWidget_Console):
	myTelnet.__init__(self,obj_myConfigs)
	self.obj_myConfigs = obj_myConfigs
	self.obj_listWidget_Console = obj_listWidget_Console
    def AmIRoot(self):
	return self.intAmIroot
    def AskForServerTime(self):
	strRet = self.TelnetCommunicate("GetServerTime")
	return time.ctime(int(strRet[:strRet.rfind("\n")]))
    def AskForServerInfo(self):
	strReturn = self.TelnetCommunicate("GetServerInfo")
	return "Moze bedzie cos"
    def AskServerForFiles(self):
	return "lista plikow"
    def RestartServer(self):
	strReturn = self.TelnetCommunicate("Restart")
	return "komunikat restart"
    def ShutdownServer(self):
	strReturn = self.TelnetCommunicate("Shutdown")
	return "komunikat shutdown"
    def ShutdownServerForced(self):
	strReturn = self.TelnetCommunicate("ShutdownForced")
	return "komunikat shutdown forced"

