#! /usr/bin/env python
# -*- coding: utf-8 -*-
'''
####Author Łukasz Buśko                
####Contact buskol.waw.pl@gmail.com    
####Licenes GPL                        
'''

try:
    import time, sys, os
except ImportError, error:
    print "[MsgLib]->", error
    time.sleep(2)
    sys.exit(1)

class MsgDeffinition():
    ClassName = None
    def __init__(self):
        self.ClassName = "[MsgDeffinition]->"
    def Quit(self, stringMsg, intTimeOut, intError):
        print stringMsg
        time.sleep(intTimeOut)
        sys.exit(intError)
    def MsgWithDelay(self, stringMsg, intTimeOut):
        print stringMsg
        time.sleep(intTimeOut)
