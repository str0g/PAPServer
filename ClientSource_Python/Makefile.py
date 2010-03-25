#! /usr/bin/env python
# -*- coding: utf-8 -*-
'''
####Author Łukasz Buśko                
####Contact buskol.waw.pl@gmail.com    
####Licenes GPL                        
'''
#### Imports
try:
    import sys,time,string,os
    import compileall
except ImportError,error:
    print "Makefile",str(error)
    time.sleep(2)
    sys.exit(1)
    
####STATIC VARUBALE
myPyVersion="2.6.4"
#print os.getcwd()
####VERSION CHECKS
if sys.version[:string.find(sys.version," ")] < myPyVersion:
    print "Too old Python!\nRequired atleast Python",myPyVersion
    time.sleep(2)
    sys.exit(1)
try:
    compileall.compile_dir(os.getcwd()+"/src", force=1)
except:
    print "Compilation Faild!"
    time.sleep(2)
    sys.exit(1)
