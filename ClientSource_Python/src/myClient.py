#! /usr/bin/env python
# -*- coding: utf-8 -*-
'''
####Author Łukasz Buśko                
####Contact buskol.waw.pl@gmail.com    
####Licenes GNU/General Public License                        
'''

try:
    import sys
    import os
    import time
    import string
    import hashlib, getpass
    from optparse import OptionParser
    from threading import Thread
    #Moje
    from MainFrame import Ui_TabWidget_MainFrame as importMainFrame
    from AdminPanel import Ui_ToolBox_AdminPanel as importAdminPanel
    from PyQt4 import QtGui
except ImportError, error:
    print "[myClient]->", error
    time.sleep(2)
    sys.exit(1)
finally:
    ####VERSION CHECKS
    pass
    #ProgConfigs.CheckRequirements()

def myClient():
    print "[myClient]->"
    app = QtGui.QApplication(sys.argv)
    TabWidget = QtGui.QTabWidget()
    MainFrame = importMainFrame()
    MainFrame.setupUi(TabWidget)
    TabWidget.show()
    return app.exec_()

